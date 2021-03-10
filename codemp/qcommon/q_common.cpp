/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2005 - 2015, ioquake3 contributors
Copyright (C) 2013 - 2019, OpenJK contributors
Copyright (C) 2019 - 2020, CleanJoKe contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

// common.c -- misc functions used in client and server

#include "qcommon/cm_public.hpp"
#include "qcommon/com_cvar.hpp"
#include "qcommon/com_cvars.hpp"
#include "qcommon/com_inputField.hpp"
#include "qcommon/game_version.hpp"
#include "qcommon/huffman.hpp"
#include "qcommon/stringed_ingame.hpp"
#include "sys/sys_local.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

FILE *debuglogfile;
fileHandle_t com_logfile;
fileHandle_t	com_journalFile;			// events are written here
fileHandle_t	com_journalDataFile;		// config files are written here

// com_speeds times
int		time_game;
int		time_frontend;		// renderer frontend time
int		time_backend;		// renderer backend time

int			com_frameTime;
int			com_frameNumber;

bool	com_errorEntered = false;
bool	com_fullyInitialized = false;

char	com_errorMessage[MAXPRINTMSG] = {0};

static char	*rd_buffer;
static int	rd_buffersize;
static void	(*rd_flush)( char *buffer );

void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *) )
{
	if (!buffer || !buffersize || !flush)
		return;
	rd_buffer = buffer;
	rd_buffersize = buffersize;
	rd_flush = flush;

	*rd_buffer = 0;
}

void Com_EndRedirect (void)
{
	if ( rd_flush ) {
		rd_flush(rd_buffer);
	}

	rd_buffer = nullptr;
	rd_buffersize = 0;
	rd_flush = nullptr;
}

// Both client and server can use this, and it will output to the appropriate place.
// A raw string should NEVER be passed as fmt, because of "%f" type crashers.
void QDECL Com_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	static bool opening_qconsole = false;

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	if ( rd_buffer ) {
		if ((strlen (msg) + strlen(rd_buffer)) > (size_t)(rd_buffersize - 1)) {
			rd_flush(rd_buffer);
			*rd_buffer = 0;
		}
		Q_strcat(rd_buffer, rd_buffersize, msg);
    // TTimo nooo .. that would defeat the purpose
		//rd_flush(rd_buffer);
		//*rd_buffer = 0;
		return;
	}

#ifndef BUILD_DEDICATED
	CL_ConsolePrint( msg );
#endif

	// echo to dedicated console and early console
	Sys_Print( msg );

	// logfile
	if ( logfile && logfile->integer ) {
    // TTimo: only open the qconsole.log if the filesystem is in an initialized state
    //   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
		if ( !com_logfile && FS_Initialized() && !opening_qconsole ) {
			struct tm *newtime;
			time_t aclock;

			opening_qconsole = true;

			time( &aclock );
			newtime = localtime( &aclock );

			com_logfile = FS_FOpenFileWrite( "qconsole.log" );

			if ( com_logfile ) {
				Com_Printf( "logfile opened on %s\n", asctime( newtime ) );
				if ( logfile->integer > 1 ) {
					// force it to not buffer so we get valid
					// data even if we are crashing
					FS_ForceFlush(com_logfile);
				}
			}
			else {
				Com_Printf( "Opening qconsole.log failed!\n" );
				Cvar_SetValue( "logfile", 0 );
			}
		}
		opening_qconsole = false;
		if ( com_logfile && FS_Initialized()) {
			FS_Write(msg, strlen(msg), com_logfile);
		}
	}

#if defined(_WIN32) && defined(_DEBUG)
	if ( *msg )
	{
		OutputDebugString ( Q_CleanStr(msg) );
		OutputDebugString ("\n");
	}
#endif
}

// A Com_Printf that only shows up if the "developer" cvar is set
void QDECL Com_DPrintf( const char *fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	if ( !developer || !developer->integer ) {
		return;			// don't confuse non-developers with techie stuff...
	}

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	Com_Printf ("%s", msg);
}

// Outputs to the VC / Windows Debug window (only in debug compile)
void QDECL Com_OPrintf( const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);
#ifdef _WIN32
	OutputDebugString(msg);
#else
	printf("%s", msg);
#endif
}

