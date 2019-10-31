/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors

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

// g_bot.c

#include "game/g_local.h"
#include "game/ai_main.h"

#define BOT_BEGIN_DELAY_BASE		2000
#define BOT_BEGIN_DELAY_INCREMENT	1500

#define BOT_SPAWN_QUEUE_DEPTH	16

static struct botSpawnQueue_s {
	int		clientNum;
	int		spawnTime;
} botSpawnQueue[BOT_SPAWN_QUEUE_DEPTH];

float trap_Cvar_VariableValue( const char *var_name ) {
	char buf[MAX_CVAR_VALUE_STRING];

	trap->Cvar_VariableStringBuffer(var_name, buf, sizeof(buf));
	return atof(buf);
}

int G_ParseInfos( char *buf, int max, char *infos[] ) {
	char	*token;
	int		count;
	char	key[MAX_TOKEN_CHARS];
	char	info[MAX_INFO_STRING];

	count = 0;

	COM_BeginParseSession ("G_ParseInfos");
	while ( 1 ) {
		token = COM_Parse( (const char **)(&buf) );
		if ( !token[0] ) {
			break;
		}
		if ( strcmp( token, "{" ) ) {
			Com_Printf( "Missing { in info file\n" );
			break;
		}

		if ( count == max ) {
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		info[0] = '\0';
		while ( 1 ) {
			token = COM_ParseExt( (const char **)(&buf), qtrue );
			if ( !token[0] ) {
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}
			if ( !strcmp( token, "}" ) ) {
				break;
			}
			Q_strncpyz( key, token, sizeof( key ) );

			token = COM_ParseExt( (const char **)(&buf), qfalse );
			if ( !token[0] ) {
				strcpy( token, "<NULL>" );
			}
			Info_SetValueForKey( info, key, token );
		}
		//NOTE: extra space for arena number
		infos[count] = (char *) G_Alloc( strlen(info) + strlen("\\num\\") + strlen(va("%d", MAX_ARENAS)) + 1);
		if (infos[count]) {
			strcpy(infos[count], info);
			count++;
		}
	}
	return count;
}

void G_LoadArenasFromFile( char *filename ) {
	int				len;
	fileHandle_t	f;
	char			buf[MAX_ARENAS_TEXT];

	len = trap->FS_Open( filename, &f, FS_READ );
	if ( !f ) {
		trap->Print( S_COLOR_RED "file not found: %s\n", filename );
		return;
	}
	if ( len >= MAX_ARENAS_TEXT ) {
		trap->Print( S_COLOR_RED "file too large: %s is %i, max allowed is %i\n", filename, len, MAX_ARENAS_TEXT );
		trap->FS_Close( f );
		return;
	}

	trap->FS_Read( buf, len, f );
	buf[len] = 0;
	trap->FS_Close( f );

	level.arenas.num += G_ParseInfos( buf, MAX_ARENAS - level.arenas.num, &level.arenas.infos[level.arenas.num] );
}

int G_GetMapTypeBits(char *type)
{
	int typeBits = 0;

	if( *type ) {
		if( strstr( type, "ffa" ) ) {
			typeBits |= (1 << GT_FFA);
			typeBits |= (1 << GT_TEAM);
			typeBits |= (1 << GT_JEDIMASTER);
		}
		if( strstr( type, "holocron" ) ) {
			typeBits |= (1 << GT_HOLOCRON);
		}
		if( strstr( type, "jedimaster" ) ) {
			typeBits |= (1 << GT_JEDIMASTER);
		}
		if( strstr( type, "duel" ) ) {
			typeBits |= (1 << GT_DUEL);
			typeBits |= (1 << GT_POWERDUEL);
		}
		if( strstr( type, "powerduel" ) ) {
			typeBits |= (1 << GT_DUEL);
			typeBits |= (1 << GT_POWERDUEL);
		}
		if( strstr( type, "ctf" ) ) {
			typeBits |= (1 << GT_CTF);
			typeBits |= (1 << GT_CTY);
		}
		if( strstr( type, "cty" ) ) {
			typeBits |= (1 << GT_CTY);
		}
	} else {
		typeBits |= (1 << GT_FFA);
		typeBits |= (1 << GT_JEDIMASTER);
	}

	return typeBits;
}

qboolean G_DoesMapSupportGametype(const char *mapname, int gametype)
{
	int			typeBits = 0;
	int			thisLevel = -1;
	int			n = 0;
	char		*type = NULL;

	if (!level.arenas.infos[0])
	{
		return qfalse;
	}

	if (!mapname || !mapname[0])
	{
		return qfalse;
	}

	for( n = 0; n < level.arenas.num; n++ )
	{
		type = Info_ValueForKey( level.arenas.infos[n], "map" );

		if (Q_stricmp(mapname, type) == 0)
		{
			thisLevel = n;
			break;
		}
	}

	if (thisLevel == -1)
	{
		return qfalse;
	}

	type = Info_ValueForKey(level.arenas.infos[thisLevel], "type");

	typeBits = G_GetMapTypeBits(type);
	if (typeBits & (1 << gametype))
	{ //the map in question supports the gametype in question, so..
		return qtrue;
	}

	return qfalse;
}

//rww - auto-obtain nextmap. I could've sworn Q3 had something like this, but I guess not.
const char *G_RefreshNextMap(int gametype, qboolean forced)
{
	int			typeBits = 0;
	int			thisLevel = 0;
	int			desiredMap = 0;
	int			n = 0;
	char		*type = NULL;
	qboolean	loopingUp = qfalse;

	if (!g_autoMapCycle.integer && !forced)
	{
		return NULL;
	}

	if (!level.arenas.infos[0])
	{
		return NULL;
	}

	for( n = 0; n < level.arenas.num; n++ )
	{
		type = Info_ValueForKey( level.arenas.infos[n], "map" );

		if (Q_stricmp(mapname.string, type) == 0)
		{
			thisLevel = n;
			break;
		}
	}

	desiredMap = thisLevel;

	n = thisLevel+1;
	while (n != thisLevel)
	{ //now cycle through the arena list and find the next map that matches the gametype we're in
		if (!level.arenas.infos[n] || n >= level.arenas.num)
		{
			if (loopingUp)
			{ //this shouldn't happen, but if it does we have a null entry break in the arena file
			  //if this is the case just break out of the loop instead of sticking in an infinite loop
				break;
			}
			n = 0;
			loopingUp = qtrue;
		}

		type = Info_ValueForKey(level.arenas.infos[n], "type");

		typeBits = G_GetMapTypeBits(type);
		if (typeBits & (1 << gametype))
		{
			desiredMap = n;
			break;
		}

		n++;
	}

	if (desiredMap == thisLevel)
	{ //If this is the only level for this game mode or we just can't find a map for this game mode, then nextmap
	  //will always restart.
		trap->Cvar_Set( "nextmap", "map_restart 0");
	}
	else
	{ //otherwise we have a valid nextmap to cycle to, so use it.
		type = Info_ValueForKey( level.arenas.infos[desiredMap], "map" );
		trap->Cvar_Set( "nextmap", va("map %s", type));
	}

	return Info_ValueForKey( level.arenas.infos[desiredMap], "map" );
}

#define MAX_MAPS 256
#define MAPSBUFSIZE (MAX_MAPS * 64)

void G_LoadArenas( void ) {
	int			numFiles;
	char		filelist[MAPSBUFSIZE];
	char		filename[MAX_QPATH];
	char*		fileptr;
	int			i, n;
	int			len;

	level.arenas.num = 0;

	// get all arenas from .arena files
	numFiles = trap->FS_GetFileList("scripts", ".arena", filelist, ARRAY_LEN(filelist) );

	fileptr  = filelist;
	i = 0;

	if (numFiles > MAX_MAPS)
		numFiles = MAX_MAPS;

	for(; i < numFiles; i++) {
		len = strlen(fileptr);
		Com_sprintf(filename, sizeof(filename), "scripts/%s", fileptr);
		G_LoadArenasFromFile(filename);
		fileptr += len + 1;
	}
//	trap->Print( "%i arenas parsed\n", level.arenas.num );

	for( n = 0; n < level.arenas.num; n++ ) {
		Info_SetValueForKey( level.arenas.infos[n], "num", va( "%i", n ) );
	}

	G_RefreshNextMap(level.gametype, qfalse);
}

const char *G_GetArenaInfoByMap( const char *map ) {
	int			n;

	for( n = 0; n < level.arenas.num; n++ ) {
		if( Q_stricmp( Info_ValueForKey( level.arenas.infos[n], "map" ), map ) == 0 ) {
			return level.arenas.infos[n];
		}
	}

	return NULL;
}

void G_AddRandomBot( int team ) {
	int		i, n, num;
	float	skill;
	char	*value, netname[36], *teamstr;
	gclient_t	*cl;

	num = 0;
	for ( n = 0; n < level.bots.num ; n++ ) {
		value = Info_ValueForKey( level.bots.infos[n], "name" );

		for ( i=0 ; i< sv_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
				continue;
			}
			if ( team >= 0 && cl->sess.sessionTeam != team ) {
				continue;
			}
			if ( !Q_stricmp( value, cl->pers.netname ) ) {
				break;
			}
		}
		if (i >= sv_maxclients.integer) {
			num++;
		}
	}
	num = Q_flrand(0.0f, 1.0f) * num;
	for ( n = 0; n < level.bots.num ; n++ ) {
		value = Info_ValueForKey( level.bots.infos[n], "name" );

		for ( i=0 ; i< sv_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
				continue;
			}
			if ( team >= 0 && cl->sess.sessionTeam != team ) {
				continue;
			}
			if ( !Q_stricmp( value, cl->pers.netname ) ) {
				break;
			}
		}
		if (i >= sv_maxclients.integer) {
			num--;
			if (num <= 0) {
				skill = g_spSkill.integer;
				if (team == TEAM_RED) teamstr = "red";
				else if (team == TEAM_BLUE) teamstr = "blue";
				else teamstr = "";
				Q_strncpyz(netname, value, sizeof(netname));
				Q_CleanStr(netname);
				trap->SendConsoleCommand( EXEC_INSERT, va("addbot \"%s\" %.2f %s %i\n", netname, skill, teamstr, 0) );
				return;
			}
		}
	}
}

