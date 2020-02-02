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

// game module information visible to server

#pragma once

#include "qcommon/q_shared.h"

#define Q3_INFINITE			16777216

#define	GAME_API_VERSION	1

// entity->svFlags
// the server does not know how to interpret most of the values
// in entityStates (level eType), so the game must explicitly flag
// special server behaviors
#define	SVF_NOCLIENT			0x00000001	// don't send entity to clients, even if it has effects
#define SVF_BROADCASTCLIENTS	0x00000002	// only broadcast to clients specified in r.broadcastClients[clientNum/32]
#define SVF_BOT					0x00000008	// set if the entity is a bot
#define SVF_PLAYER_USABLE		0x00000010	// player can use this with the use button
#define	SVF_BROADCAST			0x00000020	// send to all connected clients
#define	SVF_PORTAL				0x00000040	// merge a second pvs at origin2 into snapshots
#define	SVF_USE_CURRENT_ORIGIN	0x00000080	// entity->r.currentOrigin instead of entity->s.origin
											// for link position (missiles and movers)
#define SVF_SINGLECLIENT		0x00000100	// only send to a single client (entityShared_t->singleClient)
#define SVF_NOSERVERINFO		0x00000200	// don't send CS_SERVERINFO updates to this client
											// so that it can be updated for ping tools without
											// lagging clients
#define SVF_CAPSULE				0x00000400	// use capsule for collision detection instead of bbox
#define SVF_NOTSINGLECLIENT		0x00000800	// send entity to everyone but one client
											// (entityShared_t->singleClient)

#define SVF_OWNERNOTSHARED		0x00001000	// If it's owned by something and another thing owned by that something
											// hits it, it will still touch

#define	SVF_ICARUS_FREEZE		0x00008000	// NPCs are frozen, ents don't execute ICARUS commands

#define SVF_GLASS_BRUSH			0x08000000	// Ent is a glass brush

#define SVF_NO_BASIC_SOUNDS		0x10000000	// No basic sounds
#define SVF_NO_COMBAT_SOUNDS	0x20000000	// No combat sounds
#define SVF_NO_EXTRA_SOUNDS		0x40000000	// No extra or jedi sounds

//rww - ghoul2 trace flags
#define G2TRFLAG_DOGHOULTRACE	0x00000001 //do the ghoul2 trace
#define G2TRFLAG_HITCORPSES		0x00000002 //will try g2 collision on the ent even if it's EF_DEAD
#define G2TRFLAG_GETSURFINDEX	0x00000004 //will replace surfaceFlags with the ghoul2 surface index that was hit, if any.
#define G2TRFLAG_THICK			0x00000008 //assures that the trace radius will be significantly large regardless of the trace box size.

#define	MAX_PARMS	16
#define	MAX_PARM_STRING_LENGTH	MAX_QPATH//was 16, had to lengthen it so they could take a valid file path

#define MAX_FAILED_NODES 8

enum bState_e {//These take over only if script allows them to be autonomous
	BS_DEFAULT = 0,//# default behavior for that NPC
	BS_ADVANCE_FIGHT,//# Advance to captureGoal and shoot enemies if you can
	BS_SLEEP,//# Play awake script when startled by sound
	BS_FOLLOW_LEADER,//# Follow your leader and shoot any enemies you come across
	BS_JUMP,//# Face navgoal and jump to it.
	BS_SEARCH,//# Using current waypoint as a base, search the immediate branches of waypoints for enemies
	BS_WANDER,//# Wander down random waypoint paths
	BS_NOCLIP,//# Moves through walls, etc.
	BS_REMOVE,//# Waits for player to leave PVS then removes itself
	BS_CINEMATIC,//# Does nothing but face it's angles and move to a goal if it has one
	//# #eol
	//internal bStates only
	BS_WAIT,//# Does nothing but face it's angles
	BS_STAND_GUARD,
	BS_PATROL,
	BS_INVESTIGATE,//# head towards temp goal and look for enemies and listen for sounds
	BS_STAND_AND_SHOOT,
	BS_HUNT_AND_KILL,
	BS_FLEE,//# Run away!
	NUM_BSTATES
};