// Both client and server can use this, and it will do the appropriate things.
void NORETURN QDECL Com_Error( int code, const char *fmt, ... ) {
	va_list		argptr;
	static int	lastErrorTime;
	static int	errorCount;
	int			currentTime;

	if ( com_errorEntered ) {
		Sys_Error( "recursive error after: %s", com_errorMessage );
	}
	com_errorEntered = true;

	// when we are running automated scripts, make sure we
	// know if anything failed
	if ( com_buildScript && com_buildScript->integer ) {
		code = ERR_FATAL;
	}

	// ERR_DROPs on dedicated drop to an interactive console
	// which doesn't make sense for dedicated as it's generally
	// run unattended
	if ( dedicated && dedicated->integer ) {
		code = ERR_FATAL;
	}

	// if we are getting a solid stream of ERR_DROP, do an ERR_FATAL
	currentTime = Sys_Milliseconds();
	if ( currentTime - lastErrorTime < 100 ) {
		if ( ++errorCount > 3 ) {
			code = ERR_FATAL;
		}
	} else {
		errorCount = 0;
	}
	lastErrorTime = currentTime;

	va_start (argptr,fmt);
	Q_vsnprintf (com_errorMessage,sizeof(com_errorMessage), fmt,argptr);
	va_end (argptr);

	if ( code != ERR_DISCONNECT && code != ERR_NEED_CD ) {
		Cvar_Get("com_errorMessage", "", CVAR_ROM);	//give com_errorMessage a default so it won't come back to life after a resetDefaults
		Cvar_Set("com_errorMessage", com_errorMessage);
	}

	if ( code == ERR_DISCONNECT || code == ERR_SERVERDISCONNECT || code == ERR_DROP || code == ERR_NEED_CD ) {
		throw code;
	} else {
		CL_Shutdown ();
		SV_Shutdown (va("Server fatal crashed: %s\n", com_errorMessage));
	}

	Com_Shutdown ();

	Sys_Error ("%s", com_errorMessage);
}

// Both client and server can use this, and it will do the appropriate things.
void Com_Quit_f( void ) {
	// don't try to shutdown if we are in a recursive error
	if ( !com_errorEntered ) {
		SV_Shutdown ("Server quit\n");
		CL_Shutdown ();
		Com_Shutdown ();
		FS_Shutdown(true);
	}
	Sys_Quit ();
}

// COMMAND LINE FUNCTIONS
// + characters separate the commandLine string into multiple console command lines.
// All of these are valid:
//	quake3 +set test blah +map test
//	quake3 set test blah+map test
//	quake3 set test blah + map test

#define	MAX_CONSOLE_LINES	32
int		com_numConsoleLines;
char	*com_consoleLines[MAX_CONSOLE_LINES];

// Break it up into multiple console lines
void Com_ParseCommandLine( char *commandLine ) {
	int inq = 0;
	com_consoleLines[0] = commandLine;
	com_numConsoleLines = 1;

	while ( *commandLine ) {
		if (*commandLine == '"') {
			inq = !inq;
		}
		// look for a + seperating character
		// if commandLine came from a file, we might have real line seperators
		if ( (*commandLine == '+' && !inq) || *commandLine == '\n'  || *commandLine == '\r' ) {
			if ( com_numConsoleLines == MAX_CONSOLE_LINES ) {
				return;
			}
			com_consoleLines[com_numConsoleLines] = commandLine + 1;
			com_numConsoleLines++;
			*commandLine = 0;
		}
		commandLine++;
	}
}

// Check for "safe" on the command line, which will skip loading of jampconfig.cfg
bool Com_SafeMode( void ) {
	int		i;

	for ( i = 0 ; i < com_numConsoleLines ; i++ ) {
		Cmd_TokenizeString( com_consoleLines[i] );
		if ( !Q_stricmp( Cmd_Argv(0), "safe" )
			|| !Q_stricmp( Cmd_Argv(0), "cvar_restart" ) ) {
			com_consoleLines[i][0] = 0;
			return true;
		}
	}
	return false;
}

// Searches for command line parameters that are set commands.
// If match is not nullptr, only that cvar will be looked for.
// That is necessary because cddir and basedir need to be set before the filesystem is started, but all other sets
//	should be after execing the config and default.
//
// checks for and removes command line "+set var arg" constructs
// if match is nullptr, all set commands will be executed, otherwise
// only a set with the exact name.  Only used during startup.
void Com_StartupVariable( const char *match ) {
	for (int i=0 ; i < com_numConsoleLines ; i++) {
		Cmd_TokenizeString( com_consoleLines[i] );
		if ( strcmp( Cmd_Argv(0), "set" ) ) {
			continue;
		}

		char *s = Cmd_Argv(1);

		if ( !match || !strcmp( s, match ) )
			Cvar_User_Set( s, Cmd_ArgsFrom( 2 ) );
	}
}

// Adds command line parameters as script statements
// Commands are seperated by + signs
// Returns true if any late commands were added, which will keep the demoloop from immediately starting
bool Com_AddStartupCommands( void ) {
	int		i;
	bool	added;

	added = false;
	// quote every token, so args with semicolons can work
	for (i=0 ; i < com_numConsoleLines ; i++) {
		if ( !com_consoleLines[i] || !com_consoleLines[i][0] ) {
			continue;
		}

		// set commands won't override menu startup
		if ( Q_stricmpn( com_consoleLines[i], "set", 3 ) ) {
			added = true;
		}
		Cbuf_AddText( com_consoleLines[i] );
		Cbuf_AddText( "\n" );
	}

	return added;
}

