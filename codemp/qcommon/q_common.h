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

#pragma once

/* definitions common between client and server, but not game or ref modules

MEMORY
------

memory layout
low memory
	server vm
	server clipmap
mark
	renderer initialization (shaders, etc)
	UI vm
	cgame vm
	renderer map
	renderer models
free
	temp file loading
high memory



FILESYSTEM
----------

No stdio calls should be used by any part of the game, because we need to deal with all sorts of directory and seperator char issues.



COMMANDS
--------

Command text buffering and command execution
Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but entire text files can be execed.



NETCHAN
-------

handles packet fragmentation and out of order / duplicate suppression

*/

#include "qcommon/q_shared.h"
#include "sys/sys_public.h"



#if defined(_DEBUG)
	#define DEBUG_ZONE_ALLOCS
#endif

#ifdef DEDICATED
	#define Q3CONFIG_CFG PRODUCT_NAME "_server.cfg"
#else
	#define Q3CONFIG_CFG PRODUCT_NAME ".cfg"
#endif



#define	CL_ENCODE_START          12
#define	MAX_EDIT_LINE            256 // Edit fields and command line history/completion
#define	MAX_FILE_HANDLES         64
#define	MAX_MSGLEN               49152 // max length of a message, which may be fragmented into multiple packets
#define	MAX_PACKET_USERCMDS      32 // max number of usercmd_t in a packet
#define	MAX_RELIABLE_COMMANDS    128 // max string commands buffered for restransmit
#define	MAX_SNAPSHOT_ENTITIES    256
#define	NUM_SERVER_PORTS         4 // broadcast scan this many ports after PORT_SERVER so a single machine can run multiple servers
#define	PACKET_BACKUP            32 // number of old messages that must be kept on client and server for delta comrpession and ping estimation
#define	PACKET_MASK              (PACKET_BACKUP-1)
#define	PORT_ANY                 -1
#define	PORT_MASTER              29060
#define	PORT_SERVER              29070 //...+9 more for multiple servers
#define	PORT_UPDATE              29061
#define	PROTOCOL_VERSION         26
#define	SV_ENCODE_START          4
#define	UPDATE_SERVER_NAME       "updatejk3.ravensoft.com"
#define AVI_LINE_PADDING         4 // AVI files have the start of pixel lines 4 byte-aligned
#define BSP_VERSION              1
#define CL_DECODE_START          4
#define CONSOLE_PROMPT_CHAR      ']'
#define DEFAULT_CFG              "default.cfg"
#define DEFAULT_RENDER_LIBRARY   "rd-vanilla"
#define JKHUB_MASTER_SERVER_NAME "master.jkhub.org"
#define JKHUB_UPDATE_SERVER_NAME "update.jkhub.org"
#define MASTER_SERVER_NAME       "masterjk3.ravensoft.com"
#define MAX_DOWNLOAD_BLKSIZE     2048 // 2048 byte block chunks
#define MAX_DOWNLOAD_WINDOW      8 // max of eight download frames
#define NET_ENABLEV4             0x01
#define NUM_ID_PAKS              9
#define SV_DECODE_START          12



typedef void (*xcommand_t)( void );
typedef void (*completionCallback_t)( const char *s );
typedef void (*completionFunc_t)( char *args, int argNum );



// these are in loop specific order so don't change the order
enum fsRefFlag_e : uint32_t {
	FS_GENERAL_REF = 0x01,
	FS_UI_REF =      0x02,
	FS_CGAME_REF =   0x04,
	FS_GAME_REF =    0x08,
};

enum netsrc_e {
	NS_CLIENT,
	NS_SERVER
};

// the svc_strings[] array in cl_parse.c should mirror this
// server to client
enum svc_ops_e {
	svc_bad,
	svc_nop,
	svc_gamestate,
	svc_configstring, // [short] [string] only in gamestate messages
	svc_baseline, // only in gamestate messages
	svc_serverCommand, // [string] to be executed by client game module
	svc_download, // [short] size [size bytes]
	svc_snapshot,
	svc_setgame,
	svc_mapchange,
	svc_EOF
};

// client to server
enum clc_ops_e {
	clc_bad,
	clc_nop,
	clc_move, // [[usercmd_t]
	clc_moveNoDelta, // [[usercmd_t]
	clc_clientCommand, // [string] message
	clc_EOF
};

enum vmSlots_e {
	VM_GAME = 0,
	VM_CGAME,
	VM_UI,
	MAX_VM
};



