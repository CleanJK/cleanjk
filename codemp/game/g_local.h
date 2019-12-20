/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2005 - 2015, ioquake3 contributors
Copyright (C) 2013 - 2015, OpenJK contributors

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

// g_local.h -- local definitions for game module

#include "qcommon/q_shared.h"
#include "game/bg_public.h"
#include "game/g_public.h"
#include "game/b_public.h"

// conditional compilation
#ifndef FINAL_BUILD
#define DEBUG_SABER_BOX
#endif

// macros
#define FOFS(x) offsetof(gentity_t, x)

// substitutions
#ifndef INFINITE
#define INFINITE                    1000000
#endif
#define BODY_QUEUE_SIZE            8
#define DEFAULT_BOTFILE            "botfiles/bots.txt"
#define EC                         "\x19"      // make sure this matches game/match.h for botlibs
#define FOLLOW_ACTIVE1             -1
#define FOLLOW_ACTIVE2             -2
#define GAMEVERSION                "CleanJK"
#define INTERMISSION_DELAY_TIME    1000
#define MAX_FILEPATH               144
#define MAX_G_SHARED_BUFFER_SIZE   8192
#define MAX_NETNAME                36
#define MAX_REFNAME                32
#define MAX_VOTE_COUNT             3
#define REWARD_SPRITE_TIME         2000
#define SECURITY_LOG               "security.log"
#define SP_PODIUM_MODEL            "models/mapobjects/podium/podium4.md3"

#define FRAMETIME                  100         // msec
#define START_TIME_LINK_ENTS       FRAMETIME*1 // time-delay after map start at which all ents have been spawned, so can link them
#define START_TIME_FIND_LINKS      FRAMETIME*2 // time-delay after map start at which you can find linked entities
#define START_TIME_MOVERS_SPAWNED  FRAMETIME*2 // time-delay after map start at which all movers should be spawned
#define START_TIME_REMOVE_ENTS     FRAMETIME*3 // time-delay after map start to remove temporary ents
#define START_TIME_NAV_CALC        FRAMETIME*4 // time-delay after map start to connect waypoints and calc routes
#define START_TIME_FIND_WAYPOINT   FRAMETIME*5 // time-delay after map start after which it's okay to try to find your best waypoint

// damage flags
enum damageFlag_e : int32_t {
	DAMAGE_NORMAL =               0x00000000, // No flags set.
	DAMAGE_RADIUS =               0x00000001, // damage was indirect
	DAMAGE_NO_ARMOR =             0x00000002, // armour does not protect from this damage
	DAMAGE_NO_KNOCKBACK =         0x00000004, // do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION =        0x00000008, // armor, shields, invulnerability, and godmode have no effect
	DAMAGE_NO_TEAM_PROTECTION =   0x00000010, // armor, shields, invulnerability, and godmode have no effect
	DAMAGE_EXTRA_KNOCKBACK =      0x00000040, // add extra knockback to this damage
	DAMAGE_DEATH_KNOCKBACK =      0x00000080, // only does knockback on death of target
	DAMAGE_IGNORE_TEAM =          0x00000100, // damage is always done, regardless of teams
	DAMAGE_NO_DAMAGE =            0x00000200, // do no actual damage but react as if damage was taken
	DAMAGE_HALF_ABSORB =          0x00000400, // half shields, half health
	DAMAGE_HALF_ARMOR_REDUCTION = 0x00000800, // This damage doesn't whittle down armor as efficiently.
	DAMAGE_HEAVY_WEAP_CLASS =     0x00001000, // Heavy damage
	DAMAGE_NO_HIT_LOC =           0x00002000, // No hit location
	DAMAGE_NO_SELF_PROTECTION =   0x00004000, // Dont apply half damage to self attacks
	DAMAGE_NO_DISMEMBER =         0x00008000, // Dont do dismemberment
	DAMAGE_SABER_KNOCKBACK1 =     0x00010000, // Check the attacker's first saber for a knockbackScale
	DAMAGE_SABER_KNOCKBACK2 =     0x00020000, // Check the attacker's second saber for a knockbackScale
	DAMAGE_SABER_KNOCKBACK1_B2 =  0x00040000, // Check the attacker's first saber for a knockbackScale2
	DAMAGE_SABER_KNOCKBACK2_B2 =  0x00080000, // Check the attacker's second saber for a knockbackScale2
};

enum gentityFlag_e : int32_t {
	FL_GODMODE =                0x00000010,
	FL_NOTARGET =               0x00000020,
	FL_TEAMSLAVE =              0x00000400, // not the first on the team
	FL_NO_KNOCKBACK =           0x00000800,
	FL_DROPPED_ITEM =           0x00001000,
	FL_NO_BOTS =                0x00002000, // spawn point not for bot use
	FL_NO_HUMANS =              0x00004000, // spawn point just for bots
	FL_FORCE_GESTURE =          0x00008000, // force gesture on client
	FL_INACTIVE =               0x00010000, // inactive
	FL_NAVGOAL =                0x00020000, // for npc nav stuff
	FL_DONT_SHOOT =             0x00040000,
	FL_SHIELDED =               0x00080000,
	FL_UNDYING =                0x00100000, // takes damage down to 1, but never dies
	FL_BOUNCE =                 0x00100000, // for missiles
	FL_BOUNCE_HALF =            0x00200000, // for missiles
	FL_BOUNCE_SHRAPNEL =        0x00400000, // special shrapnel flag
	FL_VEH_BOARDING =           0x00800000, // special shrapnel flag
	FL_DMG_BY_SABER_ONLY =      0x01000000, // only take dmg from saber
	FL_DMG_BY_HEAVY_WEAP_ONLY = 0x02000000, // only take dmg from explosives
	FL_BBRUSH =                 0x04000000, // I am a breakable brush
};

enum spawnFlags_e : int32_t {
	SPF_NONE,
	SPF_BUTTON_USABLE,
	SPF_BUTTON_FPUSHABLE,
};

// movers are things like doors, plats, buttons, etc
enum moverState_t : int32_t {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_1TO2,
	MOVER_2TO1,
};

enum hitLocation_t : int32_t {
	HL_NONE = 0,
	HL_FOOT_RT,
	HL_FOOT_LT,
	HL_LEG_RT,
	HL_LEG_LT,
	HL_WAIST,
	HL_BACK_RT,
	HL_BACK_LT,
	HL_BACK,
	HL_CHEST_RT,
	HL_CHEST_LT,
	HL_CHEST,
	HL_ARM_RT,
	HL_ARM_LT,
	HL_HAND_RT,
	HL_HAND_LT,
	HL_HEAD,
	HL_GENERIC1,
	HL_GENERIC2,
	HL_GENERIC3,
	HL_GENERIC4,
	HL_GENERIC5,
	HL_GENERIC6,
	HL_MAX,
};

enum clientConnected_t : int32_t {
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED,
};

enum spectatorState_t : int32_t {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD,
};

enum playerTeamStateState_t : int32_t {
	TEAM_BEGIN,  // Beginning a team game, spawn at base
	TEAM_ACTIVE, // Now actively playing
};

// userinfo validation bitflags
// default is all except extended ascii
// numUserinfoFields + USERINFO_VALIDATION_MAX should not exceed 31
enum userinfoValidationBits_t : int32_t {
	// validation & (1<<(numUserinfoFields+USERINFO_VALIDATION_BLAH))
	USERINFO_VALIDATION_SIZE=0,
	USERINFO_VALIDATION_SLASH,
	USERINFO_VALIDATION_EXTASCII,
	USERINFO_VALIDATION_CONTROLCHARS,
	USERINFO_VALIDATION_MAX,
};

