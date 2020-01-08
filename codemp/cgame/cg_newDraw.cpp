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

#include "cgame/cg_local.h"
#include "ui/ui_shared.h"
#include "ui/ui_fonts.h"
#include "ui/menudef.h"
#include "cgame/cg_media.h"

extern displayContextDef_t cgDC;

int CG_GetSelectedPlayer() {
	if (cg_currentSelectedPlayer.integer < 0 || cg_currentSelectedPlayer.integer >= numSortedTeamPlayers) {
		cg_currentSelectedPlayer.integer = 0;
	}
	return cg_currentSelectedPlayer.integer;
}

qhandle_t CG_StatusHandle(int task) {
	qhandle_t h = media.gfx.null;
	switch (task) {
	case TEAMTASK_OFFENSE :
		h = media.gfx.null;
		break;
	case TEAMTASK_DEFENSE :
		h = media.gfx.null;
		break;
	case TEAMTASK_PATROL :
		h = media.gfx.null;
		break;
	case TEAMTASK_FOLLOW :
		h = media.gfx.null;
		break;
	case TEAMTASK_CAMP :
		h = media.gfx.null;
		break;
	case TEAMTASK_RETRIEVE :
		h = media.gfx.null;
		break;
	case TEAMTASK_ESCORT :
		h = media.gfx.null;
		break;
	default :
		h = media.gfx.null;
		break;
	}
	return h;
}

float CG_GetValue(int ownerDraw) {
	centity_t	*cent;
	clientInfo_t *ci;
	playerState_t	*ps;

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	switch (ownerDraw) {
	case CG_SELECTEDPLAYER_ARMOR:
		ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
		return ci->armor;
		break;
	case CG_SELECTEDPLAYER_HEALTH:
		ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
		return ci->health;
		break;
	case CG_PLAYER_ARMOR_VALUE:
		return ps->stats[STAT_ARMOR];
		break;
	case CG_PLAYER_AMMO_VALUE:
		if ( cent->currentState.weapon )
		{
			return ps->ammo[weaponData[cent->currentState.weapon].ammoIndex];
		}
		break;
	case CG_PLAYER_SCORE:
		return cg.snap->ps.persistant[PERS_SCORE];
		break;
	case CG_PLAYER_HEALTH:
		return ps->stats[STAT_HEALTH];
		break;
	case CG_RED_SCORE:
		return cgs.scores1;
		break;
	case CG_BLUE_SCORE:
		return cgs.scores2;
		break;
	case CG_PLAYER_FORCE_VALUE:
		return ps->fd.forcePower;
		break;
	default:
		break;
	}
	return -1;
}

