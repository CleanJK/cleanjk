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

// ======================================================================
// DEFINE
// ======================================================================

#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags ) { & name , #name , defVal , update , flags },
#endif

XCVAR_DEF( bg_fighterAltControl,             "0",                      nullptr,                 CVAR_SYSTEMINFO )
XCVAR_DEF( bg_showEvents,                    "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( broadsword,                       "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_animBlend,                     "1",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_animSpeed,                     "1",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_auraShell,                     "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_autoSwitch,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_bobPitch,                      "0.002",                  nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_bobRoll,                       "0.002",                  nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_bobUp,                         "0.005",                  nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_cameraOrbit,                   "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_cameraOrbitDelay,              "50",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_centerTime,                    "3",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_chatBeep,                      "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_chatBox,                       "10000",                  nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_chatBoxHeight,                 "350",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairHealth,               "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairSize,                 "24",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairX,                    "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairY,                    "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_currentSelectedPlayer,         "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_debugAnim,                     "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_debugEvents,                   "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_debugGun,                      "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_debugPosition,                 "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_debugSaber,                    "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_deferPlayers,                  "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_dismember,                     "2",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_draw2D,                        "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_draw3DIcons,                   "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawAmmoWarning,               "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawCrosshair,                 "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawCrosshairNames,            "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawEnemyInfo,                 "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawFPS,                       "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawFriend,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawGun,                       "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawIcons,                     "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawRadar,                     "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawRewards,                   "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawScores,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawSnapshot,                  "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawStatus,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_drawTeamOverlay,               "1",                      CG_TeamOverlayChange, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawTimer,                     "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_duelHeadAngles,                "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_dynamicCrosshair,              "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_dynamicCrosshairPrecision,     "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_errorDecay,                    "100",                    nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_fallingBob,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_footsteps,                     "3",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_forceModel,                    "0",                      CG_ForceModelChange,  CVAR_ARCHIVE )
XCVAR_DEF( cg_fov,                           "90",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_fovAspectAdjust,               "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_fovViewmodel,                  "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_fovViewmodelAdjust,            "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_g2TraceLod,                    "2",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_ghoul2Marks,                   "16",                     nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_gunAlign,                      "0 0 0",                  CVU_GunAlign,         CVAR_ARCHIVE )
XCVAR_DEF( cg_gunBob,                        "0.005 0.01 0.005",       CVU_GunBob,           CVAR_ARCHIVE )
XCVAR_DEF( cg_gunBobEnable,                  "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_gunIdleDrift,                  "0.01 0.01 0.01 0.001",   CVU_GunDrift,         CVAR_ARCHIVE )
XCVAR_DEF( cg_gunIdleDriftEnable,            "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_gunMomentumDamp,               "0.666666",               nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_gunMomentumEnable,             "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_gunMomentumFall,               "0.5",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_gunMomentumInterval,           "2.5",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_hudFiles,                      DEFAULT_HUDSET,		   nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_instantDuck,                   "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_jumpSounds,                    "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_lagometer,                     "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_marks,                         "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_noPlayerAnims,                 "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_noPredict,                     "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_noProjectileTrail,             "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_noTaunt,                       "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_oldPainSounds,                 "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_predictItems,                  "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_renderToTextureFX,             "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_repeaterOrb,                   "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_runPitch,                      "0.002",                  nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_runRoll,                       "0.005",                  nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_saberClientVisualCompensation, "1",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_saberContact,                  "1",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_saberDynamicMarks,             "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_saberDynamicMarkTime,          "60000",                  nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_saberModelTraceEffect,         "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_saberTrail,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_scoreboardBots,                "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_scorePlums,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_shadows,                       "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_showMiss,                      "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_simpleItems,                   "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_smoothCamera,                  "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_smoothClients,                 "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_snapshotTimeout,               "10",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_speedTrail,                    "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_stats,                         "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_stereoSeparation,              "0.4",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_teamChatBeep,                  "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_teamChatsOnly,                 "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPerson,                   "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonAlpha,              "1.0",                    nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_thirdPersonAngle,              "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonCameraDamp,         "0.3",                    nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_thirdPersonHorzOffset,         "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( cg_thirdPersonPitchOffset,        "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonRange,              "80",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonSpecialCam,         "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_thirdPersonTargetDamp,         "0.5",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonVertOffset,         "16",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_timescaleFadeEnd,              "1",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_timescaleFadeSpeed,            "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( cg_viewsize,                      "100",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cg_weaponBob,                     "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cjk_preciseFonts,                 "1",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( cl_motdString,                    "",                       nullptr,                 CVAR_NONE )
XCVAR_DEF( cl_paused,                        "0",                      nullptr,                 CVAR_ROM )
XCVAR_DEF( com_buildScript,                  "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( com_cameraMode,                   "0",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( com_maxFPS,                       "125",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( developer,                        "0",                      nullptr,                 CVAR_NONE )
XCVAR_DEF( forcepowers,                      DEFAULT_FORCEPOWERS,      nullptr,                 CVAR_USERINFO|CVAR_ARCHIVE )
XCVAR_DEF( g_synchronousClients,             "0",                      nullptr,                 CVAR_SYSTEMINFO )
XCVAR_DEF( model,                            DEFAULT_MODEL "/default", nullptr,                 CVAR_USERINFO|CVAR_ARCHIVE )
XCVAR_DEF( pmove_fixed,                      "0",                      nullptr,                 CVAR_SYSTEMINFO )
XCVAR_DEF( pmove_float,                      "0",                      nullptr,                 CVAR_SYSTEMINFO )
XCVAR_DEF( pmove_msec,                       "8",                      nullptr,                 CVAR_SYSTEMINFO )
XCVAR_DEF( r_autoMap,                        "0",                      nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapH,                       "128",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapW,                       "128",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapX,                       "496",                    nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapY,                       "32",                     nullptr,                 CVAR_ARCHIVE )
XCVAR_DEF( sv_running,                       "0",                      CG_SVRunningChange,   CVAR_ROM )
XCVAR_DEF( teamoverlay,                      "0",                      nullptr,                 CVAR_ROM|CVAR_USERINFO )
XCVAR_DEF( timescale,                        "1",                      nullptr,                 CVAR_CHEAT )
XCVAR_DEF( ui_about_botminplayers,           "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_capturelimit,            "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_dmflags,                 "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_duellimit,               "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_fraglimit,               "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_gametype,                "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_hostname,                "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_mapname,                 "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_maxclients,              "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_needpass,                "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_about_timelimit,               "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_myteam,                        "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c0_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c1_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c2_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c3_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c4_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c5_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_cnt,                       "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c0_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c1_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c2_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c3_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c4_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c5_cnt,                    "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_cnt,                       "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )
XCVAR_DEF( ui_tm3_cnt,                       "0",                      nullptr,                 CVAR_ROM|CVAR_INTERNAL )

#undef XCVAR_DEF