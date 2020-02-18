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

// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cgame/cg_local.hpp"
#include "cgame/cg_media.hpp"
#include "ui/ui_shared.hpp"
#include "ui/ui_fonts.hpp"
#include "ui/menudef.h"

#define	SCOREBOARD_X		(0)

#define SB_HEADER			86
#define SB_TOP				(SB_HEADER+32)

// Where the status bar starts, so we don't overwrite it
#define SB_STATUSBAR		420

#define SB_NORMAL_HEIGHT	25
#define SB_INTER_HEIGHT		15 // interleaved height

#define SB_MAXCLIENTS_NORMAL  ((SB_STATUSBAR - SB_TOP) / SB_NORMAL_HEIGHT)
#define SB_MAXCLIENTS_INTER   ((SB_STATUSBAR - SB_TOP) / SB_INTER_HEIGHT - 1)

// Used when interleaved

#define SB_LEFT_BOTICON_X	(SCOREBOARD_X+0)
#define SB_LEFT_HEAD_X		(SCOREBOARD_X+32)
#define SB_RIGHT_BOTICON_X	(SCOREBOARD_X+64)
#define SB_RIGHT_HEAD_X		(SCOREBOARD_X+96)
// Normal
#define SB_BOTICON_X		(SCOREBOARD_X+32)
#define SB_HEAD_X			(SCOREBOARD_X+64)

#define SB_SCORELINE_X		100
#define SB_SCORELINE_WIDTH	(640 - SB_SCORELINE_X * 2)

#define SB_RATING_WIDTH	    0 // (6 * BIGCHAR_WIDTH)
#define SB_NAME_X			(SB_SCORELINE_X)
#define SB_SCORE_X			(SB_SCORELINE_X + .55f * SB_SCORELINE_WIDTH)
#define SB_PING_X			(SB_SCORELINE_X + .70f * SB_SCORELINE_WIDTH)
#define SB_TIME_X			(SB_SCORELINE_X + .85f * SB_SCORELINE_WIDTH)

// The new and improved score board
// In cases where the number of clients is high, the score board heads are interleaved
// here's the layout
//	0   32   80  112  144   240  320  400   <-- pixel position
//  bot head bot head score ping time name
//  wins/losses are drawn on bot icon now

static bool localClient; // true if local client has been displayed

