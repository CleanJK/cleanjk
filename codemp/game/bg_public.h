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

// definitions shared by both the server game and client game modules
// because games can change separately from the main system version, we need a second version that must match between game and cgame

// ======================================================================
// INCLUDE
// ======================================================================

#include "game/bg_weapons.h"
#include "game/anims.h"
#include "qcommon/q_shared.h"

// ======================================================================
// DEFINE
// ======================================================================

#define	AED_ARRAY_SIZE              (MAX_RANDOM_ANIM_SOUNDS+3)
#define	AED_BOLTINDEX               1
#define	AED_EFFECT_PROBABILITY      2
#define	AED_EFFECTINDEX             0
#define	AED_FIRE_ALT                0
#define	AED_FIRE_PROBABILITY        1
#define	AED_FOOTSTEP_PROBABILITY    1
#define	AED_FOOTSTEP_TYPE           0
#define	AED_MODELINDEX              3
#define	AED_MOVE_FWD                0
#define	AED_MOVE_RT                 1
#define	AED_MOVE_UP                 2
#define	AED_SABER_SPIN_PROBABILITY  2
#define	AED_SABER_SPIN_SABERNUM     0
#define	AED_SABER_SPIN_TYPE         1 //0 = saberspinoff, 1 = saberspin, 2-4 = saberspin1-saberspin3
#define	AED_SABER_SWING_PROBABILITY 2
#define	AED_SABER_SWING_SABERNUM    0
#define	AED_SABER_SWING_TYPE        1
#define	AED_SOUND_NUMRANDOMSNDS     (MAX_RANDOM_ANIM_SOUNDS)
#define	AED_SOUND_PROBABILITY       (MAX_RANDOM_ANIM_SOUNDS+1)
#define	AED_SOUNDCHANNEL            (MAX_RANDOM_ANIM_SOUNDS+2)
#define	AED_SOUNDINDEX_END          (MAX_RANDOM_ANIM_SOUNDS-1)
#define	AED_SOUNDINDEX_START        0
#define	CS_GAME_VERSION             20
#define	CS_INTERMISSION             22 // when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define	CS_ITEMS                    27 // string of 0's and 1's that tell which items are present
#define	CS_LEVEL_START_TIME         21 // so the timer only shows the current level
#define	CS_LIGHT_STYLES             (CS_EFFECTS + MAX_FX)
#define	CS_MESSAGE                  3 // from the map worldspawn's message field
#define	CS_MODELS                   (CS_AMBIENT_SET+1)
#define	CS_MOTD                     4 // g_motd string for server message of the day
#define	CS_MUSIC                    2
#define	CS_PLAYERS                  (CS_ICONS+MAX_ICONS)
#define	CS_SCORES1                  6
#define	CS_SCORES2                  7
#define	CS_SKYBOXORG                (CS_MODELS+MAX_MODELS)		//rww - skybox info
#define	CS_SOUNDS                   (CS_SKYBOXORG+1)
#define	CS_TEAMVOTE_NO              18
#define	CS_TEAMVOTE_YES             16
#define	CS_VOTE_NO                  11
#define	CS_VOTE_YES                 10
#define	CS_WARMUP                   5		// server time when the match will be restarted
#define	DEAD_VIEWHEIGHT             -16
#define	DEFAULT_GRAVITY             800
#define	DEFAULT_MODEL               "kyle"
#define	DEFAULT_VIEWHEIGHT          (DEFAULT_MAXS_2+STANDARD_VIEWHEIGHT_OFFSET)//26
#define	EVENT_VALID_MSEC            300
#define	FX_STATE_OFF                0
#define	GAME_VERSION                "cleanjk-1"
#define	GIB_HEALTH                  -40
#define	HYPERSPACE_SPEED            10000.0f//was 30000
#define	HYPERSPACE_TELEPORT_FRAC    0.75f
#define	HYPERSPACE_TIME             4000 //For hyperspace triggers
#define	HYPERSPACE_TURN_RATE        45.0f
#define	ITEM_INDEX(x)               ((x)-bg_itemlist)
#define	ITEM_RADIUS                 15		// item sizes are needed for client side pickup detection
#define	JUMP_VELOCITY               225//270
#define	MASK_ALL                    (0xFFFFFFFFu)
#define	MASK_DEADSOLID              (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_TERRAIN)
#define	MASK_OPAQUE                 (CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_TERRAIN)
#define	MASK_PLAYERSOLID            (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY|CONTENTS_TERRAIN)
#define	MASK_SHOT                   (CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_TERRAIN)
#define	MASK_SOLID                  (CONTENTS_SOLID|CONTENTS_TERRAIN)
#define	MASK_WATER                  (CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MAX_ARENAS_TEXT             8192
#define	MAX_ITEMS                   256
#define MAX_SABER_HILTS             256 //64
#define	MAX_SPAWN_VARS              64
#define	MAX_SPAWN_VARS_CHARS        4096
#define	MAXTOUCH                    32
#define	MINS_Z                      -24
#define	PMF_ALL_TIMES               (PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK)
#define	RANK_TIED_FLAG              0x4000
#define	SCORE_NOT_PRESENT           -9999	// for the CS_SCORES[12] when only one player is present
#define	STANDARD_VIEWHEIGHT_OFFSET  -4
#define	STEPSIZE                    18
#define	VOTE_TIME                   30000	// 30 seconds before vote times out
#define ARENAS_PER_TIER             4
#define ARMOR_PROTECTION            0.50 // Shields only stop 50% of armor-piercing dmg
#define ARMOR_REDUCTION_FACTOR      0.50 // Certain damage doesn't take off armor as efficiently
#define BG_NUM_TOGGLEABLE_SURFACES  31
#define CROUCH_MAXS_2               16
#define CROUCH_VIEWHEIGHT           (CROUCH_MAXS_2+STANDARD_VIEWHEIGHT_OFFSET)//12
#define CS_AMBIENT_SET              37
#define CS_BOTINFO                  25
#define CS_BSP_MODELS               (CS_TERRAINS + MAX_TERRAINS)
#define CS_CLIENT_DUELHEALTHS       31		// nmckenzie: DUEL_HEALTH.  Hopefully adding this cs is safe and good?
#define CS_CLIENT_DUELISTS          30		// client numbers for both current duelists. Needed for a number of client-side things.
#define CS_CLIENT_DUELWINNER        29		// current duel round winner - needed for printing at top of scoreboard
#define CS_CLIENT_JEDIMASTER        28		// current jedi master
#define CS_EFFECTS                  (CS_PARTICLES+MAX_LOCATIONS)
#define CS_FLAGSTATUS               23		// string indicating flag status in CTF
#define CS_G2BONES                  (CS_PLAYERS+MAX_CLIENTS)
#define CS_GLOBAL_AMBIENT_SET       32
#define CS_ICONS                    (CS_SOUNDS+MAX_SOUNDS)
#define CS_LOCATIONS                (CS_G2BONES+MAX_G2BONES)
#define CS_MAX                      (CS_BSP_MODELS + MAX_SUB_BSP)
#define CS_PARTICLES                (CS_LOCATIONS+MAX_LOCATIONS)
#define CS_SHADERSTATE              24
#define CS_TEAMVOTE_STRING          14
#define CS_TEAMVOTE_TIME            12
#define CS_TERRAINS                 (CS_LIGHT_STYLES + (MAX_LIGHT_STYLES*3))
#define CS_VOTE_STRING              9
#define CS_VOTE_TIME                8
#define DEFAULT_BLUETEAM_NAME       "Noobie Blues"
#define DEFAULT_FORCEPOWERS         "7-1-032330000000001333"
#define DEFAULT_FORCEPOWERS_LEN     (22) // numPowers + rank + side + separators
#define DEFAULT_MAXS_2              40
#define DEFAULT_MINS_2              -24
#define DEFAULT_MODEL_FEMALE        "jan"
#define DEFAULT_REDTEAM_NAME        "Dreaded Reds"
#define DEFAULT_SABER               "single_1"
#define DEFAULT_SABER_MODEL         "models/weapons/saber_1/model.glm"
#define DEFAULT_SABER_STAFF         "dual_1"
#define DEFAULT_SABER_VIEWMODEL     "models/weapons/saber_1/model.md3"
#define FX_STATE_CONTINUOUS         20
#define FX_STATE_ONE_SHOT           1
#define FX_STATE_ONE_SHOT_LIMIT     10
#define G2_MODEL_PART               50
#define MAX_ANIM_EVENTS             300
#define MAX_ANIM_FILES              16
#define MAX_ARENAS                  1024
#define MAX_BLADES                  8
#define MAX_BOTS                    1024
#define MAX_BOTS_TEXT               8192
#define MAX_CLIENT_SCORE_SEND       20
#define MAX_CUSTOM_SIEGE_SOUNDS     30
#define MAX_ITEM_MODELS             4
#define MAX_RANDOM_ANIM_SOUNDS      4
#define MAX_SABERS                  2
#define PLAYEREVENT_HOLYSHIT        0x0004 //OJKFIXME: add holy shit :D
#define SABER_BLOCK_DUR             150 // number of milliseconds a block animation should take.
#define SABER_NAME_LENGTH           (64)
#define TEAM_LOCATION_UPDATE_TIME   1000
#define TEAM_MAXOVERLAY             32
#define TOSS_DEBOUNCE_TIME          5000

