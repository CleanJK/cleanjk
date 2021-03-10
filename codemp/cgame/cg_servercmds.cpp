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

// cg_servercmds.c -- reliably sequenced text commands sent by the server
// these are processed at snapshot transition time, so there will definately
// be a valid snapshot this frame

#include "cgame/cg_local.hpp"
#include "ghoul2/G2.hpp"
#include "cgame/cg_media.hpp"

#define SCORE_OFFSET (14)
static void CG_ParseScores( void ) {
	int i, powerups, readScores;

	cg.numScores = atoi( CG_Argv( 1 ) );

	readScores = cg.numScores;

	if (readScores > MAX_CLIENT_SCORE_SEND)
		readScores = MAX_CLIENT_SCORE_SEND;

	if ( cg.numScores > MAX_CLIENTS )
		cg.numScores = MAX_CLIENTS;

	cg.numScores = readScores;

	cg.teamScores[0] = atoi( CG_Argv( 2 ) );
	cg.teamScores[1] = atoi( CG_Argv( 3 ) );

	memset( cg.scores, 0, sizeof( cg.scores ) );
	for ( i=0; i<readScores; i++ ) {
		cg.scores[i].client				= atoi( CG_Argv( i*SCORE_OFFSET +  4 ) );
		cg.scores[i].score				= atoi( CG_Argv( i*SCORE_OFFSET +  5 ) );
		cg.scores[i].ping				= atoi( CG_Argv( i*SCORE_OFFSET +  6 ) );
		cg.scores[i].time				= atoi( CG_Argv( i*SCORE_OFFSET +  7 ) );
		cg.scores[i].scoreFlags			= atoi( CG_Argv( i*SCORE_OFFSET +  8 ) );
		powerups						= atoi( CG_Argv( i*SCORE_OFFSET +  9 ) );
		cg.scores[i].accuracy			= atoi( CG_Argv( i*SCORE_OFFSET + 10 ) );
		cg.scores[i].impressiveCount	= atoi( CG_Argv( i*SCORE_OFFSET + 11 ) );
		cg.scores[i].excellentCount		= atoi( CG_Argv( i*SCORE_OFFSET + 12 ) );
		cg.scores[i].gauntletCount		= atoi( CG_Argv( i*SCORE_OFFSET + 13 ) );
		cg.scores[i].defendCount		= atoi( CG_Argv( i*SCORE_OFFSET + 14 ) );
		cg.scores[i].assistCount		= atoi( CG_Argv( i*SCORE_OFFSET + 15 ) );
		cg.scores[i].perfect			= atoi( CG_Argv( i*SCORE_OFFSET + 16 ) );
		cg.scores[i].captures			= atoi( CG_Argv( i*SCORE_OFFSET + 17 ) );

		if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS )
			cg.scores[i].client = 0;

		cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
		cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;

		cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;
	}
	CG_SetScoreSelection( nullptr );
}

#define TEAMINFO_OFFSET (6)
static void CG_ParseTeamInfo( void ) {
	int i, client;

	numSortedTeamPlayers = atoi( CG_Argv( 1 ) );
	if ( numSortedTeamPlayers < 0 || numSortedTeamPlayers > TEAM_MAXOVERLAY ) {
		trap->Error( ERR_DROP, "CG_ParseTeamInfo: numSortedTeamPlayers out of range (%d)", numSortedTeamPlayers );
		return;
	}

	for ( i=0; i<numSortedTeamPlayers; i++ ) {
		client = atoi( CG_Argv( i*TEAMINFO_OFFSET + 2 ) );
		if ( client < 0 || client >= MAX_CLIENTS ) {
			trap->Error( ERR_DROP, "CG_ParseTeamInfo: bad client number: %d", client );
			return;
		}

		sortedTeamPlayers[i] = client;

		cgs.clientinfo[client].location		= atoi( CG_Argv( i*TEAMINFO_OFFSET + 3 ) );
		cgs.clientinfo[client].health		= atoi( CG_Argv( i*TEAMINFO_OFFSET + 4 ) );
		cgs.clientinfo[client].armor		= atoi( CG_Argv( i*TEAMINFO_OFFSET + 5 ) );
		cgs.clientinfo[client].curWeapon	= atoi( CG_Argv( i*TEAMINFO_OFFSET + 6 ) );
		cgs.clientinfo[client].powerups		= atoi( CG_Argv( i*TEAMINFO_OFFSET + 7 ) );
	}
}

