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

// qcommon.h -- definitions common between client and server, but not game or ref modules

// ======================================================================
// INCLUDE
// ======================================================================

#include "qcommon/q_shared.h"
#include "sys/sys_public.h"

// ======================================================================
// DEFINE
// ======================================================================

typedef void (*xcommand_t)(void);
typedef void (*completionCallback_t)(const char* s);
typedef void (*completionFunc_t)(char* args, int argNum);

#define DEFAULT_RENDER_LIBRARY "rd-vanilla"

#define NET_ENABLEV4		0x01
#define	PACKET_BACKUP	32	// number of old messages that must be kept on client and
							// server for delta comrpession and ping estimation
#define	PACKET_MASK		(PACKET_BACKUP-1)
#define	MAX_PACKET_USERCMDS		32		// max number of usercmd_t in a packet
#define	MAX_SNAPSHOT_ENTITIES	256
#define	PORT_ANY			-1
#define	MAX_RELIABLE_COMMANDS	128			// max string commands buffered for restransmit

//rww - 6/28/02 - Changed from 16384 to match sof2's. This does seem rather huge, but I guess it doesn't really hurt anything.
#define	MAX_MSGLEN				49152		// max length of a message, which may
											// be fragmented into multiple packets

#define MAX_DOWNLOAD_WINDOW			8		// max of eight download frames
#define MAX_DOWNLOAD_BLKSIZE		2048	// 2048 byte block chunks

#define	PROTOCOL_VERSION	26

#define	UPDATE_SERVER_NAME			"updatejk3.ravensoft.com"
#define MASTER_SERVER_NAME			"masterjk3.ravensoft.com"

#define JKHUB_MASTER_SERVER_NAME	"master.jkhub.org"
#define JKHUB_UPDATE_SERVER_NAME	"update.jkhub.org"

#define	PORT_MASTER			29060
#define	PORT_UPDATE			29061
#define	PORT_SERVER			29070	//...+9 more for multiple servers
#define	NUM_SERVER_PORTS	4		// broadcast scan this many ports after PORT_SERVER so a single machine can run multiple servers

// FILESYSTEM
// No stdio calls should be used by any part of the game, because we need to deal with all sorts of directory and seperator char issues.

// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04
#define FS_GAME_REF		0x08
// number of id paks that will never be autodownloaded from base
#define NUM_ID_PAKS		9

#define	MAX_FILE_HANDLES	64

#define DEFAULT_CFG "default.cfg"
#ifdef DEDICATED
#	define Q3CONFIG_CFG PRODUCT_NAME "_server.cfg"
#else
#	define Q3CONFIG_CFG PRODUCT_NAME ".cfg"
#endif

// Edit fields and command line history/completion
#define CONSOLE_PROMPT_CHAR ']'
#define	MAX_EDIT_LINE		256

#define RoundUp(N, M) ((N) + ((unsigned int)(M)) - (((unsigned int)(N)) % ((unsigned int)(M))))
#define RoundDown(N, M) ((N) - (((unsigned int)(N)) % ((unsigned int)(M))))

#if defined(_DEBUG)
#define DEBUG_ZONE_ALLOCS
#endif

// AVI files have the start of pixel lines 4 byte-aligned
#define AVI_LINE_PADDING 4

#define	SV_ENCODE_START		4
#define SV_DECODE_START		12
#define	CL_ENCODE_START		12
#define CL_DECODE_START		4

#define BSP_VERSION				1

// ======================================================================
// ENUM
// ======================================================================

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

// the svc_strings[] array in cl_parse.c should mirror this
// server to client
enum svc_ops_e
{
	svc_bad,
	svc_nop,
	svc_gamestate,
	svc_configstring,			// [short] [string] only in gamestate messages
	svc_baseline,				// only in gamestate messages
	svc_serverCommand,			// [string] to be executed by client game module
	svc_download,				// [short] size [size bytes]
	svc_snapshot,
	svc_setgame,
	svc_mapchange,
	svc_EOF
};

// client to server
enum clc_ops_e
{
	clc_bad,
	clc_nop,
	clc_move,				// [[usercmd_t]
	clc_moveNoDelta,		// [[usercmd_t]
	clc_clientCommand,		// [string] message
	clc_EOF
};

// ======================================================================
// STRUCT
// ======================================================================

typedef struct msg_s
{
	bool	allowoverflow;	// if false, do a Com_Error
	bool	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	bool	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte* data;
	int		maxsize;
	int		cursize;
	int		readcount;
	int		bit;				// for bitwise reads and writes
} msg_t;

