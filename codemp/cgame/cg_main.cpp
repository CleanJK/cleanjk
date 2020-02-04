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

// cg_main.c -- initialization and primary entry point for cgame
#include "cgame/cg_local.h"

#include "ui/ui_shared.h"
#include "ui/ui_fonts.h"
#include "ui/menudef.h"
#include "cgame/cg_media.h"

NORETURN_PTR void (*Com_Error)( int level, const char *fmt, ... );
void (*Com_Printf)( const char *fmt, ... );

// display context for new ui stuff
displayContextDef_t cgDC;

static int	C_PointContents(void);
static void C_GetLerpOrigin(void);
static void C_GetLerpData(void);
static void C_Trace(void);
static void C_G2Trace(void);
static void C_G2Mark(void);
static int	CG_RagCallback(int callType);

extern autoMapInput_t cg_autoMapInput; //cg_view.c
extern int cg_autoMapInputTime;
extern vec3_t cg_autoMapAngle;

//do we have any force powers that we would normally need to cycle to?
bool CG_NoUseableForce(void)
{
	int i = FP_HEAL;
	while (i < NUM_FORCE_POWERS)
	{
		if (i != FP_SABERTHROW &&
			i != FP_SABER_OFFENSE &&
			i != FP_SABER_DEFENSE &&
			i != FP_LEVITATION)
		{ //valid selectable power
			if (cg.predictedPlayerState.fd.forcePowersKnown & (1 << i))
			{ //we have it
				return false;
			}
		}
		i++;
	}

	//no useable force powers, I guess.
	return true;
}

static int C_PointContents( void ) {
	TCGPointContents *data = &cg.sharedBuffer.pointContents;
	return CG_PointContents( data->mPoint, data->mPassEntityNum );
}

static void C_GetLerpOrigin( void ) {
	TCGVectorData *data = &cg.sharedBuffer.vectorData;
	VectorCopy( cg_entities[data->mEntityNum].lerpOrigin, data->mPoint );
}

// only used by FX system to pass to getboltmat
static void C_GetLerpData( void ) {
	TCGGetBoltData *data = &cg.sharedBuffer.getBoltData;

	VectorCopy( cg_entities[data->mEntityNum].lerpOrigin, data->mOrigin );
	VectorCopy( cg_entities[data->mEntityNum].modelScale, data->mScale );
	VectorCopy( cg_entities[data->mEntityNum].lerpAngles, data->mAngles );
	if ( cg_entities[data->mEntityNum].currentState.eType == ET_PLAYER ) {
		// normal player
		data->mAngles[PITCH] = 0.0f;
		data->mAngles[ROLL] = 0.0f;
	}
}

static void C_Trace( void ) {
	TCGTrace *td = &cg.sharedBuffer.trace;
	CG_Trace( &td->mResult, td->mStart, td->mMins, td->mMaxs, td->mEnd, td->mSkipNumber, td->mMask );
}

static void C_G2Trace( void ) {
	TCGTrace *td = &cg.sharedBuffer.trace;
	CG_G2Trace( &td->mResult, td->mStart, td->mMins, td->mMaxs, td->mEnd, td->mSkipNumber, td->mMask );
}

static void C_G2Mark( void ) {
	TCGG2Mark *td = &cg.sharedBuffer.g2Mark;
	trace_t tr;
	vec3_t end;

	VectorMA( td->start, 64.0f, td->dir, end );
	CG_G2Trace( &tr, td->start, nullptr, nullptr, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );

	if ( tr.entityNum < ENTITYNUM_WORLD && cg_entities[tr.entityNum].ghoul2 ) {
		// hit someone with a ghoul2 instance, let's project the decal on them then.
		centity_t *cent = &cg_entities[tr.entityNum];

	//	CG_TestLine( tr.endpos, end, 2000, 0x0000ff, 1 );

		CG_AddGhoul2Mark( td->shader, td->size, tr.endpos, end, tr.entityNum, cent->lerpOrigin, cent->lerpAngles[YAW],
			cent->ghoul2, cent->modelScale, Q_irand( 2000, 4000 ) );
		// I'm making fx system decals have a very short lifetime.
	}
}