// This is called explicitly when the gamestate is first received, and whenever the server updates any serverinfo
//	flagged cvars
void CG_ParseServerinfo( void ) {
	const char *info = nullptr;
	char *mapname;
	int i, value;

	info = CG_ConfigString( CS_SERVERINFO );

	cgs.debugMelee = atoi( Info_ValueForKey( info, "g_debugMelee" ) ); //trap->Cvar_GetHiddenVarValue("g_iknowkungfu");
	cgs.stepSlideFix = atoi( Info_ValueForKey( info, "g_stepSlideFix" ) );

	cgs.noSpecMove = atoi( Info_ValueForKey( info, "g_noSpecMove" ) );

	cgs.showDuelHealths = atoi( Info_ValueForKey( info, "g_showDuelHealths" ) );

	cgs.gametype = (gametype_e)atoi( Info_ValueForKey( info, "g_gametype" ) );
	trap->Cvar_Set("g_gametype", va("%i", cgs.gametype));
	cgs.needpass = atoi( Info_ValueForKey( info, "g_needpass" ) );
	cgs.jediVmerc = atoi( Info_ValueForKey( info, "g_jediVmerc" ) );

	// this changes on map_restart, attempt to precache weapons
	value = atoi( Info_ValueForKey( info, "g_weaponDisable" ) );
	if ( cgs.wDisable != value ) {
		gitem_t *item = nullptr;
		itemInfo_t *itemInfo = nullptr;

		cgs.wDisable = value;

		for ( i=1, item=bg_itemlist, itemInfo = cg_items;
			i<bg_numItems;
			i++, item++, itemInfo++ )
		{// register all weapons that aren't disabled
			if ( item->giType == IT_WEAPON )
				CG_RegisterWeapon( item->giTag );
		}
	}

	cgs.fDisable = atoi( Info_ValueForKey( info, "g_forcePowerDisable" ) );
	cgs.dmflags = atoi( Info_ValueForKey( info, "dmflags" ) );
	cgs.duel_fraglimit = atoi( Info_ValueForKey( info, "duel_fraglimit" ) );
	cgs.capturelimit = atoi( Info_ValueForKey( info, "capturelimit" ) );

	// reset fraglimit warnings
	i = atoi( Info_ValueForKey( info, "fraglimit" ) );
	if ( cgs.fraglimit < i )
		cg.fraglimitWarnings &= ~(1|2|4);
	cgs.fraglimit = i;

	// reset timelimit warnings
	i = atoi( Info_ValueForKey( info, "timelimit" ) );
	if ( cgs.timelimit != i )
		cg.timelimitWarnings &= ~(1|2);
	cgs.timelimit = i;

	cgs.maxclients = Com_Clampi( 0, MAX_CLIENTS, atoi( Info_ValueForKey( info, "sv_maxclients" ) ) );
	mapname = Info_ValueForKey( info, "mapname" );

	//rww - You must do this one here, Info_ValueForKey always uses the same memory pointer.
	trap->Cvar_Set ( "ui_about_mapname", mapname );

	Com_sprintf( cgs.mapname, sizeof( cgs.mapname ), "maps/%s.bsp", mapname );
	Com_sprintf( cgs.rawmapname, sizeof( cgs.rawmapname ), "maps/%s", mapname );
//	Q_strncpyz( cgs.redTeam, Info_ValueForKey( info, "g_redTeam" ), sizeof(cgs.redTeam) );
//	trap->Cvar_Set("g_redTeam", cgs.redTeam);
//	Q_strncpyz( cgs.blueTeam, Info_ValueForKey( info, "g_blueTeam" ), sizeof(cgs.blueTeam) );
//	trap->Cvar_Set("g_blueTeam", cgs.blueTeam);

	trap->Cvar_Set ( "ui_about_gametype", va("%i", cgs.gametype ) );
	trap->Cvar_Set ( "ui_about_fraglimit", va("%i", cgs.fraglimit ) );
	trap->Cvar_Set ( "ui_about_duellimit", va("%i", cgs.duel_fraglimit ) );
	trap->Cvar_Set ( "ui_about_capturelimit", va("%i", cgs.capturelimit ) );
	trap->Cvar_Set ( "ui_about_timelimit", va("%i", cgs.timelimit ) );
	trap->Cvar_Set ( "ui_about_maxclients", va("%i", cgs.maxclients ) );
	trap->Cvar_Set ( "ui_about_dmflags", va("%i", cgs.dmflags ) );
	trap->Cvar_Set ( "ui_about_hostname", Info_ValueForKey( info, "sv_hostname" ) );
	trap->Cvar_Set ( "ui_about_needpass", Info_ValueForKey( info, "g_needpass" ) );
	trap->Cvar_Set ( "ui_about_botminplayers", Info_ValueForKey ( info, "bot_minplayers" ) );

	Q_strncpyz( cgs.vote.string, CG_ConfigString( CS_VOTE_STRING ), sizeof( cgs.vote.string ) );

	// synchronise our expected snaps/sec with the server's framerate
	i = atoi( Info_ValueForKey( info, "sv_fps" ) );
	if ( i )
		trap->Cvar_Set( "snaps", va( "%i", i ) );

	cgs.saberSpecialMoves = atoi( Info_ValueForKey( info, "cjk_saberSpecialMoves" ) );
	cgs.saberTweaks = atoi( Info_ValueForKey( info, "cjk_saberTweaks" ) );
}

static void CG_ParseWarmup( void ) {
	const char	*info;
	int			warmup;

	info = CG_ConfigString( CS_WARMUP );

	warmup = atoi( info );
	cg.warmupCount = -1;

	cg.warmup = warmup;
}

// this is a reverse map of flag statuses as seen in g_team.c
//static char ctfFlagStatusRemap[] = { '0', '1', '*', '*', '2' };
static constexpr char ctfFlagStatusRemap[] = {
	FLAG_ATBASE,
	FLAG_TAKEN,			// CTF
	// server doesn't use FLAG_TAKEN_RED or FLAG_TAKEN_BLUE
	// which was originally for 1-flag CTF.
	FLAG_DROPPED
};

