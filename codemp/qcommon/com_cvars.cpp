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

#include "game/bg_public.hpp"
#include "qcommon/com_cvar.hpp"
#include "qcommon/com_cvars.hpp"
#include "qcommon/game_version.hpp"

#ifdef _DEBUG
cvar_t *fx_freeze;
#endif // _DEBUG

#ifndef _WIN32
cvar_t *com_ansiColor;
#endif // _WIN32

#ifdef G2_PERFORMANCE_ANALYSIS
cvar_t *com_G2Report;
#endif // G2_PERFORMANCE_ANALYSIS

#ifdef MACOS_X
cvar_t *fs_apppath;                    // Also search the .app bundle for .pk3 files
#endif // MACOS_X

#ifdef USE_OPENAL
cvar_t *s_useOpenAL;
#endif // USE_OPENAL

cvar_t *activeAction;
cvar_t *bot_aasoptimize;
cvar_t *bot_challenge;
cvar_t *bot_debug;
cvar_t *bot_enable;
cvar_t *bot_fastchat;
cvar_t *bot_forceclustering;
cvar_t *bot_forcereachability;
cvar_t *bot_forcewrite;
cvar_t *bot_grapple;
cvar_t *bot_groundonly;
cvar_t *bot_highlightarea;
cvar_t *bot_interbreedbots;
cvar_t *bot_interbreedchar;
cvar_t *bot_interbreedcycle;
cvar_t *bot_interbreedwrite;
cvar_t *bot_maxdebugpolys;
cvar_t *bot_minplayers;
cvar_t *bot_nochat;
cvar_t *bot_pause;
cvar_t *bot_reachability;
cvar_t *bot_reloadcharacters;
cvar_t *bot_report;
cvar_t *bot_rocketjump;
cvar_t *bot_saveroutingcache;
cvar_t *bot_testclusters;
cvar_t *bot_testichat;
cvar_t *bot_testrchat;
cvar_t *bot_testsolid;
cvar_t *bot_thinktime;
cvar_t *bot_visualizejumppads;
cvar_t *capturelimit;
cvar_t *cg_autoswitch;
cvar_t *cg_drawRadar;
cvar_t *cg_predictItems;
cvar_t *cg_viewsize;
cvar_t *char_color_blue;
cvar_t *char_color_green;
cvar_t *char_color_red;
cvar_t *cl_allowAltEnter;
cvar_t *cl_allowDownload;
cvar_t *cl_anglespeedkey;
cvar_t *cl_autolodscale;
cvar_t *cl_avi2GBLimit;
cvar_t *cl_aviFrameRate;
cvar_t *cl_aviMotionJpeg;
cvar_t *cl_consoleKeys;
cvar_t *cl_consoleUseScanCode;
cvar_t *cl_conXOffset;
cvar_t *cl_debugMove;
cvar_t *cl_drawRecording;
cvar_t *cl_enableGuid;
cvar_t *cl_forceavidemo;
cvar_t *cl_framerate;
cvar_t *cl_freelook;
cvar_t *cl_freezeDemo;
cvar_t *cl_guidServerUniq;
cvar_t *cl_lanForcePackets;
cvar_t *cl_maxpackets;
cvar_t *cl_maxPing;
cvar_t *cl_motd;
cvar_t *cl_motdServer1;
cvar_t *cl_motdServer2;
cvar_t *cl_motdString;
cvar_t *cl_mouseAccel;
cvar_t *cl_mouseAccelOffset;
cvar_t *cl_mouseAccelStyle;
cvar_t *cl_nodelta;
cvar_t *cl_noprint;
cvar_t *cl_packetdup;
cvar_t *cl_paused;
cvar_t *cl_pitchspeed;
cvar_t *cl_recordSPDemo;
cvar_t *cl_renderer;
cvar_t *cl_run;
cvar_t *cl_running;
cvar_t *cl_serverStatusResendTime;
cvar_t *cl_showMouseRate;
cvar_t *cl_shownet;
cvar_t *cl_showSend;
cvar_t *cl_showTimeDelta;
cvar_t *cl_timeNudge;
cvar_t *cl_timeout;
cvar_t *cl_yawspeed;
cvar_t *cm_debugSize;
cvar_t *cm_extraVerbose;
cvar_t *cm_noAreas;
cvar_t *cm_noCurves;
cvar_t *cm_noMapCache;
cvar_t *cm_playerCurveClip;
cvar_t *color1;
cvar_t *color2;
cvar_t *com_affinity;
cvar_t *com_buildScript;
cvar_t *com_busyWait;
cvar_t *com_cameraMode;
cvar_t *com_journal;
cvar_t *com_showtrace;
cvar_t *com_speeds;
cvar_t *com_validateZone;
cvar_t *con_autoclear;
cvar_t *con_notifytime;
cvar_t *con_opacity;
cvar_t *d_saberStanceDebug;
cvar_t *debuggraph;
cvar_t *dedicated;
cvar_t *developer;
cvar_t *dmflags;
cvar_t *duel_fraglimit;
cvar_t *fixedtime;
cvar_t *forcepowers;
cvar_t *fraglimit;
cvar_t *fs_basegame;
cvar_t *fs_basepath;
cvar_t *fs_cdpath;
cvar_t *fs_copyfiles;
cvar_t *fs_debug;
cvar_t *fs_dirbeforepak;
cvar_t *fs_game;
cvar_t *fs_homepath;
cvar_t *fx_countScale;
cvar_t *fx_debug;
cvar_t *fx_nearCull;
cvar_t *g_duelWeaponDisable;
cvar_t *g_forceBasedTeams;
cvar_t *g_forcePowerDisable;
cvar_t *g_gametype;
cvar_t *g_jediVmerc;
cvar_t *g_maxForceRank;
cvar_t *g_maxHolocronCarry;
cvar_t *g_needpass;
cvar_t *g_privateDuel;
cvar_t *g_saberLocking;
cvar_t *g_synchronousClients;
cvar_t *g_weaponDisable;
cvar_t *graphheight;
cvar_t *graphscale;
cvar_t *graphshift;
cvar_t *in_joystick;
cvar_t *ja_guid;
cvar_t *logfile;
cvar_t *m_filter;
cvar_t *m_forward;
cvar_t *m_pitch;
cvar_t *m_pitchVeh;
cvar_t *m_side;
cvar_t *m_yaw;
cvar_t *mapname;
cvar_t *model;
cvar_t *name;
cvar_t *net_dropsim;
cvar_t *net_enabled;
cvar_t *net_forcenonlocal;
cvar_t *net_ip;
cvar_t *net_port;
cvar_t *net_qport;
cvar_t *net_socksEnabled;
cvar_t *net_socksPassword;
cvar_t *net_socksPort;
cvar_t *net_socksServer;
cvar_t *net_socksUsername;
cvar_t *nextdemo;
cvar_t *nextmap;
cvar_t *password;
cvar_t *protocol;
cvar_t *r_debugSurface;
cvar_t *r_debugSurfaceUpdate;
cvar_t *r_fullscreen;
cvar_t *r_inGameVideo;
cvar_t *rate;
cvar_t *rconAddress;
cvar_t *rconPassword;
cvar_t *s_allowDynamicMusic;
cvar_t *s_debugdynamic;
cvar_t *s_doppler;
cvar_t *s_initsound;
cvar_t *s_khz;
cvar_t *s_language;
cvar_t *s_mixahead;
cvar_t *s_mixPreStep;
cvar_t *s_mp3overhead;
cvar_t *s_musicVolume;
cvar_t *s_separation;
cvar_t *s_show;
cvar_t *s_soundpoolmegs;
cvar_t *s_testsound;
cvar_t *s_threshold1;
cvar_t *s_threshold2;
cvar_t *s_threshold3;
cvar_t *s_threshold4;
cvar_t *s_volume;
cvar_t *s_volumeVoice;
cvar_t *saber1;
cvar_t *saber2;
cvar_t *scr_conspeed;
cvar_t *se_debug;
cvar_t *se_language;
cvar_t *sensitivity;
cvar_t *sex;
cvar_t *showdrop;
cvar_t *showpackets;
cvar_t *snaps;
cvar_t *sp_leet;
cvar_t *sv_allowDownload;
cvar_t *sv_autoDemo;
cvar_t *sv_autoDemoBots;
cvar_t *sv_autoDemoMaxMaps;
cvar_t *sv_banFile;
cvar_t *sv_cheats;
cvar_t *sv_clientRate;
cvar_t *sv_filterCommands;
cvar_t *sv_floodProtect;
cvar_t *sv_floodProtectSlow;
cvar_t *sv_fps;
cvar_t *sv_hostname;
cvar_t *sv_keywords;
cvar_t *sv_killserver;
cvar_t *sv_lanForceRate;
cvar_t *sv_legacyFixes;
cvar_t *sv_mapChecksum;
cvar_t *sv_master;
cvar_t *sv_maxclients;
cvar_t *sv_maxPing;
cvar_t *sv_maxRate;
cvar_t *sv_minPing;
cvar_t *sv_minRate;
cvar_t *sv_padPackets;
cvar_t *sv_pakNames;
cvar_t *sv_paks;
cvar_t *sv_paused;
cvar_t *sv_privateClients;
cvar_t *sv_privatePassword;
cvar_t *sv_pure;
cvar_t *sv_ratePolicy;
cvar_t *sv_reconnectlimit;
cvar_t *sv_referencedPakNames;
cvar_t *sv_referencedPaks;
cvar_t *sv_running;
cvar_t *sv_serverid;
cvar_t *sv_showghoultraces;
cvar_t *sv_showloss;
cvar_t *sv_snapsMax;
cvar_t *sv_snapsMin;
cvar_t *sv_snapsPolicy;
cvar_t *sv_timeout;
cvar_t *sv_zombietime;
cvar_t *timedemo;
cvar_t *timegraph;
cvar_t *timelimit;
cvar_t *timescale;
cvar_t *version;