static void CG_DrawClientScore( int y, score_t *score, float *color, float fade, bool largeFormat )
{
	//vec3_t	headAngles;
	clientInfo_t	*ci;
	int				iconx = SB_BOTICON_X + (SB_RATING_WIDTH / 2);
	float			scale = largeFormat ? 1.0f : 0.75f,
					iconSize = largeFormat ? SB_NORMAL_HEIGHT : SB_INTER_HEIGHT;

	if ( score->client < 0 || score->client >= cgs.maxclients ) {
		Com_Printf( "Bad score->client: %i\n", score->client );
		return;
	}

	ci = &cgs.clientinfo[score->client];

	// draw the handicap or bot skill marker (unless player has flag)
	if ( ci->powerups & (1<<PW_NEUTRALFLAG) )
	{
		if ( largeFormat )
			CG_DrawFlagModel( iconx, y - (32 - BIGCHAR_HEIGHT) / 2, iconSize, iconSize, TEAM_FREE, false );
		else
			CG_DrawFlagModel( iconx, y, iconSize, iconSize, TEAM_FREE, false );
	}

	else if ( ci->powerups & ( 1 << PW_REDFLAG ) )
		CG_DrawFlagModel( iconx, y, iconSize, iconSize, TEAM_RED, false );

	else if ( ci->powerups & ( 1 << PW_BLUEFLAG ) )
		CG_DrawFlagModel( iconx, y, iconSize, iconSize, TEAM_BLUE, false );

	else if ( cgs.gametype == GT_POWERDUEL && (ci->duelTeam == DUELTEAM_LONE || ci->duelTeam == DUELTEAM_DOUBLE) )
	{
		const qhandle_t shader = media.gfx.null;//(ci->duelTeam == DUELTEAM_LONE) ? "gfx/mp/pduel_icon_lone" : "gfx/mp/pduel_icon_double";
		CG_DrawPic( iconx, y, iconSize, iconSize, shader );
	}

	// highlight your position
	if ( score->client == cg.snap->ps.clientNum )
	{
		float	hcolor[4];
		int		rank;

		localClient = true;

		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR
			|| cgs.gametype >= GT_TEAM ) {
			rank = -1;
		} else {
			rank = cg.snap->ps.persistant[PERS_RANK] & ~RANK_TIED_FLAG;
		}
		if ( rank == 0 ) {
			hcolor[0] = 0;
			hcolor[1] = 0;
			hcolor[2] = 0.7f;
		} else if ( rank == 1 ) {
			hcolor[0] = 0.7f;
			hcolor[1] = 0;
			hcolor[2] = 0;
		} else if ( rank == 2 ) {
			hcolor[0] = 0.7f;
			hcolor[1] = 0.7f;
			hcolor[2] = 0;
		} else {
			hcolor[0] = 0.7f;
			hcolor[1] = 0.7f;
			hcolor[2] = 0.7f;
		}

		hcolor[3] = fade * 0.7;
		CG_FillRect( SB_SCORELINE_X - 5, y + 2, 640 - SB_SCORELINE_X * 2 + 10, largeFormat?SB_NORMAL_HEIGHT:SB_INTER_HEIGHT, hcolor );
	}

	Font font( FONT_MEDIUM, scale );
	font.Paint (SB_NAME_X, y, ci->name, colorWhite, ITEM_TEXTSTYLE_OUTLINED );

	if ( score->ping != -1 )
	{
		if ( ci->team != TEAM_SPECTATOR || cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL )
		{
			if (cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL)
			{
				font.Paint (SB_SCORE_X, y, va("%i/%i", ci->wins, ci->losses),colorWhite, ITEM_TEXTSTYLE_OUTLINED );
			}
			else
			{
				font.Paint (SB_SCORE_X, y, va("%i", score->score),colorWhite, ITEM_TEXTSTYLE_OUTLINED );
			}
		}

		if ( cg_scoreboardBots.integer && ci->botSkill != -1 )
			font.Paint( SB_PING_X, y, "BOT", colorWhite, ITEM_TEXTSTYLE_OUTLINED );
		else
			font.Paint (SB_PING_X, y, va("%i", score->ping),colorWhite, ITEM_TEXTSTYLE_OUTLINED );
		font.Paint (SB_TIME_X, y, va("%i", score->time),colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}
	else
	{
		font.Paint (SB_SCORE_X, y, "-",colorWhite, ITEM_TEXTSTYLE_OUTLINED );
		font.Paint (SB_PING_X, y, "-",colorWhite, ITEM_TEXTSTYLE_OUTLINED );
		font.Paint (SB_TIME_X, y, "-",colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}

	// add the "ready" marker for intermission exiting
	if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << score->client ) )
	{
		font.Paint (SB_NAME_X - 64, y + 2, CG_GetStringEdString("MP_INGAME", "READY"), colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}
}

static int CG_TeamScoreboard( int y, team_e team, float fade, int maxClients, int lineHeight, bool countOnly )
{
	int		i;
	score_t	*score;
	float	color[4];
	int		count;
	clientInfo_t	*ci;

	color[0] = color[1] = color[2] = 1.0;
	color[3] = fade;

	count = 0;
	for ( i = 0 ; i < cg.numScores && count < maxClients ; i++ ) {
		score = &cg.scores[i];
		ci = &cgs.clientinfo[ score->client ];

		if ( team != ci->team ) {
			continue;
		}

		if ( !countOnly )
		{
			CG_DrawClientScore( y + lineHeight * count, score, color, fade, lineHeight == SB_NORMAL_HEIGHT );
		}

		count++;
	}

	return count;
}

int CG_GetTeamNonScoreCount(team_e team)
{
	int i = 0,count=0;
	clientInfo_t	*ci;

	for ( i = 0 ; i < cgs.maxclients ; i++ )
	{
		ci = &cgs.clientinfo[ i ];

		if ( !ci->infoValid || team != ci->team )
		{
			continue;
		}

		count++;
	}

 	return count;
}