struct msg_t {
	bool  allowoverflow; // if false, do a Com_Error
	bool  overflowed; // set to true if the buffer size failed (with allowoverflow set)
	bool  oob; // set to true if the buffer size failed (with allowoverflow set)
	byte *data;
	int   maxsize;
	int   cursize;
	int   readcount;
	int   bit; // for bitwise reads and writes
};

struct netchan_t {
	netsrc_e sock;
	int      dropped; // between last packet and previous
	netadr_t remoteAddress;
	int      qport; // qport value to write when transmitting
	// sequencing variables
	int      incomingSequence;
	int      outgoingSequence;
	// incoming fragment assembly buffer
	int      fragmentSequence;
	int      fragmentLength;
	byte     fragmentBuffer[MAX_MSGLEN];
	// outgoing fragment buffer - we need to space out the sending of large fragmented messages
	bool     unsentFragments;
	int      unsentFragmentStart;
	int      unsentLength;
	byte     unsentBuffer[MAX_MSGLEN];
};

struct vm_t {
	vmSlots_e  slot; // VM_GAME, VM_CGAME, VM_UI
	char       name[MAX_QPATH];
	void      *dllHandle;
	// fill the import/export tables
	void      *(*GetModuleAPI)( int apiVersion, ... );
};

struct cmdList_t {
	const char* name;
	const char* description;
	xcommand_t func;
	completionFunc_t complete;
};

struct field_t {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
};



extern vm_t *currentVM;
class VMSwap {
private:
	VMSwap();
	vm_t *oldVM;
public:
	VMSwap( vm_t *newVM) : oldVM( currentVM ) {
		currentVM = newVM;
	};
	~VMSwap() {
		if ( oldVM ) currentVM = oldVM;
	};
};



extern bool          com_errorEntered;
extern int           com_frameTime;
extern fileHandle_t  com_journalDataFile;
extern fileHandle_t  com_journalFile;
extern fileHandle_t  com_logfile;
extern int           time_backend; // renderer backend time
extern int           time_frontend;
extern int           time_game;
extern const char   *vmStrs[MAX_VM];



#ifdef _ONEBIT_COMBO
	void MSG_WriteDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to, int *bitComboDelta, int *bitNumDelta );
#else
	void MSG_WriteDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to );
#endif

#ifndef FINAL_BUILD
	void MSG_ReportChangeVectors_f(void);
#endif

#if defined( _GAME ) || defined( _CGAME ) || defined( UI_BUILD )
	extern NORETURN_PTR void (*Com_Error) ( int level, const char *fmt, ... );
	extern void              (*Com_Printf)( const char *fmt, ... );
#else
	void NORETURN QDECL Com_Error ( int level, const char *fmt, ... );
	void QDECL          Com_Printf( const char *fmt, ... );
#endif

#ifdef DEBUG_ZONE_ALLOCS
	void *Z_Malloc( int iSize, memtag_t eTag, bool bZeroit = false, int iAlign = 4 );	// return memory NOT zero-filled by default
	void *S_Malloc( int iSize );					// NOT 0 filled memory only for small allocations
#else
	void *Z_Malloc( int iSize, memtag_t eTag, bool bZeroit = false, int iAlign = 4 );	// return memory NOT zero-filled by default
	void *S_Malloc( int iSize );					// NOT 0 filled memory only for small allocations
#endif

#ifdef MACOS_X
	bool FS_LoadMachOBundle(const char* name);
#endif



