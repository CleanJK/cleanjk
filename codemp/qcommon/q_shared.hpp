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

#include <cassert>
#include <cstdint>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "qcommon/disablewarnings.hpp"
#include "qcommon/q_color.hpp"
#include "qcommon/q_math.hpp"
#include "qcommon/q_platform.h"
#include "qcommon/q_string.hpp"
#include "qcommon/q_type.hpp"

// included first by ALL program modules.

// conditional compiling
#define PARANOID // cry over spilled milk
#define PATCH_STITCHING // BSP patch stitching
//#define _ONEBIT_COMBO
#define _G2_GORE
//#define G2_EHNANCEMENTS
/*
#ifdef G2_EHNANCEMENTS
#define JK2_RAGDOLL
#define MP_BONECACHE
#endif
*/

#ifndef FINAL_BUILD
//#define G2_PERFORMANCE_ANALYSIS
//#define _FULL_G2_LEAK_CHECKING
#define HUNK_DEBUG
#endif

#ifdef PATH_MAX
	#define MAX_OSPATH			PATH_MAX
#else
	#define	MAX_OSPATH			256		// max length of a filesystem pathname
#endif

#if !defined MAX_LIGHT_STYLES
	#define MAX_LIGHT_STYLES	64
#endif

#define	VALIDATE( a )             ( assert( a ) )
#define	VALIDATEB( a )            if ( a == nullptr ) {	assert(0);	return false;	}
#define	VALIDATEV( a )            if ( a == nullptr ) {	assert(0);	return;			}
#define ARRAY_LEN( x )            ( sizeof( x ) / sizeof( *(x) ) )
#define INT_ID( a, b, c, d )      (uint32_t)((((a) & 0xff) << 24) | (((b) & 0xff) << 16) | (((c) & 0xff) << 8) | ((d) & 0xff))
#define ENUM2STRING( arg )        { #arg, arg }
#define TAGDEF( blah )            TAG_ ## blah
#define PAD( base, alignment )    (((base)+(alignment)-1) & ~((alignment)-1))
#define PADLEN( base, alignment ) (PAD((base), (alignment)) - (base))
#define PADP( base, alignment )   ((void *) PAD((intptr_t) (base), (alignment)))
#define STRING( a )               #a
#define VALID( a )                ( a != nullptr )
#define VALIDATEP( a )            if ( a == nullptr ) {	assert(0);	return nullptr;	}
#define VALIDENT( e )             ( ( e != nullptr ) && ( (e)->inuse ) )
#define VALIDSTRING( a )          ( ( a != nullptr ) && ( a[0] != '\0' ) )
#define XSTRING( a )              STRING( a )
#define Square(x)                 ((x)*(x))
#define	ANGLE2SHORT(x)            ((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)            ((x)*(360.0/65536))

#define BASEGAME                             "base"
#define BIG_INFO_KEY                         8192
#define BIG_INFO_STRING                      8192  // used for system info key only
#define BIG_INFO_VALUE                       8192
#define BIGCHAR_HEIGHT                       16
#define BIGCHAR_WIDTH                        16
#define BLINK_DIVISOR                        200
#define CLIENT_CONSOLE_TITLE                 "CleanJoKe Console (MP)"
#define CLIENT_WINDOW_TITLE                  "CleanJoKe (MP)"
#define DEFAULT_GRID_SPACING                 400
#define ENTITYNUM_MAX_NORMAL                 (MAX_GENTITIES-2)
#define ENTITYNUM_NONE                       (MAX_GENTITIES-1)
#define ENTITYNUM_WORLD                      (MAX_GENTITIES-2)
#define FALL_FADE_TIME                       3000
#define FORCE_DARKSIDE                       2
#define FORCE_LIGHTSIDE                      1
#define GENTITYNUM_BITS                      10		// don't need to send any more
#define GIANTCHAR_HEIGHT                     48
#define GIANTCHAR_WIDTH                      32
#define HOMEPATH_NAME_MACOSX                 HOMEPATH_NAME_WIN
#define HOMEPATH_NAME_UNIX                   "cleanjoke"
#define HOMEPATH_NAME_WIN                    "cleanjoke"
#define LS_NUM_STYLES                        32
#define LS_NUM_SWITCH                        32
#define LS_STYLES_START                      0
#define LS_SWITCH_START                      (LS_STYLES_START+LS_NUM_STYLES)
#define MATERIAL_BITS                        5
#define MATERIAL_MASK                        0x1f // mask to get the material type
#define MAX_ADDRESSLENGTH                    256 //64
#define MAX_AMBIENT_SETS                     256 //rww - ambient soundsets must be sent over in config strings.
#define MAX_AMMO                             MAX_WEAPONS
#define MAX_AMMO_TRANSMIT                    16 // This is needed because the ammo array is 19 but only 16 sized array is networked
#define MAX_CLIENTS                          32		// absolute limit
#define MAX_CONFIGSTRINGS                    1700 //this is getting pretty high. Try not to raise it anymore than it already is.
#define MAX_CVAR_VALUE_STRING                256
#define MAX_FORCE_RANK                       7
#define MAX_FX                               64 // max effects strings, I'm hoping that 64 will be plenty
#define MAX_G2_COLLISIONS                    16
#define MAX_G2BONES                          64		//rww - changed from MAX_CHARSKINS to MAX_G2BONES. value still equal.
#define MAX_GAMESTATE_CHARS                  16000
#define MAX_GENTITIES                        (1<<GENTITYNUM_BITS)
#define MAX_GLOBAL_SERVERS                   2048
#define MAX_HOSTNAMELENGTH                   256 //22
#define MAX_ICONS                            64 // max registered icons you can have per map
#define MAX_INFO_KEY                         1024
#define MAX_INFO_STRING                      1024
#define MAX_INFO_VALUE                       1024
#define MAX_LOCATIONS                        64
#define MAX_MAP_AREA_BYTES                   32		// bit vector of area visibility
#define MAX_MAPNAMELENGTH                    256 //16
#define MAX_MODELS                           512		// these are sent over the net as -12 bits
#define MAX_NAME_LENGTH                      32		// max length of a client name
#define MAX_NEIGHBOR_FORCEJUMP_LINK_DISTANCE 400
#define MAX_NEIGHBOR_LINK_DISTANCE           128
#define MAX_NEIGHBOR_SIZE                    32
#define MAX_NETNAME                          36
#define MAX_OTHER_SERVERS                    128
#define MAX_PERSISTANT                       16
#define MAX_PINGREQUESTS                     32
#define MAX_POWERUPS                         16
#define MAX_PS_EVENTS                        2
#define MAX_QPATH                            64		// max length of a quake game pathname
#define MAX_RADAR_ENTITIES                   MAX_GENTITIES
#define MAX_SAY_TEXT                         150
#define MAX_SERVERSTATUSREQUESTS             16
#define MAX_SOUNDS                           256		// so they cannot be blindly increased
#define MAX_STATS                            16
#define MAX_STATUSLENGTH                     256 //64
#define MAX_STRING_CHARS                     1024	// max length of a string passed to Cmd_TokenizeString
#define MAX_STRING_TOKENS                    1024	// max tokens resulting from Cmd_TokenizeString
#define MAX_SUB_BSP                          32 //rwwRMG - added
#define MAX_TEAMNAME                         32
#define MAX_TERRAINS                         1 //32 //rwwRMG: inserted
#define MAX_TOKEN_CHARS                      1024	// max length of an individual token
#define MAX_TOKENLENGTH                      1024
#define MAX_WEAPONS                          19
#define MAX_WORLD_COORD                      ( 64 * 1024 )
#define MAX_WPARRAY_SIZE                     4096
#define	MAXLIGHTMAPS                         4
#define MIN_WORLD_COORD                      ( -64 * 1024 )
#define MOVE_RUN                             120			// if forwardmove or rightmove are >= MOVE_RUN, then BUTTON_WALKING should be set
#define NET_ADDRSTRMAXLEN                    48 // maximum length of an IPv6 address string including trailing '\0'
#define NULL_CLIP                            ((clipHandle_t)0)
#define NULL_FILE                            ((fileHandle_t)0)
#define NULL_FX                              ((fxHandle_t)0)
#define NULL_HANDLE                          ((qhandle_t)0)
#define NULL_SFX                             ((sfxHandle_t)0)
#define NULL_SOUND                           ((sfxHandle_t)0)
#define NUM_FORCE_POWER_LEVELS               4
#define OPENJKGAME                           "cleanjoke"
#define PRODUCT_NAME                         "cleanjoke"
#define PROP_BIG_HEIGHT                      24
#define PROP_BIG_SIZE_SCALE                  1
#define PROP_GAP_BIG_WIDTH                   3
#define PROP_GAP_TINY_WIDTH                  1
#define PROP_GAP_WIDTH                       2 // 3
#define PROP_GIANT_SIZE_SCALE                2
#define PROP_HEIGHT                          16
#define PROP_SMALL_SIZE_SCALE                1
#define PROP_SPACE_BIG_WIDTH                 6
#define PROP_SPACE_TINY_WIDTH                3
#define PROP_SPACE_WIDTH                     4
#define PROP_TINY_HEIGHT                     10
#define PROP_TINY_SIZE_SCALE                 1
#define PS_PMOVEFRAMECOUNTBITS               6
#define PULSE_DIVISOR                        75
#define Q3_SCRIPT_DIR                        "scripts"
#define SCREEN_HEIGHT                        480
#define SCREEN_WIDTH                         640
#define SMALLCHAR_HEIGHT                     16
#define SMALLCHAR_WIDTH                      8
#define STYLE_BLINK                          0x40000000
#define STYLE_DROPSHADOW                     0x80000000
#define TINYCHAR_HEIGHT                      (SMALLCHAR_HEIGHT/2)
#define TINYCHAR_WIDTH                       (SMALLCHAR_WIDTH)
#define TRACK_CHANNEL_MAX                    (NUM_TRACK_CHANNELS-50)
#define TRUNCATE_LENGTH                      64
#define WORLD_SIZE                           ( MAX_WORLD_COORD - MIN_WORLD_COORD )

