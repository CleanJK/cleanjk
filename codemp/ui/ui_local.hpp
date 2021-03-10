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



#include "qcommon/q_shared.hpp"
#include "rd-common/tr_types.hpp"
#include "ui/ui_engine.hpp"
#include "client/cl_keycodes.hpp"
#include "game/bg_public.hpp"
#include "ui/ui_shared.hpp"
#include "ui/ui_engine.hpp"

#define XCVAR_PROTO
#include "ui/ui_xcvar.hpp"
#undef XCVAR_PROTO



#define ACTION_BUFFER_SIZE		128
#define DEMO_DIRECTORY			"demos"
#define DEMO_EXTENSION			"dm_"
#define MAPS_PER_TIER			3
#define MAX_ALIASES				64
#define MAX_DEMOLIST			(MAX_DEMOS * MAX_QPATH)
#define MAX_DEMOS				2048 // 256
#define MAX_DISPLAY_SERVERS		2048
#define MAX_FORCE_CONFIGS		128
#define MAX_FOUNDPLAYER_SERVERS	16
#define MAX_GAMETYPES			16
#define MAX_MAPS				512 // 128
#define MAX_MODS				64
#define MAX_MOVIES				2048 // 256
#define MAX_PINGREQUESTS		32
#define MAX_Q3PLAYERMODELS		1024 //256
#define MAX_SCROLLTEXT_LINES	64
#define MAX_SCROLLTEXT_SIZE		4096
#define MAX_SERVERSTATUS_LINES	128
#define MAX_SERVERSTATUS_TEXT	4096 //1024
#define MAX_TEAMS				64
#define MAX_TIERS				16
#define MENUSET_DEFAULT			"ui/main.txt"
#define MENUSET_INGAME			"ui/ingame.txt"
#define PLAYERS_PER_TEAM		8 //5
#define SKIN_LENGTH				16
#define TEAM_MEMBERS			8//5



struct lerpFrame_t {
	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on
	int			frame;
	int			frameTime;			// time when ->frame will be exactly on
	float		backlerp;
	float		yawAngle;
	bool	yawing;
	float		pitchAngle;
	bool	pitching;
	int			animationNumber;
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact
};

struct playerInfo_t {
	qhandle_t		legsModel;
	qhandle_t		legsSkin;
	lerpFrame_t		legs;
	qhandle_t		torsoModel;
	qhandle_t		torsoSkin;
	lerpFrame_t		torso;
	animation_t		animations[MAX_TOTALANIMATIONS];
	qhandle_t		weaponModel;
	qhandle_t		barrelModel;
	qhandle_t		flashModel;
	vec3_t			flashDlightColor;
	int				muzzleFlashTime;
	vec3_t			viewAngles;
	vec3_t			moveAngles;
	weapon_e		currentWeapon;
	int				legsAnim;
	int				torsoAnim;
	weapon_e		weapon;
	weapon_e		lastWeapon;
	weapon_e		pendingWeapon;
	int				weaponTimer;
	int				pendingLegsAnim;
	int				torsoAnimationTimer;
	int				pendingTorsoAnim;
	int				legsAnimationTimer;
	bool		chat;
	bool		newModel;
	bool		barrelSpinning;
	float			barrelAngle;
	int				barrelTime;
	int				realWeapon;
};

struct aliasInfo_t {
	const char *name;
	const char *ai;
	const char *action;
};

struct teamInfo_t {
	const char *teamName;
	const char *imageName;
	const char *teamMembers[TEAM_MEMBERS];
	qhandle_t teamIcon;
	qhandle_t teamIcon_Metal;
	qhandle_t teamIcon_Name;
	int cinematic;
};

struct gameTypeInfo_t {
	const char *gameType;
	int gtEnum;
};

struct mapInfo_t {
	const char *mapName;
	const char *mapLoadName;
	const char *imageName;
	const char *opponentName;
	int teamMembers;
	int typeBits;
	int cinematic;
	int timeToBeat[MAX_GAMETYPES];
	qhandle_t levelShot;
	bool active;
};