typedef struct netchan_s
{
	netsrc_t	sock;

	int			dropped;			// between last packet and previous

	netadr_t	remoteAddress;
	int			qport;				// qport value to write when transmitting

	// sequencing variables
	int			incomingSequence;
	int			outgoingSequence;

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	bool	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
} netchan_t;

// VIRTUAL MACHINE
typedef enum vmSlots_e
{
	VM_GAME = 0,
	VM_CGAME,
	VM_UI,
	MAX_VM
} vmSlots_t;

typedef struct vm_s
{
	vmSlots_t	slot; // VM_GAME, VM_CGAME, VM_UI
	char		name[MAX_QPATH];
	void* dllHandle;

	// fill the import/export tables
	void* (*GetModuleAPI)(int apiVersion, ...);
} vm_t;

typedef struct cmdList_s
{
	const char* name;
	const char* description;
	xcommand_t func;
	completionFunc_t complete;
} cmdList_t;

typedef struct field_s
{
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
} field_t;

// ======================================================================
// CLASS
// ======================================================================

extern vm_t* currentVM;

class VMSwap
{
private:
	VMSwap();
	vm_t* oldVM;
public:
	VMSwap(vm_t* newVM) : oldVM(currentVM)
	{
		currentVM = newVM;
	};
	~VMSwap()
	{
		if (oldVM) currentVM = oldVM;
	};
};

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern const char* vmStrs[MAX_VM];

// com_speeds times
extern	int		time_game;
extern	int		time_frontend;
extern	int		time_backend;		// renderer backend time

extern	int		com_frameTime;

extern	bool	com_errorEntered;

extern	fileHandle_t	com_logfile;
extern	fileHandle_t	com_journalFile;
extern	fileHandle_t	com_journalDataFile;

// ======================================================================
// FUNCTION
// ======================================================================

// MSG
char *MSG_ReadBigString (msg_t *sb);
char *MSG_ReadString (msg_t *sb);
char *MSG_ReadStringLine (msg_t *sb);
float MSG_ReadAngle16 (msg_t *sb);
float MSG_ReadFloat (msg_t *sb);
int	MSG_ReadBits( msg_t *msg, int bits );
int	MSG_ReadByte (msg_t *sb);
int	MSG_ReadChar (msg_t *sb);
int	MSG_ReadLong (msg_t *sb);
int	MSG_ReadShort (msg_t *sb);
void MSG_BeginReading (msg_t *sb);
void MSG_BeginReadingOOB(msg_t *sb);
void MSG_Bitstream( msg_t *buf );
void MSG_Clear (msg_t *buf);
void MSG_Init (msg_t *buf, byte *data, int length);
void MSG_InitOOB( msg_t *buf, byte *data, int length );
void MSG_ReadData (msg_t *sb, void *buffer, int size);
void MSG_ReadDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, int number );
void MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
void MSG_WriteAngle16 (msg_t *sb, float f);
void MSG_WriteBigString (msg_t *sb, const char *s);
void MSG_WriteBits( msg_t *msg, int value, int bits );
void MSG_WriteByte (msg_t *sb, int c);
void MSG_WriteChar (msg_t *sb, int c);
void MSG_WriteData (msg_t *buf, const void *data, int length);
void MSG_WriteDeltaEntity( msg_t *msg, struct entityState_s *from, struct entityState_s *to, bool force );
void MSG_WriteDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
void MSG_WriteFloat (msg_t *sb, float f);
void MSG_WriteLong (msg_t *sb, int c);
void MSG_WriteShort (msg_t *sb, int c);
void MSG_WriteString (msg_t *sb, const char *s);

#ifdef _ONEBIT_COMBO
void MSG_WriteDeltaPlayerstate(msg_t* msg, struct playerState_s* from, struct playerState_s* to, int* bitComboDelta, int* bitNumDelta);
#else
void MSG_WriteDeltaPlayerstate(msg_t* msg, struct playerState_s* from, struct playerState_s* to);
#endif
void MSG_ReadDeltaPlayerstate(msg_t* msg, struct playerState_s* from, struct playerState_s* to);

#ifndef FINAL_BUILD
void MSG_ReportChangeVectors_f(void);
#endif

// FIELD
void Field_AutoComplete(field_t* edit);
void Field_Clear(field_t* edit);
void Field_CompleteCommand(char* cmd, bool doCommands, bool doCvars);
void Field_CompleteFilename(const char* dir, const char* ext, bool stripExt, bool allowNonPureFilesOnDisk);
void Field_CompleteKeyname(void);