static void CG_DebugBoxLines( vec3_t mins, vec3_t maxs, int duration ) {
	vec3_t start, end, vert;
	float x = maxs[0] - mins[0];
	float y = maxs[1] - mins[1];

	start[2] = maxs[2];
	vert[2] = mins[2];

	vert[0] = mins[0];
	vert[1] = mins[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	vert[0] = mins[0];
	vert[1] = maxs[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	vert[0] = maxs[0];
	vert[1] = mins[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	vert[0] = maxs[0];
	vert[1] = maxs[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	// top of box
	VectorCopy(maxs, start);
	VectorCopy(maxs, end);
	start[0] -= x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	end[0] = start[0];
	end[1] -= y;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	start[1] = end[1];
	start[0] += x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	CG_TestLine(start, maxs, duration, 0x00000ff, 1);
	// bottom of box
	VectorCopy(mins, start);
	VectorCopy(mins, end);
	start[0] += x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	end[0] = start[0];
	end[1] += y;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	start[1] = end[1];
	start[0] -= x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	CG_TestLine(start, mins, duration, 0x00000ff, 1);
}

//handle ragdoll callbacks, for events and debugging -rww
static int CG_RagCallback(int callType)
{
	switch(callType)
	{
	case RAG_CALLBACK_DEBUGBOX:
		{
			ragCallbackDebugBox_t *callData = &cg.sharedBuffer.rcbDebugBox;

			CG_DebugBoxLines(callData->mins, callData->maxs, callData->duration);
		}
		break;
	case RAG_CALLBACK_DEBUGLINE:
		{
			ragCallbackDebugLine_t *callData = &cg.sharedBuffer.rcbDebugLine;

			CG_TestLine(callData->start, callData->end, callData->time, callData->color, callData->radius);
		}
		break;
	case RAG_CALLBACK_BONESNAP:
		{
			ragCallbackBoneSnap_t *callData = &cg.sharedBuffer.rcbBoneSnap;
			centity_t *cent = &cg_entities[callData->entNum];
			int snapSound = trap->S_RegisterSound(va("sound/player/bodyfall_human%i.wav", Q_irand(1, 3)));

			trap->S_StartSound(cent->lerpOrigin, callData->entNum, CHAN_AUTO, snapSound);
		}
	case RAG_CALLBACK_BONEIMPACT:
		break;
	case RAG_CALLBACK_BONEINSOLID:
		break;
	case RAG_CALLBACK_TRACELINE:
		{
			ragCallbackTraceLine_t *callData = &cg.sharedBuffer.rcbTraceLine;

			CG_Trace(&callData->tr, callData->start, callData->mins, callData->maxs,
				callData->end, callData->ignore, callData->mask);
		}
		break;
	default:
		Com_Error(ERR_DROP, "Invalid callType in CG_RagCallback");
		break;
	}

	return 0;
}

void CG_MiscEnt( void ) {
	int i, modelIndex;
	TCGMiscEnt *data = &cg.sharedBuffer.miscEnt;
	cg_staticmodel_t *staticmodel;

	if( cgs.numMiscStaticModels >= MAX_STATIC_MODELS ) {
		trap->Error( ERR_DROP, "^1MAX_STATIC_MODELS(%i) hit", MAX_STATIC_MODELS );
	}

	modelIndex = trap->R_RegisterModel(data->mModel);
	if (modelIndex == 0) {
		trap->Error( ERR_DROP, "client_model failed to load model '%s'", data->mModel );
		return;
	}

	staticmodel = &cgs.miscStaticModels[cgs.numMiscStaticModels++];
	staticmodel->model = modelIndex;
	AnglesToAxis( data->mAngles, staticmodel->axes );
	for ( i = 0; i < 3; i++ ) {
		VectorScale( staticmodel->axes[i], data->mScale[i], staticmodel->axes[i] );
	}

	VectorCopy( data->mOrigin, staticmodel->org );
	staticmodel->zoffset = 0.f;

	if( staticmodel->model ) {
		vec3_t mins, maxs;

		trap->R_ModelBounds( staticmodel->model, mins, maxs );

		VectorScaleVector(mins, data->mScale, mins);
		VectorScaleVector(maxs, data->mScale, maxs);

		staticmodel->radius = RadiusFromBounds( mins, maxs );
	} else {
		staticmodel->radius = 0;
	}
}

/*
void CG_ResizeG2Bolt(boltInfo_v *bolt, int newCount)
{
	bolt->resize(newCount);
}

void CG_ResizeG2Surface(surfaceInfo_v *surface, int newCount)
{
	surface->resize(newCount);
}

void CG_ResizeG2Bone(boneInfo_v *bone, int newCount)
{
	bone->resize(newCount);
}

void CG_ResizeG2(CGhoul2Info_v *ghoul2, int newCount)
{
	ghoul2->resize(newCount);
}

void CG_ResizeG2TempBone(mdxaBone_v *tempBone, int newCount)
{
	tempBone->resize(newCount);
}
*/
cg_t				cg;
cgs_t				cgs;
centity_t			cg_entities[MAX_GENTITIES];

centity_t			*cg_permanents[MAX_GENTITIES]; //rwwRMG - added
int					cg_numpermanents = 0;

weaponInfo_t		cg_weapons[MAX_WEAPONS];
itemInfo_t			cg_items[MAX_ITEMS];

int CG_CrosshairPlayer( void ) {
	if ( cg.time > (cg.crosshairClientTime + 1000) )
		return -1;

	if ( cg.crosshairClientNum >= MAX_CLIENTS )
		return -1;

	return cg.crosshairClientNum;
}

int CG_LastAttacker( void ) {
	if ( !cg.attackerTime )
		return -1;

	return cg.snap->ps.persistant[PERS_ATTACKER];
}

const char *CG_Argv( int arg ) {
	static char	buffer[MAX_STRING_CHARS] = {0};

	trap->Cmd_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}

// so shared code can get the local time depending on the side it's executed on
int BG_GetTime( void ) {
	return cg.time;
}

extern char *forceHolocronModels[];

const char *CG_GetStringEdString(char *refSection, const char *refName)
{
	static char text[2][1024];	//just incase it's nested
	static int		index = 0;

	index ^= 1;
	trap->SE_GetStringTextString(va("%s_%s", refSection, refName), text[index], sizeof(text[0]));
	return text[index];
}

void CG_BuildSpectatorString(void) {
	int i;
	cg.spectatorList[0] = 0;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_SPECTATOR ) {
			Q_strcat(cg.spectatorList, sizeof(cg.spectatorList), va("%s     ", cgs.clientinfo[i].name));
		}
	}
	i = strlen(cg.spectatorList);
	if (i != cg.spectatorLen) {
		cg.spectatorLen = i;
		cg.spectatorWidth = -1;
	}
}

static void CG_RegisterClients( void ) {
	int		i;

	CG_LoadingClient(cg.clientNum);
	CG_NewClientInfo(cg.clientNum, false);

	for (i=0 ; i<MAX_CLIENTS ; i++) {
		const char		*clientInfo;

		if (cg.clientNum == i) {
			continue;
		}

		clientInfo = CG_ConfigString( CS_PLAYERS+i );
		if ( !clientInfo[0]) {
			continue;
		}
		CG_LoadingClient( i );
		CG_NewClientInfo( i, false);
	}
	CG_BuildSpectatorString();
}

const char *CG_ConfigString( int index ) {
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		trap->Error( ERR_DROP, "CG_ConfigString: bad index: %i", index );
	}
	return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

void CG_StartMusic( bool bForceStart ) {
	char	*s;
	char	parm1[MAX_QPATH], parm2[MAX_QPATH];

	// start the background music
	s = (char *)CG_ConfigString( CS_MUSIC );
	Q_strncpyz( parm1, COM_Parse( (const char **)&s ), sizeof( parm1 ) );
	Q_strncpyz( parm2, COM_Parse( (const char **)&s ), sizeof( parm2 ) );

	trap->S_StartBackgroundTrack( parm1, parm2, !bForceStart );
}

char *CG_GetMenuBuffer(const char *filename) {
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUFILE];

	len = trap->FS_Open( filename, &f, FS_READ );
	if ( !f ) {
		trap->Print( S_COLOR_RED "menu file not found: %s, using default\n", filename );
		return nullptr;
	}
	if ( len >= MAX_MENUFILE ) {
		trap->Print( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", filename, len, MAX_MENUFILE );
		trap->FS_Close( f );
		return nullptr;
	}

	trap->FS_Read( buf, len, f );
	buf[len] = 0;
	trap->FS_Close( f );

	return buf;
}

// new hud stuff ( mission pack )
bool CG_Asset_Parse(int handle) {
	pc_token_t token;

	if (!trap->PC_ReadToken(handle, &token))
		return false;
	if (Q_stricmp(token.string, "{") != 0) {
		return false;
	}

	while ( 1 ) {
		if (!trap->PC_ReadToken(handle, &token))
			return false;

		if (Q_stricmp(token.string, "}") == 0) {
			return true;
		}

		// font
		if (Q_stricmp(token.string, "font") == 0) {
			int pointSize;
			if (!trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return false;
			}

//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.textFont);
			cgDC.Assets.qhMediumFont = cgDC.RegisterFont(token.string);
			continue;
		}

		// smallFont
		if (Q_stricmp(token.string, "smallFont") == 0) {
			int pointSize;
			if (!trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return false;
			}
//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.smallFont);
			cgDC.Assets.qhSmallFont = cgDC.RegisterFont(token.string);
			continue;
		}

		// smallFont
		if (Q_stricmp(token.string, "small2Font") == 0) {
			int pointSize;
			if (!trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return false;
			}
//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.smallFont);
			cgDC.Assets.qhSmall2Font = cgDC.RegisterFont(token.string);
			continue;
		}

		// font
		if (Q_stricmp(token.string, "bigfont") == 0) {
			int pointSize;
			if (!trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return false;
			}
//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.bigFont);
			cgDC.Assets.qhBigFont = cgDC.RegisterFont(token.string);
			continue;
		}

		// gradientbar
		if (Q_stricmp(token.string, "gradientbar") == 0) {
			if (!trap->PC_ReadToken(handle, &token)) {
				return false;
			}
			cgDC.Assets.gradientBar = trap->R_RegisterShaderNoMip(token.string);
			continue;
		}

		// enterMenuSound
		if (Q_stricmp(token.string, "menuEnterSound") == 0) {
			if (!trap->PC_ReadToken(handle, &token)) {
				return false;
			}
			cgDC.Assets.menuEnterSound = trap->S_RegisterSound( token.string );
			continue;
		}

		// exitMenuSound
		if (Q_stricmp(token.string, "menuExitSound") == 0) {
			if (!trap->PC_ReadToken(handle, &token)) {
				return false;
			}
			cgDC.Assets.menuExitSound = trap->S_RegisterSound( token.string );
			continue;
		}

		// itemFocusSound
		if (Q_stricmp(token.string, "itemFocusSound") == 0) {
			if (!trap->PC_ReadToken(handle, &token)) {
				return false;
			}
			cgDC.Assets.itemFocusSound = trap->S_RegisterSound( token.string );
			continue;
		}

		// menuBuzzSound
		if (Q_stricmp(token.string, "menuBuzzSound") == 0) {
			if (!trap->PC_ReadToken(handle, &token)) {
				return false;
			}
			cgDC.Assets.menuBuzzSound = trap->S_RegisterSound( token.string );
			continue;
		}

		if (Q_stricmp(token.string, "cursor") == 0) {
			if (!PC_String_Parse(handle, &cgDC.Assets.cursorStr)) {
				return false;
			}
			cgDC.Assets.cursor = trap->R_RegisterShaderNoMip( cgDC.Assets.cursorStr);
			continue;
		}

		if (Q_stricmp(token.string, "fadeClamp") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.fadeClamp)) {
				return false;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeCycle") == 0) {
			if (!PC_Int_Parse(handle, &cgDC.Assets.fadeCycle)) {
				return false;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeAmount") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.fadeAmount)) {
				return false;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowX") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.shadowX)) {
				return false;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowY") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.shadowY)) {
				return false;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowColor") == 0) {
			if (!PC_Color_Parse(handle, &cgDC.Assets.shadowColor)) {
				return false;
			}
			cgDC.Assets.shadowFadeClamp = cgDC.Assets.shadowColor[3];
			continue;
		}
	}
	return false; // bk001204 - why not?
}

void CG_ParseMenu(const char *menuFile) {
	pc_token_t token;
	int handle;

	handle = trap->PC_LoadSource(menuFile);
	if (!handle)
		handle = trap->PC_LoadSource("ui/testhud.menu");
	if (!handle)
		return;

	while ( 1 ) {
		if (!trap->PC_ReadToken( handle, &token )) {
			break;
		}

		//if ( Q_stricmp( token, "{" ) ) {
		//	Com_Printf( "Missing { in menu file\n" );
		//	break;
		//}

		//if ( menuCount == MAX_MENUS ) {
		//	Com_Printf( "Too many menus!\n" );
		//	break;
		//}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "assetGlobalDef") == 0) {
			if (CG_Asset_Parse(handle)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token.string, "menudef") == 0) {
			// start a new menu
			Menu_New(handle);
		}
	}
	trap->PC_FreeSource(handle);
}

bool CG_Load_Menu(const char **p)
{

	char *token;

	token = COM_ParseExt((const char **)p, true);

	if (token[0] != '{') {
		return false;
	}

	while ( 1 ) {

		token = COM_ParseExt((const char **)p, true);

		if (Q_stricmp(token, "}") == 0) {
			return true;
		}

		if ( !token || token[0] == 0 ) {
			return false;
		}

		CG_ParseMenu(token);
	}
	return false;
}

static bool CG_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key) {
	return false;
}