// Defined as a macro here so one change will affect all the relevant files
#define MATERIALS	\
	"none",			\
	"solidwood",	\
	"hollowwood",	\
	"solidmetal",	\
	"hollowmetal",	\
	"shortgrass",	\
	"longgrass",	\
	"dirt",	   		\
	"sand",	   		\
	"gravel",		\
	"glass",		\
	"concrete",		\
	"marble",		\
	"water",		\
	"snow",	   		\
	"ice",			\
	"flesh",		\
	"mud",			\
	"bpglass",		\
	"dryleaves",	\
	"greenleaves",	\
	"fabric",		\
	"canvas",		\
	"rock",			\
	"rubber",		\
	"plastic",		\
	"tiles",		\
	"carpet",		\
	"plaster",		\
	"shatterglass",	\
	"armor",		\
	"computer"/* this was missing, see enums above, plus ShaderEd2 pulldown options */

#define CS_SERVERINFO							0		// an info string with all the serverinfo cvars
#define CS_SYSTEMINFO							1		// an info string for server system to client system configuration (timescale, etc)
#define RESERVED_CONFIGSTRINGS					2	// game can't modify below this, only the system can

#define UI_LEFT       0x00000000 // default
#define UI_CENTER     0x00000001
#define UI_RIGHT      0x00000002
#define UI_DROPSHADOW 0x00000800
#define UI_BLINK      0x00001000
#define UI_INVERSE    0x00002000
#define UI_PULSE      0x00004000

#define Com_Memset memset
#define Com_Memcpy memcpy

#ifdef ERR_FATAL
	#undef ERR_FATAL // this is be defined in malloc.h
#endif



//Ignore __attribute__ on non-gcc platforms
#if !defined(__GNUC__) && !defined(__attribute__)
	#define __attribute__(x)
#endif

#if defined(__GNUC__)
	#define UNUSED_VAR __attribute__((unused))
#else
	#define UNUSED_VAR
#endif

#if (defined _MSC_VER)
	#define Q_EXPORT __declspec(dllexport)
#elif (defined __SUNPRO_C)
	#define Q_EXPORT __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
	#define Q_EXPORT __attribute__((visibility("default")))
#else
	#define Q_EXPORT
#endif

#if (defined(_M_IX86) || defined(__i386__)) && !defined(__sun__)
	#define id386	1
#else
	#define id386	0
#endif

#if (defined(powerc) || defined(powerpc) || defined(ppc) || defined(__ppc) || defined(__ppc__)) && !defined(C_ONLY)
	#define idppc	1
#else
	#define idppc	0
#endif

#ifdef __GNUC__
	#define QALIGN(x) __attribute__((aligned(x)))
#else
	#define QALIGN(x)
#endif



// enums
enum lumps_e {
	LUMP_ENTITIES =     0,
	LUMP_SHADERS =      1,
	LUMP_PLANES =       2,
	LUMP_NODES =        3,
	LUMP_LEAFS =        4,
	LUMP_LEAFSURFACES = 5,
	LUMP_LEAFBRUSHES =  6,
	LUMP_MODELS =       7,
	LUMP_BRUSHES =      8,
	LUMP_BRUSHSIDES =   9,
	LUMP_DRAWVERTS =    10,
	LUMP_DRAWINDEXES =  11,
	LUMP_FOGS =         12,
	LUMP_SURFACES =     13,
	LUMP_LIGHTMAPS =    14,
	LUMP_LIGHTGRID =    15,
	LUMP_VISIBILITY =   16,
	LUMP_LIGHTARRAY =   17,
	HEADER_LUMPS =      18,
};

enum contentsFlags_e : uint32_t {
	CONTENTS_NONE =           (0x00000000u),
	CONTENTS_SOLID =          (0x00000001u), // Default setting. An eye is never valid in a solid
	CONTENTS_LAVA =           (0x00000002u), //
	CONTENTS_WATER =          (0x00000004u), //
	CONTENTS_FOG =            (0x00000008u), //
	CONTENTS_PLAYERCLIP =     (0x00000010u), //
	CONTENTS_MONSTERCLIP =    (0x00000020u), // Physically block bots
	CONTENTS_BOTCLIP =        (0x00000040u), // A hint for bots - do not enter this brush by navigation (if possible)
	CONTENTS_SHOTCLIP =       (0x00000080u), //
	CONTENTS_BODY =           (0x00000100u), // should never be on a brush, only in game
	CONTENTS_CORPSE =         (0x00000200u), // should never be on a brush, only in game
	CONTENTS_TRIGGER =        (0x00000400u), //
	CONTENTS_NODROP =         (0x00000800u), // don't leave bodies or items (death fog, lava)
	CONTENTS_TERRAIN =        (0x00001000u), // volume contains terrain data
	CONTENTS_LADDER =         (0x00002000u), //
	CONTENTS_ABSEIL =         (0x00004000u), // (SOF2) used like ladder to define where an NPC can abseil
	CONTENTS_OPAQUE =         (0x00008000u), // defaults to on, when off, solid can be seen through
	CONTENTS_OUTSIDE =        (0x00010000u), // volume is considered to be in the outside (i.e. not indoors)
	CONTENTS_SLIME =          (0x00020000u), // CHC needs this since we use same tools
	CONTENTS_LIGHTSABER =     (0x00040000u), // ""
	CONTENTS_TELEPORTER =     (0x00080000u), // ""
	CONTENTS_ITEM =           (0x00100000u), // ""
	CONTENTS_NOSHOT =         (0x00200000u), // shots pass through me
	CONTENTS_UNUSED00400000 = (0x00400000u), //
	CONTENTS_UNUSED00800000 = (0x00800000u), //
	CONTENTS_UNUSED01000000 = (0x01000000u), //
	CONTENTS_UNUSED02000000 = (0x02000000u), //
	CONTENTS_UNUSED04000000 = (0x04000000u), //
	CONTENTS_DETAIL =         (0x08000000u), // brushes not used for the bsp
	CONTENTS_INSIDE =         (0x10000000u), // volume is considered to be inside (i.e. indoors)
	CONTENTS_UNUSED20000000 = (0x20000000u), //
	CONTENTS_UNUSED40000000 = (0x40000000u), //
	CONTENTS_TRANSLUCENT =    (0x80000000u), // don't consume surface fragments inside
	CONTENTS_ALL =            (0xFFFFFFFFu),
};