// Called on load to set the initial values from configure strings
void CG_SetConfigValues( void )
{
	const char *s;
	const char *str;

	cgs.scores1 = atoi( CG_ConfigString( CS_SCORES1 ) );
	cgs.scores2 = atoi( CG_ConfigString( CS_SCORES2 ) );
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );
	if( cgs.gametype == GT_CTF || cgs.gametype == GT_CTY ) {
		int redflagId = 0, blueflagId = 0;

		s = CG_ConfigString( CS_FLAGSTATUS );

		redflagId = s[0] - '0';
		blueflagId = s[1] - '0';

		// fix: proper flag statuses mapping for dropped flag
		if ( redflagId >= 0 && redflagId < ARRAY_LEN( ctfFlagStatusRemap ) )
			cgs.redflag = ctfFlagStatusRemap[redflagId];

		if ( blueflagId >= 0 && blueflagId < ARRAY_LEN( ctfFlagStatusRemap ) )
			cgs.blueflag = ctfFlagStatusRemap[blueflagId];
	}
	cg.warmup = atoi( CG_ConfigString( CS_WARMUP ) );

	// Track who the jedi master is
	cgs.jediMaster = atoi ( CG_ConfigString ( CS_CLIENT_JEDIMASTER ) );
	cgs.duelWinner = atoi ( CG_ConfigString ( CS_CLIENT_DUELWINNER ) );

	str = CG_ConfigString(CS_CLIENT_DUELISTS);

	if (str && str[0])
	{
		char buf[64];
		int c = 0;
		int i = 0;

		while (str[i] && str[i] != '|')
		{
			buf[c] = str[i];
			c++;
			i++;
		}
		buf[c] = 0;

		cgs.duelist1 = atoi ( buf );
		c = 0;

		i++;
		while (str[i])
		{
			buf[c] = str[i];
			c++;
			i++;
		}
		buf[c] = 0;

		cgs.duelist2 = atoi ( buf );
	}
}

void CG_ShaderStateChanged(void) {
	char originalShader[MAX_QPATH];
	char newShader[MAX_QPATH];
	char timeOffset[16];
	const char *o, *n,*t;

	o = CG_ConfigString( CS_SHADERSTATE );
	while (o && *o) {
		n = strstr(o, "=");
		if (n && *n) {
			strncpy(originalShader, o, n-o);
			originalShader[n-o] = 0;
			n++;
			t = strstr(n, ":");
			if (t && *t) {
				strncpy(newShader, n, t-n);
				newShader[t-n] = 0;
			} else {
				break;
			}
			t++;
			o = strstr(t, "@");
			if (o) {
				strncpy(timeOffset, t, o-t);
				timeOffset[o-t] = 0;
				o++;
				trap->R_RemapShader( originalShader, newShader, timeOffset );
			}
		} else {
			break;
		}
	}
}

void SetCustomSoundForType(clientInfo_t *ci, int setType, int index, sfxHandle_t sfx)
{
	switch (setType)
	{
	case 1:
		ci->sounds[index] = sfx;
		break;
	case 2:
		ci->combatSounds[index] = sfx;
		break;
	case 3:
		ci->extraSounds[index] = sfx;
		break;
	case 4:
		ci->jediSounds[index] = sfx;
		break;
	case 5:
		ci->siegeSounds[index] = sfx;
		break;
	case 6:
		ci->duelSounds[index] = sfx;
		break;
	default:
		assert(0);
		break;
	}
}

// nmckenzie: DUEL_HEALTH - fixme - we could really clean this up immensely with some helper functions.
void SetDuelistHealthsFromConfigString ( const char *str ) {
	char buf[64];
	int c = 0;
	int i = 0;

	while (str[i] && str[i] != '|')
	{
		buf[c] = str[i];
		c++;
		i++;
	}
	buf[c] = 0;

	cgs.duelist1health = atoi ( buf );

	c = 0;
	i++;
	while (str[i] && str[i] != '|')
	{
		buf[c] = str[i];
		c++;
		i++;
	}
	buf[c] = 0;

	cgs.duelist2health = atoi ( buf );

	c = 0;
	i++;
	if ( str[i] == '!' )
	{	// we only have 2 duelists, apparently.
		cgs.duelist3health = -1;
		return;
	}

	while (str[i] && str[i] != '|')
	{
		buf[c] = str[i];
		c++;
		i++;
	}
	buf[c] = 0;

	cgs.duelist3health = atoi ( buf );
}

