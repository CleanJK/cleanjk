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
	#define XCVAR_DEF( name, defVal, update, flags, announce ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, announce ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, announce ) { & name , #name , defVal , update , flags , announce },
#endif

#ifdef _DEBUG
XCVAR_DEF( bot_debugmessages,           "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( bot_getinthecarrr,           "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_nogoals,                 "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( g_disableServerG2,           "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_saberDebugPrint,           "0",           nullptr, CVAR_CHEAT,                                      false )
#endif

#ifdef DEBUG_SABER_BOX
XCVAR_DEF( g_saberDebugBox,             "0",           nullptr, CVAR_CHEAT,                                      false )
#endif

XCVAR_DEF( bg_fighterAltControl,        "0",           nullptr, CVAR_SYSTEMINFO,                                 true )
XCVAR_DEF( bg_showEvents,               "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( bot_attachments,             "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_camp,                    "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_enable,                  "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_developer,               "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( bot_forcepowers,             "1",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( bot_forgimmick,              "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( bot_honorableduelacceptance, "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( bot_minplayers,              "0",           nullptr, CVAR_SERVERINFO,                                 true )
XCVAR_DEF( bot_normgpath,               "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_pvstype,                 "1",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( bot_wp_clearweight,          "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_wp_distconnect,          "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_wp_edit,                 "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( bot_wp_info,                 "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( bot_wp_visconnect,           "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( capturelimit,                "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, true )
XCVAR_DEF( cjk_saberSpecialMoves,       "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE,                  true )
XCVAR_DEF( cjk_saberTweaks,             "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE,                  true )
XCVAR_DEF( cl_running,                  "",            nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( d_altRoutes,                 "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_asynchronousGroupAI,       "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_break,                     "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_JediAI,                    "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_noGroupAI,                 "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_noIntermissionWait,        "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( d_noroam,                    "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_patched,                   "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_perPlayerGhoul2,           "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( d_powerDuelPrint,            "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( d_projectileGhoul2Collision, "1",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( d_saberAlwaysBoxTrace,       "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( d_saberBoxTraceSize,         "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( d_saberCombat,               "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( d_saberGhoul2Collision,      "1",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( d_saberInterpolate,          "0",           nullptr, CVAR_CHEAT,                                      true )
XCVAR_DEF( d_saberKickTweak,            "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( d_saberSPStyleDamage,        "1",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( d_saberStanceDebug,          "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( dedicated,                   "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( developer,                   "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( dmflags,                     "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE,                  true )
XCVAR_DEF( duel_fraglimit,              "10",          nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, true )
XCVAR_DEF( fraglimit,                   "20",          nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, true )
XCVAR_DEF( g_adaptRespawn,              "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_allowDuelSuicide,          "1",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_allowHighPingDuelist,      "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_allowTeamVote,             "1",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_allowVote,                 "-1",          nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_antiFakePlayer,            "1",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_armBreakage,               "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_austrian,                  "0",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_autoMapCycle,              "0",           nullptr, CVAR_ARCHIVE | CVAR_NORESTART,                   true )
XCVAR_DEF( g_banIPs,                    "",            nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_botsFile,                  "",            nullptr, CVAR_INIT | CVAR_ROM,                            false )
XCVAR_DEF( g_charRestrictRGB,           "1",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_debugAlloc,                "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_debugDamage,               "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_debugMelee,                "0",           nullptr, CVAR_SERVERINFO,                                 true )
XCVAR_DEF( g_debugMove,                 "0",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_debugSaberLocks,           "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( g_debugServerSkel,           "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( g_dismember,                 "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_doWarmup,                  "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_duelWeaponDisable,         "1",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH,     true )
XCVAR_DEF( g_ff_objectives,             "0",           nullptr, CVAR_CHEAT | CVAR_NORESTART,                     true )
XCVAR_DEF( g_filterBan,                 "1",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_forceBasedTeams,           "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH,     false )
XCVAR_DEF( g_forceClientUpdateRate,     "250",         nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_forceDodge,                "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_forcePowerDisable,         "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH,     true )
XCVAR_DEF( g_forceRegenTime,            "200",         nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_forceRespawn,              "60",          nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_fraglimitVoteCorrection,   "1",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_friendlyFire,              "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_friendlySaber,             "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_g2TraceLod,                "3",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_gametype,                  "0",           nullptr, CVAR_SERVERINFO | CVAR_LATCH,                    false )
XCVAR_DEF( g_gravity,                   "800",         nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_inactivity,                "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_jediVmerc,                 "0",           nullptr, CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE,     true )
XCVAR_DEF( g_knockback,                 "1000",        nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_locationBasedDamage,       "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_log,                       "games.log",   nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_logClientInfo,             "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_logSync,                   "0",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_maxConnPerIP,              "3",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_maxForceRank,              "7",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH,     false )
XCVAR_DEF( g_maxGameClients,            "0",           nullptr, CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE,     false )
XCVAR_DEF( g_maxHolocronCarry,          "3",           nullptr, CVAR_LATCH,                                      false )
XCVAR_DEF( g_motd,                       "",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_needpass,                  "0",           nullptr, CVAR_SERVERINFO | CVAR_ROM,                      false )
XCVAR_DEF( g_noSpecMove,                "0",           nullptr, CVAR_SERVERINFO,                                 true )
XCVAR_DEF( g_password,                   "",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_powerDuelEndHealth,        "90",          nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_powerDuelStartHealth,      "150",         nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_privateDuel,               "1",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_randFix,                   "1",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_restarted,                 "0",           nullptr, CVAR_ROM,                                        false )
XCVAR_DEF( g_saberBladeFaces,           "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_saberDamageScale,          "1",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberDmgDelay_Idle,        "350",         nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberDmgDelay_Wound,       "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberDmgVelocityScale,     "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberLockFactor,           "2",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberLocking,              "1",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberLockRandomNess,       "2",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_saberRealisticCombat,      "0",           nullptr, CVAR_CHEAT,                                      false )
XCVAR_DEF( g_saberRestrictForce,        "0",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_saberTraceSaberFirst,      "0",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_saberWallDamageScale,      "0.4",         nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_securityLog,               "1",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_showDuelHealths,           "0",           nullptr, CVAR_SERVERINFO,                                 false )
XCVAR_DEF( g_smoothClients,             "1",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_spawnInvulnerability,      "3000",        nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_speed,                     "250",         nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_spSkill,                   "3",           nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_statLog,                   "0",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_statLogFile,               "statlog.log", nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_stepSlideFix,              "1",           nullptr, CVAR_SERVERINFO,                                 true )
XCVAR_DEF( g_synchronousClients,        "0",           nullptr, CVAR_SYSTEMINFO,                                 false )
XCVAR_DEF( g_teamAutoJoin,              "0",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_teamForceBalance,          "0",           nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_timeouttospec,             "70",          nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_userinfoValidate,          "25165823",    nullptr, CVAR_ARCHIVE,                                    false )
XCVAR_DEF( g_useWhileThrowing,          "1",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( g_voteDelay,                 "3000",        nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( g_warmup,                    "20",          nullptr, CVAR_ARCHIVE,                                    true )
XCVAR_DEF( g_weaponDisable,             "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH,     true )
XCVAR_DEF( g_weaponRespawn,             "5",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( gamedate,                    SOURCE_DATE,   nullptr, CVAR_ROM,                                        false )
XCVAR_DEF( gamename,                    GAMEVERSION,   nullptr, CVAR_SERVERINFO | CVAR_ROM,                      false )
XCVAR_DEF( mapname,                     "",            nullptr, CVAR_SERVERINFO | CVAR_ROM,                      false )
XCVAR_DEF( nextmap,                     "",            nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( pmove_fixed,                 "0",           nullptr, CVAR_SYSTEMINFO | CVAR_ARCHIVE,                  true )
XCVAR_DEF( pmove_float,                 "0",           nullptr, CVAR_SYSTEMINFO | CVAR_ARCHIVE,                  true )
XCVAR_DEF( pmove_msec,                  "8",           nullptr, CVAR_SYSTEMINFO | CVAR_ARCHIVE,                  true )
XCVAR_DEF( RMG,                         "0",           nullptr, CVAR_NONE,                                       true )
XCVAR_DEF( session,                     "",            nullptr, CVAR_INTERNAL | CVAR_ROM,                        false )
XCVAR_DEF( se_language,                 "",            nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( sv_cheats,                   "1",           nullptr, CVAR_NONE,                                       false )
XCVAR_DEF( sv_fps,                      "40",          nullptr, CVAR_ARCHIVE | CVAR_SERVERINFO,                  true )
XCVAR_DEF( sv_mapChecksum,              "",            nullptr, CVAR_ROM,                                        false )
XCVAR_DEF( sv_maxclients,               "8",           nullptr, CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE,     false )
XCVAR_DEF( timelimit,                   "0",           nullptr, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, true )
XCVAR_DEF( timescale,                   "1",           nullptr, CVAR_CHEAT | CVAR_SYSTEMINFO,                    true )

#undef XCVAR_DEF