enum surfaceFlags_e : uint32_t {
	SURF_NONE =           (0x00000000u),
	SURF_UNUSED00000001 = (0x00000001u), //
	SURF_UNUSED00000002 = (0x00000002u), //
	SURF_UNUSED00000004 = (0x00000004u), //
	SURF_UNUSED00000008 = (0x00000008u), //
	SURF_UNUSED00000010 = (0x00000010u), //
	SURF_UNUSED00000020 = (0x00000020u), //
	SURF_UNUSED00000040 = (0x00000040u), //
	SURF_UNUSED00000080 = (0x00000080u), //
	SURF_UNUSED00000100 = (0x00000100u), //
	SURF_UNUSED00000200 = (0x00000200u), //
	SURF_UNUSED00000400 = (0x00000400u), //
	SURF_UNUSED00000800 = (0x00000800u), //
	SURF_UNUSED00001000 = (0x00001000u), //
	SURF_SKY =            (0x00002000u), // lighting from environment map
	SURF_SLICK =          (0x00004000u), // affects game physics
	SURF_METALSTEPS =     (0x00008000u), // CHC needs this since we use same tools (though this flag is temp?)
	SURF_FORCEFIELD =     (0x00010000u), // CHC ""			(but not temp)
	SURF_UNUSED00020000 = (0x00020000u), //
	SURF_NODAMAGE =       (0x00040000u), // never give falling damage
	SURF_NOIMPACT =       (0x00080000u), // don't make missile explosions
	SURF_NOMARKS =        (0x00100000u), // don't leave missile marks
	SURF_NODRAW =         (0x00200000u), // don't generate a drawsurface at all
	SURF_NOSTEPS =        (0x00400000u), // no footstep sounds
	SURF_NODLIGHT =       (0x00800000u), // don't dlight even if solid (solid lava, skies)
	SURF_NOMISCENTS =     (0x01000000u), // no client models allowed on this surface
	SURF_FORCESIGHT =     (0x02000000u), // not visible without Force Sight
	SURF_UNUSED04000000 = (0x04000000u), //
	SURF_UNUSED08000000 = (0x08000000u), //
	SURF_UNUSED10000000 = (0x10000000u), //
	SURF_UNUSED20000000 = (0x20000000u), //
	SURF_UNUSED40000000 = (0x40000000u), //
	SURF_UNUSED80000000 = (0x80000000u), //
};

enum surfaceMaterial_e {
	MATERIAL_NONE, // for when the artist hasn't set anything up =)
	MATERIAL_SOLIDWOOD, // freshly cut timber
	MATERIAL_HOLLOWWOOD, // termite infested creaky wood
	MATERIAL_SOLIDMETAL, // solid girders
	MATERIAL_HOLLOWMETAL, // hollow metal machines
	MATERIAL_SHORTGRASS, // manicured lawn
	MATERIAL_LONGGRASS, // long jungle grass
	MATERIAL_DIRT, // hard mud
	MATERIAL_SAND, // sandy beach
	MATERIAL_GRAVEL, // lots of small stones
	MATERIAL_GLASS, //
	MATERIAL_CONCRETE, // hardened concrete pavement
	MATERIAL_MARBLE, // marble floors
	MATERIAL_WATER, // light covering of water on a surface
	MATERIAL_SNOW, // freshly laid snow
	MATERIAL_ICE, // packed snow/solid ice
	MATERIAL_FLESH, // hung meat, corpses in the world
	MATERIAL_MUD, // wet soil
	MATERIAL_BPGLASS, // bulletproof glass
	MATERIAL_DRYLEAVES, // dried up leaves on the floor
	MATERIAL_GREENLEAVES, // fresh leaves still on a tree
	MATERIAL_FABRIC, // Cotton sheets
	MATERIAL_CANVAS, // tent material
	MATERIAL_ROCK, //
	MATERIAL_RUBBER, // hard tire like rubber
	MATERIAL_PLASTIC, //
	MATERIAL_TILES, // tiled floor
	MATERIAL_CARPET, // lush carpet
	MATERIAL_PLASTER, // drywall style plaster
	MATERIAL_SHATTERGLASS, // glass with the Crisis Zone style shattering
	MATERIAL_ARMOR, // body armor
	MATERIAL_COMPUTER, // computers/electronic equipment
	MATERIAL_LAST, // number of materials
};

enum serverSource_e {
	AS_LOCAL,
	AS_GLOBAL,
	AS_FAVORITES,
};

enum snapshotFlag_e : uint32_t {
	SNAPFLAG_RATE_DELAYED = 0x01,
	SNAPFLAG_NOT_ACTIVE =   0x02, // snapshot used during connection and for zombies
	SNAPFLAG_SERVERCOUNT =  0x04, // toggled every map_restart so transitions can be detected
};

enum cinematicFlag_e : uint32_t {
	CIN_system = 1,
	CIN_loop =   2,
	CIN_hold =   4,
	CIN_silent = 8,
	CIN_shader = 16,
};

enum fsMode_e {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
};

enum fsOrigin_e {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
};

enum cvarFlag_e : uint32_t {
	CVAR_NONE =           0x00000000u,
	CVAR_ARCHIVE =        0x00000001u, // set to cause it to be saved to configuration file. used for system variables, not for player specific configurations
	CVAR_USERINFO =       0x00000002u, // sent to server on connect or change
	CVAR_SERVERINFO =     0x00000004u, // sent in response to front end requests
	CVAR_SYSTEMINFO =     0x00000008u, // these cvars will be duplicated on all clients
	CVAR_INIT =           0x00000010u, // don't allow change from console at all, but can be set from the command line
	CVAR_LATCH =          0x00000020u, // will only change when C code next does a Cvar_Get(), so it can't be changed without proper initialization. modified will be set, even though the value hasn't changed yet
	CVAR_ROM =            0x00000040u, // display only, cannot be set by user at all (can be set by code)
	CVAR_USER_CREATED =   0x00000080u, // created by a set command
	CVAR_TEMP =           0x00000100u, // can be set even when cheats are disabled, but is not archived
	CVAR_CHEAT =          0x00000200u, // can not be changed if cheats are disabled
	CVAR_NORESTART =      0x00000400u, // do not clear when a cvar_restart is issued
	CVAR_INTERNAL =       0x00000800u, // cvar won't be displayed, ever (for passwords and such)
	CVAR_PARENTAL =       0x00001000u, // lets cvar system know that parental stuff needs to be updated
	CVAR_SERVER_CREATED = 0x00002000u, // cvar was created by a server the client connected to.
	CVAR_VM_CREATED =     0x00004000u, // cvar was created exclusively in one of the VMs.
	CVAR_PROTECTED =      0x00008000u, // prevent modifying this var from VMs or the server
	CVAR_NODEFAULT =      0x00010000u, // do not write to config if matching with default value
	CVAR_MODIFIED =       0x40000000u,	// Cvar was modified
	CVAR_NONEXISTENT =    0x80000000u,	// Cvar doesn't exist.
	CVAR_ARCHIVE_ND =     CVAR_ARCHIVE | CVAR_NODEFAULT,
};

enum cbufExec_e {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this, because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
};

//For system-wide prints
enum WL_e {
	WL_ERROR=1,
	WL_WARNING,
	WL_VERBOSE,
	WL_DEBUG
};

// print levels from renderer (FIXME: set up for game / cgame?)
enum printParm_e {
	PRINT_ALL,
	PRINT_DEVELOPER,		// only print when "developer 1"
	PRINT_WARNING,
	PRINT_ERROR
};

// parameters to the main Error routine
enum errorParm_e {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
};

enum ha_pref_e {
	h_high,
	h_low,
	h_dontcare
};

enum saberBlockType_e : int32_t {
	BLK_NO,
	BLK_TIGHT,		// Block only attacks and shots around the saber itself, a bbox of around 12x12x12
	BLK_WIDE		// Block all attacks in an area around the player in a rough arc of 180 degrees
};

