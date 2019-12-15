/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
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

//NOTENOTE: The enums and tables in this file will obviously bitch if they are included multiple times, don't do that

typedef enum //# setType_e
{
	//# #sep Parm strings
	SET_PARM1 = 0,//## %s="" # Set entity parm1
	SET_PARM2,//## %s="" # Set entity parm2
	SET_PARM3,//## %s="" # Set entity parm3
	SET_PARM4,//## %s="" # Set entity parm4
	SET_PARM5,//## %s="" # Set entity parm5
	SET_PARM6,//## %s="" # Set entity parm6
	SET_PARM7,//## %s="" # Set entity parm7
	SET_PARM8,//## %s="" # Set entity parm8
	SET_PARM9,//## %s="" # Set entity parm9
	SET_PARM10,//## %s="" # Set entity parm10
	SET_PARM11,//## %s="" # Set entity parm11
	SET_PARM12,//## %s="" # Set entity parm12
	SET_PARM13,//## %s="" # Set entity parm13
	SET_PARM14,//## %s="" # Set entity parm14
	SET_PARM15,//## %s="" # Set entity parm15
	SET_PARM16,//## %s="" # Set entity parm16

	// NOTE!!! If you add any other SET_xxxxxxSCRIPT types, make sure you update the 'case' statements in
	//	ICARUS_InterrogateScript() (game/g_ICARUS.cpp), or the script-precacher won't find them.

	//# #sep Scripts and other file paths
	SET_SPAWNSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when spawned //0 - do not change these, these are equal to BSET_SPAWN, etc
	SET_USESCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when used
	SET_AWAKESCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when startled
	SET_ANGERSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script run when find an enemy for the first time
	SET_ATTACKSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when you shoot
	SET_VICTORYSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when killed someone
	SET_LOSTENEMYSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when you can't find your enemy
	SET_PAINSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when hit
	SET_FLEESCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when hit and low health
	SET_DEATHSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when killed
	SET_DELAYEDSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run after a delay
	SET_BLOCKEDSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when blocked by teammate
	SET_FFIRESCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when player has shot own team repeatedly
	SET_FFDEATHSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when player kills a teammate
	SET_MINDTRICKSCRIPT,//## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when player kills a teammate
	SET_VIDEO_PLAY,//## %s="filename" !!"W:\game\base\video\!!#*.roq" # Play a video (inGame)
	SET_CINEMATIC_SKIPSCRIPT, //## %s="filename" !!"W:\game\base\scripts\!!#*.txt" # Script to run when skipping the running cinematic

	//# #sep Standard strings
	SET_TARGETNAME,//## %s="NULL" # Set/change your targetname
	SET_PAINTARGET,//## %s="NULL" # Set/change what to use when hit
	SET_CAMERA_GROUP,//## %s="NULL" # all ents with this cameraGroup will be focused on
	SET_CAMERA_GROUP_TAG,//## %s="NULL" # What tag on all clients to try to track
	SET_ADDRHANDBOLT_MODEL,			//## %s="NULL" # object to place on NPC right hand bolt
	SET_REMOVERHANDBOLT_MODEL,		//## %s="NULL" # object to remove from NPC right hand bolt
	SET_ADDLHANDBOLT_MODEL,			//## %s="NULL" # object to place on NPC left hand bolt
	SET_REMOVELHANDBOLT_MODEL,		//## %s="NULL" # object to remove from NPC left hand bolt
	SET_CAPTIONTEXTCOLOR,	//## %s=""  # Color of text RED,WHITE,BLUE, YELLOW
	SET_CENTERTEXTCOLOR,	//## %s=""  # Color of text RED,WHITE,BLUE, YELLOW
	SET_SCROLLTEXTCOLOR,	//## %s=""  # Color of text RED,WHITE,BLUE, YELLOW
	SET_COPY_ORIGIN,//## %s="targetname"  # Copy the origin of the ent with targetname to your origin
	SET_DEFEND_TARGET,//## %s="targetname"  # This NPC will attack the target NPC's enemies
	SET_TARGET,//## %s="NULL" # Set/change your target
	SET_TARGET2,//## %s="NULL" # Set/change your target2, on NPC's, this fires when they're knocked out by the red hypo
	SET_LOCATION,//## %s="INVALID" # What trigger_location you're in - Can only be gotten, not set!
	SET_LOADGAME,//## %s="exitholodeck" # Load the savegame that was auto-saved when you started the holodeck
	SET_LOCKYAW,//## %s="off"  # Lock legs to a certain yaw angle (or "off" or "auto" uses current)
	SET_FULLNAME,//## %s="NULL" # This name will appear when ent is scanned by tricorder
	SET_VIEWENTITY,//## %s="NULL" # Make the player look through this ent's eyes - also shunts player movement control to this ent
	SET_LOOPSOUND,//## %s="FILENAME" !!"W:\game\base\!!#sound\*.*" # Looping sound to play on entity
	SET_ICARUS_FREEZE,//## %s="NULL" # Specify name of entity to freeze - !!!NOTE!!! since the ent is frozen, it cannot unfreeze itself, you must have some other entity unfreeze a frozen ent!!!
	SET_ICARUS_UNFREEZE,//## %s="NULL" # Specify name of entity to unfreeze - !!!NOTE!!! since the ent is frozen, it cannot unfreeze itself, you must have some other entity unfreeze a frozen ent!!!

	SET_SCROLLTEXT,	//## %s="" # key of text string to print
	SET_LCARSTEXT,	//## %s="" # key of text string to print in LCARS frame

	//# #sep vectors
	SET_ORIGIN,//## %v="0.0 0.0 0.0" # Set origin explicitly or with TAG
	SET_ANGLES,//## %v="0.0 0.0 0.0" # Set angles explicitly or with TAG
	SET_TELEPORT_DEST,//## %v="0.0 0.0 0.0" # Set origin here as soon as the area is clear

	//# #sep floats
	SET_XVELOCITY,//## %f="0.0" # Velocity along X axis
	SET_YVELOCITY,//## %f="0.0" # Velocity along Y axis
	SET_ZVELOCITY,//## %f="0.0" # Velocity along Z axis
	SET_Z_OFFSET,//## %f="0.0" # Vertical offset from original origin... offset/ent's speed * 1000ms is duration
	SET_TIMESCALE,//## %f="0.0" # Speed-up slow down game (0 - 1.0)
	SET_CAMERA_GROUP_Z_OFS,//## %s="NULL" # when following an ent with the camera, apply this z ofs
	SET_GRAVITY,//## %f="0.0" # Change this ent's gravity - 800 default
	SET_FACEAUX,		//## %f="0.0" # Set face to Aux expression for number of seconds
	SET_FACEBLINK,		//## %f="0.0" # Set face to Blink expression for number of seconds
	SET_FACEBLINKFROWN,	//## %f="0.0" # Set face to Blinkfrown expression for number of seconds
	SET_FACEFROWN,		//## %f="0.0" # Set face to Frown expression for number of seconds
	SET_FACENORMAL,		//## %f="0.0" # Set face to Normal expression for number of seconds
	SET_FACEEYESCLOSED,	//## %f="0.0" # Set face to Eyes closed
	SET_FACEEYESOPENED,	//## %f="0.0" # Set face to Eyes open
	SET_WAIT,		//## %f="0.0" # Change an entity's wait field
	SET_SCALE,			//## %f="0.0" # Scale the entity model

	//# #sep ints
	SET_ANIM_HOLDTIME_LOWER,//## %d="0" # Hold lower anim for number of milliseconds
	SET_ANIM_HOLDTIME_UPPER,//## %d="0" # Hold upper anim for number of milliseconds
	SET_ANIM_HOLDTIME_BOTH,//## %d="0" # Hold lower and upper anims for number of milliseconds
	SET_HEALTH,//## %d="0" # Change health
	SET_ARMOR,//## %d="0" # Change armor
	SET_FRICTION,//## %d="0" # Change ent's friction - 6 default
	SET_DELAYSCRIPTTIME,//## %d="0" # How many milliseconds to wait before running delayscript
	SET_FORWARDMOVE,//## %d="0" # NPC move forward -127(back) to 127
	SET_RIGHTMOVE,//## %d="0" # NPC move right -127(left) to 127
	SET_STARTFRAME,	//## %d="0" # frame to start animation sequence on
	SET_ENDFRAME,	//## %d="0" # frame to end animation sequence on
	SET_ANIMFRAME,	//## %d="0" # frame to set animation sequence to
	SET_COUNT,	//## %d="0" # Change an entity's count field
	SET_MISSIONSTATUSTIME,//## %d="0" # Amount of time until Mission Status should be shown after death
	SET_WIDTH,//## %d="0.0" # Width of NPC bounding box.

	//# #sep booleans
	SET_IGNOREENEMIES,//## %t="BOOL_TYPES" # Do not acquire enemies
	SET_DONTSHOOT,//## %t="BOOL_TYPES" # Others won't shoot you
	SET_NOTARGET,//## %t="BOOL_TYPES" # Others won't pick you as enemy
	SET_LOCKED_ENEMY,//## %t="BOOL_TYPES" # Keep current enemy until dead
	SET_UNDYING,//## %t="BOOL_TYPES" # Can take damage down to 1 but not die
	SET_SOLID,//## %t="BOOL_TYPES" # Make yourself notsolid or solid
	SET_PLAYER_USABLE,//## %t="BOOL_TYPES" # Can be activateby the player's "use" button
	SET_LOOP_ANIM,//## %t="BOOL_TYPES" # For non-NPCs, loop your animation sequence
	SET_INTERFACE,//## %t="BOOL_TYPES" # Player interface on/off
	SET_SHIELDS,//## %t="BOOL_TYPES" # NPC has no shields (Borg do not adapt)
	SET_INVISIBLE,//## %t="BOOL_TYPES" # Makes an NPC not solid and not visible
	SET_VAMPIRE,//## %t="BOOL_TYPES" # Draws only in mirrors/portals
	SET_FORCE_INVINCIBLE,//## %t="BOOL_TYPES" # Force Invincibility effect, also godmode
	SET_VIDEO_FADE_IN,//## %t="BOOL_TYPES" # Makes video playback fade in
	SET_VIDEO_FADE_OUT,//## %t="BOOL_TYPES" # Makes video playback fade out
	SET_PLAYER_LOCKED,//## %t="BOOL_TYPES" # Makes it so player cannot move
	SET_LOCK_PLAYER_WEAPONS,//## %t="BOOL_TYPES" # Makes it so player cannot switch weapons
	SET_NO_IMPACT_DAMAGE,//## %t="BOOL_TYPES" # Stops this ent from taking impact damage
	SET_NO_KNOCKBACK,//## %t="BOOL_TYPES" # Stops this ent from taking knockback from weapons
	SET_INVINCIBLE,//## %t="BOOL_TYPES" # Completely unkillable
	SET_MISSIONSTATUSACTIVE,	//# Turns on Mission Status Screen
	SET_TREASONED,//## %t="BOOL_TYPES" # Player has turned on his own- scripts will stop, NPCs will turn on him and level changes load the brig
	SET_DISABLE_SHADER_ANIM,//## %t="BOOL_TYPES" # Allows turning off an animating shader in a script
	SET_SHADER_ANIM,//## %t="BOOL_TYPES" # Sets a shader with an image map to be under frame control
	SET_SABERACTIVE,//## %t="BOOL_TYPES" # Turns saber on/off
	SET_ADJUST_AREA_PORTALS,//## %t="BOOL_TYPES" # Only set this on things you move with script commands that you *want* to open/close area portals.  Default is off.
	SET_DMG_BY_HEAVY_WEAP_ONLY,//## %t="BOOL_TYPES" # When true, only a heavy weapon class missile/laser can damage this ent.
	SET_SHIELDED,//## %t="BOOL_TYPES" # When true, ion_cannon is shielded from any kind of damage.
	SET_NO_GROUPS,//## %t="BOOL_TYPES" # This NPC cannot alert groups or be part of a group
	SET_NO_MINDTRICK,//## %t="BOOL_TYPES" # Makes NPC immune to jedi mind-trick
	SET_INACTIVE,//## %t="BOOL_TYPES" # in lieu of using a target_activate or target_deactivate
	SET_FUNC_USABLE_VISIBLE,//## %t="BOOL_TYPES" # provides an alternate way of changing func_usable to be visible or not, DOES NOT AFFECT SOLID
	SET_SECRET_AREA_FOUND,//## %t="BOOL_TYPES" # Increment secret areas found counter
	SET_MISSION_STATUS_SCREEN,//## %t="BOOL_TYPES" # Display Mission Status screen before advancing to next level
	SET_END_SCREENDISSOLVE,//## %t="BOOL_TYPES" # End of game dissolve into star background and credits
	SET_MORELIGHT,//## %t="BOOL_TYPES" # NPC will have a minlight of 96
	SET_DISMEMBERABLE,//## %t="BOOL_TYPES" # NPC will not be dismemberable if you set this to false (default is true)
	SET_USE_SUBTITLES,//## %t="BOOL_TYPES" # When true NPC will always display subtitle regardless of subtitle setting
	SET_CLEAN_DAMAGING_ENTS,//## %t="BOOL_TYPES" # Removes entities that could muck up cinematics, explosives, turrets, seekers.
	SET_HUD,//## %t="BOOL_TYPES" # Turns on/off HUD

	//# #sep calls
	SET_SKILL,//## %r%d="0" # Cannot set this, only get it - valid values are 0 through 3

	//# #sep Special tables
	SET_ANIM_UPPER,//## %t="ANIM_NAMES" # Torso and head anim
	SET_ANIM_LOWER,//## %t="ANIM_NAMES" # Legs anim
	SET_ANIM_BOTH,//## %t="ANIM_NAMES" # Set same anim on torso and legs
	SET_EVENT,//## %t="EVENT_NAMES" # Events you can initiate
	SET_ITEM,//## %t="ITEM_NAMES" # Give items
	SET_MUSIC_STATE,//## %t="MUSIC_STATES" # Set the state of the dynamic music

	SET_FORCE_HEAL_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_JUMP_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_SPEED_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_PUSH_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_PULL_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_MINDTRICK_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_GRIP_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_FORCE_LIGHTNING_LEVEL,//## %t="FORCE_LEVELS" # Change force power level
	SET_SABER_THROW,//## %t="FORCE_LEVELS" # Change force power level
	SET_SABER_DEFENSE,//## %t="FORCE_LEVELS" # Change force power level
	SET_SABER_OFFENSE,//## %t="FORCE_LEVELS" # Change force power level

	SET_OBJECTIVE_SHOW,	//## %t="OBJECTIVES" # Show objective on mission screen
	SET_OBJECTIVE_HIDE,	//## %t="OBJECTIVES" # Hide objective from mission screen
	SET_OBJECTIVE_SUCCEEDED,//## %t="OBJECTIVES" # Mark objective as completed
	SET_OBJECTIVE_FAILED,	//## %t="OBJECTIVES" # Mark objective as failed

	SET_MISSIONFAILED,		//## %t="MISSIONFAILED" # Mission failed screen activates

	SET_TACTICAL_SHOW,		//## %t="TACTICAL" # Show tactical info on mission objectives screen
	SET_TACTICAL_HIDE,		//## %t="TACTICAL" # Hide tactical info on mission objectives screen
	SET_OBJECTIVE_CLEARALL,	//## # Force all objectives to be hidden
/*
	SET_OBJECTIVEFOSTER,
*/
	SET_MISSIONSTATUSTEXT,	//## %t="STATUSTEXT" # Text to appear in mission status screen
	SET_MENU_SCREEN,//## %t="MENUSCREENS" # Brings up specified menu screen

	SET_CLOSINGCREDITS,		//## # Show closing credits

	//in-bhc tables
	SET_LEAN,//## %t="LEAN_TYPES" # Lean left, right or stop leaning

	//# #eol
	SET_
} setType_t;