#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

// ======================================================================
// ENUM
// ======================================================================

enum eventBits_e : uint32_t {
	EV_EVENT_BIT1 = 0x0100,
	EV_EVENT_BIT2 = 0x0200,
	EV_EVENT_BITS = (EV_EVENT_BIT1|EV_EVENT_BIT2),
};

enum playerEventBits_e : uint32_t {
	PLAYEREVENT_DENIEDREWARD =   0x0001,
	PLAYEREVENT_GAUNTLETREWARD = 0x0002,
};

enum setAnimFlags_e : uint32_t {
	SETANIM_FLAG_NORMAL =   0x0000, // Only set if timer is 0
	SETANIM_FLAG_OVERRIDE = 0x0001, // Override previous
	SETANIM_FLAG_HOLD =     0x0002, // Set the new timer
	SETANIM_FLAG_RESTART =  0x0004, // Allow restarting the anim if playing the same one (weapon fires)
	SETANIM_FLAG_HOLDLESS = 0x0008, // Set the new timer
};

enum setAnimBits_e : uint32_t {
	SETANIM_TORSO = 0x0001,
	SETANIM_LEGS =  0x0002,
	SETANIM_BOTH =  (SETANIM_TORSO|SETANIM_LEGS),
};

enum g2ModelParts_t {
	G2_MODELPART_HEAD = 10,
	G2_MODELPART_WAIST,
	G2_MODELPART_LARM,
	G2_MODELPART_RARM,
	G2_MODELPART_RHAND,
	G2_MODELPART_LLEG,
	G2_MODELPART_RLEG
};

enum forceHandAnims_t {
	HANDEXTEND_NONE = 0,
	HANDEXTEND_FORCEPUSH,
	HANDEXTEND_FORCEPULL,
	HANDEXTEND_FORCE_HOLD,
	HANDEXTEND_SABERPULL,
	HANDEXTEND_CHOKE,         // use handextend priorities to choke someone being gripped
	HANDEXTEND_WEAPONREADY,
	HANDEXTEND_DODGE,
	HANDEXTEND_KNOCKDOWN,
	HANDEXTEND_DUELCHALLENGE,
	HANDEXTEND_TAUNT,
	HANDEXTEND_PRETHROW,
	HANDEXTEND_POSTTHROW,
	HANDEXTEND_PRETHROWN,
	HANDEXTEND_POSTTHROWN,
	HANDEXTEND_DRAGGING,
	HANDEXTEND_JEDITAUNT,
};

enum brokenLimb_t : int32_t {
	BROKENLIMB_NONE = 0,
	BROKENLIMB_LARM,
	BROKENLIMB_RARM,
	NUM_BROKENLIMBS
};

enum gametype_t : int32_t {
	GT_FFA,        // free for all
	GT_HOLOCRON,   // holocron ffa
	GT_JEDIMASTER, // jedi master
	GT_DUEL,       // one on one tournament
	GT_POWERDUEL,

	// team games go after this
	GT_TEAM,       // team deathmatch

	// non-score based games go after this
	GT_CTF,        // capture the flag
	GT_CTY,
	GT_MAX_GAME_TYPE
};

enum gametypeBits_e : uint32_t {
	GTB_NONE =          0x000, // invalid
	GTB_FFA =           0x001, // free for all
	GTB_HOLOCRON =      0x002, // holocron ffa
	GTB_JEDIMASTER =    0x004, // jedi master
	GTB_DUEL =          0x008, // one on one tournament
	GTB_POWERDUEL =     0x010, // two on one tournament
	GTB_SINGLE_PLAYER = 0x020, // single player ffa
	GTB_NOTTEAM =       0x03F, // **SPECIAL: All of the above gametypes, i.e. not team-based
	GTB_TEAM =          0x040, // team deathmatch
	GTB_CTF =           0x100, // capture the flag
	GTB_CTY =           0x200, // capture the ysalimiri
	GTB_ALL =           0x1FF, // all
};

enum flagStatus_t : int32_t {
	FLAG_INVALID = -1,
	FLAG_ATBASE = 0,
	FLAG_TAKEN,        // CTF
	FLAG_TAKEN_RED,    // One Flag CTF
	FLAG_TAKEN_BLUE,   // One Flag CTF
	FLAG_DROPPED
};

enum gender_t : int32_t {
	GENDER_MALE,
	GENDER_FEMALE,
	GENDER_NEUTER
};

enum saberLockType_t {
	SABERLOCK_TOP,
	SABERLOCK_SIDE,
	SABERLOCK_LOCK,
	SABERLOCK_BREAK,
	SABERLOCK_SUPERBREAK,
	SABERLOCK_WIN,
	SABERLOCK_LOSE
};

enum direction_t {
	DIR_RIGHT,
	DIR_LEFT,
	DIR_FRONT,
	DIR_BACK
};

enum footstepType_t {
	FOOTSTEP_R,
	FOOTSTEP_L,
	FOOTSTEP_HEAVY_R,
	FOOTSTEP_HEAVY_L,
	NUM_FOOTSTEP_TYPES
};

enum animEventType_t {
	//NOTENOTE:  Be sure to update animEventTypeTable and ParseAnimationEvtBlock(...) if you change this enum list!
	AEV_NONE,
	AEV_SOUND,       // # animID AEV_SOUND framenum soundpath randomlow randomhi chancetoplay
	AEV_FOOTSTEP,    // # animID AEV_FOOTSTEP framenum footstepType chancetoplay
	AEV_EFFECT,      // # animID AEV_EFFECT framenum effectpath boltName chancetoplay
	AEV_FIRE,        // # animID AEV_FIRE framenum altfire chancetofire
	AEV_MOVE,        // # animID AEV_MOVE framenum forwardpush rightpush uppush
	AEV_SOUNDCHAN,   // # animID AEV_SOUNDCHAN framenum CHANNEL soundpath randomlow randomhi chancetoplay
	AEV_SABER_SWING, // # animID AEV_SABER_SWING framenum CHANNEL randomlow randomhi chancetoplay
	AEV_SABER_SPIN,  // # animID AEV_SABER_SPIN framenum CHANNEL chancetoplay
	AEV_NUM_AEV
};

enum pmtype_t {
	PM_NORMAL,       // can accelerate and turn
	PM_JETPACK,      // special jetpack movement
	PM_FLOAT,        // float with no gravity in general direction of velocity (intended for gripping)
	PM_NOCLIP,       // noclip movement
	PM_SPECTATOR,    // still run into walls
	PM_DEAD,         // no acceleration or turning, but free falling
	PM_FREEZE,       // stuck in place with no control
	PM_INTERMISSION, // no movement or status bar
};

enum weaponstate_t {
	WEAPON_READY,
	WEAPON_RAISING,
	WEAPON_DROPPING,
	WEAPON_FIRING,
	WEAPON_CHARGING,
	WEAPON_CHARGING_ALT,
	WEAPON_IDLE,         // lowered
};

enum forceMasteries_t {
	FORCE_MASTERY_UNINITIATED,
	FORCE_MASTERY_INITIATE,
	FORCE_MASTERY_PADAWAN,
	FORCE_MASTERY_JEDI,
	FORCE_MASTERY_JEDI_GUARDIAN,
	FORCE_MASTERY_JEDI_ADEPT,
	FORCE_MASTERY_JEDI_KNIGHT,
	FORCE_MASTERY_JEDI_MASTER,
	NUM_FORCE_MASTERY_LEVELS
};

