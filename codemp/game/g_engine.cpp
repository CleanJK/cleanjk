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

#include "game/g_local.hpp"
#include "game/g_ICARUScb.hpp"

NORETURN_PTR void (*Com_Error)( int level, const char *fmt, ... );
void (*Com_Printf)( const char *fmt, ... );

static void G_SpawnRMGEntity( void ) {
	if ( G_ParseSpawnVars( false ) )
		G_SpawnGEntityFromSpawnVars( false );
}

static void _G_ROFF_NotetrackCallback( int entID, const char *notetrack ) {
	G_ROFF_NotetrackCallback( &g_entities[entID], notetrack );
}

static int G_ICARUS_PlaySound( void ) {
	T_G_ICARUS_PLAYSOUND *sharedMem = &gSharedBuffer.playSound;
	return Q3_PlaySound( sharedMem->taskID, sharedMem->entID, sharedMem->name, sharedMem->channel );
}
static bool G_ICARUS_Set( void ) {
	T_G_ICARUS_SET *sharedMem = &gSharedBuffer.set;
	return Q3_Set( sharedMem->taskID, sharedMem->entID, sharedMem->type_name, sharedMem->data );
}
static void G_ICARUS_Lerp2Pos( void ) {
	T_G_ICARUS_LERP2POS *sharedMem = &gSharedBuffer.lerp2Pos;
	Q3_Lerp2Pos( sharedMem->taskID, sharedMem->entID, sharedMem->origin, sharedMem->nullAngles ? nullptr : sharedMem->angles, sharedMem->duration );
}
static void G_ICARUS_Lerp2Origin( void ) {
	T_G_ICARUS_LERP2ORIGIN *sharedMem = &gSharedBuffer.lerp2Origin;
	Q3_Lerp2Origin( sharedMem->taskID, sharedMem->entID, sharedMem->origin, sharedMem->duration );
}
static void G_ICARUS_Lerp2Angles( void ) {
	T_G_ICARUS_LERP2ANGLES *sharedMem = &gSharedBuffer.lerp2Angles;
	Q3_Lerp2Angles( sharedMem->taskID, sharedMem->entID, sharedMem->angles, sharedMem->duration );
}
static int G_ICARUS_GetTag( void ) {
	T_G_ICARUS_GETTAG *sharedMem = &gSharedBuffer.getTag;
	return Q3_GetTag( sharedMem->entID, sharedMem->name, sharedMem->lookup, sharedMem->info );
}
static void G_ICARUS_Lerp2Start( void ) {
	T_G_ICARUS_LERP2START *sharedMem = &gSharedBuffer.lerp2Start;
	Q3_Lerp2Start( sharedMem->entID, sharedMem->taskID, sharedMem->duration );
}
static void G_ICARUS_Lerp2End( void ) {
	T_G_ICARUS_LERP2END *sharedMem = &gSharedBuffer.lerp2End;
	Q3_Lerp2End( sharedMem->entID, sharedMem->taskID, sharedMem->duration );
}
static void G_ICARUS_Use( void ) {
	T_G_ICARUS_USE *sharedMem = &gSharedBuffer.use;
	Q3_Use( sharedMem->entID, sharedMem->target );
}
static void G_ICARUS_Kill( void ) {
	T_G_ICARUS_KILL *sharedMem = &gSharedBuffer.kill;
	Q3_Kill( sharedMem->entID, sharedMem->name );
}
static void G_ICARUS_Remove( void ) {
	T_G_ICARUS_REMOVE *sharedMem = &gSharedBuffer.remove;
	Q3_Remove( sharedMem->entID, sharedMem->name );
}
static void G_ICARUS_Play( void ) {
	T_G_ICARUS_PLAY *sharedMem = &gSharedBuffer.play;
	Q3_Play( sharedMem->taskID, sharedMem->entID, sharedMem->type, sharedMem->name );
}
static int G_ICARUS_GetFloat( void ) {
	T_G_ICARUS_GETFLOAT *sharedMem = &gSharedBuffer.getFloat;
	return Q3_GetFloat( sharedMem->entID, sharedMem->type, sharedMem->name, &sharedMem->value );
}
static int G_ICARUS_GetVector( void ) {
	T_G_ICARUS_GETVECTOR *sharedMem = &gSharedBuffer.getVector;
	return Q3_GetVector( sharedMem->entID, sharedMem->type, sharedMem->name, sharedMem->value );
}
static int G_ICARUS_GetString( void ) {
	T_G_ICARUS_GETSTRING *sharedMem = &gSharedBuffer.getString;
	char *crap = nullptr; //I am sorry for this -rww
	char **morecrap = &crap; //and this
	int r = Q3_GetString( sharedMem->entID, sharedMem->type, sharedMem->name, morecrap );

	if ( crap )
		strcpy( sharedMem->value, crap );

	return r;
}
static void G_ICARUS_SoundIndex( void ) {
	T_G_ICARUS_SOUNDINDEX *sharedMem = &gSharedBuffer.soundIndex;
	G_SoundIndex( sharedMem->filename );
}
static int G_ICARUS_GetSetIDForString( void ) {
	T_G_ICARUS_GETSETIDFORSTRING *sharedMem = &gSharedBuffer.getSetIDForString;
	return GetIDForString( setTable, sharedMem->string );
}