enum {
	EDGE_NORMAL,
	EDGE_PATH,
	EDGE_BLOCKED,
	EDGE_FAILED,
	EDGE_MOVEDIR
};

enum {
	NODE_NORMAL,
	NODE_START,
	NODE_GOAL,
	NODE_NAVGOAL,
};

enum taskID_e {
	TID_CHAN_VOICE = 0,	// Waiting for a voice sound to complete
	TID_ANIM_UPPER,		// Waiting to finish a lower anim holdtime
	TID_ANIM_LOWER,		// Waiting to finish a lower anim holdtime
	TID_ANIM_BOTH,		// Waiting to finish lower and upper anim holdtimes or normal md3 animating
	TID_MOVE_NAV,		// Trying to get to a navgoal or For ET_MOVERS
	TID_ANGLE_FACE,		// Turning to an angle or facing
	TID_BSTATE,			// Waiting for a certain bState to finish
	TID_LOCATION,		// Waiting for ent to enter a specific trigger_location
//	TID_MISSIONSTATUS,	// Waiting for player to finish reading MISSION STATUS SCREEN
	TID_RESIZE,			// Waiting for clear bbox to inflate size
	TID_SHOOT,			// Waiting for fire event
	NUM_TIDS,			// for def of taskID array
};

enum bSet_e {//This should check to matching a behavior state name first, then look for a script
	BSET_INVALID = -1,
	BSET_FIRST = 0,
	BSET_SPAWN = 0,//# script to use when first spawned
	BSET_USE,//# script to use when used
	BSET_AWAKE,//# script to use when awoken/startled
	BSET_ANGER,//# script to use when aquire an enemy
	BSET_ATTACK,//# script to run when you attack
	BSET_VICTORY,//# script to run when you kill someone
	BSET_LOSTENEMY,//# script to run when you can't find your enemy
	BSET_PAIN,//# script to use when take pain
	BSET_FLEE,//# script to use when take pain below 50% of health
	BSET_DEATH,//# script to use when killed
	BSET_DELAYED,//# script to run when self->delayScriptTime is reached
	BSET_BLOCKED,//# script to run when blocked by a friendly NPC or player
	BSET_BUMPED,//# script to run when bumped into a friendly NPC or player (can set bumpRadius)
	BSET_STUCK,//# script to run when blocked by a wall
	BSET_FFIRE,//# script to run when player shoots their own teammates
	BSET_FFDEATH,//# script to run when player kills a teammate
	BSET_MINDTRICK,//# script to run when player does a mind trick on this NPC

	NUM_BSETS
};

//this structure is shared by gameside and in-engine NPC nav routines.
struct failedEdge_t {
	int	startID;
	int	endID;
	int checkTime;
	int	entID;
};

struct entityShared_t {
	bool	linked;				// false if not in any good cluster
	int			linkcount;

	int			svFlags;			// SVF_NOCLIENT, SVF_BROADCAST, etc
	int			singleClient;		// only send to this client when SVF_SINGLECLIENT is set

	bool	bmodel;				// if false, assume an explicit mins / maxs bounding box
									// only set by trap_SetBrushModel
	vec3_t		mins, maxs;
	int			contents;			// CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
									// a non-solid entity should set to 0

	vec3_t		absmin, absmax;		// derived from mins/maxs and origin + rotation

	// currentOrigin will be used for all collision detection and world linking.
	// it will not necessarily be the same as the trajectory evaluation for the current
	// time, because each entity must be moved one at a time after time is advanced
	// to avoid simultanious collision issues
	vec3_t		currentOrigin;
	vec3_t		currentAngles;
	bool	mIsRoffing;			// set to true when the entity is being roffed

	// when a trace call is made and passEntityNum != ENTITYNUM_NONE,
	// an ent will be excluded from testing if:
	// ent->s.number == passEntityNum	(don't interact with self)
	// ent->s.ownerNum = passEntityNum	(don't interact with your own missiles)
	// entity[ent->s.ownerNum].ownerNum = passEntityNum	(don't interact with other missiles from owner)
	int			ownerNum;

