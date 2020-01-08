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

#pragma once

// The entire cgame module is unloaded and reloaded on each level change,
// so there is NO persistant data between levels on the client side.
// If you absolutely need something stored, it can either be kept
// by the server in the server stored userinfos, or stashed in a cvar.

// ======================================================================
// INCLUDE
// ======================================================================

#include "qcommon/q_shared.h"
#include "game/bg_public.h"
#include "cgame/cg_public.h"
#include "ui/ui_shared.h"

#define XCVAR_PROTO
#include "cgame/cg_xcvar.h"
#undef XCVAR_PROTO

// ======================================================================
// DEFINE
// ======================================================================

#define	POWERUP_BLINKS		5

#define	POWERUP_BLINK_TIME	1000
#define	FADE_TIME			200
#define	PULSE_TIME			200
#define	DAMAGE_DEFLECT_TIME	100
#define	DAMAGE_RETURN_TIME	400
#define DAMAGE_TIME			500
#define	LAND_DEFLECT_TIME	150
#define	LAND_RETURN_TIME	300
#define	STEP_TIME			200
#define	DUCK_TIME			100
#define	PAIN_TWITCH_TIME	200
#define	WEAPON_SELECT_TIME	1400
#define	ITEM_SCALEUP_TIME	1000

// Zoom vars
#define	ZOOM_TIME			150		// not currently used?
#define MAX_ZOOM_FOV		3.0f
#define ZOOM_IN_TIME		1500.0f
#define ZOOM_OUT_TIME		100.0f
#define ZOOM_START_PERCENT	0.3f

#define	ITEM_BLOB_TIME		200
#define	MUZZLE_FLASH_TIME	20
#define	SINK_TIME			1000		// time for fragments to sink into ground before going away
#define	ATTACKER_HEAD_TIME	10000
#define	REWARD_TIME			3000

#define	PULSE_SCALE			1.5			// amount to scale up the icons when activating

#define	MAX_STEP_CHANGE		32

#define	MAX_VERTS_ON_POLY	10
#define	MAX_MARK_POLYS		256

#define STAT_MINUS			10	// num frame for '-' stats digit

#define	ICON_SIZE			48
#define	TEXT_ICON_SPACE		4

// very large characters
#define	GIANT_WIDTH			32
#define	GIANT_HEIGHT		48

#define NUM_FONT_BIG	1
#define NUM_FONT_SMALL	2
#define NUM_FONT_CHUNKY	3

#define TEAM_OVERLAY_MAXNAME_WIDTH	32
#define TEAM_OVERLAY_MAXLOCATION_WIDTH	64

#define	WAVE_AMPLITUDE	1
#define	WAVE_FREQUENCY	0.4

#define DEFAULT_HUDSET	"ui/hud.txt"

#define	MAX_CUSTOM_COMBAT_SOUNDS	40
#define	MAX_CUSTOM_EXTRA_SOUNDS	40
#define	MAX_CUSTOM_JEDI_SOUNDS	40
// MAX_CUSTOM_SIEGE_SOUNDS defined in bg_public.h
#define MAX_CUSTOM_DUEL_SOUNDS	40

#define	MAX_CUSTOM_SOUNDS	40 //rww - Note that for now these must all be the same, because of the way I am
							   //cycling through them and comparing for custom sounds.

//rww - cheap looping sound struct
#define MAX_CG_LOOPSOUNDS 8

#define MAX_SKULLTRAIL		10

#define MAX_REWARDSTACK		10
#define MAX_SOUNDBUFFER		20

#define MAX_PREDICTED_EVENTS	16

#define	MAX_CHATBOX_ITEMS		5

#define MAX_TICS	14

#define NUM_CHUNK_MODELS	4

#define MAX_STATIC_MODELS 4000

// ======================================================================
// ENUM
// ======================================================================

typedef enum {
	FOOTSTEP_STONEWALK,
	FOOTSTEP_STONERUN,
	FOOTSTEP_METALWALK,
	FOOTSTEP_METALRUN,
	FOOTSTEP_PIPEWALK,
	FOOTSTEP_PIPERUN,
	FOOTSTEP_SPLASH,
	FOOTSTEP_WADE,
	FOOTSTEP_SWIM,
	FOOTSTEP_SNOWWALK,
	FOOTSTEP_SNOWRUN,
	FOOTSTEP_SANDWALK,
	FOOTSTEP_SANDRUN,
	FOOTSTEP_GRASSWALK,
	FOOTSTEP_GRASSRUN,
	FOOTSTEP_DIRTWALK,
	FOOTSTEP_DIRTRUN,
	FOOTSTEP_MUDWALK,
	FOOTSTEP_MUDRUN,
	FOOTSTEP_GRAVELWALK,
	FOOTSTEP_GRAVELRUN,
	FOOTSTEP_RUGWALK,
	FOOTSTEP_RUGRUN,
	FOOTSTEP_WOODWALK,
	FOOTSTEP_WOODRUN,

	FOOTSTEP_TOTAL
} footstep_t;

typedef enum {
	IMPACTSOUND_DEFAULT,
	IMPACTSOUND_METAL,
	IMPACTSOUND_FLESH
} impactSound_t;

typedef enum
{
	LE_MARK,
	LE_EXPLOSION,
	LE_SPRITE_EXPLOSION,
	LE_FADE_SCALE_MODEL, // currently only for Demp2 shock sphere
	LE_FRAGMENT,
	LE_PUFF,
	LE_MOVE_SCALE_FADE,
	LE_FALL_SCALE_FADE,
	LE_FADE_RGB,
	LE_SCALE_FADE,
	LE_SCOREPLUM,
	LE_OLINE,
	LE_SHOWREFENTITY,
	LE_LINE
} leType_t;

typedef enum
{
	LEF_PUFF_DONT_SCALE = 0x0001,			// do not scale size over time
	LEF_TUMBLE = 0x0002,			// tumble over time, used for ejecting shells
	LEF_FADE_RGB = 0x0004,			// explicitly fade
	LEF_NO_RANDOM_ROTATE = 0x0008			// MakeExplosion adds random rotate which could be bad in some cases
} leFlag_t;

typedef enum
{
	LEMT_NONE,
	LEMT_BURN,
	LEMT_BLOOD
} leMarkType_t;			// fragment local entities can leave marks on walls

