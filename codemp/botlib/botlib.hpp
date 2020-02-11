/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
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

#include "qcommon/q_common.hpp"

// bot AI library

#ifndef BSPTRACE
#define BSPTRACE
#endif	// BSPTRACE

#define	BOTLIB_API_VERSION		3
#define BOTFILESBASEFOLDER		"botfiles"

enum debugLineColour_e {
	//debug line colors
	LINECOLOR_NONE =   -1,
	LINECOLOR_RED =    1, // 0xf2f2f0f0L
	LINECOLOR_GREEN =  2, // 0xd0d1d2d3L
	LINECOLOR_BLUE =   3, // 0xf3f3f1f1L
	LINECOLOR_YELLOW = 4, // 0xdcdddedfL
	LINECOLOR_ORANGE = 5, // 0xe0e1e2e3L
};

enum printType_e {
	//Print types
	PRT_MESSAGE = 1,
	PRT_WARNING,
	PRT_ERROR,
	PRT_FATAL,
	PRT_EXIT,
};

enum consoleMessageType_e {
	//console message types
	CMS_NORMAL = 0,
	CMS_CHAT,
};

//botlib error codes
enum botlibError_e {
	BLERR_NOERROR = 0,            // no error
	BLERR_LIBRARYNOTSETUP,        // library not setup
	BLERR_INVALIDENTITYNUMBER,    // invalid entity number
	BLERR_NOAASFILE,              // no AAS file available
	BLERR_CANNOTOPENAASFILE,      // cannot open AAS file
	BLERR_WRONGAASFILEID,         // incorrect AAS file id
	BLERR_WRONGAASFILEVERSION,    // incorrect AAS file version
	BLERR_CANNOTREADAASLUMP,      // cannot read AAS file lump
	BLERR_CANNOTLOADICHAT,        // cannot load initial chats
	BLERR_CANNOTLOADITEMWEIGHTS,  // cannot load item weights
	BLERR_CANNOTLOADITEMCONFIG,   // cannot load item config
	BLERR_CANNOTLOADWEAPONWEIGHTS,// cannot load weapon weights
	BLERR_CANNOTLOADWEAPONCONFIG, // cannot load weapon config
};

//action flags
enum actionFlag_e : uint32_t {
	ACTION_ATTACK =      0x00000001,
	ACTION_USE =         0x00000002,
	ACTION_RESPAWN =     0x00000004,
	ACTION_JUMP =        0x00000008,
	ACTION_MOVEUP =      0x00000010,
	ACTION_CROUCH =      0x00000020,
	ACTION_MOVEDOWN =    0x00000040,
	ACTION_MOVEFORWARD = 0x00000080,
	ACTION_MOVEBACK =    0x00000100,
	ACTION_MOVELEFT =    0x00000200,
	ACTION_MOVERIGHT =   0x00000400,
	ACTION_DELAYEDJUMP = 0x00000800,
	ACTION_TALK =        0x00001000,
	ACTION_GESTURE =     0x00002000,
	ACTION_WALK =        0x00004000,
	ACTION_FORCEPOWER =  0x00008000,
	ACTION_ALT_ATTACK =  0x00010000,
	ACTION_AFFIRMATIVE = 0x00020000,
	ACTION_NEGATIVE =    0x00040000,
	ACTION_GETFLAG =     0x00100000,
	ACTION_GUARDBASE =   0x00200000,
	ACTION_PATROL =      0x00400000,
	ACTION_FOLLOWME =    0x00800000,
};

//the bot input, will be converted to a usercmd_t
struct bot_input_t {
	float thinktime;		//time since last output (in seconds)
	vec3_t dir;				//movement direction
	float speed;			//speed in the range [0, 400]
	vec3_t viewangles;		//the view angles
	int actionflags;		//one of the ACTION_? flags
	int weapon;				//weapon to use
};

//bsp_trace_t hit surface
struct bsp_surface_t {
	char name[16];
	int flags;
	int value;
};

//remove the bsp_trace_s structure definition l8r on
//a trace is returned when a box is swept through the world
struct bsp_trace_t {
	bool			allsolid;	// if true, plane is not valid
	bool			startsolid;	// if true, the initial point was in a solid area
	float			fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t			endpos;		// final position
	cplane_t		plane;		// surface normal at impact
	float			exp_dist;	// expanded plane distance
	int				sidenum;	// number of the brush side hit
	bsp_surface_t	surface;	// the hit point surface
	int				contents;	// contents on other side of surface hit
	int				ent;		// number of entity hit
};

//entity state
struct bot_entitystate_t {
	int		type;			// entity type
	int		flags;			// entity flags
	vec3_t	origin;			// origin of the entity
	vec3_t	angles;			// angles of the model
	vec3_t	old_origin;		// for lerping
	vec3_t	mins;			// bounding box minimums
	vec3_t	maxs;			// bounding box maximums
	int		groundent;		// ground entity
	int		solid;			// solid type
	int		modelindex;		// model used
	int		modelindex2;	// weapons, CTF flags, etc
	int		frame;			// model frame number
	int		event;			// impulse events -- muzzle flashes, footsteps, etc
	int		eventParm;		// even parameter
	int		powerups;		// bit flags
	int		weapon;			// determines weapon and flash model, etc
	int		legsAnim;
	int		torsoAnim;
};