enum pmoveFlag_e : uint32_t {
	PMF_DUCKED =         0x00000001,
	PMF_JUMP_HELD =      0x00000002,
	PMF_ROLLING =        0x00000004,
	PMF_BACKWARDS_JUMP = 0x00000008, // go into backwards land
	PMF_BACKWARDS_RUN =  0x00000010, // coast down to backwards run
	PMF_TIME_LAND =      0x00000020, // pm_time is time before rejump
	PMF_TIME_KNOCKBACK = 0x00000040, // pm_time is an air-accelerate only time
	PMF_FIX_MINS =       0x00000080, // mins have been brought up, keep tracing down to fix them
	PMF_TIME_WATERJUMP = 0x00000100, // pm_time is waterjump
	PMF_RESPAWNED =      0x00000200, // clear after attack and jump buttons come up
	PMF_USE_ITEM_HELD =  0x00000400,
	PMF_UPDATE_ANIM =    0x00000800, // The server updated the animation, the pmove should set the ghoul2 anim to match.
	PMF_FOLLOW =         0x00001000, // spectate following another player
	PMF_SCOREBOARD =     0x00002000, // spectate as a scoreboard
	PMF_STUCK_TO_WALL =  0x00004000, // grabbing a wall
};

enum statIndex_t {
	// NOTE: may not have more than 16
	STAT_HEALTH,
	STAT_HOLDABLE_ITEM,
	STAT_HOLDABLE_ITEMS,
	STAT_PERSISTANT_POWERUP,
	// 16 bit fields
	STAT_WEAPONS = 4,        // MAKE SURE STAT_WEAPONS REMAINS 4!!!! There is a hardcoded reference in msg.cpp to send it in 32 bits -rww
	STAT_ARMOR,
	STAT_DEAD_YAW,           // look this direction when dead (FIXME: get rid of?)
	STAT_CLIENTS_READY,      // bit mask of clients wishing to exit the intermission (FIXME: configstring?)
	STAT_MAX_HEALTH          // health / armor limit
};

enum persEnum_t {
	// these fields are the only part of playerState_t that isn't cleared on respawn
	// NOTE: may not have more than 16
	PERS_SCORE,               // !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_HITS,                // total points damage inflicted so damage beeps can sound on change
	PERS_RANK,                // player rank or team rank
	PERS_TEAM,                // player team
	PERS_SPAWN_COUNT,         // incremented every respawn
	PERS_PLAYEREVENTS,        // 16 bits that can be flipped for events
	PERS_ATTACKER,            // clientnum of last damage inflicter
	PERS_ATTACKEE_ARMOR,      // health/armor of last person we attacked
	PERS_KILLED,              // count of the number of times you died
	PERS_IMPRESSIVE_COUNT,    // two railgun hits in a row
	PERS_EXCELLENT_COUNT,     // two successive kills in a short amount of time
	PERS_DEFEND_COUNT,        // defend awards
	PERS_ASSIST_COUNT,        // assist awards
	PERS_GAUNTLET_FRAG_COUNT, // kills with the gauntlet
	PERS_CAPTURES             // captures
};

// entityState_t->eFlags
enum entityFlag_e : uint32_t {
	EF_ALT_FIRING =      0x00000001, // for alt-fires, mostly for lightning guns though
	EF_BODYPUSH =        0x00000002, // rww - claiming this for fullbody push effect
	EF_CLIENTSMOOTH =    0x00000004, // standard lerporigin smooth override on client
	EF_CONNECTION =      0x00000008, // draw a connection trouble sprite
	EF_DEAD =            0x00000010, // don't draw a foe marker over players with EF_DEAD
	EF_DISINTEGRATION =  0x00000020, // being disintegrated by the disruptor
	EF_DOUBLE_AMMO =     0x00000040, // Hacky way to get around ammo max
	EF_DROPPEDWEAPON =   0x00000080, // it's a dropped weapon
	EF_FIRING =          0x00000100, // for lightning gun
	EF_G2ANIMATING =     0x00000200, // perform g2 bone anims based on torsoAnim and legsAnim, works for ET_GENERAL -rww
	EF_INVULNERABLE =    0x00000400, // just spawned in or whatever, so is protected
	EF_ITEMPLACEHOLDER = 0x00000800, // item effect
	EF_JETPACK =         0x00001000, // rww - wearing a jetpack
	EF_JETPACK_ACTIVE =  0x00002000, // jetpack is activated
	EF_JETPACK_FLAMING = 0x00004000, // rww - jetpack fire effect
	EF_MISSILE_STICK =   0x00008000, // missiles that stick to the wall.
	EF_NODRAW =          0x00010000, // may have an event, but no model (unspawned items)
	EF_PERMANENT =       0x00020000, // rww - I am claiming this. (for permanent entities)
	EF_PLAYER_EVENT =    0x00040000, //
	EF_RADAROBJECT =     0x00080000, // display on team radar
	EF_RAG =             0x00100000, // ragdoll him even if he's alive
	EF_SEEKERDRONE =     0x00200000, // show seeker drone floating around head
	EF_SHADER_ANIM =     0x00400000, // Animating shader (by s.frame)
	EF_SOUNDTRACKER =    0x00800000, // sound position needs to be updated in relation to another entity
	EF_TALK =            0x01000000, // draw a talk balloon
	EF_TELEPORT_BIT =    0x02000000, // toggled every time the origin abruptly changes
};

enum entityFlag2_e : uint32_t {
//NOTE: we only allow 10 of these!
	EF2_USE_ALT_ANIM =                0x0001, // For certain special runs/stands for creatures like the Rancor and Wampa whose runs/stands are conditional
	EF2_ALERTED =                     0x0002, // For certain special anims, for Rancor: means you've had an enemy, so use the more alert stand
	EF2_GENERIC_NPC_FLAG =            0x0004, // So far, used for Rancor...
	EF2_BRACKET_ENTITY =              0x0008, // Draw as bracketed
	EF2__DEPRECATED_HYPERSPACE =      0x0010, // Used to both start the hyperspace effect on the predicted client and to let the vehicle know it can now jump into hyperspace (after turning to face the proper angle)
	EF2__DEPRECATED_HELD_BY_MONSTER = 0x0020, // Being held by something, like a Rancor or a Wampa
	EF2__DEPRECATED_FLYING =          0x0040, //
	EF2__DEPRECATED_SHIP_DEATH =      0x0080, // "died in ship" mode
};

enum effectTypes_t {
	EFFECT_NONE = 0,
	EFFECT_SMOKE,
	EFFECT_EXPLOSION,
	EFFECT_EXPLOSION_PAS,
	EFFECT_SPARK_EXPLOSION,
	EFFECT_EXPLOSION_TRIPMINE,
	EFFECT_EXPLOSION_DETPACK,
	EFFECT_EXPLOSION_FLECHETTE,
	EFFECT_STUNHIT,
	EFFECT_EXPLOSION_DEMP2ALT,
	EFFECT_EXPLOSION_TURRET,
	EFFECT_SPARKS,
	EFFECT_WATER_SPLASH,
	EFFECT_ACID_SPLASH,
	EFFECT_LAVA_SPLASH,
	EFFECT_LANDING_MUD,
	EFFECT_LANDING_SAND,
	EFFECT_LANDING_DIRT,
	EFFECT_LANDING_SNOW,
	EFFECT_LANDING_GRAVEL,
	EFFECT_MAX
};

enum powerup_t {
	// NOTE: may not have more than 16
	PW_NONE,
	PW_QUAD,
	PW_BATTLESUIT,
	PW_PULL,
	PW_REDFLAG,
	PW_BLUEFLAG,
	PW_NEUTRALFLAG,
	PW_SHIELDHIT,
	PW_SPEEDBURST,
	PW_DISINT_4,
	PW_SPEED,
	PW_CLOAKED,
	PW_FORCE_ENLIGHTENED_LIGHT,
	PW_FORCE_ENLIGHTENED_DARK,
	PW_FORCE_BOON,
	PW_YSALAMIRI,
	PW_NUM_POWERUPS
};