typedef enum
{
	LEBS_NONE,
	LEBS_BLOOD,
	LEBS_BRASS,
	LEBS_METAL,
	LEBS_ROCK
} leBounceSoundType_t;	// fragment local entities can make sounds on impacts

typedef enum chunkType_e
{
	CHUNK_METAL1 = 0,
	CHUNK_METAL2,
	CHUNK_ROCK1,
	CHUNK_ROCK2,
	CHUNK_ROCK3,
	CHUNK_CRATE1,
	CHUNK_CRATE2,
	CHUNK_WHITE_METAL,
	NUM_CHUNK_TYPES
} chunkType_t;

// ======================================================================
// STRUCT
// ======================================================================

// player entities need to track more information than any other type of entity.
// note that not every player entity is a client entity, because corpses after respawn are outside the normal client
//	numbering range
// when changing animation, set animationTime to frameTime + lerping time
// The current lerp will finish out, then it will lerp to the new animation
typedef struct lerpFrame_s {
	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on

	int			frame;
	int			frameTime;			// time when ->frame will be exactly on

	float		backlerp;

	bool	lastFlip; //if does not match torsoFlip/legsFlip, restart the anim.

	int			lastForcedFrame;

	float		yawAngle;
	bool	yawing;
	float		pitchAngle;
	bool	pitching;

	float		yawSwingDif;

	int			animationNumber;
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact

	float		animationSpeed;		// scale the animation speed
	float		animationTorsoSpeed;

	bool	torsoYawing;
} lerpFrame_t;

typedef struct playerEntity_s {
	lerpFrame_t		legs, torso, flag;
	int				painTime;
	int				painDirection;	// flip from 0 to 1
	int				lightningFiring;

	// machinegun spinning
	float			barrelAngle;
	int				barrelTime;
	bool		barrelSpinning;
} playerEntity_t;

// each client has an associated clientInfo_t that contains media references necessary to present the client model and
// other color coded effects
// this is regenerated each time a client's configstring changes, usually as a result of a userinfo (name, model, etc)
// change
typedef struct clientInfo_s {
	bool		infoValid;

	float			colorOverride[3];

	saberInfo_t		saber[MAX_SABERS];
	void			*ghoul2Weapons[MAX_SABERS];

	char			saberName[64];
	char			saber2Name[64];

	char			name[MAX_QPATH];
	char			cleanname[MAX_QPATH];
	team_t			team;

	int				duelTeam;

	int				botSkill;		// -1 = not bot, 0-5 = bot

	int				frame;

	vec3_t			color1;
	vec3_t			color2;

	saber_colors_t	icolor1;
	saber_colors_t	icolor2;

	int				score;			// updated by score servercmds
	int				location;		// location index for team mode
	int				health;			// you only get this info about your teammates
	int				armor;
	int				curWeapon;

	int				wins, losses;	// in tourney mode

	int				teamTask;		// task in teamplay (offence/defence)
	bool		teamLeader;		// true when this is a team leader

	int				powerups;		// so can display quad/flag status

	int				medkitUsageTime;

	int				breathPuffTime;

	// when clientinfo is changed, the loading of models/skins/sounds
	// can be deferred until you are dead, to prevent hitches in
	// gameplay
	char			modelName[MAX_QPATH];
	char			skinName[MAX_QPATH];
//	char			headModelName[MAX_QPATH];
//	char			headSkinName[MAX_QPATH];
	char			forcePowers[MAX_QPATH];
//	char			redTeam[MAX_TEAMNAME];
//	char			blueTeam[MAX_TEAMNAME];

	char			teamName[MAX_TEAMNAME];

	int				corrTime;

	vec3_t			lastHeadAngles;
	int				lookTime;

	int				brokenLimbs;

	bool		deferred;

	bool		newAnims;		// true if using the new mission pack animations
	bool		fixedlegs;		// true if legs yaw is always the same as torso yaw
	bool		fixedtorso;		// true if torso never changes yaw

	vec3_t			headOffset;		// move head in icon views
	//footstep_t		footsteps;
	gender_t		gender;			// from model

	qhandle_t		legsModel;
	qhandle_t		legsSkin;

	qhandle_t		torsoModel;
	qhandle_t		torsoSkin;

	//qhandle_t		headModel;
	//qhandle_t		headSkin;

	void			*ghoul2Model;

	qhandle_t		modelIcon;

	qhandle_t		bolt_rhand;
	qhandle_t		bolt_lhand;

	qhandle_t		bolt_head;

	qhandle_t		bolt_motion;

	qhandle_t		bolt_llumbar;

	sfxHandle_t		sounds[MAX_CUSTOM_SOUNDS];
	sfxHandle_t		combatSounds[MAX_CUSTOM_COMBAT_SOUNDS];
	sfxHandle_t		extraSounds[MAX_CUSTOM_EXTRA_SOUNDS];
	sfxHandle_t		jediSounds[MAX_CUSTOM_JEDI_SOUNDS];
	sfxHandle_t		siegeSounds[MAX_CUSTOM_SIEGE_SOUNDS]; // actually vchat
	sfxHandle_t		duelSounds[MAX_CUSTOM_DUEL_SOUNDS];

	int				legsAnim;
	int				torsoAnim;

	float		facial_blink;		// time before next blink. If a minus value, we are in blink mode
	float		facial_frown;		// time before next frown. If a minus value, we are in frown mode
	float		facial_aux;			// time before next aux. If a minus value, we are in aux mode

	int			superSmoothTime; //do crazy amount of smoothing

} clientInfo_t;

typedef struct cgLoopSound_s {
	int entityNum;
	vec3_t origin;
	vec3_t velocity;
	sfxHandle_t sfx;
} cgLoopSound_t;