// NET
bool NET_CompareAdr(netadr_t a, netadr_t b);
bool NET_CompareBaseAdr(netadr_t a, netadr_t b);
bool NET_CompareBaseAdrMask(netadr_t a, netadr_t b, int netmask);
bool NET_GetLoopPacket(netsrc_t sock, netadr_t* net_from, msg_t* net_message);
bool NET_IsLocalAddress(netadr_t adr);
bool NET_StringToAdr(const char* s, netadr_t* a);
const char* NET_AdrToString(netadr_t a);
void NET_Config(bool enableNetworking);
void NET_Init(void);
void NET_OutOfBandData(netsrc_t sock, netadr_t adr, byte* format, int len);
void NET_OutOfBandPrint(netsrc_t net_socket, netadr_t adr, const char* format, ...);
void NET_Restart_f(void);
void NET_SendPacket(netsrc_t sock, int length, const void* data, netadr_t to);
void NET_Shutdown(void);
void NET_Sleep(int msec);

// NETCHAN handles packet fragmentation and out of order / duplicate suppression
bool Netchan_Process(netchan_t* chan, msg_t* msg);
void Netchan_Init(int qport);
void Netchan_Setup(netsrc_t sock, netchan_t* chan, netadr_t adr, int qport);
void Netchan_Transmit(netchan_t* chan, int length, const byte* data);
void Netchan_TransmitNextFragment(netchan_t* chan);

// VM
vm_t* VM_Create(vmSlots_t vmSlot);
vm_t* VM_Restart(vm_t* vm);
void VM_Clear(void);
void VM_Free(vm_t* vm);
void VM_Init(void);
void VM_Shifted_Alloc(void** ptr, int size);
void VM_Shifted_Free(void** ptr);

// CMD
// Command text buffering and command execution
// Any number of commands can be added in a frame, from several different sources.
// Most commands come from either keybindings or console line input, but entire text files can be execed.
char* Cmd_Args(void);
char* Cmd_ArgsFrom(int arg);
char* Cmd_Argv(int arg);
char* Cmd_Cmd(void);
char* Cmd_DescriptionString(const char* cmd_name);
int Cmd_Argc(void);
void Cbuf_AddText(const char* text);
void Cbuf_Execute(void);
void Cbuf_ExecuteText(int exec_when, const char* text);
void Cbuf_Init(void);
void Cmd_AddCommand(const char* cmd_name, xcommand_t function, const char* cmd_desc = nullptr);
void Cmd_AddCommandList(const cmdList_t* cmdList);
void Cmd_Args_Sanitize(size_t length = MAX_CVAR_VALUE_STRING, const char* strip = "\n\r;", const char* repl = "   ");
void Cmd_ArgsBuffer(char* buffer, int bufferLength);
void Cmd_ArgsFromBuffer(int arg, char* buffer, int bufferLength);
void Cmd_ArgvBuffer(int arg, char* buffer, int bufferLength);
void Cmd_CommandCompletion(completionCallback_t callback);
void Cmd_CompleteArgument(const char* command, char* args, int argNum);
void Cmd_CompleteCfgName(char* args, int argNum);
void Cmd_ExecuteString(const char* text);
void Cmd_Init(void);
void Cmd_RemoveCommand(const char* cmd_name);
void Cmd_RemoveCommandList(const cmdList_t* cmdList);
void Cmd_SetCommandCompletionFunc(const char* command, completionFunc_t complete);
void Cmd_TokenizeString(const char* text);
void Cmd_TokenizeStringIgnoreQuotes(const char* text_in);
void Cmd_VM_RemoveCommand(const char* cmd_name, vmSlots_t vmslot);