enum holdable_t {
	HI_NONE,
	HI_SEEKER,
	HI_SHIELD,
	HI_MEDPAC,
	HI_MEDPAC_BIG,
	HI_BINOCULARS,
	HI_SENTRY_GUN,
	HI_JETPACK,
	HI_HEALTHDISP,
	HI_AMMODISP,
	HI_EWEB,
	HI_CLOAK,
	HI_NUM_HOLDABLE
};

enum ctfMsg_t {
	CTFMESSAGE_FRAGGED_FLAG_CARRIER,
	CTFMESSAGE_FLAG_RETURNED,
	CTFMESSAGE_PLAYER_RETURNED_FLAG,
	CTFMESSAGE_PLAYER_CAPTURED_FLAG,
	CTFMESSAGE_PLAYER_GOT_FLAG
};

enum pdSounds_t {
	PDSOUND_NONE,
	PDSOUND_PROTECTHIT,
	PDSOUND_PROTECT,
	PDSOUND_ABSORBHIT,
	PDSOUND_ABSORB,
	PDSOUND_FORCEJUMP,
	PDSOUND_FORCEGRIP
};

enum entity_event_t : int32_t {
	EV_NONE,
	EV_CLIENTJOIN,
	EV_FOOTSTEP,
	EV_FOOTSTEP_METAL,
	EV_FOOTSPLASH,
	EV_FOOTWADE,
	EV_SWIM,
	EV_STEP_4,
	EV_STEP_8,
	EV_STEP_12,
	EV_STEP_16,
	EV_FALL,
	EV_JUMP_PAD,                // boing sound at origin, jump sound on player
	EV_GHOUL2_MARK,             // create a projectile impact mark on something with a client-side g2 instance.
	EV_GLOBAL_DUEL,
	EV_PRIVATE_DUEL,
	EV_JUMP,
	EV_ROLL,
	EV_WATER_TOUCH,             // foot touches
	EV_WATER_LEAVE,             // foot leaves
	EV_WATER_UNDER,             // head touches
	EV_WATER_CLEAR,             // head leaves
	EV_ITEM_PICKUP,             // normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP,      // powerup / team sounds are broadcast to everyone
	EV_NOAMMO,
	EV_CHANGE_WEAPON,
	EV_FIRE_WEAPON,
	EV_ALT_FIRE,
	EV_SABER_ATTACK,
	EV_SABER_HIT,
	EV_SABER_BLOCK,
	EV_SABER_CLASHFLARE,
	EV_SABER_UNHOLSTER,
	EV_BECOME_JEDIMASTER,
	EV_DISRUPTOR_MAIN_SHOT,
	EV_DISRUPTOR_SNIPER_SHOT,
	EV_DISRUPTOR_SNIPER_MISS,
	EV_DISRUPTOR_HIT,
	EV_DISRUPTOR_ZOOMSOUND,
	EV_PREDEFSOUND,
	EV_TEAM_POWER,
	EV_SCREENSHAKE,
	EV_LOCALTIMER,
	EV_USE,                     // +Use key
	EV_USE_ITEM0,
	EV_USE_ITEM1,
	EV_USE_ITEM2,
	EV_USE_ITEM3,
	EV_USE_ITEM4,
	EV_USE_ITEM5,
	EV_USE_ITEM6,
	EV_USE_ITEM7,
	EV_USE_ITEM8,
	EV_USE_ITEM9,
	EV_USE_ITEM10,
	EV_USE_ITEM11,
	EV_USE_ITEM12,
	EV_USE_ITEM13,
	EV_USE_ITEM14,
	EV_USE_ITEM15,
	EV_ITEMUSEFAIL,
	EV_ITEM_RESPAWN,
	EV_ITEM_POP,
	EV_PLAYER_TELEPORT_IN,
	EV_PLAYER_TELEPORT_OUT,
	EV_GRENADE_BOUNCE,          // eventParm will be the soundindex
	EV_MISSILE_STICK,           // eventParm will be the soundindex
	EV_PLAY_EFFECT,
	EV_PLAY_EFFECT_ID,
	EV_PLAY_PORTAL_EFFECT_ID,
	EV_PLAYDOORSOUND,
	EV_PLAYDOORLOOPSOUND,
	EV_BMODEL_SOUND,
	EV_MUTE_SOUND,
	EV_VOICECMD_SOUND,
	EV_GENERAL_SOUND,
	EV_GLOBAL_SOUND,            // no attenuation
	EV_GLOBAL_TEAM_SOUND,
	EV_ENTITY_SOUND,
	EV_PLAY_ROFF,
	EV_GLASS_SHATTER,
	EV_DEBRIS,
	EV_MISC_MODEL_EXP,
	EV_CONC_ALT_IMPACT,
	EV_MISSILE_HIT,
	EV_MISSILE_MISS,
	EV_MISSILE_MISS_METAL,
	EV_BULLET,                  // otherEntity is the shooter
	EV_PAIN,
	EV_DEATH1,
	EV_DEATH2,
	EV_DEATH3,
	EV_OBITUARY,
	EV_POWERUP_QUAD,
	EV_POWERUP_BATTLESUIT,
	EV_FORCE_DRAINED,
	EV_GIB_PLAYER,              // gib a previously living player
	EV_SCOREPLUM,               // score plum
	EV_CTFMESSAGE,
	EV_BODYFADE,
	EV_DESTROY_GHOUL2_INSTANCE, //
	EV_DESTROY_WEAPON_MODEL,
	EV_GIVE_NEW_RANK,
	EV_SET_FREE_SABER,
	EV_SET_FORCE_DISABLE,
	EV_WEAPON_CHARGE,
	EV_WEAPON_CHARGE_ALT,
	EV_SHIELD_HIT,
	EV_DEBUG_LINE,
	EV_TESTLINE,
	EV_STOPLOOPINGSOUND,
	EV_STARTLOOPINGSOUND,
	EV_TAUNT,
	EV_ANGER1,                  // Say when acquire an enemy when didn't have one before
	EV_ANGER2,
	EV_ANGER3,
	EV_VICTORY1,                // Say when killed an enemy
	EV_VICTORY2,
	EV_VICTORY3,
	EV_CONFUSE1,                //Say when confused
	EV_CONFUSE2,
	EV_CONFUSE3,
	EV_PUSHED1,                 //Say when pushed
	EV_PUSHED2,
	EV_PUSHED3,
	EV_CHOKE1,                  //Say when choking
	EV_CHOKE2,
	EV_CHOKE3,
	EV_FFWARN,                  // ffire founds
	EV_FFTURN,
	//extra sounds for ST
	EV_CHASE1,
	EV_CHASE2,
	EV_CHASE3,
	EV_COVER1,
	EV_COVER2,
	EV_COVER3,
	EV_COVER4,
	EV_COVER5,
	EV_DETECTED1,
	EV_DETECTED2,
	EV_DETECTED3,
	EV_DETECTED4,
	EV_DETECTED5,
	EV_LOST1,
	EV_OUTFLANK1,
	EV_OUTFLANK2,
	EV_ESCAPING1,
	EV_ESCAPING2,
	EV_ESCAPING3,
	EV_GIVEUP1,
	EV_GIVEUP2,
	EV_GIVEUP3,
	EV_GIVEUP4,
	EV_LOOK1,
	EV_LOOK2,
	EV_SIGHT1,
	EV_SIGHT2,
	EV_SIGHT3,
	EV_SOUND1,
	EV_SOUND2,
	EV_SOUND3,
	EV_SUSPICIOUS1,
	EV_SUSPICIOUS2,
	EV_SUSPICIOUS3,
	EV_SUSPICIOUS4,
	EV_SUSPICIOUS5,
	//extra sounds for Jedi
	EV_COMBAT1,
	EV_COMBAT2,
	EV_COMBAT3,
	EV_JDETECTED1,
	EV_JDETECTED2,
	EV_JDETECTED3,
	EV_TAUNT1,
	EV_TAUNT2,
	EV_TAUNT3,
	EV_JCHASE1,
	EV_JCHASE2,
	EV_JCHASE3,
	EV_JLOST1,
	EV_JLOST2,
	EV_JLOST3,
	EV_DEFLECT1,
	EV_DEFLECT2,
	EV_DEFLECT3,
	EV_GLOAT1,
	EV_GLOAT2,
	EV_GLOAT3,
	EV_PUSHFAIL,
	EV_NUM_ENTITY_EVENTS
}; // There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)

