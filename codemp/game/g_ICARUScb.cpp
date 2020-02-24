/*
===========================================================================
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

// ICARUS callback file, all that can be handled within vm's is handled in here.

#include "qcommon/q_shared.hpp"
#include "game/bg_public.hpp"
#include "game/b_local.hpp"
#include "icarus/Q3_Interface.hpp"
#include "icarus/Q3_Registers.hpp"

extern stringID_table_t WPTable[];
extern stringID_table_t BSTable[];

//This is a hack I guess. It's because we can't include the file this enum is in
//unless we're using cpp. But we need it for the interpreter stuff.
//In any case, DO NOT modify this enum.

// Hack++
// This code is compiled as C++ on Xbox. We could try and rig something above
// so that we only get the C version of the includes (no full Icarus) in that
// scenario, but I think we'll just try to leave this out instead.
//#if defined(__linux__) && defined(__GCC__) || !defined(__linux__)
enum
{
	TK_EOF = -1,
	TK_UNDEFINED,
	TK_COMMENT,
	TK_EOL,
	TK_CHAR,
	TK_STRING,
	TK_INT,
	TK_INTEGER = TK_INT,
	TK_FLOAT,
	TK_IDENTIFIER,
	TK_USERDEF,
};
//#endif

#include "icarus/interpreter.hpp"

extern stringID_table_t animTable [MAX_ANIMATIONS+1];

stringID_table_t setTable[] =
{
	ENUM2STRING(SET_SPAWNSCRIPT),//0
	ENUM2STRING(SET_USESCRIPT),
	ENUM2STRING(SET_AWAKESCRIPT),
	ENUM2STRING(SET_ANGERSCRIPT),
	ENUM2STRING(SET_ATTACKSCRIPT),
	ENUM2STRING(SET_VICTORYSCRIPT),
	ENUM2STRING(SET_PAINSCRIPT),
	ENUM2STRING(SET_FLEESCRIPT),
	ENUM2STRING(SET_DEATHSCRIPT),
	ENUM2STRING(SET_DELAYEDSCRIPT),
	ENUM2STRING(SET_BLOCKEDSCRIPT),
	ENUM2STRING(SET_FFIRESCRIPT),
	ENUM2STRING(SET_FFDEATHSCRIPT),
	ENUM2STRING(SET_MINDTRICKSCRIPT),
	ENUM2STRING(SET_NO_MINDTRICK),
	ENUM2STRING(SET_ORIGIN),
	ENUM2STRING(SET_TELEPORT_DEST),
	ENUM2STRING(SET_ANGLES),
	ENUM2STRING(SET_XVELOCITY),
	ENUM2STRING(SET_YVELOCITY),
	ENUM2STRING(SET_ZVELOCITY),
	ENUM2STRING(SET_Z_OFFSET),
	ENUM2STRING(SET_ANIM_UPPER),
	ENUM2STRING(SET_ANIM_LOWER),
	ENUM2STRING(SET_ANIM_BOTH),
	ENUM2STRING(SET_ANIM_HOLDTIME_LOWER),
	ENUM2STRING(SET_ANIM_HOLDTIME_UPPER),
	ENUM2STRING(SET_ANIM_HOLDTIME_BOTH),
	ENUM2STRING(SET_HEALTH),
	ENUM2STRING(SET_ARMOR),
	ENUM2STRING(SET_COPY_ORIGIN),
	ENUM2STRING(SET_ITEM),
	ENUM2STRING(SET_FRICTION),
	ENUM2STRING(SET_GRAVITY),
	ENUM2STRING(SET_DONTSHOOT),
	ENUM2STRING(SET_LOCKED_ENEMY),
	ENUM2STRING(SET_NOTARGET),
	ENUM2STRING(SET_LEAN),
	ENUM2STRING(SET_UNDYING),
	ENUM2STRING(SET_TREASONED),
	ENUM2STRING(SET_DISABLE_SHADER_ANIM),
	ENUM2STRING(SET_SHADER_ANIM),
	ENUM2STRING(SET_INVINCIBLE),
	ENUM2STRING(SET_TARGETNAME),
	ENUM2STRING(SET_TARGET),
	ENUM2STRING(SET_TARGET2),
	ENUM2STRING(SET_LOCATION),
	ENUM2STRING(SET_PAINTARGET),
	ENUM2STRING(SET_TIMESCALE),
	ENUM2STRING(SET_DELAYSCRIPTTIME),
	ENUM2STRING(SET_FORWARDMOVE),
	ENUM2STRING(SET_RIGHTMOVE),
	ENUM2STRING(SET_LOCKYAW),
	ENUM2STRING(SET_SOLID),
	ENUM2STRING(SET_CAMERA_GROUP),
	ENUM2STRING(SET_CAMERA_GROUP_Z_OFS),
	ENUM2STRING(SET_CAMERA_GROUP_TAG),
	ENUM2STRING(SET_ADDRHANDBOLT_MODEL),
	ENUM2STRING(SET_REMOVERHANDBOLT_MODEL),
	ENUM2STRING(SET_ADDLHANDBOLT_MODEL),
	ENUM2STRING(SET_REMOVELHANDBOLT_MODEL),
	ENUM2STRING(SET_FACEAUX),
	ENUM2STRING(SET_FACEBLINK),
	ENUM2STRING(SET_FACEBLINKFROWN),
	ENUM2STRING(SET_FACEFROWN),
	ENUM2STRING(SET_FACENORMAL),
	ENUM2STRING(SET_FACEEYESCLOSED),
	ENUM2STRING(SET_FACEEYESOPENED),
	ENUM2STRING(SET_SCROLLTEXT),
	ENUM2STRING(SET_LCARSTEXT),
	ENUM2STRING(SET_SCROLLTEXTCOLOR),
	ENUM2STRING(SET_CAPTIONTEXTCOLOR),
	ENUM2STRING(SET_CENTERTEXTCOLOR),
	ENUM2STRING(SET_PLAYER_USABLE),
	ENUM2STRING(SET_STARTFRAME),
	ENUM2STRING(SET_ENDFRAME),
	ENUM2STRING(SET_ANIMFRAME),
	ENUM2STRING(SET_LOOP_ANIM),
	ENUM2STRING(SET_INTERFACE),
	ENUM2STRING(SET_SHIELDS),
	ENUM2STRING(SET_NO_KNOCKBACK),
	ENUM2STRING(SET_INVISIBLE),
	ENUM2STRING(SET_VAMPIRE),
	ENUM2STRING(SET_FORCE_INVINCIBLE),
	ENUM2STRING(SET_PLAYER_LOCKED),
	ENUM2STRING(SET_LOCK_PLAYER_WEAPONS),
	ENUM2STRING(SET_NO_IMPACT_DAMAGE),
	ENUM2STRING(SET_PARM1),
	ENUM2STRING(SET_PARM2),
	ENUM2STRING(SET_PARM3),
	ENUM2STRING(SET_PARM4),
	ENUM2STRING(SET_PARM5),
	ENUM2STRING(SET_PARM6),
	ENUM2STRING(SET_PARM7),
	ENUM2STRING(SET_PARM8),
	ENUM2STRING(SET_PARM9),
	ENUM2STRING(SET_PARM10),
	ENUM2STRING(SET_PARM11),
	ENUM2STRING(SET_PARM12),
	ENUM2STRING(SET_PARM13),
	ENUM2STRING(SET_PARM14),
	ENUM2STRING(SET_PARM15),
	ENUM2STRING(SET_PARM16),
	ENUM2STRING(SET_DEFEND_TARGET),
	ENUM2STRING(SET_WAIT),
	ENUM2STRING(SET_COUNT),
	ENUM2STRING(SET_VIDEO_PLAY),
	ENUM2STRING(SET_VIDEO_FADE_IN),
	ENUM2STRING(SET_VIDEO_FADE_OUT),
	ENUM2STRING(SET_LOADGAME),
	ENUM2STRING(SET_MENU_SCREEN),
	ENUM2STRING(SET_OBJECTIVE_SHOW),
	ENUM2STRING(SET_OBJECTIVE_HIDE),
	ENUM2STRING(SET_OBJECTIVE_SUCCEEDED),
	ENUM2STRING(SET_OBJECTIVE_FAILED),
	ENUM2STRING(SET_MISSIONFAILED),
	ENUM2STRING(SET_TACTICAL_SHOW),
	ENUM2STRING(SET_TACTICAL_HIDE),
	ENUM2STRING(SET_SCALE),
	ENUM2STRING(SET_OBJECTIVE_CLEARALL),
	ENUM2STRING(SET_MISSIONSTATUSTEXT),
	ENUM2STRING(SET_WIDTH),
	ENUM2STRING(SET_CLOSINGCREDITS),
	ENUM2STRING(SET_SKILL),
	ENUM2STRING(SET_MISSIONSTATUSTIME),
	ENUM2STRING(SET_FULLNAME),
	ENUM2STRING(SET_FORCE_HEAL_LEVEL),
	ENUM2STRING(SET_FORCE_JUMP_LEVEL),
	ENUM2STRING(SET_FORCE_SPEED_LEVEL),
	ENUM2STRING(SET_FORCE_PUSH_LEVEL),
	ENUM2STRING(SET_FORCE_PULL_LEVEL),
	ENUM2STRING(SET_FORCE_MINDTRICK_LEVEL),
	ENUM2STRING(SET_FORCE_GRIP_LEVEL),
	ENUM2STRING(SET_FORCE_LIGHTNING_LEVEL),
	ENUM2STRING(SET_SABER_THROW),
	ENUM2STRING(SET_SABER_DEFENSE),
	ENUM2STRING(SET_SABER_OFFENSE),
	ENUM2STRING(SET_VIEWENTITY),
	ENUM2STRING(SET_SABERACTIVE),
	ENUM2STRING(SET_ADJUST_AREA_PORTALS),
	ENUM2STRING(SET_DMG_BY_HEAVY_WEAP_ONLY),
	ENUM2STRING(SET_SHIELDED),
	ENUM2STRING(SET_NO_GROUPS),
	ENUM2STRING(SET_INACTIVE),
	ENUM2STRING(SET_FUNC_USABLE_VISIBLE),
	ENUM2STRING(SET_MISSION_STATUS_SCREEN),
	ENUM2STRING(SET_END_SCREENDISSOLVE),
	ENUM2STRING(SET_LOOPSOUND),
	ENUM2STRING(SET_ICARUS_FREEZE),
	ENUM2STRING(SET_ICARUS_UNFREEZE),
	ENUM2STRING(SET_MORELIGHT),
	ENUM2STRING(SET_CINEMATIC_SKIPSCRIPT),
	ENUM2STRING(SET_DISMEMBERABLE),
	ENUM2STRING(SET_MUSIC_STATE),
	ENUM2STRING(SET_USE_SUBTITLES),
	ENUM2STRING(SET_CLEAN_DAMAGING_ENTS),
	ENUM2STRING(SET_HUD),

//FIXME: add BOTH_ attributes here too
	{"",	SET_},
};

void Q3_TaskIDClear( int *taskID )
{
	*taskID = -1;
}

void G_DebugPrint( int printLevel, const char *format, ... )
{
	va_list		argptr;
	char		text[1024] = {0};

	//Don't print messages they don't want to see
	//if ( g_ICARUSDebug->integer < level )
	if (developer.integer != 2)
		return;

	va_start( argptr, format );
	Q_vsnprintf(text, sizeof( text ), format, argptr );
	va_end( argptr );

	//Add the color formatting
	switch ( printLevel )
	{
		case WL_ERROR:
			Com_Printf ( S_COLOR_RED"ERROR: %s", text );
			break;

		case WL_WARNING:
			Com_Printf ( S_COLOR_YELLOW"WARNING: %s", text );
			break;

		case WL_DEBUG:
			{
				int		entNum;
				char	*buffer;

				entNum = atoi( text );

				//if ( ( ICARUS_entFilter >= 0 ) && ( ICARUS_entFilter != entNum ) )
				//	return;

				buffer = (char *) text;
				buffer += 5;

				if ( ( entNum < 0 ) || ( entNum >= MAX_GENTITIES ) )
					entNum = 0;

				Com_Printf ( S_COLOR_BLUE"DEBUG: %s(%d): %s\n", g_entities[entNum].script_targetname, entNum, buffer );
				break;
			}
		default:
		case WL_VERBOSE:
			Com_Printf ( S_COLOR_GREEN"INFO: %s", text );
			break;
	}
}

static char *Q3_GetAnimLower( gentity_t *ent )
{
	int anim = 0;

	if ( ent->client == nullptr )
	{
		G_DebugPrint( WL_WARNING, "Q3_GetAnimLower: attempted to read animation state off non-client!\n" );
		return nullptr;
	}

	anim = ent->client->ps.legsAnim;

	return (char *)animTable[anim].name;
}

static char *Q3_GetAnimUpper( gentity_t *ent )
{
	int anim = 0;

	if ( ent->client == nullptr )
	{
		G_DebugPrint( WL_WARNING, "Q3_GetAnimUpper: attempted to read animation state off non-client!\n" );
		return nullptr;
	}

	anim = ent->client->ps.torsoAnim;

	return (char *)animTable[anim].name;
}

static char *Q3_GetAnimBoth( gentity_t *ent )
{
 	char	*lowerName, *upperName;

	lowerName = Q3_GetAnimLower( ent );
	upperName = Q3_GetAnimUpper( ent );

	if ( !lowerName || !lowerName[0] )
	{
		G_DebugPrint( WL_WARNING, "Q3_GetAnimBoth: nullptr legs animation string found!\n" );
		return nullptr;
	}

	if ( !upperName || !upperName[0] )
	{
		G_DebugPrint( WL_WARNING, "Q3_GetAnimBoth: nullptr torso animation string found!\n" );
		return nullptr;
	}

	if ( Q_stricmp( lowerName, upperName ) )
	{
#ifdef _DEBUG	// sigh, cut down on tester reports that aren't important
		G_DebugPrint( WL_WARNING, "Q3_GetAnimBoth: legs and torso animations did not match : returning legs\n" );
#endif
	}

	return lowerName;
}

int Q3_PlaySound( int taskID, int entID, const char *name, const char *channel )
{
	gentity_t		*ent = &g_entities[entID];
	char			finalName[MAX_QPATH];
	soundChannel_e	voice_chan = CHAN_VOICE; // set a default so the compiler doesn't bitch
	bool		type_voice = false;
	int				soundHandle;
	bool		bBroadcast;

	Q_strncpyz( finalName, name, MAX_QPATH );
	Q_strupr(finalName);
	//G_AddSexToMunroString( finalName, true );

	COM_StripExtension( (const char *)finalName, finalName, sizeof( finalName ) );

	soundHandle = G_SoundIndex( (char *) finalName );
	bBroadcast = false;

	if ( ( Q_stricmp( channel, "CHAN_ANNOUNCER" ) == 0 ) || (ent->classname && Q_stricmp("target_scriptrunner", ent->classname ) == 0) ) {
		bBroadcast = true;
	}

	// moved here from further down so I can easily check channel-type without code dup...

	if ( Q_stricmp( channel, "CHAN_VOICE" ) == 0 )
	{
		voice_chan = CHAN_VOICE;
		type_voice = true;
	}
	else if ( Q_stricmp( channel, "CHAN_VOICE_ATTEN" ) == 0 )
	{
		voice_chan = CHAN_AUTO;//CHAN_VOICE_ATTEN;
		type_voice = true;
	}
	else if ( Q_stricmp( channel, "CHAN_VOICE_GLOBAL" ) == 0 ) // this should broadcast to everyone, put only casue animation on G_SoundOnEnt...
	{
		voice_chan = CHAN_AUTO;//CHAN_VOICE_GLOBAL;
		type_voice = true;
		bBroadcast = true;
	}

	// if we're in-camera, check for skipping cinematic and ifso, no subtitle print (since screen is not being
	//	updated anyway during skipping). This stops leftover subtitles being left onscreen after unskipping.

	/*
	if (!in_camera ||
		(!g_skippingcin || !g_skippingcin->integer)
		)	// paranoia towards project end <g>
	{
		// Text on
		// certain NPC's we always want to use subtitles regardless of subtitle setting
		if (g_subtitles->integer == 1 || (ent->NPC && (ent->NPC->scriptFlags & SCF_USE_SUBTITLES) ) ) // Show all text
		{
			if ( in_camera)	// Cinematic
			{
				trap->SendServerCommand( -1, va("ct \"%s\" %i", finalName, soundHandle) );
			}
			else //if (precacheWav[i].speaker==SP_NONE)	//  lower screen text
			{
				sharedEntity_t		*ent2 = SV_GentityNum(0);
				// the numbers in here were either the original ones Bob entered (350), or one arrived at from checking the distance Chell stands at in stasis2 by the computer core that was submitted as a bug report...

				if (bBroadcast || (DistanceSquared(ent->currentOrigin, ent2->currentOrigin) < ((voice_chan == CHAN_VOICE_ATTEN)?(350 * 350):(1200 * 1200)) ) )
				{
					trap->SendServerCommand( -1, va("ct \"%s\" %i", finalName, soundHandle) );
				}
			}
		}
		// Cinematic only
		else if (g_subtitles->integer == 2) // Show only talking head text and CINEMATIC
		{
			if ( in_camera)	// Cinematic text
			{
				trap->SendServerCommand( -1, va("ct \"%s\" %i", finalName, soundHandle));
			}
		}

	}
	*/

	if ( type_voice )
	{
		if ( timescale.value > 1.0f ) {
			//Skip the damn sound!
			return true;
		}
		else {
			//This the voice channel
			G_Sound( ent, voice_chan, G_SoundIndex( (char *)finalName ) );
		}
		//Remember we're waiting for this
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_CHAN_VOICE, taskID );

		return false;
	}

	if ( bBroadcast )
	{//Broadcast the sound
		gentity_t	*te;

		te = G_TempEntity( ent->r.currentOrigin, EV_GLOBAL_SOUND );
		te->s.eventParm = soundHandle;
		te->r.svFlags |= SVF_BROADCAST;
	}
	else
	{
		G_Sound( ent, CHAN_AUTO, soundHandle );
	}

	return true;
}