int G_RemoveRandomBot( int team ) {
	int i;
	gclient_t	*cl;

	for ( i=0 ; i< sv_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( !(g_entities[i].r.svFlags & SVF_BOT) )
			continue;

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR && cl->sess.spectatorState == SPECTATOR_FOLLOW )
			continue;

		if ( team >= 0 && cl->sess.sessionTeam != team )
			continue;

		trap->SendConsoleCommand( EXEC_INSERT, va("clientkick %d\n", i) );
		return qtrue;
	}
	return qfalse;
}

int G_CountHumanPlayers( int team ) {
	int i, num;
	gclient_t	*cl;

	num = 0;
	for ( i=0 ; i< sv_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[i].r.svFlags & SVF_BOT ) {
			continue;
		}
		if ( team >= 0 && cl->sess.sessionTeam != team ) {
			continue;
		}
		num++;
	}
	return num;
}

int G_CountBotPlayers( int team ) {
	int i, n, num;
	gclient_t	*cl;

	num = 0;
	for ( i=0 ; i< sv_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
			continue;
		}
		if ( team >= 0 && cl->sess.sessionTeam != team ) {
			continue;
		}
		num++;
	}
	for( n = 0; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( !botSpawnQueue[n].spawnTime ) {
			continue;
		}
		if ( botSpawnQueue[n].spawnTime > level.time ) {
			continue;
		}
		num++;
	}
	return num;
}