#if !defined( _GAME ) && !defined( _CGAME ) && !defined( UI_BUILD )

// this enum isn't used directly by the game, it's mainly for BehavEd to scan for...

typedef enum //# playType_e
{
	//# #sep Types of file to play
	PLAY_ROFF = 0,//## %s="filename" !!"W:\game\base\scripts\!!#*.rof" # Play a ROFF file

	//# #eol
	PLAY_NUMBEROF

} playType_t;

const	int	Q3_TIME_SCALE	= 1;	//MILLISECONDS

extern char	cinematicSkipScript[1024];

void     Q3_CameraDistance ( float distance, float initLerp );
void     Q3_CameraFollow   ( const char *name, float speed, float initLerp );
void     Q3_CameraRoll     ( float angle, float duration );
void     Q3_CameraTrack    ( const char *name, float speed, float initLerp );
void     Q3_DeclareVariable( int type, const char *name );
void     Q3_DebugPrint     ( int level, const char *format, ... );
void     Q3_DPrintf        ( const char *, ... );
void     Q3_FreeVariable   ( const char *name );
void     Q3_SetAnimBoth    ( int entID, const char *anim_name );
void     Q3_SetVar         ( int taskID, int entID, const char *type_name, const char *data );
void     Q3_SetVelocity    ( int entID, vec3_t angles );
void     Q3_TaskIDClear    ( int *taskID );
void     Q3_TaskIDComplete ( sharedEntity_t *ent, taskID_t taskType );
qboolean Q3_TaskIDPending  ( sharedEntity_t *ent, taskID_t taskType );
void     Q3_TaskIDSet      ( sharedEntity_t *ent, taskID_t taskType, int taskID );

#endif