enum saberBlockedType_e : int32_t {
	BLOCKED_NONE,
	BLOCKED_BOUNCE_MOVE,
	BLOCKED_PARRY_BROKEN,
	BLOCKED_ATK_BOUNCE,
	BLOCKED_UPPER_RIGHT,
	BLOCKED_UPPER_LEFT,
	BLOCKED_LOWER_RIGHT,
	BLOCKED_LOWER_LEFT,
	BLOCKED_TOP,
	BLOCKED_UPPER_RIGHT_PROJ,
	BLOCKED_UPPER_LEFT_PROJ,
	BLOCKED_LOWER_RIGHT_PROJ,
	BLOCKED_LOWER_LEFT_PROJ,
	BLOCKED_TOP_PROJ
};

enum saber_colors_e : int32_t {
	SABER_RED,
	SABER_ORANGE,
	SABER_YELLOW,
	SABER_GREEN,
	SABER_BLUE,
	SABER_PURPLE,
	NUM_SABER_COLORS
};

enum forcePowers_e : int32_t {
	FP_INVALID = -1,
	FP_FIRST = 0,//marker
	FP_HEAL = 0,//instant
	FP_LEVITATION,//hold/duration
	FP_SPEED,//duration
	FP_PUSH,//hold/duration
	FP_PULL,//hold/duration
	FP_TELEPATHY,//instant
	FP_GRIP,//hold/duration
	FP_LIGHTNING,//hold/duration
	FP_RAGE,//duration
	FP_PROTECT,
	FP_ABSORB,
	FP_TEAM_HEAL,
	FP_TEAM_FORCE,
	FP_DRAIN,
	FP_SEE,
	FP_SABER_OFFENSE,
	FP_SABER_DEFENSE,
	FP_SABERTHROW,
	NUM_FORCE_POWERS
};

enum forcePowerLevels_e : int32_t {
	FORCE_LEVEL_0,
	FORCE_LEVEL_1,
	FORCE_LEVEL_2,
	FORCE_LEVEL_3,
	FORCE_LEVEL_4,
	FORCE_LEVEL_5,
};

enum sharedERagPhase_e {
	RP_START_DEATH_ANIM,
	RP_END_DEATH_ANIM,
	RP_DEATH_COLLISION,
	RP_CORPSE_SHOT,
	RP_GET_PELVIS_OFFSET,  // this actually does nothing but set the pelvisAnglesOffset, and pelvisPositionOffset
	RP_SET_PELVIS_OFFSET,  // this actually does nothing but set the pelvisAnglesOffset, and pelvisPositionOffset
	RP_DISABLE_EFFECTORS  // this removes effectors given by the effectorsToTurnOff member
};

enum sharedERagEffector_e {
	RE_MODEL_ROOT =   0x00000001, //"model_root"
	RE_PELVIS =       0x00000002, //"pelvis"
	RE_LOWER_LUMBAR = 0x00000004, //"lower_lumbar"
	RE_UPPER_LUMBAR = 0x00000008, //"upper_lumbar"
	RE_THORACIC =     0x00000010, //"thoracic"
	RE_CRANIUM =      0x00000020, //"cranium"
	RE_RHUMEROUS =    0x00000040, //"rhumerus"
	RE_LHUMEROUS =    0x00000080, //"lhumerus"
	RE_RRADIUS =      0x00000100, //"rradius"
	RE_LRADIUS =      0x00000200, //"lradius"
	RE_RFEMURYZ =     0x00000400, //"rfemurYZ"
	RE_LFEMURYZ =     0x00000800, //"lfemurYZ"
	RE_RTIBIA =       0x00001000, //"rtibia"
	RE_LTIBIA =       0x00002000, //"ltibia"
	RE_RHAND =        0x00004000, //"rhand"
	RE_LHAND =        0x00008000, //"lhand"
	RE_RTARSAL =      0x00010000, //"rtarsal"
	RE_LTARSAL =      0x00020000, //"ltarsal"
	RE_RTALUS =       0x00040000, //"rtalus"
	RE_LTALUS =       0x00080000, //"ltalus"
	RE_RRADIUSX =     0x00100000, //"rradiusX"
	RE_LRADIUSX =     0x00200000, //"lradiusX"
	RE_RFEMURX =      0x00400000, //"rfemurX"
	RE_LFEMURX =      0x00800000, //"lfemurX"
	RE_CEYEBROW =     0x01000000 //"ceyebrow"
};

enum sharedEIKMoveState_e {
	IKS_NONE = 0,
	IKS_DYNAMIC
};

//material stuff needs to be shared
enum chunkMaterial_e : int32_t {
	MAT_METAL = 0,	// scorched blue-grey metal
	MAT_GLASS,		// not a real chunk type, just plays an effect with glass sprites
	MAT_ELECTRICAL,	// sparks only
	MAT_ELEC_METAL,	// sparks/electrical type metal
	MAT_DRK_STONE,	// brown
	MAT_LT_STONE,	// tan
	MAT_GLASS_METAL,// glass sprites and METAl chunk
	MAT_METAL2,		// electrical metal type
	MAT_NONE,		// no chunks
	MAT_GREY_STONE,	// grey
	MAT_METAL3,		// METAL and METAL2 chunks
	MAT_CRATE1,		// yellow multi-colored crate chunks
	MAT_GRATE1,		// grate chunks
	MAT_ROPE,		// for yavin trial...no chunks, just wispy bits
	MAT_CRATE2,		// read multi-colored crate chunks
	MAT_WHITE_METAL,// white angular chunks
	MAT_SNOWY_ROCK,	// gray & brown chunks

	NUM_MATERIALS
};

enum tokenType_e {
	TT_STRING =      1, // string
	TT_LITERAL =     2, // literal
	TT_NUMBER =      3, // number
	TT_NAME =        4, // name
	TT_PUNCTUATION = 5, // punctuation
};

enum keyCatcher_e : uint32_t {
	KEYCATCH_CONSOLE = 0x0001,
	KEYCATCH_UI =      0x0002,
	KEYCATCH_MESSAGE = 0x0004,
	KEYCATCH_CGAME =   0x0008,
};

enum soundChannel_e {
	CHAN_AUTO,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # Auto-picks an empty channel to play sound on
	CHAN_LOCAL,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # menu sounds, etc
	CHAN_WEAPON,//## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_VOICE, //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Voice sounds cause mouth animation
	CHAN_VOICE_ATTEN, //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Causes mouth animation but still use normal sound falloff
	CHAN_ITEM,  //## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_BODY,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_AMBIENT,//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # added for ambient sounds
	CHAN_LOCAL_SOUND,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #chat messages, etc
	CHAN_ANNOUNCER,		//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #announcer voices, etc
	CHAN_LESS_ATTEN,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #attenuates similar to chan_voice, but uses empty channel auto-pick behaviour
	CHAN_MENU1,		//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #menu stuff, etc
	CHAN_VOICE_GLOBAL,  //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Causes mouth animation and is broadcast, like announcer
	CHAN_MUSIC,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #music played as a looping sound - added by BTO (VV)
};

enum trackchan_e {
	TRACK_CHANNEL_NONE = 50,
	TRACK_CHANNEL_1,
	TRACK_CHANNEL_2, // force speed
	TRACK_CHANNEL_3, // force rage
	TRACK_CHANNEL_4,
	TRACK_CHANNEL_5, // force sight
	NUM_TRACK_CHANNELS
};

enum itemUseFail_e {
	SENTRY_NOROOM = 1,
	SENTRY_ALREADYPLACED,
	SHIELD_NOROOM,
	SEEKER_ALREADYDEPLOYED
};

enum genButtons_e : uint32_t {
	BUTTON_ATTACK =          1,
	BUTTON_TALK =            2,		// displays talk balloon and disables actions
	BUTTON_USE_HOLDABLE =    4,
	BUTTON_GESTURE =         8,
	BUTTON_WALKING =         16,	// walking can't just be infered from MOVE_RUN because a key pressed late in the frame will only generate a small move value for that frame. walking will use different animations and won't generate footsteps
	BUTTON_USE =             32,	// the ol' use key returns!
	BUTTON_FORCEGRIP =       64,
	BUTTON_ALT_ATTACK =      128,
	BUTTON_ANY =             256,	// any key whatsoever
	BUTTON_FORCEPOWER =      512,	// use the "active" force power
	BUTTON_FORCE_LIGHTNING = 1024,
	BUTTON_FORCE_DRAIN =     2048,
};