void Q3_Play( int taskID, int entID, const char *type, const char *name )
{
	gentity_t *ent = &g_entities[entID];

	if ( !Q_stricmp( type, "PLAY_ROFF" ) )
	{
		// Try to load the requested ROFF
		ent->roffid = trap->ROFF_Cache((char*)name);
		if ( ent->roffid )
		{
			ent->roffname = G_NewString( name );

			// Start the roff from the beginning
			//ent->roff_ctr = 0;

			//Save this off for later
			trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_MOVE_NAV, taskID );

			// Let the ROFF playing start.
			//ent->next_roff_time = level.time;

			//rww - Maybe use pos1 and pos2? I don't think we need to care if these values are sent across the net.
			// These need to be initialised up front...
			//VectorCopy( ent->r.currentOrigin, ent->pos1 );
			//VectorCopy( ent->r.currentAngles, ent->pos2 );
			VectorCopy( ent->r.currentOrigin, ent->s.origin2 );
			VectorCopy( ent->r.currentAngles, ent->s.angles2 );

			trap->LinkEntity( (sharedEntity_t *)ent );

			trap->ROFF_Play(ent->s.number, ent->roffid, true);
		}
	}
}

void anglerCallback( gentity_t *ent )
{
	//Complete the task
	trap->ICARUS_TaskIDComplete( (sharedEntity_t *)ent, TID_ANGLE_FACE );

	//Set the currentAngles, clear all movement
	VectorMA( ent->s.apos.trBase, (ent->s.apos.trDuration*0.001f), ent->s.apos.trDelta, ent->r.currentAngles );
	VectorCopy( ent->r.currentAngles, ent->s.apos.trBase );
	VectorClear( ent->s.apos.trDelta );
	ent->s.apos.trDuration = 1;
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = level.time;

	//Stop thinking
	ent->reached = 0;
	if ( ent->think == anglerCallback )
	{
		ent->think = 0;
	}

	//link
	trap->LinkEntity( (sharedEntity_t *)ent );
}

void moverCallback( gentity_t *ent )
{	//complete the task
	trap->ICARUS_TaskIDComplete( (sharedEntity_t *)ent, TID_MOVE_NAV );

	// play sound
	ent->s.loopSound = 0;//stop looping sound
	ent->s.loopIsSoundset = false;
	G_PlayDoorSound( ent, BMS_END );//play end sound

	if ( ent->moverState == MOVER_1TO2 )
	{//reached open
		// reached pos2
		MatchTeam( ent, MOVER_POS2, level.time );
		//SetMoverState( ent, MOVER_POS2, level.time );
	}
	else if ( ent->moverState == MOVER_2TO1 )
	{//reached closed
		MatchTeam( ent, MOVER_POS1, level.time );
		//SetMoverState( ent, MOVER_POS1, level.time );
	}

	if ( ent->blocked == Blocked_Mover )
	{
		ent->blocked = 0;
	}

//	if ( !Q_stricmp( "misc_model_breakable", ent->classname ) && ent->physicsBounce )
//	{//a gravity-affected model
//		misc_model_breakable_gravity_init( ent, false );
//	}
}

void Blocked_Mover( gentity_t *ent, gentity_t *other )
{
	// remove anything other than a client -- no longer the case

	// don't remove security keys or goodie keys
	if ( other->s.eType == ET_ITEM)
	{
		// should we be doing anything special if a key blocks it... move it somehow..?
	}
	// if your not a client, or your a dead client remove yourself...
	else if ( other->s.number && (!other->client || (other->client && other->health <= 0 && other->r.contents == CONTENTS_CORPSE && !other->message)) )
	{
		//if ( !other->taskManager || !other->taskManager->IsRunning() )
		{
			// if an item or weapon can we do a little explosion..?
			G_FreeEntity( other );
			return;
		}
	}

	if ( ent->damage ) {
		G_Damage( other, ent, ent, nullptr, nullptr, ent->damage, 0, MOD_CRUSH );
	}
}

void moveAndRotateCallback( gentity_t *ent )
{
	//stop turning
	anglerCallback( ent );
	//stop moving
	moverCallback( ent );
}

// Lerps the origin of an entity to its starting position
void Q3_Lerp2Start( int entID, int taskID, float duration )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Lerp2Start: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];

	if ( ent->client || Q_stricmp(ent->classname, "target_scriptrunner") == 0 )
	{
		G_DebugPrint( WL_ERROR, "Q3_Lerp2Start: ent %d is NOT a mover!\n", entID);
		return;
	}

	if ( ent->s.eType != ET_MOVER )
	{
		ent->s.eType = ET_MOVER;
	}

	//FIXME: set up correctly!!!
	ent->moverState = MOVER_2TO1;
	ent->s.eType = ET_MOVER;
	ent->reached = moverCallback;		//Callsback the the completion of the move
	if ( ent->damage )
	{
		ent->blocked = Blocked_Mover;
	}

	ent->s.pos.trDuration = duration * 10;	//In seconds
	ent->s.pos.trTime = level.time;

	trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_MOVE_NAV, taskID );
	// starting sound
	G_PlayDoorLoopSound( ent );
	G_PlayDoorSound( ent, BMS_START );	//??

	trap->LinkEntity( (sharedEntity_t *)ent );
}

// Lerps the origin of an entity to its ending position
void Q3_Lerp2End( int entID, int taskID, float duration )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Lerp2End: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];

	if ( ent->client || Q_stricmp(ent->classname, "target_scriptrunner") == 0 )
	{
		G_DebugPrint( WL_ERROR, "Q3_Lerp2End: ent %d is NOT a mover!\n", entID);
		return;
	}

	if ( ent->s.eType != ET_MOVER )
	{
		ent->s.eType = ET_MOVER;
	}

	//FIXME: set up correctly!!!
	ent->moverState = MOVER_1TO2;
	ent->s.eType = ET_MOVER;
	ent->reached = moverCallback;		//Callsback the the completion of the move
	if ( ent->damage )
	{
		ent->blocked = Blocked_Mover;
	}

	ent->s.pos.trDuration = duration * 10;	//In seconds
	ent->s.time = level.time;

	trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_MOVE_NAV, taskID );
	// starting sound
	G_PlayDoorLoopSound( ent );
	G_PlayDoorSound( ent, BMS_START );	//??

	trap->LinkEntity( (sharedEntity_t *)ent );
}

// Lerps the origin and angles of an entity to the destination values
void Q3_Lerp2Pos( int taskID, int entID, vec3_t origin, vec3_t angles, float duration )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Lerp2Pos: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];
	vec3_t		ang;
	int			i;
	moverState_e moverState;

	if ( ent->client || Q_stricmp(ent->classname, "target_scriptrunner") == 0 )
	{
		G_DebugPrint( WL_ERROR, "Q3_Lerp2Pos: ent %d is NOT a mover!\n", entID);
		return;
	}

	if ( ent->s.eType != ET_MOVER )
	{
		ent->s.eType = ET_MOVER;
	}

	//Don't allow a zero duration
	if ( duration == 0 )
		duration = 1;

	// Movement

	moverState = ent->moverState;

	if ( moverState == MOVER_POS1 || moverState == MOVER_2TO1 )
	{
		VectorCopy( ent->r.currentOrigin, ent->pos1 );
		VectorCopy( origin, ent->pos2 );

		moverState = MOVER_1TO2;
	}
	else
	{
		VectorCopy( ent->r.currentOrigin, ent->pos2 );
		VectorCopy( origin, ent->pos1 );

		moverState = MOVER_2TO1;
	}

	InitMoverTrData( ent );

	ent->s.pos.trDuration = duration;

	// start it going
	MatchTeam( ent, moverState, level.time );
	//SetMoverState( ent, moverState, level.time );

	//Only do the angles if specified
	if ( angles != nullptr )
	{

		// Rotation

		for ( i = 0; i < 3; i++ )
		{
			ang[i] = AngleDelta( angles[i], ent->r.currentAngles[i] );
			ent->s.apos.trDelta[i] = ( ang[i] / ( duration * 0.001f ) );
		}

		VectorCopy( ent->r.currentAngles, ent->s.apos.trBase );

		if ( ent->alt_fire )
		{
			ent->s.apos.trType = TR_LINEAR_STOP;
		}
		else
		{
			ent->s.apos.trType = TR_NONLINEAR_STOP;
		}
		ent->s.apos.trDuration = duration;

		ent->s.apos.trTime = level.time;

		ent->reached = moveAndRotateCallback;
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANGLE_FACE, taskID );
	}
	else
	{
		//Setup the last bits of information
		ent->reached = moverCallback;
	}

	if ( ent->damage )
	{
		ent->blocked = Blocked_Mover;
	}

	trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_MOVE_NAV, taskID );
	// starting sound
	G_PlayDoorLoopSound( ent );
	G_PlayDoorSound( ent, BMS_START );	//??

	trap->LinkEntity( (sharedEntity_t *)ent );
}