// centity_t have a direct corespondence with gentity_t in the game, but
// only the entityState_t is directly communicated to the cgame
typedef struct centity_s {
	// CJKFIXME: bgentity_t substruct
	// This comment below is correct, but now m_pVehicle is the first thing in bg shared entity, so it goes first. - AReis
	// rww - entstate must be first, to correspond with the bg shared entity structure
	entityState_t	currentState;	// from cg.frame
	playerState_t	*playerState;	//ptr to playerstate if applicable (for bg ents)
	void			*ghoul2; //g2 instance
	int				localAnimIndex; //index locally (game/cgame) to anim data for this skel
	vec3_t			modelScale; //needed for g2 collision

	//from here up must be unified with bgEntity_t -rww

	entityState_t	nextState;		// from cg.nextFrame, if available
	bool		interpolate;	// true if next is valid to interpolate to
	bool		currentValid;	// true if cg.frame holds this entity

	int				muzzleFlashTime;	// move to playerEntity?
	int				previousEvent;
//	int				teleportFlag;

	int				trailTime;		// so missile trails can handle dropped initial packets
	int				dustTrailTime;
	int				miscTime;

	vec3_t			damageAngles;
	int				damageTime;

	int				snapShotTime;	// last time this entity was found in a snapshot

	playerEntity_t	pe;

//	int				errorTime;		// decay the error from this time
//	vec3_t			errorOrigin;
//	vec3_t			errorAngles;

//	bool		extrapolated;	// false if origin / angles is an interpolation
//	vec3_t			rawOrigin;
	vec3_t			rawAngles;

	vec3_t			beamEnd;

	// exact interpolated position of entity on this frame
	vec3_t			lerpOrigin;
	vec3_t			lerpAngles;

	vec3_t			ragLastOrigin;
	int				ragLastOriginTime;

	bool		noLumbar; //if true only do anims and things on model_root instead of lower_lumbar, this will be the case for some NPCs.
	bool		noFace;

	int				eventAnimIndex;

	int				weapon;

	void			*ghoul2weapon; //rww - pointer to ghoul2 instance of the current 3rd person weapon

	float			radius;
	int				boltInfo;

	//sometimes used as a bolt index, but these values are also used as generic values for clientside entities
	//at times
	int				bolt1;
	int				bolt2;
	int				bolt3;
	int				bolt4;

	float			bodyHeight;

	int				torsoBolt;

	vec3_t			turAngles;

	vec3_t			frame_minus1;
	vec3_t			frame_minus2;

	int				frame_minus1_refreshed;
	int				frame_minus2_refreshed;

	void			*frame_hold; //pointer to a ghoul2 instance

	int				frame_hold_time;
	int				frame_hold_refreshed;

	void			*grip_arm; //pointer to a ghoul2 instance

	int				trickAlpha;
	int				trickAlphaTime;

	int				teamPowerEffectTime;
	int				teamPowerType; //0 regen, 1 heal, 2 drain, 3 absorb

	bool		isRagging;
	bool		ownerRagging;
	int				overridingBones;

	int				bodyFadeTime;
	vec3_t			pushEffectOrigin;

	cgLoopSound_t	loopingSound[MAX_CG_LOOPSOUNDS];
	int				numLoopingSounds;

	int				serverSaberHitIndex;
	int				serverSaberHitTime;
	bool		serverSaberFleshImpact; //true if flesh, false if anything else.

	bool		ikStatus;

	bool		saberWasInFlight;

	float			smoothYaw;

	int				uncloaking;
	bool		cloaked;

	int				vChatTime;
} centity_t;

// local entities are created as a result of events or predicted actions, and live independently from all server
// transmitted entities
typedef struct markPoly_s {
	struct markPoly_s	*prevMark, *nextMark;
	int			time;
	qhandle_t	markShader;
	bool	alphaFade;		// fade alpha instead of rgb
	float		color[4];
	poly_t		poly;
	polyVert_t	verts[MAX_VERTS_ON_POLY];
} markPoly_t;

typedef struct localEntity_s {
	struct localEntity_s	*prev, *next;
	leType_t		leType;
	int				leFlags;

	int				startTime;
	int				endTime;
	int				fadeInTime;

	float			lifeRate;			// 1.0 / (endTime - startTime)

	trajectory_t	pos;
	trajectory_t	angles;

	float			bounceFactor;		// 0.0 = no bounce, 1.0 = perfect
	int				bounceSound;		// optional sound index to play upon bounce

	float			alpha;
	float			dalpha;

	int				forceAlpha;

	float			color[4];

	float			radius;

	float			light;
	vec3_t			lightColor;

	leMarkType_t		leMarkType;		// mark to leave on fragment impact
	leBounceSoundType_t	leBounceSoundType;

	union {
		struct {
			float radius;
			float dradius;
			vec3_t startRGB;
			vec3_t dRGB;
		} sprite;
		struct {
			float width;
			float dwidth;
			float length;
			float dlength;
			vec3_t startRGB;
			vec3_t dRGB;
		} trail;
		struct {
			float width;
			float dwidth;
			// Below are bezier specific.
			vec3_t			control1;				// initial position of control points
			vec3_t			control2;
			vec3_t			control1_velocity;		// initial velocity of control points
			vec3_t			control2_velocity;
			vec3_t			control1_acceleration;	// constant acceleration of control points
			vec3_t			control2_acceleration;
		} line;
		struct {
			float width;
			float dwidth;
			float width2;
			float dwidth2;
			vec3_t startRGB;
			vec3_t dRGB;
		} line2;
		struct {
			float width;
			float dwidth;
			float width2;
			float dwidth2;
			float height;
			float dheight;
		} cylinder;
		struct {
			float width;
			float dwidth;
		} electricity;
		struct
		{
			// fight the power! open and close brackets in the same column!
			float radius;
			float dradius;
			bool (*thinkFn)(struct localEntity_s *le);
			vec3_t	dir;	// magnitude is 1, but this is oldpos - newpos right before the
							//particle is sent to the renderer
			// may want to add something like particle::localEntity_s *le (for the particle's think fn)
		} particle;
		struct
		{
			bool	dontDie;
			vec3_t		dir;
			float		variance;
			int			delay;
			int			nextthink;
			bool	(*thinkFn)(struct localEntity_s *le);
			int			data1;
			int			data2;
		} spawner;
		struct
		{
			float radius;
		} fragment;
	} data;

	refEntity_t		refEntity;
} localEntity_t;

typedef struct score_s {
	int				client;
	int				score;
	int				ping;
	int				time;
	int				scoreFlags;
	int				powerUps;
	int				accuracy;
	int				impressiveCount;
	int				excellentCount;
	int				gauntletCount;
	int				defendCount;
	int				assistCount;
	int				captures;
	bool	perfect;
	int				team;
} score_t;