	// mask of clients that this entity should be broadcast to
	// the first 32 clients are represented by the first array index and the latter 32 clients are represented by the
	//	second array index.
	uint32_t	broadcastClients[2];
};

struct parms_t {
	char	parm[MAX_PARMS][MAX_PARM_STRING_LENGTH];
};

// the server looks at a sharedEntity, which is the start of the game's gentity_t structure
//mod authors should not touch this struct
struct sharedEntity_t {
	//CJKFIXME: bgentity_t substruct
	entityState_t	s;				// communicated by server to clients
	playerState_t	*playerState;	//needs to be in the gentity for bg entity access
									//if you want to actually see the contents I guess
									//you will have to be sure to VMA it first.
	void			*ghoul2; //g2 instance
	int				localAnimIndex; //index locally (game/cgame) to anim data for this skel
	vec3_t			modelScale; //needed for g2 collision

	//from here up must also be unified with bgEntity/centity

	entityShared_t	r;				// shared by both the server system and game

	//Script/ICARUS-related fields
	int				taskID[NUM_TIDS];
	parms_t			*parms;
	char			*behaviorSet[NUM_BSETS];
	char			*script_targetname;
	int				delayScriptTime;
	char			*fullName;

	//rww - targetname and classname are now shared as well. ICARUS needs access to them.
	char			*targetname;
	char			*classname;			// set in QuakeEd

	//rww - and yet more things to share. This is because the nav code is in the exe because it's all C++.
	int				waypoint;			//Set once per frame, if you've moved, and if someone asks
	int				lastWaypoint;		//To make sure you don't double-back
	int				lastValidWaypoint;	//ALWAYS valid -used for tracking someone you lost
	int				noWaypointTime;		//Debouncer - so don't keep checking every waypoint in existance every frame that you can't find one
	int				combatPoint;
	int				failedWaypoints[MAX_FAILED_NODES];
	int				failedWaypointCheckTime;

	int				next_roff_time; //rww - npc's need to know when they're getting roff'd
};

struct T_G_ICARUS_PLAYSOUND {
	int taskID;
	int entID;
	char name[2048];
	char channel[2048];
};

struct T_G_ICARUS_SET {
	int taskID;
	int entID;
	char type_name[2048];
	char data[2048];
};

struct T_G_ICARUS_LERP2POS {
	int taskID;
	int entID;
	vec3_t origin;
	vec3_t angles;
	float duration;
	bool nullAngles; //special case
};

struct T_G_ICARUS_LERP2ORIGIN {
	int taskID;
	int entID;
	vec3_t origin;
	float duration;
};

struct T_G_ICARUS_LERP2ANGLES {
	int taskID;
	int entID;
	vec3_t angles;
	float duration;
};

struct T_G_ICARUS_GETTAG {
	int entID;
	char name[2048];
	int lookup;
	vec3_t info;
};

struct T_G_ICARUS_LERP2START {
	int entID;
	int taskID;
	float duration;
};

struct T_G_ICARUS_LERP2END {
	int entID;
	int taskID;
	float duration;
};

struct T_G_ICARUS_USE {
	int entID;
	char target[2048];
};

struct T_G_ICARUS_KILL {
	int entID;
	char name[2048];
};

struct T_G_ICARUS_REMOVE {
	int entID;
	char name[2048];
};

struct T_G_ICARUS_PLAY {
	int taskID;
	int entID;
	char type[2048];
	char name[2048];
};

struct T_G_ICARUS_GETFLOAT {
	int entID;
	int type;
	char name[2048];
	float value;
};

struct T_G_ICARUS_GETVECTOR {
	int entID;
	int type;
	char name[2048];
	vec3_t value;
};

struct T_G_ICARUS_GETSTRING {
	int entID;
	int type;
	char name[2048];
	char value[2048];
};