bool CG_OtherTeamHasFlag(void) {
	if (cgs.gametype == GT_CTF || cgs.gametype == GT_CTY) {
		int team = cg.snap->ps.persistant[PERS_TEAM];
		if (team == TEAM_RED && cgs.redflag == FLAG_TAKEN) {
			return true;
		} else if (team == TEAM_BLUE && cgs.blueflag == FLAG_TAKEN) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

bool CG_YourTeamHasFlag(void) {
	if (cgs.gametype == GT_CTF || cgs.gametype == GT_CTY) {
		int team = cg.snap->ps.persistant[PERS_TEAM];
		if (team == TEAM_RED && cgs.blueflag == FLAG_TAKEN) {
			return true;
		} else if (team == TEAM_BLUE && cgs.redflag == FLAG_TAKEN) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

// THINKABOUTME: should these be exclusive or inclusive..

bool CG_OwnerDrawVisible(int flags) {

	if (flags & CG_SHOW_TEAMINFO) {
		return (cg_currentSelectedPlayer.integer == numSortedTeamPlayers);
	}

	if (flags & CG_SHOW_NOTEAMINFO) {
		return !(cg_currentSelectedPlayer.integer == numSortedTeamPlayers);
	}

	if (flags & CG_SHOW_OTHERTEAMHASFLAG) {
		return CG_OtherTeamHasFlag();
	}

	if (flags & CG_SHOW_YOURTEAMHASENEMYFLAG) {
		return CG_YourTeamHasFlag();
	}

	if (flags & (CG_SHOW_BLUE_TEAM_HAS_REDFLAG | CG_SHOW_RED_TEAM_HAS_BLUEFLAG)) {
		if (flags & CG_SHOW_BLUE_TEAM_HAS_REDFLAG && (cgs.redflag == FLAG_TAKEN || cgs.flagStatus == FLAG_TAKEN_RED)) {
			return true;
		} else if (flags & CG_SHOW_RED_TEAM_HAS_BLUEFLAG && (cgs.blueflag == FLAG_TAKEN || cgs.flagStatus == FLAG_TAKEN_BLUE)) {
			return true;
		}
		return false;
	}

	if (flags & CG_SHOW_ANYTEAMGAME) {
		if( cgs.gametype >= GT_TEAM) {
			return true;
		}
	}

	if (flags & CG_SHOW_ANYNONTEAMGAME) {
		if( cgs.gametype < GT_TEAM) {
			return true;
		}
	}

	if (flags & CG_SHOW_CTF) {
		if( cgs.gametype == GT_CTF || cgs.gametype == GT_CTY ) {
			return true;
		}
	}

	if (flags & CG_SHOW_HEALTHCRITICAL) {
		if (cg.snap->ps.stats[STAT_HEALTH] < 25) {
			return true;
		}
	}

	if (flags & CG_SHOW_HEALTHOK) {
		if (cg.snap->ps.stats[STAT_HEALTH] >= 25) {
			return true;
		}
	}

	if (flags & CG_SHOW_DURINGINCOMINGVOICE) {
	}

	if (flags & CG_SHOW_IF_PLAYER_HAS_FLAG) {
		if (cg.snap->ps.powerups[PW_REDFLAG] || cg.snap->ps.powerups[PW_BLUEFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG]) {
			return true;
		}
	}
	return false;
}

const char *CG_GetKillerText(void) {
	static const char *s = "";
	if ( cg.killerName[0] ) {
		s = va("%s %s", CG_GetStringEdString("MP_INGAME", "KILLEDBY"), cg.killerName );
	}
	return s;
}

const char *CG_GetGameStatusText(void) {
	static const char *s = "";
	if (cgs.gametype == GT_POWERDUEL)
	{
		s = "";
	}
	else if ( cgs.gametype < GT_TEAM)
	{
		if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
		{
			char sPlaceWith[256];
			trap->SE_GetStringTextString("MP_INGAME_PLACE_WITH", sPlaceWith, sizeof(sPlaceWith));

			s = va("%s %s %i",CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ), sPlaceWith, cg.snap->ps.persistant[PERS_SCORE] );
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
	}
	return s;
}

// maxX param is initially an X limit, but is also used as feedback. 0 = text was clipped to fit within, else maxX = next pos

#define PIC_WIDTH 12

void CG_DrawNewTeamInfo(rectDef_t *rect, float text_x, float text_y, float scale, vec4_t color, qhandle_t shader) {
	int xx;
	float y;
	int i, j, len, count;
	const char *p;
	vec4_t		hcolor;
	float pwidth, lwidth, maxx, leftOver;
	clientInfo_t *ci;
	gitem_t	*item;
	qhandle_t h;

	Font font( FONT_MEDIUM, scale );

	// max player name width
	pwidth = 0;
	count = (numSortedTeamPlayers > 8) ? 8 : numSortedTeamPlayers;
	for (i = 0; i < count; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {
			len = font.Width( ci->name );
			if (len > pwidth)
				pwidth = len;
		}
	}

	// max location name width
	lwidth = 0;
	for (i = 1; i < MAX_LOCATIONS; i++) {
		p = CG_GetLocationString(CG_ConfigString(CS_LOCATIONS+i));
		if (p && *p) {
			len = font.Width(p);
			if (len > lwidth)
				lwidth = len;
		}
	}

	y = rect->y;

	for (i = 0; i < count; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {

			xx = rect->x + 1;
			for (j = 0; j <= PW_NUM_POWERUPS; j++) {
				if (ci->powerups & (1 << j)) {

					item = BG_FindItemForPowerup( (powerup_t)j );

					if (item) {
						CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, media.gfx.null /* item->icon */ );
						xx += PIC_WIDTH;
					}
				}
			}

			// FIXME: max of 3 powerups shown properly
			xx = rect->x + (PIC_WIDTH * 3) + 2;

			CG_GetColorForHealth( ci->health, ci->armor, hcolor );
			trap->R_SetColor(hcolor);
			CG_DrawPic( xx, y + 1, PIC_WIDTH - 2, PIC_WIDTH - 2, media.gfx.null );

			//Com_sprintf (st, sizeof(st), "%3i %3i", ci->health,	ci->armor);
			//font.Paint(xx, y + text_y, scale, hcolor, st, 0, 0);

			// draw weapon icon
			xx += PIC_WIDTH + 1;

			trap->R_SetColor(nullptr);
			h = CG_StatusHandle(ci->teamTask);

			if (h) {
				CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, h);
			}

			xx += PIC_WIDTH + 1;

			leftOver = rect->w - xx;
			maxx = xx + leftOver / 3;

			Text_Paint_Limit(&maxx, xx, y + text_y, scale, color, ci->name, 0, 0, FONT_MEDIUM);

			p = CG_GetLocationString(CG_ConfigString(CS_LOCATIONS+ci->location));
			if (!p || !*p) {
				p = "unknown";
			}

			xx += leftOver / 3 + 2;
			maxx = rect->w - 4;

			Text_Paint_Limit(&maxx, xx, y + text_y, scale, color, p, 0, 0, FONT_MEDIUM);
			y += text_y + 2;
			if ( y + text_y + 2 > rect->y + rect->h ) {
				break;
			}

		}
	}
}

void CG_DrawTeamSpectators(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader) {
	if (cg.spectatorLen) {
		float maxX;

		if (cg.spectatorWidth == -1) {
			cg.spectatorWidth = 0;
			cg.spectatorPaintX = rect->x + 1;
			cg.spectatorPaintX2 = -1;
		}

		if (cg.spectatorOffset > cg.spectatorLen) {
			cg.spectatorOffset = 0;
			cg.spectatorPaintX = rect->x + 1;
			cg.spectatorPaintX2 = -1;
		}

		if (cg.time > cg.spectatorTime) {
			cg.spectatorTime = cg.time + 10;
			if (cg.spectatorPaintX <= rect->x + 2) {
				if (cg.spectatorOffset < cg.spectatorLen) {
					Font font( FONT_MEDIUM, scale );
					cg.spectatorPaintX += font.Width(&cg.spectatorList[cg.spectatorOffset]) - 1;
					cg.spectatorOffset++;
				} else {
					cg.spectatorOffset = 0;
					if (cg.spectatorPaintX2 >= 0) {
						cg.spectatorPaintX = cg.spectatorPaintX2;
					} else {
						cg.spectatorPaintX = rect->x + rect->w - 2;
					}
					cg.spectatorPaintX2 = -1;
				}
			} else {
				cg.spectatorPaintX--;
				if (cg.spectatorPaintX2 >= 0) {
					cg.spectatorPaintX2--;
				}
			}
		}

		maxX = rect->x + rect->w - 2;
		Text_Paint_Limit(&maxX, cg.spectatorPaintX, rect->y + rect->h - 3, scale, color, &cg.spectatorList[cg.spectatorOffset], 0, 0, FONT_MEDIUM);
		if (cg.spectatorPaintX2 >= 0) {
			float maxX2 = rect->x + rect->w - 2;
			Text_Paint_Limit(&maxX2, cg.spectatorPaintX2, rect->y + rect->h - 3, scale, color, cg.spectatorList, 0, cg.spectatorOffset, FONT_MEDIUM);
		}
		if (cg.spectatorOffset && maxX > 0) {
			// if we have an offset ( we are skipping the first part of the string ) and we fit the string
			if (cg.spectatorPaintX2 == -1) {
				cg.spectatorPaintX2 = rect->x + rect->w - 2;
			}
		} else {
			cg.spectatorPaintX2 = -1;
		}

	}
}

void CG_DrawMedal(int ownerDraw, rectDef_t *rect, float scale, vec4_t color, qhandle_t shader) {
	score_t *score = &cg.scores[cg.selectedScore];
	float value = 0;
	char *text = nullptr;
	color[3] = 0.25;

	switch (ownerDraw) {
	case CG_ACCURACY:
		value = score->accuracy;
		break;
	case CG_ASSISTS:
		value = score->assistCount;
		break;
	case CG_DEFEND:
		value = score->defendCount;
		break;
	case CG_EXCELLENT:
		value = score->excellentCount;
		break;
	case CG_IMPRESSIVE:
		value = score->impressiveCount;
		break;
	case CG_PERFECT:
		value = score->perfect;
		break;
	case CG_GAUNTLET:
		value = score->gauntletCount;
		break;
	case CG_CAPTURES:
		value = score->captures;
		break;
	}

	if (value > 0) {
		if (ownerDraw != CG_PERFECT) {
			if (ownerDraw == CG_ACCURACY) {
				text = va("%i%%", (int)value);
				if (value > 50) {
					color[3] = 1.0;
				}
			} else {
				text = va("%i", (int)value);
				color[3] = 1.0;
			}
		} else {
			if (value) {
				color[3] = 1.0;
			}
			text = "Wow";
		}
	}

	trap->R_SetColor(color);
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );

	if (text) {
		color[3] = 1.0;
		Font font( FONT_MEDIUM, scale );
		value = font.Width(text);
		font.Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h + 10 , text, color);
	}
	trap->R_SetColor(nullptr);

}