static int CG_FeederCount(float feederID) {
	int i, count;
	count = 0;
	if (feederID == FEEDER_REDTEAM_LIST) {
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == TEAM_RED) {
				count++;
			}
		}
	} else if (feederID == FEEDER_BLUETEAM_LIST) {
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == TEAM_BLUE) {
				count++;
			}
		}
	} else if (feederID == FEEDER_SCOREBOARD) {
		return cg.numScores;
	}
	return count;
}

void CG_SetScoreSelection(void *p) {
	menuDef_t *menu = (menuDef_t*)p;
	playerState_t *ps = &cg.snap->ps;
	int i, red, blue;
	red = blue = 0;
	for (i = 0; i < cg.numScores; i++) {
		if (cg.scores[i].team == TEAM_RED) {
			red++;
		} else if (cg.scores[i].team == TEAM_BLUE) {
			blue++;
		}
		if (ps->clientNum == cg.scores[i].client) {
			cg.selectedScore = i;
		}
	}

	if (menu == nullptr) {
		// just interested in setting the selected score
		return;
	}

	if ( cgs.gametype >= GT_TEAM ) {
		int feeder = FEEDER_REDTEAM_LIST;
		i = red;
		if (cg.scores[cg.selectedScore].team == TEAM_BLUE) {
			feeder = FEEDER_BLUETEAM_LIST;
			i = blue;
		}
		Menu_SetFeederSelection(menu, feeder, i, nullptr);
	} else {
		Menu_SetFeederSelection(menu, FEEDER_SCOREBOARD, cg.selectedScore, nullptr);
	}
}