void            BotDrawDebugPolygons          ( void (*drawPoly)( int color, int numPoints, float *points ), int value );
void            Cbuf_AddText                  ( const char *text );
void            Cbuf_Execute                  ( void );
void            Cbuf_ExecuteText              ( int exec_when, const char *text );
void            Cbuf_Init                     ( void );
void            CL_CharEvent                  ( int key );
bool            CL_ConnectedToRemoteServer    ( void );
void            CL_ConsolePrint               ( const char *text );
void            CL_Disconnect                 ( bool showMainMenu );
void            CL_FlushMemory                ( void );
void            CL_ForwardCommandToServer     ( const char *string );
void            CL_Frame                      ( int msec );
bool            CL_GameCommand                ( void );
void            CL_Init                       ( void );
void            CL_InitKeyCommands            ( void );
void            CL_JoystickEvent              ( int axis, int value, int time );
void            CL_KeyEvent                   ( int key, bool down, unsigned time );
void            CL_MapLoading                 ( void );
void            CL_MouseEvent                 ( int dx, int dy, int time );
void            CL_PacketEvent                ( netadr_t from, msg_t *msg );
void            CL_Shutdown                   ( void );
void            CL_StartHunkUsers             ( void );
void            Cmd_AddCommand                ( const char *cmd_name, xcommand_t function, const char *cmd_desc = nullptr );
void            Cmd_AddCommandList            ( const cmdList_t *cmdList );
int             Cmd_Argc                      ( void );
char           *Cmd_Args                      ( void );
void            Cmd_Args_Sanitize             ( size_t length = MAX_CVAR_VALUE_STRING, const char *strip = "\n\r;", const char *repl = "   ");
void            Cmd_ArgsBuffer                ( char *buffer, int bufferLength );
char           *Cmd_ArgsFrom                  ( int arg );
void            Cmd_ArgsFromBuffer            ( int arg, char *buffer, int bufferLength );
char           *Cmd_Argv                      ( int arg );
void            Cmd_ArgvBuffer                ( int arg, char *buffer, int bufferLength );
char           *Cmd_Cmd                       ( void );
void            Cmd_CommandCompletion         ( completionCallback_t callback );
void            Cmd_CompleteArgument          ( const char *command, char *args, int argNum );
void            Cmd_CompleteCfgName           ( char *args, int argNum );
char           *Cmd_DescriptionString         ( const char *cmd_name );
void            Cmd_ExecuteString             ( const char *text );
void            Cmd_Init                      ( void );
void            Cmd_RemoveCommand             ( const char *cmd_name );
void            Cmd_RemoveCommandList         ( const cmdList_t *cmdList );
void            Cmd_SetCommandCompletionFunc  ( const char *command, completionFunc_t complete );
void            Cmd_TokenizeString            ( const char *text );
void            Cmd_TokenizeStringIgnoreQuotes( const char *text_in );
void            Cmd_VM_RemoveCommand          ( const char *cmd_name, vmSlots_e vmslot );
void            Com_BeginRedirect             ( char *buffer, int buffersize, void (*flush)( char * ) );
uint32_t        Com_BlockChecksum             ( const void *buffer, int length );
void QDECL      Com_DPrintf                   ( const char *fmt, ... );
void            Com_EndRedirect               ( void );
int             Com_EventLoop                 ( void );
int             Com_Filter                    ( char *filter, char *name, int casesensitive );
int             Com_FilterPath                ( char *filter, char *name, int casesensitive );
void            Com_Frame                     ( void );
int             Com_HashKey                   ( char *string, int maxlen );
void            Com_Init                      ( char *commandLine );
void            Com_InitHunkMemory            ( void );
void            Com_InitZoneMemory            ( void );
void            Com_InitZoneMemoryVars        ( void );
char           *Com_MD5File                   ( const char *filename, int length, const char *prefix, int prefix_len );
int             Com_Milliseconds              ( void );	// will be journaled properly
void QDECL      Com_OPrintf                   ( const char *fmt, ... ); // Outputs to the VC / Windows Debug window ( only in debug compile)
void NORETURN   Com_Quit_f                    ( void );
int             Com_RealTime                  ( qtime_t *qtime );
void            Com_RunAndTimeServerPacket    ( netadr_t *evFrom, msg_t *buf );
bool            Com_SafeMode                  ( void );
void            Com_Shutdown                  ( void );
void            Com_ShutdownHunkMemory        ( void );
void            Com_ShutdownZoneMemory        ( void );
void            Com_StartupVariable           ( const char *match );
bool            Com_TheHunkMarkHasBeenMade    ( void );
void            Com_TouchMemory               ( void );
uint32_t        ConvertUTF8ToUTF32            ( char *utf8CurrentChar, char **utf8NextChar );
char           *CopyString                    ( const char *in );
void            Field_AutoComplete            ( field_t *edit );
void            Field_Clear                   ( field_t *edit );
void            Field_CompleteCommand         ( char *cmd, bool doCommands, bool doCvars );
void            Field_CompleteFilename        ( const char *dir, const char *ext, bool stripExt, bool allowNonPureFilesOnDisk );
void            Field_CompleteKeyname         ( void );
char           *FS_BuildOSPath                ( const char *base, const char *game, const char *qpath );
bool            FS_CheckDirTraversal          ( const char *checkdir );
void            FS_ClearPakReferences         ( int flags );
bool            FS_ComparePaks                ( char *neededpaks, int len, bool dlstring );
bool            FS_CompareZipChecksum         ( const char *zipfile );
bool            FS_ConditionalRestart         ( int checksumFeed );
void            FS_FCloseFile                 ( fileHandle_t f );
bool            FS_FileExists                 ( const char *file );
int             FS_FileIsInPAK                ( const char *filename, int *pChecksum );
int             FS_filelength                 ( fileHandle_t f );
bool            FS_FilenameCompare            ( const char *s1, const char *s2 );
void            FS_FilenameCompletion         ( const char *dir, const char *ext, bool stripExt, completionCallback_t callback, bool allowNonPureFilesOnDisk );
bool            FS_FindPureDLL                ( const char *name );
void            FS_Flush                      ( fileHandle_t f );
int             FS_FOpenFileByMode            ( const char *qpath, fileHandle_t *f, fsMode_e mode );
long            FS_FOpenFileRead              ( const char *qpath, fileHandle_t *file, bool uniqueFILE );
fileHandle_t    FS_FOpenFileWrite             ( const char *qpath, bool safe = true );
void            FS_ForceFlush                 ( fileHandle_t f );
void            FS_FreeFile                   ( void *buffer );
void            FS_FreeFileList               ( char **fileList );
int             FS_FTell                      ( fileHandle_t f );
const char     *FS_GetCurrentGameDir          ( bool emptybase = false );
int             FS_GetFileList                ( const char *path, const char *extension, char *listbuf, int bufsize );
int             FS_GetModList                 ( char *listbuf, int bufsize );
void            FS_HomeRemove                 ( const char *homePath );
void            FS_HomeRmdir                  ( const char *homePath, bool recursive );
bool            FS_idPak                      ( char *pak, char *base );
void            FS_InitFilesystem             ( void );
bool            FS_Initialized                ( void );
char          **FS_ListFiles                  ( const char *directory, const char *extension, int *numfiles );
const char     *FS_LoadedPakChecksums         ( void );
const char     *FS_LoadedPakNames             ( void );
const char     *FS_LoadedPakPureChecksums     ( void );
void QDECL      FS_Printf                     ( fileHandle_t f, const char *fmt, ... );
void            FS_PureServerSetLoadedPaks    ( const char *pakSums, const char *pakNames );
void            FS_PureServerSetReferencedPaks( const char *pakSums, const char *pakNames );
int             FS_Read                       ( void *buffer, int len, fileHandle_t f );
long            FS_ReadFile                   ( const char *qpath, void **buffer );
const char     *FS_ReferencedPakChecksums     ( void );
const char     *FS_ReferencedPakNames         ( void );
const char     *FS_ReferencedPakPureChecksums ( void );
void            FS_Remove                     ( const char *osPath );
void            FS_Rename                     ( const char *from, const char *to );
void            FS_Restart                    ( int checksumFeed );
void            FS_Rmdir                      ( const char *osPath, bool recursive );
int             FS_Seek                       ( fileHandle_t f, long offset, int origin );
void            FS_Shutdown                   ( bool closemfp );
fileHandle_t    FS_SV_FOpenFileWrite          ( const char *filename );
int             FS_SV_FOpenFileRead           ( const char *filename, fileHandle_t *fp );
void            FS_SV_Rename                  ( const char *from, const char *to, bool safe );
void            FS_UpdateGamedir              ( void );
int             FS_Write                      ( const void *buffer, int len, fileHandle_t f );
void            FS_WriteFile                  ( const char *qpath, const void *buffer, int size );
bool            FS_WriteToTemporaryFile       ( const void *data, size_t dataLength, char **tempFileName );
void           *Hunk_AllocateTempMemory       ( int size );
bool            Hunk_CheckMark                ( void );
void            Hunk_Clear                    ( void );
void            Hunk_ClearTempMemory          ( void );
void            Hunk_ClearToMark              ( void );
void            Hunk_FreeTempMemory           ( void *buf );
void            Hunk_Log                      ( void );
int             Hunk_MemoryRemaining          ( void );
void            Hunk_SetMark                  ( void );
void            Hunk_Trash                    ( void );
void            Info_Print                    ( const char *s );
void            Key_KeynameCompletion         ( void (*callback)( const char *s ) );
void            Key_WriteBindings             ( fileHandle_t f );
void            MSG_BeginReading              ( msg_t *sb );
void            MSG_BeginReadingOOB           ( msg_t *sb );
void            MSG_Bitstream                 ( msg_t *buf );
void            MSG_Clear                     ( msg_t *buf );
void            MSG_Init                      ( msg_t *buf, byte *data, int length );
void            MSG_InitOOB                   ( msg_t *buf, byte *data, int length );
float           MSG_ReadAngle16               ( msg_t *sb );
char           *MSG_ReadBigString             ( msg_t *sb );
int             MSG_ReadBits                  ( msg_t *msg, int bits );
int             MSG_ReadByte                  ( msg_t *sb );
int             MSG_ReadChar                  ( msg_t *sb );
void            MSG_ReadData                  ( msg_t *sb, void *buffer, int size );
void            MSG_ReadDeltaEntity           ( msg_t *msg, entityState_t *from, entityState_t *to, int number );
void            MSG_ReadDeltaPlayerstate      ( msg_t *msg, playerState_t *from, playerState_t *to );
void            MSG_ReadDeltaUsercmdKey       ( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
float           MSG_ReadFloat                 ( msg_t *sb );
int             MSG_ReadLong                  ( msg_t *sb );
int             MSG_ReadShort                 ( msg_t *sb );
char           *MSG_ReadString                ( msg_t *sb );
char           *MSG_ReadStringLine            ( msg_t *sb );
void            MSG_WriteAngle16              ( msg_t *sb, float f );
void            MSG_WriteBigString            ( msg_t *sb, const char *s );
void            MSG_WriteBits                 ( msg_t *msg, int value, int bits );
void            MSG_WriteByte                 ( msg_t *sb, int c );
void            MSG_WriteChar                 ( msg_t *sb, int c );
void            MSG_WriteData                 ( msg_t *buf, const void *data, int length );
void            MSG_WriteDeltaEntity          ( msg_t *msg, entityState_t *from, entityState_t *to, bool force );
void            MSG_WriteDeltaUsercmdKey      ( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
void            MSG_WriteFloat                ( msg_t *sb, float f );
void            MSG_WriteLong                 ( msg_t *sb, int c );
void            MSG_WriteShort                ( msg_t *sb, int c );
void            MSG_WriteString               ( msg_t *sb, const char *s );
const char     *NET_AdrToString               ( netadr_t a );
bool            NET_CompareAdr                ( netadr_t a, netadr_t b );
bool            NET_CompareBaseAdr            ( netadr_t a, netadr_t b );
bool            NET_CompareBaseAdrMask        ( netadr_t a, netadr_t b, int netmask );
void            NET_Config                    ( bool enableNetworking );
bool            NET_GetLoopPacket             ( netsrc_e sock, netadr_t *net_from, msg_t *net_message );
void            NET_Init                      ( void );
bool            NET_IsLocalAddress            ( netadr_t adr );
void            NET_OutOfBandData             ( netsrc_e sock, netadr_t adr, byte *format, int len );
void            NET_OutOfBandPrint            ( netsrc_e net_socket, netadr_t adr, const char *format, ... );
void            NET_Restart_f                 ( void );
void            NET_SendPacket                ( netsrc_e sock, int length, const void *data, netadr_t to );
void            NET_Shutdown                  ( void );
void            NET_Sleep                     ( int msec );
bool            NET_StringToAdr               ( const char *s, netadr_t *a );
void            Netchan_Init                  ( int qport );
bool            Netchan_Process               ( netchan_t *chan, msg_t *msg );
void            Netchan_Setup                 ( netsrc_e sock, netchan_t *chan, netadr_t adr, int qport );
void            Netchan_Transmit              ( netchan_t *chan, int length, const byte *data );
void            Netchan_TransmitNextFragment  ( netchan_t *chan );
const void     *PD_Load                       ( const char *name, size_t *size );
bool            PD_Store                      ( const char *name, const void *data, size_t size );
void            S_ClearSoundBuffer            ( void );
void            SV_Frame                      ( int msec );
int             SV_FrameMsec                  ( void );
bool            SV_GameCommand                ( void );
void            SV_Init                       ( void );
void            SV_PacketEvent                ( netadr_t from, msg_t *msg );
void            SV_Shutdown                   ( char *finalmsg );
void            SV_ShutdownGameProgs          ( void );
bool            UI_GameCommand                ( void );
void            VM_Clear                      ( void );
vm_t           *VM_Create                     ( vmSlots_e vmSlot );
void            VM_Free                       ( vm_t *vm );
void            VM_Init                       ( void );
vm_t           *VM_Restart                    ( vm_t *vm );
void            VM_Shifted_Alloc              ( void **ptr, int size );
void            VM_Shifted_Free               ( void **ptr );
void            Z_Free                        ( void *ptr );
int             Z_MemSize                     ( memtag_t eTag );
void            Z_MorphMallocTag              ( void *pvBuffer, memtag_t eDesiredTag );
int             Z_Size                        ( void *pvAddress );
void            Z_TagFree                     ( memtag_t eTag );
void            Z_Validate                    ( void );