// FILESYSTEM
// No stdio calls should be used by any part of the game, because we need to deal with all sorts of directory and seperator char issues.
bool FS_CheckDirTraversal(const char* checkdir);
bool FS_ComparePaks(char* neededpaks, int len, bool dlstring);
bool FS_CompareZipChecksum(const char* zipfile);
bool FS_ConditionalRestart(int checksumFeed);
bool FS_FileExists(const char* file);
bool FS_FilenameCompare(const char* s1, const char* s2);
bool FS_FindPureDLL(const char* name);
bool FS_idPak(char* pak, char* base);
bool FS_Initialized();
bool FS_WriteToTemporaryFile(const void* data, size_t dataLength, char** tempFileName);
char** FS_ListFiles(const char* directory, const char* extension, int* numfiles);
char* FS_BuildOSPath(const char* base, const char* game, const char* qpath);
const char* FS_GetCurrentGameDir(bool emptybase = false);
const char* FS_LoadedPakChecksums(void);
const char* FS_LoadedPakNames(void);
const char* FS_LoadedPakPureChecksums(void);
const char* FS_ReferencedPakChecksums(void);
const char* FS_ReferencedPakNames(void);
const char* FS_ReferencedPakPureChecksums(void);
fileHandle_t FS_FOpenFileWrite(const char* qpath, bool safe = true);
fileHandle_t FS_SV_FOpenFileWrite(const char* filename);
int	FS_FileIsInPAK(const char* filename, int* pChecksum);
int	FS_filelength(fileHandle_t f);
int	FS_filelength(fileHandle_t f);
int	FS_FOpenFileByMode(const char* qpath, fileHandle_t* f, fsMode_t mode);
int	FS_FTell(fileHandle_t f);
int	FS_GetFileList(const char* path, const char* extension, char* listbuf, int bufsize);
int	FS_GetModList(char* listbuf, int bufsize);
int	FS_Read(void* buffer, int len, fileHandle_t f);
int	FS_Seek(fileHandle_t f, long offset, int origin);
int	FS_SV_FOpenFileRead(const char* filename, fileHandle_t* fp);
int	FS_Write(const void* buffer, int len, fileHandle_t f);
long FS_FOpenFileRead(const char* qpath, fileHandle_t* file, bool uniqueFILE);
long FS_ReadFile(const char* qpath, void** buffer);
void FS_ClearPakReferences(int flags);
void FS_FCloseFile(fileHandle_t f);
void FS_FilenameCompletion(const char* dir, const char* ext, bool stripExt, completionCallback_t callback, bool allowNonPureFilesOnDisk);
void FS_Flush(fileHandle_t f);
void FS_ForceFlush(fileHandle_t f);
void FS_FreeFile(void* buffer);
void FS_FreeFileList(char** fileList);
void FS_HomeRemove(const char* homePath);
void FS_HomeRmdir(const char* homePath, bool recursive);
void FS_InitFilesystem(void);
void FS_PureServerSetLoadedPaks(const char* pakSums, const char* pakNames);
void FS_PureServerSetReferencedPaks(const char* pakSums, const char* pakNames);
void FS_Remove(const char* osPath);
void FS_Rename(const char* from, const char* to);
void FS_Restart(int checksumFeed);
void FS_Rmdir(const char* osPath, bool recursive);
void FS_Shutdown(bool closemfp);
void FS_SV_Rename(const char* from, const char* to, bool safe);
void FS_UpdateGamedir(void);
void FS_WriteFile(const char* qpath, const void* buffer, int size);
void QDECL FS_Printf(fileHandle_t f, const char* fmt, ...);

#ifdef MACOS_X
bool FS_LoadMachOBundle(const char* name);
#endif

// MISC
bool Com_SafeMode( void );
char *Com_MD5File(const char *filename, int length, const char *prefix, int prefix_len);
char *CopyString( const char *in );
int Com_EventLoop( void );
int Com_HashKey(char *string, int maxlen);
int	Com_Filter(char *filter, char *name, int casesensitive);
int	Com_FilterPath(char *filter, char *name, int casesensitive);
int	Com_Milliseconds( void );	// will be journaled properly
int	Com_RealTime(qtime_t *qtime);
uint32_t Com_BlockChecksum( const void *buffer, int length );
void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)(char *));
void Com_EndRedirect( void );
void Com_Frame(void);
void Com_Init(char* commandLine);
void Com_RunAndTimeServerPacket(netadr_t *evFrom, msg_t *buf);
void Com_Shutdown(void);
void Com_StartupVariable( const char *match );
void Info_Print( const char *s );
void NORETURN Com_Quit_f( void );
void QDECL Com_DPrintf( const char *fmt, ... );
void QDECL Com_OPrintf( const char *fmt, ...); // Outputs to the VC / Windows Debug window (only in debug compile)

#if defined( _GAME ) || defined( _CGAME ) || defined( UI_BUILD )
extern NORETURN_PTR void (*Com_Error)(int level, const char* fmt, ...);
extern void (*Com_Printf)(const char* fmt, ...);
#else
void NORETURN QDECL Com_Error(int level, const char* fmt, ...);
void QDECL Com_Printf(const char* fmt, ...);
#endif

// MEMORY
/*
typedef enum {
	TAG_FREE,
	TAG_GENERAL,
	TAG_BOTLIB,
	TAG_RENDERER,
	TAG_SMALL,
	TAG_STATIC
} memtag_t;
*/

