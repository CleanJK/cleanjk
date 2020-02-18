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

// cg_info.c -- display information while data is being loading

#include "cgame/cg_local.hpp"
#include "cgame/cg_media.hpp"
#include "ui/ui_fonts.hpp"

#define MAX_LOADING_PLAYER_ICONS	16
#define MAX_LOADING_ITEM_ICONS		26

//static int			loadingPlayerIconCount;
//static qhandle_t	loadingPlayerIcons[MAX_LOADING_PLAYER_ICONS];

void CG_LoadingString( const char *s ) {
	Q_strncpyz( cg.infoScreenText, s, sizeof( cg.infoScreenText ) );

	trap->UpdateScreen();
}

void CG_LoadingItem( int itemNum ) {
	gitem_t		*item;
	char	upperKey[1024];

	item = &bg_itemlist[itemNum];

	if (!item->classname || !item->classname[0])
	{
	//	CG_LoadingString( "Unknown item" );
		return;
	}

	Q_strncpyz(upperKey, item->classname, sizeof(upperKey));
	CG_LoadingString( CG_GetStringEdString("SP_INGAME",Q_strupr(upperKey)) );
}

void CG_LoadingClient( int clientNum ) {
	char personality[MAX_QPATH];
	const char *info = CG_ConfigString( CS_PLAYERS + clientNum );
	Q_strncpyz( personality, Info_ValueForKey( info, "n" ), sizeof(personality) );

	CG_LoadingString( personality );
}

static void CG_LoadBar( void ) {
	const float barWidth = SCREEN_HEIGHT, barHeight = 12.0f;
	const float barX = (SCREEN_WIDTH - barWidth) / 2.0f, barY = SCREEN_HEIGHT - 8.0f - barHeight;
	const float capWidth = 8.0f;

	trap->R_SetColor( colorWhite );

	// background, left cap, bar, right cap
	CG_DrawPic( barX, barY, barWidth, barHeight, media.gfx.interface.loading.background );
	CG_DrawPic( barX + capWidth, barY, -capWidth, barHeight, media.gfx.interface.loading.cap );
	CG_DrawPic( barX + capWidth, barY, ((barWidth - (capWidth*2.0f))*cg.loadFrac), barHeight, media.gfx.interface.loading.tick );
	CG_DrawPic( barX + ((barWidth - (capWidth*2.0f))*cg.loadFrac) + capWidth, barY, capWidth, barHeight, media.gfx.interface.loading.cap );
}

// Draw all the status / pacifier stuff during level loading
// overlays UI_DrawConnectScreen
void CG_DrawInformation( void ) {
	const char	*s;
	const char	*info;
	const char	*sysInfo;
	int			y;
	int			value, valueNOFP;
	qhandle_t	levelshot;
	char		buf[1024];
	int			iPropHeight = 18;	// I know, this is total crap, but as a post release asian-hack....  -Ste

	info = CG_ConfigString( CS_SERVERINFO );
	sysInfo = CG_ConfigString( CS_SYSTEMINFO );

	//CJKFIXME: most of this function's data can be safely cached

	s = Info_ValueForKey( info, "mapname" );
	levelshot = trap->R_RegisterShaderNoMip( va( "levelshots/%s", s ) );
	if ( !levelshot ) {
		levelshot = media.gfx.null /*"menu/art/unknownmap_mp"*/;
	}
	trap->R_SetColor( nullptr );
	CG_DrawPic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, levelshot );

	CG_LoadBar();

	// draw the icons of things as they are loaded