enum genCmds_e {
	GENCMD_SABERSWITCH = 1,
	GENCMD_ENGAGE_DUEL,
	GENCMD_FORCE_HEAL,
	GENCMD_FORCE_SPEED,
	GENCMD_FORCE_THROW,
	GENCMD_FORCE_PULL,
	GENCMD_FORCE_DISTRACT,
	GENCMD_FORCE_RAGE,
	GENCMD_FORCE_PROTECT,
	GENCMD_FORCE_ABSORB,
	GENCMD_FORCE_HEALOTHER,
	GENCMD_FORCE_FORCEPOWEROTHER,
	GENCMD_FORCE_SEEING,
	GENCMD_USE_SEEKER,
	GENCMD_USE_FIELD,
	GENCMD_USE_BACTA,
	GENCMD_USE_ELECTROBINOCULARS,
	GENCMD_ZOOM,
	GENCMD_USE_SENTRY,
	GENCMD_USE_JETPACK,
	GENCMD_USE_BACTABIG,
	GENCMD_USE_HEALTHDISP,
	GENCMD_USE_AMMODISP,
	GENCMD_USE_EWEB,
	GENCMD_USE_CLOAK,
	GENCMD_SABERATTACKCYCLE,
	GENCMD_TAUNT,
	GENCMD_BOW,
	GENCMD_MEDITATE,
	GENCMD_FLOURISH,
	GENCMD_GLOAT
};

enum solid_e {
	SOLID_NOT,					// no interaction with other objects
	SOLID_TRIGGER,				// only touch when inside, after moving
	SOLID_BBOX,					// touch on edge
	SOLID_BSP,					// bsp clip, touch on edge
	SOLID_BMODEL	= 0xffffff,	// if entityState->solid == SOLID_BMODEL, modelindex is an inline model number
};

enum trType_e {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_NONLINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY
};

enum connstate_e {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key
	CA_CONNECTING,		// sending request packets to the server
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
};

enum status_e {
	FMV_IDLE,
	FMV_PLAY,		// play
	FMV_EOF,		// all other conditions, i.e. stop/EOF/abort
	FMV_ID_BLT,
	FMV_ID_IDLE,
	FMV_LOOPED,
	FMV_ID_WAIT
};

enum sayType_e {
	SAY_ALL,
	SAY_TEAM,
	SAY_TELL,
};

enum Eorientations_e {
	ORIGIN = 0,
	POSITIVE_X,
	POSITIVE_Z,
	POSITIVE_Y,
	NEGATIVE_X,
	NEGATIVE_Z,
	NEGATIVE_Y
};

enum memtag_e {
	#include "qcommon/tags.hpp"
};

enum ForceReload_e {
	eForceReload_NOTHING,
	eForceReload_MODELS,
	eForceReload_ALL
};

enum fontSize_e {
	FONT_NONE,
	FONT_SMALL=1,
	FONT_MEDIUM,
	FONT_LARGE,
	FONT_SMALL2
};

enum mapSurfaceType_e {
   MST_BAD,
   MST_PLANAR,
   MST_PATCH,
   MST_TRIANGLE_SOUP,
   MST_FLARE
};



// structs
struct sharedRagDollParams_t {
	vec3_t angles;
	vec3_t position;
	vec3_t scale;
	vec3_t pelvisAnglesOffset;    // always set on return, an argument for RP_SET_PELVIS_OFFSET
	vec3_t pelvisPositionOffset; // always set on return, an argument for RP_SET_PELVIS_OFFSET
	float  fImpactStrength; //should be applicable when RagPhase is RP_DEATH_COLLISION
	float  fShotStrength; //should be applicable for setting velocity of corpse on shot (probably only on RP_CORPSE_SHOT)
	int    me; //index of entity giving this update
	int    startFrame;
	int    endFrame;
	int    collisionType; // 1 = from a fall, 0 from effectors, this will be going away soon, hence no enum
	bool   CallRagDollBegin; // a return value, means that we are now begininng ragdoll and the NPC stuff needs to happen
	int    RagPhase;
	int    effectorsToTurnOff;  // set this to an | of the above flags for a RP_DISABLE_EFFECTORS
};

//And one for updating during model animation.
struct sharedRagDollUpdateParams_t {
	vec3_t angles;
	vec3_t position;
	vec3_t scale;
	vec3_t velocity;
	int    me;
	int    settleFrame;
};

//rww - update parms for ik bone stuff
struct sharedIKMoveParams_t {
	char   boneName[512]; //name of bone
	vec3_t desiredOrigin; //world coordinate that this bone should be attempting to reach
	vec3_t origin; //world coordinate of the entity who owns the g2 instance that owns the bone
	float  movementSpeed; //how fast the bone should move toward the destination
};

struct sharedSetBoneIKStateParams_t {
	vec3_t pcjMins; //ik joint limit
	vec3_t pcjMaxs; //ik joint limit
	vec3_t origin; //origin of caller
	vec3_t angles; //angles of caller
	vec3_t scale; //scale of caller
	float  radius; //bone rad
	int    blendTime; //bone blend time
	int    pcjOverrides; //override ik bone flags
	int    startFrame; //base pose start
	int    endFrame; //base pose end
	bool   forceAnimOnBone; //normally if the bone has specified start/end frames already it will leave it alone.. if this is true, then the animation will be restarted on the bone with the specified frames anyway.
};

struct wpneighbor_t {
	int num;
	int forceJumpTo;
};

struct wpobject_t {
	vec3_t       origin;
	int          inuse;
	int          index;
	float        weight;
	float        disttonext;
	int          flags;
	int          associated_entity;
	int          forceJumpTo;
	int          neighbornum;
	wpneighbor_t neighbors[MAX_NEIGHBOR_SIZE];
};

struct pc_token_t {
	int   type;
	int   subtype;
	int   intvalue;
	float floatvalue;
	char  string[MAX_TOKENLENGTH];
};

struct qint64_t {
	byte b0;
	byte b1;
	byte b2;
	byte b3;
	byte b4;
	byte b5;
	byte b6;
	byte b7;
};

//CJKTODO: modules should access cvar_t* directly
struct cvar_t {
	char     *name;
	char     *description;
	char     *string;
	char     *resetString; // cvar_restart will reset to this value
	char     *latchedString; // for CVAR_LATCH vars
	uint32_t  flags;
	bool      modified; // set each time the cvar is changed
	int       modificationCount; // incremented each time the cvar is changed
	float     value; // atof( string )
	int       integer; // atoi( string )
	bool      validate;
	bool      integral;
	float     min, max;
	cvar_t   *next, *prev;
	cvar_t   *hashNext, *hashPrev;
	int       hashIndex;
};

// the modules that run in the virtual machine can't access the cvar_t directly,
// so they must ask for structured updates
struct vmCvar_t {
	cvarHandle_t handle;
	int          modificationCount;
	float        value;
	int          integer;
	char         string[MAX_CVAR_VALUE_STRING];
};

struct CollisionRecord_t {
	float  mDistance;
	int    mEntityNum;
	int    mModelIndex;
	int    mPolyIndex;
	int    mSurfaceIndex;
	vec3_t mCollisionPosition;
	vec3_t mCollisionNormal;
	int    mFlags;
	int    mMaterial;
	int    mLocation;
	float  mBarycentricI; // two barycentic coodinates for the hit point
	float  mBarycentricJ; // K = 1-I-J
};

struct trace_t {
	byte     allsolid; // if true, plane is not valid
	byte     startsolid; // if true, the initial point was in a solid area
	short    entityNum; // entity the contacted sirface is a part of
	float    fraction; // time completed, 1.0 = didn't hit anything
	vec3_t   endpos; // final position
	cplane_t plane; // surface normal at impact, transformed to world space
	int      surfaceFlags; // surface hit
	int      contents; // contents on other side of surface hit
};