void G_CheckMinimumPlayers( void ) {
	int minplayers;
	int humanplayers, botplayers;
	static int checkminimumplayers_time;

	if (level.intermissiontime) return;
	//only check once each 10 seconds
	if (checkminimumplayers_time > level.time - 10000) {
		return;
	}
	checkminimumplayers_time = level.time;
	minplayers = bot_minplayers.integer;
	if (minplayers <= 0) return;

	if (minplayers > sv_maxclients.integer)
	{
		minplayers = sv_maxclients.integer;
	}

	humanplayers = G_CountHumanPlayers( -1 );
	botplayers = G_CountBotPlayers(	-1 );

	if ((humanplayers+botplayers) < minplayers)
	{
		G_AddRandomBot(-1);
	}
	else if ((humanplayers+botplayers) > minplayers && botplayers)
	{
		// try to remove spectators first
		if (!G_RemoveRandomBot(TEAM_SPECTATOR))
		{
			// just remove the bot that is playing
			G_RemoveRandomBot(-1);
		}
	}

	/*
	if (level.gametype >= GT_TEAM) {
		int humanplayers2, botplayers2;
		if (minplayers >= sv_maxclients.integer / 2) {
			minplayers = (sv_maxclients.integer / 2) -1;
		}

		humanplayers = G_CountHumanPlayers( TEAM_RED );
		botplayers = G_CountBotPlayers(	TEAM_RED );
		humanplayers2 = G_CountHumanPlayers( TEAM_BLUE );
		botplayers2 = G_CountBotPlayers( TEAM_BLUE );

		if ((humanplayers+botplayers+humanplayers2+botplayers) < minplayers)
		{
			if ((humanplayers+botplayers) < (humanplayers2+botplayers2))
			{
				G_AddRandomBot( TEAM_RED );
			}
			else
			{
				G_AddRandomBot( TEAM_BLUE );
			}
		}
		else if ((humanplayers+botplayers+humanplayers2+botplayers) > minplayers && botplayers)
		{
			if ((humanplayers+botplayers) < (humanplayers2+botplayers2))
			{
				G_RemoveRandomBot( TEAM_BLUE );
			}
			else
			{
				G_RemoveRandomBot( TEAM_RED );
			}
		}
	}
	else if (level.gametype == GT_DUEL || level.gametype == GT_POWERDUEL) {
		if (minplayers >= sv_maxclients.integer) {
			minplayers = sv_maxclients.integer-1;
		}
		humanplayers = G_CountHumanPlayers( -1 );
		botplayers = G_CountBotPlayers( -1 );

		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_FREE );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			// try to remove spectators first
			if (!G_RemoveRandomBot( TEAM_SPECTATOR )) {
				// just remove the bot that is playing
				G_RemoveRandomBot( -1 );
			}
		}
	}
	else if (level.gametype == GT_FFA) {
		if (minplayers >= sv_maxclients.integer) {
			minplayers = sv_maxclients.integer-1;
		}
		humanplayers = G_CountHumanPlayers( TEAM_FREE );
		botplayers = G_CountBotPlayers( TEAM_FREE );

		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_FREE );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			G_RemoveRandomBot( TEAM_FREE );
		}
	}
	else if (level.gametype == GT_HOLOCRON || level.gametype == GT_JEDIMASTER) {
		if (minplayers >= sv_maxclients.integer) {
			minplayers = sv_maxclients.integer-1;
		}
		humanplayers = G_CountHumanPlayers( TEAM_FREE );
		botplayers = G_CountBotPlayers( TEAM_FREE );

		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_FREE );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			G_RemoveRandomBot( TEAM_FREE );
		}
	}
	*/
}