struct tierInfo_t {
	const char *tierName;
	const char *maps[MAPS_PER_TIER];
	int gameTypes[MAPS_PER_TIER];
	qhandle_t mapHandles[MAPS_PER_TIER];
};

struct serverFilter_t {
	const char *description;
	const char *basedir;
};

struct pinglist_t {
	char	adrstr[MAX_ADDRESSLENGTH];
	int		start;
};

struct serverStatus_t {
	pinglist_t pingList[MAX_PINGREQUESTS];
	int		numqueriedservers;
	int		currentping;
	int		nextpingtime;
	int		maxservers;
	int		refreshtime;
	int		numServers;
	int		sortKey;
	int		sortDir;
	int		lastCount;
	bool refreshActive;
	int		currentServer;
	int		displayServers[MAX_DISPLAY_SERVERS];
	int		numDisplayServers;
	int		numPlayersOnServers;
	int		nextDisplayRefresh;
	int		nextSortTime;
	qhandle_t currentServerPreview;
	int		currentServerCinematic;
	int		motdLen;
	int		motdWidth;
	int		motdPaintX;
	int		motdPaintX2;
	int		motdOffset;
	int		motdTime;
	char	motd[MAX_STRING_CHARS];
};

struct pendingServer_t {
	char		adrstr[MAX_ADDRESSLENGTH];
	char		name[MAX_ADDRESSLENGTH];
	int			startTime;
	int			serverNum;
	bool	valid;
};

struct pendingServerStatus_t {
	int num;
	pendingServer_t server[MAX_SERVERSTATUSREQUESTS];
};

struct serverStatusInfo_t {
	char address[MAX_ADDRESSLENGTH];
	char *lines[MAX_SERVERSTATUS_LINES][4];
	char text[MAX_SERVERSTATUS_TEXT];
	char pings[MAX_CLIENTS * 3];
	int numLines;
};

struct modInfo_t {
	const char *modName;
	const char *modDescr;
};

struct skinName_t {
	char name[SKIN_LENGTH];
};

struct playerColor_t {
	char shader[MAX_QPATH];
	char actionText[ACTION_BUFFER_SIZE];
};

struct playerSpeciesInfo_t {
	char		Name[MAX_QPATH];
	int			SkinHeadCount;
	int			SkinHeadMax;
	skinName_t	*SkinHead;
	int			SkinTorsoCount;
	int			SkinTorsoMax;
	skinName_t	*SkinTorso;
	int			SkinLegCount;
	int			SkinLegMax;
	skinName_t	*SkinLeg;
	int			ColorMax;
	int			ColorCount;
	playerColor_t	*Color;
};