struct markFragment_t {
	int firstPoint;
	int numPoints;
};

struct orientation_t {
	vec3_t    origin;
	matrix3_t axis;
};

struct gameState_t {
	int  stringOffsets[MAX_CONFIGSTRINGS];
	char stringData[MAX_GAMESTATE_CHARS];
	int  dataCount;
};

struct forcedata_t {
	int           forcePowerDebounce[NUM_FORCE_POWERS];	//for effects that must have an interval
	int           forcePowersKnown;
	int           forcePowersActive;
	forcePowers_e forcePowerSelected;
	int           forceButtonNeedRelease;
	int           forcePowerDuration[NUM_FORCE_POWERS];
	int           forcePower;
	int           forcePowerMax;
	int           forcePowerRegenDebounceTime;
	int           forcePowerLevel[NUM_FORCE_POWERS];		//so we know the max forceJump power you have
	int           forcePowerBaseLevel[NUM_FORCE_POWERS];
	int           forceUsingAdded;
	float         forceJumpZStart;					//So when you land, you don't get hurt as much
	float         forceJumpCharge;					//you're current forceJump charge-up level, increases the longer you hold the force jump button down
	int           forceJumpSound;
	int           forceJumpAddTime;
	int           forceGripEntityNum;					//what entity I'm gripping
	int           forceGripDamageDebounceTime;		//debounce for grip damage
	float         forceGripBeingGripped;				//if > level.time then client is in someone's grip
	int           forceGripCripple;					//if != 0 then make it so this client can't move quickly (he's being gripped)
	int           forceGripUseTime;					//can't use if > level.time
	float         forceGripSoundTime;
	float         forceGripStarted;					//level.time when the grip was activated
	int           forceHealTime;
	int           forceHealAmount;
	int           forceMindtrickTargetIndex; //0-15
	int           forceMindtrickTargetIndex2; //16-32
	int           forceMindtrickTargetIndex3; //33-48
	int           forceMindtrickTargetIndex4; //49-64
	int           forceRageRecoveryTime;
	int           forceDrainEntNum;
	float         forceDrainTime;
	int           forceDoInit;
	int           forceSide;
	int           forceRank;
	int           forceDeactivateAll;
	int           killSoundEntIndex[TRACK_CHANNEL_MAX]; //this goes here so it doesn't get wiped over respawn
	bool          sentryDeployed;
	int           saberAnimLevelBase;//sigh...
	int           saberAnimLevel;
	int           saberDrawAnimLevel;
	int           suicides;
	int           privateDuelTime;
};

struct playerState_t {
	int         commandTime;	// cmd->serverTime of last executed command
	int         pm_type;
	int         bobCycle;		// for view bobbing and footstep generation
	int         pm_flags;		// ducked, jump_held, etc
	int         pm_time;
	vec3_t      origin;
	vec3_t      velocity;
	vec3_t      moveDir; //NOT sent over the net - nor should it be.
	int         weaponTime;
	int         weaponChargeTime;
	int         weaponChargeSubtractTime;
	int         gravity;
	float       speed;
	int         basespeed; //used in prediction to know base server g_speed value when modifying speed between updates
	int         delta_angles[3];	// add to command angles to get view direction. changed by spawns, rotating objects, and teleporters
	int         slopeRecalcTime; //this is NOT sent across the net and is maintained seperately on game and cgame in pmove code.
	int         useTime;
	int         groundEntityNum;// ENTITYNUM_NONE = in air
	int         legsTimer;		// don't change low priority animations until this runs out
	int         legsAnim;
	int         torsoTimer;		// don't change low priority animations until this runs out
	int         torsoAnim;
	bool        legsFlip; //set to opposite when the same anim needs restarting, sent over in only 1 bit. Cleaner and makes porting easier than having that god forsaken ANIM_TOGGLEBIT.
	bool        torsoFlip;
	int         movementDir;	// a number 0 to 7 that represents the reletive angle of movement to the view angle (axial and diagonals). when at rest, the value will remain unchanged. used to twist the legs during strafing
	int         eFlags;			// copied to entityState_t->eFlags
	int         eFlags2;		// copied to entityState_t->eFlags2, EF2_??? used much less frequently
	int         eventSequence;	// pmove generated events
	int         events[MAX_PS_EVENTS];
	int         eventParms[MAX_PS_EVENTS];
	int         externalEvent;	// events set on player from another source
	int         externalEventParm;
	int         externalEventTime;
	int         clientNum;		// ranges from 0 to MAX_CLIENTS-1
	int         weapon;			// copied to entityState_t->weapon
	int         weaponstate;
	vec3_t      viewangles;		// for fixed views
	int         viewheight;
	int         damageEvent;	// when it changes, latch the other parms
	int         damageYaw;
	int         damagePitch;
	int         damageCount;
	int         damageType;
	int         painTime;		// used for both game and client side to process the pain twitch - NOT sent across the network
	int         painDirection;	// NOT sent across the network
	float       yawAngle;		// NOT sent across the network
	bool        yawing;			// NOT sent across the network
	float       pitchAngle;		// NOT sent across the network
	bool        pitching;		// NOT sent across the network
	int         stats[MAX_STATS];
	int         persistant[MAX_PERSISTANT];	// stats that aren't cleared on death
	int         powerups[MAX_POWERUPS];	// level.time that the powerup runs out
	int         ammo[MAX_AMMO];
	int         generic1;
	int         loopSound;
	int         jumppad_ent;	// jumppad entity hit this frame
	int         ping;			// server to game info for scoreboard
	int         pmove_framecount;	// FIXME: don't transmit over the network
	int         jumppad_frame;
	int         entityEventSequence;
	int         lastOnGround;	//last time you were on the ground
	bool        saberInFlight;
	int         saberMove;
	int         saberBlocking;
	int         saberBlocked;
	int         saberLockTime;
	int         saberLockEnemy;
	int         saberLockFrame; //since we don't actually have the ability to get the current anim frame
	int         saberLockHits; //every x number of buttons hits, allow one push forward in a saber lock (server only)
	int         saberLockHitCheckTime; //so we don't allow more than 1 push per server frame
	int         saberLockHitIncrementTime; //so we don't add a hit per attack button press more than once per server frame
	bool        saberLockAdvance; //do an advance (sent across net as 1 bit)
	int         saberEntityNum;
	float       saberEntityDist;
	int         saberEntityState;
	int         saberThrowDelay;
	bool        saberCanThrow;
	int         saberDidThrowTime;
	int         saberDamageDebounceTime;
	int         saberHitWallSoundDebounceTime;
	int         saberEventFlags;
	int         rocketLockIndex;
	float       rocketLastValidTime;
	float       rocketLockTime;
	float       rocketTargetTime;
	int         emplacedIndex;
	float       emplacedTime;
	bool        isJediMaster;
	bool        forceRestricted;
	bool        trueJedi;
	bool        trueNonJedi;
	int         saberIndex;
	int         genericEnemyIndex;
	float       droneFireTime;
	float       droneExistTime;
	int         activeForcePass;
	bool        hasDetPackPlanted; //better than taking up an eFlag isn't it?
	float       holocronsCarried[NUM_FORCE_POWERS];
	int         holocronCantTouch;
	float       holocronCantTouchTime; //for keeping track of the last holocron that just popped out of me (if any)
	int         holocronBits;
	int         electrifyTime;
	int         saberAttackSequence;
	int         saberIdleWound;
	int         saberAttackWound;
	int         saberBlockTime;
	int         otherKiller;
	int         otherKillerTime;
	int         otherKillerDebounceTime;
	forcedata_t fd;
	bool        forceJumpFlip;
	int         forceHandExtend;
	int         forceHandExtendTime;
	int         forceRageDrainTime;
	int         forceDodgeAnim;
	bool        quickerGetup;
	int         groundTime;		// time when first left ground
	int         footstepTime;
	int         otherSoundTime;
	float       otherSoundLen;
	int         forceGripMoveInterval;
	int         forceGripChangeMovetype;
	int         forceKickFlip;
	int         duelIndex;
	int         duelTime;
	bool        duelInProgress;
	int         saberAttackChainCount;
	int         saberHolstered;
	int         forceAllowDeactivateTime;
	int         zoomMode;		// 0 - not zoomed, 1 - disruptor weapon
	int         zoomTime;
	bool        zoomLocked;
	float       zoomFov;
	int         zoomLockTime;
	int         fallingToDeath;
	int         useDelay;
	bool        inAirAnim;
	vec3_t      lastHitLoc;
	int         heldByClient; //can only be a client index - this client should be holding onto my arm using IK stuff.
	int         ragAttach; //attach to ent while ragging
	int         iModelScale;
	int         brokenLimbs;
	bool        hasLookTarget;
	int         lookTarget;
	int         customRGBA[4];
	int         standheight;
	int         crouchheight;
	int         hyperSpaceTime;
	vec3_t      hyperSpaceAngles;
	int         hackingTime;
	int         hackingBaseTime;
	int         jetpackFuel;
	int         cloakFuel;
	int         userInt1;
	int         userInt2;
	int         userInt3;
	float       userFloat1;
	float       userFloat2;
	float       userFloat3;
	vec3_t      userVec1;
	vec3_t      userVec2;
#ifdef _ONEBIT_COMBO
	int         deltaOneBits;
	int         deltaNumBits;
#endif
};