void G_CheckBotSpawn( void ) {
	int		n;

	G_CheckMinimumPlayers();

	for( n = 0; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( !botSpawnQueue[n].spawnTime ) {
			continue;
		}
		if ( botSpawnQueue[n].spawnTime > level.time ) {
			continue;
		}
		ClientBegin( botSpawnQueue[n].clientNum, qfalse );
		botSpawnQueue[n].spawnTime = 0;
	}
}

static void AddBotToSpawnQueue( int clientNum, int delay ) {
	int		n;

	for( n = 0; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( !botSpawnQueue[n].spawnTime ) {
			botSpawnQueue[n].spawnTime = level.time + delay;
			botSpawnQueue[n].clientNum = clientNum;
			return;
		}
	}

	trap->Print( S_COLOR_YELLOW "Unable to delay spawn\n" );
	ClientBegin( clientNum, qfalse );
}

// Called on client disconnect to make sure the delayed spawn doesn't happen on a freed index
void G_RemoveQueuedBotBegin( int clientNum ) {
	int		n;

	for( n = 0; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( botSpawnQueue[n].clientNum == clientNum ) {
			botSpawnQueue[n].spawnTime = 0;
			return;
		}
	}
}

qboolean G_BotConnect( int clientNum, qboolean restart ) {
	bot_settings_t	settings;
	char			userinfo[MAX_INFO_STRING];

	trap->GetUserinfo( clientNum, userinfo, sizeof(userinfo) );

	Q_strncpyz( settings.personalityfile, Info_ValueForKey( userinfo, "personality" ), sizeof(settings.personalityfile) );
	settings.skill = atof( Info_ValueForKey( userinfo, "skill" ) );
	Q_strncpyz( settings.team, Info_ValueForKey( userinfo, "team" ), sizeof(settings.team) );

	if (!BotAISetupClient( clientNum, &settings, restart )) {
		trap->DropClient( clientNum, "BotAISetupClient failed" );
		return qfalse;
	}

	return qtrue;
}

