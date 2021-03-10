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

// cg_playerstate.c -- this file acts on changes in a new playerState_t
// With normal play, this will be done after local prediction, but when
// following another player or playing back a demo, it will be checked
// when the snapshot transitions like all the other entities

#include "cgame/cg_local.hpp"
#include "cgame/cg_media.hpp"

void CG_DamageFeedback( int yawByte, int pitchByte, int damage ) {
	float		left, front, up;
	float		kick;
	int			health;
	float		scale;
	vec3_t		dir;
	vec3_t		angles;
	float		dist;
	float		yaw, pitch;

	// show the attacking player's head and name in corner
	cg.attackerTime = cg.time;

	// the lower on health you are, the greater the view kick will be
	health = cg.snap->ps.stats[STAT_HEALTH];
	if ( health < 40 ) {
		scale = 1;
	} else {
		scale = 40.0 / health;
	}
	kick = damage * scale;

	if (kick < 5)
		kick = 5;
	if (kick > 10)
		kick = 10;

	// if yaw and pitch are both 255, make the damage always centered (falling, etc)
	if ( yawByte == 255 && pitchByte == 255 ) {
		cg.damageX = 0;
		cg.damageY = 0;
		cg.v_dmg_roll = 0;
		cg.v_dmg_pitch = -kick;
	} else {
		// positional
		pitch = pitchByte / 255.0 * 360;
		yaw = yawByte / 255.0 * 360;

		angles[PITCH] = pitch;
		angles[YAW] = yaw;
		angles[ROLL] = 0;

		AngleVectors( angles, dir, nullptr, nullptr );
		VectorSubtract( vec3_origin, dir, dir );

		front = DotProduct (dir, cg.refdef.viewaxis[0] );
		left = DotProduct (dir, cg.refdef.viewaxis[1] );
		up = DotProduct (dir, cg.refdef.viewaxis[2] );

		dir[0] = front;
		dir[1] = left;
		dir[2] = 0;
		dist = VectorLength( dir );
		if ( dist < 0.1 ) {
			dist = 0.1f;
		}

		cg.v_dmg_roll = kick * left;

		cg.v_dmg_pitch = -kick * front;

		if ( front <= 0.1 ) {
			front = 0.1f;
		}
		cg.damageX = -left / front;
		cg.damageY = up / dist;
	}

	// clamp the position
	if ( cg.damageX > 1.0 ) {
		cg.damageX = 1.0;
	}
	if ( cg.damageX < - 1.0 ) {
		cg.damageX = -1.0;
	}

	if ( cg.damageY > 1.0 ) {
		cg.damageY = 1.0;
	}
	if ( cg.damageY < - 1.0 ) {
		cg.damageY = -1.0;
	}

	// don't let the screen flashes vary as much
	if ( kick > 10 ) {
		kick = 10;
	}
	cg.damageValue = kick;
	cg.v_dmg_time = cg.time + DAMAGE_TIME;
	cg.damageTime = cg.snap->serverTime;
}

// A respawn happened this snapshot
void CG_Respawn( void ) {
	// no error decay on player movement
	cg.thisFrameTeleport = true;

	// display weapons available
	cg.weaponSelectTime = cg.time;

	// select the weapon the server says we are using
	cg.weaponSelect = cg.snap->ps.weapon;
}