struct usercmd_t {
	int         serverTime;
	int         angles[3];
	int         buttons;
	byte        weapon; // weapon
	byte        forcesel;
	byte        invensel;
	byte        generic_cmd;
	signed char forwardmove, rightmove, upmove;
};

struct addpolyArgStruct_t {
	vec3_t    p[4];
	vec2_t    ev[4];
	int       numVerts;
	vec3_t    vel;
	vec3_t    accel;
	float     alpha1;
	float     alpha2;
	float     alphaParm;
	vec3_t    rgb1;
	vec3_t    rgb2;
	float     rgbParm;
	vec3_t    rotationDelta;
	float     bounce;
	int       motionDelay;
	int       killTime;
	qhandle_t shader;
	int       flags;
};

struct addbezierArgStruct_t {
	vec3_t    start;
	vec3_t    end;
	vec3_t    control1;
	vec3_t    control1Vel;
	vec3_t    control2;
	vec3_t    control2Vel;
	float     size1;
	float     size2;
	float     sizeParm;
	float     alpha1;
	float     alpha2;
	float     alphaParm;
	vec3_t    sRGB;
	vec3_t    eRGB;
	float     rgbParm;
	int       killTime;
	qhandle_t shader;
	int       flags;
};

struct addspriteArgStruct_t {
	vec3_t    origin;
	vec3_t    vel;
	vec3_t    accel;
	float     scale;
	float     dscale;
	float     sAlpha;
	float     eAlpha;
	float     rotation;
	float     bounce;
	int       life;
	qhandle_t shader;
	int       flags;
};

struct effectTrailVertStruct_t {
	vec3_t origin;
	// very specifc case, we can modulate the color and the alpha
	vec3_t rgb;
	vec3_t destrgb;
	vec3_t curRGB;
	float  alpha;
	float  destAlpha;
	float  curAlpha;
	// this is a very specific case thing...allow interpolating the st coords so we can map the texture properly as this
	//	segement progresses through it's life
	float  ST[2];
	float  destST[2];
	float  curST[2];
};

struct effectTrailArgStruct_t {
	effectTrailVertStruct_t mVerts[4];
	qhandle_t               mShader;
	int                     mSetFlags;
	int                     mKillTime;
};

struct addElectricityArgStruct_t {
	vec3_t    start;
	vec3_t    end;
	float     size1;
	float     size2;
	float     sizeParm;
	float     alpha1;
	float     alpha2;
	float     alphaParm;
	vec3_t    sRGB;
	vec3_t    eRGB;
	float     rgbParm;
	float     chaos;
	int       killTime;
	qhandle_t shader;
	int       flags;
};

struct trajectory_t {
	trType_e trType;
	int      trTime;
	int      trDuration; // if non 0, trTime + trDuration = stop time
	vec3_t   trBase;
	vec3_t   trDelta; // velocity, etc
};

struct entityState_t {
	int          number;			// entity index
	int          eType;			// entityType_e
	int          eFlags;
	int          eFlags2;		// EF2_??? used much less frequently
	trajectory_t pos;	// for calculating position
	trajectory_t apos;	// for calculating angles
	int          time;
	int          time2;
	vec3_t       origin;
	vec3_t       origin2;
	vec3_t       angles;
	vec3_t       angles2;
	int          bolt1;
	int          bolt2;
	int          trickedentindex; //0-15
	int          trickedentindex2; //16-32
	int          trickedentindex3; //33-48
	int          trickedentindex4; //49-64
	float        speed;
	int          fireflag;
	int          genericenemyindex;
	int          activeForcePass;
	int          emplacedOwner;
	int          otherEntityNum;	// shotgun sources, etc
	int          otherEntityNum2;
	int          groundEntityNum;	// ENTITYNUM_NONE = in air
	int          constantLight;	// r + (g<<8) + (b<<16) + (intensity<<24)
	int          loopSound;		// constantly loop this sound
	bool         loopIsSoundset; //true if the loopSound index is actually a soundset index
	int          soundSetIndex;
	int          modelGhoul2;
	int          g2radius;
	int          modelindex;
	int          modelindex2;
	int          clientNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
	int          frame;
	bool         saberInFlight;
	int          saberEntityNum;
	int          saberMove;
	int          forcePowersActive;
	int          saberHolstered;//sent in only only 2 bits - should be 0, 1 or 2
	bool         isJediMaster;
	bool         isPortalEnt; //this needs to be seperate for all entities I guess, which is why I couldn't reuse another value.
	int          solid;			// for client side prediction, trap_linkentity sets this properly
	int          event;			// impulse events -- muzzle flashes, footsteps, etc
	int          eventParm;
	int          owner;
	int          teamowner;
	bool         shouldtarget;
	int          powerups;		// bit flags
	int          weapon;			// determines weapon and flash model, etc
	int          legsAnim;
	int          torsoAnim;
	bool         legsFlip; //set to opposite when the same anim needs restarting, sent over in only 1 bit. Cleaner and makes porting easier than having that god forsaken ANIM_TOGGLEBIT.
	bool         torsoFlip;
	int          forceFrame;		//if non-zero, force the anim frame
	int          generic1;
	int          heldByClient; //can only be a client index - this client should be holding onto my arm using IK stuff.
	int          ragAttach; //attach to ent while ragging
	int          iModelScale; //rww - transfer a percentage of the normal scale in a single int instead of 3 x-y-z scale values
	int          brokenLimbs;
	int          boltToPlayer; //set to index of a real client+1 to bolt the ent to that client. Must be a real client, NOT an NPC.
	bool         hasLookTarget;
	int          lookTarget;
	int          customRGBA[4];
	int          health;
	int          maxhealth; //so I know how to draw the stupid health bar
	int          csSounds_Std;
	int          csSounds_Combat;
	int          csSounds_Extra;
	int          csSounds_Jedi;
	int          surfacesOn; //a bitflag of corresponding surfaces from a lookup table. These surfaces will be forced on.
	int          surfacesOff; //same as above, but forced off instead.
	int          boneIndex1; // Allow up to 4 PCJ lookup values to be stored here. The resolve to configstrings which contain the name of the desired bone.
	int          boneIndex2;
	int          boneIndex3;
	int          boneIndex4;
	int          boneOrient; //packed with x, y, z orientations for bone angles
	vec3_t       boneAngles1; //angles of boneIndex1
	vec3_t       boneAngles2; //angles of boneIndex2
	vec3_t       boneAngles3; //angles of boneIndex3
	vec3_t       boneAngles4; //angles of boneIndex4
	int          userInt1;
	int          userInt2;
	int          userInt3;
	float        userFloat1;
	float        userFloat2;
	float        userFloat3;
	vec3_t       userVec1;
	vec3_t       userVec2;
};