struct T_G_ICARUS_SOUNDINDEX {
	char filename[2048];
};

struct T_G_ICARUS_GETSETIDFORSTRING {
	char string[2048];
};

struct gameImport_t {
	// misc
	void		(*Print)								( const char *msg, ... );
	NORETURN_PTR void (*Error)( int level, const char *fmt, ... );
	int			(*Milliseconds)							( void );
	void		(*PrecisionTimerStart)					( void **timer );
	int			(*PrecisionTimerEnd)					( void *timer );
	void		(*SV_RegisterSharedMemory)				( char *memory );
	int			(*RealTime)								( qtime_t *qtime );
	void		(*TrueMalloc)							( void **ptr, int size );
	void		(*TrueFree)								( void **ptr );
	void		(*SnapVector)							( float *v );

	// cvar
	void		(*Cvar_Register)						( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, uint32_t flags );
	void		(*Cvar_Set)								( const char *var_name, const char *value );
	void		(*Cvar_Update)							( vmCvar_t *vmCvar );
	int			(*Cvar_VariableIntegerValue)			( const char *var_name );
	void		(*Cvar_VariableStringBuffer)			( const char *var_name, char *buffer, int bufsize );

	// cmd
	int			(*Argc)									( void );
	void		(*Argv)									( int n, char *buffer, int bufferLength );

	// filesystem
	void		(*FS_Close)								( fileHandle_t f );
	int			(*FS_GetFileList)						( const char *path, const char *extension, char *listbuf, int bufsize );
	int			(*FS_Open)								( const char *qpath, fileHandle_t *f, fsMode_e mode );
	int			(*FS_Read)								( void *buffer, int len, fileHandle_t f );
	int			(*FS_Write)								( const void *buffer, int len, fileHandle_t f );

	// server
	void		(*AdjustAreaPortalState)				( sharedEntity_t *ent, bool open );
	bool		(*AreasConnected)						( int area1, int area2 );
	int			(*DebugPolygonCreate)					( int color, int numPoints, vec3_t *points );
	void		(*DebugPolygonDelete)					( int id );
	void		(*DropClient)							( int clientNum, const char *reason );
	int			(*EntitiesInBox)						( const vec3_t mins, const vec3_t maxs, int *list, int maxcount );
	bool		(*EntityContact)						( const vec3_t mins, const vec3_t maxs, const sharedEntity_t *ent, int capsule );
	void		(*GetConfigstring)						( int num, char *buffer, int bufferSize );
	bool		(*GetEntityToken)						( char *buffer, int bufferSize );
	void		(*GetServerinfo)						( char *buffer, int bufferSize );
	void		(*GetUsercmd)							( int clientNum, usercmd_t *cmd );
	void		(*GetUserinfo)							( int num, char *buffer, int bufferSize );
	bool		(*InPVS)								( const vec3_t p1, const vec3_t p2 );
	bool		(*InPVSIgnorePortals)					( const vec3_t p1, const vec3_t p2 );
	void		(*LinkEntity)							( sharedEntity_t *ent );
	void		(*LocateGameData)						( sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGClient );
	int			(*PointContents)						( const vec3_t point, int passEntityNum );
	void		(*SendConsoleCommand)					( int exec_when, const char *text );
	void		(*SendServerCommand)					( int clientNum, const char *text );
	void		(*SetBrushModel)						( sharedEntity_t *ent, const char *name );
	void		(*SetConfigstring)						( int num, const char *string );
	void		(*SetServerCull)						( float cullDistance );
	void		(*SetUserinfo)							( int num, const char *buffer );
	void		(*Trace)								( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, int capsule, int traceFlags, int useLod );
	void		(*TraceEntity)							( trace_t *trace, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int entityNum, int contentmask, int capsule );
	void		(*UnlinkEntity)							( sharedEntity_t *ent );

	// ROFF
	bool		(*ROFF_Clean)							( void );
	void		(*ROFF_UpdateEntities)					( void );
	int			(*ROFF_Cache)							( char *file );
	bool		(*ROFF_Play)							( int entID, int roffID, bool doTranslation );
	bool		(*ROFF_Purge_Ent)						( int entID );