enum global_team_sound_t {
	GTS_RED_CAPTURE,
	GTS_BLUE_CAPTURE,
	GTS_RED_RETURN,
	GTS_BLUE_RETURN,
	GTS_RED_TAKEN,
	GTS_BLUE_TAKEN,
	GTS_REDTEAM_SCORED,
	GTS_BLUETEAM_SCORED,
	GTS_REDTEAM_TOOK_LEAD,
	GTS_BLUETEAM_TOOK_LEAD,
	GTS_TEAMS_ARE_TIED
};

enum team_t : int32_t {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,
	TEAM_NUM_TEAMS
};

enum duelTeam_t {
	DUELTEAM_FREE,
	DUELTEAM_LONE,
	DUELTEAM_DOUBLE,
	DUELTEAM_SINGLE, // for regular duel matches (not power duel)
};

enum teamtask_t {
	TEAMTASK_NONE,
	TEAMTASK_OFFENSE,
	TEAMTASK_DEFENSE,
	TEAMTASK_PATROL,
	TEAMTASK_FOLLOW,
	TEAMTASK_RETRIEVE,
	TEAMTASK_ESCORT,
	TEAMTASK_CAMP
};

enum meansOfDeath_t {
	MOD_UNKNOWN,
	MOD_STUN_BATON,
	MOD_MELEE,
	MOD_SABER,
	MOD_BRYAR_PISTOL,
	MOD_BRYAR_PISTOL_ALT,
	MOD_BLASTER,
	MOD_TURBLAST,
	MOD_DISRUPTOR,
	MOD_DISRUPTOR_SPLASH,
	MOD_DISRUPTOR_SNIPER,
	MOD_BOWCASTER,
	MOD_REPEATER,
	MOD_REPEATER_ALT,
	MOD_REPEATER_ALT_SPLASH,
	MOD_DEMP2,
	MOD_DEMP2_ALT,
	MOD_FLECHETTE,
	MOD_FLECHETTE_ALT_SPLASH,
	MOD_ROCKET,
	MOD_ROCKET_SPLASH,
	MOD_ROCKET_HOMING,
	MOD_ROCKET_HOMING_SPLASH,
	MOD_THERMAL,
	MOD_THERMAL_SPLASH,
	MOD_TRIP_MINE_SPLASH,
	MOD_TIMED_MINE_SPLASH,
	MOD_DET_PACK_SPLASH,
	MOD_CONC,
	MOD_CONC_ALT,
	MOD_FORCE_DARK,
	MOD_SENTRY,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,
	MOD_TEAM_CHANGE,
	MOD_MAX
};

enum itemType_t {
	IT_BAD,
	IT_WEAPON,             // EFX: rotate + upscale + minlight
	IT_AMMO,               // EFX: rotate
	IT_ARMOR,              // EFX: rotate + minlight
	IT_HEALTH,             // EFX: static external sphere + rotating internal
	IT_POWERUP,            // EFX: rotate + external ring that rotates (instant on, timer based)
	IT_HOLDABLE,           // EFX: rotate + bob (single use, holdable item)
	IT_PERSISTANT_POWERUP,
	IT_TEAM
};

enum dmFlags_e : uint32_t {
	DF_NO_FALLING =   0x0001,
	DF_FIXED_FOV =    0x0002,
	DF_NO_FOOTSTEPS = 0x0004,
};

enum entityType_t {
	ET_GENERAL,
	ET_PLAYER,
	ET_ITEM,
	ET_MISSILE,
	ET_SPECIAL,          // rww - force fields
	ET_HOLOCRON,         // rww - holocron icon displays
	ET_MOVER,
	ET_BEAM,
	ET_PORTAL,
	ET_SPEAKER,
	ET_PUSH_TRIGGER,
	ET_TELEPORT_TRIGGER, //
	ET_INVISIBLE,
	ET_TEAM,
	ET_BODY,
	ET_TERRAIN,
	ET_FX,
	ET_EVENTS            // any of the EV_* events can be added freestanding by setting eType to ET_EVENTS + eventNum this avoids having to set eFlags and eventNum
};

#ifdef LS_NONE
#undef LS_NONE
#endif

enum saberMoveName_t {
	//totally invalid
	LS_INVALID	= -1,
	// Invalid, or saber not armed
	LS_NONE		= 0,
	// General movements with saber
	LS_READY,
	LS_DRAW,
	LS_PUTAWAY,
	// Attacks
	LS_A_TL2BR,//4
	LS_A_L2R,
	LS_A_BL2TR,
	LS_A_BR2TL,
	LS_A_R2L,
	LS_A_TR2BL,
	LS_A_T2B,
	LS_A_BACKSTAB,
	LS_A_BACK,
	LS_A_BACK_CR,
	LS_ROLL_STAB,
	LS_A_LUNGE,
	LS_A_JUMP_T__B_,
	LS_A_FLIP_STAB,
	LS_A_FLIP_SLASH,
	LS_JUMPATTACK_DUAL,
	LS_JUMPATTACK_ARIAL_LEFT,
	LS_JUMPATTACK_ARIAL_RIGHT,
	LS_JUMPATTACK_CART_LEFT,
	LS_JUMPATTACK_CART_RIGHT,
	LS_JUMPATTACK_STAFF_LEFT,
	LS_JUMPATTACK_STAFF_RIGHT,
	LS_BUTTERFLY_LEFT,
	LS_BUTTERFLY_RIGHT,
	LS_A_BACKFLIP_ATK,
	LS_SPINATTACK_DUAL,
	LS_SPINATTACK,
	LS_LEAP_ATTACK,
	LS_SWOOP_ATTACK_RIGHT,
	LS_SWOOP_ATTACK_LEFT,
	LS_TAUNTAUN_ATTACK_RIGHT,
	LS_TAUNTAUN_ATTACK_LEFT,
	LS_KICK_F,
	LS_KICK_B,
	LS_KICK_R,
	LS_KICK_L,
	LS_KICK_S,
	LS_KICK_BF,
	LS_KICK_RL,
	LS_KICK_F_AIR,
	LS_KICK_B_AIR,
	LS_KICK_R_AIR,
	LS_KICK_L_AIR,
	LS_STABDOWN,
	LS_STABDOWN_STAFF,
	LS_STABDOWN_DUAL,
	LS_DUAL_SPIN_PROTECT,
	LS_STAFF_SOULCAL,
	LS_A1_SPECIAL,
	LS_A2_SPECIAL,
	LS_A3_SPECIAL,
	LS_UPSIDE_DOWN_ATTACK,
	LS_PULL_ATTACK_STAB,
	LS_PULL_ATTACK_SWING,
	LS_SPINATTACK_ALORA,
	LS_DUAL_FB,
	LS_DUAL_LR,
	LS_HILT_BASH,
	//starts
	LS_S_TL2BR,//26
	LS_S_L2R,
	LS_S_BL2TR,//# Start of attack chaining to SLASH LR2UL
	LS_S_BR2TL,//# Start of attack chaining to SLASH LR2UL
	LS_S_R2L,
	LS_S_TR2BL,
	LS_S_T2B,
	//returns
	LS_R_TL2BR,//33
	LS_R_L2R,
	LS_R_BL2TR,
	LS_R_BR2TL,
	LS_R_R2L,
	LS_R_TR2BL,
	LS_R_T2B,
	//transitions
	LS_T1_BR__R,//40
	LS_T1_BR_TR,
	LS_T1_BR_T_,
	LS_T1_BR_TL,
	LS_T1_BR__L,
	LS_T1_BR_BL,
	LS_T1__R_BR,//46
	LS_T1__R_TR,
	LS_T1__R_T_,
	LS_T1__R_TL,
	LS_T1__R__L,
	LS_T1__R_BL,
	LS_T1_TR_BR,//52
	LS_T1_TR__R,
	LS_T1_TR_T_,
	LS_T1_TR_TL,
	LS_T1_TR__L,
	LS_T1_TR_BL,
	LS_T1_T__BR,//58
	LS_T1_T___R,
	LS_T1_T__TR,
	LS_T1_T__TL,
	LS_T1_T___L,
	LS_T1_T__BL,
	LS_T1_TL_BR,//64
	LS_T1_TL__R,
	LS_T1_TL_TR,
	LS_T1_TL_T_,
	LS_T1_TL__L,
	LS_T1_TL_BL,
	LS_T1__L_BR,//70
	LS_T1__L__R,
	LS_T1__L_TR,
	LS_T1__L_T_,
	LS_T1__L_TL,
	LS_T1__L_BL,
	LS_T1_BL_BR,//76
	LS_T1_BL__R,
	LS_T1_BL_TR,
	LS_T1_BL_T_,
	LS_T1_BL_TL,
	LS_T1_BL__L,
	//Bounces
	LS_B1_BR,
	LS_B1__R,
	LS_B1_TR,
	LS_B1_T_,
	LS_B1_TL,
	LS_B1__L,
	LS_B1_BL,
	//Deflected attacks
	LS_D1_BR,
	LS_D1__R,
	LS_D1_TR,
	LS_D1_T_,
	LS_D1_TL,
	LS_D1__L,
	LS_D1_BL,
	LS_D1_B_,
	//Reflected attacks
	LS_V1_BR,
	LS_V1__R,
	LS_V1_TR,
	LS_V1_T_,
	LS_V1_TL,
	LS_V1__L,
	LS_V1_BL,
	LS_V1_B_,
	// Broken parries
	LS_H1_T_,//
	LS_H1_TR,
	LS_H1_TL,
	LS_H1_BR,
	LS_H1_B_,
	LS_H1_BL,
	// Knockaways
	LS_K1_T_,//
	LS_K1_TR,
	LS_K1_TL,
	LS_K1_BR,
	LS_K1_BL,
	// Parries
	LS_PARRY_UP,//
	LS_PARRY_UR,
	LS_PARRY_UL,
	LS_PARRY_LR,
	LS_PARRY_LL,
	// Projectile Reflections
	LS_REFLECT_UP,//
	LS_REFLECT_UR,
	LS_REFLECT_UL,
	LS_REFLECT_LR,
	LS_REFLECT_LL,