static void CG_ConfigStringModified( void ) {
	const char	*str;
	int		num;

	num = atoi( CG_Argv( 1 ) );

	// get the gamestate from the client system, which will have the
	// new configstring already integrated
	trap->GetGameState( &cgs.gameState );

	// look up the individual string that was modified
	str = CG_ConfigString( num );

	// do something with it if necessary
	if ( num == CS_MUSIC ) {
		CG_StartMusic( true );
	} else if ( num == CS_SERVERINFO ) {
		CG_ParseServerinfo();
	} else if ( num == CS_WARMUP ) {
		CG_ParseWarmup();
	} else if ( num == CS_SCORES1 ) {
		cgs.scores1 = atoi( str );
	} else if ( num == CS_SCORES2 ) {
		cgs.scores2 = atoi( str );
	} else if ( num == CS_CLIENT_JEDIMASTER ) {
		cgs.jediMaster = atoi ( str );
	}
	else if ( num == CS_CLIENT_DUELWINNER )
	{
		cgs.duelWinner = atoi ( str );
	}
	else if ( num == CS_CLIENT_DUELISTS )
	{
		char buf[64];
		int c = 0;
		int i = 0;

		while (str[i] && str[i] != '|')
		{
			buf[c] = str[i];
			c++;
			i++;
		}
		buf[c] = 0;

		cgs.duelist1 = atoi ( buf );
		c = 0;

		i++;
		while (str[i] && str[i] != '|')
		{
			buf[c] = str[i];
			c++;
			i++;
		}
		buf[c] = 0;

		cgs.duelist2 = atoi ( buf );

		if (str[i])
		{
			c = 0;
			i++;

			while (str[i])
			{
				buf[c] = str[i];
				c++;
				i++;
			}
			buf[c] = 0;

			cgs.duelist3 = atoi(buf);
		}
	}
	else if ( num == CS_CLIENT_DUELHEALTHS ) {	// nmckenzie: DUEL_HEALTH
		SetDuelistHealthsFromConfigString(str);
	}
	else if ( num == CS_LEVEL_START_TIME ) {
		cgs.levelStartTime = atoi( str );
	} else if ( num == CS_VOTE_TIME ) {
		cgs.vote.time = atoi( str );
		cgs.vote.modified = true;
	} else if ( num == CS_VOTE_YES ) {
		cgs.vote.yes = atoi( str );
		cgs.vote.modified = true;
	} else if ( num == CS_VOTE_NO ) {
		cgs.vote.no = atoi( str );
		cgs.vote.modified = true;
	} else if ( num == CS_VOTE_STRING ) {
		Q_strncpyz( cgs.vote.string, str, sizeof( cgs.vote.string ) );
	} else if ( num >= CS_TEAMVOTE_TIME && num <= CS_TEAMVOTE_TIME + 1) {
		cgs.teamVote.time[num-CS_TEAMVOTE_TIME] = atoi( str );
		cgs.teamVote.modified[num-CS_TEAMVOTE_TIME] = true;
	} else if ( num >= CS_TEAMVOTE_YES && num <= CS_TEAMVOTE_YES + 1) {
		cgs.teamVote.yes[num-CS_TEAMVOTE_YES] = atoi( str );
		cgs.teamVote.modified[num-CS_TEAMVOTE_YES] = true;
	} else if ( num >= CS_TEAMVOTE_NO && num <= CS_TEAMVOTE_NO + 1) {
		cgs.teamVote.no[num-CS_TEAMVOTE_NO] = atoi( str );
		cgs.teamVote.modified[num-CS_TEAMVOTE_NO] = true;
	} else if ( num >= CS_TEAMVOTE_STRING && num <= CS_TEAMVOTE_STRING + 1) {
		Q_strncpyz( cgs.teamVote.string[num-CS_TEAMVOTE_STRING], str, sizeof( cgs.teamVote.string ) );
	} else if ( num == CS_INTERMISSION ) {
		cg.intermissionStarted = atoi( str );
	} else if ( num >= CS_MODELS && num < CS_MODELS+MAX_MODELS ) {
		char modelName[MAX_QPATH];
		strcpy(modelName, str);
		if (strstr(modelName, ".glm") || modelName[0] == '$')
		{ //Check to see if it has a custom skin attached.
			CG_HandleAppendedSkin(modelName);
			CG_CacheG2AnimInfo(modelName);
		}

		if (modelName[0] != '$' && modelName[0] != '@')
		{ //don't register vehicle names and saber names as models.
			cgs.gameModels[ num-CS_MODELS ] = trap->R_RegisterModel( modelName );
		}
		else
		{
            cgs.gameModels[ num-CS_MODELS ] = 0;
		}
		/*
	} else if ( num >= CS_CHARSKINS && num < CS_CHARSKINS+MAX_CHARSKINS ) {
		cgs.skins[ num-CS_CHARSKINS ] = trap->R_RegisterSkin( str );
		*/
		//rww - removed and replaced with CS_G2BONES
	} else if ( num >= CS_SOUNDS && num < CS_SOUNDS+MAX_SOUNDS ) {
		if ( str[0] != '*' ) {	// player specific sounds don't register here
			cgs.gameSounds[ num-CS_SOUNDS] = trap->S_RegisterSound( str );
		}
	} else if ( num >= CS_EFFECTS && num < CS_EFFECTS+MAX_FX ) {
		if (str[0] == '*')
		{ //it's a special global weather effect
			trap->R_WorldEffectCommand( str+1 );
			cgs.gameEffects[ num-CS_EFFECTS] = 0;
		}
		else
		{
			cgs.gameEffects[ num-CS_EFFECTS] = trap->FX_RegisterEffect( str );
		}
	}
	else if ( num >= CS_PLAYERS && num < CS_PLAYERS+MAX_CLIENTS )
	{
		CG_NewClientInfo( num - CS_PLAYERS, true);
		CG_BuildSpectatorString();
	} else if ( num == CS_FLAGSTATUS ) {
		if( cgs.gametype == GT_CTF || cgs.gametype == GT_CTY ) {
			// format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
			int redflagId = str[0] - '0', blueflagId = str[1] - '0';

			if ( redflagId >= 0 && redflagId < ARRAY_LEN( ctfFlagStatusRemap ) )
				cgs.redflag = ctfFlagStatusRemap[redflagId];

			if ( blueflagId >= 0 && blueflagId < ARRAY_LEN( ctfFlagStatusRemap ) )
				cgs.blueflag = ctfFlagStatusRemap[blueflagId];
		}
	}
	else if ( num == CS_SHADERSTATE ) {
		CG_ShaderStateChanged();
	}
	else if ( num >= CS_LIGHT_STYLES && num < CS_LIGHT_STYLES + (MAX_LIGHT_STYLES * 3))
	{
		CG_SetLightstyle(num - CS_LIGHT_STYLES);
	}

}