	// ICARUS
	int			(*ICARUS_RunScript)						( sharedEntity_t *ent, const char *name );
	bool		(*ICARUS_RegisterScript)				( const char *name, bool bCalledDuringInterrogate );
	void		(*ICARUS_Init)							( void );
	bool		(*ICARUS_ValidEnt)						( sharedEntity_t *ent );
	bool		(*ICARUS_IsInitialized)					( int entID );
	bool		(*ICARUS_MaintainTaskManager)			( int entID );
	bool		(*ICARUS_IsRunning)						( int entID );
	bool		(*ICARUS_TaskIDPending)					( sharedEntity_t *ent, int taskID );
	void		(*ICARUS_InitEnt)						( sharedEntity_t *ent );
	void		(*ICARUS_FreeEnt)						( sharedEntity_t *ent );
	void		(*ICARUS_AssociateEnt)					( sharedEntity_t *ent );
	void		(*ICARUS_Shutdown)						( void );
	void		(*ICARUS_TaskIDSet)						( sharedEntity_t *ent, int taskType, int taskID );
	void		(*ICARUS_TaskIDComplete)				( sharedEntity_t *ent, int taskType );
	void		(*ICARUS_SetVar)						( int taskID, int entID, const char *type_name, const char *data );
	int			(*ICARUS_VariableDeclared)				( const char *type_name );
	int			(*ICARUS_GetFloatVariable)				( const char *name, float *value );
	int			(*ICARUS_GetStringVariable)				( const char *name, const char *value );
	int			(*ICARUS_GetVectorVariable)				( const char *name, const vec3_t value );

	// botlib
	int			(*BotAllocateClient)					( void );
	void		(*BotFreeClient)						( int clientNum );
	int			(*BotLibSetup)							( void );
	int			(*BotLibShutdown)						( void );
	int			(*BotLibVarSet)							( char *var_name, char *value );
	int			(*BotLibVarGet)							( char *var_name, char *value, int size );
	int			(*BotLibDefine)							( char *string );
	int			(*BotLibStartFrame)						( float time );
	int			(*BotLibLoadMap)						( const char *mapname );
	int			(*BotLibUpdateEntity)					( int ent, void *bue );
	int			(*BotLibTest)							( int parm0, char *parm1, vec3_t parm2, vec3_t parm3 );
	int			(*BotGetSnapshotEntity)					( int clientNum, int sequence );
	int			(*BotGetServerCommand)					( int clientNum, char *message, int size );
	void		(*BotUserCommand)						( int clientNum, usercmd_t *ucmd );
	void		(*BotUpdateWaypoints)					( int wpnum, wpobject_t **wps );
	void		(*BotCalculatePaths)					( int rmg );

	// elementary action
	void		(*EA_Say)								( int client, const char *str );
	void		(*EA_SayTeam)							( int client, const char *str );
	void		(*EA_Command)							( int client, const char *command );
	void		(*EA_Action)							( int client, int action );
	void		(*EA_Gesture)							( int client );
	void		(*EA_Talk)								( int client );
	void		(*EA_Attack)							( int client );
	void		(*EA_Alt_Attack)						( int client );
	void		(*EA_ForcePower)						( int client );
	void		(*EA_Use)								( int client );
	void		(*EA_Respawn)							( int client );
	void		(*EA_Crouch)							( int client );
	void		(*EA_MoveUp)							( int client );
	void		(*EA_MoveDown)							( int client );
	void		(*EA_MoveForward)						( int client );
	void		(*EA_MoveBack)							( int client );
	void		(*EA_MoveLeft)							( int client );
	void		(*EA_MoveRight)							( int client );
	void		(*EA_Walk)								( int client );
	void		(*EA_SelectWeapon)						( int client, int weapon );
	void		(*EA_Jump)								( int client );
	void		(*EA_DelayedJump)						( int client );
	void		(*EA_Move)								( int client, vec3_t dir, float speed );
	void		(*EA_View)								( int client, vec3_t viewangles );
	void		(*EA_EndRegular)						( int client, float thinktime );
	void		(*EA_GetInput)							( int client, float thinktime, void *input );
	void		(*EA_ResetInput)						( int client );