struct qtime_t {
	int tm_sec; // seconds after the minute - [0,59]
	int tm_min; // minutes after the hour - [0,59]
	int tm_hour; // hours since midnight - [0,23]
	int tm_mday; // day of the month - [1,31]
	int tm_mon; // months since January - [0,11]
	int tm_year; // years since 1900
	int tm_wday; // days since Sunday - [0,6]
	int tm_yday; // days since January 1 - [0,365]
	int tm_isdst; // daylight savings time flag
};

struct mdxaBone_t {
	float matrix[3][4];
};

struct SSkinGoreData {
	vec3_t angles;
	vec3_t position;
	int    currentTime;
	int    entNum;
	vec3_t rayDirection;	// in world space
	vec3_t hitLocation;	// in world space
	vec3_t scale;
	float  SSize;			// size of splotch in the S texture direction in world units
	float  TSize;			// size of splotch in the T texture direction in world units
	float  theta;			// angle to rotate the splotch
	int    growDuration;			// time over which we want this to scale up, set to -1 for no scaling
	float  goreScaleStartFraction; // fraction of the final size at which we want the gore to initially appear
	bool   frontFaces;
	bool   backFaces;
	bool   baseModelOnly;
	int    lifeTime;				// effect expires after this amount of time
	int    fadeOutTime;			//specify the duration of fading, from the lifeTime (e.g. 3000 will start fading 3 seconds before removal and be faded entirely by removal)
	int    shrinkOutTime;			// unimplemented
	float  alphaModulate;			// unimplemented
	vec3_t tint;					// unimplemented
	float  impactStrength;			// unimplemented
	int    shader; // shader handle
	int    myIndex; // used internally
	bool   fadeRGB; //specify fade method to modify RGB (by default, the alpha is set instead)
};

struct stringID_table_t {
	const char *name;
	int         id;
};

struct lump_t {
	int fileofs, filelen;
};

// stuff to help out during development process, force reloading/uncacheing of certain filetypes...
struct dheader_t {
	int    ident;
	int    version;
	lump_t lumps[HEADER_LUMPS];
};

struct dmodel_t {
	float mins[3], maxs[3];
	int   firstSurface, numSurfaces;
	int   firstBrush, numBrushes;
};

struct dshader_t {
	char shader[MAX_QPATH];
	int  surfaceFlags;
	int  contentFlags;
};

struct dbrushside_t {
	int planeNum; // positive plane side faces out of the leaf
	int shaderNum;
	int drawSurfNum;
};

struct dbrush_t {
	int firstSide;
	int numSides;
	int shaderNum; // the shader that determines the contents flags
};

struct drawVert_t {
	vec3_t xyz;
	float  st[2];
	float  lightmap[MAXLIGHTMAPS][2];
	vec3_t normal;
	byte   color[MAXLIGHTMAPS][4];
};

struct dsurface_t {
	int       shaderNum;
	int       fogNum;
	int       surfaceType;
	int       firstVert;
	int       numVerts;
	int       firstIndex;
	int       numIndexes;
	byte      lightmapStyles[MAXLIGHTMAPS], vertexStyles[MAXLIGHTMAPS];
	int       lightmapNum[MAXLIGHTMAPS];
	int       lightmapX[MAXLIGHTMAPS], lightmapY[MAXLIGHTMAPS];
	int       lightmapWidth, lightmapHeight;
	vec3_t    lightmapOrigin;
	matrix3_t lightmapVecs; // for patches, [0] and [1] are lodbounds
	int       patchWidth;
	int       patchHeight;
};

// planes x^1 is always the opposite of plane x
struct dplane_t {
	float normal[3];
	float dist;
};

struct dnode_t {
	int planeNum;
	int children[2]; // negative numbers are -(leafs+1), not nodes
	int mins[3]; // for frustom culling
	int maxs[3];
};

struct dleaf_t {
	int cluster; // -1 = opaque cluster (do I still store these?)
	int area;
	int mins[3]; // for frustum culling
	int maxs[3];
	int firstLeafSurface;
	int numLeafSurfaces;
	int firstLeafBrush;
	int numLeafBrushes;
};



// unions
union byteAlias_t {
	float    f;
	int32_t  i;
	uint32_t ui;
	byte     b[4];
	char     c[4];
};

union fileBuffer_t {
	void *v;
	char *c;
	byte *b;
};



// type definitions
typedef int( *cmpFunc_t )(const void *a, const void *b);
typedef CollisionRecord_t G2Trace_t[MAX_G2_COLLISIONS];	// map that describes all of the parts of ghoul2 models that got hit



// global variables
#ifndef FINAL_BUILD
//	extern int g_Ghoul2Allocations;
//	extern int g_G2ServerAlloc;
//	extern int g_G2ClientAlloc;
//	extern int g_G2AllocServer;
#endif

extern float forceSpeedLevels[4];



// functions

// this is only here so the functions in q_shared.c and bg_*.c can link
#if defined( _GAME ) || defined( _CGAME ) || defined( UI_BUILD )
	extern NORETURN_PTR void (*Com_Error)( int level, const char *fmt, ... );
	extern void (*Com_Printf)( const char *fmt, ... );
#else
	void NORETURN QDECL Com_Error( int level, const char *fmt, ... );
	void QDECL Com_Printf( const char *fmt, ... );
#endif

void        COM_BeginParseSession  ( const char *name );
bool        COM_CompareExtension   ( const char *in, const char *ext );
int         COM_Compress           ( char *data_p );
void        COM_DefaultExtension   ( char *path, int maxSize, const char *extension );
int         COM_GetCurrentParseLine( void );
const char *COM_GetExtension       ( const char *name );
int         Com_HexStrToInt        ( const char *str );
void        COM_MatchToken         ( const char **buf_p, char *match );
char       *COM_Parse              ( const char **data_p );
char       *COM_ParseExt           ( const char **data_p, bool allowLineBreak );
void        COM_ParseError         ( char *format, ... );
bool        COM_ParseFloat         ( const char **data, float *f );
bool        COM_ParseInt           ( const char **data, int *i );
bool        COM_ParseString        ( const char **data, const char **s );
bool        COM_ParseVec4          ( const char **buffer, vec4_t *c );
void        COM_ParseWarning       ( char *format, ... );
void        Com_RandomBytes        ( byte *string, int len );
char       *Com_SkipCharset        ( char *s, char *sep );
char       *COM_SkipPath           ( char *pathname );
char       *Com_SkipTokens         ( char *s, int numTokens, char *sep );
int	QDECL   Com_sprintf            ( char *dest, int size, const char *fmt, ... );
void        COM_StripExtension     ( const char *in, char *out, int destsize );
void        Com_TruncateLongString ( char *buffer, const char *s );
int         FloatAsInt             ( float f );
int         GetIDForString         ( const stringID_table_t *table, const char *string );
const char *GetStringForID         ( stringID_table_t *table, int id );
void       *Hunk_Alloc             ( int size, ha_pref_e preference );
bool        Info_NextPair          ( const char **s, char *key, char *value );
bool        Info_Validate          ( const char *s );
char       *Info_ValueForKey       ( const char *s, const char *key );
void        Info_RemoveKey         ( char *s, const char *key );
void        Info_RemoveKey_Big     ( char *s, const char *key );
void        Info_SetValueForKey    ( char *s, const char *key, const char *value );
void        Info_SetValueForKey_Big( char *s, const char *key, const char *value );
void        NET_AddrToString       ( char *out, size_t size, void *addr );
void        Parse1DMatrix          ( const char **buf_p, int x, float *m );
void        Parse2DMatrix          ( const char **buf_p, int y, int x, float *m );
void        Parse3DMatrix          ( const char **buf_p, int z, int y, int x, float *m );
void        Q_AddToBitflags        ( uint32_t *bits, int index, uint32_t bitsPerByte );
bool        Q_InBitflags           ( const uint32_t *bits, int index, uint32_t bitsPerByte );
void       *Q_LinearSearch         ( const void *key, const void *ptr, size_t count, size_t size, cmpFunc_t cmp );
void        Q_RemoveFromBitflags   ( uint32_t *bits, int index, uint32_t bitsPerByte );
bool        SkipBracedSection      ( const char **program, int depth );
void        SkipRestOfLine         ( const char **data );
const char *SkipWhitespace         ( const char *data, bool *hasNewLines );

char * QDECL va( const char *format, ... );