static void G_AddBot( const char *name, float skill, const char *team, int delay, char *altname) {
	gentity_t *bot = NULL;
	int       clientNum;
	team_t    preTeam = TEAM_FREE;
	char      userinfo[MAX_INFO_STRING] = {0};
	char      *botinfo = NULL;
	char      *key = NULL;
	char      *s = NULL;
	char      *botname = NULL;
	char      *model = NULL;

	// have the server allocate a client slot
	clientNum = trap->BotAllocateClient();
	if ( clientNum == -1 ) {
//		trap->Print( S_COLOR_RED "Unable to add bot.  All player slots are in use.\n" );
//		trap->Print( S_COLOR_RED "Start server with more 'open' slots.\n" );
		trap->SendServerCommand( -1, va("print \"%s\n\"", G_GetStringEdString("MP_SVGAME", "UNABLE_TO_ADD_BOT")));
		return;
	}

	// get the botinfo from bots.txt
	botinfo = G_GetBotInfoByName( name );
	if ( !botinfo ) {
		trap->Print( S_COLOR_RED "Error: Bot '%s' not defined\n", name );
		trap->BotFreeClient( clientNum );
		return;
	}

	// create the bot's userinfo
	userinfo[0] = '\0';

	botname = Info_ValueForKey( botinfo, "funname" );
	if( !botname[0] )
		botname = Info_ValueForKey( botinfo, "name" );
	// check for an alternative name
	if ( altname && altname[0] )
		botname = altname;

	Info_SetValueForKey( userinfo, "name", botname );
	Info_SetValueForKey( userinfo, "rate", "25000" );
	Info_SetValueForKey( userinfo, "snaps", "20" );
	Info_SetValueForKey( userinfo, "ip", "localhost" );
	Info_SetValueForKey( userinfo, "skill", va("%.2f", skill) );

	key = "model";
	model = Info_ValueForKey( botinfo, key );
	if ( !*model )	model = DEFAULT_MODEL"/default";
	Info_SetValueForKey( userinfo, key, model );

	key = "sex";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = Info_ValueForKey( botinfo, "gender" );
	if ( !*s )	s = "male";
	Info_SetValueForKey( userinfo, key, s );

	key = "color1";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "4";
	Info_SetValueForKey( userinfo, key, s );

	key = "color2";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "4";
	Info_SetValueForKey( userinfo, key, s );

	key = "saber1";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = DEFAULT_SABER;
	Info_SetValueForKey( userinfo, key, s );

	key = "saber2";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "none";
	Info_SetValueForKey( userinfo, key, s );

	key = "forcepowers";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = DEFAULT_FORCEPOWERS;
	Info_SetValueForKey( userinfo, key, s );

	key = "cg_predictItems";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "1";
	Info_SetValueForKey( userinfo, key, s );

	key = "char_color_red";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "255";
	Info_SetValueForKey( userinfo, key, s );

	key = "char_color_green";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "255";
	Info_SetValueForKey( userinfo, key, s );

	key = "char_color_blue";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "255";
	Info_SetValueForKey( userinfo, key, s );

	key = "teamtask";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "0";
	Info_SetValueForKey( userinfo, key, s );

	key = "personality";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s )	s = "botfiles/default.jkb";
	Info_SetValueForKey( userinfo, key, s );

	// initialize the bot settings
	if ( !team || !*team ) {
		if ( level.gametype >= GT_TEAM ) {
			if ( PickTeam( clientNum ) == TEAM_RED )
				team = "red";
			else
				team = "blue";
		}
		else
			team = "red";
	}
	Info_SetValueForKey( userinfo, "team", team );

	bot = &g_entities[ clientNum ];
