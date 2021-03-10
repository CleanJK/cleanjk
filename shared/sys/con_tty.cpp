/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "qcommon/q_shared.hpp"
#include "qcommon/q_common.hpp"
#include "qcommon/com_cvars.hpp"
#include "qcommon/com_inputField.hpp"
#include "sys/sys_local.hpp"
#include "sys/con_local.hpp"

#include <unistd.h>
#include <csignal>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>

/*
	tty console routines

NOTE: if the user is editing a line when something gets printed to the early console then it won't look good so we
	provide CON_Hide and CON_Show to be called before and after a stdout or stderr output
*/

extern bool stdinIsATTY;
static bool stdin_active;
// general flag to tell about tty console mode
static bool ttycon_on = false;
static int ttycon_hide = 0;
static int ttycon_show_overdue = 0;

// some key codes that the terminal may be using, initialised on start up
static int TTY_erase;
static int TTY_eof;

static struct termios TTY_tc;

static InputField TTY_con( nullptr, nullptr );

#ifndef BUILD_DEDICATED
	// Don't use "]" as it would be the same as in-game console, this makes it clear where input came from.
	#define TTY_CONSOLE_PROMPT "tty]"
#else
	#define TTY_CONSOLE_PROMPT "]"
#endif

// Flush stdin, I suspect some terminals are sending a LOT of shit
// FIXME relevant?
static void CON_FlushIn( void )
{
	char key;
	while (read(STDIN_FILENO, &key, 1)!=-1);
}

// Clear the display of the line currently edited
// bring cursor back to beginning of line
static void CON_Hide( void ) {
	if ( ttycon_on ) {
		if ( ttycon_hide ) {
			return;
		}
		else {
			TTY_con.Clear();
		}
		ttycon_hide++;
	}
}

// Show the current line
//FIXME need to position the cursor if needed?
static void CON_Show( void ) {
	if ( ttycon_on ) {
		assert( ttycon_hide > 0 );
		ttycon_hide--;
		if ( ttycon_hide == 0 ) {
			size_t UNUSED_VAR size;
			size = write( STDOUT_FILENO, TTY_CONSOLE_PROMPT, strlen( TTY_CONSOLE_PROMPT ) );
			/*
			if ( TTY_con.cursor ) {
				for ( int i = 0; i < TTY_con.cursor; i++ ) {
					size = write( STDOUT_FILENO, TTY_con.buffer.c_str() + i, 1 );
				}
			}
			*/
		}
	}
}

// Never exit without calling this, or your terminal will be left in a pretty bad state
void CON_Shutdown( void ) {
	if ( ttycon_on ) {
		CON_Hide();
		tcsetattr( STDIN_FILENO, TCSADRAIN, &TTY_tc );
	}

	// Restore blocking to stdin reads
	fcntl( STDIN_FILENO, F_SETFL, fcntl( STDIN_FILENO, F_GETFL, 0 ) & ~O_NONBLOCK );
}

// Reinitialize console input after receiving SIGCONT, as on Linux the terminal seems to lose all set attributes if user
//	did CTRL+Z and then does fg again.
void CON_SigCont( int signum ) {
	CON_Init();
}

// Initialize the console input (tty mode if possible)
void CON_Init( void ) {
	struct termios tc;

	// If the process is backgrounded (running non interactively) then SIGTTIN or SIGTOU is emitted, if not caught,
	//	turns into a SIGSTP
	signal( SIGTTIN, SIG_IGN );
	signal( SIGTTOU, SIG_IGN );

	// If SIGCONT is received, reinitialize console
	signal( SIGCONT, CON_SigCont );

	// Make stdin reads non-blocking
	fcntl( STDIN_FILENO, F_SETFL, fcntl( STDIN_FILENO, F_GETFL, 0 ) | O_NONBLOCK );

	if ( !stdinIsATTY ) {
		Com_Printf( "tty console mode disabled\n" );
		ttycon_on = false;
		stdin_active = true;
		return;
	}

	TTY_con.Clear();
	tcgetattr( STDIN_FILENO, &TTY_tc );
	TTY_erase = TTY_tc.c_cc[VERASE];
	TTY_eof = TTY_tc.c_cc[VEOF];
	tc = TTY_tc;

	/*
	ECHO: don't echo input characters
	ICANON: enable canonical mode.  This  enables  the  special
	characters  EOF,  EOL,  EOL2, ERASE, KILL, REPRINT,
	STATUS, and WERASE, and buffers by lines.
	ISIG: when any of the characters  INTR,  QUIT,  SUSP,  or
	DSUSP are received, generate the corresponding signal
	*/
	tc.c_lflag &= ~(ECHO | ICANON);

	/*
	ISTRIP strip off bit 8
	INPCK enable input parity checking
	*/
	tc.c_iflag &= ~(ISTRIP | INPCK);
	tc.c_cc[VMIN] = 1;
	tc.c_cc[VTIME] = 0;
	tcsetattr( STDIN_FILENO, TCSADRAIN, &tc );
	ttycon_on = true;
	ttycon_hide = 1; // Mark as hidden, so prompt is shown in CON_Show
	CON_Show();
}