enum votedFlags : int32_t {
	PSG_VOTED =     0x00000001, // already cast a vote
	PSG_TEAMVOTED = 0x00000002, // already cast a team vote
};

union sharedBuffer_t {
	char                         raw[MAX_G_SHARED_BUFFER_SIZE];
	T_G_ICARUS_PLAYSOUND         playSound;
	T_G_ICARUS_SET               set;
	T_G_ICARUS_LERP2POS          lerp2Pos;
	T_G_ICARUS_LERP2ORIGIN       lerp2Origin;
	T_G_ICARUS_LERP2ANGLES       lerp2Angles;
	T_G_ICARUS_GETTAG            getTag;
	T_G_ICARUS_LERP2START        lerp2Start;
	T_G_ICARUS_LERP2END          lerp2End;
	T_G_ICARUS_USE               use;
	T_G_ICARUS_KILL              kill;
	T_G_ICARUS_REMOVE            remove;
	T_G_ICARUS_PLAY              play;
	T_G_ICARUS_GETFLOAT          getFloat;
	T_G_ICARUS_GETVECTOR         getVector;
	T_G_ICARUS_GETSTRING         getString;
	T_G_ICARUS_SOUNDINDEX        soundIndex;
	T_G_ICARUS_GETSETIDFORSTRING getSetIDForString;
};

struct gentity_t {

	// - - - - - - - - - - - - - - - -

	//CJKFIXME: bgentity_t substruct
	// rww - entstate must be first, to correspond with the bg shared entity structure
	entityState_t    s;                            // communicated by server to clients
	playerState_t    *playerState;                 // ptr to playerstate if applicable (for bg ents)
	void             *ghoul2;                      // g2 instance
	int              localAnimIndex;               // index locally (game/cgame) to anim data for this skel
	vec3_t           modelScale;                   // needed for g2 collision
	// From here up must be the same as centity_t/bgEntity_t

	// - - - - - - - - - - - - - - - -

	entityShared_t   r;                           // shared by both the server system and game
	int              taskID[NUM_TIDS];
	parms_t          *parms;
	char             *behaviorSet[NUM_BSETS];
	char             *script_targetname;
	int              delayScriptTime;
	char             *fullName;
	char             *targetname;
	char             *classname;                  // set in QuakeEd
	int              waypoint;                    // Set once per frame, if you've moved, and if someone asks
	int              lastWaypoint;                // To make sure you don't double-back
	int              lastValidWaypoint;           // ALWAYS valid -used for tracking someone you lost
	int              noWaypointTime;              // Debouncer - so don't keep checking every waypoint in existance every frame that you can't find one
	int              combatPoint;
	int              failedWaypoints[MAX_FAILED_NODES];
	int              failedWaypointCheckTime;
	int              next_roff_time;              // rww - npc's need to know when they're getting roff'd
	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER EXPECTS THE FIELDS IN THAT ORDER! see svClient_t

	// - - - - - - - - - - - - - - - -

	struct gclient_t *client;                       // NULL if not a client
	qboolean         noLumbar;                      // see note in cg_local.h
	qboolean         inuse;
	int              lockCount;                     // used by NPCs
	int              spawnflags;                    // set in QuakeEd
	int              teamnodmg;                     // damage will be ignored if it comes from this team
	char             *roffname;                     // set in QuakeEd
	char             *rofftarget;                   // set in QuakeEd
	char             *healingclass;                 // set in quakeed
	char             *healingsound;                 // set in quakeed
	int              healingrate;                   // set in quakeed
	int              healingDebounce;               // debounce for generic object healing shiz
	char             *ownername;
	int              objective;
	int              side;
	int              passThroughNum;                // set to index to pass through (+1) for missiles
	int              aimDebounceTime;
	int              painDebounceTime;
	int              attackDebounceTime;
	int              alliedTeam;                    // only useable by this team, never target this team
	int              roffid;                        // if roffname != NULL then set on spawn
	qboolean         neverFree;                     // if true, FreeEntity will only unlink. bodyque uses this
	int              flags;                         // FL_* variables
	char             *model;
	char             *model2;
	int              freetime;                      // level.time when the object was freed
	int              eventTime;                     // events will be cleared EVENT_VALID_MSEC after set
	qboolean         freeAfterEvent;
	qboolean         unlinkAfterEvent;
	qboolean         physicsObject;                 // if true, it can be pushed by movers and fall off edges. all game items are physicsObjects,
	float            physicsBounce;                 // 1.0 = continuous bounce, 0.0 = no bounce
	int              clipmask;                      // brushes with this content value will be collided against when moving.  items and corpses do not collide against players, for instance
	moverState_t     moverState;
	int              soundPos1;
	int              sound1to2;
	int              sound2to1;
	int              soundPos2;
	int              soundLoop;
	gentity_t        *parent;
	gentity_t        *nextTrain;
	gentity_t        *prevTrain;
	vec3_t           pos1;
	vec3_t           pos2;
	vec3_t           pos3;
	char             *message;
	int              timestamp;                     // body queue sinking, etc
	float            angle;                         // set in editor, -1 = up, -2 = down
	char             *target;
	char             *target2;
	char             *target3;                      // For multiple targets, not used for firing/triggering/using, though, only for path branches
	char             *target4;                      // For multiple targets, not used for firing/triggering/using, though, only for path branches
	char             *target5;                      // mainly added for siege items
	char             *target6;                      // mainly added for siege items
	char             *team;
	char             *targetShaderName;
	char             *targetShaderNewName;
	gentity_t        *target_ent;
	char             *closetarget;
	char             *opentarget;
	char             *paintarget;
	char             *goaltarget;
	float            radius;
	int              maxHealth;                     // used as a base for crosshair health display
	float            speed;
	vec3_t           movedir;
	float            mass;
	int              setTime;
	int              nextthink;
	void             (*think)( gentity_t *self );
	void             (*reached)( gentity_t *self ); // movers call this when hitting endpoint
	void             (*blocked)( gentity_t *self, gentity_t *other );
	void             (*touch)( gentity_t *self, gentity_t *other, trace_t *trace );
	void             (*use)( gentity_t *self, gentity_t *other, gentity_t *activator );
	void             (*pain)( gentity_t *self, gentity_t *attacker, int damage );
	void             (*die)( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );
	int              pain_debounce_time;
	int              fly_sound_debounce_time;       // wind tunnel
	int              last_move_time;
	int              health;
	qboolean         takedamage;
	material_t       material;
	int              damage;
	int              dflags;
	int              splashDamage;                  // quad will increase this without increasing radius
	int              splashRadius;
	int              methodOfDeath;
	int              splashMethodOfDeath;
	int              locationDamage[HL_MAX];        // Damage accumulated on different body locations
	int              count;
	int              bounceCount;
	qboolean         alt_fire;
	gentity_t        *chain;
	gentity_t        *enemy;
	gentity_t        *lastEnemy;
	gentity_t        *activator;
	gentity_t        *teamchain;                    // next entity in team
	gentity_t        *teammaster;                   // master of the team
	int              watertype;
	int              waterlevel;
	int              noise_index;
	float            wait;
	float            random;
	int              delay;
	int              genericValue1;
	int              genericValue2;
	int              genericValue3;
	int              genericValue4;
	int              genericValue5;
	int              genericValue6;
	int              genericValue7;
	int              genericValue8;
	int              genericValue9;
	int              genericValue10;
	int              genericValue11;
	int              genericValue12;
	int              genericValue13;
	int              genericValue14;
	int              genericValue15;
	char             *soundSet;
	qboolean         isSaberEntity;
	vec3_t           epVelocity;
	float            epGravFactor;
	gitem_t          *item;                         // for bonus items
};