void CG_CheckPlayerstateEvents( playerState_t *ps, playerState_t *ops ) {
	int			i;
	int			event;
	centity_t	*cent;

	if ( ps->externalEvent && ps->externalEvent != ops->externalEvent ) {
		cent = &cg_entities[ ps->clientNum ];
		cent->currentState.event = ps->externalEvent;
		cent->currentState.eventParm = ps->externalEventParm;
		CG_EntityEvent( cent, cent->lerpOrigin );
	}

	cent = &cg_entities[ ps->clientNum ];
	// go through the predictable events buffer
	for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {
		// if we have a new predictable event
		if ( i >= ops->eventSequence
			// or the server told us to play another event instead of a predicted event we already issued
			// or something the server told us changed our prediction causing a different event
			|| (i > ops->eventSequence - MAX_PS_EVENTS && ps->events[i & (MAX_PS_EVENTS-1)] != ops->events[i & (MAX_PS_EVENTS-1)]) ) {

			event = ps->events[ i & (MAX_PS_EVENTS-1) ];
			cent->currentState.event = event;
			cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
//JLF ADDED to hopefully mark events as player event
			cent->playerState = ps;
			CG_EntityEvent( cent, cent->lerpOrigin );

			cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

			cg.eventSequence++;
		}
	}
}

void CG_CheckChangedPredictableEvents( playerState_t *ps ) {
	int i;
	int event;
	centity_t	*cent;

	cent = &cg_entities[ps->clientNum];
	for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {

		if (i >= cg.eventSequence) {
			continue;
		}
		// if this event is not further back in than the maximum predictable events we remember
		if (i > cg.eventSequence - MAX_PREDICTED_EVENTS) {
			// if the new playerstate event is different from a previously predicted one
			if ( ps->events[i & (MAX_PS_EVENTS-1)] != cg.predictableEvents[i & (MAX_PREDICTED_EVENTS-1) ] ) {

				event = ps->events[ i & (MAX_PS_EVENTS-1) ];
				cent->currentState.event = event;
				cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
				CG_EntityEvent( cent, cent->lerpOrigin );

				cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

				if ( cg_showMiss.integer ) {
					trap->Print("WARNING: changed predicted event\n");
				}
			}
		}
	}
}

static void pushReward(sfxHandle_t sfx, qhandle_t shader, int rewardCount) {
	if (cg.rewardStack < (MAX_REWARDSTACK-1)) {
		cg.rewardStack++;
		cg.rewardSound[cg.rewardStack] = sfx;
		cg.rewardShader[cg.rewardStack] = shader;
		cg.rewardCount[cg.rewardStack] = rewardCount;
	}
}

int cgAnnouncerTime = 0; //to prevent announce sounds from playing on top of each other