// Lerps the angles to the destination value
void Q3_Lerp2Angles( int taskID, int entID, vec3_t angles, float duration )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Lerp2Angles: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];
	vec3_t		ang;
	int			i;

	if ( ent->client || Q_stricmp(ent->classname, "target_scriptrunner") == 0 )
	{
		G_DebugPrint( WL_ERROR, "Q3_Lerp2Angles: ent %d is NOT a mover!\n", entID);
		return;
	}

	//If we want an instant move, don't send 0...
	ent->s.apos.trDuration = (duration>0) ? duration : 1;

	for ( i = 0; i < 3; i++ )
	{
		ang [i] = AngleSubtract( angles[i], ent->r.currentAngles[i]);
		ent->s.apos.trDelta[i] = ( ang[i] / ( ent->s.apos.trDuration * 0.001f ) );
	}

	VectorCopy( ent->r.currentAngles, ent->s.apos.trBase );

	if ( ent->alt_fire )
	{
		ent->s.apos.trType = TR_LINEAR_STOP;
	}
	else
	{
		ent->s.apos.trType = TR_NONLINEAR_STOP;
	}

	ent->s.apos.trTime = level.time;

	trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANGLE_FACE, taskID );

	//ent->e_ReachedFunc = reachedF_NULL;
	ent->think = anglerCallback;
	ent->nextthink = level.time + duration;

	trap->LinkEntity( (sharedEntity_t *)ent );
}

// Gets the value of a tag by the give name
int	Q3_GetTag( int entID, const char *name, int lookup, vec3_t info )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_GetTag: invalid entID %d\n", entID);

		return 0;
	}

	gentity_t	*ent = &g_entities[entID];

	if (!ent->inuse)
	{
		assert(0);
		return 0;
	}

	switch ( lookup )
	{
	case TYPE_ORIGIN:
		return TAG_GetOrigin( ent->ownername, name, info );
		break;

	case TYPE_ANGLES:
		return TAG_GetAngles( ent->ownername, name, info );
		break;
	}

	return 0;
}

// Uses an entity
void Q3_Use( int entID, const char *target )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Use: invalid entID %d\n", entID);

		return;
	}

	if (!target || !target[0])
	{
		G_DebugPrint(WL_WARNING, "Q3_Use: target is nullptr!\n");
		return;
	}

	gentity_t *ent  = &g_entities[entID];

	G_UseTargets2(ent, ent, target);
}

void Q3_Kill( int entID, const char *name )
{
	gentity_t	*ent = &g_entities[entID];
	gentity_t	*victim = nullptr;
	int			o_health;

	if( !Q_stricmp( name, "self") )
	{
		victim = ent;
	}
	else if( !Q_stricmp( name, "enemy" ) )
	{
		victim = ent->enemy;
	}
	else
	{
		victim = G_Find (nullptr, FOFS(targetname), (char *) name );
	}

	if ( !victim )
	{
		G_DebugPrint( WL_WARNING, "Q3_Kill: can't find %s\n", name);
		return;
	}

	//rww - I guess this would only apply to NPCs anyway. I'm not going to bother.
	//if ( victim == ent )
	//{//don't ICARUS_FreeEnt me, I'm in the middle of a script!  (FIXME: shouldn't ICARUS handle this internally?)
	//	victim->svFlags |= SVF_KILLED_SELF;
	//}

	o_health = victim->health;
	victim->health = 0;
	if ( victim->client )
	{
		victim->flags |= FL_NO_KNOCKBACK;
	}
	if( victim->die != nullptr )	// check can be omitted
	{
		//GEntity_DieFunc( victim, nullptr, nullptr, o_health, MOD_UNKNOWN );
		victim->die(victim, victim, victim, o_health, MOD_UNKNOWN);
	}
}

void Q3_RemoveEnt( gentity_t *victim )
{
	if( victim->client )
	{
		G_DebugPrint( WL_WARNING, "Q3_RemoveEnt: You can't remove clients in MP!\n" );
		assert(0); //can't remove clients in MP
		/*
		//ClientDisconnect(ent);
		victim->s.eFlags |= EF_NODRAW;
		victim->s.eType = ET_INVISIBLE;
		victim->contents = 0;
		victim->health = 0;
		victim->targetname = nullptr;

		if ( victim->NPC && victim->NPC->tempGoal != nullptr )
		{
			G_FreeEntity( victim->NPC->tempGoal );
			victim->NPC->tempGoal = nullptr;
		}
		if ( victim->client->ps.saberEntityNum != ENTITYNUM_NONE && victim->client->ps.saberEntityNum > 0 )
		{
			if ( g_entities[victim->client->ps.saberEntityNum].inuse )
			{
				G_FreeEntity( &g_entities[victim->client->ps.saberEntityNum] );
			}
			victim->client->ps.saberEntityNum = ENTITYNUM_NONE;
		}
		//Disappear in half a second
		victim->e_ThinkFunc = thinkF_G_FreeEntity;
		victim->nextthink = level.time + 500;
		return;
		*/
	}
	else
	{
		victim->think = G_FreeEntity;
		victim->nextthink = level.time + 100;
	}
}

void Q3_Remove( int entID, const char *name )
{
	gentity_t *ent = &g_entities[entID];
	gentity_t	*victim = nullptr;

	if( !Q_stricmp( "self", name ) )
	{
		victim = ent;
		if ( !victim )
		{
			G_DebugPrint( WL_WARNING, "Q3_Remove: can't find %s\n", name );
			return;
		}
		Q3_RemoveEnt( victim );
	}
	else if( !Q_stricmp( "enemy", name ) )
	{
		victim = ent->enemy;
		if ( !victim )
		{
			G_DebugPrint( WL_WARNING, "Q3_Remove: can't find %s\n", name );
			return;
		}
		Q3_RemoveEnt( victim );
	}
	else
	{
		victim = G_Find( nullptr, FOFS(targetname), (char *) name );
		if ( !victim )
		{
			G_DebugPrint( WL_WARNING, "Q3_Remove: can't find %s\n", name );
			return;
		}

		while ( victim )
		{
			Q3_RemoveEnt( victim );
			victim = G_Find( victim, FOFS(targetname), (char *) name );
		}
	}
}

int Q3_GetFloat( int entID, int type, const char *name, float *value )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_GetFloat: invalid entID %d\n", entID);

		return 0;
	}

	gentity_t	*ent = &g_entities[entID];
	int toGet = 0;

	toGet = GetIDForString( setTable, name );	//FIXME: May want to make a "getTable" as well
	//FIXME: I'm getting really sick of these huge switch statements!

	//NOTENOTE: return true if the value was correctly obtained
	switch ( toGet )
	{
	case SET_PARM1:
	case SET_PARM2:
	case SET_PARM3:
	case SET_PARM4:
	case SET_PARM5:
	case SET_PARM6:
	case SET_PARM7:
	case SET_PARM8:
	case SET_PARM9:
	case SET_PARM10:
	case SET_PARM11:
	case SET_PARM12:
	case SET_PARM13:
	case SET_PARM14:
	case SET_PARM15:
	case SET_PARM16:
		if (ent->parms == nullptr)
		{
			G_DebugPrint( WL_ERROR, "GET_PARM: %s %s did not have any parms set!\n", ent->classname, ent->targetname );
			return 0;	// would prefer false, but I'm fitting in with what's here <sigh>
		}
		*value = atof( ent->parms->parm[toGet - SET_PARM1] );
		break;

	case SET_COUNT:
		*value = ent->count;
		break;

	case SET_HEALTH:
		*value = ent->health;
		break;

	case SET_SKILL:
		return 0;
		break;

	case SET_XVELOCITY://## %f="0.0" # Velocity along X axis
		if ( ent->client == nullptr )
		{
			G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_XVELOCITY, %s not a client\n", ent->targetname );
			return 0;
		}
		*value = ent->client->ps.velocity[0];
		break;

	case SET_YVELOCITY://## %f="0.0" # Velocity along Y axis
		if ( ent->client == nullptr )
		{
			G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_YVELOCITY, %s not a client\n", ent->targetname );
			return 0;
		}
		*value = ent->client->ps.velocity[1];
		break;

	case SET_ZVELOCITY://## %f="0.0" # Velocity along Z axis
		if ( ent->client == nullptr )
		{
			G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_ZVELOCITY, %s not a client\n", ent->targetname );
			return 0;
		}
		*value = ent->client->ps.velocity[2];
		break;

	case SET_Z_OFFSET:
		*value = ent->r.currentOrigin[2] - ent->s.origin[2];
		break;
	case SET_WIDTH://## %f="0.0" # Width of NPC bounding box
		*value = ent->r.mins[0];
		break;
	case SET_TIMESCALE://## %f="0.0" # Speed-up slow down game (0 - 1.0)
		return 0;
		break;
	case SET_CAMERA_GROUP_Z_OFS://## %s="NULL" # all ents with this cameraGroup will be focused on
		return 0;
		break;

	case SET_GRAVITY://## %f="0.0" # Change this ent's gravity - 800 default
		*value = g_gravity.value;
		break;

	case SET_FACEEYESCLOSED:
	case SET_FACEEYESOPENED:
	case SET_FACEAUX:		//## %f="0.0" # Set face to Aux expression for number of seconds
	case SET_FACEBLINK:		//## %f="0.0" # Set face to Blink expression for number of seconds
	case SET_FACEBLINKFROWN:	//## %f="0.0" # Set face to Blinkfrown expression for number of seconds
	case SET_FACEFROWN:		//## %f="0.0" # Set face to Frown expression for number of seconds
	case SET_FACENORMAL:		//## %f="0.0" # Set face to Normal expression for number of seconds
		G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_FACE___ not implemented\n" );
		return 0;
		break;
	case SET_WAIT:		//## %f="0.0" # Change an entity's wait field
		*value = ent->wait;
		break;
	//# #sep ints
	case SET_ANIM_HOLDTIME_LOWER://## %d="0" # Hold lower anim for number of milliseconds
		if ( ent->client == nullptr )
		{
			G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_ANIM_HOLDTIME_LOWER, %s not a client\n", ent->targetname );
			return 0;
		}
		*value = ent->client->ps.legsTimer;
		break;
	case SET_ANIM_HOLDTIME_UPPER://## %d="0" # Hold upper anim for number of milliseconds
		if ( ent->client == nullptr )
		{
			G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_ANIM_HOLDTIME_UPPER, %s not a client\n", ent->targetname );
			return 0;
		}
		*value = ent->client->ps.torsoTimer;
		break;
	case SET_ANIM_HOLDTIME_BOTH://## %d="0" # Hold lower and upper anims for number of milliseconds
		G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_ANIM_HOLDTIME_BOTH not implemented\n" );
		return 0;
		break;
	case SET_ARMOR://## %d="0" # Change armor
		if ( ent->client == nullptr )
		{
			G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_ARMOR, %s not a client\n", ent->targetname );
			return 0;
		}
		*value = ent->client->ps.stats[STAT_ARMOR];
		break;
	case SET_FRICTION://## %d="0" # Change ent's friction - 6 default
		return 0;
		break;
	case SET_DELAYSCRIPTTIME://## %d="0" # How many seconds to wait before running delayscript
		return 0;
		break;
	case SET_FORWARDMOVE://## %d="0" # NPC move forward -127(back) to 127
		return 0;
		break;
	case SET_RIGHTMOVE://## %d="0" # NPC move right -127(left) to 127
		return 0;
		break;
	case SET_STARTFRAME:	//## %d="0" # frame to start animation sequence on
		return 0;
		break;
	case SET_ENDFRAME:	//## %d="0" # frame to end animation sequence on
		return 0;
		break;
	case SET_ANIMFRAME:	//## %d="0" # of current frame
		return 0;
		break;
	case SET_MISSIONSTATUSTIME://## %d="0" # Amount of time until Mission Status should be shown after death
		return 0;
		break;
	//# #sep booleans
	case SET_IGNOREENEMIES://## %t="BOOL_TYPES" # Do not acquire enemies
		return 0;
		break;
	case SET_DONTSHOOT://## %t="BOOL_TYPES" # Others won't shoot you
		return 0;
		break;
	case SET_NOTARGET://## %t="BOOL_TYPES" # Others won't pick you as enemy
		*value = (ent->flags&FL_NOTARGET);
		break;
	case SET_LOCKED_ENEMY://## %t="BOOL_TYPES" # Keep current enemy until dead
		return 0;
		break;
	case SET_UNDYING://## %t="BOOL_TYPES" # Can take damage down to 1 but not die
		return 0;
		break;

	case SET_SOLID://## %t="BOOL_TYPES" # Make yourself notsolid or solid
		*value = ent->r.contents;
		break;
	case SET_PLAYER_USABLE://## %t="BOOL_TYPES" # Can be activateby the player's "use" button
		*value = (ent->r.svFlags&SVF_PLAYER_USABLE);
		break;
	case SET_LOOP_ANIM://## %t="BOOL_TYPES" # For non-NPCs: loop your animation sequence
		return 0;
		break;
	case SET_INTERFACE://## %t="BOOL_TYPES" # Player interface on/off
		G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_INTERFACE not implemented\n" );
		return 0;
		break;
	case SET_SHIELDS://## %t="BOOL_TYPES" # NPC has no shields (Borg do not adapt)
		return 0;
		break;
	case SET_INVISIBLE://## %t="BOOL_TYPES" # Makes an NPC not solid and not visible
		*value = (ent->s.eFlags&EF_NODRAW);
		break;
	case SET_VAMPIRE://## %t="BOOL_TYPES" # Makes an NPC not solid and not visible
		return 0;
		break;
	case SET_FORCE_INVINCIBLE://## %t="BOOL_TYPES" # Makes an NPC not solid and not visible
		return 0;
		break;
	case SET_VIDEO_FADE_IN://## %t="BOOL_TYPES" # Makes video playback fade in
		G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_VIDEO_FADE_IN not implemented\n" );
		return 0;
		break;
	case SET_VIDEO_FADE_OUT://## %t="BOOL_TYPES" # Makes video playback fade out
		G_DebugPrint( WL_WARNING, "Q3_GetFloat: SET_VIDEO_FADE_OUT not implemented\n" );
		return 0;
		break;
	case SET_PLAYER_LOCKED://## %t="BOOL_TYPES" # Makes it so player cannot move
		return 0;
		break;
	case SET_LOCK_PLAYER_WEAPONS://## %t="BOOL_TYPES" # Makes it so player cannot switch weapons
		return 0;
		break;
	case SET_NO_IMPACT_DAMAGE://## %t="BOOL_TYPES" # Makes it so player cannot switch weapons
		return 0;
		break;
	case SET_NO_KNOCKBACK://## %t="BOOL_TYPES" # Stops this ent from taking knockback from weapons
		*value = (ent->flags&FL_NO_KNOCKBACK);
		break;
	case SET_INVINCIBLE://## %t="BOOL_TYPES" # Completely unkillable
		*value = (ent->flags&FL_GODMODE);
		break;
	case SET_MISSIONSTATUSACTIVE:	//# Turns on Mission Status Screen
		return 0;
		break;
	case SET_DISMEMBERABLE://## %t="BOOL_TYPES" # NPC will not be affected by force powers
		return 0;
		break;
	case SET_USE_SUBTITLES:
		return 0;
		break;
	case SET_MORELIGHT://## %t="BOOL_TYPES" # NPCs will use their closest combat points, not try and find ones next to the player, or flank player
		return 0;
		break;
	case SET_TREASONED://## %t="BOOL_TYPES" # Player has turned on his own- scripts will stop: NPCs will turn on him and level changes load the brig
		return 0;
		break;
	case SET_DISABLE_SHADER_ANIM:	//## %t="BOOL_TYPES" # Shaders won't animate
		return 0;
		break;
	case SET_SHADER_ANIM:	//## %t="BOOL_TYPES" # Shader will be under frame control
		return 0;
		break;

	default:
		if ( trap->ICARUS_VariableDeclared( name ) != VTYPE_FLOAT )
			return 0;

		return trap->ICARUS_GetFloatVariable( name, value );
	}

	return 1;
}