// each WP_* weapon enum has an associated weaponInfo_t
// that contains media references necessary to present the
// weapon and its effects
typedef struct weaponInfo_s {
	//CJKTODO: all assets should be pointers to handles so we can load them in after initialising this struct
	bool		registered;
	gitem_t			*item;

	qhandle_t		handsModel;			// the hands don't actually draw, they just position the weapon
	qhandle_t		weaponModel;		// this is the pickup model
	qhandle_t		viewModel;			// this is the in-view model used by the player
	qhandle_t		barrelModel;
	qhandle_t		flashModel;

	vec3_t			weaponMidpoint;		// so it will rotate centered instead of by tag

	float			flashDlight;
	vec3_t			flashDlightColor;

	qhandle_t		weaponIcon;
	qhandle_t		ammoIcon;

	qhandle_t		ammoModel;

	sfxHandle_t		flashSound[4];		// fast firing weapons randomly choose
	sfxHandle_t		firingSound;
	sfxHandle_t		chargeSound;
	fxHandle_t		muzzleEffect;
	qhandle_t		missileModel;
	sfxHandle_t		missileSound;
	void			(*missileTrailFunc)( centity_t *, const struct weaponInfo_s *wi );
	float			missileDlight;
	vec3_t			missileDlightColor;
	int				missileRenderfx;
	sfxHandle_t		missileHitSound;

	sfxHandle_t		altFlashSound[4];
	sfxHandle_t		altFiringSound;
	sfxHandle_t		altChargeSound;
	fxHandle_t		altMuzzleEffect;
	qhandle_t		altMissileModel;
	sfxHandle_t		altMissileSound;
	void			(*altMissileTrailFunc)( centity_t *, const struct weaponInfo_s *wi );
	float			altMissileDlight;
	vec3_t			altMissileDlightColor;
	int				altMissileRenderfx;
	sfxHandle_t		altMissileHitSound;

	sfxHandle_t		selectSound;

	sfxHandle_t		readySound;
	float			trailRadius;
	float			wiTrailTime;

} weaponInfo_t;

// each IT_* item has an associated itemInfo_t
// that constains media references necessary to present the
// item and its effects
typedef struct itemInfo_s {
	bool		registered;
	qhandle_t		models[MAX_ITEM_MODELS];
	qhandle_t		icon;
	void			*g2Models[MAX_ITEM_MODELS];
	float			radius[MAX_ITEM_MODELS];
} itemInfo_t;

typedef struct powerupInfo_s {
	int				itemNum;
} powerupInfo_t;

typedef struct skulltrail_s {
	vec3_t positions[MAX_SKULLTRAIL];
	int numpositions;
} skulltrail_t;

// all cg.stepTime, cg.duckTime, cg.landTime, etc are set to cg.time when the action occurs, and they will have visible
//	effects for e.g. STEP_TIME msec after

typedef struct chatBoxItem_s
{
	char	string[MAX_SAY_TEXT];
	int		time;
	int		lines;
} chatBoxItem_t;