	// botlib preprocessor
	int			(*PC_LoadSource)						( const char *filename );
	int			(*PC_FreeSource)						( int handle );
	int			(*PC_ReadToken)							( int handle, pc_token_t *pc_token );
	int			(*PC_SourceFileAndLine)					( int handle, char *filename, int *line );

	// renderer, terrain
	qhandle_t	(*R_RegisterSkin)						( const char *name );
	const char *(*SetActiveSubBSP)						( int index );
	int			(*CM_RegisterTerrain)					( const char *config );
	void		(*RMG_Init)								( void );

	void		(*G2API_ListModelBones)					( void *ghlInfo, int frame );
	void		(*G2API_ListModelSurfaces)				( void *ghlInfo );
	bool		(*G2API_HaveWeGhoul2Models)				( void *ghoul2 );
	void		(*G2API_SetGhoul2ModelIndexes)			( void *ghoul2, qhandle_t *modelList, qhandle_t *skinList );
	bool		(*G2API_GetBoltMatrix)					( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale );
	bool		(*G2API_GetBoltMatrix_NoReconstruct)	( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale );
	bool		(*G2API_GetBoltMatrix_NoRecNoRot)		( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale );
	int			(*G2API_InitGhoul2Model)				( void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin, qhandle_t customShader, int modelFlags, int lodBias );
	bool		(*G2API_SetSkin)						( void *ghoul2, int modelIndex, qhandle_t customSkin, qhandle_t renderSkin );
	int			(*G2API_Ghoul2Size)						( void *ghlInfo );
	int			(*G2API_AddBolt)						( void *ghoul2, int modelIndex, const char *boneName );
	void		(*G2API_SetBoltInfo)					( void *ghoul2, int modelIndex, int boltInfo );
	bool		(*G2API_SetBoneAngles)					( void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags, const int up, const int right, const int forward, qhandle_t *modelList, int blendTime , int currentTime );
	bool		(*G2API_SetBoneAnim)					( void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime );
	bool		(*G2API_GetBoneAnim)					( void *ghoul2, const char *boneName, const int currentTime, float *currentFrame, int *startFrame, int *endFrame, int *flags, float *animSpeed, int *modelList, const int modelIndex );
	void		(*G2API_GetGLAName)						( void *ghoul2, int modelIndex, char *fillBuf );
	int			(*G2API_CopyGhoul2Instance)				( void *g2From, void *g2To, int modelIndex );
	void		(*G2API_CopySpecificGhoul2Model)		( void *g2From, int modelFrom, void *g2To, int modelTo );
	void		(*G2API_DuplicateGhoul2Instance)		( void *g2From, void **g2To );
	bool		(*G2API_HasGhoul2ModelOnIndex)			( void *ghlInfo, int modelIndex );
	bool		(*G2API_RemoveGhoul2Model)				( void *ghlInfo, int modelIndex );
	bool		(*G2API_RemoveGhoul2Models)				( void *ghlInfo );
	void		(*G2API_CleanGhoul2Models)				( void **ghoul2Ptr );
	void		(*G2API_CollisionDetect)				( CollisionRecord_t *collRecMap, void *ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius );
	void		(*G2API_CollisionDetectCache)			( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius );
	bool		(*G2API_SetRootSurface)					( void *ghoul2, const int modelIndex, const char *surfaceName );
	bool		(*G2API_SetSurfaceOnOff)				( void *ghoul2, const char *surfaceName, const int flags );
	bool		(*G2API_SetNewOrigin)					( void *ghoul2, const int boltIndex );
	bool		(*G2API_DoesBoneExist)					( void *ghoul2, int modelIndex, const char *boneName );
	int			(*G2API_GetSurfaceRenderStatus)			( void *ghoul2, const int modelIndex, const char *surfaceName );
	void		(*G2API_AbsurdSmoothing)				( void *ghoul2, bool status );
	void		(*G2API_SetRagDoll)						( void *ghoul2, sharedRagDollParams_t *params );
	void		(*G2API_AnimateG2Models)				( void *ghoul2, int time, sharedRagDollUpdateParams_t *params );
	bool		(*G2API_RagPCJConstraint)				( void *ghoul2, const char *boneName, vec3_t min, vec3_t max );
	bool		(*G2API_RagPCJGradientSpeed)			( void *ghoul2, const char *boneName, const float speed );
	bool		(*G2API_RagEffectorGoal)				( void *ghoul2, const char *boneName, vec3_t pos );
	bool		(*G2API_GetRagBonePos)					( void *ghoul2, const char *boneName, vec3_t pos, vec3_t entAngles, vec3_t entPos, vec3_t entScale );
	bool		(*G2API_RagEffectorKick)				( void *ghoul2, const char *boneName, vec3_t velocity );
	bool		(*G2API_RagForceSolve)					( void *ghoul2, bool force );
	bool		(*G2API_SetBoneIKState)					( void *ghoul2, int time, const char *boneName, int ikState, sharedSetBoneIKStateParams_t *params );
	bool		(*G2API_IKMove)							( void *ghoul2, int time, sharedIKMoveParams_t *params );
	bool		(*G2API_RemoveBone)						( void *ghoul2, const char *boneName, int modelIndex );
	void		(*G2API_AttachInstanceToEntNum)			( void *ghoul2, int entityNum, bool server );
	void		(*G2API_ClearAttachedInstance)			( int entityNum );
	void		(*G2API_CleanEntAttachments)			( void );
	bool		(*G2API_OverrideServer)					( void *serverInstance );
	void		(*G2API_GetSurfaceName)					( void *ghoul2, int surfNumber, int modelIndex, char *fillBuf );
};