void CG_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle,int font) {
	//Ignore all this, at least for now. May put some stat stuff back in menu files later.
	rectDef_t rect;

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	if (ownerDrawFlags != 0 && !CG_OwnerDrawVisible(ownerDrawFlags)) {
		return;
	}

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	switch ( ownerDraw ) {

	default: {
		// ...
	} break;

	}
}

void CG_MouseEvent(int x, int y) {
	int n;

	/*
	if ( (cg.predictedPlayerState.pm_type == PM_NORMAL || cg.predictedPlayerState.pm_type == PM_JETPACK || cg.predictedPlayerState.pm_type == PM_FLOAT || cg.predictedPlayerState.pm_type == PM_SPECTATOR) && cg.showScores == false) {
		trap->Key_SetCatcher(0);
		return;
	}
	*/

	cgs.cursorX+= x;
	if (cgs.cursorX < 0)
		cgs.cursorX = 0;
	else if (cgs.cursorX > 640)
		cgs.cursorX = 640;

	cgs.cursorY += y;
	if (cgs.cursorY < 0)
		cgs.cursorY = 0;
	else if (cgs.cursorY > 480)
		cgs.cursorY = 480;

	n = Display_CursorType(cgs.cursorX, cgs.cursorY);
	cgs.activeCursor = 0;
	if (n == CURSOR_ARROW) {
		cgs.activeCursor = media.gfx.null;
	} else if (n == CURSOR_SIZER) {
		cgs.activeCursor = media.gfx.null;
	}

	if (cgs.capturedItem) {
		Display_MouseMove(cgs.capturedItem, x, y);
	} else {
		Display_MouseMove(nullptr, cgs.cursorX, cgs.cursorY);
	}

}