struct playerTeamState_t {
	playerTeamStateState_t state;
	int                    location;
	int                    captures;
	int                    basedefense;
	int                    carrierdefense;
	int                    flagrecovery;
	int                    fragcarrier;
	int                    assists;
	float                  lasthurtcarrier;
	float                  lastreturnedflag;
	float                  flagsince;
	float                  lastfraggedcarrier;
};

// the auto following clients don't follow a specific client number, but instead follow the first two active players

// client data that stays across multiple levels or tournament restarts.
// this is achieved by writing all the data to cvar strings at game shutdown time and reading them back at connection time.
// Anything added here MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
struct clientSession_t {
	team_t           sessionTeam;
	int              spectatorNum;    // for determining next-in-line to play
	spectatorState_t spectatorState;
	int              spectatorClient; // for chasecam and follow mode
	int              wins, losses;    // tournament stats
	forcePowers_t    selectedFP;      // check against this, if doesn't match value in playerstate then update userinfo
	int              saberLevel;      // similar to above method, but for current saber attack level
	int              setForce;        // set to true once player is given the chance to set force powers
	int              updateUITime;    // only update userinfo for FP/SL if < level.time
	qboolean         teamLeader;      // true when this client is a team leader
	int              duelTeam;
	char             IP[NET_ADDRSTRMAXLEN];
};

// client data that stays across multiple respawns, but is cleared on each level change or team change at ClientBegin()
struct clientPersistant_t {
	clientConnected_t connected;
	usercmd_t         cmd;               // we would lose angles if not persistant
	qboolean          localClient;       // true if "ip" info key is "localhost"
	qboolean          initialSpawn;      // the first spawn should be at a cool location
	qboolean          predictItemPickup; // based on cg_predictItems userinfo
	qboolean          pmoveFixed;
	char              netname[MAX_NETNAME];
	char              netname_nocolor[MAX_NETNAME];
	int               netnameTime;       // Last time the name was changed
	int               enterTime;         // level.time the client entered the game
	playerTeamState_t teamState;         // status in teamplay games
	qboolean          teamInfo;          // send team overlay updates?
	int               connectTime;
	char              saber1[MAX_QPATH];
	char              saber2[MAX_QPATH];
	int               vote;
	int               teamvote;          // 0 = none, 1 = yes, 2 = no
	char              guid[33];
};

struct renderInfo_t {
	vec3_t muzzlePoint;
	vec3_t muzzleDir;
	vec3_t muzzlePointOld;
	vec3_t muzzleDirOld;
	int    mPCalcTime;             // Last time muzzle point was calced
	vec3_t headPoint;           // Where your tag_head is
	vec3_t headAngles;          // where the tag_head in the torso is pointing
	vec3_t handRPoint;          // where your right hand is
	vec3_t handLPoint;          // where your left hand is
	vec3_t crotchPoint;         // Where your crotch is
	vec3_t footRPoint;          // where your right hand is
	vec3_t footLPoint;          // where your left hand is
	vec3_t torsoPoint;          // Where your chest is
	vec3_t torsoAngles;         // Where the chest is pointing
	vec3_t eyePoint;            // Where your eyes are
	vec3_t eyeAngles;           // Where your eyes face
	int    lookTarget;          // Which ent to look at with lookAngles
	int    lookTargetClearTime; // Time to clear the lookTarget
	int    lastVoiceVolume;     // Last frame's voice volume
	vec3_t lastHeadAngles;      // Last headAngles, NOT actual facing of head model
	vec3_t headBobAngles;       // headAngle offsets
	vec3_t targetHeadBobAngles; // head bob angles will try to get to targetHeadBobAngles
	int    lookingDebounceTime; // When we can stop using head looking angle behavior
	float  legsYaw;             // yaw angle your legs are actually rendering at
	void   *lastG2;             // if it doesn't match ent->ghoul2, the bolts are considered invalid.
	int    headBolt;
	int    handRBolt;
	int    handLBolt;
	int    torsoBolt;
	int    crotchBolt;
	int    footRBolt;
	int    footLBolt;
	int    motionBolt;
	int    boltValidityTime;
};

// this structure is cleared on each ClientSpawn(), except for 'client->pers' and 'client->sess'
struct gclient_t {

	// - - - - - - - - - - - - - - - -

	// ps MUST be the first element, because the server expects it (see svClient_t)
	playerState_t                        ps;                                                // communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t pers;
	clientSession_t    sess;
	saberInfo_t        saber[MAX_SABERS];
	void               *weaponGhoul2[MAX_SABERS];
	int                tossableItemDebounce;
	int                bodyGrabTime;
	int                bodyGrabIndex;
	int                pushEffectTime;
	int                invulnerableTimer;
	int                saberCycleQueue;
	int                legsAnimExecute;
	int                torsoAnimExecute;
	qboolean           legsLastFlip;
	qboolean           torsoLastFlip;
	qboolean           readyToExit;             // wishes to leave the intermission
	qboolean           noclip;
	int                lastCmdTime;             // level.time of last usercmd_t, for EF_CONNECTION. we can't just use pers.lastCommand.time, because of the g_sycronousclients case
	int                buttons;
	int                oldbuttons;
	int                latched_buttons;
	vec3_t             oldOrigin;
	int                damage_armor;            // damage absorbed by armor
	int                damage_blood;            // damage taken out of health
	int                damage_knockback;        // impact damage
	vec3_t             damage_from;             // origin for vector calculation
	qboolean           damage_fromWorld;        // if true, don't use the damage_from vector
	int                damageBoxHandle_Head;    // entity number of head damage box
	int                damageBoxHandle_RLeg;    // entity number of right leg damage box
	int                damageBoxHandle_LLeg;    // entity number of left leg damage box
	int                accurateCount;           // for "impressive" reward sound
	int                accuracy_shots;          // total number of shots
	int                accuracy_hits;           // total number of hits
	int                lastkilled_client;       // last client that this client killed
	int                lasthurt_client;         // last client that damaged this client
	int                lasthurt_mod;            // type of damage the client did
	int                respawnTime;             // can respawn when time > this, force after g_forcerespwan
	int                inactivityTime;          // kick players when time > this
	qboolean           inactivityWarning;       // qtrue if the five seoond warning has been given
	int                rewardTime;              // clear the EF_AWARD_IMPRESSIVE, etc when time > this
	int                airOutTime;
	int                lastKillTime;            // for multiple kill rewards
	qboolean           fireHeld;                // used for hook
	gentity_t          *hook;                   // grapple hook if out
	int                switchTeamTime;          // time the player switched teams
	int                switchDuelTeamTime;      // time the player switched duel teams
	int                switchClassTime;         // class changed debounce timer
	int                timeResidual;
	char               *areabits;
	int                g2LastSurfaceHit;        // index of surface hit during the most recent ghoul2 collision performed on this client.
	int                g2LastSurfaceTime;       // time when the surface index was set (to make sure it's up to date)
	int                corrTime;
	vec3_t             lastHeadAngles;
	int                lookTime;
	int                brokenLimbs;
	qboolean           noCorpse;                // don't leave a corpse on respawn this time.
	int                jetPackTime;
	qboolean           jetPackOn;
	int                jetPackToggleTime;
	int                jetPackDebRecharge;
	int                jetPackDebReduce;
	int                cloakToggleTime;
	int                cloakDebRecharge;
	int                cloakDebReduce;
	int                saberStoredIndex;        // stores saberEntityNum from playerstate for when it's set to 0 (indicating saber was knocked out of the air)
	int                saberKnockedTime;        // if saber gets knocked away, can't pull it back until this value is < level.time
	vec3_t             olderSaberBase;          // Set before lastSaberBase_Always, to whatever lastSaberBase_Always was previously
	qboolean           olderIsValid;            // is it valid?
	vec3_t             lastSaberDir_Always;     // every getboltmatrix, set to saber dir
	vec3_t             lastSaberBase_Always;    // every getboltmatrix, set to saber base
	int                lastSaberStorageTime;    // server time that the above two values were updated (for making sure they aren't out of date)
	qboolean           hasCurrentPosition;      // are lastSaberTip and lastSaberBase valid?
	int                dangerTime;              // level.time when last attack occured
	int                idleTime;                // keep track of when to play an idle anim on the client.
	int                idleHealth;              // stop idling if health decreases
	vec3_t             idleViewAngles;          // stop idling if viewangles change
	int                forcePowerSoundDebounce; // if > level.time, don't do certain sound events again (drain sound, absorb sound, etc)
	char               modelname[MAX_QPATH];
	qboolean           fjDidJump;
	qboolean           ikStatus;
	int                throwingIndex;
	int                beingThrown;
	int                doingThrow;
	float              hiddenDist;              // How close ents have to be to pick you up as an enemy
	vec3_t             hiddenDir;               // Normalized direction in which NPCs can't see you (you are hidden)
	renderInfo_t       renderInfo;
	vec3_t             pushVec;
	int                pushVecTime;
	int                isMedHealed;
	int                isMedSupplied;
	int                medSupplyDebounce;
	int                isHacking;
	vec3_t             hackingAngles;
	int                ewebIndex;               // index of e-web gun if spawned
	int                ewebTime;                // e-web use debounce
	int                ewebHealth;              // health of e-web (to keep track between deployments)
	int                inSpaceIndex;            // ent index of space trigger if inside one
	int                inSpaceSuffocation;      // suffocation timer
	int                tempSpectate;            // time to force spectator mode
	int                jediKickIndex;
	int                jediKickTime;
	int                grappleIndex;
	int                grappleState;
	int                solidHack;
	int                noLightningTime;
	uint32_t           mGameFlags;
	qboolean           iAmALoser;
	int                lastGenCmd;
	int                lastGenCmdTime;