//bot AI library exported functions
struct botlib_import_t {
	//print messages from the bot library
	void		(QDECL *Print)(int type, char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
	//trace a bbox through the world
	void		(*Trace)(bsp_trace_t *trace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask);
	//trace a bbox against a specific entity
	void		(*EntityTrace)(bsp_trace_t *trace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int entnum, int contentmask);
	//retrieve the contents at the given point
	int			(*PointContents)(vec3_t point);
	//check if the point is in potential visible sight
	int			(*inPVS)(vec3_t p1, vec3_t p2);
	//retrieve the BSP entity data lump
	char		*(*BSPEntityData)(void);

	void		(*BSPModelMinsMaxsOrigin)(int modelnum, vec3_t angles, vec3_t mins, vec3_t maxs, vec3_t origin);
	//send a bot client command
	void		(*BotClientCommand)(int client, const char *command);
	//memory allocation
	void		*(*GetMemory)(int size);		// allocate from Zone
	void		(*FreeMemory)(void *ptr);		// free memory from Zone
	int			(*AvailableMemory)(void);		// available Zone memory
	void		*(*HunkAlloc)(int size);		// allocate from hunk
	//file system access
	int			(*FS_FOpenFile)( const char *qpath, fileHandle_t *file, fsMode_e mode );
	int			(*FS_Read)( void *buffer, int len, fileHandle_t f );
	int			(*FS_Write)( const void *buffer, int len, fileHandle_t f );
	void		(*FS_FCloseFile)( fileHandle_t f );
	int			(*FS_Seek)( fileHandle_t f, long offset, int origin );
	//debug visualisation stuff
	int			(*DebugLineCreate)(void);
	void		(*DebugLineDelete)(int line);
	void		(*DebugLineShow)(int line, vec3_t start, vec3_t end, int color);

	int			(*DebugPolygonCreate)(int color, int numPoints, vec3_t *points);
	void		(*DebugPolygonDelete)(int id);
};

struct ea_export_t {
	//ClientCommand elementary actions
	void	(*EA_Command)(int client, const char *command );
	void	(*EA_Say)(int client, const char *str);
	void	(*EA_SayTeam)(int client, const char *str);

	void	(*EA_Action)(int client, int action);
	void	(*EA_Gesture)(int client);
	void	(*EA_Talk)(int client);
	void	(*EA_Attack)(int client);
	void	(*EA_Use)(int client);
	void	(*EA_Respawn)(int client);
	void	(*EA_MoveUp)(int client);
	void	(*EA_MoveDown)(int client);
	void	(*EA_MoveForward)(int client);
	void	(*EA_MoveBack)(int client);
	void	(*EA_MoveLeft)(int client);
	void	(*EA_MoveRight)(int client);
	void	(*EA_Walk)(int client);
	void	(*EA_Crouch)(int client);
	void	(*EA_Alt_Attack)(int client);
	void	(*EA_ForcePower)(int client);

	void	(*EA_SelectWeapon)(int client, int weapon);
	void	(*EA_Jump)(int client);
	void	(*EA_DelayedJump)(int client);
	void	(*EA_Move)(int client, vec3_t dir, float speed);
	void	(*EA_View)(int client, vec3_t viewangles);
	//send regular input to the server
	void	(*EA_EndRegular)(int client, float thinktime);
	void	(*EA_GetInput)(int client, float thinktime, bot_input_t *input);
	void	(*EA_ResetInput)(int client);
};

//bot AI library imported functions
struct botlib_export_t {
	//Elementary Action functions
	ea_export_t ea;
	//setup the bot library, returns BLERR_
	int (*BotLibSetup)(void);
	//shutdown the bot library, returns BLERR_
	int (*BotLibShutdown)(void);
	//sets a library variable returns BLERR_
	int (*BotLibVarSet)(char *var_name, char *value);
	//gets a library variable returns BLERR_
	int (*BotLibVarGet)(char *var_name, char *value, int size);

	//sets a C-like define returns BLERR_
	void (*PC_Init)( void );
	int (*PC_AddGlobalDefine)(char *string);
	int (*PC_LoadSourceHandle)(const char *filename);
	int (*PC_FreeSourceHandle)(int handle);
	int (*PC_ReadTokenHandle)(int handle, pc_token_t *pc_token);
	int (*PC_SourceFileAndLine)(int handle, char *filename, int *line);
	int (*PC_LoadGlobalDefines)(const char* filename );
	void (*PC_RemoveAllGlobalDefines) ( void );

	//start a frame in the bot library
	int (*BotLibStartFrame)(float time);
	//load a new map in the bot library
	int (*BotLibLoadMap)(const char *mapname);
	//entity updates
	int (*BotLibUpdateEntity)(int ent, bot_entitystate_t *state);
	//just for testing
	int (*Test)(int parm0, char *parm1, vec3_t parm2, vec3_t parm3);
};

//linking of bot library
botlib_export_t *GetBotLibAPI( int apiVersion, botlib_import_t *import );

/* Library variables:

name:						default:			module(s):			description:

"basedir"					""					l_utils.c			base directory
"gamedir"					""					l_utils.c			game directory
"cddir"						""					l_utils.c			CD directory

"log"						"0"					l_log.c				enable/disable creating a log file
"maxclients"				"4"					be_interface.c		maximum number of clients
"maxentities"				"1024"				be_interface.c		maximum number of entities
"bot_developer"				"0"					be_interface.c		bot developer mode (it's "botDeveloper" in C to prevent symbol clash).

*/