int Q3_GetVector( int entID, int type, const char *name, vec3_t value )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_GetVector: invalid entID %d\n", entID);

		return 0;
	}

	gentity_t	*ent = &g_entities[entID];
	int toGet = 0;

	toGet = GetIDForString( setTable, name );	//FIXME: May want to make a "getTable" as well
	//FIXME: I'm getting really sick of these huge switch statements!

	//NOTENOTE: return true if the value was correctly obtained
	switch ( toGet )
	{
	case SET_PARM1:
	case SET_PARM2:
	case SET_PARM3:
	case SET_PARM4:
	case SET_PARM5:
	case SET_PARM6:
	case SET_PARM7:
	case SET_PARM8:
	case SET_PARM9:
	case SET_PARM10:
	case SET_PARM11:
	case SET_PARM12:
	case SET_PARM13:
	case SET_PARM14:
	case SET_PARM15:
	case SET_PARM16:
		if ( sscanf( ent->parms->parm[toGet - SET_PARM1], "%f %f %f", &value[0], &value[1], &value[2] ) != 3 ) {
			G_DebugPrint( WL_WARNING, "Q3_GetVector: failed sscanf on SET_PARM%d (%s)\n", toGet, name );
			VectorClear( value );
		}
		break;

	case SET_ORIGIN:
		VectorCopy(ent->r.currentOrigin, value);
		break;

	case SET_ANGLES:
		VectorCopy(ent->r.currentAngles, value);
		break;

	case SET_TELEPORT_DEST://## %v="0.0 0.0 0.0" # Set origin here as soon as the area is clear
		G_DebugPrint( WL_WARNING, "Q3_GetVector: SET_TELEPORT_DEST not implemented\n" );
		return 0;
		break;

	default:

		if ( trap->ICARUS_VariableDeclared( name ) != VTYPE_VECTOR )
			return 0;

		return trap->ICARUS_GetVectorVariable( name, value );
	}

	return 1;
}

int Q3_GetString( int entID, int type, const char *name, char **value )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_GetString: invalid entID %d\n", entID);

		return 0;
	}

	gentity_t	*ent = &g_entities[entID];
	int toGet = 0;

	toGet = GetIDForString( setTable, name );	//FIXME: May want to make a "getTable" as well

	switch ( toGet )
	{
	case SET_ANIM_BOTH:
		*value = (char *) Q3_GetAnimBoth( ent );

		if ( !value || !value[0] )
			return 0;

		break;

	case SET_PARM1:
	case SET_PARM2:
	case SET_PARM3:
	case SET_PARM4:
	case SET_PARM5:
	case SET_PARM6:
	case SET_PARM7:
	case SET_PARM8:
	case SET_PARM9:
	case SET_PARM10:
	case SET_PARM11:
	case SET_PARM12:
	case SET_PARM13:
	case SET_PARM14:
	case SET_PARM15:
	case SET_PARM16:
		if ( ent->parms )
		{
			*value = (char *) ent->parms->parm[toGet - SET_PARM1];
		}
		else
		{
			G_DebugPrint( WL_WARNING, "Q3_GetString: invalid ent %s has no parms!\n", ent->targetname );
			return 0;
		}
		break;

	case SET_TARGET:
		*value = (char *) ent->target;
		break;

	case SET_LOCATION:
		return 0;
		break;

	//# #sep Scripts and other file paths
	case SET_SPAWNSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when spawned //0 - do not change these, these are equal to BSET_SPAWN, etc
		*value = ent->behaviorSet[BSET_SPAWN];
		break;
	case SET_USESCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when used
		*value = ent->behaviorSet[BSET_USE];
		break;
	case SET_AWAKESCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when startled
		*value = ent->behaviorSet[BSET_AWAKE];
		break;
	case SET_ANGERSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script run when find an enemy for the first time
		*value = ent->behaviorSet[BSET_ANGER];
		break;
	case SET_ATTACKSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when you shoot
		*value = ent->behaviorSet[BSET_ATTACK];
		break;
	case SET_VICTORYSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when killed someone
		*value = ent->behaviorSet[BSET_VICTORY];
		break;
	case SET_LOSTENEMYSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when you can't find your enemy
		*value = ent->behaviorSet[BSET_LOSTENEMY];
		break;
	case SET_PAINSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when hit
		*value = ent->behaviorSet[BSET_PAIN];
		break;
	case SET_FLEESCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when hit and low health
		*value = ent->behaviorSet[BSET_FLEE];
		break;
	case SET_DEATHSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when killed
		*value = ent->behaviorSet[BSET_DEATH];
		break;
	case SET_DELAYEDSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run after a delay
		*value = ent->behaviorSet[BSET_DELAYED];
		break;
	case SET_BLOCKEDSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when blocked by teammate
		*value = ent->behaviorSet[BSET_BLOCKED];
		break;
	case SET_FFIRESCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when player has shot own team repeatedly
		*value = ent->behaviorSet[BSET_FFIRE];
		break;
	case SET_FFDEATHSCRIPT://## %s="NULL" !!"W:\game\base\scripts\!!#*.txt" # Script to run when player kills a teammate
		*value = ent->behaviorSet[BSET_FFDEATH];
		break;

	//# #sep Standard strings
	case SET_TARGETNAME://## %s="NULL" # Set/change your targetname
		*value = ent->targetname;
		break;
	case SET_PAINTARGET://## %s="NULL" # Set/change what to use when hit
		return 0;
		break;
	case SET_CAMERA_GROUP://## %s="NULL" # all ents with this cameraGroup will be focused on
		return 0;
		break;
	case SET_CAMERA_GROUP_TAG://## %s="NULL" # all ents with this cameraGroup will be focused on
		return 0;
		break;
	case SET_TARGET2://## %s="NULL" # Set/change your target2: on NPC's: this fires when they're knocked out by the red hypo
		return 0;
		break;

	case SET_ITEM:
		return 0;
		break;
	case SET_MUSIC_STATE:
		return 0;
		break;
	//The below cannot be gotten
	case SET_VIEWENTITY:
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_VIEWENTITY not implemented\n" );
		return 0;
		break;
	case SET_CAPTIONTEXTCOLOR:	//## %s=""  # Color of text RED:WHITE:BLUE: YELLOW
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_CAPTIONTEXTCOLOR not implemented\n" );
		return 0;
		break;
	case SET_CENTERTEXTCOLOR:	//## %s=""  # Color of text RED:WHITE:BLUE: YELLOW
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_CENTERTEXTCOLOR not implemented\n" );
		return 0;
		break;
	case SET_SCROLLTEXTCOLOR:	//## %s=""  # Color of text RED:WHITE:BLUE: YELLOW
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_SCROLLTEXTCOLOR not implemented\n" );
		return 0;
		break;
	case SET_COPY_ORIGIN://## %s="targetname"  # Copy the origin of the ent with targetname to your origin
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_COPY_ORIGIN not implemented\n" );
		return 0;
		break;
	case SET_DEFEND_TARGET://## %s="targetname"  # This NPC will attack the target NPC's enemies
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_COPY_ORIGIN not implemented\n" );
		return 0;
		break;
	case SET_VIDEO_PLAY://## %s="filename" !!"W:\game\base\video\!!#*.roq" # Play a Video (inGame)
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_VIDEO_PLAY not implemented\n" );
		return 0;
		break;
	case SET_LOADGAME://## %s="exitholodeck" # Load the savegame that was auto-saved when you started the holodeck
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_LOADGAME not implemented\n" );
		return 0;
		break;
	case SET_LOCKYAW://## %s="off"  # Lock legs to a certain yaw angle (or "off" or "auto" uses current)
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_LOCKYAW not implemented\n" );
		return 0;
		break;
	case SET_SCROLLTEXT:	//## %s="" # key of text string to print
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_SCROLLTEXT not implemented\n" );
		return 0;
		break;
	case SET_LCARSTEXT:	//## %s="" # key of text string to print in LCARS frame
		G_DebugPrint( WL_WARNING, "Q3_GetString: SET_LCARSTEXT not implemented\n" );
		return 0;
		break;

	case SET_FULLNAME://## %s="NULL" # Set/change your targetname
		*value = ent->fullName;
		break;
	default:

		if ( trap->ICARUS_VariableDeclared( name ) != VTYPE_STRING )
			return 0;

		return trap->ICARUS_GetStringVariable( name, (const char *) *value );
	}

	return 1;
}

void MoveOwner( gentity_t *self )
{
	gentity_t *owner = &g_entities[self->r.ownerNum];

	self->nextthink = level.time + FRAMETIME;
	self->think = G_FreeEntity;

	if ( !owner || !owner->inuse )
	{
		return;
	}

	if ( SpotWouldTelefrag2( owner, self->r.currentOrigin ) )
	{
		self->think = MoveOwner;
	}
	else
	{
		G_SetOrigin( owner, self->r.currentOrigin );
		trap->ICARUS_TaskIDComplete( (sharedEntity_t *)owner, TID_MOVE_NAV );
	}
}

// Copies passed origin to ent running script once there is nothing there blocking the spot
static bool Q3_SetTeleportDest( int entID, vec3_t org )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetTeleportDest: invalid entID %d\n", entID);

		return true;
	}

	gentity_t	*teleEnt = &g_entities[entID];

	if ( teleEnt )
	{
		if ( SpotWouldTelefrag2( teleEnt, org ) )
		{
			gentity_t *teleporter = G_Spawn();

			G_SetOrigin( teleporter, org );
			teleporter->r.ownerNum = teleEnt->s.number;

			teleporter->think = MoveOwner;
			teleporter->nextthink = level.time + FRAMETIME;

			return false;
		}
		else
		{
			G_SetOrigin( teleEnt, org );
		}
	}

	return true;
}

// Sets the origin of an entity directly
static void Q3_SetOrigin( int entID, vec3_t origin )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetOrigin: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];

	trap->UnlinkEntity ((sharedEntity_t *)ent);

	if(ent->client)
	{
		VectorCopy(origin, ent->client->ps.origin);
		VectorCopy(origin, ent->r.currentOrigin);
		ent->client->ps.origin[2] += 1;

		VectorClear (ent->client->ps.velocity);
		ent->client->ps.pm_time = 160;		// hold time
		ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

		ent->client->ps.eFlags ^= EF_TELEPORT_BIT;

//		G_KillBox (ent);
	}
	else
	{
		G_SetOrigin( ent, origin );
	}

	trap->LinkEntity( (sharedEntity_t *)ent );
}

// Copies origin of found ent into ent running script
static void Q3_SetCopyOrigin( int entID, const char *name )
{
	gentity_t	*found = G_Find( nullptr, FOFS(targetname), (char *) name);

	if(found)
	{
		Q3_SetOrigin( entID, found->r.currentOrigin );
		SetClientViewAngle( &g_entities[entID], found->s.angles );
	}
	else
	{
		G_DebugPrint( WL_WARNING, "Q3_SetCopyOrigin: ent %s not found!\n", name);
	}
}

// Set the velocity of an entity directly
static void Q3_SetVelocity( int entID, int axis, float speed )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetVelocity: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*found = &g_entities[entID];

	if(!found->client)
	{
		G_DebugPrint( WL_WARNING, "Q3_SetVelocity: not a client %d\n", entID);
		return;
	}

	//FIXME: add or set?
	found->client->ps.velocity[axis] += speed;

	found->client->ps.pm_time = 500;
	found->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
}

// Sets the angles of an entity directly
static void Q3_SetAngles( int entID, vec3_t angles )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetAngles: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];

	if (ent->client)
	{
		SetClientViewAngle( ent, angles );
	}
	else
	{
		VectorCopy( angles, ent->s.angles );
	}
	trap->LinkEntity( (sharedEntity_t *)ent );
}

