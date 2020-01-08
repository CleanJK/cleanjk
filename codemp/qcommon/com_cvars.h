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

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

#ifdef _DEBUG
extern cvar_t *fx_freeze;
#endif // _DEBUG

#ifndef _WIN32
extern cvar_t *com_ansiColor;
#endif // _WIN32

#ifdef G2_PERFORMANCE_ANALYSIS
extern cvar_t *com_G2Report;
#endif // G2_PERFORMANCE_ANALYSIS

#ifdef MACOS_X
extern cvar_t *fs_apppath;                    // Also search the .app bundle for .pk3 files
#endif // MACOS_X

#ifdef USE_OPENAL
extern cvar_t *s_useOpenAL;
#endif // USE_OPENAL

extern cvar_t *activeAction;
extern cvar_t *bot_aasoptimize;
extern cvar_t *bot_challenge;
extern cvar_t *bot_debug;
extern cvar_t *bot_enable;
extern cvar_t *bot_fastchat;
extern cvar_t *bot_forceclustering;
extern cvar_t *bot_forcereachability;
extern cvar_t *bot_forcewrite;
extern cvar_t *bot_grapple;
extern cvar_t *bot_groundonly;
extern cvar_t *bot_groundonly;
extern cvar_t *bot_highlightarea;
extern cvar_t *bot_interbreedbots;
extern cvar_t *bot_interbreedchar;
extern cvar_t *bot_interbreedcycle;
extern cvar_t *bot_interbreedwrite;
extern cvar_t *bot_maxdebugpolys;
extern cvar_t *bot_minplayers;
extern cvar_t *bot_nochat;
extern cvar_t *bot_pause;
extern cvar_t *bot_reachability;
extern cvar_t *bot_reloadcharacters;
extern cvar_t *bot_report;
extern cvar_t *bot_rocketjump;
extern cvar_t *bot_saveroutingcache;
extern cvar_t *bot_testclusters;
extern cvar_t *bot_testichat;
extern cvar_t *bot_testrchat;
extern cvar_t *bot_testsolid;
extern cvar_t *bot_thinktime;
extern cvar_t *bot_visualizejumppads;
extern cvar_t *capturelimit;
extern cvar_t *cg_autoswitch;
extern cvar_t *cg_drawRadar;
extern cvar_t *cg_predictItems;
extern cvar_t *cg_viewsize;
extern cvar_t *char_color_blue;
extern cvar_t *char_color_green;
extern cvar_t *char_color_red;
extern cvar_t *cl_allowAltEnter;
extern cvar_t *cl_allowDownload;
extern cvar_t *cl_anglespeedkey;
extern cvar_t *cl_autolodscale;
extern cvar_t *cl_avi2GBLimit;
extern cvar_t *cl_aviFrameRate;
extern cvar_t *cl_aviMotionJpeg;
extern cvar_t *cl_consoleKeys;
extern cvar_t *cl_consoleUseScanCode;
extern cvar_t *cl_conXOffset;
extern cvar_t *cl_debugMove;
extern cvar_t *cl_drawRecording;
extern cvar_t *cl_enableGuid;
extern cvar_t *cl_forceavidemo;
extern cvar_t *cl_framerate;
extern cvar_t *cl_freelook;
extern cvar_t *cl_freezeDemo;
extern cvar_t *cl_guidServerUniq;
extern cvar_t *cl_lanForcePackets;
extern cvar_t *cl_maxpackets;
extern cvar_t *cl_maxPing;
extern cvar_t *cl_motd;
extern cvar_t *cl_motdServer1;
extern cvar_t *cl_motdServer2;
extern cvar_t *cl_motdString;
extern cvar_t *cl_mouseAccel;
extern cvar_t *cl_mouseAccelOffset;
extern cvar_t *cl_mouseAccelStyle;
extern cvar_t *cl_nodelta;
extern cvar_t *cl_noprint;
extern cvar_t *cl_packetdup;
extern cvar_t *cl_paused;
extern cvar_t *cl_pitchspeed;
extern cvar_t *cl_renderer;
extern cvar_t *cl_recordSPDemo;
extern cvar_t *cl_run;
extern cvar_t *cl_running;
extern cvar_t *cl_serverStatusResendTime;
extern cvar_t *cl_showMouseRate;
extern cvar_t *cl_shownet;
extern cvar_t *cl_showSend;
extern cvar_t *cl_showTimeDelta;
extern cvar_t *cl_timeNudge;
extern cvar_t *cl_timeout;
extern cvar_t *cl_yawspeed;
extern cvar_t *cm_debugSize;
extern cvar_t *cm_extraVerbose;
extern cvar_t *cm_noAreas;
extern cvar_t *cm_noCurves;
extern cvar_t *cm_noMapCache;
extern cvar_t *cm_playerCurveClip;
extern cvar_t *color1;
extern cvar_t *color2;
extern cvar_t *com_affinity;
extern cvar_t *com_buildScript;
extern cvar_t *com_busyWait;
extern cvar_t *com_cameraMode;
extern cvar_t *com_journal;
extern cvar_t *com_showtrace;
extern cvar_t *com_speeds;
extern cvar_t *com_validateZone;
extern cvar_t *con_autoclear;
extern cvar_t *con_notifytime;
extern cvar_t *con_opacity;
extern cvar_t *d_saberStanceDebug;
extern cvar_t *debuggraph;
extern cvar_t *dedicated;
extern cvar_t *developer;
extern cvar_t *dmflags;
extern cvar_t *duel_fraglimit;
extern cvar_t *fixedtime;
extern cvar_t *forcepowers;
extern cvar_t *fraglimit;
extern cvar_t *fs_basegame;
extern cvar_t *fs_basepath;
extern cvar_t *fs_cdpath;
extern cvar_t *fs_copyfiles;
extern cvar_t *fs_debug;
extern cvar_t *fs_dirbeforepak;
extern cvar_t *fs_game;
extern cvar_t *fs_homepath;
extern cvar_t *fx_countScale;
extern cvar_t *fx_debug;
extern cvar_t *fx_nearCull;
extern cvar_t *g_duelWeaponDisable;
extern cvar_t *g_forceBasedTeams;
extern cvar_t *g_forcePowerDisable;
extern cvar_t *g_gametype;
extern cvar_t *g_jediVmerc;
extern cvar_t *g_maxForceRank;
extern cvar_t *g_maxHolocronCarry;
extern cvar_t *g_needpass;
extern cvar_t *g_privateDuel;
extern cvar_t *g_saberLocking;
extern cvar_t *g_synchronousClients;
extern cvar_t *g_weaponDisable;
extern cvar_t *graphheight;
extern cvar_t *graphscale;
extern cvar_t *graphshift;
extern cvar_t *in_joystick;
extern cvar_t *ja_guid;
extern cvar_t *logfile;
extern cvar_t *m_filter;
extern cvar_t *m_forward;
extern cvar_t *m_pitch;
extern cvar_t *m_pitchVeh;
extern cvar_t *m_side;
extern cvar_t *m_yaw;
extern cvar_t *mapname;
extern cvar_t *model;
extern cvar_t *name;
extern cvar_t *net_dropsim;
extern cvar_t *net_enabled;
extern cvar_t *net_forcenonlocal;
extern cvar_t *net_ip;
extern cvar_t *net_port;
extern cvar_t *net_qport;
extern cvar_t *net_socksEnabled;
extern cvar_t *net_socksPassword;
extern cvar_t *net_socksPort;
extern cvar_t *net_socksServer;
extern cvar_t *net_socksUsername;
extern cvar_t *nextdemo;
extern cvar_t *nextmap;
extern cvar_t *password;
extern cvar_t *protocol;
extern cvar_t *r_debugSurface;
extern cvar_t *r_debugSurfaceUpdate;
extern cvar_t *r_fullscreen;
extern cvar_t *r_inGameVideo;
extern cvar_t *rate;
extern cvar_t *rconAddress;
extern cvar_t *rconPassword;
extern cvar_t *s_allowDynamicMusic;
extern cvar_t *s_debugdynamic;
extern cvar_t *s_doppler;
extern cvar_t *s_initsound;
extern cvar_t *s_khz;
extern cvar_t *s_language;
extern cvar_t *s_mixahead;
extern cvar_t *s_mixPreStep;
extern cvar_t *s_mp3overhead;
extern cvar_t *s_musicVolume;
extern cvar_t *s_separation;
extern cvar_t *s_show;
extern cvar_t *s_soundpoolmegs;
extern cvar_t *s_testsound;
extern cvar_t *s_threshold1;
extern cvar_t *s_threshold2;
extern cvar_t *s_threshold3;
extern cvar_t *s_threshold4;
extern cvar_t *s_volume;
extern cvar_t *s_volumeVoice;
extern cvar_t *saber1;
extern cvar_t *saber2;
extern cvar_t *scr_conspeed;
extern cvar_t *se_debug;
extern cvar_t *se_language;
extern cvar_t *sensitivity;
extern cvar_t *sex;
extern cvar_t *showdrop;
extern cvar_t *showpackets;
extern cvar_t *snaps;
extern cvar_t *sp_leet;
extern cvar_t *sv_allowDownload;
extern cvar_t *sv_autoDemo;
extern cvar_t *sv_autoDemoBots;
extern cvar_t *sv_autoDemoMaxMaps;
extern cvar_t *sv_banFile;
extern cvar_t *sv_cheats;
extern cvar_t *sv_cheats;
extern cvar_t *sv_clientRate;
extern cvar_t *sv_filterCommands;
extern cvar_t *sv_floodProtect;
extern cvar_t *sv_floodProtectSlow;
extern cvar_t *sv_fps;
extern cvar_t *sv_hostname;
extern cvar_t *sv_keywords;
extern cvar_t *sv_killserver;
extern cvar_t *sv_lanForceRate;
extern cvar_t *sv_legacyFixes;
extern cvar_t *sv_mapChecksum;
extern cvar_t *sv_master;
extern cvar_t *sv_maxclients;
extern cvar_t *sv_maxclients;
extern cvar_t *sv_maxPing;
extern cvar_t *sv_maxRate;
extern cvar_t *sv_minPing;
extern cvar_t *sv_minRate;
extern cvar_t *sv_padPackets;
extern cvar_t *sv_pakNames;
extern cvar_t *sv_paks;
extern cvar_t *sv_paused;
extern cvar_t *sv_privateClients;
extern cvar_t *sv_privatePassword;
extern cvar_t *sv_pure;
extern cvar_t *sv_ratePolicy;
extern cvar_t *sv_reconnectlimit;
extern cvar_t *sv_referencedPakNames;
extern cvar_t *sv_referencedPaks;
extern cvar_t *sv_running;
extern cvar_t *sv_serverid;
extern cvar_t *sv_showghoultraces;
extern cvar_t *sv_showloss;
extern cvar_t *sv_snapsMax;
extern cvar_t *sv_snapsMin;
extern cvar_t *sv_snapsPolicy;
extern cvar_t *sv_timeout;
extern cvar_t *sv_zombietime;
extern cvar_t *timedemo;
extern cvar_t *timegraph;
extern cvar_t *timelimit;
extern cvar_t *timescale;
extern cvar_t *version;

// ======================================================================
// FUNCTION
// ======================================================================

void Com_InitCvars( void );