//frees all ghoul2 stuff and npc stuff from a centity -rww
void CG_KillCEntityG2(int entNum)
{
	int j;
	clientInfo_t *ci = (entNum < MAX_CLIENTS) ? &cgs.clientinfo[entNum] : nullptr;
	centity_t *cent = &cg_entities[entNum];

	if (ci)
	{
		if (ci->ghoul2Model == cent->ghoul2)
		{
			ci->ghoul2Model = nullptr;
		}
		else if (ci->ghoul2Model && trap->G2_HaveWeGhoul2Models(ci->ghoul2Model))
		{
			trap->G2API_CleanGhoul2Models(&ci->ghoul2Model);
			ci->ghoul2Model = nullptr;
		}

		//Clean up any weapon instances for custom saber stuff
		j = 0;
		while (j < MAX_SABERS)
		{
			if (ci->ghoul2Weapons[j] && trap->G2_HaveWeGhoul2Models(ci->ghoul2Weapons[j]))
			{
				trap->G2API_CleanGhoul2Models(&ci->ghoul2Weapons[j]);
				ci->ghoul2Weapons[j] = nullptr;
			}

			j++;
		}
	}

	if (cent->ghoul2 && trap->G2_HaveWeGhoul2Models(cent->ghoul2))
	{
		trap->G2API_CleanGhoul2Models(&cent->ghoul2);
		cent->ghoul2 = nullptr;
	}

	if (cent->grip_arm && trap->G2_HaveWeGhoul2Models(cent->grip_arm))
	{
		trap->G2API_CleanGhoul2Models(&cent->grip_arm);
		cent->grip_arm = nullptr;
	}

	if (cent->frame_hold && trap->G2_HaveWeGhoul2Models(cent->frame_hold))
	{
		trap->G2API_CleanGhoul2Models(&cent->frame_hold);
		cent->frame_hold = nullptr;
	}

	cent->isRagging = false; //just in case.
	cent->ikStatus = false;

	cent->localAnimIndex = 0;
}

void CG_KillCEntityInstances(void)
{
	int i = 0;
	centity_t *cent;

	while (i < MAX_GENTITIES)
	{
		cent = &cg_entities[i];

		if (i >= MAX_CLIENTS && cent->currentState.number == i)
		{ //do not clear G2 instances on client ents, they are constant
			CG_KillCEntityG2(i);
		}

		cent->bolt1 = 0;
		cent->bolt2 = 0;
		cent->bolt3 = 0;
		cent->bolt4 = 0;

		cent->bodyHeight = 0;//SABER_LENGTH_MAX;
		//cent->saberExtendTime = 0;

		cent->boltInfo = 0;

		cent->frame_minus1_refreshed = 0;
		cent->frame_minus2_refreshed = 0;
		cent->dustTrailTime = 0;
		cent->ghoul2weapon = nullptr;
		//cent->torsoBolt = 0;
		cent->trailTime = 0;
		cent->frame_hold_time = 0;
		cent->frame_hold_refreshed = 0;
		cent->trickAlpha = 0;
		cent->trickAlphaTime = 0;
		VectorClear(cent->turAngles);
		cent->weapon = 0;
		cent->teamPowerEffectTime = 0;
		cent->teamPowerType = 0;
		cent->numLoopingSounds = 0;

		cent->localAnimIndex = 0;

		i++;
	}
}

// The server has issued a map_restart, so the next snapshot is completely new and should not be interpolated to.
// A tournament restart will clear everything, but doesn't require a reload of all the media
static void CG_MapRestart( void ) {
	if ( cg_showMiss.integer ) {
		trap->Print( "CG_MapRestart\n" );
	}

	trap->R_ClearDecals ( );
	//FIXME: trap->FX_Reset?

	CG_InitLocalEntities();
	CG_InitMarkPolys();
	CG_KillCEntityInstances();

	// make sure the "3 frags left" warnings play again
	cg.fraglimitWarnings = 0;

	cg.timelimitWarnings = 0;

	cg.intermissionStarted = false;

	cgs.vote.time = 0;

	cg.mapRestart = true;

	CG_StartMusic(true);

	trap->S_ClearLoopingSounds();

	// we really should clear more parts of cg here and stop sounds

	// play the "fight" sound if this is a restart without warmup
	if ( cg.warmup == 0 && cgs.gametype != GT_POWERDUEL/* && cgs.gametype == GT_DUEL */) {
		trap->S_StartLocalSound( media.sounds.null, CHAN_ANNOUNCER );
		CG_CenterPrint( CG_GetStringEdString("MP_SVGAME", "BEGIN_DUEL"), 120, GIANTCHAR_WIDTH*2 );
	}
//	trap->Cvar_Set("cg_thirdPerson", "0");
}

static void CG_RemoveChatEscapeChar( char *text ) {
	int i, l;

	l = 0;
	for ( i = 0; text[i]; i++ ) {
		if (text[i] == '\x19')
			continue;
		text[l++] = text[i];
	}
	text[l] = '\0';
}

#define MAX_STRINGED_SV_STRING 1024	// this is an quake-engine limit, not a StringEd limit