// Lerps the origin to the destination value
void Q3_Lerp2Origin( int taskID, int entID, vec3_t origin, float duration )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Lerp2Origin: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];
	moverState_e moverState;

	if ( ent->client || Q_stricmp(ent->classname, "target_scriptrunner") == 0 )
	{
		G_DebugPrint( WL_ERROR, "Q3_Lerp2Origin: ent %d is NOT a mover!\n", entID);
		return;
	}

	if ( ent->s.eType != ET_MOVER )
	{
		ent->s.eType = ET_MOVER;
	}

	moverState = ent->moverState;

	if ( moverState == MOVER_POS1 || moverState == MOVER_2TO1 )
	{
		VectorCopy( ent->r.currentOrigin, ent->pos1 );
		VectorCopy( origin, ent->pos2 );

		moverState = MOVER_1TO2;
	}
	else if ( moverState == MOVER_POS2 || moverState == MOVER_1TO2 )
	{
		VectorCopy( ent->r.currentOrigin, ent->pos2 );
		VectorCopy( origin, ent->pos1 );

		moverState = MOVER_2TO1;
	}

	InitMoverTrData( ent );	//FIXME: This will probably break normal things that are being moved...

	ent->s.pos.trDuration = duration;

	// start it going
	MatchTeam( ent, moverState, level.time );
	//SetMoverState( ent, moverState, level.time );

	ent->reached = moverCallback;
	if ( ent->damage )
	{
		ent->blocked = Blocked_Mover;
	}
	if ( taskID != -1 )
	{
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_MOVE_NAV, taskID );
	}
	// starting sound
	G_PlayDoorLoopSound( ent );//start looping sound
	G_PlayDoorSound( ent, BMS_START );	//play start sound

	trap->LinkEntity( (sharedEntity_t *)ent );
}

static void Q3_SetOriginOffset( int entID, int axis, float offset )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetOriginOffset: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent = &g_entities[entID];
	vec3_t origin;
	float duration;

	if ( ent->client || Q_stricmp(ent->classname, "target_scriptrunner") == 0 )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetOriginOffset: ent %d is NOT a mover!\n", entID);
		return;
	}

	VectorCopy( ent->s.origin, origin );
	origin[axis] += offset;
	duration = 0;
	if ( ent->speed )
	{
		duration = fabs(offset)/fabs(ent->speed)*1000.0f;
	}
	Q3_Lerp2Origin( -1, entID, origin, duration );
}

static void SetLowerAnim( int entID, int animID)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "SetLowerAnim: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !ent->client )
	{
		G_DebugPrint( WL_ERROR, "SetLowerAnim: ent %d is NOT a player or NPC!\n", entID);
		return;
	}

	G_SetAnim(ent,nullptr,SETANIM_LEGS,animID,SETANIM_FLAG_RESTART|SETANIM_FLAG_HOLD|SETANIM_FLAG_OVERRIDE,0);
}

static void SetUpperAnim ( int entID, int animID)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "SetUpperAnim: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !ent->client )
	{
		G_DebugPrint( WL_ERROR, "SetLowerAnim: ent %d is NOT a player or NPC!\n", entID);
		return;
	}

	G_SetAnim(ent,nullptr,SETANIM_TORSO,animID,SETANIM_FLAG_RESTART|SETANIM_FLAG_HOLD|SETANIM_FLAG_OVERRIDE,0);
}

// Sets the upper animation of an entity
static bool Q3_SetAnimUpper( int entID, const char *anim_name )
{
	int			animID = 0;

	animID = GetIDForString( animTable, anim_name );

	if( animID == -1 )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetAnimUpper: unknown animation sequence '%s'\n", anim_name );
		return false;
	}

	/*
	if ( !PM_HasAnimation( SV_GentityNum(entID), animID ) )
	{
		return false;
	}
	*/

	SetUpperAnim( entID, animID );
	return true;
}

// Sets the lower animation of an entity
static bool Q3_SetAnimLower( int entID, const char *anim_name )
{
	int			animID = 0;

	//FIXME: Setting duck anim does not actually duck!

	animID = GetIDForString( animTable, anim_name );

	if( animID == -1 )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetAnimLower: unknown animation sequence '%s'\n", anim_name );
		return false;
	}

	/*
	if ( !PM_HasAnimation( SV_GentityNum(entID), animID ) )
	{
		return false;
	}
	*/

	SetLowerAnim( entID, animID );
	return true;
}

static void Q3_SetAnimHoldTime( int entID, int int_data, bool lower )
{
	G_DebugPrint( WL_WARNING, "Q3_SetAnimHoldTime is not currently supported in MP\n");
	/*
	gentity_t	*ent  = &g_entities[entID];

	if ( !ent )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetAnimHoldTime: invalid entID %d\n", entID);
		return;
	}

	if ( !ent->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetAnimHoldTime: ent %d is NOT a player or NPC!\n", entID);
		return;
	}

	if(lower)
	{
		PM_SetLegsAnimTimer( ent, &ent->client->ps.legsAnimTimer, int_data );
	}
	else
	{
		PM_SetTorsoAnimTimer( ent, &ent->client->ps.torsoAnimTimer, int_data );
	}
	*/
}

static void Q3_SetHealth( int entID, int data )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetHealth: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( data < 0 )
	{
		data = 0;
	}

	ent->health = data;

	if(!ent->client)
	{
		return;
	}

	ent->client->ps.stats[STAT_HEALTH] = data;

	if ( ent->client->ps.stats[STAT_HEALTH] > ent->client->ps.stats[STAT_MAX_HEALTH] )
	{
		ent->health = ent->client->ps.stats[STAT_HEALTH] = ent->client->ps.stats[STAT_MAX_HEALTH];
	}
	if ( data == 0 )
	{
		ent->health = 1;
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
		{ //this would be silly
			return;
		}

		if ( ent->client->tempSpectate >= level.time )
		{ //this would also be silly
			return;
		}

		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
		player_die (ent, ent, ent, 100000, MOD_FALLING);
	}
}

static void Q3_SetArmor( int entID, int data )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetArmor: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if(!ent->client)
	{
		return;
	}

	ent->client->ps.stats[STAT_ARMOR] = data;
	if ( ent->client->ps.stats[STAT_ARMOR] > ent->client->ps.stats[STAT_MAX_HEALTH] )
	{
		ent->client->ps.stats[STAT_ARMOR] = ent->client->ps.stats[STAT_MAX_HEALTH];
	}
}

static void Q3_SetWidth( int entID, int data )
{
	G_DebugPrint( WL_WARNING, "Q3_SetWidth: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetTimeScale( int entID, const char *data )
{
	trap->Cvar_Set("timescale", data);
}

static void Q3_SetInvisible( int entID, bool invisible )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetInvisible: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if ( invisible )
	{
		self->s.eFlags |= EF_NODRAW;
		if ( self->client )
		{
			self->client->ps.eFlags |= EF_NODRAW;
		}
		self->r.contents = 0;
	}
	else
	{
		self->s.eFlags &= ~EF_NODRAW;
		if ( self->client )
		{
			self->client->ps.eFlags &= ~EF_NODRAW;
		}
	}
}

static void Q3_SetVampire( int entID, bool vampire )
{
	G_DebugPrint( WL_WARNING, "Q3_SetVampire: NOT SUPPORTED IN MP\n");
	return;
}

void Q3_SetLoopSound(int entID, const char *name)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetLoopSound: invalid entID %d\n", entID);

		return;
	}

	sfxHandle_t	index;
	gentity_t	*self  = &g_entities[entID];

	if ( Q_stricmp( "NULL", name ) == 0 || Q_stricmp( "NONE", name )==0)
	{
		self->s.loopSound = 0;
		self->s.loopIsSoundset = false;
		return;
	}

	index = G_SoundIndex( (char*)name );

	if (index)
	{
		self->s.loopSound = index;
		self->s.loopIsSoundset = false;
	}
	else
	{
		G_DebugPrint( WL_WARNING, "Q3_SetLoopSound: can't find sound file: '%s'\n", name );
	}
}

void Q3_SetICARUSFreeze( int entID, const char *name, bool freeze )
{
	gentity_t	*self  = G_Find( nullptr, FOFS(targetname), name );
	if ( !self )
	{//hmm, targetname failed, try script_targetname?
		self = G_Find( nullptr, FOFS(script_targetname), name );
	}

	if ( !self )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetICARUSFreeze: invalid ent %s\n", name);
		return;
	}

	if ( freeze )
	{
		self->r.svFlags |= SVF_ICARUS_FREEZE;
	}
	else
	{
		self->r.svFlags &= ~SVF_ICARUS_FREEZE;
	}
}

void Q3_SetViewEntity(int entID, const char *name)
{
	G_DebugPrint( WL_WARNING, "Q3_SetViewEntity currently unsupported in MP, ask if you need it.\n");
}

static void Q3_SetItem (int entID, const char *item_name)
{ //rww - unused in mp
	G_DebugPrint( WL_WARNING, "Q3_SetItem: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetFriction(int entID, int int_data)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetFriction: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if ( !self->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetFriction: '%s' is not an NPC/player!\n", self->targetname );
		return;
	}

	G_DebugPrint( WL_WARNING, "Q3_SetFriction currently unsupported in MP\n");
	//	self->client->ps.friction = int_data;
}

static void Q3_SetGravity(int entID, float float_data)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetGravity: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if ( !self->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetGravity: '%s' is not an NPC/player!\n", self->targetname );
		return;
	}

	self->client->ps.gravity = float_data;
}

static void Q3_SetWait(int entID, float float_data)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetWait: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	self->wait = float_data;
}

static void Q3_SetScale(int entID, float float_data)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetScale: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if (self->client)
	{
		self->client->ps.iModelScale = float_data*100.0f;
	}
	else
	{
		self->s.iModelScale = float_data*100.0f;
	}
}

static float Q3_GameSideCheckStringCounterIncrement( const char *string )
{
	char	*numString;
	float	val = 0.0f;

	if ( string[0] == '+' )
	{//We want to increment whatever the value is by whatever follows the +
		if ( string[1] )
		{
			numString = (char *)&string[1];
			val = atof( numString );
		}
	}
	else if ( string[0] == '-' )
	{//we want to decrement
		if ( string[1] )
		{
			numString = (char *)&string[1];
			val = atof( numString ) * -1;
		}
	}

	return val;
}

static void Q3_SetCount(int entID, const char *data)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetCount: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];
	float		val = 0.0f;

	//FIXME: use FOFS() stuff here to make a generic entity field setting?

	if ( (val = Q3_GameSideCheckStringCounterIncrement( data )) )
	{
		self->count += (int)(val);
	}
	else
	{
		self->count = atoi((char *) data);
	}
}

static void Q3_SetTargetName (int entID, const char *targetname)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetTargetName: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if(!Q_stricmp("NULL", ((char *)targetname)))
	{
		self->targetname = nullptr;
	}
	else
	{
		self->targetname = G_NewString( targetname );
	}
}

static void Q3_SetTarget (int entID, const char *target)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetTarget: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if(!Q_stricmp("NULL", ((char *)target)))
	{
		self->target = nullptr;
	}
	else
	{
		self->target = G_NewString( target );
	}
}

static void Q3_SetTarget2 (int entID, const char *target2)
{
	G_DebugPrint( WL_WARNING, "Q3_SetTarget2 does not exist in MP\n");
	/*
	sharedEntity_t	*self  = SV_GentityNum(entID);

	if ( !self )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetTarget2: invalid entID %d\n", entID);
		return;
	}

	if(!Q_stricmp("NULL", ((char *)target2)))
	{
		self->target2 = nullptr;
	}
	else
	{
		self->target2 = G_NewString( target2 );
	}
	*/
}

static void Q3_SetPainTarget (int entID, const char *targetname)
{
	G_DebugPrint( WL_WARNING, "Q3_SetPainTarget: NOT SUPPORTED IN MP\n");
	/*
	sharedEntity_t	*self  = SV_GentityNum(entID);

	if ( !self )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetPainTarget: invalid entID %d\n", entID);
		return;
	}

	if(Q_stricmp("NULL", ((char *)targetname)) == 0)
	{
		self->paintarget = nullptr;
	}
	else
	{
		self->paintarget = G_NewString((char *)targetname);
	}
	*/
}

static void Q3_SetFullName (int entID, const char *fullName)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetFullName: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];


	if(!Q_stricmp("NULL", ((char *)fullName)))
	{
		self->fullName = nullptr;
	}
	else
	{
		self->fullName = G_NewString( fullName );
	}
}

static void Q3_SetMusicState( const char *dms )
{
	G_DebugPrint( WL_WARNING, "Q3_SetMusicState: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetForcePowerLevel ( int entID, int forcePower, int forceLevel )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetForcePowerLevel: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*self  = &g_entities[entID];

	if ( forcePower < FP_FIRST || forceLevel >= NUM_FORCE_POWERS )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetForcePowerLevel: Force Power index %d out of range (%d-%d)\n", forcePower, FP_FIRST, (NUM_FORCE_POWERS-1) );
		return;
	}

	if ( forceLevel < 0 || forceLevel >= NUM_FORCE_POWER_LEVELS )
	{
		if ( forcePower != FP_SABER_OFFENSE || forceLevel >= SS_NUM_SABER_STYLES )
		{
			G_DebugPrint( WL_ERROR, "Q3_SetForcePowerLevel: Force power setting %d out of range (0-3)\n", forceLevel );
			return;
		}
	}

	if ( !self->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetForcePowerLevel: ent %s is not a player or NPC\n", self->targetname );
		return;
	}

	self->client->ps.fd.forcePowerLevel[forcePower] = forceLevel;
	if ( forceLevel )
	{
		self->client->ps.fd.forcePowersKnown |= ( 1 << forcePower );
	}
	else
	{
		self->client->ps.fd.forcePowersKnown &= ~( 1 << forcePower );
	}
}