int CG_GetTeamCount(team_e team, int maxClients)
{
	int i = 0;
	int count = 0;
	clientInfo_t	*ci;
	score_t	*score;

	for ( i = 0 ; i < cg.numScores && count < maxClients ; i++ )
	{
		score = &cg.scores[i];
		ci = &cgs.clientinfo[ score->client ];

		if ( team != ci->team )
		{
			continue;
		}

		count++;
	}

	return count;

}

// Draw the normal in-game scoreboard
bool CG_DrawOldScoreboard( void ) {
	int		x, y, i, n1, n2;
	float	fade;
	float	*fadeColor;
	char	*s;
	int maxClients, realMaxClients;
	int lineHeight;
	int topBorderSize, bottomBorderSize;

	// don't draw amuthing if the menu or console is up
	if ( cl_paused.integer ) {
		cg.deferredPlayerLoading = 0;
		return false;
	}

	// don't draw scoreboard during death while warmup up
	if ( cg.warmup && !cg.showScores ) {
		return false;
	}

	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
		 cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );

		if ( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return false;
		}
		fade = *fadeColor;
	}

	Font font( FONT_MEDIUM, 1.0f );

	// fragged by ... line
	// or if in intermission and duel, prints the winner of the duel round
	if ((cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL) && cgs.duelWinner != -1 &&
		cg.predictedPlayerState.pm_type == PM_INTERMISSION)
	{
		s = va("%s^7 %s", cgs.clientinfo[cgs.duelWinner].name, CG_GetStringEdString("MP_INGAME", "DUEL_WINS") );
		/*w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		x = ( SCREEN_WIDTH - w ) / 2;
		y = 40;
		CG_DrawBigString( x, y, s, fade );
		*/
		x = ( SCREEN_WIDTH ) / 2;
		y = 40;
		font.Paint ( x - font.Width( s ) / 2, y, s, colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}
	else if ((cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL) && cgs.duelist1 != -1 && cgs.duelist2 != -1 &&
		cg.predictedPlayerState.pm_type == PM_INTERMISSION)
	{
		if (cgs.gametype == GT_POWERDUEL && cgs.duelist3 != -1)
		{
			s = va("%s^7 %s %s^7 %s %s", cgs.clientinfo[cgs.duelist1].name, CG_GetStringEdString("MP_INGAME", "SPECHUD_VERSUS"), cgs.clientinfo[cgs.duelist2].name, CG_GetStringEdString("MP_INGAME", "AND"), cgs.clientinfo[cgs.duelist3].name );
		}
		else
		{
			s = va("%s^7 %s %s", cgs.clientinfo[cgs.duelist1].name, CG_GetStringEdString("MP_INGAME", "SPECHUD_VERSUS"), cgs.clientinfo[cgs.duelist2].name );
		}
		/*w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		x = ( SCREEN_WIDTH - w ) / 2;
		y = 40;
		CG_DrawBigString( x, y, s, fade );
		*/
		x = ( SCREEN_WIDTH ) / 2;
		y = 40;
		font.Paint ( x - font.Width( s ) / 2, y, s, colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}
	else if ( cg.killerName[0] ) {
		s = va("%s %s", CG_GetStringEdString("MP_INGAME", "KILLEDBY"), cg.killerName );
		/*w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		x = ( SCREEN_WIDTH - w ) / 2;
		y = 40;
		CG_DrawBigString( x, y, s, fade );
		*/
		x = ( SCREEN_WIDTH ) / 2;
		y = 40;
		font.Paint ( x - font.Width( s ) / 2, y, s, colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}

	// current rank
	if (cgs.gametype == GT_POWERDUEL)
	{ //do nothing?
	}
	else if ( cgs.gametype < GT_TEAM) {
		if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
		{
			char sPlace[256];
			char sOf[256];
			char sWith[256];

			trap->SE_GetStringTextString("MP_INGAME_PLACE",	sPlace,	sizeof(sPlace));
			trap->SE_GetStringTextString("MP_INGAME_OF",		sOf,	sizeof(sOf));
			trap->SE_GetStringTextString("MP_INGAME_WITH",	sWith,	sizeof(sWith));

			s = va("%s %s (%s %i) %s %i",
				CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),
				sPlace,
				sOf,
				cg.numScores,
				sWith,
				cg.snap->ps.persistant[PERS_SCORE] );
		//	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
			x = ( SCREEN_WIDTH ) / 2;
			y = 60;
			//CG_DrawBigString( x, y, s, fade );
			Text_Paint_Proportional(x, y, s, UI_CENTER|UI_DROPSHADOW, colorTable[CT_WHITE]);
		}
	}
	else
	{
		if ( cg.teamScores[0] == cg.teamScores[1] ) {
			s = va("%s %i", CG_GetStringEdString("MP_INGAME", "TIEDAT"), cg.teamScores[0] );
		} else if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			s = va("%s, %i / %i", CG_GetStringEdString("MP_INGAME", "RED_LEADS"), cg.teamScores[0], cg.teamScores[1] );
		} else {
			s = va("%s, %i / %i", CG_GetStringEdString("MP_INGAME", "BLUE_LEADS"), cg.teamScores[1], cg.teamScores[0] );
		}

		x = ( SCREEN_WIDTH ) / 2;
		y = 60;

		font.Paint ( x - font.Width( s ) / 2, y, s, colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}

	// scoreboard
	y = SB_HEADER;

	CG_DrawPic ( SB_SCORELINE_X - 40, y - 5, SB_SCORELINE_WIDTH + 80, 40, media.gfx.null/*media.gfx.interface.button.back "gfx/menus/menu_buttonback.tga"*/ );

	font.Paint ( SB_NAME_X, y, CG_GetStringEdString("MP_INGAME", "NAME"),colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	if (cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL)
	{
		char sWL[100];
		trap->SE_GetStringTextString("MP_INGAME_W_L", sWL,	sizeof(sWL));

		font.Paint ( SB_SCORE_X, y, sWL, colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}
	else
	{
		font.Paint ( SB_SCORE_X, y, CG_GetStringEdString("MP_INGAME", "SCORE"), colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	}
	font.Paint ( SB_PING_X, y, CG_GetStringEdString("MP_INGAME", "PING"), colorWhite, ITEM_TEXTSTYLE_OUTLINED );
	font.Paint ( SB_TIME_X, y, CG_GetStringEdString("MP_INGAME", "TIME"), colorWhite, ITEM_TEXTSTYLE_OUTLINED );

	y = SB_TOP;

	// If there are more than SB_MAXCLIENTS_NORMAL, use the interleaved scores
	if ( cg.numScores > SB_MAXCLIENTS_NORMAL ) {
		maxClients = SB_MAXCLIENTS_INTER;
		lineHeight = SB_INTER_HEIGHT;
		topBorderSize = 8;
		bottomBorderSize = 16;
	} else {
		maxClients = SB_MAXCLIENTS_NORMAL;
		lineHeight = SB_NORMAL_HEIGHT;
		topBorderSize = 8;
		bottomBorderSize = 8;
	}
	realMaxClients = maxClients;

	localClient = false;

	//I guess this should end up being able to display 19 clients at once.
	//In a team game, if there are 9 or more clients on the team not in the lead,
	//we only want to show 10 of the clients on the team in the lead, so that we
	//have room to display the clients in the lead on the losing team.

	//I guess this can be accomplished simply by printing the first teams score with a maxClients
	//value passed in related to how many players are on both teams.
	if ( cgs.gametype >= GT_TEAM ) {

		// teamplay scoreboard

		y += lineHeight/2;

		if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			int team1MaxCl = CG_GetTeamCount(TEAM_RED, maxClients);
			int team2MaxCl = CG_GetTeamCount(TEAM_BLUE, maxClients);

			if (team1MaxCl > 10 && (team1MaxCl+team2MaxCl) > maxClients)
			{
				team1MaxCl -= team2MaxCl;
				//subtract as many as you have to down to 10, once we get there
				//we just set it to 10

				if (team1MaxCl < 10)
				{
					team1MaxCl = 10;
				}
			}

			team2MaxCl = (maxClients-team1MaxCl); //team2 can display however many is left over after team1's display

			n1 = CG_TeamScoreboard( y, TEAM_RED, fade, team1MaxCl, lineHeight, true );
			CG_DrawTeamBackground( SB_SCORELINE_X - 5, y - topBorderSize, 640 - SB_SCORELINE_X * 2 + 10, n1 * lineHeight + bottomBorderSize, 0.33f, TEAM_RED );
			CG_TeamScoreboard( y, TEAM_RED, fade, team1MaxCl, lineHeight, false );
			y += (n1 * lineHeight) + BIGCHAR_HEIGHT;

			//maxClients -= n1;

			n2 = CG_TeamScoreboard( y, TEAM_BLUE, fade, team2MaxCl, lineHeight, true );
			CG_DrawTeamBackground( SB_SCORELINE_X - 5, y - topBorderSize, 640 - SB_SCORELINE_X * 2 + 10, n2 * lineHeight + bottomBorderSize, 0.33f, TEAM_BLUE );
			CG_TeamScoreboard( y, TEAM_BLUE, fade, team2MaxCl, lineHeight, false );
			y += (n2 * lineHeight) + BIGCHAR_HEIGHT;

			//maxClients -= n2;

			maxClients -= (team1MaxCl+team2MaxCl);
		} else {
			int team1MaxCl = CG_GetTeamCount(TEAM_BLUE, maxClients);
			int team2MaxCl = CG_GetTeamCount(TEAM_RED, maxClients);

			if (team1MaxCl > 10 && (team1MaxCl+team2MaxCl) > maxClients)
			{
				team1MaxCl -= team2MaxCl;
				//subtract as many as you have to down to 10, once we get there
				//we just set it to 10

				if (team1MaxCl < 10)
				{
					team1MaxCl = 10;
				}
			}

			team2MaxCl = (maxClients-team1MaxCl); //team2 can display however many is left over after team1's display

			n1 = CG_TeamScoreboard( y, TEAM_BLUE, fade, team1MaxCl, lineHeight, true );
			CG_DrawTeamBackground( SB_SCORELINE_X - 5, y - topBorderSize, 640 - SB_SCORELINE_X * 2 + 10, n1 * lineHeight + bottomBorderSize, 0.33f, TEAM_BLUE );
			CG_TeamScoreboard( y, TEAM_BLUE, fade, team1MaxCl, lineHeight, false );
			y += (n1 * lineHeight) + BIGCHAR_HEIGHT;

			//maxClients -= n1;

			n2 = CG_TeamScoreboard( y, TEAM_RED, fade, team2MaxCl, lineHeight, true );
			CG_DrawTeamBackground( SB_SCORELINE_X - 5, y - topBorderSize, 640 - SB_SCORELINE_X * 2 + 10, n2 * lineHeight + bottomBorderSize, 0.33f, TEAM_RED );
			CG_TeamScoreboard( y, TEAM_RED, fade, team2MaxCl, lineHeight, false );
			y += (n2 * lineHeight) + BIGCHAR_HEIGHT;

			//maxClients -= n2;

			maxClients -= (team1MaxCl+team2MaxCl);
		}
		maxClients = realMaxClients;
		n1 = CG_TeamScoreboard( y, TEAM_SPECTATOR, fade, maxClients, lineHeight, false );
		y += (n1 * lineHeight) + BIGCHAR_HEIGHT;

	} else {

		// free for all scoreboard

		n1 = CG_TeamScoreboard( y, TEAM_FREE, fade, maxClients, lineHeight, false );
		y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
		n2 = CG_TeamScoreboard( y, TEAM_SPECTATOR, fade, maxClients - n1, lineHeight, false );
		y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
	}

	if (!localClient) {
		// draw local client at the bottom
		for ( i = 0 ; i < cg.numScores ; i++ ) {
			if ( cg.scores[i].client == cg.snap->ps.clientNum ) {
				CG_DrawClientScore( y, &cg.scores[i], fadeColor, fade, lineHeight == SB_NORMAL_HEIGHT );
				break;
			}
		}
	}

	// load any models that have been deferred
	if ( ++cg.deferredPlayerLoading > 10 ) {
		CG_LoadDeferredPlayers();
	}

	return true;
}