	LS_MOVE_MAX//
};

enum saberQuadrant_t {
	Q_BR,
	Q_R,
	Q_TR,
	Q_T,
	Q_TL,
	Q_L,
	Q_BL,
	Q_B,
	Q_NUM_QUADS
};

enum saberType_t {
	SABER_NONE = 0,
	SABER_SINGLE,
	SABER_STAFF,
	SABER_DAGGER,
	SABER_BROAD,
	SABER_PRONG,
	SABER_ARC,
	SABER_SAI,
	SABER_CLAW,
	SABER_LANCE,
	SABER_STAR,
	SABER_TRIDENT,
	SABER_SITH_SWORD,
	NUM_SABERS
};

enum saber_styles_t {
	SS_NONE = 0,
	SS_FAST,
	SS_MEDIUM,
	SS_STRONG,
	SS_DUAL,
	SS_STAFF,
	SS_NUM_SABER_STYLES
};

// cjk_saberSpecialMoves bit-field
enum saberSpecialMove_t : uint32_t {
	SSM_ROLL =      0x0001,
	SSM_ROLLSTAB =  0x0002,
	SSM_CARTWHEEL = 0x0004,
	SSM_FLIP =      0x0008,
	SSM_WALLRUN =   0x0010,
	SSM_WALLGRAB =  0x0020,
	SSM_STABDOWN =  0x0040,
	SSM_MIRROR =    0x0080,
	SSM_KICK =      0x0100,
};

// cjk_saberTweaks bit-fields
enum saberTweaks_t : uint32_t {
	ST_TRANSITION_DAMAGE = 0x0001,
	ST_RETURN_DAMAGE =     0x0002,
	ST_NO_DEACTIVATE =     0x0004,
};

// ======================================================================
// STRUCT
// ======================================================================

#pragma pack(push, 1)
struct animation_t {
	unsigned short firstFrame;
	unsigned short numFrames;
	short          frameLerp;  // msec between frames
	//initialLerp is abs(frameLerp)
	signed char    loopFrames; // 0 to numFrames
};
#pragma pack(pop)

struct animevent_t {
	animEventType_t  eventType;
	unsigned short   keyFrame;                  // Frame to play event on
	signed short     eventData[AED_ARRAY_SIZE];	// Unique IDs, can be soundIndex of sound file to play OR effect index or footstep type, etc.
	char            *stringData;                // we allow storage of one string, temporarily (in case we have to look up an index later, then make sure to set stringData to nullptr so we only do the look-up once)
};

struct bgLoadedAnim_t {
	char         filename[MAX_QPATH];
	animation_t *anims;
};

struct bgLoadedEvents_t {
	char        filename[MAX_QPATH];
	animevent_t torsoAnimEvents[MAX_ANIM_EVENTS];
	animevent_t legsAnimEvents[MAX_ANIM_EVENTS];
	bool    eventsParsed;
};

struct bgEntity_t {
	//CJKFIXME: bgentity_t substruct
	entityState_t  s;
	playerState_t *playerState;
	void          *ghoul2;        // g2 instance
	int           localAnimIndex; // index locally (game/cgame) to anim data for this skel
	vec3_t         modelScale;    // needed for g2 collision
	//Data type(s) must directly correspond to the head of the gentity and centity structures
};