void Info_Print( const char *s ) {
	char	key[BIG_INFO_KEY];
	char	value[BIG_INFO_VALUE];
	char	*o;
	int		l;

	if (*s == '\\')
		s++;
	while (*s)
	{
		o = key;
		while (*s && *s != '\\')
			*o++ = *s++;

		l = o - key;
		if (l < 20)
		{
			Com_Memset (o, ' ', 20-l);
			key[20] = 0;
		}
		else
			*o = 0;
		Com_Printf ("%s ", key);

		if (!*s)
		{
			Com_Printf ("MISSING VALUE\n");
			return;
		}

		o = value;
		s++;
		while (*s && *s != '\\')
			*o++ = *s++;
		*o = 0;

		if (*s)
			s++;
		Com_Printf ("%s\n", value);
	}
}

char *Com_StringContains(char *str1, char *str2, int casesensitive) {
	int len, i, j;

	len = strlen(str1) - strlen(str2);
	for (i = 0; i <= len; i++, str1++) {
		for (j = 0; str2[j]; j++) {
			if (casesensitive) {
				if (str1[j] != str2[j]) {
					break;
				}
			}
			else {
				if (toupper(str1[j]) != toupper(str2[j])) {
					break;
				}
			}
		}
		if (!str2[j]) {
			return str1;
		}
	}
	return nullptr;
}

int Com_Filter(char *filter, char *name, int casesensitive)
{
	char buf[MAX_TOKEN_CHARS];
	char *ptr;
	int i, found;

	while(*filter) {
		if (*filter == '*') {
			filter++;
			for (i = 0; *filter; i++) {
				if (*filter == '*' || *filter == '?') break;
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if (strlen(buf)) {
				ptr = Com_StringContains(name, buf, casesensitive);
				if (!ptr) return false;
				name = ptr + strlen(buf);
			}
		}
		else if (*filter == '?') {
			filter++;
			name++;
		}
		else if (*filter == '[' && *(filter+1) == '[') {
			filter++;
		}
		else if (*filter == '[') {
			filter++;
			found = false;
			while(*filter && !found) {
				if (*filter == ']' && *(filter+1) != ']') break;
				if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']')) {
					if (casesensitive) {
						if (*name >= *filter && *name <= *(filter+2)) found = true;
					}
					else {
						if (toupper(*name) >= toupper(*filter) &&
							toupper(*name) <= toupper(*(filter+2))) found = true;
					}
					filter += 3;
				}
				else {
					if (casesensitive) {
						if (*filter == *name) found = true;
					}
					else {
						if (toupper(*filter) == toupper(*name)) found = true;
					}
					filter++;
				}
			}
			if (!found) return false;
			while(*filter) {
				if (*filter == ']' && *(filter+1) != ']') break;
				filter++;
			}
			filter++;
			name++;
		}
		else {
			if (casesensitive) {
				if (*filter != *name) return false;
			}
			else {
				if (toupper(*filter) != toupper(*name)) return false;
			}
			filter++;
			name++;
		}
	}
	return true;
}

int Com_FilterPath(char *filter, char *name, int casesensitive)
{
	int i;
	char new_filter[MAX_QPATH];
	char new_name[MAX_QPATH];

	for (i = 0; i < MAX_QPATH-1 && filter[i]; i++) {
		if ( filter[i] == '\\' || filter[i] == ':' ) {
			new_filter[i] = '/';
		}
		else {
			new_filter[i] = filter[i];
		}
	}
	new_filter[i] = '\0';
	for (i = 0; i < MAX_QPATH-1 && name[i]; i++) {
		if ( name[i] == '\\' || name[i] == ':' ) {
			new_name[i] = '/';
		}
		else {
			new_name[i] = name[i];
		}
	}
	new_name[i] = '\0';
	return Com_Filter(new_filter, new_name, casesensitive);
}

int Com_HashKey(char *string, int maxlen) {
	int hash, i;

	hash = 0;
	for (i = 0; i < maxlen && string[i] != '\0'; i++) {
		hash += string[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20));
	return hash;
}

int Com_RealTime(qtime_t *qtime) {
	time_t t;
	struct tm *tms;

	t = time(nullptr);
	if (!qtime)
		return t;
	tms = localtime(&t);
	if (tms) {
		qtime->tm_sec = tms->tm_sec;
		qtime->tm_min = tms->tm_min;
		qtime->tm_hour = tms->tm_hour;
		qtime->tm_mday = tms->tm_mday;
		qtime->tm_mon = tms->tm_mon;
		qtime->tm_year = tms->tm_year;
		qtime->tm_wday = tms->tm_wday;
		qtime->tm_yday = tms->tm_yday;
		qtime->tm_isdst = tms->tm_isdst;
	}
	return t;
}

// EVENTS AND JOURNALING
// In addition to these events, .cfg files are also copied to the journaled file