char *CON_Input( void ) {
	// we use this when sending back commands
	static char text[MAX_STRING_CHARS];
	int avail;
	char key;
	size_t UNUSED_VAR size;

	if ( ttycon_on ) {
		avail = read( STDIN_FILENO, &key, 1 );
		if ( avail != -1 ) {
			// we have something
			// backspace?
			// NOTE TTimo testing a lot of values .. seems it's the only way to get it to work everywhere
			if ( key == TTY_erase || key == 127 || key == 8 ) {
				/*
				if ( TTY_con.cursor > 0 ) {
					TTY_con.cursor--;
					TTY_con.buffer[TTY_con.cursor] = '\0';
					CON_Back();
				}
				*/
				return nullptr;
			}
			// check if this is a control char
			if ( key && key < ' ' ) {
				if ( key == '\n' ) {
#ifndef BUILD_DEDICATED
					if ( TTY_con.buffer[0] == '/' || TTY_con.buffer[0] == '\\' ) {
						Q_strncpyz( text, TTY_con.buffer.c_str() + 1, sizeof(text) );
					}
					/*
					else if ( TTY_con.cursor ) {
						Q_strncpyz( text, TTY_con.buffer.c_str(), sizeof(text) );
					}
					*/
					else {
						text[0] = '\0';
					}

					// push it in history
					TTY_con.history.push_back( TTY_con.buffer );
					Com_Printf( TTY_CONSOLE_PROMPT "%s\n", TTY_con.buffer.c_str() );
					TTY_con.Clear();
#else
					// push it in history
					TTY_con.history.push_back( TTY_con.buffer );
					Q_strncpyz( text, TTY_con.buffer.c_str(), sizeof(text));
					TTY_con.Clear();
					key = '\n';
					size = write( STDOUT_FILENO, &key, 1 );
					size = write( STDOUT_FILENO, TTY_CONSOLE_PROMPT, strlen( TTY_CONSOLE_PROMPT ) );
#endif
					return text;
				}
				if ( key == '\t' ) {
					//TTY_con.autoComplete();
					return nullptr;
				}
				avail = read( STDIN_FILENO, &key, 1 );
				if ( avail != -1 ) {
					// VT 100 keys
					if ( key == '[' || key == 'O' ) {
						avail = read( STDIN_FILENO, &key, 1 );
						if ( avail != -1 ) {
							switch ( key ) {
							/*
							case 'A': {
								history = Hist_Prev();
								if ( history ) {
									CON_Hide();
									TTY_con = *history;
									CON_Show();
								}
								CON_FlushIn();
								return nullptr;
							} break;
							case 'B': {
								history = Hist_Next();
								CON_Hide();
								if ( history ) {
									TTY_con = *history;
								}
								else {
									TTY_con.Clear();
								}
								CON_Show();
								CON_FlushIn();
								return nullptr;
							} break;
							*/
							case 'C': {
								return nullptr;
							} break;
							case 'D': {
								return nullptr;
							} break;

							}
						}
					}
				}
				Com_DPrintf("droping ISCTL sequence: %d, TTY_erase: %d\n", key, TTY_erase);
				CON_FlushIn();
				return nullptr;
			}
			/*
			if ( TTY_con.cursor >= (int)sizeof(text) - 1 ) {
				return nullptr;
			}
			// push regular character
			TTY_con.buffer[TTY_con.cursor] = key;
			TTY_con.cursor++; // next char will always be '\0'
			*/
			// print the current line (this is differential)
			size = write(STDOUT_FILENO, &key, 1);
		}

		return nullptr;
	}
	else if (stdin_active)
	{
		int     len;
		fd_set  fdset;
		struct timeval timeout;

		FD_ZERO(&fdset);
		FD_SET(STDIN_FILENO, &fdset); // stdin
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if(select (STDIN_FILENO + 1, &fdset, nullptr, nullptr, &timeout) == -1 || !FD_ISSET(STDIN_FILENO, &fdset))
			return nullptr;

		len = read(STDIN_FILENO, text, sizeof(text));
		if (len == 0)
		{ // eof!
			stdin_active = false;
			return nullptr;
		}

		if (len < 1)
			return nullptr;
		text[len-1] = 0;    // rip off the /n and terminate

		return text;
	}
	return nullptr;
}

/*
==================
CON_Print
==================
*/
void CON_Print( const char *msg )
{
	if (!msg[0])
		return;

	CON_Hide( );

	if( com_ansiColor && com_ansiColor->integer )
		Sys_AnsiColorPrint( msg );
	else
		fputs( msg, stderr );

	if (!ttycon_on) {
		// CON_Hide didn't do anything.
		return;
	}

	// Only print prompt when msg ends with a newline, otherwise the console
	//   might get garbled when output does not fit on one line.
	if (msg[strlen(msg) - 1] == '\n') {
		CON_Show();

		// Run CON_Show the number of times it was deferred.
		while (ttycon_show_overdue > 0) {
			CON_Show();
			ttycon_show_overdue--;
		}
	}
	else
	{
		// Defer calling CON_Show
		ttycon_show_overdue++;
	}
}