struct pmove_t {
	// state (in / out)
	playerState_t *ps;
	void          *ghoul2;
	int            g2Bolts_LFoot;
	int            g2Bolts_RFoot;
	vec3_t         modelScale;
	bool       nonHumanoid;         // hacky bool so we know if we're dealing with a nonhumanoid (which is probably a rockettrooper)
	// command (in)
	usercmd_t      cmd;
	int            tracemask;           // collide against these types of surfaces
	int            debugLevel;          // if set, diagnostic output will be printed
	bool       noFootsteps;         // if the game is setup for no footsteps by the server
	bool       gauntletHit;         // true if a gauntlet attack would actually hit something
	int            framecount;
	// results (out)
	int            numtouch;
	int            touchents[MAXTOUCH];
	int            useEvent;
	vec3_t         mins, maxs;          // bounding box size
	int            watertype;
	int            waterlevel;
	int            gametype;
	int            debugMelee;
	int            stepSlideFix;
	int            noSpecMove;
	animation_t   *animations;
	float          xyspeed;
	int            pmove_fixed;
	int            pmove_float;
	int            pmove_msec;
	void           (*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
	int            (*pointcontents)( const vec3_t point, int passEntityNum );
	int            checkDuelLoss;
	bgEntity_t    *baseEnt;             // base address of the entity array (g_entities or cg_entities)
	int            entSize;             // size of the struct (gentity_t or centity_t) so things can be dynamic
	int32_t        saberSpecialMoves;
	int32_t        saberTweaks;
};

struct gitem_t {
	char       *classname;	// spawning name
	char       *pickup_sound;
	char       *world_model[MAX_ITEM_MODELS];
	char       *view_model;
	char       *icon;
	int         quantity;		// for ammo how much, or duration of powerup
	itemType_t  giType;			// IT_* flags
	int         giTag;
	char       *precaches;		// string of all models and images this item will use
	char       *sounds;		// string of all sounds this item will use
	char       *description;
};

struct saberMoveData_t {
	char            *name;
	int              animToUse;
	int              startQuad;
	int              endQuad;
	unsigned         animSetFlags;
	int              blendTime;
	int              blocking;
	saberMoveName_t chain_idle;   // What move to call if the attack button is not pressed at the end of this anim
	saberMoveName_t chain_attack; // What move to call if the attack button (and nothing else) is pressed
	int				trailLength;
};

struct saberTrail_t {
	int      inAction;     // controls whether should we even consider starting one
	int      duration;     // how long each trail seg stays in existence
	int      lastTime;     // time a saber segement was last stored
	vec3_t   base, dualbase;
	vec3_t   tip, dualtip;
	bool haveOldPos[2];
	vec3_t   oldPos[2];
	vec3_t   oldNormal[2]; // store this in case we don't have a connect-the-dots situation...then we'll need the normal to project a mark blob onto the impact point
};

struct bladeInfo_t {
	bool       active;
	saber_colors_t color;
	float          radius;
	float          length, lengthMax, lengthOld;
	float          desiredLength;
	vec3_t         muzzlePoint, muzzlePointOld;
	vec3_t         muzzleDir, muzzleDirOld;
	saberTrail_t   trail;
	int            hitWallDebounceTime;
	int            storageTime;
	int            extendDebounce;
};

struct saberInfo_t {
	char           name[SABER_NAME_LENGTH];                // entry in sabers.cfg, if any
	char           fullName[SABER_NAME_LENGTH];            // the "Proper Name" of the saber, shown in UI
	saberType_t    type;                                   // none, single or staff
	char           model[MAX_QPATH];                       // hilt model
	qhandle_t      skin;                                   // registered skin id
	int            soundOn;                                // game soundindex for turning on sound
	int            soundLoop;                              // game soundindex for hum/loop sound
	int            soundOff;                               // game soundindex for turning off sound
	int            numBlades;
	bladeInfo_t    blade[MAX_BLADES];                      // blade info - like length, trail, origin, dir, etc.
	int            stylesLearned;                          // styles you get when you get this saber, if any
	int            stylesForbidden;                        // styles you cannot use with this saber, if any
	int            maxChain;                               // how many moves can be chained in a row with this weapon (-1 is infinite, 0 is use default behavior)
	int            forceRestrictions;                      // force powers that cannot be used while this saber is on (bitfield) - FIXME: maybe make this a limit on the max level, per force power, that can be used with this type?
	int            lockBonus;                              // in saberlocks, this type of saber pushes harder or weaker
	int            parryBonus;                             // added to strength of parry with this saber
	int            breakParryBonus, breakParryBonus2;      // added to strength when hit a parry
	int            disarmBonus, disarmBonus2;              // added to disarm chance when win saberlock or have a good parry (knockaway)
	saber_styles_t singleBladeStyle;                       // makes it so that you use a different style if you only have the first blade active
	qhandle_t      spinSound;                              // none - if set, plays this sound as it spins when thrown
	qhandle_t      swingSound[3];                          // none - if set, plays one of these 3 sounds when swung during an attack - NOTE: must provide all 3!!!
	float          moveSpeedScale;                         // 1.0 - you move faster/slower when using this saber
	float          animSpeedScale;                         // 1.0 - plays normal attack animations faster/slower
	int            kataMove;                               // LS_INVALID - if set, player will execute this move when they press both attack buttons at the same time
	int            lungeAtkMove;                           // LS_INVALID - if set, player will execute this move when they crouch+fwd+attack
	int            jumpAtkUpMove;                          // LS_INVALID - if set, player will execute this move when they jump+attack
	int            jumpAtkFwdMove;                         // LS_INVALID - if set, player will execute this move when they jump+fwd+attack
	int            jumpAtkBackMove;                        // LS_INVALID - if set, player will execute this move when they jump+back+attack
	int            jumpAtkRightMove;                       // LS_INVALID - if set, player will execute this move when they jump+rightattack
	int            jumpAtkLeftMove;                        // LS_INVALID - if set, player will execute this move when they jump+left+attack
	int            readyAnim;                              // -1 - anim to use when standing idle
	int            drawAnim;                               // -1 - anim to use when drawing weapon
	int            putawayAnim;                            // -1 - anim to use when putting weapon away
	int            tauntAnim;                              // -1 - anim to use when hit "taunt"
	int            bowAnim;                                // -1 - anim to use when hit "bow"
	int            meditateAnim;                           // -1 - anim to use when hit "meditate"
	int            flourishAnim;                           // -1 - anim to use when hit "flourish"
	int            gloatAnim;                              // -1 - anim to use when hit "gloat"
	int            bladeStyle2Start;                       // 0 - if set, blades from this number and higher use the following values (otherwise, they use the normal values already set)
	int            trailStyle, trailStyle2;                // 0 - default (0) is normal, 1 is a motion blur and 2 is no trail at all (good for real-sword type mods)
	int            g2MarksShader, g2MarksShader2;          // none - if set, the game will use this shader for marks on enemies instead of the default "gfx/damage/saberglowmark"
	int            g2WeaponMarkShader, g2WeaponMarkShader2;// none - if set, the game will ry to project this shader onto the weapon when it damages a person (good for a blood splatter on the weapon)
	qhandle_t      hitSound[3], hit2Sound[3];              // none - if set, plays one of these 3 sounds when saber hits a person - NOTE: must provide all 3!!!
	qhandle_t      blockSound[3], block2Sound[3];          // none - if set, plays one of these 3 sounds when saber/sword hits another saber/sword - NOTE: must provide all 3!!!
	qhandle_t      bounceSound[3], bounce2Sound[3];        // none - if set, plays one of these 3 sounds when saber/sword hits a wall and bounces off (must set bounceOnWall to 1 to use these sounds) - NOTE: must provide all 3!!!
	int            blockEffect, blockEffect2;              // none - if set, plays this effect when the saber/sword hits another saber/sword (instead of "saber/saber_block.efx")
	int            hitPersonEffect, hitPersonEffect2;      // none - if set, plays this effect when the saber/sword hits a person (instead of "saber/blood_sparks_mp.efx")
	int            hitOtherEffect, hitOtherEffect2;        // none - if set, plays this effect when the saber/sword hits something else damagable (instead of "saber/saber_cut.efx")
	int            bladeEffect, bladeEffect2;              // none - if set, plays this effect at the blade tag
	float          knockbackScale, knockbackScale2;        // 0 - if non-zero, uses damage done to calculate an appropriate amount of knockback
	float          damageScale, damageScale2;              // 1 - scale up or down the damage done by the saber
	float          splashRadius, splashRadius2;            // 0 - radius of splashDamage
	int            splashDamage, splashDamage2;            // 0 - amount of splashDamage, 100% at a distance of 0, 0% at a distance = splashRadius
	float          splashKnockback, splashKnockback2;      // 0 - amount of splashKnockback, 100% at a distance of 0, 0% at a distance = splashRadius
};

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

#ifndef _GAME
extern bgLoadedEvents_t bgAllEvents[MAX_ANIM_FILES];
extern int              bgNumAnimEvents;
#endif

extern animation_t bgHumanoidAnimations[MAX_TOTALANIMATIONS];
extern bgLoadedAnim_t bgAllAnims[MAX_ANIM_FILES];
extern bool BGPAFtextLoaded;
extern const char *forceMasteryLevels[NUM_FORCE_MASTERY_LEVELS];
extern const char *bg_customSiegeSoundNames[MAX_CUSTOM_SIEGE_SOUNDS];
extern const char *bgToggleableSurfaces[BG_NUM_TOGGLEABLE_SURFACES];
extern const char *gametypeStringShort[GT_MAX_GAME_TYPE];
extern const int bgToggleableSurfaceDebris[BG_NUM_TOGGLEABLE_SURFACES];
extern gitem_t bg_itemlist[];
extern int bg_numItems;
extern int bgForcePowerCost[NUM_FORCE_POWERS][NUM_FORCE_POWER_LEVELS];
extern const int forceMasteryPoints[NUM_FORCE_MASTERY_LEVELS];
extern const int forcePowerDarkLight[NUM_FORCE_POWERS];
extern const int forcePowerSorted[NUM_FORCE_POWERS];
extern const int WeaponAttackAnim[WP_NUM_WEAPONS];
extern const int WeaponReadyAnim[WP_NUM_WEAPONS];
extern pmove_t *pm;
extern const saberMoveData_t saberMoveData[LS_MOVE_MAX];
extern const stringID_table_t animEventTypeTable[MAX_ANIM_EVENTS+1];
extern const stringID_table_t footstepTypeTable[NUM_FOOTSTEP_TYPES+1];
extern const vec3_t WP_MuzzlePoint[WP_NUM_WEAPONS];

// ======================================================================
// FUNCTION
// ======================================================================

#ifndef _GAME
int BG_ParseAnimationEvtFile(const char* as_filename, int animFileIndex, int eventFileIndex);
#endif

bgEntity_t* PM_BGEntForNum(int num);
bool BG_CanItemBeGrabbed(int gametype, const entityState_t* ent, const playerState_t* ps);
bool BG_CanUseFPNow(int gametype, playerState_t* ps, int time, forcePowers_t power);
bool BG_CheckIncrementLockAnim(int anim, int winOrLose);
bool BG_DirectFlippingAnim(int anim);
bool BG_FlippingAnim(int anim);
bool BG_FullBodyTauntAnim(int anim);
bool BG_GetRootSurfNameWithVariant(void* ghoul2, const char* rootSurfName, char* returnSurfName, int returnSize);
bool BG_HasAnimation(int animIndex, int animation);
bool BG_HasYsalamiri(int gametype, playerState_t* ps);
bool BG_InBackFlip(int anim);
bool BG_InDeathAnim(int anim);
bool BG_InExtraDefenseSaberMove(int move);
bool BG_InKataAnim(int anim);
bool BG_InKnockDown(int anim);
bool BG_InKnockDownOnGround(playerState_t* ps);
bool BG_InKnockDownOnly(int anim);
bool BG_InReboundHold(int anim);
bool BG_InReboundJump(int anim);
bool BG_InReboundRelease(int anim);
bool BG_InRoll(playerState_t* ps, int anim);
bool BG_InSaberLock(int anim);
bool BG_InSaberLockOld(int anim);
bool BG_InSaberStandAnim(int anim);
bool BG_InSpecialJump(int anim);
bool BG_IsItemSelectable(playerState_t* ps, int item);
bool BG_KickingAnim(int anim);
bool BG_KickMove(int move);
bool BG_KnockDownable(playerState_t* ps);
bool BG_LegalizedForcePowers(char* powerOut, size_t powerOutSize, int maxRank, bool freeSaber, int teamForce, int gametype, int fpDisabled);
bool BG_OutOfMemory(void);
bool BG_PlayerTouchesItem(playerState_t* ps, entityState_t* item, int atTime);
bool BG_SaberInAttack(int move);
bool BG_SaberInIdle(int move);
bool BG_SaberInKata(int saberMove);
bool BG_SaberInReturn(int move);
bool BG_SaberInSpecial(int move);
bool BG_SaberInSpecialAttack(int anim);
bool BG_SaberInTransitionAny(int move);
bool BG_SabersOff(playerState_t* ps);
bool BG_SI_Active(saberInfo_t* saber);
bool BG_SpinningSaberAnim(int anim);
bool BG_StabDownAnim(int anim);
bool BG_SuperBreakLoseAnim(int anim);
bool BG_SuperBreakWinAnim(int anim);
bool BG_ValidateSkinForTeam(const char* modelName, char* skinName, int team, float* colors);
bool PM_InKnockDown(playerState_t* ps);
bool PM_RunningAnim(int anim);
bool PM_SaberInBounce(int move);
bool PM_SaberInBrokenParry(int move);
bool PM_SaberInDeflect(int move);
bool PM_SaberInReturn(int move);
bool PM_SaberInStart(int move);
bool PM_SaberInTransition(int move);
bool PM_WalkingAnim(int anim);
bool WP_SaberBladeUseSecondBladeStyle(saberInfo_t* saber, int bladeNum);
bool WP_SaberParseParm(const char* saberName, const char* parmname, char* saberData);
bool WP_SaberStyleValidForSaber(saberInfo_t* saber1, saberInfo_t* saber2, int saberHolstered, int saberAnimLevel);
bool WP_UseFirstValidSaberStyle(saberInfo_t* saber1, saberInfo_t* saber2, int saberHolstered, int* saberAnimLevel);
char* BG_StringAlloc(const char* source);
const char* BG_GetGametypeString(int gametype);
const char* SaberColorToString(saber_colors_t color);
float BG_SI_Length(saberInfo_t* saber);
float BG_SI_LengthMax(saberInfo_t* saber);
float vectoyaw(const vec3_t vec);
gitem_t* BG_FindItem(const char* classname);
gitem_t* BG_FindItemForHoldable(holdable_t pw);
gitem_t* BG_FindItemForPowerup(powerup_t pw);
gitem_t* BG_FindItemForWeapon(weapon_t weapon);
int BG_AnimLength(int index, animNumber_t anim);
int BG_BrokenParryForAttack(int move);
int BG_BrokenParryForParry(int move);
int BG_EmplacedView(vec3_t baseAngles, vec3_t angles, float* newYaw, float constraint);
int BG_GetGametypeForString(const char* gametype);
int BG_GetItemIndexByTag(int tag, int type);
int BG_InGrappleMove(int anim);
int BG_KnockawayForParry(int move);
int BG_ParseAnimationFile(const char* filename, animation_t* animSet, bool isHumanoid);
int BG_PickAnim(int animIndex, int minAnim, int maxAnim);
int PM_SaberBounceForAttack(int move);
int PM_SaberDeflectionForQuad(int quad);
saber_colors_t TranslateSaberColor(const char* name);
saberType_t TranslateSaberType(const char* name);
void BG_AddPredictableEventToPlayerstate(int newEvent, int eventParm, playerState_t* ps);
void BG_BLADE_ActivateTrail(bladeInfo_t* blade, float duration);
void BG_BLADE_DeactivateTrail(bladeInfo_t* blade, float duration);
void BG_ClearAnimsets(void);
void BG_ClearRocketLock(playerState_t* ps);
void BG_CycleInven(playerState_t* ps, int direction);
void BG_EvaluateTrajectory(const trajectory_t* tr, int atTime, vec3_t result);
void BG_EvaluateTrajectoryDelta(const trajectory_t* tr, int atTime, vec3_t result);
void BG_ForcePowerDrain(playerState_t* ps, forcePowers_t forcePower, int overrideAmt);
void BG_G2ATSTAngles(void* ghoul2, int time, vec3_t cent_lerpAngles);
void BG_G2PlayerAngles(void* ghoul2, int motionBolt, entityState_t* cent, int time, vec3_t cent_lerpOrigin, vec3_t cent_lerpAngles, matrix3_t legs, vec3_t legsAngles, bool* tYawing, bool* tPitching, bool* lYawing, float* tYawAngle, float* tPitchAngle, float* lYawAngle, int frametime, vec3_t turAngles, vec3_t modelScale, int ciLegs, int ciTorso, int* corrTime, vec3_t lookAngles, vec3_t lastHeadAngles, int lookTime, entityState_t* emplaced, int* crazySmoothFactor);
void BG_GiveMeVectorFromMatrix(mdxaBone_t* boltMatrix, int flags, vec3_t vec);
void BG_IK_MoveArm(void* ghoul2, int lHandBolt, int time, entityState_t* ent, int basePose, vec3_t desiredPos, bool* ikInProgress, vec3_t origin, vec3_t angles, vec3_t scale, int blendTime, bool forceHalt);
void BG_InitAnimsets(void);
void BG_PlayerStateToEntityState(playerState_t* ps, entityState_t* s, bool snap);
void BG_PlayerStateToEntityStateExtraPolate(playerState_t* ps, entityState_t* s, int time, bool snap);
void BG_SaberStartTransAnim(int clientNum, int saberAnimLevel, int weapon, int anim, float* animSpeed, int broken);
void BG_SetAnim(playerState_t* ps, animation_t* animations, int setAnimParts, int anim, int setAnimFlags);
void BG_SI_Activate(saberInfo_t* saber);
void BG_SI_ActivateTrail(saberInfo_t* saber, float duration);
void BG_SI_BladeActivate(saberInfo_t* saber, int iBlade, bool bActive);
void BG_SI_Deactivate(saberInfo_t* saber);
void BG_SI_DeactivateTrail(saberInfo_t* saber, float duration);
void BG_SI_SetDesiredLength(saberInfo_t* saber, float len, int bladeNum);
void BG_SI_SetLength(saberInfo_t* saber, float length);
void BG_SI_SetLengthGradual(saberInfo_t* saber, int time);
void BG_TempFree(int size);
void BG_TouchJumpPad(playerState_t* ps, entityState_t* jumppad);
void PM_UpdateViewAngles(playerState_t* ps, const usercmd_t* cmd);
void Pmove(pmove_t* pmove);
void WP_SaberGetHiltInfo(const char* singleHilts[MAX_SABER_HILTS], const char* staffHilts[MAX_SABER_HILTS]);
void WP_SaberLoadParms(void);
void WP_SetSaber(int entNum, saberInfo_t* sabers, int saberNum, const char* saberName);
void* BG_Alloc(int size);
void* BG_AllocUnaligned(int size);
void* BG_TempAlloc(int size);