// FIXME: might need to cache this info
static clientInfo_t * CG_InfoFromScoreIndex(int index, int team, int *scoreIndex) {
	int i, count;
	if ( cgs.gametype >= GT_TEAM ) {
		count = 0;
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == team) {
				if (count == index) {
					*scoreIndex = i;
					return &cgs.clientinfo[cg.scores[i].client];
				}
				count++;
			}
		}
	}
	*scoreIndex = index;
	return &cgs.clientinfo[ cg.scores[index].client ];
}

static const char *CG_FeederItemText(float feederID, int index, int column,
									 qhandle_t *handle1, qhandle_t *handle2, qhandle_t *handle3) {
	gitem_t *item;
	int scoreIndex = 0;
	clientInfo_t *info = nullptr;
	int team = -1;
	score_t *sp = nullptr;

	*handle1 = *handle2 = *handle3 = -1;

	if (feederID == FEEDER_REDTEAM_LIST) {
		team = TEAM_RED;
	} else if (feederID == FEEDER_BLUETEAM_LIST) {
		team = TEAM_BLUE;
	}

	info = CG_InfoFromScoreIndex(index, team, &scoreIndex);
	sp = &cg.scores[scoreIndex];

	if (info && info->infoValid) {
		switch (column) {
			case 0:
				if ( info->powerups & ( 1 << PW_NEUTRALFLAG ) ) {
					item = BG_FindItemForPowerup( PW_NEUTRALFLAG );
					*handle1 = cg_items[ ITEM_INDEX(item) ].icon;
				} else if ( info->powerups & ( 1 << PW_REDFLAG ) ) {
					item = BG_FindItemForPowerup( PW_REDFLAG );
					*handle1 = cg_items[ ITEM_INDEX(item) ].icon;
				} else if ( info->powerups & ( 1 << PW_BLUEFLAG ) ) {
					item = BG_FindItemForPowerup( PW_BLUEFLAG );
					*handle1 = cg_items[ ITEM_INDEX(item) ].icon;
				}
			break;
			case 1:
				if (team == -1) {
					return "";
				} else {
					*handle1 = CG_StatusHandle(info->teamTask);
				}
		  break;
			case 2:
				if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << sp->client ) ) {
					return "Ready";
				}
				if (team == -1) {
					if (cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL) {
						return va("%i/%i", info->wins, info->losses);
					} else if (info->infoValid && info->team == TEAM_SPECTATOR ) {
						return "Spectator";
					} else {
						return "";
					}
				} else {
					if (info->teamLeader) {
						return "Leader";
					}
				}
			break;
			case 3:
				return info->name;
			break;
			case 4:
				return va("%i", info->score);
			break;
			case 5:
				return va("%4i", sp->time);
			break;
			case 6:
				if ( sp->ping == -1 ) {
					return "connecting";
				}
				return va("%4i", sp->ping);
			break;
		}
	}

	return "";
}

static qhandle_t CG_FeederItemImage(float feederID, int index) {
	return 0;
}

static bool CG_FeederSelection(float feederID, int index, itemDef_t *item) {
	if ( cgs.gametype >= GT_TEAM ) {
		int i, count;
		int team = (feederID == FEEDER_REDTEAM_LIST) ? TEAM_RED : TEAM_BLUE;
		count = 0;
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == team) {
				if (index == count) {
					cg.selectedScore = i;
				}
				count++;
			}
		}
	} else {
		cg.selectedScore = index;
	}

	return true;
}