//	bot->r.svFlags |= SVF_BOT;
//	bot->inuse = qtrue;

	// register the userinfo
	trap->SetUserinfo( clientNum, userinfo );

	if ( level.gametype >= GT_TEAM )
	{
		if ( team && !Q_stricmp( team, "red" ) )
			bot->client->sess.sessionTeam = TEAM_RED;
		else if ( team && !Q_stricmp( team, "blue" ) )
			bot->client->sess.sessionTeam = TEAM_BLUE;
		else
			bot->client->sess.sessionTeam = PickTeam( -1 );
	}

	preTeam = bot->client->sess.sessionTeam;

	// have it connect to the game as a normal client
	if ( ClientConnect( clientNum, qtrue, qtrue ) )
		return;

	if ( bot->client->sess.sessionTeam != preTeam )
	{
		trap->GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

		if ( bot->client->sess.sessionTeam == TEAM_SPECTATOR )
			bot->client->sess.sessionTeam = preTeam;

		if ( bot->client->sess.sessionTeam == TEAM_RED )
			team = "Red";
		else
			team = "Blue";

		Info_SetValueForKey( userinfo, "team", team );

		trap->SetUserinfo( clientNum, userinfo );

		bot->client->ps.persistant[ PERS_TEAM ] = bot->client->sess.sessionTeam;

		G_ReadSessionData( bot->client );
		if ( !ClientUserinfoChanged( clientNum ) )
			return;
	}

	if (level.gametype == GT_DUEL ||
		level.gametype == GT_POWERDUEL)
	{
		int loners = 0;
		int doubles = 0;

		bot->client->sess.duelTeam = 0;
		G_PowerDuelCount(&loners, &doubles, qtrue);

		if (!doubles || loners > (doubles/2))
		{
            bot->client->sess.duelTeam = DUELTEAM_DOUBLE;
		}
		else
		{
            bot->client->sess.duelTeam = DUELTEAM_LONE;
		}

		bot->client->sess.sessionTeam = TEAM_SPECTATOR;
		SetTeam(bot, "s");
	}
	else
	{
		if( delay == 0 ) {
			ClientBegin( clientNum, qfalse );
			return;
		}

		AddBotToSpawnQueue( clientNum, delay );
	}
}

void Svcmd_AddBot_f( void ) {
	float			skill;
	int				delay;
	char			name[MAX_TOKEN_CHARS];
	char			altname[MAX_TOKEN_CHARS];
	char			string[MAX_TOKEN_CHARS];
	char			team[MAX_TOKEN_CHARS];

	// are bots enabled?
	if ( !bot_enable.integer ) {
		trap->Print( "Bots are disabled on this server" );
		return;
	}

	// name
	trap->Argv( 1, name, sizeof( name ) );
	if ( !name[0] ) {
		trap->Print( "Usage: Addbot <botname> [skill 1-5] [team] [msec delay] [altname]\n" );
		return;
	}

	// skill
	trap->Argv( 2, string, sizeof( string ) );
	if ( !string[0] ) {
		skill = 4;
	}
	else {
		skill = atof( string );
	}

	// team
	trap->Argv( 3, team, sizeof( team ) );

	// delay
	trap->Argv( 4, string, sizeof( string ) );
	if ( !string[0] ) {
		delay = 0;
	}
	else {
		delay = atoi( string );
	}

	// alternative name
	trap->Argv( 5, altname, sizeof( altname ) );

	G_AddBot( name, skill, team, delay, altname );

	// if this was issued during gameplay and we are playing locally,
	// go ahead and load the bot's media immediately
	if ( level.time - level.startTime > 1000 && cl_running.integer ) {
		trap->SendServerCommand( -1, "loaddefered\n" );	// FIXME: spelled wrong, but not changing for demo
	}
}