struct uiInfo_t {
	displayContextDef_t		uiDC;
	int						characterCount;
	int						botIndex;
	int						aliasCount;
	aliasInfo_t				aliasList[MAX_ALIASES];
	int						teamCount;
	teamInfo_t				teamList[MAX_TEAMS];
	int						numGameTypes;
	gameTypeInfo_t			gameTypes[MAX_GAMETYPES];
	int						redBlue;
	int						playerCount;
	int						myTeamCount;
	int						teamIndex;
	int						playerRefresh;
	int						playerIndex;
	int						playerNumber;
	bool				teamLeader;
	char					playerNames[MAX_CLIENTS][MAX_NETNAME];
	char					teamNames[MAX_CLIENTS][MAX_TEAMNAME];
	int						teamClientNums[MAX_CLIENTS];
	int						playerIndexes[MAX_CLIENTS]; //so we can vote-kick by index
	int						mapCount;
	mapInfo_t				mapList[MAX_MAPS];
	int						tierCount;
	tierInfo_t				tierList[MAX_TIERS];
	int						skillIndex;
	modInfo_t				modList[MAX_MODS];
	int						modCount;
	int						modIndex;
	char					demoList[MAX_DEMOS][MAX_QPATH];
	int						demoCount;
	int						demoIndex;
	int						loadedDemos;
	const char				*movieList[MAX_MOVIES];
	int						movieCount;
	int						movieIndex;
	int						previewMovie;
	char					scrolltext[MAX_SCROLLTEXT_SIZE];
	const char				*scrolltextLine[MAX_SCROLLTEXT_LINES];
	int						scrolltextLineCount;
	serverStatus_t			serverStatus;
	char					serverStatusAddress[MAX_ADDRESSLENGTH];
	serverStatusInfo_t		serverStatusInfo;
	int						nextServerStatusRefresh;
	pendingServerStatus_t	pendingServerStatus;
	char					findPlayerName[MAX_STRING_CHARS];
	char					foundPlayerServerAddresses[MAX_FOUNDPLAYER_SERVERS][MAX_ADDRESSLENGTH];
	char					foundPlayerServerNames[MAX_FOUNDPLAYER_SERVERS][MAX_ADDRESSLENGTH];
	int						currentFoundPlayerServer;
	int						numFoundPlayerServers;
	int						nextFindPlayerRefresh;
	int						currentCrosshair;
	int						q3HeadCount;
	char					q3HeadNames[MAX_Q3PLAYERMODELS][64];
	qhandle_t				q3HeadIcons[MAX_Q3PLAYERMODELS];
	int						q3SelectedHead;
	int						forceConfigCount;
	int						forceConfigSelected;
	char					forceConfigNames[MAX_FORCE_CONFIGS][128];
	bool				forceConfigSide[MAX_FORCE_CONFIGS]; //true if it's a light side config, false if dark side
	int						forceConfigDarkIndexBegin; //mark the index number dark configs start at
	int						forceConfigLightIndexBegin; //mark the index number light configs start at
	bool				inGameLoad;
	int						playerSpeciesMax;
	int						playerSpeciesCount;
	playerSpeciesInfo_t		*playerSpecies;
	int						playerSpeciesIndex;
	short					movesTitleIndex;
	const char				*movesBaseAnim;
	int						moveAnimTime;
	int						languageCount;
	int						languageCountIndex;
};



extern bool ui_saber_parms_parsed;
extern uiImport_t* trap;
extern uiInfo_t uiInfo;



bool        UI_ConsoleCommand         ( int realTime );
bool        UI_FeederSelection        ( float feederID, int index, itemDef_t *item );
bool        UI_SaberModelForSaber     ( const char *saberName, char *saberModel );
bool        UI_SaberProperNameForSaber( const char *saberName, char *saberProperName );
bool        UI_SaberTypeForSaber      ( const char *saberName, char *saberType );
bool        UI_TrueJediEnabled        ( void );
char       *UI_Cvar_VariableString    ( const char *var_name );
char       *UI_GetBotNameByNumber     ( int num );
const char *UI_GetStringEdString      ( const char *refSection, const char *refName );
const char *UI_TeamName               ( int team );
int         UI_GetNumBots             ( void );
void        UI_CacheSaberGlowGraphics ( void );
void        UI_DrawConnectScreen      ( bool overlay );
void        UI_DrawHandlePic          ( float x, float y, float w, float h, qhandle_t hShader );
void        UI_FillRect               ( float x, float y, float width, float height, const float *color );
void        UI_Init                   ( bool inGameLoad  );
void        UI_KeyEvent               ( int key, bool down );
void        UI_Load                   ( void );
void        UI_LoadArenas             ( void );
void        UI_LoadBots               ( void );
void        UI_LoadForceConfig_List   ( void );
void        UI_LoadMenus              ( const char *menuFile, bool reset );
void        UI_MouseEvent             ( int dx, int dy );
void        UI_Refresh                ( int realtime );
void        UI_RegisterCvars          ( void );
void        UI_Report                 ( void );
void        UI_SaberDrawBlades        ( itemDef_t *item, vec3_t origin, vec3_t angles );
void        UI_SaberLoadParms         ( void );
void        UI_SetActiveMenu          ( uiMenuCommand_e menu );
void        UI_Shutdown               ( void );
void        UI_UpdateCvars            ( void );
void        UpdateForceUsed           ( void );