void CG_CheckLocalSounds( playerState_t *ps, playerState_t *ops ) {
	int			highScore, health, armor, reward;
	sfxHandle_t sfx;

	// don't play the sounds if the player just changed teams
	if ( ps->persistant[PERS_TEAM] != ops->persistant[PERS_TEAM] ) {
		return;
	}

	// hit changes
	if ( ps->persistant[PERS_HITS] > ops->persistant[PERS_HITS] ) {
		armor  = ps->persistant[PERS_ATTACKEE_ARMOR] & 0xff;
		health = ps->persistant[PERS_ATTACKEE_ARMOR] >> 8;

		if (armor > health/2)
		{	// We also hit shields along the way, so consider them "pierced".
//			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		}
		else
		{	// Shields didn't really stand in our way.
//			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		}

		//FIXME: Hit sounds?
		/*
		if (armor > 50 ) {
			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		} else if (armor || health > 100) {
			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		} else {
			trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
		}
		*/
	} else if ( ps->persistant[PERS_HITS] < ops->persistant[PERS_HITS] ) {
		//trap->S_StartLocalSound( media.sounds.null, CHAN_LOCAL_SOUND );
	}

	// health changes of more than -3 should make pain sounds
	if (cg_oldPainSounds.integer)
	{
		if ( ps->stats[STAT_HEALTH] < (ops->stats[STAT_HEALTH] - 3))
		{
			if ( ps->stats[STAT_HEALTH] > 0 )
			{
				CG_PainEvent( &cg_entities[cg.predictedPlayerState.clientNum], ps->stats[STAT_HEALTH] );
			}
		}
	}

	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted || (cg.snap && cg.snap->ps.pm_type == PM_INTERMISSION) ) {
		return;
	}

	// reward sounds
	reward = false;
	if (ps->persistant[PERS_CAPTURES] != ops->persistant[PERS_CAPTURES]) {
		pushReward(media.gfx.null, media.gfx.null, ps->persistant[PERS_CAPTURES]);
		reward = true;
		//Com_Printf("capture\n");
	}
	if (ps->persistant[PERS_IMPRESSIVE_COUNT] != ops->persistant[PERS_IMPRESSIVE_COUNT]) {
		sfx = media.gfx.null;

		pushReward(sfx, media.gfx.null, ps->persistant[PERS_IMPRESSIVE_COUNT]);
		reward = true;
		//Com_Printf("impressive\n");
	}
	if (ps->persistant[PERS_EXCELLENT_COUNT] != ops->persistant[PERS_EXCELLENT_COUNT]) {
		sfx = media.gfx.null;
		pushReward(sfx, media.gfx.null, ps->persistant[PERS_EXCELLENT_COUNT]);
		reward = true;
		//Com_Printf("excellent\n");
	}
	if (ps->persistant[PERS_GAUNTLET_FRAG_COUNT] != ops->persistant[PERS_GAUNTLET_FRAG_COUNT]) {
		sfx = media.gfx.null;
		pushReward(sfx, media.gfx.null, ps->persistant[PERS_GAUNTLET_FRAG_COUNT]);
		reward = true;
		//Com_Printf("gauntlet frag\n");
	}
	if (ps->persistant[PERS_DEFEND_COUNT] != ops->persistant[PERS_DEFEND_COUNT]) {
		pushReward(media.gfx.null, media.gfx.null, ps->persistant[PERS_DEFEND_COUNT]);
		reward = true;
		//Com_Printf("defend\n");
	}
	if (ps->persistant[PERS_ASSIST_COUNT] != ops->persistant[PERS_ASSIST_COUNT]) {
		//pushReward(media.gfx.null, media.gfx.null, ps->persistant[PERS_ASSIST_COUNT]);
		//reward = true;
		//Com_Printf("assist\n");
	}
	// if any of the player event bits changed
	if (ps->persistant[PERS_PLAYEREVENTS] != ops->persistant[PERS_PLAYEREVENTS]) {
		if ((ps->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_DENIEDREWARD) !=
				(ops->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_DENIEDREWARD)) {
			trap->S_StartLocalSound( media.sounds.null, CHAN_ANNOUNCER );
		}
		else if ((ps->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_GAUNTLETREWARD) !=
				(ops->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_GAUNTLETREWARD)) {
			trap->S_StartLocalSound( media.sounds.null, CHAN_ANNOUNCER );
		}
		reward = true;
	}

	// lead changes
	if (!reward && cgAnnouncerTime < cg.time) {

		if ( !cg.warmup && cgs.gametype != GT_POWERDUEL ) {
			// never play lead changes during warmup and powerduel
			if ( ps->persistant[PERS_RANK] != ops->persistant[PERS_RANK] ) {
				if ( cgs.gametype < GT_TEAM) {
					/*
					if (  ps->persistant[PERS_RANK] == 0 ) {
						CG_AddBufferedSound(media.sounds.null);
						cgAnnouncerTime = cg.time + 3000;
					} else if ( ps->persistant[PERS_RANK] == RANK_TIED_FLAG ) {
						//CG_AddBufferedSound(media.sounds.null);
					} else if ( ( ops->persistant[PERS_RANK] & ~RANK_TIED_FLAG ) == 0 ) {
						//rww - only bother saying this if you have more than 1 kill already.
						//joining the server and hearing "the force is not with you" is silly.
						if (ps->persistant[PERS_SCORE] > 0)
						{
							CG_AddBufferedSound(media.sounds.null);
							cgAnnouncerTime = cg.time + 3000;
						}
					}
					*/
				}
			}
		}
	}

	// timelimit warnings
	if ( cgs.timelimit > 0 && cgAnnouncerTime < cg.time ) {
		int		msec;

		msec = cg.time - cgs.levelStartTime;
		if ( !( cg.timelimitWarnings & 4 ) && msec > ( cgs.timelimit * 60 + 2 ) * 1000 ) {
			cg.timelimitWarnings |= 1 | 2 | 4;
			//trap->S_StartLocalSound( media.sounds.null, CHAN_ANNOUNCER );
		}
		else if ( !( cg.timelimitWarnings & 2 ) && msec > (cgs.timelimit - 1) * 60 * 1000 ) {
			cg.timelimitWarnings |= 1 | 2;
			trap->S_StartLocalSound( media.sounds.null, CHAN_ANNOUNCER );
			cgAnnouncerTime = cg.time + 3000;
		}
		else if ( cgs.timelimit > 5 && !( cg.timelimitWarnings & 1 ) && msec > (cgs.timelimit - 5) * 60 * 1000 ) {
			cg.timelimitWarnings |= 1;
			trap->S_StartLocalSound( media.sounds.null, CHAN_ANNOUNCER );
			cgAnnouncerTime = cg.time + 3000;
		}
	}

	// fraglimit warnings
	if ( cgs.fraglimit > 0 && cgs.gametype < GT_CTF && cgs.gametype != GT_DUEL && cgs.gametype != GT_POWERDUEL && cgAnnouncerTime < cg.time) {
		highScore = cgs.scores1;
		if ( cgs.gametype == GT_TEAM && cgs.scores2 > highScore )
			highScore = cgs.scores2;

		if ( !( cg.fraglimitWarnings & 4 ) && highScore == (cgs.fraglimit - 1) ) {
			cg.fraglimitWarnings |= 1 | 2 | 4;
			CG_AddBufferedSound(media.sounds.null);
			cgAnnouncerTime = cg.time + 3000;
		}
		else if ( cgs.fraglimit > 2 && !( cg.fraglimitWarnings & 2 ) && highScore == (cgs.fraglimit - 2) ) {
			cg.fraglimitWarnings |= 1 | 2;
			CG_AddBufferedSound(media.sounds.null);
			cgAnnouncerTime = cg.time + 3000;
		}
		else if ( cgs.fraglimit > 3 && !( cg.fraglimitWarnings & 1 ) && highScore == (cgs.fraglimit - 3) ) {
			cg.fraglimitWarnings |= 1;
			CG_AddBufferedSound(media.sounds.null);
			cgAnnouncerTime = cg.time + 3000;
		}
	}
}

void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops ) {
	// check for changing follow mode
	if ( ps->clientNum != ops->clientNum ) {
		cg.thisFrameTeleport = true;
		// make sure we don't get any unwanted transition effects
		*ops = *ps;
	}

	// damage events (player is getting wounded)
	if ( ps->damageEvent != ops->damageEvent && ps->damageCount ) {
		CG_DamageFeedback( ps->damageYaw, ps->damagePitch, ps->damageCount );
	}

	// respawning
	if ( ps->persistant[PERS_SPAWN_COUNT] != ops->persistant[PERS_SPAWN_COUNT] ) {
		CG_Respawn();
	}

	if ( cg.mapRestart ) {
		CG_Respawn();
		cg.mapRestart = false;
	}

	if ( cg.snap->ps.pm_type != PM_INTERMISSION
		&& ps->persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
		CG_CheckLocalSounds( ps, ops );
	}

	// run events
	CG_CheckPlayerstateEvents( ps, ops );

	// smooth the ducking viewheight change
	if ( ps->viewheight != ops->viewheight ) {
		cg.duckChange = ps->viewheight - ops->viewheight;
		if ( cg_instantDuck.integer ) {
			cg.duckTime = cg.time - DUCK_TIME;
		}
		else {
			cg.duckTime = cg.time;
		}
	}
}
