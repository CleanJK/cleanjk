/*
===========================================================================
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

#include "qcommon/q_shared.h"

#define MAXPRINTMSG	4096



enum netadrtype_e {
	NA_BAD = 0, // an address lookup failed
	NA_BOT,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP
};

enum joystickAxis_e {
	AXIS_SIDE,
	AXIS_FORWARD,
	AXIS_UP,
	AXIS_ROLL,
	AXIS_YAW,
	AXIS_PITCH,
	MAX_JOYSTICK_AXIS
};

enum sysEventType_e {
  // bk001129 - make sure SE_NONE is zero
	SE_NONE = 0, // evTime is still valid
	SE_KEY, // evValue is a key code, evValue2 is the down flag
	SE_CHAR, // evValue is an ascii char
	SE_MOUSE, // evValue and evValue2 are reletive signed x / y moves
	SE_JOYSTICK_AXIS, // evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_CONSOLE, // evPtr is a char*
	SE_MAX
};

enum graphicsApi_e {
	GRAPHICS_API_GENERIC,

	// Only OpenGL needs special treatment..
	GRAPHICS_API_OPENGL,
};

enum glProfile_e {
	GLPROFILE_COMPATIBILITY,
	GLPROFILE_CORE,
	GLPROFILE_ES,
};

enum glContextFlag_e : uint32_t {
	GLCONTEXT_DEBUG = (1 << 1),
};



struct netadr_t {
	netadrtype_e type;
	byte         ip[4];
	uint16_t     port;
};

struct sysEvent_t {
	int             evTime;
	sysEventType_e  evType;
	int             evValue, evValue2;
	int             evPtrLength;	// bytes of data pointed to by evPtr, for journaling
	void           *evPtr;			// this must be manually freed if not nullptr
};

// Graphics API
struct window_t {
	void          *handle; // OS-dependent window handle
	graphicsApi_e  api;
};

struct windowDesc_t {
	graphicsApi_e api;
	// Only used if api == GRAPHICS_API_OPENGL
	struct gl_ {
		int         majorVersion;
		int         minorVersion;
		glProfile_e profile;
		uint32_t    contextFlags;
	} gl;
};



typedef void *         GetGameAPIProc  ( void * );
typedef intptr_t QDECL VMMainProc      ( int, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t );
typedef intptr_t QDECL SystemCallProc  ( intptr_t, ... );
typedef void * QDECL   GetModuleAPIProc( int, ... );



extern cvar_t *com_maxfps;
extern cvar_t *com_maxfpsMinimized;
extern cvar_t *com_maxfpsUnfocused;
extern cvar_t *com_minimized;
extern cvar_t *com_unfocused;



extern "C" {
	void Sys_SnapVector( float *v );
}

uint8_t               ConvertUTF32ToExpectedCharset( uint32_t utf32 );
const char           *Sys_Basename                 ( char *path );
char                 *Sys_Cwd                      ( void );
#ifdef MACOS_X
char                 *Sys_DefaultAppPath           ( void );
#endif
char                 *Sys_DefaultHomePath          ( void );
char                 *Sys_DefaultInstallPath       ( void );
const char           *Sys_Dirname                  ( char *path );
void NORETURN QDECL   Sys_Error                    ( const char *error, ... );
time_t                Sys_FileTime                 ( const char *path );
void                  Sys_FreeFileList             ( char **fileList );
char                 *Sys_GetClipboardData         ( void ); // note that this isn't journaled...
char                 *Sys_GetCurrentUser           ( void );
sysEvent_t            Sys_GetEvent                 ( void );
void                  Sys_Init                     ( void );
bool                  Sys_IsLANAddress             ( netadr_t adr );
char                **Sys_ListFiles                ( const char *directory, const char *extension, char *filter, int *numfiles, bool wantsubs );
void * QDECL          Sys_LoadDll                  ( const char *name, bool useSystemLib );
void * QDECL          Sys_LoadGameDll              ( const char *name, GetModuleAPIProc **moduleAPI );
void                 *Sys_LoadSPGameDll            ( const char *name, GetGameAPIProc **GetGameAPI );
bool                  Sys_LowPhysicalMemory        ( void );
int                   Sys_Milliseconds             ( bool baseTime = false );
int                   Sys_Milliseconds2            ( void );
bool                  Sys_Mkdir                    ( const char *path );
bool                  Sys_PathCmp                  ( const char *path1, const char *path2 );
void                  Sys_Print                    ( const char *msg );
void NORETURN         Sys_Quit                     ( void );
bool                  Sys_RandomBytes              ( byte *string, int len );
void                  Sys_SendPacket               ( int length, const void *data, netadr_t to );
void                  Sys_SetDefaultInstallPath    ( const char *path );
void                  Sys_SetErrorText             ( const char *text );
void                  Sys_SetProcessorAffinity     ( void );
void                  Sys_ShowIP                   ( void );
void                  Sys_Sleep                    ( int msec );
bool                  Sys_StringToAdr              ( const char *s, netadr_t *a );
void                  Sys_UnloadDll                ( void *dllHandle );
bool                  WIN_GL_ExtensionSupported    ( const char *extension );
void                 *WIN_GL_GetProcAddress        ( const char *proc );
window_t              WIN_Init                     ( const windowDesc_t *desc, struct glconfig_t *glConfig );
void                  WIN_Present                  ( window_t *window );
void                  WIN_SetGamma                 ( struct glconfig_t *glConfig, byte red[256], byte green[256], byte blue[256] );
void                  WIN_Shutdown                 ( void );