#define	MAX_PUSHED_EVENTS	            1024
static int		com_pushedEventsHead = 0;
static int             com_pushedEventsTail = 0;
static sysEvent_t	com_pushedEvents[MAX_PUSHED_EVENTS];

void Com_InitJournaling( void ) {
	Com_StartupVariable( "journal" );
	if ( !com_journal->integer ) {
		return;
	}

	if ( com_journal->integer == 1 ) {
		Com_Printf( "Journaling events\n");
		com_journalFile = FS_FOpenFileWrite( "journal.dat" );
		com_journalDataFile = FS_FOpenFileWrite( "journaldata.dat" );
	} else if ( com_journal->integer == 2 ) {
		Com_Printf( "Replaying journaled events\n");
		FS_FOpenFileRead( "journal.dat", &com_journalFile, true );
		FS_FOpenFileRead( "journaldata.dat", &com_journalDataFile, true );
	}

	if ( !com_journalFile || !com_journalDataFile ) {
		Cvar_Set( "com_journal", "0" );
		com_journalFile = 0;
		com_journalDataFile = 0;
		Com_Printf( "Couldn't open journal files\n" );
	}
}

sysEvent_t	Com_GetRealEvent( void ) {
	int			r;
	sysEvent_t	ev;

	// either get an event from the system or the journal file
	if ( com_journal->integer == 2 ) {
		r = FS_Read( &ev, sizeof(ev), com_journalFile );
		if ( r != sizeof(ev) ) {
			Com_Error( ERR_FATAL, "Error reading from journal file" );
		}
		if ( ev.evPtrLength ) {
			ev.evPtr = Z_Malloc( ev.evPtrLength, TAG_EVENT, true );
			r = FS_Read( ev.evPtr, ev.evPtrLength, com_journalFile );
			if ( r != ev.evPtrLength ) {
				Com_Error( ERR_FATAL, "Error reading from journal file" );
			}
		}
	} else {
		ev = Sys_GetEvent();

		// write the journal value out if needed
		if ( com_journal->integer == 1 ) {
			r = FS_Write( &ev, sizeof(ev), com_journalFile );
			if ( r != sizeof(ev) ) {
				Com_Error( ERR_FATAL, "Error writing to journal file" );
			}
			if ( ev.evPtrLength ) {
				r = FS_Write( ev.evPtr, ev.evPtrLength, com_journalFile );
				if ( r != ev.evPtrLength ) {
					Com_Error( ERR_FATAL, "Error writing to journal file" );
				}
			}
		}
	}

	return ev;
}

void Com_InitPushEvent( void ) {
	// clear the static buffer array
	// this requires SE_NONE to be accepted as a valid but NOP event
	memset( com_pushedEvents, 0, sizeof(com_pushedEvents) );
	// reset counters while we are at it
	// beware: GetEvent might still return an SE_NONE from the buffer
	com_pushedEventsHead = 0;
	com_pushedEventsTail = 0;
}

void Com_PushEvent( sysEvent_t *event ) {
	sysEvent_t		*ev;
	static int printedWarning = 0;

	ev = &com_pushedEvents[ com_pushedEventsHead & (MAX_PUSHED_EVENTS-1) ];

	if ( com_pushedEventsHead - com_pushedEventsTail >= MAX_PUSHED_EVENTS ) {

		// don't print the warning constantly, or it can give time for more...
		if ( !printedWarning ) {
			printedWarning = true;
			Com_Printf( "WARNING: Com_PushEvent overflow\n" );
		}

		if ( ev->evPtr ) {
			Z_Free( ev->evPtr );
		}
		com_pushedEventsTail++;
	} else {
		printedWarning = false;
	}

	*ev = *event;
	com_pushedEventsHead++;
}

sysEvent_t	Com_GetEvent( void ) {
	if ( com_pushedEventsHead > com_pushedEventsTail ) {
		com_pushedEventsTail++;
		return com_pushedEvents[ (com_pushedEventsTail-1) & (MAX_PUSHED_EVENTS-1) ];
	}
	return Com_GetRealEvent();
}

void Com_RunAndTimeServerPacket( netadr_t *evFrom, msg_t *buf ) {
	int		t1, t2, msec;

	t1 = 0;

	if ( com_speeds->integer ) {
		t1 = Sys_Milliseconds ();
	}

	SV_PacketEvent( *evFrom, buf );

	if ( com_speeds->integer ) {
		t2 = Sys_Milliseconds ();
		msec = t2 - t1;
		if ( com_speeds->integer == 3 ) {
			Com_Printf( "SV_PacketEvent time: %i\n", msec );
		}
	}
}