static float CG_Cvar_Get(const char *cvar) {
	char buff[MAX_CVAR_VALUE_STRING];
	memset(buff, 0, sizeof(buff));
	trap->Cvar_VariableStringBuffer(cvar, buff, sizeof(buff));
	return atof(buff);
}

static int CG_OwnerDrawWidth(int ownerDraw, float scale) {
	//FIXME: we don't know the actual font...
	Font font( FONT_MEDIUM, scale );
	switch (ownerDraw) {
	  case CG_GAME_TYPE:
			return font.Width(BG_GetGametypeString( cgs.gametype ));
	  case CG_GAME_STATUS:
			return font.Width(CG_GetGameStatusText());
			break;
	  case CG_KILLER:
			return font.Width(CG_GetKillerText());
			break;
	  case CG_RED_NAME:
			return font.Width(DEFAULT_REDTEAM_NAME/*cg_redTeamName.string*/);
			break;
	  case CG_BLUE_NAME:
			return font.Width(DEFAULT_BLUETEAM_NAME/*cg_blueTeamName.string*/);
			break;
	}
	return 0;
}

static int CG_PlayCinematic(const char *name, float x, float y, float w, float h) {
	return trap->CIN_PlayCinematic(name, x, y, w, h, CIN_loop);
}

static void CG_StopCinematic(int handle) {
	trap->CIN_StopCinematic(handle);
}

static void CG_DrawCinematic(int handle, float x, float y, float w, float h) {
	trap->CIN_SetExtents(handle, x, y, w, h);
	trap->CIN_DrawCinematic(handle);
}

static void CG_RunCinematicFrame(int handle) {
	trap->CIN_RunCinematic(handle);
}

void CG_LoadMenus(const char *menuFile)
{
	const char	*token;
	const char	*p;
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUDEFFILE];

	len = trap->FS_Open( menuFile, &f, FS_READ );

	if ( !f )
	{
		if( Q_isanumber( menuFile ) ) // cg_hudFiles 1
			trap->Print( S_COLOR_GREEN "hud menu file skipped, using default\n" );
		else
			trap->Print( S_COLOR_YELLOW "hud menu file not found: %s, using default\n", menuFile );

		len = trap->FS_Open( DEFAULT_HUDSET, &f, FS_READ );
		if (!f)
		{
			trap->Error( ERR_DROP, S_COLOR_RED "default hud menu file not found: " DEFAULT_HUDSET ", unable to continue!" );
		}
	}

	if ( len >= MAX_MENUDEFFILE )
	{
		trap->FS_Close( f );
		trap->Error( ERR_DROP, S_COLOR_RED "menu file too large: %s is %i, max allowed is %i", menuFile, len, MAX_MENUDEFFILE );
	}

	trap->FS_Read( buf, len, f );
	buf[len] = 0;
	trap->FS_Close( f );

	p = buf;

	COM_BeginParseSession ("CG_LoadMenus");
	while ( 1 )
	{
		token = COM_ParseExt( &p, true );
		if( !token || token[0] == 0 || token[0] == '}')
		{
			break;
		}

		if ( Q_stricmp( token, "}" ) == 0 )
		{
			break;
		}

		if (Q_stricmp(token, "loadmenu") == 0)
		{
			if (CG_Load_Menu(&p))
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	//Com_Printf("UI menu load time = %d milli seconds\n", cgi_Milliseconds() - start);
}

void CG_LoadHudMenu()
{
	const char *hudSet;

	cgDC.registerShaderNoMip			= trap->R_RegisterShaderNoMip;
	cgDC.setColor						= trap->R_SetColor;
	cgDC.drawHandlePic					= &CG_DrawPic;
	cgDC.drawStretchPic					= trap->R_DrawStretchPic;
	cgDC.registerModel					= trap->R_RegisterModel;
	cgDC.modelBounds					= trap->R_ModelBounds;
	cgDC.fillRect						= &CG_FillRect;
	cgDC.drawRect						= &CG_DrawRect;
	cgDC.drawSides						= &CG_DrawSides;
	cgDC.drawTopBottom					= &CG_DrawTopBottom;
	cgDC.clearScene						= trap->R_ClearScene;
	cgDC.addRefEntityToScene			= trap->R_AddRefEntityToScene;
	cgDC.renderScene					= trap->R_RenderScene;
	cgDC.RegisterFont					= trap->R_RegisterFont;
	cgDC.Font_StrLenPixels				= trap->R_Font_StrLenPixels;
	cgDC.Font_StrLenChars				= trap->R_Font_StrLenChars;
	cgDC.Font_HeightPixels				= trap->R_Font_HeightPixels;
	cgDC.Font_DrawString				= trap->R_Font_DrawString;
	cgDC.Language_IsAsian				= trap->R_Language_IsAsian;
	cgDC.Language_UsesSpaces			= trap->R_Language_UsesSpaces;
	cgDC.AnyLanguage_ReadCharFromString	= trap->R_AnyLanguage_ReadCharFromString;
	cgDC.ownerDrawItem					= &CG_OwnerDraw;
	cgDC.getValue						= &CG_GetValue;
	cgDC.ownerDrawVisible				= &CG_OwnerDrawVisible;
	cgDC.runScript						= &CG_RunMenuScript;
	cgDC.deferScript					= &CG_DeferMenuScript;
	cgDC.getTeamColor					= &CG_GetTeamColor;
	cgDC.setCVar						= trap->Cvar_Set;
	cgDC.getCVarString					= trap->Cvar_VariableStringBuffer;
	cgDC.getCVarValue					= CG_Cvar_Get;
	//cgDC.setOverstrikeMode			= &trap->Key_SetOverstrikeMode;
	//cgDC.getOverstrikeMode			= &trap->Key_GetOverstrikeMode;
	cgDC.startLocalSound				= trap->S_StartLocalSound;
	cgDC.ownerDrawHandleKey				= &CG_OwnerDrawHandleKey;
	cgDC.feederCount					= &CG_FeederCount;
	cgDC.feederItemImage				= &CG_FeederItemImage;
	cgDC.feederItemText					= &CG_FeederItemText;
	cgDC.feederSelection				= &CG_FeederSelection;
	//cgDC.setBinding					= &trap->Key_SetBinding;
	//cgDC.getBindingBuf				= &trap->Key_GetBindingBuf;
	//cgDC.keynumToStringBuf			= &trap->Key_KeynumToStringBuf;
	//cgDC.executeText					= &trap->Cmd_ExecuteText;
	cgDC.Error							= Com_Error;
	cgDC.Print							= Com_Printf;
	cgDC.ownerDrawWidth					= &CG_OwnerDrawWidth;
	//cgDC.Pause						= &CG_Pause;
	cgDC.registerSound					= trap->S_RegisterSound;
	cgDC.startBackgroundTrack			= trap->S_StartBackgroundTrack;
	cgDC.stopBackgroundTrack			= trap->S_StopBackgroundTrack;
	cgDC.playCinematic					= &CG_PlayCinematic;
	cgDC.stopCinematic					= &CG_StopCinematic;
	cgDC.drawCinematic					= &CG_DrawCinematic;
	cgDC.runCinematicFrame				= &CG_RunCinematicFrame;
	cgDC.ext.Font_StrLenPixels			= trap->ext.R_Font_StrLenPixels;

	Init_Display(&cgDC);

	Menu_Reset();

	hudSet = cg_hudFiles.string;
	if ( hudSet[0] == '\0' ) {
		hudSet = DEFAULT_HUDSET;
	}

	CG_LoadMenus(hudSet);

}

void CG_AssetCache() {
	//if (Assets.textFont == nullptr) {
	//  trap->R_RegisterFont("fonts/arial.ttf", 72, &Assets.textFont);
	//}
	//Com_Printf("Menu Size: %i bytes\n", sizeof(Menus));
	cgDC.Assets.gradientBar = trap->R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
	cgDC.Assets.fxBasePic = trap->R_RegisterShaderNoMip( ART_FX_BASE );
	cgDC.Assets.fxPic[0] = trap->R_RegisterShaderNoMip( ART_FX_RED );
	cgDC.Assets.fxPic[1] = trap->R_RegisterShaderNoMip( ART_FX_YELLOW );
	cgDC.Assets.fxPic[2] = trap->R_RegisterShaderNoMip( ART_FX_GREEN );
	cgDC.Assets.fxPic[3] = trap->R_RegisterShaderNoMip( ART_FX_TEAL );
	cgDC.Assets.fxPic[4] = trap->R_RegisterShaderNoMip( ART_FX_BLUE );
	cgDC.Assets.fxPic[5] = trap->R_RegisterShaderNoMip( ART_FX_CYAN );
	cgDC.Assets.fxPic[6] = trap->R_RegisterShaderNoMip( ART_FX_WHITE );
	cgDC.Assets.scrollBar = trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	cgDC.Assets.scrollBarArrowDown = trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
	cgDC.Assets.scrollBarArrowUp = trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
	cgDC.Assets.scrollBarArrowLeft = trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
	cgDC.Assets.scrollBarArrowRight = trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
	cgDC.Assets.scrollBarThumb = trap->R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
	cgDC.Assets.sliderBar = trap->R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
	cgDC.Assets.sliderThumb = trap->R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );
}