//	CG_DrawLoadingIcons();

	// the first 150 rows are reserved for the client connection
	// screen to write into
	if ( cg.infoScreenText[0] ) {
		const char *psLoading = CG_GetStringEdString("MENUS", "LOADING_MAPNAME");
		Text_Paint_Proportional( 320, 128-32, va(/*"Loading... %s"*/ psLoading, cg.infoScreenText), UI_CENTER|UI_DROPSHADOW, colorWhite );
	} else {
		const char *psAwaitingSnapshot = CG_GetStringEdString("MENUS", "AWAITING_SNAPSHOT");
		Text_Paint_Proportional( 320, 128-32, /*"Awaiting snapshot..."*/psAwaitingSnapshot, UI_CENTER|UI_DROPSHADOW, colorWhite );
	}

	// draw info string information

	y = 180-32;

	// don't print server lines if playing a local game
	if ( !sv_running.integer ) {
		// server hostname
		Q_strncpyz(buf, Info_ValueForKey( info, "sv_hostname" ), sizeof( buf ) );
		Q_CleanStr(buf);
		Text_Paint_Proportional( 320, y, buf, UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;

		// pure server
		s = Info_ValueForKey( sysInfo, "sv_pure" );
		if ( s[0] == '1' ) {
			const char *psPure = CG_GetStringEdString("MP_INGAME", "PURE_SERVER");
			Text_Paint_Proportional( 320, y, psPure, UI_CENTER|UI_DROPSHADOW, colorWhite );
			y += iPropHeight;
		}

		// server-specific message of the day
		s = CG_ConfigString( CS_MOTD );
		if ( s[0] ) {
			Text_Paint_Proportional( 320, y, s, UI_CENTER|UI_DROPSHADOW, colorWhite );
			y += iPropHeight;
		}

		{	// display global MOTD at bottom (mirrors ui_main UI_DrawConnectScreen
			if ( cl_motdString.string[0] ) {
				Text_Paint_Proportional( 320, 425, cl_motdString.string, UI_CENTER|UI_DROPSHADOW, colorWhite );
			}
		}

		// some extra space after hostname and motd
		y += 10;
	}

	// map-specific message (long map name)
	s = CG_ConfigString( CS_MESSAGE );
	if ( s[0] ) {
		Text_Paint_Proportional( 320, y, s, UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}

	// cheats warning
	s = Info_ValueForKey( sysInfo, "sv_cheats" );
	if ( s[0] == '1' ) {
		Text_Paint_Proportional( 320, y, CG_GetStringEdString("MP_INGAME", "CHEATSAREENABLED"), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}

	// game type
	s = BG_GetGametypeString( cgs.gametype );
	Text_Paint_Proportional( 320, y, s, UI_CENTER|UI_DROPSHADOW, colorWhite );
	y += iPropHeight;

	value = atoi( Info_ValueForKey( info, "timelimit" ) );
	if ( value ) {
		Text_Paint_Proportional( 320, y, va( "%s %i", CG_GetStringEdString("MP_INGAME", "TIMELIMIT"), value ), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}

	if (cgs.gametype < GT_CTF ) {
		value = atoi( Info_ValueForKey( info, "fraglimit" ) );
		if ( value ) {
			Text_Paint_Proportional( 320, y, va( "%s %i", CG_GetStringEdString("MP_INGAME", "FRAGLIMIT"), value ), UI_CENTER|UI_DROPSHADOW, colorWhite );
			y += iPropHeight;
		}

		if (cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL)
		{
			value = atoi( Info_ValueForKey( info, "duel_fraglimit" ) );
			if ( value ) {
				Text_Paint_Proportional( 320, y, va( "%s %i", CG_GetStringEdString("MP_INGAME", "WINLIMIT"), value ), UI_CENTER|UI_DROPSHADOW, colorWhite );
				y += iPropHeight;
			}
		}
	}

	if (cgs.gametype >= GT_CTF) {
		value = atoi( Info_ValueForKey( info, "capturelimit" ) );
		if ( value ) {
			Text_Paint_Proportional( 320, y, va( "%s %i", CG_GetStringEdString("MP_INGAME", "CAPTURELIMIT"), value ), UI_CENTER|UI_DROPSHADOW, colorWhite );
			y += iPropHeight;
		}
	}

	if (cgs.gametype >= GT_TEAM)
	{
		value = atoi( Info_ValueForKey( info, "g_forceBasedTeams" ) );
		if ( value ) {
			Text_Paint_Proportional( 320, y, CG_GetStringEdString("MP_INGAME", "FORCEBASEDTEAMS"), UI_CENTER|UI_DROPSHADOW, colorWhite );
			y += iPropHeight;
		}
	}

	valueNOFP = atoi( Info_ValueForKey( info, "g_forcePowerDisable" ) );

	value = atoi( Info_ValueForKey( info, "g_maxForceRank" ) );
	if ( value && !valueNOFP && (value < NUM_FORCE_MASTERY_LEVELS) ) {
		char fmStr[1024];

		trap->SE_GetStringTextString("MP_INGAME_MAXFORCERANK",fmStr, sizeof(fmStr));

		Text_Paint_Proportional( 320, y, va( "%s %s", fmStr, CG_GetStringEdString("MP_INGAME", forceMasteryLevels[value]) ), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}
	else if (!valueNOFP)
	{
		char fmStr[1024];
		trap->SE_GetStringTextString("MP_INGAME_MAXFORCERANK",fmStr, sizeof(fmStr));

		Text_Paint_Proportional( 320, y, va( "%s %s", fmStr, (char *)CG_GetStringEdString("MP_INGAME", forceMasteryLevels[7]) ), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}

	if (cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL)
	{
		value = atoi( Info_ValueForKey( info, "g_duelWeaponDisable" ) );
	}
	else
	{
		value = atoi( Info_ValueForKey( info, "g_weaponDisable" ) );
	}
	if ( cgs.gametype != GT_JEDIMASTER && value ) {
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "SABERONLYSET") ), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}

	if ( valueNOFP ) {
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "NOFPSET") ), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
	}

	// Display the rules based on type
		y += iPropHeight;
	switch ( cgs.gametype ) {
	case GT_FFA:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_FFA_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_HOLOCRON:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_HOLO_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_HOLO_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_JEDIMASTER:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_JEDI_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_JEDI_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_DUEL:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_DUEL_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_DUEL_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_POWERDUEL:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_POWERDUEL_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_POWERDUEL_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_TEAM:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_TEAM_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_TEAM_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_CTF:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_CTF_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_CTF_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	case GT_CTY:
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_CTY_1")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		Text_Paint_Proportional( 320, y, va( "%s", (char *)CG_GetStringEdString("MP_INGAME", "RULES_CTY_2")), UI_CENTER|UI_DROPSHADOW, colorWhite );
		y += iPropHeight;
		break;
	default:
		break;
	}
}