void CG_HideTeamMenu() {
	Menus_CloseByName("teamMenu");
	Menus_CloseByName("getMenu");
}

void CG_ShowTeamMenu() {
	Menus_OpenByName("teamMenu");
}

// type:
//	0 - no event handling
//	1 - team menu
//	2 - hud editor
void CG_EventHandling(int type) {
	cgs.eventHandling = type;
	if (type == CGAME_EVENT_NONE) {
		CG_HideTeamMenu();
	} else if (type == CGAME_EVENT_TEAMMENU) {
		//CG_ShowTeamMenu();
	} else if (type == CGAME_EVENT_SCOREBOARD) {
	}

}

void CG_KeyEvent(int key, bool down) {

	if (!down) {
		return;
	}

	if ( cg.predictedPlayerState.pm_type == PM_NORMAL || cg.predictedPlayerState.pm_type == PM_JETPACK || cg.predictedPlayerState.pm_type == PM_NORMAL || (cg.predictedPlayerState.pm_type == PM_SPECTATOR && cg.showScores == false)) {
		CG_EventHandling(CGAME_EVENT_NONE);
		trap->Key_SetCatcher(0);
		return;
	}

	//if (key == trap->Key_GetKey("teamMenu") || !Display_CaptureItem(cgs.cursorX, cgs.cursorY)) {
	// if we see this then we should always be visible
	//  CG_EventHandling(CGAME_EVENT_NONE);
	//  trap->Key_SetCatcher(0);
	//}

	Display_HandleKey(key, down, cgs.cursorX, cgs.cursorY);

	if (cgs.capturedItem) {
		cgs.capturedItem = nullptr;
	}	else {
		if (key == A_MOUSE2 && down) {
			cgs.capturedItem = Display_CaptureItem(cgs.cursorX, cgs.cursorY);
		}
	}
}

int CG_ClientNumFromName(const char *p) {
	int i;
	for (i = 0; i < cgs.maxclients; i++) {
		if (cgs.clientinfo[i].infoValid && Q_stricmp(cgs.clientinfo[i].name, p) == 0) {
			return i;
		}
	}
	return -1;
}

void CG_ShowResponseHead(void) {
	Menus_OpenByName("voiceMenu");
	trap->Cvar_Set("cl_conXOffset", "72");
	cg.voiceTime = cg.time;
}

void CG_RunMenuScript(char **args) {
}

bool CG_DeferMenuScript (char **args)
{
	return false;
}

void CG_GetTeamColor(vec4_t *color) {
	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
		(*color)[0] = 1.0f;
		(*color)[3] = 0.25f;
		(*color)[1] = (*color)[2] = 0.0f;
	} else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
		(*color)[0] = (*color)[1] = 0.0f;
		(*color)[2] = 1.0f;
		(*color)[3] = 0.25f;
	} else {
		(*color)[0] = (*color)[2] = 0.0f;
		(*color)[1] = 0.17f;
		(*color)[3] = 0.25f;
	}
}