// Returns last event time
int Com_EventLoop( void ) {
	sysEvent_t	ev;
	netadr_t	evFrom;
	byte		bufData[MAX_MSGLEN];
	msg_t		buf;

	MSG_Init( &buf, bufData, sizeof( bufData ) );

	while ( 1 ) {
		ev = Com_GetEvent();

		// if no more events are available
		if ( ev.evType == SE_NONE ) {
			// manually send packet events for the loopback channel
			while ( NET_GetLoopPacket( NS_CLIENT, &evFrom, &buf ) ) {
				CL_PacketEvent( evFrom, &buf );
			}

			while ( NET_GetLoopPacket( NS_SERVER, &evFrom, &buf ) ) {
				// if the server just shut down, flush the events
				if ( sv_running->integer ) {
					Com_RunAndTimeServerPacket( &evFrom, &buf );
				}
			}

			return ev.evTime;
		}

		switch ( ev.evType ) {
		default:
			Com_Error( ERR_FATAL, "Com_EventLoop: bad event type %i", ev.evType );
			break;
        case SE_NONE:
            break;
		case SE_KEY:
			CL_KeyEvent( ev.evValue, (bool)ev.evValue2, ev.evTime );
			break;
		case SE_CHAR:
			CL_CharEvent( ev.evValue );
			break;
		case SE_MOUSE:
			CL_MouseEvent( ev.evValue, ev.evValue2, ev.evTime );
			break;
		case SE_JOYSTICK_AXIS:
			CL_JoystickEvent( ev.evValue, ev.evValue2, ev.evTime );
			break;
		case SE_CONSOLE:
			if ( ((char *)ev.evPtr)[0] == '\\' || ((char *)ev.evPtr)[0] == '/' )
			{
				Cbuf_AddText( (char *)ev.evPtr+1 );
			}
			else
			{
				Cbuf_AddText( (char *)ev.evPtr );
			}
			Cbuf_AddText( "\n" );
			break;
		}

		// free any block data
		if ( ev.evPtr ) {
			Z_Free( ev.evPtr );
		}
	}

	return 0;	// never reached
}

// Can be used for profiling, but will be journaled accurately
int Com_Milliseconds (void) {
	sysEvent_t	ev;

	// get events and push them until we get a null event with the current time
	do {

		ev = Com_GetRealEvent();
		if ( ev.evType != SE_NONE ) {
			Com_PushEvent( &ev );
		}
	} while ( ev.evType != SE_NONE );

	return ev.evTime;
}

// Just throw a fatal error to test error shutdown procedures
static void NORETURN Com_Error_f (void) {
	if ( Cmd_Argc() > 1 ) {
		Com_Error( ERR_DROP, "Testing drop error" );
	} else {
		Com_Error( ERR_FATAL, "Testing fatal error" );
	}
}

// Just freeze in place for a given number of seconds to test error recovery
static void Com_Freeze_f (void) {
	float	s;
	int		start, now;

	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "freeze <seconds>\n" );
		return;
	}
	s = atof( Cmd_Argv(1) );

	start = Com_Milliseconds();

	while ( 1 ) {
		now = Com_Milliseconds();
		if ( ( now - start ) * 0.001 > s ) {
			break;
		}
	}
}

// A way to force a bus error for development reasons
static void NORETURN Com_Crash_f( void ) {
	* ( volatile int * ) 0 = 0x12345678;
	/* that should crash already, but to reassure the compiler: */
	abort();
}

void Com_ExecuteCfg( void ) {
	Cbuf_ExecuteText(EXEC_NOW, "exec " DEFAULT_CFG "\n");
	Cbuf_Execute(); // Always execute after exec to prevent text buffer overflowing

	Cbuf_ExecuteText(EXEC_NOW, "exec " Q3CONFIG_CFG "\n");
	Cbuf_Execute();
}

// Seed the random number generator, if possible with an OS supplied random seed.
static void Com_InitRand(void)
{
	unsigned int seed;

	if(Sys_RandomBytes((byte *) &seed, sizeof(seed)))
		srand(seed);
	else
		srand(time(nullptr));
}

// Error string for the given error code (from Com_Error).
static const char *Com_ErrorString ( int code )
{
	switch ( code )
	{
		case ERR_DISCONNECT:
		// fallthrough
		case ERR_SERVERDISCONNECT:
			return "DISCONNECTED";

		case ERR_DROP:
			return "DROPPED";

		case ERR_NEED_CD:
			return "NEED CD";

		default:
			return "UNKNOWN";
	}
}