// initialise the cg_entities structure - take into account the ghoul2 stl stuff in the active snap shots
void CG_Init_CG(void)
{
	memset( &cg, 0, sizeof(cg));
}

// initialise the cg_entities structure - take into account the ghoul2 stl stuff
void CG_Init_CGents(void)
{
	memset(&cg_entities, 0, sizeof(cg_entities));
}

void CG_TransitionPermanent(void)
{
	centity_t	*cent = cg_entities;
	int			i;

	cg_numpermanents = 0;
	for(i=0;i<MAX_GENTITIES;i++,cent++)
	{
		if (trap->GetDefaultState(i, &cent->currentState))
		{
			cent->nextState = cent->currentState;
			VectorCopy (cent->currentState.origin, cent->lerpOrigin);
			VectorCopy (cent->currentState.angles, cent->lerpAngles);
			cent->currentValid = true;

			cg_permanents[cg_numpermanents++] = cent;
		}
	}
}

extern playerState_t *cgSendPS[MAX_GENTITIES]; //is not MAX_CLIENTS because NPCs exceed MAX_CLIENTS

// Called after every level change or subsystem restart
// Will perform callbacks to make the loading info screen update.
void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum )
{
	const char	*s;

	// initialisation
	CG_LoadingString( "module initialisation" );
	memset( cg_entities, 0, sizeof(cg_entities) );
	memset( &cg, 0, sizeof(cg) );
	memset( &cgs, 0, sizeof( cgs ) );
	memset( cg_items, 0, sizeof(cg_items) );
	memset( cg_weapons, 0, sizeof(cg_weapons) );

	trap->RegisterSharedMemory( cg.sharedBuffer.raw );

	CG_LoadingString( "commands & cvars" );
	CG_RegisterCvars();
	CG_InitConsoleCommands();

	CG_LoadingString( "Ghoul2 setup" );
	BG_InitAnimsets();
	CG_InitJetpackGhoul2();
	CG_PmoveClientPointerUpdate();


	// === DO NOT LOAD MEDIA BEFORE THIS POINT ===
	CG_PreloadMedia();

	cg.clientNum = clientNum;
	cgs.processedSnapshotNum = serverMessageNum;
	cgs.serverCommandSequence = serverCommandSequence;
	cg.itemSelect = -1;
	cg.forceSelect = 0xFFFFFFFFu;
	cg.forceHUDActive = true;
	cg.forceHUDTotalFlashTime = 0;
	cg.forceHUDNextFlashTime = 0;
	cg.renderingThirdPerson = cg_thirdPerson.integer;
	cg.weaponSelect = WP_BRYAR_PISTOL;
	cgs.redflag = cgs.blueflag = FLAG_ATBASE; //-1
	cgs.flagStatus = -1;
	//cg.demoPlayback = demoPlayback;
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );

	// get the rendering configuration from the client system
	trap->GetGlconfig( &cgs.glconfig );
	cgs.screenXScale = cgs.glconfig.vidWidth / SCREEN_WIDTH;
	cgs.screenYScale = cgs.glconfig.vidHeight / SCREEN_HEIGHT;

	// get the gamestate from the client system
	trap->GetGameState( &cgs.gameState );
	s = CG_ConfigString( CS_GAME_VERSION );
	if ( strcmp( s, GAME_VERSION ) ) {
		trap->Error( ERR_DROP, "Client/Server game mismatch: " GAME_VERSION "/%s", s );
		return;
	}

	CG_TransitionPermanent();

	CG_LoadingString( "Server info" );
	CG_ParseServerinfo();

	CG_LoadingString( "String pool" );
	String_Init();

	CG_LoadingString( "Sabers" );
	WP_SaberLoadParms();

	CG_LoadMedia();
	cgs.activeCursor = media.gfx.interface.cursor;

	CG_LoadingString( "Asset cache" );
	CG_AssetCache();

	CG_LoadingString( "HUD" );
	CG_LoadHudMenu();

	CG_LoadingString( "Players" );
	CG_RegisterClients();

	CG_LoadingString( "Local entities" );
	CG_InitLocalEntities();

	CG_LoadingString( "" );
	CG_InitMarkPolys();

	CG_LoadingString( "Finalising world state" );
	CG_SetConfigValues();
	CG_StartMusic( false );
	CG_ClearLightStyles();
	CG_ShaderStateChanged();
	trap->S_ClearLoopingSounds();
	cg.distanceCull = trap->R_GetDistanceCull();

	CG_LoadingString( "Spawning entities" );
	CG_ParseEntitiesFromString();

	CG_LoadingString( "" );
}