void CG_CheckSVStringEdRef(char *buf, const char *str)
{ //I don't really like doing this. But it utilizes the system that was already in place.
	int i = 0;
	int b = 0;
	int strLen = 0;
	bool gotStrip = false;

	if (!str || !str[0])
	{
		if (str)
		{
			strcpy(buf, str);
		}
		return;
	}

	strcpy(buf, str);

	strLen = strlen(str);

	if (strLen >= MAX_STRINGED_SV_STRING)
	{
		return;
	}

	while (i < strLen && str[i])
	{
		gotStrip = false;

		if (str[i] == '@' && (i+1) < strLen)
		{
			if (str[i+1] == '@' && (i+2) < strLen)
			{
				if (str[i+2] == '@' && (i+3) < strLen)
				{ //@@@ should mean to insert a StringEd reference here, so insert it into buf at the current place
					char stringRef[MAX_STRINGED_SV_STRING];
					int r = 0;

					while (i < strLen && str[i] == '@')
					{
						i++;
					}

					while (i < strLen && str[i] && str[i] != ' ' && str[i] != ':' && str[i] != '.' && str[i] != '\n')
					{
						stringRef[r] = str[i];
						r++;
						i++;
					}
					stringRef[r] = 0;

					buf[b] = 0;
					Q_strcat(buf, MAX_STRINGED_SV_STRING, CG_GetStringEdString("MP_SVGAME", stringRef));
					b = strlen(buf);
				}
			}
		}

		if (!gotStrip)
		{
			buf[b] = str[i];
			b++;
		}
		i++;
	}

	buf[b] = 0;
}

static void CG_BodyQueueCopy(centity_t *cent, int clientNum, int knownWeapon)
{
	centity_t		*source;
	animation_t		*anim;
	float			animSpeed;
	int				flags=BONE_ANIM_OVERRIDE_FREEZE;

	if (cent->ghoul2)
	{
		trap->G2API_CleanGhoul2Models(&cent->ghoul2);
	}

	if (clientNum < 0 || clientNum >= MAX_CLIENTS)
	{
		return;
	}

	source = &cg_entities[ clientNum ];

	if (!source)
	{
		return;
	}

	if (!source->ghoul2)
	{
		return;
	}

	cent->isRagging = false; //reset in case it's still set from another body that was in this cent slot.
	cent->ownerRagging = source->isRagging; //if the owner was in ragdoll state, then we want to go into it too right away.

	cent->bodyFadeTime = 0;
	cent->bodyHeight = 0;

	cent->dustTrailTime = source->dustTrailTime;

	trap->G2API_DuplicateGhoul2Instance(source->ghoul2, &cent->ghoul2);

	if (source->isRagging)
	{ //just reset it now.
		source->isRagging = false;
		trap->G2API_SetRagDoll(source->ghoul2, nullptr); //calling with null parms resets to no ragdoll.
	}

	//either force the weapon from when we died or remove it if it was a dropped weapon
	if (knownWeapon > WP_BRYAR_PISTOL && trap->G2API_HasGhoul2ModelOnIndex(&(cent->ghoul2), 1))
	{
		trap->G2API_RemoveGhoul2Model(&(cent->ghoul2), 1);
	}
	else if (trap->G2API_HasGhoul2ModelOnIndex(&(cent->ghoul2), 1))
	{
		trap->G2API_CopySpecificGhoul2Model(CG_G2WeaponInstance(cent, knownWeapon), 0, cent->ghoul2, 1);
	}

	if (!cent->ownerRagging)
	{
		int aNum;
		int eFrame;
		bool fallBack = false;

		//anim = &bgAllAnims[cent->localAnimIndex].anims[ cent->currentState.torsoAnim ];
		if (!BG_InDeathAnim(source->currentState.torsoAnim))
		{ //then just snap the corpse into a default
			anim = &bgAllAnims[source->localAnimIndex].anims[ BOTH_DEAD1 ];
			fallBack = true;
		}
		else
		{
			anim = &bgAllAnims[source->localAnimIndex].anims[ source->currentState.torsoAnim ];
		}
		animSpeed = 50.0f / anim->frameLerp;

		if (!fallBack)
		{
			//this will just set us to the last frame of the animation, in theory
			aNum = cgs.clientinfo[source->currentState.number].frame+1;

			while (aNum >= anim->firstFrame+anim->numFrames)
			{
				aNum--;
			}

			if (aNum < anim->firstFrame-1)
			{ //wrong animation...?
				aNum = (anim->firstFrame+anim->numFrames)-1;
			}
		}
		else
		{
			aNum = anim->firstFrame;
		}

		eFrame = anim->firstFrame + anim->numFrames;

		//if (!cgs.clientinfo[source->currentState.number].frame || (cent->currentState.torsoAnim) != (source->currentState.torsoAnim) )
		//{
		//	aNum = (anim->firstFrame+anim->numFrames)-1;
		//}

		trap->G2API_SetBoneAnim(cent->ghoul2, 0, "upper_lumbar", aNum, eFrame, flags, animSpeed, cg.time, -1, 150);
		trap->G2API_SetBoneAnim(cent->ghoul2, 0, "model_root", aNum, eFrame, flags, animSpeed, cg.time, -1, 150);
		trap->G2API_SetBoneAnim(cent->ghoul2, 0, "Motion", aNum, eFrame, flags, animSpeed, cg.time, -1, 150);
	}

	//After we create the bodyqueue, regenerate any limbs on the real instance
	if (source->torsoBolt)
	{
		CG_ReattachLimb(source);
	}
}

static void CG_NewForceRank_f( void ) {
	//"nfr" == "new force rank" (want a short string)
	int doMenu = 0;
	int setTeam = 0;
	int newRank = 0;

	if ( trap->Cmd_Argc() < 3 ) {
#ifdef _DEBUG
		trap->Print("WARNING: Invalid newForceRank string\n");
#endif
		return;
	}

	newRank = atoi( CG_Argv( 1 ) );
	doMenu = atoi( CG_Argv( 2 ) );
	setTeam = atoi( CG_Argv( 3 ) );

	trap->Cvar_Set( "ui_rankChange", va( "%i", newRank ) );

	trap->Cvar_Set( "ui_myteam", va( "%i", setTeam ) );

	if ( !( trap->Key_GetCatcher() & KEYCATCH_UI ) && doMenu && !cg.demoPlayback )
		trap->OpenUIMenu( UIMENU_PLAYERCONFIG );
}