gameImport_t *trap = nullptr;

Q_CABI {
Q_EXPORT gameExport_t* QDECL GetModuleAPI( int apiVersion, gameImport_t *import )
{
	static gameExport_t ge = {0};

	assert( import );
	trap = import;
	Com_Printf	= trap->Print;
	Com_Error	= trap->Error;

	memset( &ge, 0, sizeof( ge ) );

	if ( apiVersion != GAME_API_VERSION ) {
		trap->Print( "Mismatched GAME_API_VERSION: expected %i, got %i\n", GAME_API_VERSION, apiVersion );
		return nullptr;
	}

	ge.InitGame							= G_InitGame;
	ge.ShutdownGame						= G_ShutdownGame;
	ge.ClientConnect					= ClientConnect;
	ge.ClientBegin						= ClientBegin;
	ge.ClientUserinfoChanged			= ClientUserinfoChanged;
	ge.ClientDisconnect					= ClientDisconnect;
	ge.ClientCommand					= ClientCommand;
	ge.ClientThink						= ClientThink;
	ge.RunFrame							= G_RunFrame;
	ge.ConsoleCommand					= ConsoleCommand;
	ge.BotAIStartFrame					= BotAIStartFrame;
	ge.ROFF_NotetrackCallback			= _G_ROFF_NotetrackCallback;
	ge.SpawnRMGEntity					= G_SpawnRMGEntity;
	ge.ICARUS_PlaySound					= G_ICARUS_PlaySound;
	ge.ICARUS_Set						= G_ICARUS_Set;
	ge.ICARUS_Lerp2Pos					= G_ICARUS_Lerp2Pos;
	ge.ICARUS_Lerp2Origin				= G_ICARUS_Lerp2Origin;
	ge.ICARUS_Lerp2Angles				= G_ICARUS_Lerp2Angles;
	ge.ICARUS_GetTag					= G_ICARUS_GetTag;
	ge.ICARUS_Lerp2Start				= G_ICARUS_Lerp2Start;
	ge.ICARUS_Lerp2End					= G_ICARUS_Lerp2End;
	ge.ICARUS_Use						= G_ICARUS_Use;
	ge.ICARUS_Kill						= G_ICARUS_Kill;
	ge.ICARUS_Remove					= G_ICARUS_Remove;
	ge.ICARUS_Play						= G_ICARUS_Play;
	ge.ICARUS_GetFloat					= G_ICARUS_GetFloat;
	ge.ICARUS_GetVector					= G_ICARUS_GetVector;
	ge.ICARUS_GetString					= G_ICARUS_GetString;
	ge.ICARUS_SoundIndex				= G_ICARUS_SoundIndex;
	ge.ICARUS_GetSetIDForString			= G_ICARUS_GetSetIDForString;
	ge.BG_GetItemIndexByTag				= BG_GetItemIndexByTag;

	return &ge;
}
}