typedef struct cg_s {
	int			clientFrame;		// incremented each frame

	int			clientNum;

	bool	demoPlayback;
	bool	levelShot;			// taking a level menu screenshot
	int			deferredPlayerLoading;
	bool	loading;			// don't defer players at initial startup
	bool	intermissionStarted;	// don't play voice rewards, because game will end shortly

	// there are only one or two snapshot_t that are relevent at a time
	int			latestSnapshotNum;	// the number of snapshots the client system has received
	int			latestSnapshotTime;	// the time from latestSnapshotNum, so we don't need to read the snapshot yet

	snapshot_t	*snap;				// cg.snap->serverTime <= cg.time
	snapshot_t	*nextSnap;			// cg.nextSnap->serverTime > cg.time, or nullptr
//	snapshot_t	activeSnapshots[2];

	float		frameInterpolation;	// (float)( cg.time - cg.frame->serverTime ) / (cg.nextFrame->serverTime - cg.frame->serverTime)

	bool	mMapChange;

	bool	thisFrameTeleport;
	bool	nextFrameTeleport;

	int			frametime;		// cg.time - cg.oldTime

	int			time;			// this is the time value that the client
								// is rendering at.
	int			oldTime;		// time at last frame, used for missile trails and prediction checking

	int			physicsTime;	// either cg.snap->time or cg.nextSnap->time

	int			timelimitWarnings;	// 5 min, 1 min, overtime
	int			fraglimitWarnings;

	bool	mapRestart;			// set on a map restart to set back the weapon

	bool	renderingThirdPerson;		// during deaths, chasecams, etc

	// prediction state
	bool	hyperspace;				// true if prediction has hit a trigger_teleport
	playerState_t	predictedPlayerState;

	//centity_t		predictedPlayerEntity;
	//rww - I removed this and made it use cg_entities[clnum] directly.

	bool	validPPS;				// clear until the first call to CG_PredictPlayerState
	int			predictedErrorTime;
	vec3_t		predictedError;

	int			eventSequence;
	int			predictableEvents[MAX_PREDICTED_EVENTS];

	float		stepChange;				// for stair up smoothing
	int			stepTime;

	float		duckChange;				// for duck viewheight smoothing
	int			duckTime;

	float		landChange;				// for landing hard
	int			landTime;

	// input state sent to server
	int			weaponSelect;

	int			forceSelect;
	int			itemSelect;

	// auto rotating items
	vec3_t		autoAngles;
	matrix3_t	autoAxis;
	vec3_t		autoAnglesFast;
	matrix3_t	autoAxisFast;

	// view rendering
	refdef_t	refdef;

	// zoom key
	bool	zoomed;
	int			zoomTime;
	float		zoomSensitivity;

	// information screen text during loading
	char		infoScreenText[MAX_STRING_CHARS];

	// scoreboard
	int			scoresRequestTime;
	int			numScores;
	int			selectedScore;
	int			teamScores[2];
	score_t		scores[MAX_CLIENTS];
	bool	showScores;
	bool	scoreBoardShowing;
	int			scoreFadeTime;
	char		killerName[MAX_NETNAME];
	char			spectatorList[MAX_STRING_CHARS];		// list of names
	int				spectatorLen;												// length of list
	float			spectatorWidth;											// width in device units
	int				spectatorTime;											// next time to offset
	int				spectatorPaintX;										// current paint x
	int				spectatorPaintX2;										// current paint x
	int				spectatorOffset;										// current offset from start
	int				spectatorPaintLen; 									// current offset from start

	// skull trails
	skulltrail_t	skulltrails[MAX_CLIENTS];

	// centerprinting
	int			centerPrintTime;
	int			centerPrintCharWidth;
	int			centerPrintY;
	char		centerPrint[1024];
	int			centerPrintLines;

	int			oldammo;
	int			oldAmmoTime;

	// low ammo warning state
	int			lowAmmoWarning;		// 1 = low, 2 = empty

	// kill timers for carnage reward
	int			lastKillTime;

	// crosshair client ID
	int			crosshairClientNum;
	int			crosshairClientTime;

	// powerup active flashing
	int			powerupActive;
	int			powerupTime;

	// attacking player
	int			attackerTime;
	int			voiceTime;

	// reward medals
	int			rewardStack;
	int			rewardTime;
	int			rewardCount[MAX_REWARDSTACK];
	qhandle_t	rewardShader[MAX_REWARDSTACK];
	qhandle_t	rewardSound[MAX_REWARDSTACK];

	// sound buffer mainly for announcer sounds
	int			soundBufferIn;
	int			soundBufferOut;
	int			soundTime;
	qhandle_t	soundBuffer[MAX_SOUNDBUFFER];

	// for voice chat buffer
	int			voiceChatTime;
	int			voiceChatBufferIn;
	int			voiceChatBufferOut;

	// warmup countdown
	int			warmup;
	int			warmupCount;

	int			itemPickup;
	int			itemPickupTime;
	int			itemPickupBlendTime;	// the pulse around the crosshair is timed seperately

	int			weaponSelectTime;
	int			weaponAnimation;
	int			weaponAnimationTime;

	// blend blobs
	float		damageTime;
	float		damageX, damageY, damageValue;

	// status bar head
	float		headYaw;
	float		headEndPitch;
	float		headEndYaw;
	int			headEndTime;
	float		headStartPitch;
	float		headStartYaw;
	int			headStartTime;

	// view movement
	float		v_dmg_time;
	float		v_dmg_pitch;
	float		v_dmg_roll;

	vec3_t		kick_angles;	// weapon kicks
	int			kick_time;
	vec3_t		kick_origin;

	// temp working variables for player view
	float		bobfracsin;
	int			bobcycle;
	float		xyspeed;
	int     nextOrbitTime;

	//bool cameraMode;		// if rendering from a loaded camera

	int			forceHUDTotalFlashTime;
	int			forceHUDNextFlashTime;
	bool	forceHUDActive;				// Flag to show force hud is off/on

	// development tool
	refEntity_t		testModelEntity;
	char			testModelName[MAX_QPATH];
	bool		testGun;

	// HUD stuff
	float			HUDTickFlashTime;
	bool		HUDArmorFlag;
	bool		HUDHealthFlag;
	bool		iconHUDActive;
	float			iconHUDPercent;
	float			iconSelectTime;
	float			invenSelectTime;
	float			forceSelectTime;

	vec3_t			lastFPFlashPoint;

	int				testModel;
	// had to be moved so we wouldn't wipe these out with the memset - these have STL in them and shouldn't be cleared that way
	snapshot_t	activeSnapshots[2];

	// used for communication with the engine
	union {
		char						raw[MAX_CG_SHARED_BUFFER_SIZE];
		TCGPointContents			pointContents;
		TCGVectorData				vectorData;
		TCGGetBoltData				getBoltData;
		TCGTrace					trace;
		TCGG2Mark					g2Mark;
		TCGImpactMark				impactMark;
		ragCallbackDebugBox_t		rcbDebugBox;
		ragCallbackDebugLine_t		rcbDebugLine;
		ragCallbackBoneSnap_t		rcbBoneSnap;
		ragCallbackBoneInSolid_t	rcbBoneInSolid;
		ragCallbackTraceLine_t		rcbTraceLine;
		TCGMiscEnt					miscEnt;
		TCGIncomingConsoleCommand	icc;
		autoMapInput_t				autoMapInput;
		TCGCameraShake				cameraShake;
	} sharedBuffer;

	short				radarEntityCount;
	short				radarEntities[MAX_CLIENTS+16];

	short				bracketedEntityCount;
	short				bracketedEntities[MAX_CLIENTS+16];

	float				distanceCull;

	chatBoxItem_t		chatItems[MAX_CHATBOX_ITEMS];
	int					chatItemActive;

	bool spawning;
	int	numSpawnVars;
	char *spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int numSpawnVarChars;
	char spawnVarChars[MAX_SPAWN_VARS_CHARS];

	struct {
		vec3_t		amount;
		float		speed;
	} gunIdleDrift;
	vec3_t		gunAlign;
	vec3_t		gunBob;

	float	loadFrac;
	float	fps;
} cg_t;

typedef struct forceTicPos_s
{
	int				x;
	int				y;
	int				width;
	int				height;
	char			*file;
	qhandle_t		tic;
} forceTicPos_t;

typedef struct cgscreffects_s
{
	float		FOV;
	float		FOV2;

	float		shake_intensity;
	int			shake_duration;
	int			shake_start;

	float		music_volume_multiplier;
	int			music_volume_time;
	bool	music_volume_set;
} cgscreffects_t;

typedef struct cg_staticmodel_s {
	qhandle_t		model;
	vec3_t			org;
	matrix3_t		axes;
	float			radius;
	float			zoffset;
} cg_staticmodel_t;