static void CG_KillGhoul2_f( void ) {
	//Kill a ghoul2 instance in this slot.
	//If it has been occupied since this message was sent somehow, the worst that can (should) happen
	//is the instance will have to reinit with its current info.
	int indexNum = 0;
	int argNum = trap->Cmd_Argc();
	int i;

	if ( argNum < 1 )
		return;

	for ( i=1; i<argNum; i++ ) {
		indexNum = atoi( CG_Argv( i ) );

		if ( cg_entities[indexNum].ghoul2 && trap->G2_HaveWeGhoul2Models( cg_entities[indexNum].ghoul2 ) ) {
			if ( indexNum < MAX_CLIENTS ) { //You try to do very bad thing!
#ifdef _DEBUG
				Com_Printf("WARNING: Tried to kill a client ghoul2 instance with a kg2 command!\n");
#endif
				return;
			}

			CG_KillCEntityG2( indexNum );
		}
	}
}

static void CG_KillLoopSounds_f( void ) {
	//kill looping sounds
	int indexNum = 0;
	int argNum = trap->Cmd_Argc();
	centity_t *clent = nullptr;
	centity_t *trackerent = nullptr;

	if ( argNum < 1 ) {
		assert( 0 );
		return;
	}

	indexNum = atoi( CG_Argv( 1 ) );

	if ( indexNum >= 0 && indexNum < MAX_GENTITIES )
		clent = &cg_entities[indexNum];

	if ( argNum >= 2 ) {
		indexNum = atoi( CG_Argv( 2 ) );

		if ( indexNum >= 0 && indexNum < MAX_GENTITIES )
			trackerent = &cg_entities[indexNum];
	}

	if ( clent )
		CG_S_StopLoopingSound( clent->currentState.number, -1 );
	if ( trackerent )
		CG_S_StopLoopingSound( trackerent->currentState.number, -1 );
}

static void CG_RestoreClientGhoul_f( void ) {
	//rcg - Restore Client Ghoul (make sure limbs are reattached and ragdoll state is reset - this must be done reliably)
	int			indexNum = 0;
	int			argNum = trap->Cmd_Argc();
	centity_t	*clent;
	bool	IRCG = false;

	if ( !strcmp( CG_Argv( 0 ), "ircg" ) )
		IRCG = true;

	if ( argNum < 1 ) {
		assert( 0 );
		return;
	}

	indexNum = atoi( CG_Argv( 1 ) );
	if ( indexNum < 0 || indexNum >= MAX_CLIENTS ) {
		assert( 0 );
		return;
	}

	clent = &cg_entities[indexNum];

	//assert( clent->ghoul2 );
	//this can happen while connecting as a client
	if ( !clent->ghoul2 )
		return;

#ifdef _DEBUG
	if ( !trap->G2_HaveWeGhoul2Models( clent->ghoul2 ) )
		assert( !"Tried to reset state on a bad instance. Crash is inevitable." );
#endif

	if ( IRCG ) {
		int bodyIndex = 0;
		int weaponIndex = 0;
		int side = 0;
		centity_t *body;

		assert( argNum >= 3 );
		bodyIndex = atoi( CG_Argv( 2 ) );
		weaponIndex = atoi( CG_Argv( 3 ) );
		side = atoi( CG_Argv( 4 ) );

		body = &cg_entities[bodyIndex];

		if ( side )
			body->teamPowerType = 1; //light side
		else
			body->teamPowerType = 0; //dark side

		CG_BodyQueueCopy( body, clent->currentState.number, weaponIndex );
	}

	//reattach any missing limbs
	if ( clent->torsoBolt )
		CG_ReattachLimb(clent);

	//make sure ragdoll state is reset
	if ( clent->isRagging ) {
		clent->isRagging = false;
		trap->G2API_SetRagDoll( clent->ghoul2, nullptr ); //calling with null parms resets to no ragdoll.
	}

	//clear all the decals as well
	trap->G2API_ClearSkinGore( clent->ghoul2 );

	clent->weapon = 0;
	clent->ghoul2weapon = nullptr; //force a weapon reinit
}