// Handles freeing up of resources when Com_Error is called.
static void Com_CatchError ( int code )
{
	if ( code == ERR_DISCONNECT || code == ERR_SERVERDISCONNECT ) {
		SV_Shutdown( "Server disconnected" );
		CL_Disconnect( true );
		CL_FlushMemory(  );
		// make sure we can get at our local stuff
		FS_PureServerSetLoadedPaks( "", "" );
		com_errorEntered = false;
	} else if ( code == ERR_DROP ) {
		Com_Printf ("********************\n"
					"ERROR: %s\n"
					"********************\n", com_errorMessage);
		SV_Shutdown (va("Server crashed: %s\n",  com_errorMessage));
		CL_Disconnect( true );
		CL_FlushMemory( );
		// make sure we can get at our local stuff
		FS_PureServerSetLoadedPaks( "", "" );
		com_errorEntered = false;
	} else if ( code == ERR_NEED_CD ) {
		SV_Shutdown( "Server didn't have CD" );
		if ( cl_running && cl_running->integer ) {
			CL_Disconnect( true );
			CL_FlushMemory( );
		} else {
			Com_Printf("Server didn't have CD\n" );
		}
		// make sure we can get at our local stuff
		FS_PureServerSetLoadedPaks( "", "" );
		com_errorEntered = false;
	}
}

void Com_WriteConfigToFile( const char *filename ) {
	fileHandle_t	f;

	f = FS_FOpenFileWrite( filename );
	if ( !f ) {
		Com_Printf ("Couldn't write %s.\n", filename );
		return;
	}

	FS_Printf (f, "// generated by OpenJK MP, do not modify\n");
	Key_WriteBindings (f);
	Cvar_WriteVariables (f);
	FS_FCloseFile( f );
}

// Write the config file to a specific name
static void Com_WriteConfig_f( void ) {
	char	filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "Usage: writeconfig <filename>\n" );
		return;
	}

	Q_strncpyz( filename, Cmd_Argv(1), sizeof( filename ) );
	COM_DefaultExtension( filename, sizeof( filename ), ".cfg" );

	if(!COM_CompareExtension(filename, ".cfg"))
	{
		Com_Printf( "Com_WriteConfig_f: Only the \".cfg\" extension is supported by this command!\n" );
		return;
	}

	if(!FS_FilenameCompare(filename, DEFAULT_CFG))
	{
		Com_Printf( S_COLOR_YELLOW "Com_WriteConfig_f: The filename \"%s\" is reserved! Please choose another name.\n", filename );
		return;
	}

	Com_Printf( "Writing %s.\n", filename );
	Com_WriteConfigToFile( filename );
}

// commandLine should not include the executable name (argv[0])
void Com_Init( char *commandLine ) {
	int		qport;

	Com_Printf( "%s %s %s\n", JK_VERSION, PLATFORM_STRING, SOURCE_DATE );

	try
	{
		// initialize the weak pseudo-random number generator for use later.
		Com_InitRand();

		// do this before anything else decides to push events
		Com_InitPushEvent();

		Com_InitZoneMemory();
		Cvar_Init ();

		// prepare enough of the subsystems to handle
		// cvar and command buffer management
		Com_ParseCommandLine( commandLine );

	//	Swap_Init ();
		Cbuf_Init ();

		// override anything from the config files with command line args
		Com_StartupVariable( nullptr );

		Com_InitZoneMemoryVars();
		Cmd_Init ();

		// Seed the random number generator
		Rand_Init(Sys_Milliseconds(true));

		// done early so bind command exists
		CL_InitKeyCommands();

		// init commands and vars
		Com_InitCvars();

		FS_InitFilesystem ();

		Com_InitJournaling();

		// Add some commands here already so users can use them from config files
		if ( developer && developer->integer ) {
			Cmd_AddCommand ("error", Com_Error_f);
			Cmd_AddCommand ("crash", Com_Crash_f );
			Cmd_AddCommand ("freeze", Com_Freeze_f);
		}
		Cmd_AddCommand ("quit", Com_Quit_f, "Quits the game" );
#ifndef FINAL_BUILD
		Cmd_AddCommand ("changeVectors", MSG_ReportChangeVectors_f );
#endif
		Cmd_AddCommand ("writeconfig", Com_WriteConfig_f, "Write the configuration to file" );

		Com_ExecuteCfg();

		// override anything from the config files with command line args
		Com_StartupVariable( nullptr );

		// allocate the stack based hunk allocator
		Com_InitHunkMemory();

		// if any archived cvars are modified after this, we will trigger a writing
		// of the config file
		cvar_modifiedFlags &= ~CVAR_ARCHIVE;

		SE_Init();

		Sys_Init();

		Sys_SetProcessorAffinity();

		// Pick a random port value
		Com_RandomBytes( (byte*)&qport, sizeof(int) );
		Netchan_Init( qport );	// pick a port value that should be nice and random

		VM_Init();
		SV_Init();

		dedicated->modified = false;
		if ( !dedicated->integer ) {
			CL_Init();
		}

		// set com_frameTime so that if a map is started on the
		// command line it will still be able to count on com_frameTime
		// being random enough for a serverid
		com_frameTime = Com_Milliseconds();

		// add + commands from command line
		Com_AddStartupCommands();

		// start in full screen ui mode
		Cvar_Set("r_uiFullScreen", "1");

		CL_StartHunkUsers();

		// make sure single player is off by default

		com_fullyInitialized = true;
		Com_Printf ("--- Common Initialization Complete ---\n");
	}
	catch ( int code )
	{
		Com_CatchError (code);
		Sys_Error ("Error during initialization: %s", Com_ErrorString (code));
	}
}