	struct force {
		int regenDebounce;
		int drainDebounce;
		int lightningDebounce;
	} force;
};

struct locationData_t {
	char   message[MAX_SPAWN_VARS_CHARS];
	int    count;
	int    cs_index;
	vec3_t origin;
};

struct level_locals_t {
	struct {
		fileHandle_t log;
	} security;

	struct {
		int  num;
		char *infos[MAX_BOTS];
	} bots;

	struct {
		int  num;
		char *infos[MAX_ARENAS];
	} arenas;

	struct {
		int            num;
		qboolean       linked;
		locationData_t data[MAX_LOCATIONS];
	} locations;

	gclient_t    *clients;                      // [maxclients]
	gentity_t    *gentities;
	int          gentitySize;
	int          num_entities;                  // current number, <= MAX_GENTITIES
	int          warmupTime;                    // restart match at this time
	fileHandle_t logFile;
	int          maxclients;
	int          framenum;
	int          time;                          // in msec
	int          previousTime;                  // so movers can back up when blocked
	int          startTime;                     // level.time the map was started
	int          teamScores[TEAM_NUM_TEAMS];
	int          lastTeamLocationTime;          // last time of client team location update
	qboolean     newSession;                    // don't use any old session data, because we changed gametype
	qboolean     restarted;                     // waiting for a map_restart to fire
	int          numConnectedClients;
	int          numNonSpectatorClients;        // includes connecting clients
	int          numPlayingClients;             // connected, non-spectators
	int          sortedClients[MAX_CLIENTS];    // sorted by score
	int          follow1, follow2;              // clientNums for auto-follow spectators
	int          snd_fry;                       // sound index for standing in lava
	int          snd_hack;                      // hacking loop sound
    int          snd_medHealed;                 // being healed by supply class
	int          snd_medSupplied;               // being supplied by supply class
	char         voteString[MAX_STRING_CHARS];
	char         voteStringClean[MAX_STRING_CHARS];
	char         voteDisplayString[MAX_STRING_CHARS];
	int          voteTime;                      // level.time vote was called
	int          voteExecuteTime;               // time the vote is executed
	int          voteExecuteDelay;              // set per-vote
	int          voteYes;
	int          voteNo;
	int          numVotingClients;              // set by CalculateRanks
	qboolean     votingGametype;
	int          votingGametypeTo;
	char         teamVoteString[2][MAX_STRING_CHARS];
	char         teamVoteStringClean[2][MAX_STRING_CHARS];
	char         teamVoteDisplayString[2][MAX_STRING_CHARS];
	int          teamVoteTime[2];               // level.time vote was called
	int          teamVoteExecuteTime[2];        // time the vote is executed
	int          teamVoteYes[2];
	int          teamVoteNo[2];
	int          numteamVotingClients[2];       // set by CalculateRanks
	qboolean     spawning;                      // the G_Spawn*() functions are valid
	int          numSpawnVars;
	char         *spawnVars[MAX_SPAWN_VARS][2]; // key / value pairs
	int          numSpawnVarChars;
	char         spawnVarChars[MAX_SPAWN_VARS_CHARS];
	int          intermissionQueued;            // intermission was qualified, but wait INTERMISSION_DELAY_TIME before actually going there so the last frag can be watched.  Disable future kills during this delay
	int          intermissiontime;              // time the intermission was started
	char         *changemap;
	qboolean     readyToExit;                   // at least one client wants to exit
	int          exitTime;
	vec3_t       intermission_origin;           // also used for spectator spawns
	vec3_t       intermission_angle;
	int          bodyQueIndex;                  // dead bodies
	gentity_t    *bodyQue[BODY_QUEUE_SIZE];
	int          portalSequence;
	int          mNumBSPInstances;
	int          mBSPInstanceDepth;
	vec3_t       mOriginAdjust;
	float        mRotationAdjust;
	char         *mTargetAdjust;
	char         mTeamFilter[MAX_QPATH];
	gametype_t   gametype;
	char         mapname[MAX_QPATH];
	char         rawmapname[MAX_QPATH];
};

struct reference_tag_t {
	char     name[MAX_REFNAME];
	vec3_t   origin;
	vec3_t   angles;
	int      flags; //Just in case
	int      radius; //For nav goals
	qboolean inuse;
};

struct bot_settings_t {
	char  personalityfile[MAX_QPATH];
	float skill;
	char  team[MAX_FILEPATH];
};

qboolean G_CanBeEnemy(gentity_t* self, gentity_t* enemy); //w_saber.cpp
qboolean TryGrapple(gentity_t *ent); //g_cmds.cpp
void G_CheapWeaponFire(int entNum, int ev); //g_active.cpp