void Q3_SetParm (int entID, int parmNum, const char *parmValue)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetParm: invalid entID %d\n", entID);

		return;
	}

	if (parmNum < 0 || parmNum >= MAX_PARMS)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetParm: SET_PARM: parmNum %d out of range!\n", parmNum);
		return;
	}

	gentity_t	*ent = &g_entities[entID];
	float		val;

	if( !ent->parms )
	{
		ent->parms = (parms_t *)G_Alloc( sizeof(parms_t) );
		memset( ent->parms, 0, sizeof(parms_t) );
	}

	if ( (val = Q3_GameSideCheckStringCounterIncrement( parmValue )) )
	{
		val += atof( ent->parms->parm[parmNum] );
		Com_sprintf( ent->parms->parm[parmNum], sizeof(ent->parms->parm[parmNum]), "%f", val );
	}
	else
	{//Just copy the string
		//copy only 16 characters
		Q_strncpyz( ent->parms->parm[parmNum], parmValue, sizeof(ent->parms->parm[parmNum]) );
		/*
		//set the last character to null in case we had to truncate their passed string
		if ( ent->parms->parm[parmNum][sizeof(ent->parms->parm[parmNum]) - 1] != 0 )
		{//Tried to set a string that is too long
			ent->parms->parm[parmNum][sizeof(ent->parms->parm[parmNum]) - 1] = 0;
			G_DebugPrint( WL_WARNING, "SET_PARM: parm%d string too long, truncated to '%s'!\n", parmNum, ent->parms->parm[parmNum] );
		}
		*/
	}
}

static void Q3_SetEvent( int entID, const char *event_name )
{ //rwwFIXMEFIXME: Use in MP?
	G_DebugPrint( WL_WARNING, "Q3_SetEvent: NOT SUPPORTED IN MP (may be in future, ask if needed)\n");
	return;
}

static void Q3_SetIgnoreEnemies( int entID, bool data)
{
	G_DebugPrint( WL_WARNING, "Q3_SetIgnoreEnemies: NOT SUPPORTED IN MP");
	return;
}

static void Q3_SetNoTarget( int entID, bool data)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetNoTarget: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if(data)
		ent->flags |= FL_NOTARGET;
	else
		ent->flags &= ~FL_NOTARGET;
}

static void Q3_SetDontShoot( int entID, bool add)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetDontShoot: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if(add)
	{
		ent->flags |= FL_DONT_SHOOT;
	}
	else
	{
		ent->flags &= ~FL_DONT_SHOOT;
	}
}

static void Q3_SetInactive(int entID, bool add)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetDontShoot: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if(add)
	{
		ent->flags |= FL_INACTIVE;
	}
	else
	{
		ent->flags &= ~FL_INACTIVE;
	}
}

static void Q3_SetFuncUsableVisible(int entID, bool visible )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetDontShoot: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	// Yeah, I know that this doesn't even do half of what the func_usable use code does, but if I've got two things on top of each other...and only
	//	one is visible at a time....and neither can ever be used......and finally, the shader on it has the shader_anim stuff going on....It doesn't seem
	//	like I can easily use the other version without nasty side effects.
	if( visible )
	{
		ent->r.svFlags &= ~SVF_NOCLIENT;
		ent->s.eFlags &= ~EF_NODRAW;
	}
	else
	{
		ent->r.svFlags |= SVF_NOCLIENT;
		ent->s.eFlags |= EF_NODRAW;
	}
}

static void Q3_SetLockedEnemy ( int entID, bool locked)
{
	G_DebugPrint( WL_WARNING, "Q3_SetLockedEnemy: NOT SUPPORTED IN MP\n");
	return;
}

char cinematicSkipScript[1024];

static void Q3_SetCinematicSkipScript( char *scriptname )
{
	G_DebugPrint( WL_WARNING, "Q3_SetCinematicSkipScript: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetNoMindTrick( int entID, bool add)
{
	G_DebugPrint( WL_WARNING, "Q3_SetNoMindTrick: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetUseSubtitles( int entID, bool add)
{
	G_DebugPrint( WL_WARNING, "Q3_SetUseSubtitles: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetDismemberable( int entID, bool dismemberable)
{
	G_DebugPrint( WL_WARNING, "Q3_SetDismemberable: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetMoreLight( int entID, bool add )
{
	G_DebugPrint( WL_WARNING, "Q3_SetMoreLight: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetUndying( int entID, bool undying)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetUndying: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if(undying)
	{
		ent->flags |= FL_UNDYING;
	}
	else
	{
		ent->flags &= ~FL_UNDYING;
	}
}

static void Q3_SetInvincible( int entID, bool invincible)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetInvincible: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !Q_stricmp( "func_breakable", ent->classname ) )
	{
		if ( invincible )
		{
			ent->spawnflags |= 1;
		}
		else
		{
			ent->spawnflags &= ~1;
		}
		return;
	}

	if ( invincible )
	{
		ent->flags |= FL_GODMODE;
	}
	else
	{
		ent->flags &= ~FL_GODMODE;
	}
}

static void Q3_SetForceInvincible( int entID, bool forceInv )
{
	G_DebugPrint( WL_WARNING, "Q3_SetForceInvicible: NOT SUPPORTED IN MP\n");
	return;
}

void SolidifyOwner( gentity_t *self )
{
	int oldContents;
	gentity_t *owner = &g_entities[self->r.ownerNum];

	self->nextthink = level.time + FRAMETIME;
	self->think = G_FreeEntity;

	if ( !owner || !owner->inuse )
	{
		return;
	}

	oldContents = owner->r.contents;
	owner->r.contents = CONTENTS_BODY;
	if ( SpotWouldTelefrag2( owner, owner->r.currentOrigin ) )
	{
		owner->r.contents = oldContents;
		self->think = SolidifyOwner;
	}
	else
	{
		trap->ICARUS_TaskIDComplete( (sharedEntity_t *)owner, TID_RESIZE );
	}
}

static bool Q3_SetSolid( int entID, bool solid)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetSolid: invalid entID %d\n", entID);

		return true;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !ent || !ent->inuse )
	{
		G_DebugPrint( WL_WARNING, "Q3_SetSolid: invalid entID %d\n", entID);
		return true;
	}

	if ( solid )
	{//FIXME: Presumption
		int oldContents = ent->r.contents;
		ent->r.contents = CONTENTS_BODY;
		if ( SpotWouldTelefrag2( ent, ent->r.currentOrigin ) )
		{
			gentity_t *solidifier = G_Spawn();

			solidifier->r.ownerNum = ent->s.number;

			solidifier->think = SolidifyOwner;
			solidifier->nextthink = level.time + FRAMETIME;

			ent->r.contents = oldContents;
			return false;
		}
		ent->clipmask |= CONTENTS_BODY;
	}
	else
	{//FIXME: Presumption
		if ( ent->s.eFlags & EF_NODRAW )
		{//We're invisible too, so set contents to none
			ent->r.contents = 0;
		}
		else
		{
			ent->r.contents = CONTENTS_CORPSE;
		}
	}
	return true;
}

static void Q3_SetForwardMove( int entID, int fmoveVal)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetForwardMove: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !ent->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetForwardMove: '%s' is not an NPC/player!\n", ent->targetname );
		return;
	}

	G_DebugPrint( WL_WARNING, "Q3_SetForwardMove: NOT SUPPORTED IN MP\n");
	//ent->client->forced_forwardmove = fmoveVal;
}

static void Q3_SetRightMove( int entID, int rmoveVal)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetRightMove: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !ent->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetRightMove: '%s' is not an NPC/player!\n", ent->targetname );
		return;
	}

	G_DebugPrint( WL_WARNING, "Q3_SetRightMove: NOT SUPPORTED IN MP\n");
	//ent->client->forced_rightmove = rmoveVal;
}

static void Q3_SetLockAngle( int entID, const char *lockAngle)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetLockAngle: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( !ent->client )
	{
		G_DebugPrint( WL_ERROR, "Q3_SetLockAngle: '%s' is not an NPC/player!\n", ent->targetname );
		return;
	}

	G_DebugPrint( WL_WARNING, "Q3_SetLockAngle is not currently available. Ask if you really need it.\n");
}