// The client game static (cgs) structure hold everything
// loaded or calculated from the gamestate.  It will NOT
// be cleared when a tournament restart is done, allowing
// all clients to begin playing instantly
typedef struct cgs_s {
	gameState_t		gameState;			// gamestate from server
	glconfig_t		glconfig;			// rendering configuration
	float			screenXScale;		// derived from glconfig
	float			screenYScale;
	float			screenXBias;

	int				serverCommandSequence;	// reliable command stream counter
	int				processedSnapshotNum;// the number of snapshots cgame has requested

	bool		localServer;		// detected on startup by checking sv_running

	// parsed from serverinfo
	int				showDuelHealths;
	gametype_t		gametype;
	int				debugMelee;
	int				stepSlideFix;
	int				noSpecMove;
	int				dmflags;
	int				fraglimit;
	int				duel_fraglimit;
	int				capturelimit;
	int				timelimit;
	int				maxclients;
	bool		needpass;
	bool		jediVmerc;
	int				wDisable;
	int				fDisable;

	char			mapname[MAX_QPATH];
	char			rawmapname[MAX_QPATH];
//	char			redTeam[MAX_QPATH];
//	char			blueTeam[MAX_QPATH];

	int				voteTime;
	int				voteYes;
	int				voteNo;
	bool		voteModified;			// beep whenever changed
	char			voteString[MAX_STRING_TOKENS];

	int				teamVoteTime[2];
	int				teamVoteYes[2];
	int				teamVoteNo[2];
	bool		teamVoteModified[2];	// beep whenever changed
	char			teamVoteString[2][MAX_STRING_TOKENS];

	int				levelStartTime;

	int				scores1, scores2;		// from configstrings
	int				jediMaster;
	int				duelWinner;
	int				duelist1;
	int				duelist2;
	int				duelist3;
// nmckenzie: DUEL_HEALTH.  hmm.
	int				duelist1health;
	int				duelist2health;
	int				duelist3health;

	int				redflag, blueflag;		// flag status from configstrings
	int				flagStatus;

	bool  newHud;

	// locally derived information from gamestate

	qhandle_t		gameModels[MAX_MODELS];
	sfxHandle_t		gameSounds[MAX_SOUNDS];
	fxHandle_t		gameEffects[MAX_FX];
	qhandle_t		gameIcons[MAX_ICONS];

	int				numInlineModels;
	qhandle_t		inlineDrawModel[MAX_MODELS];
	vec3_t			inlineModelMidpoints[MAX_MODELS];

	clientInfo_t	clientinfo[MAX_CLIENTS];

	int cursorX;
	int cursorY;
	bool eventHandling;
	bool mouseCaptured;
	bool sizingHud;
	void *capturedItem;
	qhandle_t activeCursor;

	int					numMiscStaticModels;
	cg_staticmodel_t	miscStaticModels[MAX_STATIC_MODELS];

	int32_t			saberSpecialMoves;
	int32_t			saberTweaks;
} cgs_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern cgs_t cgs;
extern cg_t cg;
extern centity_t cg_entities[MAX_GENTITIES];

extern centity_t* cg_permanents[MAX_GENTITIES];
extern int cg_numpermanents;

extern weaponInfo_t	cg_weapons[MAX_WEAPONS];
extern itemInfo_t cg_items[MAX_ITEMS];
extern markPoly_t cg_markPolys[MAX_MARK_POLYS];

extern int sortedTeamPlayers[TEAM_MAXOVERLAY];
extern int numSortedTeamPlayers;
extern char systemChat[256];

extern cgameImport_t *trap;
extern char *cg_customSoundNames[MAX_CUSTOM_SOUNDS];
extern const char *cg_customCombatSoundNames[MAX_CUSTOM_COMBAT_SOUNDS];
extern const char *cg_customExtraSoundNames[MAX_CUSTOM_EXTRA_SOUNDS];
extern const char *cg_customJediSoundNames[MAX_CUSTOM_JEDI_SOUNDS];
extern const char *cg_customDuelSoundNames[MAX_CUSTOM_DUEL_SOUNDS];

extern forceTicPos_t forceTicPos[];
extern forceTicPos_t ammoTicPos[];

extern cgscreffects_t cgScreenEffects;

// ======================================================================
// FUNCTION
// ======================================================================