void Svcmd_BotList_f( void ) {
	int i;
	char name[MAX_NETNAME];
	char funname[MAX_NETNAME];
	char model[MAX_QPATH];
	char personality[MAX_QPATH];

	trap->Print("name             model            personality              funname\n");
	for (i = 0; i < level.bots.num; i++) {
		Q_strncpyz(name, Info_ValueForKey( level.bots.infos[i], "name" ), sizeof( name ));
		if ( !*name ) {
			Q_strncpyz(name, "Padawan", sizeof( name ));
		}
		Q_strncpyz(funname, Info_ValueForKey( level.bots.infos[i], "funname"), sizeof( funname ));
		if ( !*funname ) {
			funname[0] = '\0';
		}
		Q_strncpyz(model, Info_ValueForKey( level.bots.infos[i], "model" ), sizeof( model ));
		if ( !*model ) {
			Q_strncpyz(model, DEFAULT_MODEL"/default", sizeof( model ));
		}
		Q_strncpyz(personality, Info_ValueForKey( level.bots.infos[i], "personality"), sizeof( personality ));
		if (!*personality ) {
			Q_strncpyz(personality, "botfiles/kyle.jkb", sizeof( personality ));
		}
		trap->Print("%-16s %-16s %-20s %-20s\n", name, model, COM_SkipPath(personality), funname);
	}
}

static void G_LoadBotsFromFile( const char *filename ) {
	int				len;
	fileHandle_t	f;
	char			buf[MAX_BOTS_TEXT];

	len = trap->FS_Open( filename, &f, FS_READ );
	if ( !f ) {
		trap->Print( S_COLOR_RED "file not found: %s\n", filename );
		return;
	}
	if ( len >= MAX_BOTS_TEXT ) {
		trap->Print( S_COLOR_RED "file too large: %s is %i, max allowed is %i\n", filename, len, MAX_BOTS_TEXT );
		trap->FS_Close( f );
		return;
	}

	trap->FS_Read( buf, len, f );
	buf[len] = 0;
	trap->FS_Close( f );

	level.bots.num += G_ParseInfos( buf, MAX_BOTS - level.bots.num, &level.bots.infos[level.bots.num] );
}

static void G_LoadBots( void ) {
	int			numdirs;
	char		filename[128];
	char		dirlist[1024];
	char*		dirptr;
	int			i;
	int			dirlen;

	if ( !bot_enable.integer ) {
		return;
	}

	level.bots.num = 0;

	G_LoadBotsFromFile( g_botsFile.string[0] ? g_botsFile.string : DEFAULT_BOTFILE );

	// get all bots from .bot files
	numdirs = trap->FS_GetFileList("scripts", ".bot", dirlist, 1024 );
	dirptr  = dirlist;
	for (i = 0; i < numdirs; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		G_LoadBotsFromFile(filename);
	}
//	trap->Print( "%i bots parsed\n", level.bots.num );
}

char *G_GetBotInfoByNumber( int num ) {
	if( num < 0 || num >= level.bots.num ) {
		trap->Print( S_COLOR_RED "Invalid bot number: %i\n", num );
		return NULL;
	}
	return level.bots.infos[num];
}

char *G_GetBotInfoByName( const char *name ) {
	int		n;
	char	*value;

	for ( n = 0; n < level.bots.num ; n++ ) {
		value = Info_ValueForKey( level.bots.infos[n], "name" );
		if ( !Q_stricmp( value, name ) ) {
			return level.bots.infos[n];
		}
	}

	return NULL;
}

void G_InitBots( void ) {
	G_LoadBots();
	G_LoadArenas();
	LoadPath_ThisLevel();
}