//makes sure returned string is in localized format
const char *CG_GetLocationString(const char *loc)
{
	static char text[1024]={0};

	if (!loc || loc[0] != '@')
	{ //just a raw string
		return loc;
	}

	trap->SE_GetStringTextString(loc+1, text, sizeof(text));
	return text;
}

//clean up all the ghoul2 allocations, the nice and non-hackly way -rww
void CG_DestroyAllGhoul2(void)
{
	int i = 0;
	int j;

//	Com_Printf("... CGameside GHOUL2 Cleanup\n");
	while (i < MAX_GENTITIES)
	{ //free all dynamically allocated npc client info structs and ghoul2 instances
		CG_KillCEntityG2(i);
		i++;
	}

	//Clean the weapon instances
	CG_ShutDownG2Weapons();

	i = 0;
	while (i < MAX_ITEMS)
	{ //and now for items
		j = 0;
		while (j < MAX_ITEM_MODELS)
		{
			if (cg_items[i].g2Models[j] && trap->G2_HaveWeGhoul2Models(cg_items[i].g2Models[j]))
			{
				trap->G2API_CleanGhoul2Models(&cg_items[i].g2Models[j]);
				cg_items[i].g2Models[j] = nullptr;
			}
			j++;
		}
		i++;
	}

	//Clean the global jetpack instance
	CG_CleanJetpackGhoul2();
}

// Called before every level change or subsystem restart
void CG_Shutdown( void )
{
	BG_ClearAnimsets(); //free all dynamic allocations made through the engine

    CG_DestroyAllGhoul2();

//	Com_Printf("... FX System Cleanup\n");
	trap->FX_FreeSystem();
	trap->ROFF_Clean();

	//reset weather
	trap->R_WorldEffectCommand("die");

	UI_CleanupGhoul2();
	//If there was any ghoul2 stuff in our side of the shared ui code, then remove it now.

	// some mods may need to do cleanup work here,
	// like closing files or archiving session data
}