static void Q3_CameraGroup( int entID, char *camG)
{
	G_DebugPrint( WL_WARNING, "Q3_CameraGroup: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_CameraGroupZOfs( float camGZOfs )
{
	G_DebugPrint( WL_WARNING, "Q3_CameraGroupZOfs: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_CameraGroupTag( char *camGTag )
{
	G_DebugPrint( WL_WARNING, "Q3_CameraGroupTag: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_RemoveRHandModel( int entID, char *addModel)
{
	G_DebugPrint( WL_WARNING, "Q3_RemoveRHandModel: NOT SUPPORTED IN MP\n");
}

static void Q3_AddRHandModel( int entID, char *addModel)
{
	G_DebugPrint( WL_WARNING, "Q3_AddRHandModel: NOT SUPPORTED IN MP\n");
}

static void Q3_AddLHandModel( int entID, char *addModel)
{
	G_DebugPrint( WL_WARNING, "Q3_AddLHandModel: NOT SUPPORTED IN MP\n");
}

static void Q3_RemoveLHandModel( int entID, char *addModel)
{
	G_DebugPrint( WL_WARNING, "Q3_RemoveLHandModel: NOT SUPPORTED IN MP\n");
}

static void Q3_Face( int entID,int expression, float holdtime)
{
	G_DebugPrint( WL_WARNING, "Q3_Face: NOT SUPPORTED IN MP\n");
}

static bool Q3_SetLocation( int entID, const char *location )
{
	G_DebugPrint( WL_WARNING, "Q3_SetLocation: NOT SUPPORTED IN MP\n");
	return true;
}

bool	player_locked = false;
static void Q3_SetPlayerLocked( int entID, bool locked )
{
	G_DebugPrint( WL_WARNING, "Q3_SetPlayerLocked: NOT SUPPORTED IN MP\n");
}

static void Q3_SetLockPlayerWeapons( int entID, bool locked )
{
	G_DebugPrint( WL_WARNING, "Q3_SetLockPlayerWeapons: NOT SUPPORTED IN MP\n");
}

static void Q3_SetNoImpactDamage( int entID, bool noImp )
{
	G_DebugPrint( WL_WARNING, "Q3_SetNoImpactDamage: NOT SUPPORTED IN MP\n");
}

static bool Q3_SetBehaviorSet( int entID, int toSet, const char *scriptname)
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetBehaviorSet: invalid entID %d\n", entID);

		return false;
	}

	gentity_t	*ent  = &g_entities[entID];
	bSet_e		bSet = BSET_INVALID;

	switch(toSet)
	{
	case SET_SPAWNSCRIPT:
		bSet = BSET_SPAWN;
		break;
	case SET_USESCRIPT:
		bSet = BSET_USE;
		break;
	case SET_AWAKESCRIPT:
		bSet = BSET_AWAKE;
		break;
	case SET_ANGERSCRIPT:
		bSet = BSET_ANGER;
		break;
	case SET_ATTACKSCRIPT:
		bSet = BSET_ATTACK;
		break;
	case SET_VICTORYSCRIPT:
		bSet = BSET_VICTORY;
		break;
	case SET_LOSTENEMYSCRIPT:
		bSet = BSET_LOSTENEMY;
		break;
	case SET_PAINSCRIPT:
		bSet = BSET_PAIN;
		break;
	case SET_FLEESCRIPT:
		bSet = BSET_FLEE;
		break;
	case SET_DEATHSCRIPT:
		bSet = BSET_DEATH;
		break;
	case SET_DELAYEDSCRIPT:
		bSet = BSET_DELAYED;
		break;
	case SET_BLOCKEDSCRIPT:
		bSet = BSET_BLOCKED;
		break;
	case SET_FFIRESCRIPT:
		bSet = BSET_FFIRE;
		break;
	case SET_FFDEATHSCRIPT:
		bSet = BSET_FFDEATH;
		break;
	case SET_MINDTRICKSCRIPT:
		bSet = BSET_MINDTRICK;
		break;
	}

	if(bSet < BSET_SPAWN || bSet >= NUM_BSETS)
	{
		return false;
	}

	if(!Q_stricmp("NULL", scriptname))
	{
		if ( ent->behaviorSet[bSet] != nullptr )
		{
//			trap->TagFree( ent->behaviorSet[bSet] );
		}

		ent->behaviorSet[bSet] = nullptr;
		//memset( &ent->behaviorSet[bSet], 0, sizeof(ent->behaviorSet[bSet]) );
	}
	else
	{
		if ( scriptname )
		{
			if ( ent->behaviorSet[bSet] != nullptr )
			{
//				trap->TagFree( ent->behaviorSet[bSet] );
			}

			ent->behaviorSet[bSet] = G_NewString( (char *) scriptname );	//FIXME: This really isn't good...
		}

		//ent->behaviorSet[bSet] = scriptname;
		//strncpy( (char *) &ent->behaviorSet[bSet], scriptname, MAX_BSET_LENGTH );
	}
	return true;
}

static void Q3_SetDelayScriptTime(int entID, int delayTime)
{
	G_DebugPrint( WL_WARNING, "Q3_SetDelayScriptTime: NOT SUPPORTED IN MP\n");
}

static void Q3_SetPlayerUsable( int entID, bool usable )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetPlayerUsable: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if(usable)
	{
		ent->r.svFlags |= SVF_PLAYER_USABLE;
	}
	else
	{
		ent->r.svFlags &= ~SVF_PLAYER_USABLE;
	}
}

static void Q3_SetDisableShaderAnims( int entID, int disabled )
{
	G_DebugPrint( WL_WARNING, "Q3_SetDisableShaderAnims: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetShaderAnim( int entID, int disabled )
{
	G_DebugPrint( WL_WARNING, "Q3_SetShaderAnim: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetStartFrame( int entID, int startFrame )
{
	G_DebugPrint( WL_WARNING, "Q3_SetStartFrame: NOT SUPPORTED IN MP\n");
}

static void Q3_SetEndFrame( int entID, int endFrame )
{
	G_DebugPrint( WL_WARNING, "Q3_SetEndFrame: NOT SUPPORTED IN MP\n");
}

static void Q3_SetAnimFrame( int entID, int animFrame )
{
	G_DebugPrint( WL_WARNING, "Q3_SetAnimFrame: NOT SUPPORTED IN MP\n");
}

static void Q3_SetLoopAnim( int entID, bool loopAnim )
{
	G_DebugPrint( WL_WARNING, "Q3_SetLoopAnim: NOT SUPPORTED IN MP\n");
}

static void Q3_SetShields( int entID, bool shields )
{
	G_DebugPrint( WL_WARNING, "Q3_SetShields: NOT SUPPORTED IN MP\n");
	return;
}

static void Q3_SetSaberActive( int entID, bool active )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetSaberActive: invalid entID %d\n", entID);

		return;
	}

	gentity_t *ent = &g_entities[entID];

	if (!ent->inuse)
	{
		return;
	}

	if (!ent->client)
	{
		G_DebugPrint( WL_WARNING, "Q3_SetSaberActive: %d is not a client\n", entID);

		return;
	}

	//fixme: Take into account player being in state where saber won't toggle? For now we simply won't care.
	if (!ent->client->ps.saberHolstered && active)
	{
		Cmd_ToggleSaber_f(ent);
	}
	else if (BG_SabersOff( &ent->client->ps ) && !active)
	{
		Cmd_ToggleSaber_f(ent);
	}
}

static void Q3_SetNoKnockback( int entID, bool noKnockback )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_SetNoKnockback: invalid entID %d\n", entID);

		return;
	}

	gentity_t	*ent  = &g_entities[entID];

	if ( noKnockback )
	{
		ent->flags |= FL_NO_KNOCKBACK;
	}
	else
	{
		ent->flags &= ~FL_NO_KNOCKBACK;
	}
}

static void Q3_SetCleanDamagingEnts( void )
{
	G_DebugPrint( WL_WARNING, "Q3_SetCleanDamagingEnts: NOT SUPPORTED IN MP\n");
	return;
}

vec4_t textcolor_caption;
vec4_t textcolor_center;
vec4_t textcolor_scroll;

static void Q3_SetTextColor ( vec4_t textcolor,const char *color)
{
	G_DebugPrint( WL_WARNING, "Q3_SetTextColor: NOT SUPPORTED IN MP\n");
	return;
}

// Change color text prints in
static void Q3_SetCaptionTextColor ( const char *color)
{
	Q3_SetTextColor(textcolor_caption,color);
}

// Change color text prints in
static void Q3_SetCenterTextColor ( const char *color)
{
	Q3_SetTextColor(textcolor_center,color);
}

// Change color text prints in
static void Q3_SetScrollTextColor ( const char *color)
{
	Q3_SetTextColor(textcolor_scroll,color);
}

// Prints a message in the center of the screen
static void Q3_ScrollText ( const char *id)
{
	G_DebugPrint( WL_WARNING, "Q3_ScrollText: NOT SUPPORTED IN MP\n");
	//trap->SendServerCommand( -1, va("st \"%s\"", id));

	return;
}

// Prints a message in the center of the screen giving it an LCARS frame around it
static void Q3_LCARSText ( const char *id)
{
	G_DebugPrint( WL_WARNING, "Q3_ScrollText: NOT SUPPORTED IN MP\n");
	//trap->SendServerCommand( -1, va("lt \"%s\"", id));

	return;
}

//returns true if it got to the end, otherwise false.
bool Q3_Set( int taskID, int entID, const char *type_name, const char *data )
{
	if (entID < 0 || entID >= MAX_GENTITIES)
	{
		G_DebugPrint(WL_WARNING, "Q3_Set: invalid entID %d\n", entID);

		return true;
	}

	gentity_t	*ent = &g_entities[entID];
	float		float_data;
	int			int_data, toSet;
	vec3_t		vector_data;

	//Set this for callbacks
	toSet = GetIDForString( setTable, type_name );

	//TODO: Throw in a showscript command that will list each command and what they're doing...
	//		maybe as simple as printing that line of the script to the console preceeded by the person's name?
	//		showscript can take any number of targetnames or "all"?  Groupname?
	switch ( toSet )
	{
	case SET_ORIGIN:
		if ( sscanf( data, "%f %f %f", &vector_data[0], &vector_data[1], &vector_data[2] ) != 3 ) {
			G_DebugPrint( WL_WARNING, "Q3_Set: failed sscanf on SET_ORIGIN (%s)\n", type_name );
			VectorClear( vector_data );
		}
		G_SetOrigin( ent, vector_data );
		break;

	case SET_TELEPORT_DEST:
		if ( sscanf( data, "%f %f %f", &vector_data[0], &vector_data[1], &vector_data[2] ) != 3 ) {
			G_DebugPrint( WL_WARNING, "Q3_Set: failed sscanf on SET_TELEPORT_DEST (%s)\n", type_name );
			VectorClear( vector_data );
		}
		if ( !Q3_SetTeleportDest( entID, vector_data ) )
		{
			trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_MOVE_NAV, taskID );
			return false;
		}
		break;

	case SET_COPY_ORIGIN:
		Q3_SetCopyOrigin( entID, (char *) data );
		break;

	case SET_ANGLES:
		//Q3_SetAngles( entID, *(vec3_t *) data);
		if ( sscanf( data, "%f %f %f", &vector_data[0], &vector_data[1], &vector_data[2] ) != 3 ) {
			G_DebugPrint( WL_WARNING, "Q3_Set: failed sscanf on SET_ANGLES (%s)\n", type_name );
			VectorClear( vector_data );
		}
		Q3_SetAngles( entID, vector_data);
		break;

	case SET_XVELOCITY:
		float_data = atof((char *) data);
		Q3_SetVelocity( entID, 0, float_data);
		break;

	case SET_YVELOCITY:
		float_data = atof((char *) data);
		Q3_SetVelocity( entID, 1, float_data);
		break;

	case SET_ZVELOCITY:
		float_data = atof((char *) data);
		Q3_SetVelocity( entID, 2, float_data);
		break;

	case SET_Z_OFFSET:
		float_data = atof((char *) data);
		Q3_SetOriginOffset( entID, 2, float_data);
		break;

	case SET_ANIM_UPPER:
		if ( Q3_SetAnimUpper( entID, (char *) data ) )
		{
			Q3_TaskIDClear( &ent->taskID[TID_ANIM_BOTH] );//We only want to wait for the top
			trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_UPPER, taskID );
			return false;	//Don't call it back
		}
		break;

	case SET_ANIM_LOWER:
		if ( Q3_SetAnimLower( entID, (char *) data ) )
		{
			Q3_TaskIDClear( &ent->taskID[TID_ANIM_BOTH] );//We only want to wait for the bottom
			trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_LOWER, taskID );
			return false;	//Don't call it back
		}
		break;

	case SET_ANIM_BOTH:
		{
			int	both = 0;
			if ( Q3_SetAnimUpper( entID, (char *) data ) )
			{
				trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_UPPER, taskID );
				both++;
			}
			else
			{
				G_DebugPrint( WL_ERROR, "Q3_SetAnimUpper: %s does not have anim %s!\n", ent->targetname, (char *)data );
			}
			if ( Q3_SetAnimLower( entID, (char *) data ) )
			{
				trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_LOWER, taskID );
				both++;
			}
			else
			{
				G_DebugPrint( WL_ERROR, "Q3_SetAnimLower: %s does not have anim %s!\n", ent->targetname, (char *)data );
			}
			if ( both >= 2 )
			{
				trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_BOTH, taskID );
			}
			if ( both )
			{
				return false;	//Don't call it back
			}
		}
		break;

	case SET_ANIM_HOLDTIME_LOWER:
		int_data = atoi((char *) data);
		Q3_SetAnimHoldTime( entID, int_data, true );
		Q3_TaskIDClear( &ent->taskID[TID_ANIM_BOTH] );//We only want to wait for the bottom
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_LOWER, taskID );
		return false;	//Don't call it back
		break;

	case SET_ANIM_HOLDTIME_UPPER:
		int_data = atoi((char *) data);
		Q3_SetAnimHoldTime( entID, int_data, false );
		Q3_TaskIDClear( &ent->taskID[TID_ANIM_BOTH] );//We only want to wait for the top
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_UPPER, taskID );
		return false;	//Don't call it back
		break;

	case SET_ANIM_HOLDTIME_BOTH:
		int_data = atoi((char *) data);
		Q3_SetAnimHoldTime( entID, int_data, false );
		Q3_SetAnimHoldTime( entID, int_data, true );
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_BOTH, taskID );
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_UPPER, taskID );
		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_LOWER, taskID );
		return false;	//Don't call it back
		break;

	case SET_HEALTH:
		int_data = atoi((char *) data);
		Q3_SetHealth( entID, int_data );
		break;

	case SET_ARMOR:
		int_data = atoi((char *) data);
		Q3_SetArmor( entID, int_data );
		break;

	case SET_EVENT:
		Q3_SetEvent( entID, (char *) data );
		break;

	case SET_VIEWENTITY:
		Q3_SetViewEntity( entID, (char *) data );
		break;

	case SET_LOOPSOUND:
		Q3_SetLoopSound( entID, (char *) data );
		break;

	case SET_ICARUS_FREEZE:
	case SET_ICARUS_UNFREEZE:
		Q3_SetICARUSFreeze( entID, (char *) data, (bool)(toSet==SET_ICARUS_FREEZE) );
		break;

	case SET_ITEM:
		Q3_SetItem ( entID, (char *) data);
		break;

	case SET_WIDTH:
		int_data = atoi((char *) data);
		Q3_SetWidth( entID, int_data );
		return false;
		break;

	case SET_FRICTION:
		int_data = atoi((char *) data);
		Q3_SetFriction ( entID, int_data);
		break;

	case SET_GRAVITY:
		float_data = atof((char *) data);
		Q3_SetGravity ( entID, float_data);
		break;

	case SET_WAIT:
		float_data = atof((char *) data);
		Q3_SetWait( entID, float_data);
		break;

	case SET_SCALE:
		float_data = atof((char *) data);
		Q3_SetScale( entID, float_data);
		break;

	case SET_COUNT:
		Q3_SetCount( entID, (char *) data);
		break;

	case SET_IGNOREENEMIES:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetIgnoreEnemies( entID, true);
		else if(!Q_stricmp("false", ((char *)data)))
			Q3_SetIgnoreEnemies( entID, false);
		break;

	case SET_DONTSHOOT:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetDontShoot( entID, true);
		else if(!Q_stricmp("false", ((char *)data)))
			Q3_SetDontShoot( entID, false);
		break;

	case SET_LOCKED_ENEMY:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetLockedEnemy( entID, true);
		else if(!Q_stricmp("false", ((char *)data)))
			Q3_SetLockedEnemy( entID, false);
		break;

	case SET_NOTARGET:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetNoTarget( entID, true);
		else if(!Q_stricmp("false", ((char *)data)))
			Q3_SetNoTarget( entID, false);
		break;

	case SET_LEAN:
		G_DebugPrint( WL_WARNING, "SET_LEAN NOT SUPPORTED IN MP\n" );
		break;

	case SET_TIMESCALE:
		Q3_SetTimeScale( entID, (char *) data );
		break;

	case SET_TARGETNAME:
		Q3_SetTargetName( entID, (char *) data );
		break;

	case SET_TARGET:
		Q3_SetTarget( entID, (char *) data );
		break;

	case SET_TARGET2:
		Q3_SetTarget2( entID, (char *) data );
		break;

	case SET_LOCATION:
		if ( !Q3_SetLocation( entID, (char *) data ) )
		{
			trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_LOCATION, taskID );
			return false;
		}
		break;

	case SET_PAINTARGET:
		Q3_SetPainTarget( entID, (char *) data );
		break;

	case SET_DEFEND_TARGET:
		G_DebugPrint( WL_WARNING, "Q3_SetDefendTarget unimplemented\n", entID );
		//Q3_SetEnemy( entID, (char *) data);
		break;

	case SET_PARM1:
	case SET_PARM2:
	case SET_PARM3:
	case SET_PARM4:
	case SET_PARM5:
	case SET_PARM6:
	case SET_PARM7:
	case SET_PARM8:
	case SET_PARM9:
	case SET_PARM10:
	case SET_PARM11:
	case SET_PARM12:
	case SET_PARM13:
	case SET_PARM14:
	case SET_PARM15:
	case SET_PARM16:
		Q3_SetParm( entID, (toSet-SET_PARM1), (char *) data );
		break;

	case SET_SPAWNSCRIPT:
	case SET_USESCRIPT:
	case SET_AWAKESCRIPT:
	case SET_ANGERSCRIPT:
	case SET_ATTACKSCRIPT:
	case SET_VICTORYSCRIPT:
	case SET_PAINSCRIPT:
	case SET_FLEESCRIPT:
	case SET_DEATHSCRIPT:
	case SET_DELAYEDSCRIPT:
	case SET_BLOCKEDSCRIPT:
	case SET_FFIRESCRIPT:
	case SET_FFDEATHSCRIPT:
	case SET_MINDTRICKSCRIPT:
		if( !Q3_SetBehaviorSet(entID, toSet, (char *) data) )
			G_DebugPrint( WL_ERROR, "Q3_SetBehaviorSet: Invalid bSet %s\n", type_name );
		break;

	case SET_NO_MINDTRICK:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetNoMindTrick( entID, true);
		else
			Q3_SetNoMindTrick( entID, false);
		break;

	case SET_CINEMATIC_SKIPSCRIPT :
		Q3_SetCinematicSkipScript((char *) data);
		break;

	case SET_DELAYSCRIPTTIME:
		int_data = atoi((char *) data);
		Q3_SetDelayScriptTime( entID, int_data );
		break;

	case SET_USE_SUBTITLES:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetUseSubtitles( entID, true);
		else
			Q3_SetUseSubtitles( entID, false);
		break;

	case SET_DISMEMBERABLE:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetDismemberable( entID, true);
		else
			Q3_SetDismemberable( entID, false);
		break;

	case SET_MORELIGHT:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetMoreLight( entID, true);
		else
			Q3_SetMoreLight( entID, false);
		break;

	case SET_TREASONED:
		G_DebugPrint( WL_VERBOSE, "SET_TREASONED is disabled, do not use\n" );
		/*
		G_TeamRetaliation( nullptr, SV_GentityNum(0), false );
		ffireLevel = FFIRE_LEVEL_RETALIATION;
		*/
		break;

	case SET_UNDYING:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetUndying( entID, true);
		else
			Q3_SetUndying( entID, false);
		break;

	case SET_INVINCIBLE:
		if(!Q_stricmp("true", ((char *)data)))
			Q3_SetInvincible( entID, true);
		else
			Q3_SetInvincible( entID, false);
		break;

	case SET_SOLID:
		if(!Q_stricmp("true", ((char *)data)))
		{
			if ( !Q3_SetSolid( entID, true) )
			{
				trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_RESIZE, taskID );
				return false;
			}
		}
		else
		{
			Q3_SetSolid( entID, false);
		}
		break;

	case SET_INVISIBLE:
		if( !Q_stricmp("true", ((char *)data)) )
			Q3_SetInvisible( entID, true );
		else
			Q3_SetInvisible( entID, false );
		break;

	case SET_VAMPIRE:
		if( !Q_stricmp("true", ((char *)data)) )
			Q3_SetVampire( entID, true );
		else
			Q3_SetVampire( entID, false );
		break;

	case SET_FORCE_INVINCIBLE:
		if( !Q_stricmp("true", ((char *)data)) )
			Q3_SetForceInvincible( entID, true );
		else
			Q3_SetForceInvincible( entID, false );
		break;

	case SET_PLAYER_LOCKED:
		if( !Q_stricmp("true", ((char *)data)) )
			Q3_SetPlayerLocked( entID, true );
		else
			Q3_SetPlayerLocked( entID, false );
		break;

	case SET_LOCK_PLAYER_WEAPONS:
		if( !Q_stricmp("true", ((char *)data)) )
			Q3_SetLockPlayerWeapons( entID, true );
		else
			Q3_SetLockPlayerWeapons( entID, false );
		break;

	case SET_NO_IMPACT_DAMAGE:
		if( !Q_stricmp("true", ((char *)data)) )
			Q3_SetNoImpactDamage( entID, true );
		else
			Q3_SetNoImpactDamage( entID, false );
		break;

	case SET_FORWARDMOVE:
		int_data = atoi((char *) data);
		Q3_SetForwardMove( entID, int_data);
		break;

	case SET_RIGHTMOVE:
		int_data = atoi((char *) data);
		Q3_SetRightMove( entID, int_data);
		break;

	case SET_LOCKYAW:
		Q3_SetLockAngle( entID, data);
		break;

	case SET_CAMERA_GROUP:
		Q3_CameraGroup(entID, (char *)data);
		break;
	case SET_CAMERA_GROUP_Z_OFS:
		float_data = atof((char *) data);
		Q3_CameraGroupZOfs( float_data );
		break;
	case SET_CAMERA_GROUP_TAG:
		Q3_CameraGroupTag( (char *)data );
		break;

	case SET_ADDRHANDBOLT_MODEL:
		Q3_AddRHandModel(entID, (char *)data);
		break;

	case SET_REMOVERHANDBOLT_MODEL:
		Q3_RemoveRHandModel(entID, (char *)data);
		break;

	case SET_ADDLHANDBOLT_MODEL:
		Q3_AddLHandModel(entID, (char *)data);
		break;

	case SET_REMOVELHANDBOLT_MODEL:
		Q3_RemoveLHandModel(entID, (char *)data);
		break;

	case SET_FACEEYESCLOSED:
	case SET_FACEEYESOPENED:
	case SET_FACEAUX:
	case SET_FACEBLINK:
	case SET_FACEBLINKFROWN:
	case SET_FACEFROWN:
	case SET_FACENORMAL:
		float_data = atof((char *) data);
		Q3_Face(entID, toSet, float_data);
		break;

	case SET_SCROLLTEXT:
		Q3_ScrollText( (char *)data );
		break;

	case SET_LCARSTEXT:
		Q3_LCARSText( (char *)data );
		break;

	case SET_CAPTIONTEXTCOLOR:
		Q3_SetCaptionTextColor ( (char *)data );
		break;
	case SET_CENTERTEXTCOLOR:
		Q3_SetCenterTextColor ( (char *)data );
		break;
	case SET_SCROLLTEXTCOLOR:
		Q3_SetScrollTextColor ( (char *)data );
		break;

	case SET_PLAYER_USABLE:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetPlayerUsable(entID, true);
		}
		else
		{
			Q3_SetPlayerUsable(entID, false);
		}
		break;

	case SET_STARTFRAME:
		int_data = atoi((char *) data);
		Q3_SetStartFrame(entID, int_data);
		break;

	case SET_ENDFRAME:
		int_data = atoi((char *) data);
		Q3_SetEndFrame(entID, int_data);

		trap->ICARUS_TaskIDSet( (sharedEntity_t *)ent, TID_ANIM_BOTH, taskID );
		return false;
		break;

	case SET_ANIMFRAME:
		int_data = atoi((char *) data);
		Q3_SetAnimFrame(entID, int_data);
		return false;
		break;

	case SET_LOOP_ANIM:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetLoopAnim(entID, true);
		}
		else
		{
			Q3_SetLoopAnim(entID, false);
		}
		break;

	case SET_INTERFACE:
		G_DebugPrint( WL_WARNING, "Q3_SetInterface: NOT SUPPORTED IN MP\n");

		break;

	case SET_SHIELDS:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetShields(entID, true);
		}
		else
		{
			Q3_SetShields(entID, false);
		}
		break;

	case SET_SABERACTIVE:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetSaberActive( entID, true );
		}
		else
		{
			Q3_SetSaberActive( entID, false );
		}
		break;

	case SET_ADJUST_AREA_PORTALS:
		G_DebugPrint( WL_WARNING, "Q3_SetAdjustAreaPortals: NOT SUPPORTED IN MP\n");
		break;

	case SET_DMG_BY_HEAVY_WEAP_ONLY:
		G_DebugPrint( WL_WARNING, "Q3_SetDmgByHeavyWeapOnly: NOT SUPPORTED IN MP\n");
		break;

	case SET_SHIELDED:
		G_DebugPrint( WL_WARNING, "Q3_SetShielded: NOT SUPPORTED IN MP\n");
		break;

	case SET_NO_GROUPS:
		G_DebugPrint( WL_WARNING, "Q3_SetNoGroups: NOT SUPPORTED IN MP\n");
		break;

	case SET_INACTIVE:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetInactive( entID, true);
		}
		else if(!Q_stricmp("false", ((char *)data)))
		{
			Q3_SetInactive( entID, false);
		}
		else if(!Q_stricmp("unlocked", ((char *)data)))
		{
			UnLockDoors(&g_entities[entID]);
		}
		else if(!Q_stricmp("locked", ((char *)data)))
		{
			LockDoors(&g_entities[entID]);
		}
		break;
	case SET_END_SCREENDISSOLVE:
		G_DebugPrint( WL_WARNING, "SET_END_SCREENDISSOLVE: NOT SUPPORTED IN MP\n");
		break;

	case SET_MISSION_STATUS_SCREEN:
		//Cvar_Set("cg_missionstatusscreen", "1");
		G_DebugPrint( WL_WARNING, "SET_MISSION_STATUS_SCREEN: NOT SUPPORTED IN MP\n");
		break;

	case SET_FUNC_USABLE_VISIBLE:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetFuncUsableVisible( entID, true);
		}
		else if(!Q_stricmp("false", ((char *)data)))
		{
			Q3_SetFuncUsableVisible( entID, false);
		}
		break;

	case SET_NO_KNOCKBACK:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetNoKnockback(entID, true);
		}
		else
		{
			Q3_SetNoKnockback(entID, false);
		}
		break;

	case SET_VIDEO_PLAY:
		// don't do this check now, James doesn't want a scripted cinematic to also skip any Video cinematics as well,
		//	the "timescale" and "skippingCinematic" cvars will be set back to normal in the Video code, so doing a
		//	skip will now only skip one section of a multiple-part story (eg VOY1 bridge sequence)