int              AcceptBotCommand                    ( char *cmd, gentity_t *pl );
void             Add_Ammo                            ( gentity_t *ent, int weapon, int count );
void             AddRemap                            ( const char *oldShader, const char *newShader, float timeOffset );
void             AddScore                            ( gentity_t *ent, vec3_t origin, int score );
void             AddTournamentQueue                  ( gclient_t *client );
void             B_CleanupAlloc                      ( void );
void             B_InitAlloc                         ( void );
void             BeginIntermission                   ( void );
void             Blocked_Mover                       ( gentity_t *ent, gentity_t *other );
void             BlowDetpacks                        ( gentity_t *ent );
void             body_die                            ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
int              BotAILoadMap                        ( int restart );
int              BotAISetup                          ( int restart );
int              BotAISetupClient                    ( int client, bot_settings_t *settings, qboolean restart );
int              BotAIShutdown                       ( int restart );
int              BotAIShutdownClient                 ( int client, qboolean restart );
int              BotAIStartFrame                     ( int time );
void             BotDamageNotification               ( gclient_t *bot, gentity_t *attacker );
void             BotInterbreedEndMatch               ( void );
void             BotOrder                            ( gentity_t *ent, int clientnum, int ordernum );
void             BroadcastTeamChange                 ( gclient_t *client, int oldTeam );
const char      *BuildShaderStateConfig              ( void );
void             CalcMuzzlePoint                     ( gentity_t *ent, const vec3_t inForward, const vec3_t inRight, const vec3_t inUp, vec3_t muzzlePoint );
void             CalculateRanks                      ( void );
qboolean         CanDamage                           ( gentity_t *targ, vec3_t origin );
void             CheckExitRules                      ( void );
qboolean         CheckGauntletAttack                 ( gentity_t *ent );
void             CheckTeamLeader                     ( int team );
void             ClearRegisteredItems                ( void );
void             Client_CheckImpactBBrush            ( gentity_t *self, gentity_t *other );
void             ClientBegin                         ( int clientNum, qboolean allowTeamReset );
void             ClientCommand                       ( int clientNum );
char            *ClientConnect                       ( int clientNum, qboolean firstTime, qboolean isBot );
void             ClientDisconnect                    ( int clientNum );
void             ClientEndFrame                      ( gentity_t *ent );
void             ClientRespawn                       ( gentity_t *ent );
void             ClientSpawn                         ( gentity_t *ent );
void             ClientThink                         ( int clientNum, usercmd_t *ucmd );
qboolean         ClientUserinfoChanged               ( int clientNum );
void             Cmd_EngageDuel_f                    ( gentity_t *ent );
void             Cmd_FollowCycle_f                   ( gentity_t *ent, int dir );
void             Cmd_SaberAttackCycle_f              ( gentity_t *ent );
void             Cmd_Score_f                         ( gentity_t *ent );
void             Cmd_ToggleSaber_f                   ( gentity_t *ent );
char            *ConcatArgs                          ( int start );
qboolean         ConsoleCommand                      ( void );
gentity_t       *CreateMissile                       ( vec3_t org, vec3_t dir, float vel, int life, gentity_t *owner, qboolean altFire );
void             DeathmatchScoreboardMessage         ( gentity_t *client );
void             DoImpact                            ( gentity_t *self, gentity_t *other, qboolean damageSelf );
gentity_t       *Drop_Item                           ( gentity_t *ent, gitem_t *item, float angle );
void             ExplodeDeath                        ( gentity_t *self );
void             FindIntermissionPoint               ( void );
void             FinishSpawningItem                  ( gentity_t *ent );
void             FireWeapon                          ( gentity_t *ent, qboolean altFire );
void             ForceAbsorb                         ( gentity_t *self );
void             ForceGrip                           ( gentity_t *self );
void             ForceHeal                           ( gentity_t *self );
int              ForcePowerUsableOn                  ( gentity_t *attacker, gentity_t *other, forcePowers_t forcePower );
void             ForceProtect                        ( gentity_t *self );
void             ForceRage                           ( gentity_t *self );
void             ForceSeeing                         ( gentity_t *self );
void             ForceSpeed                          ( gentity_t *self, int forceDuration );
void             ForceTeamForceReplenish             ( gentity_t *self );
void             ForceTeamHeal                       ( gentity_t *self );
void             ForceTelepathy                      ( gentity_t *self );
void             ForceThrow                          ( gentity_t *self, qboolean pull );
qboolean         G_ActivateBehavior                  ( gentity_t *self, int bset );
void             G_AddEvent                          ( gentity_t *ent, int event, int eventParm );
void             G_AddPredictableEvent               ( gentity_t *ent, int event, int eventParm );
void            *G_Alloc                             ( int size );
void             G_ApplyKnockback                    ( gentity_t *targ, vec3_t newDir, float knockback );
void             G_BounceProjectile                  ( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout );
int              G_BoneIndex                         ( const char *name );
qboolean         G_BotConnect                        ( int clientNum, qboolean restart );
qboolean         G_BoxInBounds                       ( vec3_t point, vec3_t mins, vec3_t maxs, vec3_t boundsMins, vec3_t boundsMaxs );
void             G_BreakArm                          ( gentity_t *ent, int arm );
int              G_BSPIndex                          ( const char *name );
int              G_CheckAlertEvents                  ( gentity_t *self, qboolean checkSight, qboolean checkSound, float maxSeeDist, float maxHearDist, int ignoreAlert, qboolean mustHaveOwner, int minAlertLevel );                                                // ignoreAlert = -1, mustHaveOwner = qfalse, minAlertLevel = AEL_MINOR
void             G_CheckBotSpawn                     ( void );
void             G_CheckClientTimeouts               ( gentity_t *ent );
qboolean         G_CheckForDanger                    ( gentity_t *self, int alertEvent );
void             G_CheckForDismemberment             ( gentity_t *ent, gentity_t *enemy, vec3_t point, int damage, int deathAnim, qboolean postDeath );
qboolean         G_CheckInSolid                      ( gentity_t *self, qboolean fix );
void             G_CheckTeamItems                    ( void );
void             G_ClearClientLog                    ( int client );
qboolean         G_ClearLOS                          ( gentity_t *self, const vec3_t start, const vec3_t end );
qboolean         G_ClearLOS2                         ( gentity_t *self, gentity_t *ent, const vec3_t end );
qboolean         G_ClearLOS3                         ( gentity_t *self, const vec3_t start, gentity_t *ent );
qboolean         G_ClearLOS4                         ( gentity_t *self, gentity_t *ent );
qboolean         G_ClearLOS5                         ( gentity_t *self, const vec3_t end );
void             G_ClearTeamVote                     ( gentity_t *ent, int team );
void             G_ClearVote                         ( gentity_t *ent );
void             G_Damage                            ( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod );
qboolean         G_DoesMapSupportGametype            ( const char *mapname, int gametype );
int              G_EffectIndex                       ( const char *name );
qboolean         G_EntIsBreakable                    ( int entityNum );
qboolean         G_EntitiesFree                      ( void );
void             G_EntitySound                       ( gentity_t *ent, int channel, int soundIndex );
void             G_ExplodeMissile                    ( gentity_t *ent );
qboolean         G_FilterPacket                      ( char *from );
gentity_t       *G_Find                              ( gentity_t *from, int fieldofs, const char *match );
gentity_t       *G_FindDoorTrigger                   ( gentity_t *ent );
void             G_FreeEntity                        ( gentity_t *e );
const char      *G_GetArenaInfoByMap                 ( const char *map );
void             G_GetBoltPosition                   ( gentity_t *self, int boltIndex, vec3_t pos, int modelIndex );
char            *G_GetBotInfoByName                  ( const char *name );
char            *G_GetBotInfoByNumber                ( int num );
int              G_GetHitLocation                    ( gentity_t *target, vec3_t ppoint );
const char      *G_GetStringEdString                 ( char *refSection, char *refName );
int              G_IconIndex                         ( const char* name );
void             G_InitBots                          ( void );
void             G_InitGentity                       ( gentity_t *e );
void             G_InitMemory                        ( void );
void             G_InitSessionData                   ( gclient_t *client, char *userinfo, qboolean isBot );
void             G_InitWorldSession                  ( void );
int              G_ItemUsable                        ( playerState_t *ps, int forcedUse );
void             G_KillBox                           ( gentity_t *ent );
void             G_KillG2Queue                       ( int entNum );
void             G_Knockdown                         ( gentity_t *victim );
void             G_LetGoOfWall                       ( gentity_t *ent );
void             G_LoadArenas                        ( void );
void             G_LogExit                           ( const char *string );
void             G_LogPrintf                         ( const char *fmt, ... );
void             G_LogWeaponDamage                   ( int client, int mod, int amount );
void             G_LogWeaponDeath                    ( int client, int weaponid );
void             G_LogWeaponFire                     ( int client, int weaponid );
void             G_LogWeaponFrag                     ( int attacker, int deadguy );
void             G_LogWeaponInit                     ( void );
void             G_LogWeaponItem                     ( int client, int itemid );
void             G_LogWeaponKill                     ( int client, int mod );
void             G_LogWeaponOutput                   ( void );
void             G_LogWeaponPickup                   ( int client, int weaponid );
void             G_LogWeaponPowerup                  ( int client, int powerupid );
int              G_ModelIndex                        ( const char *name );
void             G_MoverTouchPushTriggers            ( gentity_t *ent, vec3_t oldOrg );
void             G_MuteSound                         ( int entnum, int channel );
char            *G_NewString                         ( const char *string );
qboolean         G_ParseSpawnVars                    ( qboolean inSubBSP );
gentity_t       *G_PickTarget                        ( char *targetname );
void             G_PlayDoorLoopSound                 ( gentity_t *ent );
void             G_PlayDoorSound                     ( gentity_t *ent, int type );
gentity_t       *G_PlayEffect                        ( int fxID, vec3_t org, vec3_t ang );
gentity_t       *G_PlayEffectID                      ( const int fxID, vec3_t org, vec3_t ang );
gentity_t       *G_PlayEffectID                      ( const int fxID, vec3_t org, vec3_t ang );
qboolean         G_PointInBounds                     ( vec3_t point, vec3_t mins, vec3_t maxs );
void             G_PowerDuelCount                    ( int *loners, int *doubles, qboolean countSpec );
void             G_PrecacheDispensers                ( void );
gentity_t       *G_PreDefSound                       ( vec3_t org, int pdSound );
void             G_ProcessIPBans                     ( void );
qboolean         G_RadiusDamage                      ( vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, gentity_t *missile, int mod );
int              G_RadiusList                        ( vec3_t origin, float radius, gentity_t *ignore, qboolean takeDamage, gentity_t *ent_list[MAX_GENTITIES] );
void             G_ReadSessionData                   ( gclient_t *client );
void             G_ReflectMissile                    ( gentity_t *ent, gentity_t *missile, vec3_t forward );
const char      *G_RefreshNextMap                    ( int gametype, qboolean forced );
void             G_RegisterCvars                     ( void );
void             G_RemoveQueuedBotBegin              ( int clientNum );
void             G_ROFF_NotetrackCallback            ( gentity_t *cent, const char *notetrack );
void             G_RunClient                         ( gentity_t *ent );
void             G_RunExPhys                         ( gentity_t *ent, float gravity, float mass, float bounce, qboolean autoKill, int *g2Bolts, int numG2Bolts );
void             G_RunItem                           ( gentity_t *ent );
void             G_RunMissile                        ( gentity_t *ent );
void             G_RunMover                          ( gentity_t *ent );
void             G_RunObject                         ( gentity_t *ent );
void             G_RunThink                          ( gentity_t *ent );
void             G_ScaleNetHealth                    ( gentity_t *self );
gentity_t       *G_ScreenShake                       ( vec3_t org, gentity_t *target, float intensity, int duration, qboolean global );
void             G_SecurityLogPrintf                 ( const char *fmt, ... );
void             G_SendG2KillQueue                   ( void );
void             G_SetAngles                         ( gentity_t *ent, vec3_t angles );
void             G_SetAnim                           ( gentity_t *ent, usercmd_t *ucmd, int setAnimParts, int anim, int setAnimFlags, int blendTime );
void             G_SetMovedir                        ( vec3_t angles, vec3_t movedir );
void             G_SetOrigin                         ( gentity_t *ent, vec3_t origin );
qboolean         G_SetSaber                          ( gentity_t *ent, int saberNum, char *saberName );
void             G_SetStats                          ( gentity_t *ent );
void             G_Sound                             ( gentity_t *ent, int channel, int soundIndex );
void             G_SoundAtLoc                        ( vec3_t loc, int channel, int soundIndex );
int              G_SoundIndex                        ( const char *name );
int              G_SoundSetIndex                     ( const char *name );
gentity_t       *G_Spawn                             ( void );
qboolean         G_SpawnBoolean                      ( const char *key, const char *defaultString, qboolean *out );
void             G_SpawnEntitiesFromString           ( qboolean inSubBSP );
qboolean         G_SpawnFloat                        ( const char *key, const char *defaultString, float *out );
void             G_SpawnGEntityFromSpawnVars         ( qboolean inSubBSP );
qboolean         G_SpawnInt                          ( const char *key, const char *defaultString, int *out );
qboolean         G_SpawnString                       ( const char *key, const char *defaultString, char **out );
qboolean         G_SpawnVector                       ( const char *key, const char *defaultString, float *out );
void             G_SpawnItem                         ( gentity_t *ent, gitem_t *item );
void             G_TeamCommand                       ( team_t team, char *cmd );
gentity_t       *G_TempEntity                        ( vec3_t origin, int event );
void             G_TestLine                          ( vec3_t start, vec3_t end, int color, int time );
qboolean         G_ThereIsAMaster                    ( void );
void             G_Throw                             ( gentity_t *targ, vec3_t newDir, float push );
void             G_TouchSolids                       ( gentity_t *ent );
void             G_TouchTriggers                     ( gentity_t *ent );
void             G_UpdateClientAnims                 ( gentity_t *self, float animSpeedScale );
void             G_UpdateCvars                       ( void );
void             G_UseTargets                        ( gentity_t *ent, gentity_t *activator );
void             G_UseTargets2                       ( gentity_t *ent, gentity_t *activator, const char *string );
void             G_WriteClientSessionData            ( gclient_t *client );
void             G_WriteSessionData                  ( void );
void             GetAnglesForDirection               ( const vec3_t p1, const vec3_t p2, vec3_t out );
void             GlobalUse                           ( gentity_t *self, gentity_t *other, gentity_t *activator );
qboolean         HasSetSaberOnly                     ( void );
int              InFieldOfVision                     ( vec3_t viewangles, float fov, vec3_t angles );
qboolean         InFront                             ( vec3_t spot, vec3_t from, vec3_t fromAngles, float threshHold );
void             InitBodyQue                         ( void );
void             InitMoverTrData                     ( gentity_t *ent );
void             ItemUse_Binoculars                  ( gentity_t *ent );
void             ItemUse_Jetpack                     ( gentity_t *ent );
void             ItemUse_MedPack                     ( gentity_t *ent );
void             ItemUse_MedPack_Big                 ( gentity_t *ent );
void             ItemUse_Seeker                      ( gentity_t *ent );
void             ItemUse_Sentry                      ( gentity_t *ent );
void             ItemUse_Shield                      ( gentity_t *ent );
void             ItemUse_UseCloak                    ( gentity_t *ent );
void             ItemUse_UseDisp                     ( gentity_t *ent, int type );
void             ItemUse_UseEWeb                     ( gentity_t *ent );
void             Jedi_Cloak                          ( gentity_t *self );
void             Jedi_Decloak                        ( gentity_t *self );
qboolean         Jedi_DodgeEvasion                   ( gentity_t *self, gentity_t *shooter, trace_t *tr, int hitLoc );
void             Jetpack_Off                         ( gentity_t *ent );
void             Jetpack_On                          ( gentity_t *ent );
void             laserTrapStick                      ( gentity_t *ent, vec3_t endpos, vec3_t normal );
gentity_t       *LaunchItem                          ( gitem_t *item, vec3_t origin, vec3_t velocity );
void             LockDoors                           ( gentity_t *const ent );
qboolean         LogAccuracyHit                      ( gentity_t *target, gentity_t *attacker );
void             LogExit                             ( const char *string );
void             MaintainBodyQueue                   ( gentity_t *ent );
void             MatchTeam                           ( gentity_t *teamLeader, int moverState, int time );
void             MoveClientToIntermission            ( gentity_t *client );
void             ObjectDie                           ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
qboolean         OnSameTeam                          ( gentity_t *ent1, gentity_t *ent2 );
int              OrgVisible                          ( vec3_t org1, vec3_t org2, int ignore );
team_t           PickTeam                            ( int ignoreClientNum );
void             player_die                          ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );
float            RandFloat                           ( float min, float max );
void             RegisterItem                        ( gitem_t *item );
void             RemoveDetpacks                      ( gentity_t *ent );
void             RespawnItem                         ( gentity_t *ent );
void             saberBackToOwner                    ( gentity_t *saberent );
qboolean         saberCheckKnockdown_DuelLoss        ( gentity_t *saberent, gentity_t *saberOwner, gentity_t *other );
void             saberKnockDown                      ( gentity_t *saberent, gentity_t *saberOwner, gentity_t *other );
void             saberReactivate                     ( gentity_t *saberent, gentity_t *saberOwner );
void             SaveRegisteredItems                 ( void );
void             scriptrunner_run                    ( gentity_t *self );
gentity_t       *SelectRandomDeathmatchSpawnPoint    ( qboolean isbot );
gentity_t       *SelectSpawnPoint                    ( vec3_t avoidPoint, vec3_t origin, vec3_t angles, team_t team, qboolean isbot );
void             SendScoreboardMessageToAllClients   ( void );
void             SetClientViewAngle                  ( gentity_t *ent, vec3_t angle );
void             SetLeader                           ( int team, int client );
void             SetTeam                             ( gentity_t *ent, char *s );
void             SetTeamQuick                        ( gentity_t *ent, team_t team, qboolean doBegin );
void             SnapVectorTowards                   ( vec3_t v, vec3_t to );
void             SP_CreateRain                       ( gentity_t *self );
void             SP_CreateSnow                       ( gentity_t *self );
void             SP_CreateSpaceDust                  ( gentity_t *self );
void             SP_CreateWind                       ( gentity_t *self );
void             SP_emplaced_gun                     ( gentity_t *self );
void             SP_func_bobbing                     ( gentity_t *self );
void             SP_func_breakable                   ( gentity_t *self );
void             SP_func_button                      ( gentity_t *self );
void             SP_func_door                        ( gentity_t *self );
void             SP_func_glass                       ( gentity_t *self );
void             SP_func_pendulum                    ( gentity_t *self );
void             SP_func_plat                        ( gentity_t *self );
void             SP_func_rotating                    ( gentity_t *self );
void             SP_func_static                      ( gentity_t *self );
void             SP_func_timer                       ( gentity_t *self );
void             SP_func_train                       ( gentity_t *self );
void             SP_func_usable                      ( gentity_t *self );
void             SP_func_wall                        ( gentity_t *self );
void             SP_fx_runner                        ( gentity_t *self );
void             SP_gametype_item                    ( gentity_t *self );
void             SP_info_camp                        ( gentity_t *self );
void             SP_info_jedimaster_start            ( gentity_t *self );
void             SP_info_notnull                     ( gentity_t *self );
void             SP_info_null                        ( gentity_t *self );
void             SP_info_null                        ( gentity_t *self );
void             SP_info_player_deathmatch           ( gentity_t *self );
void             SP_info_player_duel                 ( gentity_t *self );
void             SP_info_player_duel1                ( gentity_t *self );
void             SP_info_player_duel2                ( gentity_t *self );
void             SP_info_player_intermission_blue    ( gentity_t *self );
void             SP_info_player_intermission_red     ( gentity_t *self );
void             SP_info_player_intermission         ( gentity_t *self );
void             SP_info_player_start_blue           ( gentity_t *self );
void             SP_info_player_start_red            ( gentity_t *self );
void             SP_info_player_start                ( gentity_t *self );
void             SP_item_botroam                     ( gentity_t *self );
void             SP_light                            ( gentity_t *self );
void             SP_misc_ammo_floor_unit             ( gentity_t *self );
void             SP_misc_bsp                         ( gentity_t *self );
void             SP_misc_cubemap                     ( gentity_t *self );
void             SP_misc_faller                      ( gentity_t *self );
void             SP_misc_G2model                     ( gentity_t *self );
void             SP_misc_holocron                    ( gentity_t *self );
void             SP_misc_maglock                     ( gentity_t *self );
void             SP_misc_model_ammo_power_converter  ( gentity_t *self );
void             SP_misc_model_breakable             ( gentity_t *self );
void             SP_misc_model_health_power_converter( gentity_t *self );
void             SP_misc_model_shield_power_converter( gentity_t *self );
void             SP_misc_model_static                ( gentity_t *self );
void             SP_misc_model                       ( gentity_t *self );
void             SP_misc_portal_camera               ( gentity_t *self );
void             SP_misc_portal_surface              ( gentity_t *self );
void             SP_misc_shield_floor_unit           ( gentity_t *self );
void             SP_misc_skyportal_orient            ( gentity_t *self );
void             SP_misc_skyportal                   ( gentity_t *self );
void             SP_misc_teleporter_dest             ( gentity_t *self );
void             SP_misc_turret                      ( gentity_t *self );
void             SP_misc_turretG2                    ( gentity_t *self );
void             SP_misc_weapon_shooter              ( gentity_t *self );
void             SP_misc_weather_zone                ( gentity_t *self );
void             SP_path_corner                      ( gentity_t *self );
void             SP_reference_tag                    ( gentity_t *self );
void             SP_reference_tag                    ( gentity_t *self );
void             SP_shooter_blaster                  ( gentity_t *self );
void             SP_target_activate                  ( gentity_t *self );
void             SP_target_counter                   ( gentity_t *self );
void             SP_target_deactivate                ( gentity_t *self );
void             SP_target_delay                     ( gentity_t *self );
void             SP_target_give                      ( gentity_t *self );
void             SP_target_kill                      ( gentity_t *self );
void             SP_target_laser                     ( gentity_t *self );
void             SP_target_level_change              ( gentity_t *self );
void             SP_target_location                  ( gentity_t *self );
void             SP_target_play_music                ( gentity_t *self );
void             SP_target_position                  ( gentity_t *self );
void             SP_target_print                     ( gentity_t *self );
void             SP_target_push                      ( gentity_t *self );
void             SP_target_random                    ( gentity_t *self );
void             SP_target_relay                     ( gentity_t *self );
void             SP_target_remove_powerups           ( gentity_t *self );
void             SP_target_score                     ( gentity_t *self );
void             SP_target_screenshake               ( gentity_t *self );
void             SP_target_scriptrunner              ( gentity_t *self );
void             SP_target_speaker                   ( gentity_t *self );
void             SP_target_teleporter                ( gentity_t *self );
void             SP_team_CTF_blueplayer              ( gentity_t *self );
void             SP_team_CTF_bluespawn               ( gentity_t *self );
void             SP_team_CTF_redplayer               ( gentity_t *self );
void             SP_team_CTF_redspawn                ( gentity_t *self );
void             SP_terrain                          ( gentity_t *self );
void             SP_trigger_always                   ( gentity_t *self );
void             SP_trigger_asteroid_field           ( gentity_t *self );
void             SP_trigger_hurt                     ( gentity_t *self );
void             SP_trigger_lightningstrike          ( gentity_t *self );
void             SP_trigger_multiple                 ( gentity_t *self );
void             SP_trigger_once                     ( gentity_t *self );
void             SP_trigger_push                     ( gentity_t *self );
void             SP_trigger_space                    ( gentity_t *self );
void             SP_trigger_teleport                 ( gentity_t *self );
void             SP_waypoint_navgoal_1               ( gentity_t *self );
void             SP_waypoint_navgoal_2               ( gentity_t *self );
void             SP_waypoint_navgoal_4               ( gentity_t *self );
void             SP_waypoint_navgoal_8               ( gentity_t *self );
void             SP_waypoint_navgoal                 ( gentity_t *self );
void             SP_waypoint_small                   ( gentity_t *self );
void             SP_waypoint                         ( gentity_t *self );
qboolean         SpotWouldTelefrag                   ( gentity_t *spot );
qboolean         SpotWouldTelefrag2                  ( gentity_t *mover, vec3_t dest );
qboolean         StringIsInteger                     ( const char *s );
void             StopFollowing                       ( gentity_t *ent );
void             Svcmd_AddBot_f                      ( void );
void             Svcmd_BotList_f                     ( void );
void             Svcmd_GameMem_f                     ( void );
void             Svcmd_ToggleAllowVote_f             ( void );
void             Svcmd_ToggleUserinfoValidation_f    ( void );
reference_tag_t *TAG_Add                             ( const char *name, const char *owner, vec3_t origin, vec3_t angles, int radius, int flags );
reference_tag_t *TAG_Find                            ( const char *owner, const char *name );
int              TAG_GetAngles                       ( const char *owner, const char *name, vec3_t angles );
int              TAG_GetFlags                        ( const char *owner, const char *name );
int              TAG_GetOrigin                       ( const char *owner, const char *name, vec3_t origin );
int              TAG_GetOrigin2                      ( const char *owner, const char *name, vec3_t origin );
int              TAG_GetRadius                       ( const char *owner, const char *name );
void             TAG_Init                            ( void );
void             Team_CheckDroppedItem               ( gentity_t *dropped );
int              TeamCount                           ( int ignoreClientNum, team_t team );
int              TeamLeader                          ( int team );
void             TeleportPlayer                      ( gentity_t *player, vec3_t origin, vec3_t angles );
void             ThrowSaberToAttacker                ( gentity_t *self, gentity_t *attacker );
void             TIMER_Clear                         ( void );
void             TIMER_Clear2                        ( gentity_t *ent );
qboolean         TIMER_Done                          ( gentity_t *ent, const char *identifier );
qboolean         TIMER_Done2                         ( gentity_t *ent, const char *identifier, qboolean remove );
qboolean         TIMER_Exists                        ( gentity_t *ent, const char *identifier );
int              TIMER_Get                           ( gentity_t *ent, const char *identifier );
void             TIMER_Remove                        ( gentity_t *ent, const char *identifier );
void             TIMER_Set                           ( gentity_t *ent, const char *identifier, int duration );
qboolean         TIMER_Start                         ( gentity_t *self, const char *identifier, int duration );
qboolean         tri_tri_intersect                   ( vec3_t V0, vec3_t V1, vec3_t V2, vec3_t U0, vec3_t U1, vec3_t U2 );
void             TossClientCubes                     ( gentity_t *self );
void             TossClientItems                     ( gentity_t *self );
void             TossClientWeapon                    ( gentity_t *self, vec3_t direction, float speed );
void             Touch_Button                        ( gentity_t *ent, gentity_t *other, trace_t *trace );
void             Touch_DoorTrigger                   ( gentity_t *ent, gentity_t *other, trace_t *trace );
void             Touch_Item                          ( gentity_t *ent, gentity_t *other, trace_t *trace );
void             trigger_teleporter_touch            ( gentity_t *self, gentity_t *other, trace_t *trace );
void             TryUse                              ( gentity_t *ent );
void             UnLockDoors                         ( gentity_t *const ent );
void             UpdateClientRenderBolts             ( gentity_t *self, vec3_t renderOrigin, vec3_t renderAngles );
float           *tv                                  ( float x, float y, float z );
char            *vtos                                ( const vec3_t v );
void             WP_FireBlasterMissile               ( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire );
void             WP_FireGenericBlasterMissile        ( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire, int damage, int velocity, int mod );
void             WP_FireTurboLaserMissile            ( gentity_t *ent, vec3_t start, vec3_t dir );
void             WP_FireTurretMissile                ( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire, int damage, int velocity, int mod, gentity_t *ignore );
void             WP_flechette_alt_blow               ( gentity_t *ent );
void             WP_ForcePowerStop                   ( gentity_t *self, forcePowers_t forcePower );
void             WP_ForcePowersUpdate                ( gentity_t *self, usercmd_t *ucmd );
qboolean         WP_HasForcePowers                   ( const playerState_t *ps );
void             WP_InitForcePowers                  ( gentity_t *ent );
void             WP_SaberAddG2Model                  ( gentity_t *saberent, const char *saberModel, qhandle_t saberSkin );
void             WP_SaberBlockNonRandom              ( gentity_t *self, vec3_t hitloc, qboolean missileBlock );
int              WP_SaberCanBlock                    ( gentity_t *self, vec3_t point, int dflags, int mod, qboolean projectile, int attackStr );
void             WP_SaberInitBladeData               ( gentity_t *ent );
void             WP_SaberPositionUpdate              ( gentity_t *self, usercmd_t *ucmd );
void             WP_SaberStartMissileBlockCheck      ( gentity_t *self, usercmd_t *ucmd );
void             WP_SpawnInitForcePowers             ( gentity_t *ent );

#define XCVAR_PROTO
	#include "game/g_xcvar.h"
#undef XCVAR_PROTO

extern int            BMS_END;
extern int            BMS_MID;
extern int            BMS_START;
extern gentity_t      g_entities[MAX_GENTITIES];
extern void           *g2SaberInstance;
extern int            gEscapeTime;
extern qboolean       gEscaping;
extern int            gGAvoidDismember;
extern gentity_t      *gJMSaberEnt;
extern int            gPainHitLoc;
extern int            gPainMOD;
extern vec3_t         gPainPoint;
extern sharedBuffer_t gSharedBuffer;
extern level_locals_t level;
extern void           *precachedKyle;
extern gameImport_t   *trap;