// Writes key bindings and archived cvars to config file if modified
void Com_WriteConfiguration( void ) {
	// if we are quiting without fully initializing, make sure
	// we don't write out anything
	if ( !com_fullyInitialized ) {
		return;
	}

	if ( !(cvar_modifiedFlags & CVAR_ARCHIVE ) ) {
		return;
	}
	cvar_modifiedFlags &= ~CVAR_ARCHIVE;

	Com_WriteConfigToFile( Q3CONFIG_CFG );
}

int Com_ModifyMsec( int msec ) {
	int		clampTime;

	// modify time for debugging values

	if ( fixedtime->integer ) {
		msec = fixedtime->integer;
	} else if ( timescale->value ) {
		msec *= timescale->value;
	} else if (com_cameraMode->integer) {
		msec *= timescale->value;
	}

	// don't let it scale below 1 msec
	if ( msec < 1 && timescale->value) {
		msec = 1;
	}

	if ( dedicated->integer ) {
		// dedicated servers don't want to clamp for a much longer
		// period, because it would mess up all the client's views
		// of time.
		if ( sv_running->integer && msec > 500 ) {
			Com_Printf( "Hitch warning: %i msec frame time\n", msec );
		}
		clampTime = 5000;
	} else
	if ( !sv_running->integer ) {
		// clients of remote servers do not want to clamp time, because
		// it would skew their view of the server's time temporarily
		clampTime = 5000;
	} else {
		// for local single player gaming
		// we may want to clamp the time to prevent players from
		// flying off edges when something hitches.
		clampTime = 200;
	}

	if ( msec > clampTime ) {
		msec = clampTime;
	}

	return msec;
}

#ifdef G2_PERFORMANCE_ANALYSIS
#include "qcommon/timing.hpp"
extern timing_c G2PerformanceTimer_PreciseFrame;
extern int G2Time_PreciseFrame;
#endif

int Com_TimeVal(int minMsec)
{
	int timeVal;

	timeVal = Sys_Milliseconds() - com_frameTime;

	if(timeVal >= minMsec)
		timeVal = 0;
	else
		timeVal = minMsec - timeVal;

	return timeVal;
}