bool CG_CalcMuzzlePoint(int entityNum, vec3_t muzzle);
bool CG_ConsoleCommand(void);
bool CG_CullPointAndRadius(const vec3_t pt, float radius);
bool CG_DeferMenuScript(char** args);
bool CG_DrawOldScoreboard(void);
bool CG_G2TraceCollide(trace_t* tr, const vec3_t mins, const vec3_t maxs, const vec3_t lastValidStart, const vec3_t lastValidEnd);
bool CG_OtherTeamHasFlag(void);
bool CG_OwnerDrawVisible(int flags);
bool CG_RagDoll(centity_t* cent, vec3_t forcedAngles);
bool CG_SpawnBoolean(const char* key, const char* defaultString, bool* out);
bool CG_SpawnFloat(const char* key, const char* defaultString, float* out);
bool CG_SpawnInt(const char* key, const char* defaultString, int* out);
bool CG_SpawnString(const char* key, const char* defaultString, char** out);
bool CG_SpawnVector(const char* key, const char* defaultString, float* out);
bool CG_ThereIsAMaster(void);
bool CG_UsingEWeb(void);
bool CG_YourTeamHasFlag(void);
const char* CG_Argv(int arg);
const char* CG_ConfigString(int index);
const char* CG_GetGameStatusText(void);
const char* CG_GetKillerText(void);
const char* CG_GetLocationString(const char* loc);
const char* CG_GetStringEdString(char* refSection, char* refName);
const char* CG_PlaceString(int rank);
float CG_GetValue(int ownerDraw);
float CG_RadiusForCent(centity_t* cent);
float* CG_FadeColor(int startMsec, int totalMsec);
float* CG_TeamColor(int team);
int BG_ProperForceIndex(int power);
int CG_CrosshairPlayer(void);
int CG_DrawStrlen(const char* str);
int CG_HandleAppendedSkin(char* modelName);
int CG_IsMindTricked(int trickIndex1, int trickIndex2, int trickIndex3, int trickIndex4, int client);
int CG_LastAttacker(void);
int	CG_PointContents(const vec3_t point, int passEntityNum);
localEntity_t* CG_AllocLocalEntity(void);
localEntity_t* CG_MakeExplosion(vec3_t origin, vec3_t dir, qhandle_t hModel, int numframes, qhandle_t shader, int msec, bool isSprite, float scale, int flags);// Overloaded in single player
localEntity_t* CG_SmokePuff(const vec3_t p, const vec3_t vel, float radius, float r, float g, float b, float a, float duration, int startTime, int fadeInTime, int leFlags, qhandle_t hShader);
markPoly_t* CG_AllocMark(void);
qhandle_t CG_StatusHandle(int task);
sfxHandle_t	CG_CustomSound(int clientNum, const char* soundName);
void BG_CycleForce(playerState_t* ps, int direction);
void CG_ActualLoadDeferredPlayers(void);
void CG_AddBracketedEnt(centity_t* cent);
void CG_AddBufferedSound(sfxHandle_t sfx);
void CG_AddGhoul2Mark(int shader, float size, vec3_t start, vec3_t end, int entnum, vec3_t entposition, float entangle, void* ghoul2, vec3_t scale, int lifeTime);
void CG_AddLagometerFrameInfo(void);
void CG_AddLagometerSnapshotInfo(snapshot_t* snap);
void CG_AddLocalEntities(void);
void CG_AddMarks(void);
void CG_AddPacketEntities(bool isPortal);
void CG_AddPlayerWeapon(refEntity_t* parent, playerState_t* ps, centity_t* cent, int team, vec3_t newAngles, bool thirdPerson);
void CG_AddRadarEnt(centity_t* cent);
void CG_AddRefEntityWithPowerups(refEntity_t* ent, entityState_t* state, int team);
void CG_AddViewWeapon(playerState_t* ps);
void CG_AdjustPositionForMover(const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out);
void CG_Beam(centity_t* cent);
void CG_BubbleTrail(vec3_t start, vec3_t end, float spacing);
void CG_BuildSolidList(void);
void CG_BuildSpectatorString(void);
void CG_CacheG2AnimInfo(char* modelName);
void CG_CalcEntityLerpPositions(centity_t* cent);
void CG_CenterPrint(const char* str, int y, int charWidth);
void CG_ChatBox_AddString(char* chatStr);
void CG_CheckChangedPredictableEvents(playerState_t* ps);
void CG_CheckEvents(centity_t* cent);
void CG_CheckPlayerG2Weapons(playerState_t* ps, centity_t* cent);
void CG_Chunks(int owner, vec3_t origin, const vec3_t normal, const vec3_t mins, const vec3_t maxs, float speed, int numChunks, material_t chunkType, int customChunk, float baseScale);
void CG_CleanJetpackGhoul2(void);
void CG_ClearLightStyles(void);
void CG_ColorForHealth(vec4_t hcolor);
void CG_CopyG2WeaponInstance(centity_t* cent, int weaponNum, void* toGhoul2);
void CG_CreateBBRefEnts(entityState_t* s1, vec3_t origin);
void CG_Disintegration(centity_t* cent, refEntity_t* ent);
void CG_DoCameraShake(vec3_t origin, float intensity, int radius, int time);
void CG_Draw3DModel(float x, float y, float w, float h, qhandle_t model, void* ghoul2, int g2radius, qhandle_t skin, vec3_t origin, vec3_t angles);
void CG_DrawActive(stereoFrame_t stereoView);
void CG_DrawActiveFrame(int serverTime, stereoFrame_t stereoView, bool demoPlayback);
void CG_DrawBigString(int x, int y, const char* s, float alpha);
void CG_DrawBigStringColor(int x, int y, const char* s, vec4_t color);
void CG_DrawFlagModel(float x, float y, float w, float h, int team, bool force2D);
void CG_DrawHead(float x, float y, float w, float h, int clientNum, vec3_t headAngles);
void CG_DrawIconBackground(void);
void CG_DrawInformation(void);
void CG_DrawNumField(int x, int y, int width, int value, int charWidth, int charHeight, int style, bool zeroFill);
void CG_DrawOldTourneyScoreboard(void);
void CG_DrawPic(float x, float y, float width, float height, qhandle_t hShader);
void CG_DrawRect(float x, float y, float width, float height, float size, const float* color);
void CG_DrawRotatePic(float x, float y, float width, float height, float angle, qhandle_t hShader);
void CG_DrawRotatePic2(float x, float y, float width, float height, float angle, qhandle_t hShader);
void CG_DrawSides(float x, float y, float w, float h, float size);
void CG_DrawSmallString(int x, int y, const char* s, float alpha);
void CG_DrawSmallStringColor(int x, int y, const char* s, vec4_t color);
void CG_DrawString(float x, float y, const char* string, float charWidth, float charHeight, const float* modulate);
void CG_DrawStringExt(int x, int y, const char* string, const float* setColor, bool forceColor, bool shadow, int charWidth, int charHeight, int maxChars);
void CG_DrawTeamBackground(int x, int y, int w, int h, float alpha, int team);
void CG_DrawTopBottom(float x, float y, float w, float h, float size);
void CG_DrawWeaponSelect(void);
void CG_EntityEvent(centity_t* cent, vec3_t position);
void CG_EventHandling(int type);
void CG_ExecuteNewServerCommands(int latestSequence);
void CG_FillRect(float x, float y, float width, float height, const float* color);
void CG_FireWeapon(centity_t* cent, bool alt_fire);
void CG_G2ServerBoneAngles(centity_t* cent);
void CG_G2Trace(trace_t* result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipNumber, int mask);
void CG_GetClientWeaponMuzzleBoltPoint(int clIndex, vec3_t to);
void CG_GetColorForHealth(int health, int armor, vec4_t hcolor);
void CG_GetTeamColor(vec4_t* color);
void CG_GlassShatter(int entnum, vec3_t dmgPt, vec3_t dmgDir, float dmgRadius, int maxShards);
void CG_ImpactMark(qhandle_t markShader, const vec3_t origin, const vec3_t dir, float orientation, float r, float g, float b, float a, bool alphaFade, float radius, bool temporary);
void CG_Init_CG(void);
void CG_Init_CGents(void);
void CG_InitConsoleCommands(void);
void CG_InitG2Weapons(void);
void CG_InitGlass(void);
void CG_InitJetpackGhoul2(void);
void CG_InitLocalEntities(void);
void CG_InitMarkPolys(void);
void CG_KeyEvent(int key, bool down);
void CG_KillCEntityG2(int entNum);
void CG_ListModelBones_f(void);
void CG_ListModelSurfaces_f(void);
void CG_LoadDeferredPlayers(void);
void CG_LoadingClient(int clientNum);
void CG_LoadingItem(int itemNum);
void CG_LoadingString(const char* s);
void CG_LoadMenus(const char* menuFile);
void CG_ManualEntityRender(centity_t* cent);
void CG_MiscModelExplosion(vec3_t mins, vec3_t maxs, int size, material_t chunkType);
void CG_MissileHitPlayer(int weapon, vec3_t origin, vec3_t dir, int entityNum, bool alt_fire);
void CG_MissileHitWall(int weapon, int clientNum, vec3_t origin, vec3_t dir, impactSound_t soundType, bool alt_fire, int charge);
void CG_MouseEvent(int x, int y);
void CG_NewClientInfo(int clientNum, bool entitiesInitialized);
void CG_NextForcePower_f(void);
void CG_NextInventory_f(void);
void CG_NextWeapon_f(void);
void CG_OutOfAmmoChange(int oldWeapon);	// should this be in pmove?
void CG_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle, int font);
void CG_PainEvent(centity_t* cent, int health);
void CG_ParseEntitiesFromString(void);
void CG_ParseServerinfo(void);
void CG_PlayDoorLoopSound(centity_t* cent);
void CG_PlayDoorSound(centity_t* cent, int type);
void CG_Player(centity_t* cent);
void CG_PlayerShieldHit(int entitynum, vec3_t angles, int amount);
void CG_PmoveClientPointerUpdate();
void CG_PositionEntityOnTag(refEntity_t* entity, const refEntity_t* parent, qhandle_t parentModel, char* tagName);
void CG_PositionRotatedEntityOnTag(refEntity_t* entity, const refEntity_t* parent, qhandle_t parentModel, char* tagName);
void CG_PredictPlayerState(void);
void CG_PrevForcePower_f(void);
void CG_PrevInventory_f(void);
void CG_PrevWeapon_f(void);
void CG_ProcessSnapshots(void);
void CG_RankRunFrame(void);
void CG_ReattachLimb(centity_t* source);
void CG_RegisterCvars(void);
void CG_RegisterItemVisuals(int itemNum);
void CG_RegisterWeapon(int weaponNum);
void CG_ResetPlayerEntity(centity_t* cent);
void CG_Respawn(void);
void CG_ROFF_NotetrackCallback(centity_t* cent, const char* notetrack);
void CG_RunLightStyles(void);
void CG_RunMenuScript(char** args);
void CG_S_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);
void CG_S_AddRealLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);
void CG_S_StopLoopingSound(int entityNum, sfxHandle_t sfx);
void CG_S_UpdateLoopingSounds(int entityNum);
void CG_ScorePlum(int client, vec3_t org, int score);
void CG_SetConfigValues(void);
void CG_SetEntitySoundPosition(centity_t* cent);
void CG_SetGhoul2Info(refEntity_t* ent, centity_t* cent);
void CG_SetLightstyle(int i);
void CG_SetScoreSelection(void* menu);
void CG_ShaderStateChanged(void);
void CG_ShowResponseHead(void);
void CG_ShutDownG2Weapons(void);
void CG_Spark(vec3_t origin, vec3_t dir);
void CG_Spark(vec3_t origin, vec3_t dir);
void CG_StartMusic(bool bForceStart);
void CG_SurfaceExplosion(vec3_t origin, vec3_t normal, float radius, float shake_speed, bool smoke);
void CG_TestG2Model_f(void);
void CG_TestGun_f(void);
void CG_TestLine(vec3_t start, vec3_t end, int time, unsigned int color, int radius);
void CG_TestModel_f(void);
void CG_TestModelAnimate_f(void);
void CG_TestModelNextFrame_f(void);
void CG_TestModelNextSkin_f(void);
void CG_TestModelPrevFrame_f(void);
void CG_TestModelPrevSkin_f(void);
void CG_TestModelSetAnglespost_f(void);
void CG_TestModelSetAnglespre_f(void);
void CG_TestModelSurfaceOnOff_f(void);
void CG_TileClear(void);
void CG_Trace(trace_t* result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipNumber, int mask);
void CG_TransitionPlayerState(playerState_t* ps, playerState_t* ops);
void CG_UpdateCvars(void);
void CG_UpdateCvars(void);
void CG_Weapon_f(void);
void CG_WeaponClean_f(void);
void CG_ZoomDown_f(void);
void CG_ZoomUp_f(void);
void CGCam_SetMusicMult(float multiplier, int duration);
void CGCam_Shake(float intensity, int duration);
void FX_BlasterAltFireThink(centity_t* cent, const struct weaponInfo_s* weapon);
void FX_BlasterAltFireThink(centity_t* cent, const struct weaponInfo_s* weapon);
void FX_BlasterProjectileThink(centity_t* cent, const struct weaponInfo_s* weapon);
void FX_BlasterProjectileThink(centity_t* cent, const struct weaponInfo_s* weapon);
void FX_BlasterWeaponHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_BlasterWeaponHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_BlasterWeaponHitWall(vec3_t origin, vec3_t normal);
void FX_BlasterWeaponHitWall(vec3_t origin, vec3_t normal);
void FX_BryarAltHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_BryarAltHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_BryarAltHitWall(vec3_t origin, vec3_t normal, int power);
void FX_BryarAltHitWall(vec3_t origin, vec3_t normal, int power);
void FX_BryarHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_BryarHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_BryarHitWall(vec3_t origin, vec3_t normal);
void FX_BryarHitWall(vec3_t origin, vec3_t normal);
void FX_ConcAltShot(vec3_t start, vec3_t end);
void FX_ConcussionHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_ConcussionHitWall(vec3_t origin, vec3_t normal);
void FX_ConcussionProjectileThink(centity_t* cent, const struct weaponInfo_s* weapon);
void FX_ForceDrained(vec3_t origin, vec3_t dir);
void FX_TurretHitPlayer(vec3_t origin, vec3_t normal, bool humanoid);
void FX_TurretHitWall(vec3_t origin, vec3_t normal);
void FX_TurretProjectileThink(centity_t* cent, const struct weaponInfo_s* weapon);
void ScaleModelAxis(refEntity_t* ent);
void TurretClientRun(centity_t* ent);
void* CG_G2WeaponInstance(centity_t* cent, int weapon);