void CG_NextForcePower_f( void )
{
	int current;
	usercmd_t cmd;
	if ( !cg.snap )
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	current = trap->GetCurrentCmdNumber();
	trap->GetUserCmd(current, &cmd);
	if ((cmd.buttons & BUTTON_USE) || CG_NoUseableForce())
	{
		CG_NextInventory_f();
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

//	BG_CycleForce(&cg.snap->ps, 1);
	if (cg.forceSelect != -1)
	{
		cg.snap->ps.fd.forcePowerSelected = (forcePowers_e)cg.forceSelect;
	}

	BG_CycleForce(&cg.snap->ps, 1);

	if (cg.snap->ps.fd.forcePowersKnown & (1 << cg.snap->ps.fd.forcePowerSelected))
	{
		cg.forceSelect = cg.snap->ps.fd.forcePowerSelected;
		cg.forceSelectTime = cg.time;
	}
}

void CG_PrevForcePower_f( void )
{
	int current;
	usercmd_t cmd;
	if ( !cg.snap )
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	current = trap->GetCurrentCmdNumber();
	trap->GetUserCmd(current, &cmd);
	if ((cmd.buttons & BUTTON_USE) || CG_NoUseableForce())
	{
		CG_PrevInventory_f();
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

//	BG_CycleForce(&cg.snap->ps, -1);
	if (cg.forceSelect != -1)
	{
		cg.snap->ps.fd.forcePowerSelected = (forcePowers_e)cg.forceSelect;
	}

	BG_CycleForce(&cg.snap->ps, -1);

	if (cg.snap->ps.fd.forcePowersKnown & (1 << cg.snap->ps.fd.forcePowerSelected))
	{
		cg.forceSelect = cg.snap->ps.fd.forcePowerSelected;
		cg.forceSelectTime = cg.time;
	}
}

void CG_NextInventory_f(void)
{
	if ( !cg.snap )
	{
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	if (cg.itemSelect != -1)
	{
		cg.snap->ps.stats[STAT_HOLDABLE_ITEM] = BG_GetItemIndexByTag(cg.itemSelect, IT_HOLDABLE);
	}
	BG_CycleInven(&cg.snap->ps, 1);

	if (cg.snap->ps.stats[STAT_HOLDABLE_ITEM])
	{
		cg.itemSelect = bg_itemlist[cg.snap->ps.stats[STAT_HOLDABLE_ITEM]].giTag;
		cg.invenSelectTime = cg.time;
	}
}

void CG_PrevInventory_f(void)
{
	if ( !cg.snap )
	{
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	if (cg.itemSelect != -1)
	{
		cg.snap->ps.stats[STAT_HOLDABLE_ITEM] = BG_GetItemIndexByTag(cg.itemSelect, IT_HOLDABLE);
	}
	BG_CycleInven(&cg.snap->ps, -1);

	if (cg.snap->ps.stats[STAT_HOLDABLE_ITEM])
	{
		cg.itemSelect = bg_itemlist[cg.snap->ps.stats[STAT_HOLDABLE_ITEM]].giTag;
		cg.invenSelectTime = cg.time;
	}
}

static void _CG_MouseEvent( int x, int y ) {
	cgDC.cursorx = cgs.cursorX;
	cgDC.cursory = cgs.cursorY;
	CG_MouseEvent( x, y );
}

static bool CG_IncomingConsoleCommand( void ) {
	return true;
}

static void CG_GetOrigin( int entID, vec3_t out ) {
	VectorCopy( cg_entities[entID].currentState.pos.trBase, out );
}

static void CG_GetAngles( int entID, vec3_t out ) {
	VectorCopy( cg_entities[entID].currentState.apos.trBase, out );
}

static trajectory_t *CG_GetOriginTrajectory( int entID ) {
	return &cg_entities[entID].nextState.pos;
}

static trajectory_t *CG_GetAngleTrajectory( int entID ) {
	return &cg_entities[entID].nextState.apos;
}

static void _CG_ROFF_NotetrackCallback( int entID, const char *notetrack ) {
	CG_ROFF_NotetrackCallback( &cg_entities[entID], notetrack );
}

static void CG_MapChange( void ) {
	// this may be called more than once for a given map change, as the server is going to attempt to send out
	// multiple broadcasts in hopes that the client will receive one of them
	cg.mMapChange = true;
}

static void CG_AutomapInput( void ) {
	autoMapInput_t *autoInput = &cg.sharedBuffer.autoMapInput;

	memcpy( &cg_autoMapInput, autoInput, sizeof( autoMapInput_t ) );

	if ( cg_autoMapInput.yaw ) {
		cg_autoMapAngle[YAW] += cg_autoMapInput.yaw;
	}
	if ( cg_autoMapInput.pitch ) {
		cg_autoMapAngle[PITCH] += cg_autoMapInput.pitch;
	}
	cg_autoMapInput.yaw = 0.0f;
	cg_autoMapInput.pitch = 0.0f;
}

static void CG_FX_CameraShake( void ) {
	TCGCameraShake *data = &cg.sharedBuffer.cameraShake;
	CG_DoCameraShake( data->mOrigin, data->mIntensity, data->mRadius, data->mTime );
}

cgameImport_t *trap = nullptr;

Q_CABI {
Q_EXPORT cgameExport_t* QDECL GetModuleAPI( int apiVersion, cgameImport_t *import )
{
	static cgameExport_t cge = {0};

	assert( import );
	trap = import;
	Com_Printf	= trap->Print;
	Com_Error	= trap->Error;

	memset( &cge, 0, sizeof( cge ) );

	if ( apiVersion != CGAME_API_VERSION ) {
		trap->Print( "Mismatched CGAME_API_VERSION: expected %i, got %i\n", CGAME_API_VERSION, apiVersion );
		return nullptr;
	}

	cge.Init					= CG_Init;
	cge.Shutdown				= CG_Shutdown;
	cge.ConsoleCommand			= CG_ConsoleCommand;
	cge.DrawActiveFrame			= CG_DrawActiveFrame;
	cge.CrosshairPlayer			= CG_CrosshairPlayer;
	cge.LastAttacker			= CG_LastAttacker;
	cge.KeyEvent				= CG_KeyEvent;
	cge.MouseEvent				= _CG_MouseEvent;
	cge.EventHandling			= CG_EventHandling;
	cge.PointContents			= C_PointContents;
	cge.GetLerpOrigin			= C_GetLerpOrigin;
	cge.GetLerpData				= C_GetLerpData;
	cge.Trace					= C_Trace;
	cge.G2Trace					= C_G2Trace;
	cge.G2Mark					= C_G2Mark;
	cge.RagCallback				= CG_RagCallback;
	cge.IncomingConsoleCommand	= CG_IncomingConsoleCommand;
	cge.NoUseableForce			= CG_NoUseableForce;
	cge.GetOrigin				= CG_GetOrigin;
	cge.GetAngles				= CG_GetAngles;
	cge.GetOriginTrajectory		= CG_GetOriginTrajectory;
	cge.GetAngleTrajectory		= CG_GetAngleTrajectory;
	cge.ROFF_NotetrackCallback	= _CG_ROFF_NotetrackCallback;
	cge.MapChange				= CG_MapChange;
	cge.AutomapInput			= CG_AutomapInput;
	cge.MiscEnt					= CG_MiscEnt;
	cge.CameraShake				= CG_FX_CameraShake;

	return &cge;
}
}