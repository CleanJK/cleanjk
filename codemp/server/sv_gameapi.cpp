/*
===========================================================================
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

// sv_gameapi.cpp  -- interface to the game dll
//Anything above this #include will be ignored by the compiler

#include "server/server.hpp"
#include "botlib/botlib.hpp"
#include "qcommon/stringed_ingame.hpp"
#include "qcommon/RoffSystem.hpp"
#include "ghoul2/ghoul2_shared.hpp"
#include "qcommon/cm_public.hpp"
#include "icarus/GameInterface.hpp"
#include "icarus/Q3_Interface.hpp"
#include "icarus/Q3_Registers.hpp"
#include "icarus/icarus.hpp"
#include "qcommon/timing.hpp"
#include "qcommon/com_cvar.hpp"
#include "qcommon/com_cvars.hpp"

botlib_export_t	*botlib_export;

// game interface
static gameExport_t *ge; // game export table
static vm_t *gvm; // game vm, valid for legacy and new api

// game vmMain calls
void GVM_InitGame( int levelTime, int randomSeed, int restart ) {
	VMSwap v( gvm );
	ge->InitGame( levelTime, randomSeed, restart );
}

void GVM_ShutdownGame( int restart ) {
	VMSwap v( gvm );
	ge->ShutdownGame( restart );
}

char *GVM_ClientConnect( int clientNum, bool firstTime, bool isBot ) {
	VMSwap v( gvm );
	return ge->ClientConnect( clientNum, firstTime, isBot );
}

void GVM_ClientBegin( int clientNum ) {
	VMSwap v( gvm );
	ge->ClientBegin( clientNum, true );
}

bool GVM_ClientUserinfoChanged( int clientNum ) {
	VMSwap v( gvm );
	return ge->ClientUserinfoChanged( clientNum );
}

void GVM_ClientDisconnect( int clientNum ) {
	VMSwap v( gvm );
	ge->ClientDisconnect( clientNum );
}

void GVM_ClientCommand( int clientNum ) {
	VMSwap v( gvm );
	ge->ClientCommand( clientNum );
}

void GVM_ClientThink( int clientNum, usercmd_t *ucmd ) {
	VMSwap v( gvm );
	ge->ClientThink( clientNum, ucmd );
}

void GVM_RunFrame( int levelTime ) {
	VMSwap v( gvm );
	ge->RunFrame( levelTime );
}

bool GVM_ConsoleCommand( void ) {
	VMSwap v( gvm );
	return ge->ConsoleCommand();
}

int GVM_BotAIStartFrame( int time ) {
	VMSwap v( gvm );
	return ge->BotAIStartFrame( time );
}

void GVM_ROFF_NotetrackCallback( int entID, const char *notetrack ) {
	VMSwap v( gvm );
	ge->ROFF_NotetrackCallback( entID, notetrack );
}

void GVM_SpawnRMGEntity( void ) {
	VMSwap v( gvm );
	ge->SpawnRMGEntity();
}

int GVM_ICARUS_PlaySound( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_PlaySound();
}

bool GVM_ICARUS_Set( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_Set();
}

void GVM_ICARUS_Lerp2Pos( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Lerp2Pos();
}

void GVM_ICARUS_Lerp2Origin( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Lerp2Origin();
}

void GVM_ICARUS_Lerp2Angles( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Lerp2Angles();
}

int GVM_ICARUS_GetTag( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_GetTag();
}

void GVM_ICARUS_Lerp2Start( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Lerp2Start();
}

void GVM_ICARUS_Lerp2End( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Lerp2End();
}

void GVM_ICARUS_Use( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Use();
}

void GVM_ICARUS_Kill( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Kill();
}

void GVM_ICARUS_Remove( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Remove();
}

void GVM_ICARUS_Play( void ) {
	VMSwap v( gvm );
	ge->ICARUS_Play();
}

int GVM_ICARUS_GetFloat( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_GetFloat();
}

int GVM_ICARUS_GetVector( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_GetVector();
}

int GVM_ICARUS_GetString( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_GetString();
}

void GVM_ICARUS_SoundIndex( void ) {
	VMSwap v( gvm );
	ge->ICARUS_SoundIndex();
}

int GVM_ICARUS_GetSetIDForString( void ) {
	VMSwap v( gvm );
	return ge->ICARUS_GetSetIDForString();
}

int GVM_BG_GetItemIndexByTag( int tag, int type ) {
	VMSwap v( gvm );
	return ge->BG_GetItemIndexByTag( tag, type );
}

// game syscalls
//	only used by legacy mods!

// legacy syscall
extern float g_svCullDist;

static void SV_LocateGameData( sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient ) {
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;

	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
}

static void SV_GameDropClient( int clientNum, const char *reason ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		return;
	}
	SV_DropClient( svs.clients + clientNum, reason );
}

static void SV_GameSendServerCommand( int clientNum, const char *text ) {
	if ( clientNum == -1 ) {
		SV_SendServerCommand( nullptr, "%s", text );
	} else {
		if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, "%s", text );
	}
}

static bool SV_EntityContact( const vec3_t mins, const vec3_t maxs, const sharedEntity_t *gEnt, int capsule ) {
	const float	*origin, *angles;
	clipHandle_t	ch;
	trace_t			trace;

	// check for exact collision
	origin = gEnt->r.currentOrigin;
	angles = gEnt->r.currentAngles;

	ch = SV_ClipHandleForEntity( gEnt );
	CM_TransformedBoxTrace ( &trace, vec3_origin, vec3_origin, mins, maxs,
		ch, -1, origin, angles, capsule );

	return (bool)trace.startsolid;
}

static void SV_SetBrushModel( sharedEntity_t *ent, const char *name ) {
	clipHandle_t	h;
	vec3_t			mins, maxs;

	if (!name)
	{
		Com_Error( ERR_DROP, "SV_SetBrushModel: nullptr" );
	}

	if (name[0] == '*')
	{
		ent->s.modelindex = atoi( name + 1 );

		if (sv.mLocalSubBSPIndex != -1)
		{
			ent->s.modelindex += sv.mLocalSubBSPModelOffset;
		}

		h = CM_InlineModel( ent->s.modelindex );

		CM_ModelBounds(h, mins, maxs);

		VectorCopy (mins, ent->r.mins);
		VectorCopy (maxs, ent->r.maxs);
		ent->r.bmodel = true;
		ent->r.contents = CM_ModelContents( h, -1 );
	}
	else if (name[0] == '#')
	{
		ent->s.modelindex = CM_LoadSubBSP(va("maps/%s.bsp", name + 1), false);
		CM_ModelBounds( ent->s.modelindex, mins, maxs );

		VectorCopy (mins, ent->r.mins);
		VectorCopy (maxs, ent->r.maxs);
		ent->r.bmodel = true;

		//rwwNOTE: We don't ever want to set contents -1, it includes CONTENTS_LIGHTSABER.
		//Lots of stuff will explode if there's a brush with CONTENTS_LIGHTSABER that isn't attached to a client owner.
		//ent->contents = -1;		// we don't know exactly what is in the brushes
		h = CM_InlineModel( ent->s.modelindex );
		ent->r.contents = CM_ModelContents( h, CM_FindSubBSP(ent->s.modelindex) );
	}
	else
	{
		Com_Error( ERR_DROP, "SV_SetBrushModel: %s isn't a brush model", name );
	}
}

static bool SV_inPVSIgnorePortals( const vec3_t p1, const vec3_t p2 ) {
	int		leafnum, cluster;
//	int		area1, area2;
	byte	*mask;

	leafnum = CM_PointLeafnum( p1 );
	cluster = CM_LeafCluster( leafnum );
//	area1 = CM_LeafArea( leafnum );
	mask = CM_ClusterPVS( cluster );

	leafnum = CM_PointLeafnum( p2 );
	cluster = CM_LeafCluster( leafnum );
//	area2 = CM_LeafArea( leafnum );

	if ( mask && (!(mask[cluster>>3] & (1<<(cluster&7)) ) ) )
		return false;

	return true;
}

static void SV_GetServerinfo( char *buffer, int bufferSize ) {
	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize );
		return;
	}
	Q_strncpyz( buffer, Cvar_InfoString( CVAR_SERVERINFO ), bufferSize );
}

static void SV_AdjustAreaPortalState( sharedEntity_t *ent, bool open ) {
	svEntity_t	*svEnt;

	svEnt = SV_SvEntityForGentity( ent );
	if ( svEnt->areanum2 == -1 )
		return;

	CM_AdjustAreaPortalState( svEnt->areanum, svEnt->areanum2, open );
}

static void SV_GetUsercmd( int clientNum, usercmd_t *cmd ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
		return;
	}
	*cmd = svs.clients[clientNum].lastUsercmd;
}

static const char *SV_SetActiveSubBSP( int index ) {
	if ( index >= 0 ) {
		sv.mLocalSubBSPIndex = CM_FindSubBSP( index );

		if (sv.mLocalSubBSPIndex == -1)
		{
			return nullptr;
		}

		sv.mLocalSubBSPModelOffset = index;
		sv.mLocalSubBSPEntityParsePoint = CM_SubBSPEntityString( sv.mLocalSubBSPIndex );
		return sv.mLocalSubBSPEntityParsePoint;
	}

	sv.mLocalSubBSPIndex = -1;
	return nullptr;
}

static bool SV_GetEntityToken( char *buffer, int bufferSize ) {
	char *s;

	if ( sv.mLocalSubBSPIndex == -1 ) {
		s = COM_Parse( (const char **)&sv.entityParsePoint );
		Q_strncpyz( buffer, s, bufferSize );
		if ( !sv.entityParsePoint && !s[0] )
			return false;
		else
			return true;
	}
	else {
		s = COM_Parse( (const char **)&sv.mLocalSubBSPEntityParsePoint);
		Q_strncpyz( buffer, s, bufferSize );
		if ( !sv.mLocalSubBSPEntityParsePoint && !s[0] )
			return false;
		else
			return true;
	}
}

static void SV_PrecisionTimerStart( void **timer ) {
	timing_c *newTimer = new timing_c; //create the new timer
	*timer = newTimer; //assign the pointer within the pointer to point at the mem addr of our new timer
	newTimer->Start(); //start the timer
}

static int SV_PrecisionTimerEnd( void *timer ) {
	int r;
	timing_c *theTimer = (timing_c *)timer; //this is the pointer we assigned in start, so we can directly cast it back
	r = theTimer->End(); //get the result
	delete theTimer; //delete the timer since we're done with it
	return r; //return the result
}

static void SV_RegisterSharedMemory( char *memory ) {
	sv.mSharedMemory = memory;
}

static void SV_SetServerCull( float cullDistance ) {
	g_svCullDist = cullDistance;
}

bool SV_ROFF_Clean( void ) {
	return theROFFSystem.Clean( false );
}

void SV_ROFF_UpdateEntities( void ) {
	theROFFSystem.UpdateEntities( false );
}

int SV_ROFF_Cache( char *file ) {
	return theROFFSystem.Cache( file, false );
}

bool SV_ROFF_Play( int entID, int roffID, bool doTranslation ) {
	return theROFFSystem.Play( entID, roffID, doTranslation, false );
}

bool SV_ROFF_Purge_Ent( int entID ) {
	return theROFFSystem.PurgeEnt( entID, false );
}

static bool SV_ICARUS_RegisterScript( const char *name, bool bCalledDuringInterrogate ) {
	return (bool)ICARUS_RegisterScript( name, bCalledDuringInterrogate );
}

static bool SV_ICARUS_ValidEnt( sharedEntity_t *ent ) {
	return (bool)ICARUS_ValidEnt( ent );
}

static bool ICARUS_IsInitialized( int entID ) {
	if ( !gSequencers[entID] || !gTaskManagers[entID] )
		return false;

	return true;
}

static bool ICARUS_MaintainTaskManager( int entID ) {
	if ( gTaskManagers[entID] ) {
		gTaskManagers[entID]->Update();
		return true;
	}
	return false;
}

static bool ICARUS_IsRunning( int entID ) {
	if ( !gTaskManagers[entID] || !gTaskManagers[entID]->IsRunning() )
		return false;
	return true;
}

static bool ICARUS_TaskIDPending( sharedEntity_t *ent, int taskID ) {
	return Q3_TaskIDPending( ent, (taskID_e)taskID );
}

static void SV_ICARUS_TaskIDSet( sharedEntity_t *ent, int taskType, int taskID ) {
	Q3_TaskIDSet( ent, (taskID_e)taskType, taskID );
}

static void SV_ICARUS_TaskIDComplete( sharedEntity_t *ent, int taskType ) {
	Q3_TaskIDComplete( ent, (taskID_e)taskType );
}

static int SV_ICARUS_GetStringVariable( const char *name, const char *value ) {
	const char *rec = (const char *)value;
	return Q3_GetStringVariable( name, (const char **)&rec );
}
static int SV_ICARUS_GetVectorVariable( const char *name, const vec3_t value ) {
	return Q3_GetVectorVariable( name, (float *)value );
}

static int SV_BotLibSetup( void ) {
	return botlib_export->BotLibSetup();
}

static int SV_BotLibShutdown( void ) {
	return botlib_export->BotLibShutdown();
}

static int SV_BotLibVarSet( char *var_name, char *value ) {
	return botlib_export->BotLibVarSet( var_name, value );
}

static int SV_BotLibVarGet( char *var_name, char *value, int size ) {
	return botlib_export->BotLibVarGet( var_name, value, size );
}

static int SV_BotLibDefine( char *string ) {
	return botlib_export->PC_AddGlobalDefine( string );
}

static int SV_BotLibStartFrame( float time ) {
	return botlib_export->BotLibStartFrame( time );
}

static int SV_BotLibLoadMap( const char *mapname ) {
	return botlib_export->BotLibLoadMap( mapname );
}

static int SV_BotLibUpdateEntity( int ent, void *bue ) {
	return botlib_export->BotLibUpdateEntity( ent, (bot_entitystate_t *)bue );
}

static int SV_BotLibTest( int parm0, char *parm1, vec3_t parm2, vec3_t parm3 ) {
	return botlib_export->Test( parm0, parm1, parm2, parm3 );
}

static int SV_BotGetServerCommand( int clientNum, char *message, int size ) {
	return SV_BotGetConsoleMessage( clientNum, message, size );
}

static void SV_BotUserCommand( int clientNum, usercmd_t *ucmd ) {
	SV_ClientThink( &svs.clients[clientNum], ucmd );
}

static void SV_EA_Say( int client, const char *str ) {
	botlib_export->ea.EA_Say( client, str );
}

static void SV_EA_SayTeam( int client, const char *str ) {
	botlib_export->ea.EA_SayTeam( client, str );
}

static void SV_EA_Command( int client, const char *command ) {
	botlib_export->ea.EA_Command( client, command );
}

static void SV_EA_Action( int client, int action ) {
	botlib_export->ea.EA_Action( client, action );
}

static void SV_EA_Gesture( int client ) {
	botlib_export->ea.EA_Gesture( client );
}

static void SV_EA_Talk( int client ) {
	botlib_export->ea.EA_Talk( client );
}

static void SV_EA_Attack( int client ) {
	botlib_export->ea.EA_Attack( client );
}

static void SV_EA_Alt_Attack( int client ) {
	botlib_export->ea.EA_Alt_Attack( client );
}

static void SV_EA_ForcePower( int client ) {
	botlib_export->ea.EA_ForcePower( client );
}

static void SV_EA_Use( int client ) {
	botlib_export->ea.EA_Use( client );
}

static void SV_EA_Respawn( int client ) {
	botlib_export->ea.EA_Respawn( client );
}

static void SV_EA_Crouch( int client ) {
	botlib_export->ea.EA_Crouch( client );
}

static void SV_EA_MoveUp( int client ) {
	botlib_export->ea.EA_MoveUp( client );
}

static void SV_EA_MoveDown( int client ) {
	botlib_export->ea.EA_MoveDown( client );
}

static void SV_EA_MoveForward( int client ) {
	botlib_export->ea.EA_MoveForward( client );
}

static void SV_EA_MoveBack( int client ) {
	botlib_export->ea.EA_MoveBack( client );
}

static void SV_EA_MoveLeft( int client ) {
	botlib_export->ea.EA_MoveLeft( client );
}

static void SV_EA_MoveRight( int client ) {
	botlib_export->ea.EA_MoveRight( client );
}

static void SV_EA_Walk( int client ) {
	botlib_export->ea.EA_Walk( client );
}

static void SV_EA_SelectWeapon( int client, int weapon ) {
	botlib_export->ea.EA_SelectWeapon( client, weapon );
}

static void SV_EA_Jump( int client ) {
	botlib_export->ea.EA_Jump( client );
}

static void SV_EA_DelayedJump( int client ) {
	botlib_export->ea.EA_DelayedJump( client );
}

static void SV_EA_Move( int client, vec3_t dir, float speed ) {
	botlib_export->ea.EA_Move( client, dir, speed );
}

static void SV_EA_View( int client, vec3_t viewangles ) {
	botlib_export->ea.EA_View( client, viewangles );
}

static void SV_EA_EndRegular( int client, float thinktime ) {
	botlib_export->ea.EA_EndRegular( client, thinktime );
}

static void SV_EA_GetInput( int client, float thinktime, void *input ) {
	botlib_export->ea.EA_GetInput( client, thinktime, (bot_input_t *)input );
}

static void SV_EA_ResetInput( int client ) {
	botlib_export->ea.EA_ResetInput( client );
}

static int SV_PC_LoadSource( const char *filename ) {
	return botlib_export->PC_LoadSourceHandle( filename );
}

static int SV_PC_FreeSource( int handle ) {
	return botlib_export->PC_FreeSourceHandle( handle );
}

static int SV_PC_ReadToken( int handle, pc_token_t *pc_token ) {
	return botlib_export->PC_ReadTokenHandle( handle, pc_token );
}

static int SV_PC_SourceFileAndLine( int handle, char *filename, int *line, int sizeFilename ) {
	return botlib_export->PC_SourceFileAndLine( handle, filename, line, sizeFilename );
}

static qhandle_t SV_RE_RegisterSkin( const char *name ) {
	return re->RegisterServerSkin( name );
}

static int SV_CM_RegisterTerrain( const char *config ) {
	return 0;
}

static void SV_RMG_Init( void ) { }

static void SV_G2API_ListModelSurfaces( void *ghlInfo ) {
	re->G2API_ListSurfaces( (CGhoul2Info *)ghlInfo );
}

static void SV_G2API_ListModelBones( void *ghlInfo, int frame ) {
	re->G2API_ListBones( (CGhoul2Info *)ghlInfo, frame );
}

static void SV_G2API_SetGhoul2ModelIndexes( void *ghoul2, qhandle_t *modelList, qhandle_t *skinList ) {
	if ( !ghoul2 ) return;
	re->G2API_SetGhoul2ModelIndexes( *((CGhoul2Info_v *)ghoul2), modelList, skinList );
}

static bool SV_G2API_HaveWeGhoul2Models( void *ghoul2) {
	if ( !ghoul2 ) return false;
	return re->G2API_HaveWeGhoul2Models( *((CGhoul2Info_v *)ghoul2) );
}

static bool SV_G2API_GetBoltMatrix( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale ) {
	if ( !ghoul2 ) return false;
	return re->G2API_GetBoltMatrix( *((CGhoul2Info_v *)ghoul2), modelIndex, boltIndex, matrix, angles, position, frameNum, modelList, scale );
}

static bool SV_G2API_GetBoltMatrix_NoReconstruct( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale ) {
	if ( !ghoul2 ) return false;
	re->G2API_BoltMatrixReconstruction( false );
	return re->G2API_GetBoltMatrix( *((CGhoul2Info_v *)ghoul2), modelIndex, boltIndex, matrix, angles, position, frameNum, modelList, scale );
}

static bool SV_G2API_GetBoltMatrix_NoRecNoRot( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale ) {
	if ( !ghoul2 ) return false;
	re->G2API_BoltMatrixReconstruction( false );
	re->G2API_BoltMatrixSPMethod( true );
	return re->G2API_GetBoltMatrix( *((CGhoul2Info_v *)ghoul2), modelIndex, boltIndex, matrix, angles, position, frameNum, modelList, scale );
}

static int SV_G2API_InitGhoul2Model( void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin, qhandle_t customShader, int modelFlags, int lodBias ) {
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 1;
#endif
	return re->G2API_InitGhoul2Model( (CGhoul2Info_v **)ghoul2Ptr, fileName, modelIndex, customSkin, customShader, modelFlags, lodBias );
}

static bool SV_G2API_SetSkin( void *ghoul2, int modelIndex, qhandle_t customSkin, qhandle_t renderSkin ) {
	if ( !ghoul2 ) return false;
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	return re->G2API_SetSkin( g2, modelIndex, customSkin, renderSkin );
}

static void SV_G2API_CollisionDetect( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius ) {
	if ( !ghoul2 ) return;
	re->G2API_CollisionDetect( collRecMap, *((CGhoul2Info_v *)ghoul2), angles, position, frameNumber, entNum, rayStart, rayEnd, scale, G2VertSpaceServer, traceFlags, useLod, fRadius );
}

static void SV_G2API_CollisionDetectCache( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius ) {
	if ( !ghoul2 ) return;
	re->G2API_CollisionDetectCache( collRecMap, *((CGhoul2Info_v *)ghoul2), angles, position, frameNumber, entNum, rayStart, rayEnd, scale, G2VertSpaceServer, traceFlags, useLod, fRadius );
}

static void SV_G2API_CleanGhoul2Models( void **ghoul2Ptr ) {
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 1;
#endif
	re->G2API_CleanGhoul2Models( (CGhoul2Info_v **)ghoul2Ptr );
}

static bool SV_G2API_SetBoneAngles( void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags, const int up, const int right, const int forward, qhandle_t *modelList, int blendTime , int currentTime ) {
	if ( !ghoul2 ) return false;
	return re->G2API_SetBoneAngles( *((CGhoul2Info_v *)ghoul2), modelIndex, boneName, angles, flags, (const Eorientations_e)up, (const Eorientations_e)right, (const Eorientations_e)forward, modelList, blendTime , currentTime );
}

static bool SV_G2API_SetBoneAnim( void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime ) {
	if ( !ghoul2 ) return false;
	return re->G2API_SetBoneAnim( *((CGhoul2Info_v *)ghoul2), modelIndex, boneName, startFrame, endFrame, flags, animSpeed, currentTime, setFrame, blendTime );
}

static bool SV_G2API_GetBoneAnim( void *ghoul2, const char *boneName, const int currentTime, float *currentFrame, int *startFrame, int *endFrame, int *flags, float *animSpeed, int *modelList, const int modelIndex ) {
	if ( !ghoul2 ) return false;
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	return re->G2API_GetBoneAnim( g2, modelIndex, boneName, currentTime, currentFrame, startFrame, endFrame, flags, animSpeed, modelList );
}

static void SV_G2API_GetGLAName( void *ghoul2, int modelIndex, char *fillBuf, int fillBufSize) {
	if ( !ghoul2 )
	{
		fillBuf[0] = '\0';
		return;
	}

	char *tmp = re->G2API_GetGLAName( *((CGhoul2Info_v *)ghoul2), modelIndex );
	if ( tmp )
		Q_strncpyz( fillBuf, tmp, fillBufSize);
	else
		fillBuf[0] = '\0';
}

static int SV_G2API_CopyGhoul2Instance( void *g2From, void *g2To, int modelIndex ) {
	if ( !g2From || !g2To ) return 0;
	return re->G2API_CopyGhoul2Instance( *((CGhoul2Info_v *)g2From), *((CGhoul2Info_v *)g2To), modelIndex );
}

static void SV_G2API_CopySpecificGhoul2Model( void *g2From, int modelFrom, void *g2To, int modelTo ) {
	if ( !g2From || !g2To ) return;
	re->G2API_CopySpecificG2Model( *((CGhoul2Info_v *)g2From), modelFrom, *((CGhoul2Info_v *)g2To), modelTo );
}

static void SV_G2API_DuplicateGhoul2Instance( void *g2From, void **g2To ) {
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 1;
#endif
	if ( !g2From || !g2To ) return;
	re->G2API_DuplicateGhoul2Instance( *((CGhoul2Info_v *)g2From), (CGhoul2Info_v **)g2To );
}

static bool SV_G2API_HasGhoul2ModelOnIndex( void *ghlInfo, int modelIndex ) {
	return re->G2API_HasGhoul2ModelOnIndex( (CGhoul2Info_v **)ghlInfo, modelIndex );
}

static bool SV_G2API_RemoveGhoul2Model( void *ghlInfo, int modelIndex ) {
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 1;
#endif
	return re->G2API_RemoveGhoul2Model( (CGhoul2Info_v **)ghlInfo, modelIndex );
}

static bool SV_G2API_RemoveGhoul2Models( void *ghlInfo ) {
#ifdef _FULL_G2_LEAK_CHECKING
	g_G2AllocServer = 1;
#endif
	return re->G2API_RemoveGhoul2Models( (CGhoul2Info_v **)ghlInfo );
}

static int SV_G2API_Ghoul2Size( void *ghlInfo ) {
	if ( !ghlInfo ) return 0;
	return re->G2API_Ghoul2Size( *((CGhoul2Info_v *)ghlInfo) );
}

static int SV_G2API_AddBolt( void *ghoul2, int modelIndex, const char *boneName ) {
	if ( !ghoul2 ) return -1;
	return re->G2API_AddBolt( *((CGhoul2Info_v *)ghoul2), modelIndex, boneName );
}

static void SV_G2API_SetBoltInfo( void *ghoul2, int modelIndex, int boltInfo ) {
	if ( !ghoul2 ) return;
	re->G2API_SetBoltInfo( *((CGhoul2Info_v *)ghoul2), modelIndex, boltInfo );
}

static bool SV_G2API_SetRootSurface( void *ghoul2, const int modelIndex, const char *surfaceName ) {
	if ( !ghoul2 ) return false;
	return re->G2API_SetRootSurface( *((CGhoul2Info_v *)ghoul2), modelIndex, surfaceName );
}

static bool SV_G2API_SetSurfaceOnOff( void *ghoul2, const char *surfaceName, const int flags ) {
	if ( !ghoul2 ) return false;
	return re->G2API_SetSurfaceOnOff( *((CGhoul2Info_v *)ghoul2), surfaceName, flags );
}

static bool SV_G2API_SetNewOrigin( void *ghoul2, const int boltIndex ) {
	if ( !ghoul2 ) return false;
	return re->G2API_SetNewOrigin( *((CGhoul2Info_v *)ghoul2), boltIndex );
}

static bool SV_G2API_DoesBoneExist( void *ghoul2, int modelIndex, const char *boneName ) {
	if ( !ghoul2 ) return false;
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	return re->G2API_DoesBoneExist( g2, modelIndex, boneName );
}

static int SV_G2API_GetSurfaceRenderStatus( void *ghoul2, const int modelIndex, const char *surfaceName ) {
	if ( !ghoul2 ) return -1;
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	return re->G2API_GetSurfaceRenderStatus( g2, modelIndex, surfaceName );
}

static void SV_G2API_AbsurdSmoothing( void *ghoul2, bool status ) {
	if ( !ghoul2 ) return;
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	re->G2API_AbsurdSmoothing( g2, status );
}

static void SV_G2API_SetRagDoll( void *ghoul2, sharedRagDollParams_t *params ) {
	if ( !ghoul2 ) return;

	CRagDollParams rdParams;

	if ( !params ) {
		re->G2API_ResetRagDoll( *((CGhoul2Info_v *)ghoul2) );
		return;
	}

	VectorCopy( params->angles, rdParams.angles );
	VectorCopy( params->position, rdParams.position );
	VectorCopy( params->scale, rdParams.scale );
	VectorCopy( params->pelvisAnglesOffset, rdParams.pelvisAnglesOffset );
	VectorCopy( params->pelvisPositionOffset, rdParams.pelvisPositionOffset );

	rdParams.fImpactStrength = params->fImpactStrength;
	rdParams.fShotStrength = params->fShotStrength;
	rdParams.me = params->me;

	rdParams.startFrame = params->startFrame;
	rdParams.endFrame = params->endFrame;

	rdParams.collisionType = params->collisionType;
	rdParams.CallRagDollBegin = params->CallRagDollBegin;

	rdParams.RagPhase = (CRagDollParams::ERagPhase_e)params->RagPhase;
	rdParams.effectorsToTurnOff = (CRagDollParams::ERagEffector_e)params->effectorsToTurnOff;

	re->G2API_SetRagDoll( *((CGhoul2Info_v *)ghoul2), &rdParams );
}

static void SV_G2API_AnimateG2Models( void *ghoul2, int time, sharedRagDollUpdateParams_t *params ) {
	CRagDollUpdateParams rduParams;

	if ( !params )
		return;

	VectorCopy( params->angles, rduParams.angles );
	VectorCopy( params->position, rduParams.position );
	VectorCopy( params->scale, rduParams.scale );
	VectorCopy( params->velocity, rduParams.velocity );

	rduParams.me = params->me;
	rduParams.settleFrame = params->settleFrame;

	re->G2API_AnimateG2ModelsRag( *((CGhoul2Info_v *)ghoul2), time, &rduParams );
}

static bool SV_G2API_RagPCJConstraint( void *ghoul2, const char *boneName, vec3_t min, vec3_t max ) {
	return re->G2API_RagPCJConstraint( *((CGhoul2Info_v *)ghoul2), boneName, min, max );
}

static bool SV_G2API_RagPCJGradientSpeed( void *ghoul2, const char *boneName, const float speed ) {
	return re->G2API_RagPCJGradientSpeed( *((CGhoul2Info_v *)ghoul2), boneName, speed );
}

static bool SV_G2API_RagEffectorGoal( void *ghoul2, const char *boneName, vec3_t pos ) {
	return re->G2API_RagEffectorGoal( *((CGhoul2Info_v *)ghoul2), boneName, pos );
}

static bool SV_G2API_GetRagBonePos( void *ghoul2, const char *boneName, vec3_t pos, vec3_t entAngles, vec3_t entPos, vec3_t entScale ) {
	return re->G2API_GetRagBonePos( *((CGhoul2Info_v *)ghoul2), boneName, pos, entAngles, entPos, entScale );
}

static bool SV_G2API_RagEffectorKick( void *ghoul2, const char *boneName, vec3_t velocity ) {
	return re->G2API_RagEffectorKick( *((CGhoul2Info_v *)ghoul2), boneName, velocity );
}

static bool SV_G2API_RagForceSolve( void *ghoul2, bool force ) {
	return re->G2API_RagForceSolve( *((CGhoul2Info_v *)ghoul2), force );
}

static bool SV_G2API_SetBoneIKState( void *ghoul2, int time, const char *boneName, int ikState, sharedSetBoneIKStateParams_t *params ) {
	return re->G2API_SetBoneIKState( *((CGhoul2Info_v *)ghoul2), time, boneName, ikState, params );
}

static bool SV_G2API_IKMove( void *ghoul2, int time, sharedIKMoveParams_t *params ) {
	return re->G2API_IKMove( *((CGhoul2Info_v *)ghoul2), time, params );
}

static bool SV_G2API_RemoveBone( void *ghoul2, const char *boneName, int modelIndex ) {
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	return re->G2API_RemoveBone( g2, modelIndex, boneName );
}

static void SV_G2API_AttachInstanceToEntNum( void *ghoul2, int entityNum, bool server ) {
	re->G2API_AttachInstanceToEntNum( *((CGhoul2Info_v *)ghoul2), entityNum, server );
}

static void SV_G2API_ClearAttachedInstance( int entityNum ) {
	re->G2API_ClearAttachedInstance( entityNum );
}

static void SV_G2API_CleanEntAttachments( void ) {
	re->G2API_CleanEntAttachments();
}

static bool SV_G2API_OverrideServer( void *serverInstance ) {
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)serverInstance);
	return re->G2API_OverrideServerWithClientData( g2, 0 );
}

static void SV_G2API_GetSurfaceName( void *ghoul2, int surfNumber, int modelIndex, char *fillBuf, int fillBufSize ) {
	CGhoul2Info_v &g2 = *((CGhoul2Info_v *)ghoul2);
	char *tmp = re->G2API_GetSurfaceName( g2, modelIndex, surfNumber );
	Q_strncpyz( fillBuf, tmp, fillBufSize );
}

static void GVM_Cvar_Set( const char *var_name, const char *value ) {
	Cvar_VM_Set( var_name, value, VM_GAME );
}

void SV_InitGame( bool restart ) {
	int i=0;
	client_t *cl = nullptr;

	// clear level pointers
	sv.entityParsePoint = CM_EntityString();
	for ( i=0, cl=svs.clients; i<sv_maxclients->integer; i++, cl++ )
		cl->gentity = nullptr;

	GVM_InitGame( sv.time, Com_Milliseconds(), restart );
}

void SV_BindGame( void ) {
	static gameImport_t gi;
	gameExport_t		*ret;
	GetGameAPI_t		GetGameAPI;
	char				dllName[MAX_OSPATH] = "jampgame" ARCH_STRING DLL_EXT;

	memset( &gi, 0, sizeof( gi ) );

	gvm = VM_Create( VM_GAME );
	if ( !gvm ) {
		svs.gameStarted = false;
		Com_Error( ERR_DROP, "VM_Create on game failed" );
		return;
	}

	gi.Print								= Com_Printf;
	gi.Error								= Com_Error;
	gi.Milliseconds							= Com_Milliseconds;
	gi.PrecisionTimerStart					= SV_PrecisionTimerStart;
	gi.PrecisionTimerEnd					= SV_PrecisionTimerEnd;
	gi.SV_RegisterSharedMemory				= SV_RegisterSharedMemory;
	gi.RealTime								= Com_RealTime;
	gi.TrueMalloc							= VM_Shifted_Alloc;
	gi.TrueFree								= VM_Shifted_Free;
	gi.SnapVector							= Sys_SnapVector;
	gi.Cvar_Register						= Cvar_Register;
	gi.Cvar_Set								= GVM_Cvar_Set;
	gi.Cvar_Update							= Cvar_Update;
	gi.Cvar_VariableIntegerValue			= Cvar_VariableIntegerValue;
	gi.Cvar_VariableStringBuffer			= Cvar_VariableStringBuffer;
	gi.Argc									= Cmd_Argc;
	gi.Argv									= Cmd_ArgvBuffer;
	gi.FS_Close								= FS_FCloseFile;
	gi.FS_GetFileList						= FS_GetFileList;
	gi.FS_Open								= FS_FOpenFileByMode;
	gi.FS_Read								= FS_Read;
	gi.FS_Write								= FS_Write;
	gi.AdjustAreaPortalState				= SV_AdjustAreaPortalState;
	gi.AreasConnected						= CM_AreasConnected;
	gi.DebugPolygonCreate					= BotImport_DebugPolygonCreate;
	gi.DebugPolygonDelete					= BotImport_DebugPolygonDelete;
	gi.DropClient							= SV_GameDropClient;
	gi.EntitiesInBox						= SV_AreaEntities;
	gi.EntityContact						= SV_EntityContact;
	gi.Trace								= SV_Trace;
	gi.TraceEntity							= SV_ClipToEntity;
	gi.GetConfigstring						= SV_GetConfigstring;
	gi.GetEntityToken						= SV_GetEntityToken;
	gi.GetServerinfo						= SV_GetServerinfo;
	gi.GetUsercmd							= SV_GetUsercmd;
	gi.GetUserinfo							= SV_GetUserinfo;
	gi.InPVS								= SV_inPVS;
	gi.InPVSIgnorePortals					= SV_inPVSIgnorePortals;
	gi.LinkEntity							= SV_LinkEntity;
	gi.LocateGameData						= SV_LocateGameData;
	gi.PointContents						= SV_PointContents;
	gi.SendConsoleCommand					= Cbuf_ExecuteText;
	gi.SendServerCommand					= SV_GameSendServerCommand;
	gi.SetBrushModel						= SV_SetBrushModel;
	gi.SetConfigstring						= SV_SetConfigstring;
	gi.SetServerCull						= SV_SetServerCull;
	gi.SetUserinfo							= SV_SetUserinfo;
	gi.UnlinkEntity							= SV_UnlinkEntity;
	gi.ROFF_Clean							= SV_ROFF_Clean;
	gi.ROFF_UpdateEntities					= SV_ROFF_UpdateEntities;
	gi.ROFF_Cache							= SV_ROFF_Cache;
	gi.ROFF_Play							= SV_ROFF_Play;
	gi.ROFF_Purge_Ent						= SV_ROFF_Purge_Ent;
	gi.ICARUS_RunScript						= ICARUS_RunScript;
	gi.ICARUS_RegisterScript				= SV_ICARUS_RegisterScript;
	gi.ICARUS_Init							= ICARUS_Init;
	gi.ICARUS_ValidEnt						= SV_ICARUS_ValidEnt;
	gi.ICARUS_IsInitialized					= ICARUS_IsInitialized;
	gi.ICARUS_MaintainTaskManager			= ICARUS_MaintainTaskManager;
	gi.ICARUS_IsRunning						= ICARUS_IsRunning;
	gi.ICARUS_TaskIDPending					= ICARUS_TaskIDPending;
	gi.ICARUS_InitEnt						= ICARUS_InitEnt;
	gi.ICARUS_FreeEnt						= ICARUS_FreeEnt;
	gi.ICARUS_AssociateEnt					= ICARUS_AssociateEnt;
	gi.ICARUS_Shutdown						= ICARUS_Shutdown;
	gi.ICARUS_TaskIDSet						= SV_ICARUS_TaskIDSet;
	gi.ICARUS_TaskIDComplete				= SV_ICARUS_TaskIDComplete;
	gi.ICARUS_SetVar						= Q3_SetVar;
	gi.ICARUS_VariableDeclared				= Q3_VariableDeclared;
	gi.ICARUS_GetFloatVariable				= Q3_GetFloatVariable;
	gi.ICARUS_GetStringVariable				= SV_ICARUS_GetStringVariable;
	gi.ICARUS_GetVectorVariable				= SV_ICARUS_GetVectorVariable;
	gi.BotAllocateClient					= SV_BotAllocateClient;
	gi.BotFreeClient						= SV_BotFreeClient;

	gi.BotLibSetup							= SV_BotLibSetup;
	gi.BotLibShutdown						= SV_BotLibShutdown;
	gi.BotLibVarSet							= SV_BotLibVarSet;
	gi.BotLibVarGet							= SV_BotLibVarGet;
	gi.BotLibDefine							= SV_BotLibDefine;
	gi.BotLibStartFrame						= SV_BotLibStartFrame;
	gi.BotLibLoadMap						= SV_BotLibLoadMap;
	gi.BotLibUpdateEntity					= SV_BotLibUpdateEntity;
	gi.BotLibTest							= SV_BotLibTest;
	gi.BotGetSnapshotEntity					= SV_BotGetSnapshotEntity;
	gi.BotGetServerCommand					= SV_BotGetServerCommand;
	gi.BotUserCommand						= SV_BotUserCommand;
	gi.BotUpdateWaypoints					= SV_BotWaypointReception;
	gi.BotCalculatePaths					= SV_BotCalculatePaths;

	gi.EA_Say								= SV_EA_Say;
	gi.EA_SayTeam							= SV_EA_SayTeam;
	gi.EA_Command							= SV_EA_Command;
	gi.EA_Action							= SV_EA_Action;
	gi.EA_Gesture							= SV_EA_Gesture;
	gi.EA_Talk								= SV_EA_Talk;
	gi.EA_Attack							= SV_EA_Attack;
	gi.EA_Alt_Attack						= SV_EA_Alt_Attack;
	gi.EA_ForcePower						= SV_EA_ForcePower;
	gi.EA_Use								= SV_EA_Use;
	gi.EA_Respawn							= SV_EA_Respawn;
	gi.EA_Crouch							= SV_EA_Crouch;
	gi.EA_MoveUp							= SV_EA_MoveUp;
	gi.EA_MoveDown							= SV_EA_MoveDown;
	gi.EA_MoveForward						= SV_EA_MoveForward;
	gi.EA_MoveBack							= SV_EA_MoveBack;
	gi.EA_MoveLeft							= SV_EA_MoveLeft;
	gi.EA_MoveRight							= SV_EA_MoveRight;
	gi.EA_Walk								= SV_EA_Walk;
	gi.EA_SelectWeapon						= SV_EA_SelectWeapon;
	gi.EA_Jump								= SV_EA_Jump;
	gi.EA_DelayedJump						= SV_EA_DelayedJump;
	gi.EA_Move								= SV_EA_Move;
	gi.EA_View								= SV_EA_View;
	gi.EA_EndRegular						= SV_EA_EndRegular;
	gi.EA_GetInput							= SV_EA_GetInput;
	gi.EA_ResetInput						= SV_EA_ResetInput;
	gi.PC_LoadSource						= SV_PC_LoadSource;
	gi.PC_FreeSource						= SV_PC_FreeSource;
	gi.PC_ReadToken							= SV_PC_ReadToken;
	gi.PC_SourceFileAndLine					= SV_PC_SourceFileAndLine;
	gi.R_RegisterSkin						= SV_RE_RegisterSkin;
	gi.SetActiveSubBSP						= SV_SetActiveSubBSP;
	gi.CM_RegisterTerrain					= SV_CM_RegisterTerrain;
	gi.RMG_Init								= SV_RMG_Init;
	gi.G2API_ListModelBones					= SV_G2API_ListModelBones;
	gi.G2API_ListModelSurfaces				= SV_G2API_ListModelSurfaces;
	gi.G2API_HaveWeGhoul2Models				= SV_G2API_HaveWeGhoul2Models;
	gi.G2API_SetGhoul2ModelIndexes			= SV_G2API_SetGhoul2ModelIndexes;
	gi.G2API_GetBoltMatrix					= SV_G2API_GetBoltMatrix;
	gi.G2API_GetBoltMatrix_NoReconstruct	= SV_G2API_GetBoltMatrix_NoReconstruct;
	gi.G2API_GetBoltMatrix_NoRecNoRot		= SV_G2API_GetBoltMatrix_NoRecNoRot;
	gi.G2API_InitGhoul2Model				= SV_G2API_InitGhoul2Model;
	gi.G2API_SetSkin						= SV_G2API_SetSkin;
	gi.G2API_Ghoul2Size						= SV_G2API_Ghoul2Size;
	gi.G2API_AddBolt						= SV_G2API_AddBolt;
	gi.G2API_SetBoltInfo					= SV_G2API_SetBoltInfo;
	gi.G2API_SetBoneAngles					= SV_G2API_SetBoneAngles;
	gi.G2API_SetBoneAnim					= SV_G2API_SetBoneAnim;
	gi.G2API_GetBoneAnim					= SV_G2API_GetBoneAnim;
	gi.G2API_GetGLAName						= SV_G2API_GetGLAName;
	gi.G2API_CopyGhoul2Instance				= SV_G2API_CopyGhoul2Instance;
	gi.G2API_CopySpecificGhoul2Model		= SV_G2API_CopySpecificGhoul2Model;
	gi.G2API_DuplicateGhoul2Instance		= SV_G2API_DuplicateGhoul2Instance;
	gi.G2API_HasGhoul2ModelOnIndex			= SV_G2API_HasGhoul2ModelOnIndex;
	gi.G2API_RemoveGhoul2Model				= SV_G2API_RemoveGhoul2Model;
	gi.G2API_RemoveGhoul2Models				= SV_G2API_RemoveGhoul2Models;
	gi.G2API_CleanGhoul2Models				= SV_G2API_CleanGhoul2Models;
	gi.G2API_CollisionDetect				= SV_G2API_CollisionDetect;
	gi.G2API_CollisionDetectCache			= SV_G2API_CollisionDetectCache;
	gi.G2API_SetRootSurface					= SV_G2API_SetRootSurface;
	gi.G2API_SetSurfaceOnOff				= SV_G2API_SetSurfaceOnOff;
	gi.G2API_SetNewOrigin					= SV_G2API_SetNewOrigin;
	gi.G2API_DoesBoneExist					= SV_G2API_DoesBoneExist;
	gi.G2API_GetSurfaceRenderStatus			= SV_G2API_GetSurfaceRenderStatus;
	gi.G2API_AbsurdSmoothing				= SV_G2API_AbsurdSmoothing;
	gi.G2API_SetRagDoll						= SV_G2API_SetRagDoll;
	gi.G2API_AnimateG2Models				= SV_G2API_AnimateG2Models;
	gi.G2API_RagPCJConstraint				= SV_G2API_RagPCJConstraint;
	gi.G2API_RagPCJGradientSpeed			= SV_G2API_RagPCJGradientSpeed;
	gi.G2API_RagEffectorGoal				= SV_G2API_RagEffectorGoal;
	gi.G2API_GetRagBonePos					= SV_G2API_GetRagBonePos;
	gi.G2API_RagEffectorKick				= SV_G2API_RagEffectorKick;
	gi.G2API_RagForceSolve					= SV_G2API_RagForceSolve;
	gi.G2API_SetBoneIKState					= SV_G2API_SetBoneIKState;
	gi.G2API_IKMove							= SV_G2API_IKMove;
	gi.G2API_RemoveBone						= SV_G2API_RemoveBone;
	gi.G2API_AttachInstanceToEntNum			= SV_G2API_AttachInstanceToEntNum;
	gi.G2API_ClearAttachedInstance			= SV_G2API_ClearAttachedInstance;
	gi.G2API_CleanEntAttachments			= SV_G2API_CleanEntAttachments;
	gi.G2API_OverrideServer					= SV_G2API_OverrideServer;
	gi.G2API_GetSurfaceName					= SV_G2API_GetSurfaceName;

	GetGameAPI = (GetGameAPI_t)gvm->GetModuleAPI;
	ret = GetGameAPI( GAME_API_VERSION, &gi );
	if ( !ret ) {
		//free VM?
		svs.gameStarted = false;
		Com_Error( ERR_FATAL, "GetGameAPI failed on %s", dllName );
		return;
	}
	ge = ret;
}

void SV_UnbindGame( void ) {
	GVM_ShutdownGame( false );
	VM_Free( gvm );
	gvm = nullptr;
}

void SV_RestartGame( void ) {
	GVM_ShutdownGame( true );

	gvm = VM_Restart( gvm );
	SV_BindGame();
	if ( !gvm ) {
		svs.gameStarted = false;
		Com_Error( ERR_DROP, "VM_Restart on game failed" );
		return;
	}

	SV_InitGame( true );
}