static void CG_CenterPrint_f( void ) {
	char strEd[MAX_STRINGED_SV_STRING] = {0};

	CG_CheckSVStringEdRef( strEd, CG_Argv( 1 ) );
	CG_CenterPrint( strEd, SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
}

static void CG_CenterPrintSE_f( void ) {
	char strEd[MAX_STRINGED_SV_STRING] = {0};
	char *x = (char *)CG_Argv( 1 );

	if ( x[0] == '@' )
		x++;

	trap->SE_GetStringTextString( x, strEd, MAX_STRINGED_SV_STRING );
	CG_CenterPrint( strEd, SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
}

static void CG_Print_f( void ) {
	char strEd[MAX_STRINGED_SV_STRING] = {0};

	CG_CheckSVStringEdRef( strEd, CG_Argv( 1 ) );
	trap->Print( "%s", strEd );
}

static void CG_Chat_f( void ) {
	char cmd[MAX_STRING_CHARS] = {0}, text[MAX_SAY_TEXT] = {0};

	trap->Cmd_Argv( 0, cmd, sizeof( cmd ) );

	if ( !strcmp( cmd, "chat" ) ) {
		if ( !cg_teamChatsOnly.integer ) {
			if( cg_chatBeep.integer )
				trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
			trap->Cmd_Argv( 1, text, sizeof( text ) );
			CG_RemoveChatEscapeChar( text );
			CG_ChatBox_AddString( text );
			trap->Print( "*%s\n", text );
		}
	}
	else if ( !strcmp( cmd, "lchat" ) ) {
		if ( !cg_teamChatsOnly.integer ) {
			char	name[MAX_NETNAME]={0},	loc[MAX_STRING_CHARS]={0},
					color[8]={0},			message[MAX_STRING_CHARS]={0};

			if ( trap->Cmd_Argc() < 4 )
				return;

			trap->Cmd_Argv( 1, name, sizeof( name ) );
			trap->Cmd_Argv( 2, loc, sizeof( loc ) );
			trap->Cmd_Argv( 3, color, sizeof( color ) );
			trap->Cmd_Argv( 4, message, sizeof( message ) );

			//get localized text
			if ( loc[0] == '@' )
				trap->SE_GetStringTextString( loc+1, loc, sizeof( loc ) );

			if( cg_chatBeep.integer )
				trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
			Com_sprintf( text, sizeof( text ), "%s^7<%s> ^%s%s", name, loc, color, message );
			CG_RemoveChatEscapeChar( text );
			CG_ChatBox_AddString( text );
			trap->Print( "*%s\n", text );
		}
	}
	else if ( !strcmp( cmd, "tchat" ) ) {
		if( cg_teamChatBeep.integer )
			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		trap->Cmd_Argv( 1, text, sizeof( text ) );
		CG_RemoveChatEscapeChar( text );
		CG_ChatBox_AddString( text );
		trap->Print( "*%s\n", text );
	}
	else if ( !strcmp( cmd, "ltchat" ) ) {
		char	name[MAX_NETNAME]={0},	loc[MAX_STRING_CHARS]={0},
				color[8]={0},			message[MAX_STRING_CHARS]={0};

		if ( trap->Cmd_Argc() < 4 )
			return;

		trap->Cmd_Argv( 1, name, sizeof( name ) );
		trap->Cmd_Argv( 2, loc, sizeof( loc ) );
		trap->Cmd_Argv( 3, color, sizeof( color ) );
		trap->Cmd_Argv( 4, message, sizeof( message ) );

		//get localized text
		if ( loc[0] == '@' )
			trap->SE_GetStringTextString( loc+1, loc, sizeof( loc ) );

		if( cg_teamChatBeep.integer )
			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		Com_sprintf( text, sizeof( text ), "%s^7<%s> ^%s%s", name, loc, color, message );
		CG_RemoveChatEscapeChar( text );
		CG_ChatBox_AddString( text );
		trap->Print( "*%s\n", text );
	}
}

static void CG_RemapShader_f( void ) {
	if ( trap->Cmd_Argc() == 4 ) {
		char shader1[MAX_QPATH]={0},	shader2[MAX_QPATH]={0};

		trap->Cmd_Argv( 1, shader1, sizeof( shader1 ) );
		trap->Cmd_Argv( 2, shader2, sizeof( shader2 ) );
		trap->R_RemapShader( shader1, shader2, CG_Argv( 3 ) );
	}
}

static void CG_ClientLevelShot_f( void ) {
	// clientLevelShot is sent before taking a special screenshot for
	// the menu system during development
	cg.levelShot = true;
}

struct serverCommand_t {
	const char	*cmd;
	void		(*func)(void);
};

int svcmdcmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, ((serverCommand_t*)b)->cmd );
}

static constexpr serverCommand_t commands[] = {
	{ "chat",            CG_Chat_f },
	{ "clientLevelShot", CG_ClientLevelShot_f },
	{ "cp",              CG_CenterPrint_f },
	{ "cps",             CG_CenterPrintSE_f },
	{ "cs",              CG_ConfigStringModified },
	{ "ircg",            CG_RestoreClientGhoul_f },
	{ "kg2",             CG_KillGhoul2_f },
	{ "kls",             CG_KillLoopSounds_f },
	{ "lchat",           CG_Chat_f },
	// loaddeferred can be both a servercmd and a consolecmd
	{ "loaddefered",     CG_LoadDeferredPlayers }, // FIXME: spelled wrong, but not changing for demo
	{ "ltchat",          CG_Chat_f },
	{ "map_restart",     CG_MapRestart },
	{ "nfr",             CG_NewForceRank_f },
	{ "print",           CG_Print_f },
	{ "rcg",             CG_RestoreClientGhoul_f },
	{ "remapShader",     CG_RemapShader_f },
	{ "scores",          CG_ParseScores },
	{ "tchat",           CG_Chat_f },
	{ "tinfo",           CG_ParseTeamInfo },
};

static const size_t numCommands = ARRAY_LEN( commands );

// The string has been tokenized and can be retrieved with Cmd_Argc() / Cmd_Argv()
static void CG_ServerCommand( void ) {
	const char		*cmd = CG_Argv( 0 );
	serverCommand_t	*command = nullptr;

	if ( !cmd[0] ) {
		// server claimed the command
		return;
	}

	command = (serverCommand_t *)Q_LinearSearch( cmd, commands, numCommands, sizeof( commands[0] ), svcmdcmp );

	if ( command ) {
		command->func();
		return;
	}

	trap->Print( "Unknown client game command: %s\n", cmd );
}

// Execute all of the server commands that were received along with this this snapshot.
void CG_ExecuteNewServerCommands( int latestSequence ) {
	while ( cgs.serverCommandSequence < latestSequence ) {
		if ( trap->GetServerCommand( ++cgs.serverCommandSequence ) ) {
			CG_ServerCommand();
		}
	}
}