void Com_Frame( void ) {

	try
	{
#ifdef G2_PERFORMANCE_ANALYSIS
		G2PerformanceTimer_PreciseFrame.Start();
#endif
		int		msec, minMsec;
		int		timeVal;
		static int	lastTime = 0, bias = 0;

		int		timeBeforeFirstEvents = 0;
		int           timeBeforeServer = 0;
		int           timeBeforeEvents = 0;
		int           timeBeforeClient = 0;
		int           timeAfter = 0;

		// write config file if anything changed
		Com_WriteConfiguration();

		// main event loop

		if ( com_speeds->integer ) {
			timeBeforeFirstEvents = Sys_Milliseconds ();
		}

		// Figure out how much time we have
		if(!timedemo->integer)
		{
			if(dedicated->integer)
				minMsec = SV_FrameMsec();
			else
			{
				if(com_minimized->integer && com_maxfpsMinimized->integer > 0)
					minMsec = 1000 / com_maxfpsMinimized->integer;
				else if(com_unfocused->integer && com_maxfpsUnfocused->integer > 0)
					minMsec = 1000 / com_maxfpsUnfocused->integer;
				else if(com_maxfps->integer > 0)
					minMsec = 1000 / com_maxfps->integer;
				else
					minMsec = 1;

				timeVal = com_frameTime - lastTime;
				bias += timeVal - minMsec;

				if (bias > minMsec)
					bias = minMsec;

				// Adjust minMsec if previous frame took too long to render so
				// that framerate is stable at the requested value.
				minMsec -= bias;
			}
		}
		else
			minMsec = 1;

		timeVal = Com_TimeVal(minMsec);
		do {
			// Busy sleep the last millisecond for better timeout precision
			if(com_busyWait->integer || timeVal < 1)
				NET_Sleep(0);
			else
				NET_Sleep(timeVal - 1);
		} while( (timeVal = Com_TimeVal(minMsec)) != 0 );
		IN_Frame();

		lastTime = com_frameTime;
		com_frameTime = Com_EventLoop();

		msec = com_frameTime - lastTime;

		Cbuf_Execute ();

		// mess with msec if needed
		msec = Com_ModifyMsec( msec );

		// server side

		if ( com_speeds->integer ) {
			timeBeforeServer = Sys_Milliseconds ();
		}

		SV_Frame( msec );

		// if "dedicated" has been modified, start up
		// or shut down the client system.
		// Do this after the server may have started,
		// but before the client tries to auto-connect
		if ( dedicated->modified ) {
			// get the latched value
			Cvar_Get( "dedicated", "0", 0 );
			dedicated->modified = false;
			if ( !dedicated->integer ) {
				CL_Init();
				CL_StartHunkUsers();	//fire up the UI!
			} else {
				CL_Shutdown();
			}
		}

		// client system

		if ( !dedicated->integer ) {

			// run event loop a second time to get server to client packets
			// without a frame of latency

			if ( com_speeds->integer ) {
				timeBeforeEvents = Sys_Milliseconds ();
			}
			Com_EventLoop();
			Cbuf_Execute ();

			// client side

			if ( com_speeds->integer ) {
				timeBeforeClient = Sys_Milliseconds ();
			}

			CL_Frame( msec );

			if ( com_speeds->integer ) {
				timeAfter = Sys_Milliseconds ();
			}
		}
		else
		{
			if ( com_speeds->integer )
			{
				timeBeforeEvents = timeBeforeClient = timeAfter = Sys_Milliseconds ();
			}
		}

		// report timing information

		if ( com_speeds->integer ) {
			int			all, sv, ev, cl;

			all = timeAfter - timeBeforeServer;
			sv = timeBeforeEvents - timeBeforeServer;
			ev = timeBeforeServer - timeBeforeFirstEvents + timeBeforeClient - timeBeforeEvents;
			cl = timeAfter - timeBeforeClient;
			sv -= time_game;
			cl -= time_frontend + time_backend;

			Com_Printf ("frame:%i all:%3i sv:%3i ev:%3i cl:%3i gm:%3i rf:%3i bk:%3i\n",
						 com_frameNumber, all, sv, ev, cl, time_game, time_frontend, time_backend );
		}

		// trace optimization tracking

		if ( com_showtrace->integer ) {

			extern	int c_traces, c_brush_traces, c_patch_traces;
			extern	int	c_pointcontents;

			Com_Printf ("%4i traces  (%ib %ip) %4i points\n", c_traces,
				c_brush_traces, c_patch_traces, c_pointcontents);
			c_traces = 0;
			c_brush_traces = 0;
			c_patch_traces = 0;
			c_pointcontents = 0;
		}

		if ( com_affinity->modified )
		{
			com_affinity->modified = false;
			Sys_SetProcessorAffinity();
		}

		com_frameNumber++;
	}
	catch (int code) {
		Com_CatchError (code);
		Com_Printf ("%s\n", Com_ErrorString (code));
		return;
	}

#ifdef G2_PERFORMANCE_ANALYSIS
	G2Time_PreciseFrame += G2PerformanceTimer_PreciseFrame.End();

	if (com_G2Report && com_G2Report->integer)
	{
		G2Time_ReportTimers();
	}

	G2Time_ResetTimers();
#endif
}

void Com_Shutdown (void)
{
	CM_ClearMap();

	if (com_logfile) {
		FS_FCloseFile (com_logfile);
		com_logfile = 0;
		logfile->integer = 0;//don't open up the log file again!!
	}

	if ( com_journalFile ) {
		FS_FCloseFile( com_journalFile );
		com_journalFile = 0;
	}

	MSG_shutdownHuffman();
}

// fills string array with len radom bytes, peferably from the OS randomizer
void Com_RandomBytes( byte *string, int len )
{
	int i;

	if( Sys_RandomBytes( string, len ) )
		return;

	Com_Printf( "Com_RandomBytes: using weak randomization\n" );
	for( i = 0; i < len; i++ )
		string[i] = (unsigned char)( rand() % 256 );
}

// Converts a UTF-8 character to UTF-32.
uint32_t ConvertUTF8ToUTF32( char *utf8CurrentChar, char **utf8NextChar )
{
	uint32_t utf32 = 0;
	char *c = utf8CurrentChar;

	if( ( *c & 0x80 ) == 0 )
		utf32 = *c++;
	else if( ( *c & 0xE0 ) == 0xC0 ) // 110x xxxx
	{
		utf32 |= ( *c++ & 0x1F ) << 6;
		utf32 |= ( *c++ & 0x3F );
	}
	else if( ( *c & 0xF0 ) == 0xE0 ) // 1110 xxxx
	{
		utf32 |= ( *c++ & 0x0F ) << 12;
		utf32 |= ( *c++ & 0x3F ) << 6;
		utf32 |= ( *c++ & 0x3F );
	}
	else if( ( *c & 0xF8 ) == 0xF0 ) // 1111 0xxx
	{
		utf32 |= ( *c++ & 0x07 ) << 18;
		utf32 |= ( *c++ & 0x3F ) << 12;
		utf32 |= ( *c++ & 0x3F ) << 6;
		utf32 |= ( *c++ & 0x3F );
	}
	else
	{
		Com_DPrintf( "Unrecognised UTF-8 lead byte: 0x%x\n", (unsigned int)*c );
		c++;
	}

	*utf8NextChar = c;

	return utf32;
}