void Com_InitCvars( void ) {
	#ifdef _DEBUG
	fx_freeze =                 Cvar_Get( "fx_freeze",                 "0",                                    CVAR_CHEAT,                                 "" );
	#endif // _DEBUG

	#ifndef _WIN32
	com_ansiColor =             Cvar_Get( "com_ansiColor",             "0",                                    CVAR_ARCHIVE_ND,                            "" );
	#endif // _WIN32

	#ifdef DEDICATED
	dedicated =                 Cvar_Get( "dedicated",                 "2",                                    CVAR_INIT,                                  "" );
	#else
	//OJKFIXME: Temporarily disabled dedicated server when not using the dedicated server binary.
	//			Issue is the server not having a renderer when not using ^  and crashing in SV_SpawnServer calling re.RegisterMedia_LevelLoadBegin
	//			Until we fully remove the renderer from the server, the client executable will not have dedicated support capabilities.
	//			Use the dedicated server package.
	dedicated =                 Cvar_Get( "dedicated",                 "0",                                    CVAR_ROM | CVAR_INIT | CVAR_PROTECTED,      "" );
	#endif // DEDICATED

	#ifdef G2_PERFORMANCE_ANALYSIS
	com_G2Report =              Cvar_Get( "com_G2Report",              "0",                                    CVAR_NONE,                                  "" );
	#endif // G2_PERFORMANCE_ANALYSIS

	#ifdef MACOS_X
	fs_apppath =                Cvar_Get( "fs_apppath",                Sys_DefaultAppPath(),                   CVAR_INIT | CVAR_PROTECTED,                 "(Read Only) Location of OSX .app bundle" );
	m_filter =                  Cvar_Get( "m_filter",                  "1",                                    CVAR_ARCHIVE_ND,                            "" ); // Input is jittery on OS X w/o this
	#else
	m_filter =                  Cvar_Get( "m_filter",                  "0",                                    CVAR_ARCHIVE_ND,                            "" );
	#endif // MACOS_X

	#ifdef USE_OPENAL
	s_useOpenAL =               Cvar_Get( "s_useOpenAL",               "0",                                    CVAR_ARCHIVE | CVAR_LATCH,                  "" );
	#endif // USE_OPENAL

	activeAction =              Cvar_Get( "activeAction",              "",                                     CVAR_TEMP,                                   "" );
	bot_aasoptimize =           Cvar_Get( "bot_aasoptimize",           "0",                                    CVAR_NONE,                                   "" ); // no aas file optimisation
	bot_challenge =             Cvar_Get( "bot_challenge",             "0",                                    CVAR_NONE,                                   "" ); // challenging bot
	bot_debug =                 Cvar_Get( "bot_debug",                 "0",                                    CVAR_CHEAT,                                  "" ); // enable bot debugging
	bot_debug =                 Cvar_Get( "bot_debug",                 "0",                                    CVAR_NONE,                                   "" );
	bot_enable =                Cvar_Get( "bot_enable",                "1",                                    CVAR_LATCH,                                  "" ); // enable the bot
	bot_fastchat =              Cvar_Get( "bot_fastchat",              "0",                                    CVAR_NONE,                                   "" ); // fast chatting bots
	bot_forceclustering =       Cvar_Get( "bot_forceclustering",       "0",                                    CVAR_NONE,                                   "" ); // force cluster calculations
	bot_forcereachability =     Cvar_Get( "bot_forcereachability",     "0",                                    CVAR_NONE,                                   "" ); // force reachability calculations
	bot_forcewrite =            Cvar_Get( "bot_forcewrite",            "0",                                    CVAR_NONE,                                   "" ); // force writing aas file
	bot_grapple =               Cvar_Get( "bot_grapple",               "0",                                    CVAR_NONE,                                   "" ); // enable grapple
	bot_groundonly =            Cvar_Get( "bot_groundonly",            "1",                                    CVAR_NONE,                                   "" );
	bot_groundonly =            Cvar_Get( "bot_groundonly",            "1",                                    CVAR_NONE,                                   "" ); // only show ground faces of areas
	bot_highlightarea =         Cvar_Get( "bot_highlightarea",         "0",                                    CVAR_NONE,                                   "" );
	bot_interbreedbots =        Cvar_Get( "bot_interbreedbots",        "10",                                   CVAR_CHEAT,                                  "" ); // number of bots used for interbreeding
	bot_interbreedchar =        Cvar_Get( "bot_interbreedchar",        "",                                     CVAR_CHEAT,                                  "" ); // bot character used for interbreeding
	bot_interbreedcycle =       Cvar_Get( "bot_interbreedcycle",       "20",                                   CVAR_CHEAT,                                  "" ); // bot interbreeding cycle
	bot_interbreedwrite =       Cvar_Get( "bot_interbreedwrite",       "",                                     CVAR_CHEAT,                                  "" ); // write interbreeded bots to this file
	bot_maxdebugpolys =         Cvar_Get( "bot_maxdebugpolys",         "2",                                    CVAR_NONE,                                   "" ); // maximum number of debug polys
	bot_minplayers =            Cvar_Get( "bot_minplayers",            "0",                                    CVAR_NONE,                                   "" ); // minimum players in a team or the game
	bot_nochat =                Cvar_Get( "bot_nochat",                "0",                                    CVAR_NONE,                                   "" ); // disable chats
	bot_pause =                 Cvar_Get( "bot_pause",                 "0",                                    CVAR_CHEAT,                                  "" ); // pause the bots thinking
	bot_reachability =          Cvar_Get( "bot_reachability",          "0",                                    CVAR_NONE,                                   "" );
	bot_reachability =          Cvar_Get( "bot_reachability",          "0",                                    CVAR_NONE,                                   "" ); // show all reachabilities to other areas
	bot_reloadcharacters =      Cvar_Get( "bot_reloadcharacters",      "0",                                    CVAR_NONE,                                   "" ); // reload the bot characters each time
	bot_report =                Cvar_Get( "bot_report",                "0",                                    CVAR_CHEAT,                                  "" ); // get a full report in ctf
	bot_rocketjump =            Cvar_Get( "bot_rocketjump",            "1",                                    CVAR_NONE,                                   "" ); // enable rocket jumping
	bot_saveroutingcache =      Cvar_Get( "bot_saveroutingcache",      "0",                                    CVAR_NONE,                                   "" ); // save routing cache
	bot_testclusters =          Cvar_Get( "bot_testclusters",          "0",                                    CVAR_CHEAT,                                  "" ); // test the AAS clusters
	bot_testichat =             Cvar_Get( "bot_testichat",             "0",                                    CVAR_NONE,                                   "" ); // test ichats
	bot_testrchat =             Cvar_Get( "bot_testrchat",             "0",                                    CVAR_NONE,                                   "" ); // test rchats
	bot_testsolid =             Cvar_Get( "bot_testsolid",             "0",                                    CVAR_CHEAT,                                  "" ); // test for solid areas
	bot_thinktime =             Cvar_Get( "bot_thinktime",             "100",                                  CVAR_CHEAT,                                  "" ); // msec the bots thinks
	bot_visualizejumppads =     Cvar_Get( "bot_visualizejumppads",     "0",                                    CVAR_CHEAT,                                  "" ); // show jumppads
	capturelimit =              Cvar_Get( "capturelimit",              "0",                                    CVAR_SERVERINFO,                             "" );
	cg_autoswitch =             Cvar_Get( "cg_autoswitch",             "1",                                    CVAR_ARCHIVE,                                "" );
	cg_drawRadar =              Cvar_Get( "cg_drawRadar",              "1",                                    CVAR_NONE,                                   "" );
	cg_predictItems =           Cvar_Get( "cg_predictItems",           "1",                                    CVAR_USERINFO | CVAR_ARCHIVE,                "" );
	cg_viewsize =               Cvar_Get( "cg_viewsize",               "100",                                  CVAR_ARCHIVE_ND,                             "" );
	char_color_blue =           Cvar_Get( "char_color_blue",           "255",                                  CVAR_USERINFO | CVAR_ARCHIVE,                "Player tint (Blue)" );
	char_color_green =          Cvar_Get( "char_color_green",          "255",                                  CVAR_USERINFO | CVAR_ARCHIVE,                "Player tint (Green)" );
	char_color_red =            Cvar_Get( "char_color_red",            "255",                                  CVAR_USERINFO | CVAR_ARCHIVE,                "Player tint (Red)" );
	cl_allowAltEnter =          Cvar_Get( "cl_allowAltEnter",          "1",                                    CVAR_ARCHIVE_ND,                             "Enables use of ALT+ENTER keyboard combo to toggle fullscreen" );
	cl_allowDownload =          Cvar_Get( "cl_allowDownload",          "0",                                    CVAR_ARCHIVE_ND,                             "Allow downloading custom paks from server" );
	cl_anglespeedkey =          Cvar_Get( "cl_anglespeedkey",          "1.5",                                  CVAR_ARCHIVE_ND,                             "" );
	cl_autolodscale =           Cvar_Get( "cl_autolodscale",           "1",                                    CVAR_ARCHIVE_ND,                             "" );
	cl_avi2GBLimit =            Cvar_Get( "cl_avi2GBLimit",            "1",                                    CVAR_ARCHIVE,                                "" );
	cl_aviFrameRate =           Cvar_Get( "cl_aviFrameRate",           "25",                                   CVAR_ARCHIVE,                                "" );
	cl_aviMotionJpeg =          Cvar_Get( "cl_aviMotionJpeg",          "1",                                    CVAR_ARCHIVE,                                "" );
	cl_consoleKeys =            Cvar_Get( "cl_consoleKeys",            "~ ` 0x7e 0x60 0xb2",                   CVAR_ARCHIVE,                                "Which keys are used to toggle the console" );
	cl_consoleUseScanCode =     Cvar_Get( "cl_consoleUseScanCode",     "1",                                    CVAR_ARCHIVE,                                "Use native console key detection" );
	cl_conXOffset =             Cvar_Get( "cl_conXOffset",             "0",                                    CVAR_NONE,                                   "" );
	cl_conXOffset =             Cvar_Get( "cl_conXOffset",             "0",                                    CVAR_NONE,                                   "" );
	cl_conXOffset =             Cvar_Get( "cl_conXOffset",             "0",                                    CVAR_NONE,                                   "" );
	cl_debugMove =              Cvar_Get( "cl_debugMove",              "0",                                    CVAR_NONE,                                   "" );
	cl_drawRecording =          Cvar_Get( "cl_drawRecording",          "1",                                    CVAR_ARCHIVE,                                "" );
	cl_enableGuid =             Cvar_Get( "cl_enableGuid",             "1",                                    CVAR_ARCHIVE_ND,                             "Enable GUID userinfo identifier" ); // enable the ja_guid player identifier in userinfo by default in OpenJK
	cl_forceavidemo =           Cvar_Get( "cl_forceavidemo",           "0",                                    CVAR_NONE,                                   "" );
	cl_framerate =              Cvar_Get( "cl_framerate",              "0",                                    CVAR_TEMP,                                   "" );
	cl_freelook =               Cvar_Get( "cl_freelook",               "1",                                    CVAR_ARCHIVE_ND,                             "Mouse look" );
	cl_freezeDemo =             Cvar_Get( "cl_freezeDemo",             "0",                                    CVAR_TEMP,                                   "" );
	cl_guidServerUniq =         Cvar_Get( "cl_guidServerUniq",         "1",                                    CVAR_ARCHIVE_ND,                             "Use a unique guid value per server" );
	cl_lanForcePackets =        Cvar_Get( "cl_lanForcePackets",        "1",                                    CVAR_ARCHIVE_ND,                             "" );
	cl_maxpackets =             Cvar_Get( "cl_maxpackets",             "63",                                   CVAR_ARCHIVE,                                "" );
	cl_maxPing =                Cvar_Get( "cl_maxPing",                "800",                                  CVAR_ARCHIVE_ND,                             "Max. ping for servers when searching the serverlist" );
	cl_motd =                   Cvar_Get( "cl_motd",                   "1",                                    CVAR_ARCHIVE_ND,                             "Display welcome message from master server on the bottom of connection screen" );
	cl_motdServer1 =            Cvar_Get( "cl_motdServer1",            UPDATE_SERVER_NAME,                     CVAR_NONE,                                   "" );
	cl_motdServer2 =            Cvar_Get( "cl_motdServer2",            JKHUB_UPDATE_SERVER_NAME,               CVAR_NONE,                                   "" );
	cl_motdString =             Cvar_Get( "cl_motdString",             "",                                     CVAR_ROM,                                    "" );
	cl_mouseAccel =             Cvar_Get( "cl_mouseAccel",             "0",                                    CVAR_ARCHIVE_ND,                             "Mouse acceleration value" );
	cl_mouseAccelOffset =       Cvar_Get( "cl_mouseAccelOffset",       "5",                                    CVAR_ARCHIVE_ND,                             "Mouse acceleration offset for style 1" );
	cl_mouseAccelStyle =        Cvar_Get( "cl_mouseAccelStyle",        "0",                                    CVAR_ARCHIVE_ND,                             "Mouse acceleration style (0:legacy, 1:QuakeLive)" );
	cl_nodelta =                Cvar_Get( "cl_nodelta",                "0",                                    CVAR_NONE,                                   "" );
	cl_noprint =                Cvar_Get( "cl_noprint",                "0",                                    CVAR_NONE,                                   "" );
	cl_packetdup =              Cvar_Get( "cl_packetdup",              "1",                                    CVAR_ARCHIVE_ND,                             "" );
	cl_paused =                 Cvar_Get( "cl_paused",                 "0",                                    CVAR_ROM,                                    "" );
	cl_pitchspeed =             Cvar_Get( "cl_pitchspeed",             "140",                                  CVAR_ARCHIVE_ND,                             "" );
	cl_renderer =               Cvar_Get( "cl_renderer",               DEFAULT_RENDER_LIBRARY,                 CVAR_ARCHIVE | CVAR_LATCH | CVAR_PROTECTED,  "Which renderer library to use" );
	cl_recordSPDemo =           Cvar_Get( "cl_recordSPDemo",           "0",                                    CVAR_ARCHIVE | CVAR_INTERNAL,                "" );
	cl_run =                    Cvar_Get( "cl_run",                    "1",                                    CVAR_ARCHIVE_ND,                             "Always run" );
	cl_running =                Cvar_Get( "cl_running",                "0",                                    CVAR_ROM,                                    "Is the client running?" );
	cl_serverStatusResendTime = Cvar_Get( "cl_serverStatusResendTime", "750",                                  CVAR_NONE,                                   "" );
	cl_showMouseRate =          Cvar_Get( "cl_showMouseRate",          "0",                                    CVAR_NONE,                                   "" );
	cl_shownet =                Cvar_Get( "cl_shownet",                "0",                                    CVAR_TEMP,                                   "" );
	cl_showSend =               Cvar_Get( "cl_showSend",               "0",                                    CVAR_TEMP,                                   "" );
	cl_showTimeDelta =          Cvar_Get( "cl_showTimeDelta",          "0",                                    CVAR_TEMP,                                   "" );
	cl_timeNudge =              Cvar_Get( "cl_timeNudge",              "0",                                    CVAR_TEMP,                                   "" );
	cl_timeout =                Cvar_Get( "cl_timeout",                "200",                                  CVAR_NONE,                                   "" );
	cl_yawspeed =               Cvar_Get( "cl_yawspeed",               "140",                                  CVAR_ARCHIVE_ND,                             "" );
	cm_debugSize =              Cvar_Get( "cm_debugSize",              "2",                                    CVAR_NONE,                                   "" );
	cm_extraVerbose =           Cvar_Get( "cm_extraVerbose",           "0",                                    CVAR_TEMP,                                   "" );
	cm_noAreas =                Cvar_Get( "cm_noAreas",                "0",                                    CVAR_CHEAT,                                  "" );
	cm_noCurves =               Cvar_Get( "cm_noCurves",               "0",                                    CVAR_CHEAT,                                  "" );
	cm_noMapCache =             Cvar_Get( "cm_noMapCache",             "0",                                    CVAR_ARCHIVE,                                "Free memory used by maps when changing map" );
	cm_playerCurveClip =        Cvar_Get( "cm_playerCurveClip",        "1",                                    CVAR_ARCHIVE_ND | CVAR_CHEAT,                "" );
	color1 =                    Cvar_Get( "color1",                    "4",                                    CVAR_USERINFO | CVAR_ARCHIVE,                "Player saber1 color" );
	color2 =                    Cvar_Get( "color2",                    "4",                                    CVAR_USERINFO | CVAR_ARCHIVE,                "Player saber2 color" );
	com_affinity =              Cvar_Get( "com_affinity",              "0",                                    CVAR_ARCHIVE_ND,                             "" );
	com_buildScript =           Cvar_Get( "com_buildScript",           "0",                                    CVAR_NONE,                                   "" );
	com_busyWait =              Cvar_Get( "com_busyWait",              "0",                                    CVAR_ARCHIVE_ND,                             "" );
	com_cameraMode =            Cvar_Get( "com_cameraMode",            "0",                                    CVAR_CHEAT,                                  "" );
	com_journal =               Cvar_Get( "com_journal",               "0",                                    CVAR_INIT,                                   "" );
	com_showtrace =             Cvar_Get( "com_showtrace",             "0",                                    CVAR_CHEAT,                                  "" );
	com_speeds =                Cvar_Get( "com_speeds",                "0",                                    CVAR_NONE,                                   "" );
	com_validateZone =          Cvar_Get( "com_validateZone",          "0",                                    CVAR_NONE,                                   "" );
	con_autoclear =             Cvar_Get( "con_autoclear",             "1",                                    CVAR_ARCHIVE_ND,                             "Automatically clear console input on close" );
	con_notifytime =            Cvar_Get( "con_notifytime",            "3",                                    CVAR_NONE,                                   "How many seconds notify messages should be shown before they fade away" );
	con_opacity =               Cvar_Get( "con_opacity",               "1.0",                                  CVAR_ARCHIVE_ND,                             "Opacity of console background" );
	d_saberStanceDebug =        Cvar_Get( "d_saberStanceDebug",        "0",                                    CVAR_NONE,                                   "" );
	debuggraph =                Cvar_Get( "debuggraph",                "0",                                    CVAR_CHEAT,                                  "" );
	dedicated =                 Cvar_Get( "dedicated",                 "2",                                    CVAR_INIT,                                   "" );
	developer =                 Cvar_Get( "developer",                 "0",                                    CVAR_TEMP,                                   "Developer mode" );
	dmflags =                   Cvar_Get( "dmflags",                   "0",                                    CVAR_SERVERINFO,                             "" );
	duel_fraglimit =            Cvar_Get( "duel_fraglimit",            "10",                                   CVAR_SERVERINFO,                             "" );
	fixedtime =                 Cvar_Get( "fixedtime",                 "0",                                    CVAR_CHEAT,                                  "" );
	forcepowers =               Cvar_Get( "forcepowers",               "7-1-032330000000001333",               CVAR_USERINFO | CVAR_ARCHIVE,                "Player forcepowers" );
	fraglimit =                 Cvar_Get( "fraglimit",                 "20",                                   CVAR_SERVERINFO,                             "" );
	fs_basegame =               Cvar_Get( "fs_basegame",               "",                                     CVAR_INIT,                                   "" );
	fs_basepath =               Cvar_Get( "fs_basepath",               Sys_DefaultInstallPath(),               CVAR_INIT | CVAR_PROTECTED,                  "(Read Only) Location for game files" );
	fs_cdpath =                 Cvar_Get( "fs_cdpath",                 "",                                     CVAR_INIT | CVAR_PROTECTED,                  "(Read Only) Location for development files" );
	fs_copyfiles =              Cvar_Get( "fs_copyfiles",              "0",                                    CVAR_INIT,                                   "" );
	fs_debug =                  Cvar_Get( "fs_debug",                  "0",                                    CVAR_NONE,                                   "" );
	fs_dirbeforepak =           Cvar_Get( "fs_dirbeforepak",           "0",                                    CVAR_INIT | CVAR_PROTECTED,                  "Prioritize directories before paks if not pure" );
	fs_game =                   Cvar_Get( "fs_game",                   "",                                     CVAR_INIT | CVAR_SYSTEMINFO,                 "Mod directory" );
	fs_homepath =               Cvar_Get( "fs_homepath",               "",                                     CVAR_INIT | CVAR_PROTECTED,                  "(Read/Write) Location for user generated files" );
	fx_countScale =             Cvar_Get( "fx_countScale",             "1",                                    CVAR_ARCHIVE_ND,                             "" );
	fx_debug =                  Cvar_Get( "fx_debug",                  "0",                                    CVAR_TEMP,                                   "" );
	fx_nearCull =               Cvar_Get( "fx_nearCull",               "16",                                   CVAR_ARCHIVE_ND,                             "" );
	g_duelWeaponDisable =       Cvar_Get( "g_duelWeaponDisable",       "1",                                    CVAR_SERVERINFO,                             "" );
	g_forceBasedTeams =         Cvar_Get( "g_forceBasedTeams",         "0",                                    CVAR_SERVERINFO,                             "" );
	g_forcePowerDisable =       Cvar_Get( "g_forcePowerDisable",       "0",                                    CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, "" );
	g_gametype =                Cvar_Get( "g_gametype",                "0",                                    CVAR_SERVERINFO | CVAR_LATCH,                "" );
	g_jediVmerc =               Cvar_Get( "g_jediVmerc",               "0",                                    CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, "" );
	g_maxForceRank =            Cvar_Get( "g_maxForceRank",            "7",                                    CVAR_SERVERINFO,                             "" );
	g_maxHolocronCarry =        Cvar_Get( "g_maxHolocronCarry",        "3",                                    CVAR_SERVERINFO,                             "" );
	g_needpass =                Cvar_Get( "g_needpass",                "0",                                    CVAR_SERVERINFO | CVAR_ROM,                  "Server needs password to join" );
	g_privateDuel =             Cvar_Get( "g_privateDuel",             "1",                                    CVAR_SERVERINFO,                             "" );
	g_saberLocking =            Cvar_Get( "g_saberLocking",            "1",                                    CVAR_SERVERINFO,                             "" );
	g_synchronousClients =		Cvar_Get( "g_synchronousClients",      "0",                                    CVAR_SYSTEMINFO,                             "" );
	g_weaponDisable =           Cvar_Get( "g_weaponDisable",           "0",                                    CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, "" );
	graphheight =               Cvar_Get( "graphheight",               "32",                                   CVAR_CHEAT,                                  "" );
	graphscale =                Cvar_Get( "graphscale",                "1",                                    CVAR_CHEAT,                                  "" );
	graphshift =                Cvar_Get( "graphshift",                "0",                                    CVAR_CHEAT,                                  "" );
	in_joystick =               Cvar_Get( "in_joystick",               "0",                                    CVAR_NONE,                                   "" );
	ja_guid =                   Cvar_Get( "ja_guid",                   "",                                     CVAR_USERINFO | CVAR_ROM,                    "Client GUID" );
	logfile =                   Cvar_Get( "logfile",                   "0",                                    CVAR_TEMP,                                   "" );
	m_forward =                 Cvar_Get( "m_forward",                 "0.25",                                 CVAR_ARCHIVE_ND,                             "" );
	m_pitch =                   Cvar_Get( "m_pitch",                   "0.022",                                CVAR_ARCHIVE_ND,                             "" );
	m_pitchVeh =                Cvar_Get( "m_pitchVeh",                "0.022",                                CVAR_ARCHIVE_ND,                             "" );
	m_side =                    Cvar_Get( "m_side",                    "0.25",                                 CVAR_ARCHIVE_ND,                             "" );
	m_yaw =                     Cvar_Get( "m_yaw",                     "0.022",                                CVAR_ARCHIVE_ND,                             "" );
	mapname =                   Cvar_Get( "mapname",                   "nomap",                                CVAR_SERVERINFO | CVAR_ROM,                  "" );
	model =                     Cvar_Get( "model",                     DEFAULT_MODEL "/default",               CVAR_USERINFO | CVAR_ARCHIVE,                "Player model" );
	name =                      Cvar_Get( "name",                      "Padawan",                              CVAR_USERINFO | CVAR_ARCHIVE_ND,             "Player name" );
	net_dropsim =               Cvar_Get( "net_dropsim",               "",                                     CVAR_TEMP,                                   "" );
	net_enabled =               Cvar_Get( "net_enabled",               "1",                                    CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	net_forcenonlocal =         Cvar_Get( "net_forcenonlocal",         "0",                                    CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	net_forcenonlocal =         Cvar_Get( "net_forcenonlocal",         "0",                                    CVAR_NONE,                                   "" );
	net_ip =                    Cvar_Get( "net_ip",                    "localhost",                            CVAR_LATCH,                                  "" );
	net_port =                  Cvar_Get( "net_port",                  XSTRING( PORT_SERVER ),                 CVAR_LATCH,                                  "" );
	net_qport =                 Cvar_Get( "net_qport",                 "0",                                    CVAR_INIT,                                   "" );
	net_socksEnabled =          Cvar_Get( "net_socksEnabled",          "0",                                    CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	net_socksPassword =         Cvar_Get( "net_socksPassword",         "",                                     CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	net_socksPort =             Cvar_Get( "net_socksPort",             "1080",                                 CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	net_socksServer =           Cvar_Get( "net_socksServer",           "",                                     CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	net_socksUsername =         Cvar_Get( "net_socksUsername",         "",                                     CVAR_LATCH | CVAR_ARCHIVE_ND,                "" );
	nextdemo =                  Cvar_Get( "nextdemo",                  "",                                     CVAR_INTERNAL,                               "" );
	nextmap =                   Cvar_Get( "nextmap",                   "",                                     CVAR_TEMP,                                   "" );
	password =                  Cvar_Get( "password",                  "",                                     CVAR_USERINFO,                               "Password to join server" );
	protocol =                  Cvar_Get( "protocol",                  XSTRING( PROTOCOL_VERSION ),            CVAR_SERVERINFO | CVAR_ROM,                  "" );
	r_debugSurface =            Cvar_Get( "r_debugSurface",            "0",                                    CVAR_NONE,                                   "" );
	r_debugSurfaceUpdate =      Cvar_Get( "r_debugSurfaceUpdate",      "1",                                    CVAR_NONE,                                   "" );
	r_fullscreen =              Cvar_Get( "r_fullscreen",              "0",                                    CVAR_NONE,                                   "" );
	r_inGameVideo =             Cvar_Get( "r_inGameVideo",             "1",                                    CVAR_ARCHIVE_ND,                             "" );
	rate =                      Cvar_Get( "rate",                      "25000",                                CVAR_USERINFO | CVAR_ARCHIVE,                "Data rate" );
	rconAddress =               Cvar_Get( "rconAddress",               "",                                     CVAR_NONE,                                   "Alternate server address to remotely access via rcon protocol" );
	rconPassword =              Cvar_Get( "rconPassword",              "",                                     CVAR_TEMP,                                   "" );
	rconPassword =              Cvar_Get( "rconPassword",              "",                                     CVAR_TEMP,                                   "Password for remote console access" );
	s_allowDynamicMusic =       Cvar_Get( "s_allowDynamicMusic",       "1",                                    CVAR_ARCHIVE_ND,                             "" );
	s_debugdynamic =            Cvar_Get( "s_debugdynamic",            "0",                                    CVAR_CHEAT,                                  "" );
	s_doppler =                 Cvar_Get( "s_doppler",                 "1",                                    CVAR_ARCHIVE_ND,                             "" );
	s_initsound =               Cvar_Get( "s_initsound",               "1",                                    CVAR_NONE,                                   "" );
	s_khz =                     Cvar_Get( "s_khz",                     "44",                                   CVAR_ARCHIVE | CVAR_LATCH,                   "" );
	s_language =                Cvar_Get( "s_language",                "english",                              CVAR_ARCHIVE | CVAR_NORESTART,               "Sound language" );
	s_mixahead =                Cvar_Get( "s_mixahead",                "0.2",                                  CVAR_ARCHIVE,                                "" );
	s_mixPreStep =              Cvar_Get( "s_mixPreStep",              "0.05",                                 CVAR_ARCHIVE,                                "" );
	s_mp3overhead =             Cvar_Get( "s_mp3overhead",             "0",                                    CVAR_ARCHIVE,                                "" );
	s_musicVolume =             Cvar_Get( "s_musicVolume",             "0.25",                                 CVAR_ARCHIVE,                                "Music Volume" );
	s_separation =              Cvar_Get( "s_separation",              "0.5",                                  CVAR_ARCHIVE,                                "" );
	s_show =                    Cvar_Get( "s_show",                    "0",                                    CVAR_CHEAT,                                  "" );
	s_soundpoolmegs =           Cvar_Get( "s_soundpoolmegs",           "25",                                   CVAR_ARCHIVE,                                "" );
	s_testsound =               Cvar_Get( "s_testsound",               "0",                                    CVAR_CHEAT,                                  "" );
	s_threshold1 =              Cvar_Get( "s_threshold1",              "0.5",                                  CVAR_NONE,                                   "" );
	s_threshold2 =              Cvar_Get( "s_threshold2",              "4.0",                                  CVAR_NONE,                                   "" );
	s_threshold3 =              Cvar_Get( "s_threshold3",              "7.0",                                  CVAR_NONE,                                   "" );
	s_threshold4 =              Cvar_Get( "s_threshold4",              "8.0",                                  CVAR_NONE,                                   "" );
	s_volume =                  Cvar_Get( "s_volume",                  "0.5",                                  CVAR_ARCHIVE,                                "Volume" );
	s_volumeVoice =             Cvar_Get( "s_volumeVoice",             "1.0",                                  CVAR_ARCHIVE,                                "Volume for voice channels" );
	saber1 =                    Cvar_Get( "saber1",                    DEFAULT_SABER,                          CVAR_USERINFO | CVAR_ARCHIVE,                "Player default right hand saber" );
	saber2 =                    Cvar_Get( "saber2",                    "none",                                 CVAR_USERINFO | CVAR_ARCHIVE,                "Player left hand saber" );
	scr_conspeed =              Cvar_Get( "scr_conspeed",              "3",                                    CVAR_NONE,                                   "Console open/close speed" );
	se_debug =                  Cvar_Get( "se_debug",                  "0",                                    CVAR_NONE,                                   "" );
	se_language =               Cvar_Get( "se_language",               "english",                              CVAR_ARCHIVE | CVAR_NORESTART,               "" );
	sensitivity =               Cvar_Get( "sensitivity",               "5",                                    CVAR_ARCHIVE,                                "Mouse sensitivity value" );
	sex =                       Cvar_Get( "sex",                       "male",                                 CVAR_USERINFO | CVAR_ARCHIVE,                "Player sex" );
	showdrop =                  Cvar_Get( "showdrop",                  "0",                                    CVAR_TEMP,                                   "" );
	showpackets =               Cvar_Get( "showpackets",               "0",                                    CVAR_TEMP,                                   "" );
	snaps =                     Cvar_Get( "snaps",                     "40",                                   CVAR_USERINFO | CVAR_ARCHIVE,                "Client snapshots per second" );
	sp_leet =                   Cvar_Get( "sp_leet",                   "0",                                    CVAR_ROM,                                    "" );
	sv_allowDownload =          Cvar_Get( "sv_allowDownload",          "0",                                    CVAR_SERVERINFO,                             "Allow clients to download mod files via UDP from the server" );
	sv_autoDemo =               Cvar_Get( "sv_autoDemo",               "0",                                    CVAR_ARCHIVE_ND | CVAR_SERVERINFO,           "Automatically take server-side demos" );
	sv_autoDemoBots =           Cvar_Get( "sv_autoDemoBots",           "0",                                    CVAR_ARCHIVE_ND,                             "Record server-side demos for bots" );
	sv_autoDemoMaxMaps =        Cvar_Get( "sv_autoDemoMaxMaps",        "0",                                    CVAR_ARCHIVE_ND,                             "" );
	sv_banFile =                Cvar_Get( "sv_banFile",                "serverbans.dat",                       CVAR_ARCHIVE,                                "File to use to store bans and exceptions" );
	sv_cheats =                 Cvar_Get( "sv_cheats",                 "1",                                    CVAR_ROM | CVAR_SYSTEMINFO,                  "Allow cheats on server if set to 1" );
	sv_cheats =                 Cvar_Get( "sv_cheats",                 "1",                                    CVAR_SYSTEMINFO | CVAR_ROM,                  "Allow cheats on server if set to 1" );
	sv_clientRate =             Cvar_Get( "sv_clientRate",             "50000",                                CVAR_ARCHIVE_ND,                             "" );
	sv_filterCommands =         Cvar_Get( "sv_filterCommands",         "1",                                    CVAR_ARCHIVE,                                "" );
	sv_floodProtect =           Cvar_Get( "sv_floodProtect",           "1",                                    CVAR_ARCHIVE | CVAR_SERVERINFO,              "Protect against flooding of server commands" );
	sv_floodProtectSlow =       Cvar_Get( "sv_floodProtectSlow",       "1",                                    CVAR_ARCHIVE | CVAR_SERVERINFO,              "Use original method of delaying commands with flood protection" );
	sv_fps =                    Cvar_Get( "sv_fps",                    "40",                                   CVAR_SERVERINFO,                             "Server frames per second" );
	sv_hostname =               Cvar_Get( "sv_hostname",               "*Jedi*",                               CVAR_SERVERINFO | CVAR_ARCHIVE,              "The name of the server that is displayed in the serverlist" );
	sv_keywords =               Cvar_Get( "sv_keywords",               "",                                     CVAR_SERVERINFO,                             "" );
	sv_killserver =             Cvar_Get( "sv_killserver",             "0",                                    CVAR_NONE,                                   "" );
	sv_lanForceRate =           Cvar_Get( "sv_lanForceRate",           "1",                                    CVAR_ARCHIVE_ND,                             "" );
	sv_legacyFixes =            Cvar_Get( "sv_legacyFixes",            "1",                                    CVAR_ARCHIVE,                                "" );
	sv_mapChecksum =            Cvar_Get( "sv_mapChecksum",            "",                                     CVAR_ROM,                                    "" );
	sv_master =                 Cvar_Get( "sv_master",                 MASTER_SERVER_NAME,                     CVAR_PROTECTED,                              "" );
	sv_maxclients =             Cvar_Get( "sv_maxclients",             "8",                                    CVAR_NONE,                                   "" );
	sv_maxclients =             Cvar_Get( "sv_maxclients",             "8",                                    CVAR_SERVERINFO | CVAR_LATCH,                "Max. connected clients" );
	sv_maxPing =                Cvar_Get( "sv_maxPing",                "0",                                    CVAR_ARCHIVE_ND | CVAR_SERVERINFO,           "" );
	sv_maxRate =                Cvar_Get( "sv_maxRate",                "0",                                    CVAR_ARCHIVE_ND | CVAR_SERVERINFO,           "Max bandwidth rate allowed on server. Use 0 for unlimited." );
	sv_minPing =                Cvar_Get( "sv_minPing",                "0",                                    CVAR_ARCHIVE_ND | CVAR_SERVERINFO,           "" );
	sv_minRate =                Cvar_Get( "sv_minRate",                "0",                                    CVAR_ARCHIVE_ND | CVAR_SERVERINFO,           "Min bandwidth rate allowed on server. Use 0 for unlimited." );
	sv_padPackets =             Cvar_Get( "sv_padPackets",             "0",                                    CVAR_NONE,                                   "" );
	sv_pakNames =               Cvar_Get( "sv_pakNames",               "",                                     CVAR_SYSTEMINFO | CVAR_ROM,                  "" );
	sv_paks =                   Cvar_Get( "sv_paks",                   "",                                     CVAR_SYSTEMINFO | CVAR_ROM,                  "" );
	sv_paused =                 Cvar_Get( "sv_paused",                 "0",                                    CVAR_ROM,                                    "" );
	sv_privateClients =         Cvar_Get( "sv_privateClients",         "0",                                    CVAR_SERVERINFO,                             "Number of reserved client slots available with password" );
	sv_privatePassword =        Cvar_Get( "sv_privatePassword",        "",                                     CVAR_TEMP,                                   "" );
	sv_pure =                   Cvar_Get( "sv_pure",                   "0",                                    CVAR_SYSTEMINFO,                             "Pure server" );
	sv_ratePolicy =             Cvar_Get( "sv_ratePolicy",             "1",                                    CVAR_ARCHIVE_ND,                             "Determines which policy of enforcement is used for client's \"rate\" cvar" );
	sv_reconnectlimit =         Cvar_Get( "sv_reconnectlimit",         "3",                                    CVAR_NONE,                                   "" );
	sv_referencedPakNames =     Cvar_Get( "sv_referencedPakNames",     "",                                     CVAR_SYSTEMINFO | CVAR_ROM,                  "" );
	sv_referencedPaks =         Cvar_Get( "sv_referencedPaks",         "",                                     CVAR_SYSTEMINFO | CVAR_ROM,                  "" );
	sv_running =                Cvar_Get( "sv_running",                "0",                                    CVAR_ROM,                                    "Is a server running?" );
	sv_serverid =               Cvar_Get( "sv_serverid",               "0",                                    CVAR_SYSTEMINFO | CVAR_ROM,                  "" );
	sv_showghoultraces =        Cvar_Get( "sv_showghoultraces",        "0",                                    CVAR_NONE,                                   "" );
	sv_showloss =               Cvar_Get( "sv_showloss",               "0",                                    CVAR_NONE,                                   "" );
	sv_snapsMax =               Cvar_Get( "sv_snapsMax",               "40",                                   CVAR_ARCHIVE_ND,                             "" ); // sv_snapsMin <=> sv_fps
	sv_snapsMin =               Cvar_Get( "sv_snapsMin",               "10",                                   CVAR_ARCHIVE_ND,                             "" ); // 1 <=> sv_snapsMax
	sv_snapsPolicy =            Cvar_Get( "sv_snapsPolicy",            "1",                                    CVAR_ARCHIVE_ND,                             "Determines which policy of enforcement is used for client's \"snaps\" cvar" );
	sv_timeout =                Cvar_Get( "sv_timeout",                "200",                                  CVAR_TEMP,                                   "" );
	sv_zombietime =             Cvar_Get( "sv_zombietime",             "2",                                    CVAR_TEMP,                                   "" );
	timedemo =                  Cvar_Get( "timedemo",                  "0",                                    CVAR_NONE,                                   "" );
	timedemo =                  Cvar_Get( "timedemo",                  "0",                                    CVAR_NONE,                                   "" );
	timegraph =                 Cvar_Get( "timegraph",                 "0",                                    CVAR_CHEAT,                                  "" );
	timelimit =                 Cvar_Get( "timelimit",                 "0",                                    CVAR_SERVERINFO,                             "" );
	timescale =                 Cvar_Get( "timescale",                 "1",                                    CVAR_CHEAT | CVAR_SYSTEMINFO,                "" );
	version =                   Cvar_Get( "version",                   JK_VERSION PLATFORM_STRING SOURCE_DATE, CVAR_ROM | CVAR_SERVERINFO,                  "" );

	#ifdef DEDICATED
		Cvar_CheckRange( dedicated, 1, 2, true );
	#endif
	Cvar_CheckRange( scr_conspeed, 1.0f, 100.0f, false );
	Cvar_CheckRange( sv_privateClients, 0, MAX_CLIENTS, true );
	Cvar_CheckRange( sv_ratePolicy, 1, 2, true );
	Cvar_CheckRange( sv_snapsPolicy, 0, 2, true );
}