/*

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

*/

/*
#ifdef DEBUG_ZONE_ALLOCS
	#define Z_TagMalloc(size, tag)			Z_TagMallocDebug(size, tag, #size, __FILE__, __LINE__)
	#define Z_Malloc(size)					Z_MallocDebug(size, #size, __FILE__, __LINE__)
	#define S_Malloc(size)					S_MallocDebug(size, #size, __FILE__, __LINE__)
	void *Z_TagMallocDebug( int size, int tag, char *label, char *file, int line );	// NOT 0 filled memory
	void *Z_MallocDebug( int size, char *label, char *file, int line );			// returns 0 filled memory
	void *S_MallocDebug( int size, char *label, char *file, int line );			// returns 0 filled memory
#else
	void *Z_TagMalloc( int size, int tag );	// NOT 0 filled memory
	void *Z_Malloc( int size );			// returns 0 filled memory
	void *S_Malloc( int size );			// NOT 0 filled memory only for small allocations
#endif
void Z_Free( void *ptr );
void Z_FreeTags( int tag );
int Z_AvailableMemory( void );
void Z_LogHeap( void );
*/

// later on I'll re-implement __FILE__, __LINE__ etc, but for now...

#ifdef DEBUG_ZONE_ALLOCS
void *Z_Malloc  ( int iSize, memtag_t eTag, bool bZeroit = false, int iAlign = 4);	// return memory NOT zero-filled by default
void *S_Malloc	( int iSize );					// NOT 0 filled memory only for small allocations
#else
void *Z_Malloc  ( int iSize, memtag_t eTag, bool bZeroit = false, int iAlign = 4);	// return memory NOT zero-filled by default
void *S_Malloc	( int iSize );					// NOT 0 filled memory only for small allocations
#endif

bool Com_TheHunkMarkHasBeenMade(void);
bool Hunk_CheckMark(void);
int Z_MemSize(memtag_t eTag);
int	Z_Size(void* pvAddress);
int	Hunk_MemoryRemaining(void);
void Z_Free(void* ptr);
void Z_MorphMallocTag(void* pvBuffer, memtag_t eDesiredTag);
void Z_TagFree(memtag_t eTag);
void Z_Validate(void);
void Com_InitHunkMemory(void);
void Com_InitZoneMemory(void);
void Com_InitZoneMemoryVars(void);
void Com_ShutdownHunkMemory(void);
void Com_ShutdownZoneMemory(void);
void Com_TouchMemory(void);
void Hunk_Clear(void);
void Hunk_ClearTempMemory(void);
void Hunk_ClearToMark(void);
void Hunk_FreeTempMemory(void* buf);
void Hunk_Log(void);
void Hunk_SetMark(void);
void Hunk_Trash(void);
void* Hunk_AllocateTempMemory(int size);

// CLIENT INTERFACE
bool CL_ConnectedToRemoteServer(void);
bool CL_GameCommand(void);
void CL_CharEvent(int key);
void CL_ConsolePrint(const char* text);
void CL_Disconnect(bool showMainMenu);
void CL_FlushMemory(void);
void CL_ForwardCommandToServer(const char* string);
void CL_Frame(int msec);
void CL_Init(void);
void CL_InitKeyCommands(void);
void CL_JoystickEvent(int axis, int value, int time);
void CL_KeyEvent(int key, bool down, unsigned time);
void CL_MapLoading(void);
void CL_MouseEvent(int dx, int dy, int time);
void CL_PacketEvent(netadr_t from, msg_t* msg);
void CL_Shutdown(void);
void CL_StartHunkUsers(void);
void Key_KeynameCompletion(void(*callback)(const char* s));
void Key_WriteBindings(fileHandle_t f);
void S_ClearSoundBuffer(void);

// SERVER INTERFACE
bool SV_GameCommand(void);
int SV_FrameMsec(void);
void SV_Frame(int msec);
void SV_Init(void);
void SV_PacketEvent(netadr_t from, msg_t* msg);
void SV_Shutdown(char* finalmsg);
void SV_ShutdownGameProgs(void);

// UI INTERFACE
bool UI_GameCommand( void );

// PERSISTENT DATA STORE API
bool PD_Store(const char* name, const void* data, size_t size);
const void* PD_Load(const char* name, size_t* size);
uint32_t ConvertUTF8ToUTF32(char* utf8CurrentChar, char** utf8NextChar);
void BotDrawDebugPolygons(void (*drawPoly)(int color, int numPoints, float* points), int value);

#include "sys/sys_public.h"