struct gameExport_t {
	void		(*InitGame)							( int levelTime, int randomSeed, int restart );
	void		(*ShutdownGame)						( int restart );
	char *		(*ClientConnect)					( int clientNum, bool firstTime, bool isBot );
	void		(*ClientBegin)						( int clientNum, bool allowTeamReset );
	bool		(*ClientUserinfoChanged)			( int clientNum );
	void		(*ClientDisconnect)					( int clientNum );
	void		(*ClientCommand)					( int clientNum );
	void		(*ClientThink)						( int clientNum, usercmd_t *ucmd );
	void		(*RunFrame)							( int levelTime );
	bool		(*ConsoleCommand)					( void );
	int			(*BotAIStartFrame)					( int time );
	void		(*ROFF_NotetrackCallback)			( int entID, const char *notetrack );
	void		(*SpawnRMGEntity)					( void );
	int			(*ICARUS_PlaySound)					( void );
	bool		(*ICARUS_Set)						( void );
	void		(*ICARUS_Lerp2Pos)					( void );
	void		(*ICARUS_Lerp2Origin)				( void );
	void		(*ICARUS_Lerp2Angles)				( void );
	int			(*ICARUS_GetTag)					( void );
	void		(*ICARUS_Lerp2Start)				( void );
	void		(*ICARUS_Lerp2End)					( void );
	void		(*ICARUS_Use)						( void );
	void		(*ICARUS_Kill)						( void );
	void		(*ICARUS_Remove)					( void );
	void		(*ICARUS_Play)						( void );
	int			(*ICARUS_GetFloat)					( void );
	int			(*ICARUS_GetVector)					( void );
	int			(*ICARUS_GetString)					( void );
	void		(*ICARUS_SoundIndex)				( void );
	int			(*ICARUS_GetSetIDForString)			( void );
	int			(*BG_GetItemIndexByTag)				( int tag, int type );
};

//linking of game library
typedef gameExport_t* (QDECL *GetGameAPI_t)( int apiVersion, gameImport_t *import );