//		if ( g_timescale->value <= 1.0f )
		{
			G_DebugPrint( WL_WARNING, "SET_VIDEO_PLAY: NOT SUPPORTED IN MP\n");
			//SV_SendConsoleCommand( va("inGameCinematic %s\n", (char *)data) );
		}
		break;

	case SET_VIDEO_FADE_IN:
		G_DebugPrint( WL_WARNING, "SET_VIDEO_FADE_IN: NOT SUPPORTED IN MP\n");
		break;

	case SET_VIDEO_FADE_OUT:
		G_DebugPrint( WL_WARNING, "SET_VIDEO_FADE_OUT: NOT SUPPORTED IN MP\n");
		break;

	case SET_LOADGAME:
		//trap->SendConsoleCommand( va("load %s\n", (const char *) data ) );
		G_DebugPrint( WL_WARNING, "SET_LOADGAME: NOT SUPPORTED IN MP\n");
		break;

	case SET_MENU_SCREEN:
		//UI_SetActiveMenu( (const char *) data );
		break;

	case SET_OBJECTIVE_SHOW:
		G_DebugPrint( WL_WARNING, "SET_OBJECTIVE_SHOW: NOT SUPPORTED IN MP\n");
		break;
	case SET_OBJECTIVE_HIDE:
		G_DebugPrint( WL_WARNING, "SET_OBJECTIVE_HIDE: NOT SUPPORTED IN MP\n");
		break;
	case SET_OBJECTIVE_SUCCEEDED:
		G_DebugPrint( WL_WARNING, "SET_OBJECTIVE_SUCCEEDED: NOT SUPPORTED IN MP\n");
		break;
	case SET_OBJECTIVE_FAILED:
		G_DebugPrint( WL_WARNING, "SET_OBJECTIVE_FAILED: NOT SUPPORTED IN MP\n");
		break;

	case SET_OBJECTIVE_CLEARALL:
		G_DebugPrint( WL_WARNING, "SET_OBJECTIVE_CLEARALL: NOT SUPPORTED IN MP\n");
		break;

	case SET_MISSIONFAILED:
		G_DebugPrint( WL_WARNING, "SET_MISSIONFAILED: NOT SUPPORTED IN MP\n");
		break;

	case SET_MISSIONSTATUSTEXT:
		G_DebugPrint( WL_WARNING, "SET_MISSIONSTATUSTEXT: NOT SUPPORTED IN MP\n");
		break;

	case SET_MISSIONSTATUSTIME:
		G_DebugPrint( WL_WARNING, "SET_MISSIONSTATUSTIME: NOT SUPPORTED IN MP\n");
		break;

	case SET_CLOSINGCREDITS:
		G_DebugPrint( WL_WARNING, "SET_CLOSINGCREDITS: NOT SUPPORTED IN MP\n");
		break;

	case SET_SKILL:
//		//can never be set
		break;

	case SET_FULLNAME:
		Q3_SetFullName( entID, (char *) data );
		break;

	case SET_DISABLE_SHADER_ANIM:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetDisableShaderAnims( entID, true);
		}
		else
		{
			Q3_SetDisableShaderAnims( entID, false);
		}
		break;

	case SET_SHADER_ANIM:
		if(!Q_stricmp("true", ((char *)data)))
		{
			Q3_SetShaderAnim( entID, true);
		}
		else
		{
			Q3_SetShaderAnim( entID, false);
		}
		break;

	case SET_MUSIC_STATE:
		Q3_SetMusicState( (char *) data );
		break;

	case SET_CLEAN_DAMAGING_ENTS:
		Q3_SetCleanDamagingEnts();
		break;

	case SET_HUD:
		G_DebugPrint( WL_WARNING, "SET_HUD: NOT SUPPORTED IN MP\n");
		break;

	case SET_FORCE_HEAL_LEVEL:
	case SET_FORCE_JUMP_LEVEL:
	case SET_FORCE_SPEED_LEVEL:
	case SET_FORCE_PUSH_LEVEL:
	case SET_FORCE_PULL_LEVEL:
	case SET_FORCE_MINDTRICK_LEVEL:
	case SET_FORCE_GRIP_LEVEL:
	case SET_FORCE_LIGHTNING_LEVEL:
	case SET_SABER_THROW:
	case SET_SABER_DEFENSE:
	case SET_SABER_OFFENSE:
		int_data = atoi((char *) data);
		Q3_SetForcePowerLevel( entID, (toSet-SET_FORCE_HEAL_LEVEL), int_data );
		break;

	default:
		//G_DebugPrint( WL_ERROR, "Q3_Set: '%s' is not a valid set field\n", type_name );
		trap->ICARUS_SetVar( taskID, entID, type_name, data );
		break;
	}

	return true;
}
