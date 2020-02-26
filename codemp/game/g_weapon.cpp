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

// g_weapon.c
// perform the server side effects of a weapon firing

#include "game/g_local.hpp"
#include "ghoul2/G2.hpp"
#include "qcommon/q_shared.hpp"

static vec3_t forward, vright, up;
static vec3_t muzzle;

// Bryar Pistol
#define BRYAR_PISTOL_VEL			1600
#define BRYAR_PISTOL_DAMAGE			10
#define BRYAR_CHARGE_UNIT			200.0f	// bryar charging gives us one more unit every 200ms--if you change this, you'll have to do the same in bg_pmove
#define BRYAR_ALT_SIZE				1.0f

// E11 Blaster
#define BLASTER_SPREAD				1.6f//1.2f
#define BLASTER_VELOCITY			2300
#define BLASTER_DAMAGE				20

// Tenloss Disruptor
#define DISRUPTOR_MAIN_DAMAGE			30 //40
#define DISRUPTOR_NPC_MAIN_DAMAGE_CUT	0.25f

#define DISRUPTOR_ALT_DAMAGE			100 //125
#define DISRUPTOR_NPC_ALT_DAMAGE_CUT	0.2f
#define DISRUPTOR_ALT_TRACES			3		// can go through a max of 3 damageable(sp?) entities
#define DISRUPTOR_CHARGE_UNIT			50.0f	// distruptor charging gives us one more unit every 50ms--if you change this, you'll have to do the same in bg_pmove

// Wookiee Bowcaster
#define	BOWCASTER_DAMAGE			50
#define	BOWCASTER_VELOCITY			1300
#define BOWCASTER_SPLASH_DAMAGE		0
#define BOWCASTER_SPLASH_RADIUS		0
#define BOWCASTER_SIZE				2

#define BOWCASTER_ALT_SPREAD		5.0f
#define BOWCASTER_VEL_RANGE			0.3f
#define BOWCASTER_CHARGE_UNIT		200.0f	// bowcaster charging gives us one more unit every 200ms--if you change this, you'll have to do the same in bg_pmove

// Heavy Repeater
#define REPEATER_SPREAD				1.4f
#define	REPEATER_DAMAGE				14
#define	REPEATER_VELOCITY			1600

#define REPEATER_ALT_SIZE				3	// half of bbox size
#define	REPEATER_ALT_DAMAGE				60
#define REPEATER_ALT_SPLASH_DAMAGE		60
#define REPEATER_ALT_SPLASH_RADIUS		128
#define	REPEATER_ALT_VELOCITY			1100

// DEMP2
#define	DEMP2_DAMAGE				35
#define	DEMP2_VELOCITY				1800
#define	DEMP2_SIZE					2		// half of bbox size

#define DEMP2_ALT_DAMAGE			8 //12		// does 12, 36, 84 at each of the 3 charge levels.
#define DEMP2_CHARGE_UNIT			700.0f	// demp2 charging gives us one more unit every 700ms--if you change this, you'll have to do the same in bg_weapons
#define DEMP2_ALT_RANGE				4096
#define DEMP2_ALT_SPLASHRADIUS		256

// Golan Arms Flechette
#define FLECHETTE_SHOTS				5
#define FLECHETTE_SPREAD			4.0f
#define FLECHETTE_DAMAGE			12//15
#define FLECHETTE_VEL				3500
#define FLECHETTE_SIZE				1
#define FLECHETTE_MINE_RADIUS_CHECK	256
#define FLECHETTE_ALT_DAMAGE		60
#define FLECHETTE_ALT_SPLASH_DAM	60
#define FLECHETTE_ALT_SPLASH_RAD	128

// Personal Rocket Launcher
#define	ROCKET_VELOCITY				900
#define	ROCKET_DAMAGE				100
#define	ROCKET_SPLASH_DAMAGE		100
#define	ROCKET_SPLASH_RADIUS		160
#define ROCKET_SIZE					3
#define ROCKET_ALT_THINK_TIME		100

// Concussion Rifle
//primary
//man, this thing is too absurdly powerful. having to
//slash the values way down from sp.
#define	CONC_VELOCITY				3000
#define	CONC_DAMAGE					75 //150
#define	CONC_NPC_DAMAGE_EASY		40
#define	CONC_NPC_DAMAGE_NORMAL		80
#define	CONC_NPC_DAMAGE_HARD		100
#define	CONC_SPLASH_DAMAGE			40 //50
#define	CONC_SPLASH_RADIUS			200 //300
//alt
#define CONC_ALT_DAMAGE				25 //100
#define CONC_ALT_NPC_DAMAGE_EASY	20
#define CONC_ALT_NPC_DAMAGE_MEDIUM	35
#define CONC_ALT_NPC_DAMAGE_HARD	50

// Stun Baton
#define STUN_BATON_DAMAGE			20
#define STUN_BATON_ALT_DAMAGE		20
#define STUN_BATON_RANGE			8

// Melee
#define MELEE_SWING1_DAMAGE			10
#define MELEE_SWING2_DAMAGE			12
#define MELEE_RANGE					8

// ATST Main Gun
#define ATST_MAIN_VEL				4000	//
#define ATST_MAIN_DAMAGE			25		//
#define ATST_MAIN_SIZE				3		// make it easier to hit things

// ATST Side Gun
#define ATST_SIDE_MAIN_DAMAGE				75
#define ATST_SIDE_MAIN_VELOCITY				1300
#define ATST_SIDE_MAIN_NPC_DAMAGE_EASY		30
#define ATST_SIDE_MAIN_NPC_DAMAGE_NORMAL	40
#define ATST_SIDE_MAIN_NPC_DAMAGE_HARD		50
#define ATST_SIDE_MAIN_SIZE					4
#define ATST_SIDE_MAIN_SPLASH_DAMAGE		10	// yeah, pretty small, either zero out or make it worth having?
#define ATST_SIDE_MAIN_SPLASH_RADIUS		16	// yeah, pretty small, either zero out or make it worth having?

#define ATST_SIDE_ALT_VELOCITY				1100
#define ATST_SIDE_ALT_NPC_VELOCITY			600
#define ATST_SIDE_ALT_DAMAGE				130

#define ATST_SIDE_ROCKET_NPC_DAMAGE_EASY	30
#define ATST_SIDE_ROCKET_NPC_DAMAGE_NORMAL	50
#define ATST_SIDE_ROCKET_NPC_DAMAGE_HARD	90

#define	ATST_SIDE_ALT_SPLASH_DAMAGE			130
#define	ATST_SIDE_ALT_SPLASH_RADIUS			200
#define ATST_SIDE_ALT_ROCKET_SIZE			5
#define ATST_SIDE_ALT_ROCKET_SPLASH_SCALE	0.5f	// scales splash for NPC's

static void WP_FireEmplaced( gentity_t *ent, bool altFire );

void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace )
{

}

//We should really organize weapon data into tables or parse from the ext data so we have accurate info for this,
float WP_SpeedOfMissileForWeapon( int wp, bool alt_fire )
{
	return 500;
}

void W_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs )
{
	//make sure our start point isn't on the other side of a wall
	trace_t	tr;
	vec3_t	entMins;
	vec3_t	entMaxs;
	vec3_t	eyePoint;

	VectorAdd( ent->r.currentOrigin, ent->r.mins, entMins );
	VectorAdd( ent->r.currentOrigin, ent->r.maxs, entMaxs );

	if ( G_BoxInBounds( start, mins, maxs, entMins, entMaxs ) )
	{
		return;
	}

	if ( !ent->client )
	{
		return;
	}

	VectorCopy( ent->s.pos.trBase, eyePoint);
	eyePoint[2] += ent->client->ps.viewheight;

	trap->Trace( &tr, eyePoint, mins, maxs, start, ent->s.number, MASK_SOLID|CONTENTS_SHOTCLIP, false, 0, 0 );

	if ( tr.startsolid || tr.allsolid )
	{
		return;
	}

	if ( tr.fraction < 1.0f )
	{
		VectorCopy( tr.endpos, start );
	}
}

// PLAYER WEAPONS

// BRYAR PISTOL
static void WP_FireBryarPistol( gentity_t *ent, bool altFire )
{
	int damage = BRYAR_PISTOL_DAMAGE;
	int count;

	gentity_t	*missile = CreateMissile( muzzle, forward, BRYAR_PISTOL_VEL, 10000, ent, altFire );

	missile->classname = "bryar_proj";
	missile->s.weapon = WP_BRYAR_PISTOL;

	if ( altFire )
	{
		float boxSize = 0;

		count = ( level.time - ent->client->ps.weaponChargeTime ) / BRYAR_CHARGE_UNIT;

		if ( count < 1 )
		{
			count = 1;
		}
		else if ( count > 5 )
		{
			count = 5;
		}

		if (count > 1)
		{
			damage *= (count*1.7);
		}
		else
		{
			damage *= (count*1.5);
		}

		missile->s.generic1 = count; // The missile will then render according to the charge level.

		boxSize = BRYAR_ALT_SIZE*(count*0.5);

		VectorSet( missile->r.maxs, boxSize, boxSize, boxSize );
		VectorSet( missile->r.mins, -boxSize, -boxSize, -boxSize );
	}

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	if (altFire)
	{
		missile->methodOfDeath = MOD_BRYAR_PISTOL_ALT;
	}
	else
	{
		missile->methodOfDeath = MOD_BRYAR_PISTOL;
	}
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

// GENERIC

void WP_FireTurretMissile( gentity_t *ent, vec3_t start, vec3_t dir, bool altFire, int damage, int velocity, int mod, gentity_t *ignore )
{
	gentity_t *missile;

	missile = CreateMissile( start, dir, velocity, 10000, ent, altFire );

	missile->classname = "generic_proj";
	missile->s.weapon = WP_TURRET;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = mod;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	if (ignore)
	{
		missile->passThroughNum = ignore->s.number+1;
	}

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

void WP_Explode( gentity_t *self )
{
	gentity_t	*attacker = self;
	vec3_t		forwardVec={0,0,1};

	// stop chain reaction runaway loops
	self->takedamage = false;

	self->s.loopSound = 0;

//	VectorCopy( self->currentOrigin, self->s.pos.trBase );
	if ( !self->client )
	{
	AngleVectors( self->s.angles, forwardVec, nullptr, nullptr );
	}

	// FIXME
	/*if ( self->e > 0 )
	{
		G_PlayEffect( self->fxID, self->r.currentOrigin, forwardVec );
	}*/

	if ( self->s.owner && self->s.owner != 1023 )
	{
		attacker = &g_entities[self->s.owner];
	}
	else if ( self->activator )
	{
		attacker = self->activator;
	}
	else if ( self->client )
	{
		attacker = self;
	}

	if ( self->splashDamage > 0 && self->splashRadius > 0 )
	{
		G_RadiusDamage( self->r.currentOrigin, attacker, self->splashDamage, self->splashRadius, 0/*don't ignore attacker*/, self, MOD_UNKNOWN );
	}

	if ( self->target )
	{
		G_UseTargets( self, attacker );
	}

	G_SetOrigin( self, self->r.currentOrigin );

	self->nextthink = level.time + 50;
	self->think = G_FreeEntity;
}

//Currently only the seeker drone uses this, but it might be useful for other things as well.

void WP_FireGenericBlasterMissile( gentity_t *ent, vec3_t start, vec3_t dir, bool altFire, int damage, int velocity, int mod )
{
	gentity_t *missile;

	missile = CreateMissile( start, dir, velocity, 10000, ent, altFire );

	missile->classname = "generic_proj";
	missile->s.weapon = WP_BRYAR_PISTOL;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = mod;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

// BLASTER

void WP_FireBlasterMissile( gentity_t *ent, vec3_t start, vec3_t dir, bool altFire )
{
	int velocity	= BLASTER_VELOCITY;
	int	damage		= BLASTER_DAMAGE;
	gentity_t *missile;

	missile = CreateMissile( start, dir, velocity, 10000, ent, altFire );

	missile->classname = "blaster_proj";
	missile->s.weapon = WP_BLASTER;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = MOD_BLASTER;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

void WP_FireTurboLaserMissile( gentity_t *ent, vec3_t start, vec3_t dir )
{
	int velocity	= ent->mass; //FIXME: externalize
	gentity_t *missile;

	missile = CreateMissile( start, dir, velocity, 10000, ent, false );

	//use a custom shot effect
	missile->s.otherEntityNum2 = ent->genericValue14;
	//use a custom impact effect
	missile->s.emplacedOwner = ent->genericValue15;

	missile->classname = "turbo_proj";
	missile->s.weapon = WP_TURRET;

	missile->damage = ent->damage;		//FIXME: externalize
	missile->splashDamage = ent->splashDamage;	//FIXME: externalize
	missile->splashRadius = ent->splashRadius;	//FIXME: externalize
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = MOD_TURBLAST; //count as a heavy weap
	missile->splashMethodOfDeath = MOD_TURBLAST;// ?SPLASH;
	missile->clipmask = MASK_SHOT;

	// we don't want it to bounce forever
	missile->bounceCount = 8;

	//set veh as cgame side owner for purpose of fx overrides
	missile->s.owner = ent->s.number;

	//don't let them last forever
	missile->think = G_FreeEntity;
	missile->nextthink = level.time + 5000;//at 20000 speed, that should be more than enough
}

void WP_FireEmplacedMissile( gentity_t *ent, vec3_t start, vec3_t dir, bool altFire, gentity_t *ignore )
{
	int velocity	= BLASTER_VELOCITY;
	int	damage		= BLASTER_DAMAGE;
	gentity_t *missile;

	missile = CreateMissile( start, dir, velocity, 10000, ent, altFire );

	missile->classname = "emplaced_gun_proj";
	missile->s.weapon = WP_TURRET;//WP_EMPLACED_GUN;

	missile->activator = ignore;

	missile->damage = damage;
	missile->dflags = (DAMAGE_DEATH_KNOCKBACK|DAMAGE_HEAVY_WEAP_CLASS);
	missile->methodOfDeath = MOD_BLASTER;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	if (ignore)
	{
		missile->passThroughNum = ignore->s.number+1;
	}

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

static void WP_FireBlaster( gentity_t *ent, bool altFire )
{
	vec3_t  dir, angs;

	vectoangles( forward, angs );

	if ( altFire )
	{
		// add some slop to the alt-fire direction
		angs[PITCH] += Q_flrand(-1.0f, 1.0f) * BLASTER_SPREAD;
		angs[YAW]       += Q_flrand(-1.0f, 1.0f) * BLASTER_SPREAD;
	}

	AngleVectors( angs, dir, nullptr, nullptr );

	// FIXME: if temp_org does not have clear trace to inside the bbox, don't shoot!
	WP_FireBlasterMissile( ent, muzzle, dir, altFire );
}

// DISRUPTOR

static void WP_DisruptorMainFire( gentity_t *ent )
{
	int			damage = DISRUPTOR_MAIN_DAMAGE;
	bool	render_impact = true;
	vec3_t		start, end;
	trace_t		tr;
	gentity_t	*traceEnt, *tent;
	float		shotRange = 8192;
	int			ignore, traces;

	memset(&tr, 0, sizeof(tr)); //to shut the compiler up

	VectorCopy( ent->client->ps.origin, start );
	start[2] += ent->client->ps.viewheight;//By eyes

	VectorMA( start, shotRange, forward, end );

	ignore = ent->s.number;
	traces = 0;
	while ( traces < 10 )
	{//need to loop this in case we hit a Jedi who dodges the shot
		if (d_projectileGhoul2Collision.integer)
		{
			trap->Trace( &tr, start, nullptr, nullptr, end, ignore, MASK_SHOT, false, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_THICK|G2TRFLAG_HITCORPSES, g_g2TraceLod.integer );
		}
		else
		{
			trap->Trace( &tr, start, nullptr, nullptr, end, ignore, MASK_SHOT, false, 0, 0 );
		}

		traceEnt = &g_entities[tr.entityNum];

		if (d_projectileGhoul2Collision.integer && traceEnt->inuse && traceEnt->client)
		{ //g2 collision checks -rww
			if (traceEnt->inuse && traceEnt->client && traceEnt->ghoul2)
			{ //since we used G2TRFLAG_GETSURFINDEX, tr.surfaceFlags will actually contain the index of the surface on the ghoul2 model we collided with.
				traceEnt->client->g2LastSurfaceHit = tr.surfaceFlags;
				traceEnt->client->g2LastSurfaceTime = level.time;
			}

			if (traceEnt->ghoul2)
			{
				tr.surfaceFlags = 0; //clear the surface flags after, since we actually care about them in here.
			}
		}

		if (traceEnt && traceEnt->client && traceEnt->client->ps.duelInProgress &&
			traceEnt->client->ps.duelIndex != ent->s.number)
		{
			VectorCopy( tr.endpos, start );
			ignore = tr.entityNum;
			traces++;
			continue;
		}

		if ( Jedi_DodgeEvasion( traceEnt, ent, &tr, G_GetHitLocation(traceEnt, tr.endpos) ) )
		{//act like we didn't even hit him
			VectorCopy( tr.endpos, start );
			ignore = tr.entityNum;
			traces++;
			continue;
		}
		else if (traceEnt && traceEnt->client && traceEnt->client->ps.fd.forcePowerLevel[FP_SABER_DEFENSE] >= FORCE_LEVEL_3)
		{
			if (WP_SaberCanBlock(traceEnt, tr.endpos, 0, MOD_DISRUPTOR, true, 0))
			{ //broadcast and stop the shot because it was blocked
				gentity_t *te = nullptr;

				tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_MAIN_SHOT );
				VectorCopy( muzzle, tent->s.origin2 );
				tent->s.eventParm = ent->s.number;

				te = G_TempEntity( tr.endpos, EV_SABER_BLOCK );
				VectorCopy(tr.endpos, te->s.origin);
				VectorCopy(tr.plane.normal, te->s.angles);
				if (!te->s.angles[0] && !te->s.angles[1] && !te->s.angles[2])
				{
					te->s.angles[1] = 1;
				}
				te->s.eventParm = 0;
				te->s.weapon = 0;//saberNum
				te->s.legsAnim = 0;//bladeNum

				return;
			}
		}
		else if ( (traceEnt->flags&FL_SHIELDED) )
		{//stopped cold
			return;
		}
		//a Jedi is not dodging this shot
		break;
	}

	if ( tr.surfaceFlags & SURF_NOIMPACT )
	{
		render_impact = false;
	}

	// always render a shot beam, doing this the old way because I don't much feel like overriding the effect.
	tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_MAIN_SHOT );
	VectorCopy( muzzle, tent->s.origin2 );
	tent->s.eventParm = ent->s.number;

	traceEnt = &g_entities[tr.entityNum];

	if ( render_impact )
	{
		if ( tr.entityNum < ENTITYNUM_WORLD && traceEnt->takedamage )
		{
			if ( traceEnt->client && LogAccuracyHit( traceEnt, ent ))
			{
				ent->client->accuracy_hits++;
			}

			G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NORMAL, MOD_DISRUPTOR );

			tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_HIT );
			tent->s.eventParm = DirToByte( tr.plane.normal );
			if (traceEnt->client)
			{
				tent->s.weapon = 1;
			}
		}
		else
		{
			 // Hmmm, maybe don't make any marks on things that could break
			tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_SNIPER_MISS );
			tent->s.eventParm = DirToByte( tr.plane.normal );
			tent->s.weapon = 1;
		}
	}
}

void WP_DisruptorAltFire( gentity_t *ent )
{
	int			damage = 0, skip;
	bool	render_impact = true;
	vec3_t		start, end;
	//vec3_t		muzzle2;
	trace_t		tr;
	gentity_t	*traceEnt, *tent;
	float		shotRange = 8192.0f;
	int			i;
	int			count, maxCount = 60;
	int			traces = DISRUPTOR_ALT_TRACES;
	bool	fullCharge = false;

	damage = DISRUPTOR_ALT_DAMAGE-30;

	//VectorCopy( muzzle, muzzle2 ); // making a backup copy

	if (ent->client)
	{
		VectorCopy( ent->client->ps.origin, start );
		start[2] += ent->client->ps.viewheight;//By eyes

		count = ( level.time - ent->client->ps.weaponChargeTime ) / DISRUPTOR_CHARGE_UNIT;
	}
	else
	{
		VectorCopy( ent->r.currentOrigin, start );
		start[2] += 24;

		count = ( 100 ) / DISRUPTOR_CHARGE_UNIT;
	}

	count *= 2;

	if ( count < 1 )
	{
		count = 1;
	}
	else if ( count >= maxCount )
	{
		count = maxCount;
		fullCharge = true;
	}

	// more powerful charges go through more things
	if ( count < 10 )
	{
		traces = 1;
	}
	else if ( count < 20 )
	{
		traces = 2;
	}

	damage += count;

	skip = ent->s.number;

	for (i = 0; i < traces; i++ )
	{
		VectorMA( start, shotRange, forward, end );

		if (d_projectileGhoul2Collision.integer)
		{
			trap->Trace( &tr, start, nullptr, nullptr, end, skip, MASK_SHOT, false, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_THICK|G2TRFLAG_HITCORPSES, g_g2TraceLod.integer );
		}
		else
		{
			trap->Trace( &tr, start, nullptr, nullptr, end, skip, MASK_SHOT, false, 0, 0 );
		}

		if ( tr.entityNum == ent->s.number )
		{
			// should never happen, but basically we don't want to consider a hit to ourselves?
			// Get ready for an attempt to trace through another person
			//VectorCopy( tr.endpos, muzzle2 );
			VectorCopy( tr.endpos, start );
			skip = tr.entityNum;
#ifdef _DEBUG
			trap->Print( "BAD! Disruptor gun shot somehow traced back and hit the owner!\n" );
#endif
			continue;
		}

		traceEnt = &g_entities[tr.entityNum];

		if (d_projectileGhoul2Collision.integer && traceEnt->inuse && traceEnt->client)
		{ //g2 collision checks -rww
			if (traceEnt->inuse && traceEnt->client && traceEnt->ghoul2)
			{ //since we used G2TRFLAG_GETSURFINDEX, tr.surfaceFlags will actually contain the index of the surface on the ghoul2 model we collided with.
				traceEnt->client->g2LastSurfaceHit = tr.surfaceFlags;
				traceEnt->client->g2LastSurfaceTime = level.time;
			}

			if (traceEnt->ghoul2)
			{
				tr.surfaceFlags = 0; //clear the surface flags after, since we actually care about them in here.
			}
		}

		if ( tr.surfaceFlags & SURF_NOIMPACT )
		{
			render_impact = false;
		}

		if (traceEnt && traceEnt->client && traceEnt->client->ps.duelInProgress &&
			traceEnt->client->ps.duelIndex != ent->s.number)
		{
			skip = tr.entityNum;
			VectorCopy(tr.endpos, start);
			continue;
		}

		if (traceEnt && Jedi_DodgeEvasion(traceEnt, ent, &tr, G_GetHitLocation(traceEnt, tr.endpos)))
		{
			skip = tr.entityNum;
			VectorCopy(tr.endpos, start);
			continue;
		}
		else if (traceEnt && traceEnt->client && traceEnt->client->ps.fd.forcePowerLevel[FP_SABER_DEFENSE] >= FORCE_LEVEL_3)
		{
			if (WP_SaberCanBlock(traceEnt, tr.endpos, 0, MOD_DISRUPTOR_SNIPER, true, 0))
			{ //broadcast and stop the shot because it was blocked
				gentity_t *te = nullptr;

				tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_SNIPER_SHOT );
				VectorCopy( muzzle, tent->s.origin2 );
				tent->s.shouldtarget = fullCharge;
				tent->s.eventParm = ent->s.number;

				te = G_TempEntity( tr.endpos, EV_SABER_BLOCK );
				VectorCopy(tr.endpos, te->s.origin);
				VectorCopy(tr.plane.normal, te->s.angles);
				if (!te->s.angles[0] && !te->s.angles[1] && !te->s.angles[2])
				{
					te->s.angles[1] = 1;
				}
				te->s.eventParm = 0;
				te->s.weapon = 0;//saberNum
				te->s.legsAnim = 0;//bladeNum

				return;
			}
		}

		// always render a shot beam, doing this the old way because I don't much feel like overriding the effect.
		tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_SNIPER_SHOT );
		VectorCopy( muzzle, tent->s.origin2 );
		tent->s.shouldtarget = fullCharge;
		tent->s.eventParm = ent->s.number;

		// If the beam hits a skybox, etc. it would look foolish to add impact effects
		if ( render_impact )
		{
			if ( traceEnt->takedamage && traceEnt->client )
			{
				tent->s.otherEntityNum = traceEnt->s.number;

				// Create a simple impact type mark
				tent = G_TempEntity(tr.endpos, EV_MISSILE_MISS);
				tent->s.eventParm = DirToByte(tr.plane.normal);
				tent->s.eFlags |= EF_ALT_FIRING;

				if ( LogAccuracyHit( traceEnt, ent ))
				{
					if (ent->client)
					{
						ent->client->accuracy_hits++;
					}
				}
			}
			else
			{
				 if ( traceEnt->r.svFlags & SVF_GLASS_BRUSH
						|| traceEnt->takedamage
						|| traceEnt->s.eType == ET_MOVER )
				 {
					if ( traceEnt->takedamage )
					{
						G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage,
								DAMAGE_NO_KNOCKBACK, MOD_DISRUPTOR_SNIPER );

						tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_HIT );
						tent->s.eventParm = DirToByte( tr.plane.normal );
					}
				 }
				 else
				 {
					 // Hmmm, maybe don't make any marks on things that could break
					tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_SNIPER_MISS );
					tent->s.eventParm = DirToByte( tr.plane.normal );
				 }
				break; // and don't try any more traces
			}

			if ( (traceEnt->flags&FL_SHIELDED) )
			{//stops us cold
				break;
			}

			if ( traceEnt->takedamage )
			{
				vec3_t preAng;
				int preHealth = traceEnt->health;
				int preLegs = 0;
				int preTorso = 0;

				if (traceEnt->client)
				{
					preLegs = traceEnt->client->ps.legsAnim;
					preTorso = traceEnt->client->ps.torsoAnim;
					VectorCopy(traceEnt->client->ps.viewangles, preAng);
				}

				G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, MOD_DISRUPTOR_SNIPER );

				if (traceEnt->client && preHealth > 0 && traceEnt->health <= 0 && fullCharge)
				{ //was killed by a fully charged sniper shot, so disintegrate
					VectorCopy(preAng, traceEnt->client->ps.viewangles);

					traceEnt->client->ps.eFlags |= EF_DISINTEGRATION;
					VectorCopy(tr.endpos, traceEnt->client->ps.lastHitLoc);

					traceEnt->client->ps.legsAnim = preLegs;
					traceEnt->client->ps.torsoAnim = preTorso;

					traceEnt->r.contents = 0;

					VectorClear(traceEnt->client->ps.velocity);
				}

				tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_HIT );
				tent->s.eventParm = DirToByte( tr.plane.normal );
				if (traceEnt->client)
				{
					tent->s.weapon = 1;
				}
			}
		}
		else // not rendering impact, must be a skybox or other similar thing?
		{
			break; // don't try anymore traces
		}

		// Get ready for an attempt to trace through another person
		VectorCopy( tr.endpos, muzzle );
		VectorCopy( tr.endpos, start );
		skip = tr.entityNum;
	}
}

static void WP_FireDisruptor( gentity_t *ent, bool altFire )
{
	if (ent == nullptr)
	{
		return;
	}

	if (!ent->client || ent->client->ps.zoomMode != 1)
	{ //do not ever let it do the alt fire when not zoomed
		altFire = false;
	}

	if ( altFire )
	{
		WP_DisruptorAltFire( ent );
	}
	else
	{
		WP_DisruptorMainFire( ent );
	}
}

// BOWCASTER

static void WP_BowcasterAltFire( gentity_t *ent )
{
	int	damage	= BOWCASTER_DAMAGE;

	gentity_t *missile = CreateMissile( muzzle, forward, BOWCASTER_VELOCITY, 10000, ent, false);

	missile->classname = "bowcaster_proj";
	missile->s.weapon = WP_BOWCASTER;

	VectorSet( missile->r.maxs, BOWCASTER_SIZE, BOWCASTER_SIZE, BOWCASTER_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = MOD_BOWCASTER;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	missile->flags |= FL_BOUNCE;
	missile->bounceCount = 3;
}

static void WP_BowcasterMainFire( gentity_t *ent )
{
	int			damage	= BOWCASTER_DAMAGE, count;
	float		vel;
	vec3_t		angs, dir;
	gentity_t	*missile;
	int i;

	if (!ent->client)
	{
		count = 1;
	}
	else
	{
		count = ( level.time - ent->client->ps.weaponChargeTime ) / BOWCASTER_CHARGE_UNIT;
	}

	if ( count < 1 )
	{
		count = 1;
	}
	else if ( count > 5 )
	{
		count = 5;
	}

	if ( !(count & 1 ))
	{
		// if we aren't odd, knock us down a level
		count--;
	}

	//scale the damage down based on how many are about to be fired
	if (count <= 1)
	{
		damage = 50;
	}
	else if (count == 2)
	{
		damage = 45;
	}
	else if (count == 3)
	{
		damage = 40;
	}
	else if (count == 4)
	{
		damage = 35;
	}
	else
	{
		damage = 30;
	}

	for (i = 0; i < count; i++ )
	{
		// create a range of different velocities
		vel = BOWCASTER_VELOCITY * ( Q_flrand(-1.0f, 1.0f) * BOWCASTER_VEL_RANGE + 1.0f );

		vectoangles( forward, angs );

		// add some slop to the alt-fire direction
		angs[PITCH] += Q_flrand(-1.0f, 1.0f) * BOWCASTER_ALT_SPREAD * 0.2f;
		angs[YAW]	+= ((i+0.5f) * BOWCASTER_ALT_SPREAD - count * 0.5f * BOWCASTER_ALT_SPREAD );

		AngleVectors( angs, dir, nullptr, nullptr );

		missile = CreateMissile( muzzle, dir, vel, 10000, ent, true );

		missile->classname = "bowcaster_alt_proj";
		missile->s.weapon = WP_BOWCASTER;

		VectorSet( missile->r.maxs, BOWCASTER_SIZE, BOWCASTER_SIZE, BOWCASTER_SIZE );
		VectorScale( missile->r.maxs, -1, missile->r.mins );

		missile->damage = damage;
		missile->dflags = DAMAGE_DEATH_KNOCKBACK;
		missile->methodOfDeath = MOD_BOWCASTER;
		missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

		// we don't want it to bounce
		missile->bounceCount = 0;
	}
}

static void WP_FireBowcaster( gentity_t *ent, bool altFire )
{
	if ( altFire )
	{
		WP_BowcasterAltFire( ent );
	}
	else
	{
		WP_BowcasterMainFire( ent );
	}
}

// REPEATER

static void WP_RepeaterMainFire( gentity_t *ent, vec3_t dir )
{
	int	damage	= REPEATER_DAMAGE;

	gentity_t *missile = CreateMissile( muzzle, dir, REPEATER_VELOCITY, 10000, ent, false );

	missile->classname = "repeater_proj";
	missile->s.weapon = WP_REPEATER;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = MOD_REPEATER;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

static void WP_RepeaterAltFire( gentity_t *ent )
{
	int	damage	= REPEATER_ALT_DAMAGE;

	gentity_t *missile = CreateMissile( muzzle, forward, REPEATER_ALT_VELOCITY, 10000, ent, true );

	missile->classname = "repeater_alt_proj";
	missile->s.weapon = WP_REPEATER;

	VectorSet( missile->r.maxs, REPEATER_ALT_SIZE, REPEATER_ALT_SIZE, REPEATER_ALT_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );
	missile->s.pos.trType = TR_GRAVITY;
	missile->s.pos.trDelta[2] += 40.0f; //give a slight boost in the upward direction
	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = MOD_REPEATER_ALT;
	missile->splashMethodOfDeath = MOD_REPEATER_ALT_SPLASH;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;
	missile->splashDamage = REPEATER_ALT_SPLASH_DAMAGE;
	missile->splashRadius = REPEATER_ALT_SPLASH_RADIUS;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

static void WP_FireRepeater( gentity_t *ent, bool altFire )
{
	vec3_t	dir, angs;

	vectoangles( forward, angs );

	if ( altFire )
	{
		WP_RepeaterAltFire( ent );
	}
	else
	{
		// add some slop to the alt-fire direction
		angs[PITCH] += Q_flrand(-1.0f, 1.0f) * REPEATER_SPREAD;
		angs[YAW]	+= Q_flrand(-1.0f, 1.0f) * REPEATER_SPREAD;

		AngleVectors( angs, dir, nullptr, nullptr );

		WP_RepeaterMainFire( ent, dir );
	}
}

// DEMP2

static void WP_DEMP2_MainFire( gentity_t *ent )
{
	int	damage	= DEMP2_DAMAGE;

	gentity_t *missile = CreateMissile( muzzle, forward, DEMP2_VELOCITY, 10000, ent, false);

	missile->classname = "demp2_proj";
	missile->s.weapon = WP_DEMP2;

	VectorSet( missile->r.maxs, DEMP2_SIZE, DEMP2_SIZE, DEMP2_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );
	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = MOD_DEMP2;
	missile->clipmask = MASK_SHOT;

	// we don't want it to ever bounce
	missile->bounceCount = 0;
}

static gentity_t *ent_list[MAX_GENTITIES];

void DEMP2_AltRadiusDamage( gentity_t *ent )
{
	float		frac = ( level.time - ent->genericValue5 ) / 800.0f; // / 1600.0f; // synchronize with demp2 effect
	float		dist, radius, fact;
	gentity_t	*gent;
	int			iEntityList[MAX_GENTITIES];
	gentity_t	*entityList[MAX_GENTITIES];
	gentity_t	*myOwner = nullptr;
	int			numListedEntities, i, e;
	vec3_t		mins, maxs;
	vec3_t		v, dir;

	if (ent->r.ownerNum >= 0 &&
		ent->r.ownerNum < /*MAX_CLIENTS ... let npc's/shooters use it*/MAX_GENTITIES)
	{
		myOwner = &g_entities[ent->r.ownerNum];
	}

	if (!myOwner || !myOwner->inuse || !myOwner->client)
	{
		ent->think = G_FreeEntity;
		ent->nextthink = level.time;
		return;
	}

	frac *= frac * frac; // yes, this is completely ridiculous...but it causes the shell to grow slowly then "explode" at the end

	radius = frac * 200.0f; // 200 is max radius...the model is aprox. 100 units tall...the fx draw code mults. this by 2.

	fact = ent->count*0.6;

	if (fact < 1)
	{
		fact = 1;
	}

	radius *= fact;

	for ( i = 0 ; i < 3 ; i++ )
	{
		mins[i] = ent->r.currentOrigin[i] - radius;
		maxs[i] = ent->r.currentOrigin[i] + radius;
	}

	numListedEntities = trap->EntitiesInBox( mins, maxs, iEntityList, MAX_GENTITIES );

	i = 0;
	while (i < numListedEntities)
	{
		entityList[i] = &g_entities[iEntityList[i]];
		i++;
	}

	for ( e = 0 ; e < numListedEntities ; e++ )
	{
		gent = entityList[ e ];

		if ( !gent || !gent->takedamage || !gent->r.contents )
		{
			continue;
		}

		// find the distance from the edge of the bounding box
		for ( i = 0 ; i < 3 ; i++ )
		{
			if ( ent->r.currentOrigin[i] < gent->r.absmin[i] )
			{
				v[i] = gent->r.absmin[i] - ent->r.currentOrigin[i];
			}
			else if ( ent->r.currentOrigin[i] > gent->r.absmax[i] )
			{
				v[i] = ent->r.currentOrigin[i] - gent->r.absmax[i];
			}
			else
			{
				v[i] = 0;
			}
		}

		// shape is an ellipsoid, so cut vertical distance in half`
		v[2] *= 0.5f;

		dist = VectorLength( v );

		if ( dist >= radius )
		{
			// shockwave hasn't hit them yet
			continue;
		}

		if (dist+(16*ent->count) < ent->genericValue6)
		{
			// shockwave has already hit this thing...
			continue;
		}

		VectorCopy( gent->r.currentOrigin, v );
		VectorSubtract( v, ent->r.currentOrigin, dir);

		// push the center of mass higher than the origin so players get knocked into the air more
		dir[2] += 12;

		if (gent != myOwner)
		{
			G_Damage( gent, myOwner, myOwner, dir, ent->r.currentOrigin, ent->damage, DAMAGE_DEATH_KNOCKBACK, ent->splashMethodOfDeath );
			if ( gent->takedamage
				&& gent->client )
			{
				if ( gent->client->ps.electrifyTime < level.time )
				{//electrocution effect
					gent->client->ps.electrifyTime = level.time + Q_irand( 300, 800 );
				}
				if ( gent->client->ps.powerups[PW_CLOAKED] )
				{//disable cloak temporarily
					Jedi_Decloak( gent );
					gent->client->cloakToggleTime = level.time + Q_irand( 3000, 10000 );
				}
			}
		}
	}

	// store the last fraction so that next time around we can test against those things that fall between that last point and where the current shockwave edge is
	ent->genericValue6 = radius;

	if ( frac < 1.0f )
	{
		// shock is still happening so continue letting it expand
		ent->nextthink = level.time + 50;
	}
	else
	{ //don't just leave the entity around
		ent->think = G_FreeEntity;
		ent->nextthink = level.time;
	}
}

void DEMP2_AltDetonate( gentity_t *ent )
{
	gentity_t *efEnt;

	G_SetOrigin( ent, ent->r.currentOrigin );
	if (!ent->pos1[0] && !ent->pos1[1] && !ent->pos1[2])
	{ //don't play effect with a 0'd out directional vector
		ent->pos1[1] = 1;
	}
	//Let's just save ourself some bandwidth and play both the effect and sphere spawn in 1 event
	efEnt = G_PlayEffect( EFFECT_EXPLOSION_DEMP2ALT, ent->r.currentOrigin, ent->pos1 );

	if (efEnt)
	{
		efEnt->s.weapon = ent->count*2;
	}

	ent->genericValue5 = level.time;
	ent->genericValue6 = 0;
	ent->nextthink = level.time + 50;
	ent->think = DEMP2_AltRadiusDamage;
	ent->s.eType = ET_GENERAL; // make us a missile no longer
}

static void WP_DEMP2_AltFire( gentity_t *ent )
{
	int		damage	= DEMP2_ALT_DAMAGE;
	int		count, origcount;
	float	fact;
	vec3_t	start, end;
	trace_t	tr;
	gentity_t *missile;

	VectorCopy( muzzle, start );

	VectorMA( start, DEMP2_ALT_RANGE, forward, end );

	count = ( level.time - ent->client->ps.weaponChargeTime ) / DEMP2_CHARGE_UNIT;

	origcount = count;

	if ( count < 1 )
	{
		count = 1;
	}
	else if ( count > 3 )
	{
		count = 3;
	}

	fact = count*0.8;
	if (fact < 1)
	{
		fact = 1;
	}
	damage *= fact;

	if (!origcount)
	{ //this was just a tap-fire
		damage = 1;
	}

	trap->Trace( &tr, start, nullptr, nullptr, end, ent->s.number, MASK_SHOT, false, 0, 0);

	missile = G_Spawn();
	G_SetOrigin(missile, tr.endpos);
	//In SP the impact actually travels as a missile based on the trace fraction, but we're
	//just going to be instant. -rww

	VectorCopy( tr.plane.normal, missile->pos1 );

	missile->count = count;

	missile->classname = "demp2_alt_proj";
	missile->s.weapon = WP_DEMP2;

	missile->think = DEMP2_AltDetonate;
	missile->nextthink = level.time;

	missile->splashDamage = missile->damage = damage;
	missile->splashMethodOfDeath = missile->methodOfDeath = MOD_DEMP2;
	missile->splashRadius = DEMP2_ALT_SPLASHRADIUS;

	missile->r.ownerNum = ent->s.number;

	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to ever bounce
	missile->bounceCount = 0;
}

static void WP_FireDEMP2( gentity_t *ent, bool altFire )
{
	if ( altFire )
	{
		WP_DEMP2_AltFire( ent );
	}
	else
	{
		WP_DEMP2_MainFire( ent );
	}
}

// FLECHETTE

static void WP_FlechetteMainFire( gentity_t *ent )
{
	vec3_t		fwd, angs;
	gentity_t	*missile;
	int i;

	for (i = 0; i < FLECHETTE_SHOTS; i++ )
	{
		vectoangles( forward, angs );

		if (i != 0)
		{ //do nothing on the first shot, it will hit the crosshairs
			angs[PITCH] += Q_flrand(-1.0f, 1.0f) * FLECHETTE_SPREAD;
			angs[YAW]	+= Q_flrand(-1.0f, 1.0f) * FLECHETTE_SPREAD;
		}

		AngleVectors( angs, fwd, nullptr, nullptr );

		missile = CreateMissile( muzzle, fwd, FLECHETTE_VEL, 10000, ent, false);

		missile->classname = "flech_proj";
		missile->s.weapon = WP_FLECHETTE;

		VectorSet( missile->r.maxs, FLECHETTE_SIZE, FLECHETTE_SIZE, FLECHETTE_SIZE );
		VectorScale( missile->r.maxs, -1, missile->r.mins );

		missile->damage = FLECHETTE_DAMAGE;
		missile->dflags = DAMAGE_DEATH_KNOCKBACK;
		missile->methodOfDeath = MOD_FLECHETTE;
		missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

		// we don't want it to bounce forever
		missile->bounceCount = Q_irand(5,8);

		missile->flags |= FL_BOUNCE_SHRAPNEL;
	}
}

static void laserTrapExplode( gentity_t *self )
{
	vec3_t v;
	self->takedamage = false;

	if (self->activator)
	{
		G_RadiusDamage( self->r.currentOrigin, self->activator, self->splashDamage, self->splashRadius, self, self, MOD_TRIP_MINE_SPLASH/*MOD_LT_SPLASH*/ );
	}

	if (self->s.weapon != WP_FLECHETTE)
	{
		G_AddEvent( self, EV_MISSILE_MISS, 0);
	}

	VectorCopy(self->s.pos.trDelta, v);
	//Explode outward from the surface

	if (self->s.time == -2)
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
	}

	if (self->s.weapon == WP_FLECHETTE)
	{
		G_PlayEffect(EFFECT_EXPLOSION_FLECHETTE, self->r.currentOrigin, v);
	}
	else
	{
		G_PlayEffect(EFFECT_EXPLOSION_TRIPMINE, self->r.currentOrigin, v);
	}

	self->think = G_FreeEntity;
	self->nextthink = level.time;
}

void prox_mine_think( gentity_t *ent )
{
	int			count, i;
	bool	blow = false;

	// if it isn't time to auto-explode, do a small proximity check
	if ( ent->delay > level.time )
	{
		count = G_RadiusList( ent->r.currentOrigin, FLECHETTE_MINE_RADIUS_CHECK, ent, true, ent_list );

		for ( i = 0; i < count; i++ )
		{
			if ( ent_list[i]->client && ent_list[i]->health > 0 && ent->activator && ent_list[i]->s.number != ent->activator->s.number )
			{
				blow = true;
				break;
			}
		}
	}
	else
	{
		// well, we must die now
		blow = true;
	}

	if ( blow )
	{
		ent->think = laserTrapExplode;
		ent->nextthink = level.time + 200;
	}
	else
	{
		// we probably don't need to do this thinking logic very often...maybe this is fast enough?
		ent->nextthink = level.time + 500;
	}
}

static void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs )
{
	//make sure our start point isn't on the other side of a wall
	trace_t	tr;
	vec3_t	entMins;
	vec3_t	entMaxs;

	VectorAdd( ent->r.currentOrigin, ent->r.mins, entMins );
	VectorAdd( ent->r.currentOrigin, ent->r.maxs, entMaxs );

	if ( G_BoxInBounds( start, mins, maxs, entMins, entMaxs ) )
	{
		return;
	}

	if ( !ent->client )
	{
		return;
	}

	trap->Trace( &tr, ent->client->ps.origin, mins, maxs, start, ent->s.number, MASK_SOLID|CONTENTS_SHOTCLIP, false, 0, 0 );

	if ( tr.startsolid || tr.allsolid )
	{
		return;
	}

	if ( tr.fraction < 1.0f )
	{
		VectorCopy( tr.endpos, start );
	}
}

void WP_ExplosiveDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	laserTrapExplode(self);
}

void WP_flechette_alt_blow( gentity_t *ent )
{
	ent->s.pos.trDelta[0] = 1;
	ent->s.pos.trDelta[1] = 0;
	ent->s.pos.trDelta[2] = 0;

	laserTrapExplode(ent);
}

static void WP_CreateFlechetteBouncyThing( vec3_t start, vec3_t fwd, gentity_t *self )
{
	gentity_t	*missile = CreateMissile( start, fwd, 700 + Q_flrand(0.0f, 1.0f) * 700, 1500 + Q_flrand(0.0f, 1.0f) * 2000, self, true );

	missile->think = WP_flechette_alt_blow;

	missile->activator = self;

	missile->s.weapon = WP_FLECHETTE;
	missile->classname = "flech_alt";
	missile->mass = 4;

	// How 'bout we give this thing a size...
	VectorSet( missile->r.mins, -3.0f, -3.0f, -3.0f );
	VectorSet( missile->r.maxs, 3.0f, 3.0f, 3.0f );
	missile->clipmask = MASK_SHOT;

	missile->touch = touch_NULL;

	// normal ones bounce, alt ones explode on impact
	missile->s.pos.trType = TR_GRAVITY;

	missile->flags |= FL_BOUNCE_HALF;
	missile->s.eFlags |= EF_ALT_FIRING;

	missile->bounceCount = 50;

	missile->damage = FLECHETTE_ALT_DAMAGE;
	missile->dflags = 0;
	missile->splashDamage = FLECHETTE_ALT_SPLASH_DAM;
	missile->splashRadius = FLECHETTE_ALT_SPLASH_RAD;

	missile->r.svFlags = SVF_USE_CURRENT_ORIGIN;

	missile->methodOfDeath = MOD_FLECHETTE_ALT_SPLASH;
	missile->splashMethodOfDeath = MOD_FLECHETTE_ALT_SPLASH;

	VectorCopy( start, missile->pos2 );
}

static void WP_FlechetteAltFire( gentity_t *self )
{
	vec3_t 	dir, fwd, start, angs;
	int i;

	vectoangles( forward, angs );
	VectorCopy( muzzle, start );

	WP_TraceSetStart( self, start, vec3_origin, vec3_origin );//make sure our start point isn't on the other side of a wall

	for ( i = 0; i < 2; i++ )
	{
		VectorCopy( angs, dir );

		dir[PITCH] -= Q_flrand(0.0f, 1.0f) * 4 + 8; // make it fly upwards
		dir[YAW] += Q_flrand(-1.0f, 1.0f) * 2;
		AngleVectors( dir, fwd, nullptr, nullptr );

		WP_CreateFlechetteBouncyThing( start, fwd, self );
	}
}

static void WP_FireFlechette( gentity_t *ent, bool altFire )
{
	if ( altFire )
	{
		//WP_FlechetteProxMine( ent );
		WP_FlechetteAltFire(ent);
	}
	else
	{
		WP_FlechetteMainFire( ent );
	}
}

// ROCKET LAUNCHER

void RocketDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	self->die = 0;
	self->r.contents = 0;

	G_ExplodeMissile( self );

	self->think = G_FreeEntity;
	self->nextthink = level.time;
}

void rocketThink( gentity_t *ent )
{
	vec3_t newdir, targetdir,
			rup={0,0,1}, right;
	vec3_t	org;
	float dot, dot2, dis;
	int i;
	float vel = (ent->spawnflags&1)?ent->speed:ROCKET_VELOCITY;

	if ( ent->genericValue1 && ent->genericValue1 < level.time )
	{//time's up, we're done, remove us
		if ( ent->genericValue2 )
		{//explode when die
			RocketDie( ent, &g_entities[ent->r.ownerNum], &g_entities[ent->r.ownerNum], 0, MOD_UNKNOWN );
		}
		else
		{//just remove when die
			G_FreeEntity( ent );
		}
		return;
	}
	if ( !ent->enemy
		|| !ent->enemy->client
		|| ent->enemy->health <= 0
		|| ent->enemy->client->ps.powerups[PW_CLOAKED] )
	{//no enemy or enemy not a client or enemy dead or enemy cloaked
		if ( !ent->genericValue1  )
		{//doesn't have its own self-kill time
			ent->nextthink = level.time + 10000;
			ent->think = G_FreeEntity;
		}
		return;
	}

	if ( ent->enemy && ent->enemy->inuse )
	{
		float newDirMult = ent->angle?ent->angle*2.0f:1.0f;
		float oldDirMult = ent->angle?(1.0f-ent->angle)*2.0f:1.0f;

		VectorCopy( ent->enemy->r.currentOrigin, org );
		org[2] += (ent->enemy->r.mins[2] + ent->enemy->r.maxs[2]) * 0.5f;

		VectorSubtract( org, ent->r.currentOrigin, targetdir );
		VectorNormalize( targetdir );

		// Now the rocket can't do a 180 in space, so we'll limit the turn to about 45 degrees.
		dot = DotProduct( targetdir, ent->movedir );
		if ( (ent->spawnflags&1) )
		{//vehicle rocket
			if ( ent->radius > -1.0f )
			{//can lose the lock if DotProduct drops below this number
				if ( dot < ent->radius )
				{//lost the lock!!!
					//HMM... maybe can re-lock on if they come in front again?
					/*
					//OR: should it stop trying to lock altogether?
					if ( ent->genericValue1 )
					{//have a timelimit, set next think to that
						ent->nextthink = ent->genericValue1;
						if ( ent->genericValue2 )
						{//explode when die
							ent->think = G_ExplodeMissile;
						}
						else
						{
							ent->think = G_FreeEntity;
						}
					}
					else
					{
						ent->think = nullptr;
						ent->nextthink = -1;
					}
					*/
					return;
				}
			}
		}

		// a dot of 1.0 means right-on-target.
		if ( dot < 0.0f )
		{
			// Go in the direction opposite, start a 180.
			CrossProduct( ent->movedir, rup, right );
			dot2 = DotProduct( targetdir, right );

			if ( dot2 > 0 )
			{
				// Turn 45 degrees right.
				VectorMA( ent->movedir, 0.4f*newDirMult, right, newdir );
			}
			else
			{
				// Turn 45 degrees left.
				VectorMA( ent->movedir, -0.4f*newDirMult, right, newdir );
			}

			// Yeah we've adjusted horizontally, but let's split the difference vertically, so we kinda try to move towards it.
			newdir[2] = ( (targetdir[2]*newDirMult) + (ent->movedir[2]*oldDirMult) ) * 0.5;

			// let's also slow down a lot
			vel *= 0.5f;
		}
		else if ( dot < 0.70f )
		{
			// Still a bit off, so we turn a bit softer
			VectorMA( ent->movedir, 0.5f*newDirMult, targetdir, newdir );
		}
		else
		{
			// getting close, so turn a bit harder
			VectorMA( ent->movedir, 0.9f*newDirMult, targetdir, newdir );
		}

		// add crazy drunkenness
		for (i = 0; i < 3; i++ )
		{
			newdir[i] += Q_flrand(-1.0f, 1.0f) * ent->random * 0.25f;
		}

		// decay the randomness
		ent->random *= 0.9f;

		if ( ent->enemy->client
			&& ent->enemy->client->ps.groundEntityNum != ENTITYNUM_NONE )
		{//tracking a client who's on the ground, aim at the floor...?
			// Try to crash into the ground if we get close enough to do splash damage
			dis = Distance( ent->r.currentOrigin, org );

			if ( dis < 128 )
			{
				// the closer we get, the more we push the rocket down, heh heh.
				newdir[2] -= (1.0f - (dis / 128.0f)) * 0.6f;
			}
		}

		VectorNormalize( newdir );

		VectorScale( newdir, vel * 0.5f, ent->s.pos.trDelta );
		VectorCopy( newdir, ent->movedir );
		SnapVector( ent->s.pos.trDelta );			// save net bandwidth
		VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
		ent->s.pos.trTime = level.time;
	}

	ent->nextthink = level.time + ROCKET_ALT_THINK_TIME;	// Nothing at all spectacular happened, continue.
	return;
}

static void WP_FireRocket( gentity_t *ent, bool altFire )
{
	int	damage	= ROCKET_DAMAGE;
	int	vel = ROCKET_VELOCITY;
	int dif = 0;
	float rTime;
	gentity_t *missile;

	if ( altFire )
	{
		vel *= 0.5f;
	}

	missile = CreateMissile( muzzle, forward, vel, 30000, ent, altFire );

	if (ent->client && ent->client->ps.rocketLockIndex != ENTITYNUM_NONE)
	{
		float lockTimeInterval = 1200.0f/16.0f;
		rTime = ent->client->ps.rocketLockTime;

		if (rTime == -1)
		{
			rTime = ent->client->ps.rocketLastValidTime;
		}
		dif = ( level.time - rTime ) / lockTimeInterval;

		if (dif < 0)
		{
			dif = 0;
		}

		//It's 10 even though it locks client-side at 8, because we want them to have a sturdy lock first, and because there's a slight difference in time between server and client
		if ( dif >= 10 && rTime != -1 )
		{
			missile->enemy = &g_entities[ent->client->ps.rocketLockIndex];

			if (missile->enemy && missile->enemy->client && missile->enemy->health > 0 && !OnSameTeam(ent, missile->enemy))
			{ //if enemy became invalid, died, or is on the same team, then don't seek it
				missile->angle = 0.5f;
				missile->think = rocketThink;
				missile->nextthink = level.time + ROCKET_ALT_THINK_TIME;
			}
		}

		ent->client->ps.rocketLockIndex = ENTITYNUM_NONE;
		ent->client->ps.rocketLockTime = 0;
		ent->client->ps.rocketTargetTime = 0;
	}

	missile->classname = "rocket_proj";
	missile->s.weapon = WP_ROCKET_LAUNCHER;

	// Make it easier to hit things
	VectorSet( missile->r.maxs, ROCKET_SIZE, ROCKET_SIZE, ROCKET_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	if (altFire)
	{
		missile->methodOfDeath = MOD_ROCKET_HOMING;
		missile->splashMethodOfDeath = MOD_ROCKET_HOMING_SPLASH;
	}
	else
	{
		missile->methodOfDeath = MOD_ROCKET;
		missile->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	}
	missile->health = 10;
	missile->takedamage = true;
	missile->r.contents = MASK_SHOT;
	missile->die = RocketDie;
	missile->clipmask = MASK_SHOT;
	missile->splashDamage = ROCKET_SPLASH_DAMAGE;
	missile->splashRadius = ROCKET_SPLASH_RADIUS;

	// we don't want it to ever bounce
	missile->bounceCount = 0;
}

// THERMAL DETONATOR

#define TD_DAMAGE			70 //only do 70 on a direct impact
#define TD_SPLASH_RAD		128
#define TD_SPLASH_DAM		90
#define TD_VELOCITY			900
#define TD_MIN_CHARGE		0.15f
#define TD_TIME				3000//6000
#define TD_ALT_TIME			3000

#define TD_ALT_DAMAGE		60//100
#define TD_ALT_SPLASH_RAD	128
#define TD_ALT_SPLASH_DAM	50//90
#define TD_ALT_VELOCITY		600
#define TD_ALT_MIN_CHARGE	0.15f
#define TD_ALT_TIME			3000

void thermalThinkStandard( gentity_t *ent );

static void thermalDetonatorExplode( gentity_t *ent )
{
	if ( !ent->count )
	{
		G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/thermal/warning.wav" ) );
		ent->count = 1;
		ent->genericValue5 = level.time + 500;
		ent->think = thermalThinkStandard;
		ent->nextthink = level.time;
		ent->r.svFlags |= SVF_BROADCAST;//so everyone hears/sees the explosion?
	}
	else
	{
		vec3_t	origin;
		vec3_t	dir={0,0,1};

		BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
		origin[2] += 8;
		SnapVector( origin );
		G_SetOrigin( ent, origin );

		ent->s.eType = ET_GENERAL;
		G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );
		ent->freeAfterEvent = true;

		if (G_RadiusDamage( ent->r.currentOrigin, ent->parent,  ent->splashDamage, ent->splashRadius,
				ent, ent, ent->splashMethodOfDeath))
		{
			g_entities[ent->r.ownerNum].client->accuracy_hits++;
		}

		trap->LinkEntity( (sharedEntity_t *)ent );
	}
}

void thermalThinkStandard(gentity_t *ent)
{
	if (ent->genericValue5 < level.time)
	{
		ent->think = thermalDetonatorExplode;
		ent->nextthink = level.time;
		return;
	}

	G_RunObject(ent);
	ent->nextthink = level.time;
}

gentity_t *WP_FireThermalDetonator( gentity_t *ent, bool altFire )
{
	gentity_t	*bolt;
	vec3_t		dir, start;
	float chargeAmount = 1.0f; // default of full charge

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	bolt = G_Spawn();

	bolt->physicsObject = true;

	bolt->classname = "thermal_detonator";
	bolt->think = thermalThinkStandard;
	bolt->nextthink = level.time;
	bolt->touch = touch_NULL;

	// How 'bout we give this thing a size...
	VectorSet( bolt->r.mins, -3.0f, -3.0f, -3.0f );
	VectorSet( bolt->r.maxs, 3.0f, 3.0f, 3.0f );
	bolt->clipmask = MASK_SHOT;

	W_TraceSetStart( ent, start, bolt->r.mins, bolt->r.maxs );//make sure our start point isn't on the other side of a wall

	if ( ent->client )
	{
		chargeAmount = level.time - ent->client->ps.weaponChargeTime;
	}

	// get charge amount
	chargeAmount = chargeAmount / (float)TD_VELOCITY;

	if ( chargeAmount > 1.0f )
	{
		chargeAmount = 1.0f;
	}
	else if ( chargeAmount < TD_MIN_CHARGE )
	{
		chargeAmount = TD_MIN_CHARGE;
	}

	// normal ones bounce, alt ones explode on impact
	bolt->genericValue5 = level.time + TD_TIME; // How long 'til she blows
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale( dir, TD_VELOCITY * chargeAmount, bolt->s.pos.trDelta );

	if ( ent->health >= 0 )
	{
		bolt->s.pos.trDelta[2] += 120;
	}

	if ( !altFire )
	{
		bolt->flags |= FL_BOUNCE_HALF;
	}

	bolt->s.loopSound = G_SoundIndex( "sound/weapons/thermal/thermloop.wav" );
	bolt->s.loopIsSoundset = false;

	bolt->damage = TD_DAMAGE;
	bolt->dflags = 0;
	bolt->splashDamage = TD_SPLASH_DAM;
	bolt->splashRadius = TD_SPLASH_RAD;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_THERMAL;

	bolt->methodOfDeath = MOD_THERMAL;
	bolt->splashMethodOfDeath = MOD_THERMAL_SPLASH;

	bolt->s.pos.trTime = level.time;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );

	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	VectorCopy( start, bolt->pos2 );

	bolt->bounceCount = -5;

	return bolt;
}

gentity_t *WP_DropThermal( gentity_t *ent )
{
	AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	return (WP_FireThermalDetonator( ent, false ));
}

bool WP_LobFire( gentity_t *self, vec3_t start, vec3_t target, vec3_t mins, vec3_t maxs, int clipmask,
				vec3_t velocity, bool tracePath, int ignoreEntNum, int enemyNum,
				float minSpeed, float maxSpeed, float idealSpeed, bool mustHit )
{ //for the galak mech NPC
	float	targetDist, shotSpeed, speedInc = 100, travelTime, impactDist, bestImpactDist = Q3_INFINITE;//fireSpeed,
	vec3_t	targetDir = { 0 }, shotVel = { 0 }, failCase = { 0 };
	trace_t	trace;
	trajectory_t	tr;
	bool	blocked;
	int		elapsedTime, skipNum, timeStep = 500, hitCount = 0, maxHits = 7;
	vec3_t	lastPos = { 0 }, testPos = { 0 };
	gentity_t	*traceEnt;

	if ( !idealSpeed )
	{
		idealSpeed = 300;
	}
	else if ( idealSpeed < speedInc )
	{
		idealSpeed = speedInc;
	}
	shotSpeed = idealSpeed;
	skipNum = (idealSpeed-speedInc)/speedInc;
	if ( !minSpeed )
	{
		minSpeed = 100;
	}
	if ( !maxSpeed )
	{
		maxSpeed = 900;
	}
	while ( hitCount < maxHits )
	{
		VectorSubtract( target, start, targetDir );
		targetDist = VectorNormalize( targetDir );

		VectorScale( targetDir, shotSpeed, shotVel );
		travelTime = targetDist/shotSpeed;
		shotVel[2] += travelTime * 0.5 * g_gravity.value;

		if ( !hitCount )
		{//save the first (ideal) one as the failCase (fallback value)
			if ( !mustHit )
			{//default is fine as a return value
				VectorCopy( shotVel, failCase );
			}
		}

		if ( tracePath )
		{//do a rough trace of the path
			blocked = false;

			VectorCopy( start, tr.trBase );
			VectorCopy( shotVel, tr.trDelta );
			tr.trType = TR_GRAVITY;
			tr.trTime = level.time;
			travelTime *= 1000.0f;
			VectorCopy( start, lastPos );

			//This may be kind of wasteful, especially on long throws... use larger steps?  Divide the travelTime into a certain hard number of slices?  Trace just to apex and down?
			for ( elapsedTime = timeStep; elapsedTime < floor(travelTime)+timeStep; elapsedTime += timeStep )
			{
				if ( (float)elapsedTime > travelTime )
				{//cap it
					elapsedTime = floor( travelTime );
				}
				BG_EvaluateTrajectory( &tr, level.time + elapsedTime, testPos );
				trap->Trace( &trace, lastPos, mins, maxs, testPos, ignoreEntNum, clipmask, false, 0, 0 );

				if ( trace.allsolid || trace.startsolid )
				{
					blocked = true;
					break;
				}
				if ( trace.fraction < 1.0f )
				{//hit something
					if ( trace.entityNum == enemyNum )
					{//hit the enemy, that's perfect!
						break;
					}
					else if ( trace.plane.normal[2] > 0.7 && DistanceSquared( trace.endpos, target ) < 4096 )//hit within 64 of desired location, should be okay
					{//close enough!
						break;
					}
					else
					{//FIXME: maybe find the extents of this brush and go above or below it on next try somehow?
						impactDist = DistanceSquared( trace.endpos, target );
						if ( impactDist < bestImpactDist )
						{
							bestImpactDist = impactDist;
							VectorCopy( shotVel, failCase );
						}
						blocked = true;
						//see if we should store this as the failCase
						if ( trace.entityNum < ENTITYNUM_WORLD )
						{//hit an ent
							traceEnt = &g_entities[trace.entityNum];
							if ( traceEnt && traceEnt->takedamage && !OnSameTeam( self, traceEnt ) )
							{//hit something breakable, so that's okay
								//we haven't found a clear shot yet so use this as the failcase
								VectorCopy( shotVel, failCase );
							}
						}
						break;
					}
				}
				if ( elapsedTime == floor( travelTime ) )
				{//reached end, all clear
					break;
				}
				else
				{
					//all clear, try next slice
					VectorCopy( testPos, lastPos );
				}
			}
			if ( blocked )
			{//hit something, adjust speed (which will change arc)
				hitCount++;
				shotSpeed = idealSpeed + ((hitCount-skipNum) * speedInc);//from min to max (skipping ideal)
				if ( hitCount >= skipNum )
				{//skip ideal since that was the first value we tested
					shotSpeed += speedInc;
				}
			}
			else
			{//made it!
				break;
			}
		}
		else
		{//no need to check the path, go with first calc
			break;
		}
	}

	if ( hitCount >= maxHits )
	{//NOTE: worst case scenario, use the one that impacted closest to the target (or just use the first try...?)
		VectorCopy( failCase, velocity );
		return false;
	}
	VectorCopy( shotVel, velocity );
	return true;
}

// LASER TRAP / TRIP MINE

#define LT_DAMAGE			100
#define LT_SPLASH_RAD		256.0f
#define LT_SPLASH_DAM		105
#define LT_VELOCITY			900.0f
#define LT_SIZE				1.5f
#define LT_ALT_TIME			2000
#define	LT_ACTIVATION_DELAY	1000
#define	LT_DELAY_TIME		50

void laserTrapDelayedExplode( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	self->enemy = attacker;
	self->think = laserTrapExplode;
	self->nextthink = level.time + FRAMETIME;
	self->takedamage = false;
	if ( attacker && attacker->s.number < MAX_CLIENTS )
	{
		//less damage when shot by player
		self->splashDamage /= 3;
		self->splashRadius /= 3;
	}
}

void touchLaserTrap( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	if (other && other->s.number < ENTITYNUM_WORLD)
	{ //just explode if we hit any entity. This way we don't have things happening like tripmines floating
	  //in the air after getting stuck to a moving door
		if ( ent->activator != other )
		{
			ent->touch = 0;
			ent->nextthink = level.time + FRAMETIME;
			ent->think = laserTrapExplode;
			VectorCopy(trace->plane.normal, ent->s.pos.trDelta);
		}
	}
	else
	{
		ent->touch = 0;
		if (trace->entityNum != ENTITYNUM_NONE)
		{
			ent->enemy = &g_entities[trace->entityNum];
		}
		laserTrapStick(ent, trace->endpos, trace->plane.normal);
	}
}

void proxMineThink(gentity_t *ent)
{
	int i = 0;
	gentity_t *cl;
	gentity_t *owner = nullptr;

	if (ent->r.ownerNum < ENTITYNUM_WORLD)
	{
		owner = &g_entities[ent->r.ownerNum];
	}

	ent->nextthink = level.time;

	if (ent->genericValue15 < level.time ||
		!owner ||
		!owner->inuse ||
		!owner->client ||
		owner->client->pers.connected != CON_CONNECTED)
	{ //time to die!
		ent->think = laserTrapExplode;
		return;
	}

	while (i < MAX_CLIENTS)
	{ //eh, just check for clients, don't care about anyone else...
		cl = &g_entities[i];

		if (cl->inuse && cl->client && cl->client->pers.connected == CON_CONNECTED &&
			owner != cl && cl->client->sess.sessionTeam != TEAM_SPECTATOR &&
			cl->client->tempSpectate < level.time && cl->health > 0)
		{
			if (!OnSameTeam(owner, cl) || g_friendlyFire.integer)
			{ //not on the same team, or friendly fire is enabled
				vec3_t v;

				VectorSubtract(ent->r.currentOrigin, cl->client->ps.origin, v);
				if (VectorLength(v) < (ent->splashRadius/2.0f))
				{
					ent->think = laserTrapExplode;
					return;
				}
			}
		}
		i++;
	}
}

void laserTrapThink ( gentity_t *ent )
{
	gentity_t	*traceEnt;
	vec3_t		end;
	trace_t		tr;

	//just relink it every think
	trap->LinkEntity((sharedEntity_t *)ent);

	//turn on the beam effect
	if ( !(ent->s.eFlags&EF_FIRING) )
	{//arm me
		G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/laser_trap/warning.wav" ) );
		ent->s.eFlags |= EF_FIRING;
	}
	ent->think = laserTrapThink;
	ent->nextthink = level.time + FRAMETIME;

	// Find the main impact point
	VectorMA ( ent->s.pos.trBase, 1024, ent->movedir, end );
	trap->Trace ( &tr, ent->r.currentOrigin, nullptr, nullptr, end, ent->s.number, MASK_SHOT, false, 0, 0);

	traceEnt = &g_entities[ tr.entityNum ];

	ent->s.time = -1; //let all clients know to draw a beam from this guy

	if ( traceEnt->client || tr.startsolid )
	{
		//go boom
		ent->touch = 0;
		ent->nextthink = level.time + LT_DELAY_TIME;
		ent->think = laserTrapExplode;
	}
}

void laserTrapStick( gentity_t *ent, vec3_t endpos, vec3_t normal )
{
	G_SetOrigin( ent, endpos );
	VectorCopy( normal, ent->pos1 );

	VectorClear( ent->s.apos.trDelta );
	// This will orient the object to face in the direction of the normal
	VectorCopy( normal, ent->s.pos.trDelta );
	//VectorScale( normal, -1, ent->s.pos.trDelta );
	ent->s.pos.trTime = level.time;

	//This does nothing, cg_missile makes assumptions about direction of travel controlling angles
	vectoangles( normal, ent->s.apos.trBase );
	VectorClear( ent->s.apos.trDelta );
	ent->s.apos.trType = TR_STATIONARY;
	VectorCopy( ent->s.apos.trBase, ent->s.angles );
	VectorCopy( ent->s.angles, ent->r.currentAngles );

	G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/laser_trap/stick.wav" ) );
	if ( ent->count )
	{//a tripwire
		//add draw line flag
		VectorCopy( normal, ent->movedir );
		ent->think = laserTrapThink;
		ent->nextthink = level.time + LT_ACTIVATION_DELAY;//delay the activation
		ent->touch = touch_NULL;
		//make it shootable
		ent->takedamage = true;
		ent->health = 5;
		ent->die = laserTrapDelayedExplode;

		//shove the box through the wall
		VectorSet( ent->r.mins, -LT_SIZE*2, -LT_SIZE*2, -LT_SIZE*2 );
		VectorSet( ent->r.maxs, LT_SIZE*2, LT_SIZE*2, LT_SIZE*2 );

		//so that the owner can blow it up with projectiles
		ent->r.svFlags |= SVF_OWNERNOTSHARED;
	}
	else
	{
		ent->touch = touchLaserTrap;
		ent->think = proxMineThink;//laserTrapExplode;
		ent->genericValue15 = level.time + 30000; //auto-explode after 30 seconds.
		ent->nextthink = level.time + LT_ALT_TIME; // How long 'til she blows

		//make it shootable
		ent->takedamage = true;
		ent->health = 5;
		ent->die = laserTrapDelayedExplode;

		//shove the box through the wall
		VectorSet( ent->r.mins, -LT_SIZE*2, -LT_SIZE*2, -LT_SIZE*2 );
		VectorSet( ent->r.maxs, LT_SIZE*2, LT_SIZE*2, LT_SIZE*2 );

		//so that the owner can blow it up with projectiles
		ent->r.svFlags |= SVF_OWNERNOTSHARED;

		if ( !(ent->s.eFlags&EF_FIRING) )
		{//arm me
			G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/laser_trap/warning.wav" ) );
			ent->s.eFlags |= EF_FIRING;
			ent->s.time = -1;
			ent->s.bolt2 = 1;
		}
	}
}

void TrapThink(gentity_t *ent)
{ //laser trap think
	ent->nextthink = level.time + 50;
	G_RunObject(ent);
}

void CreateLaserTrap( gentity_t *laserTrap, vec3_t start, gentity_t *owner )
{ //create a laser trap entity
	laserTrap->classname = "laserTrap";
	laserTrap->flags |= FL_BOUNCE_HALF;
	laserTrap->s.eFlags |= EF_MISSILE_STICK;
	laserTrap->splashDamage = LT_SPLASH_DAM;
	laserTrap->splashRadius = LT_SPLASH_RAD;
	laserTrap->damage = LT_DAMAGE;
	laserTrap->methodOfDeath = MOD_TRIP_MINE_SPLASH;
	laserTrap->splashMethodOfDeath = MOD_TRIP_MINE_SPLASH;
	laserTrap->s.eType = ET_GENERAL;
	laserTrap->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	laserTrap->s.weapon = WP_TRIP_MINE;
	laserTrap->s.pos.trType = TR_GRAVITY;
	laserTrap->r.contents = MASK_SHOT;
	laserTrap->parent = owner;
	laserTrap->activator = owner;
	laserTrap->r.ownerNum = owner->s.number;
	VectorSet( laserTrap->r.mins, -LT_SIZE, -LT_SIZE, -LT_SIZE );
	VectorSet( laserTrap->r.maxs, LT_SIZE, LT_SIZE, LT_SIZE );
	laserTrap->clipmask = MASK_SHOT;
	laserTrap->s.solid = 2;
	laserTrap->s.modelindex = G_ModelIndex( "models/weapons/laser_trap/laser_trap_w.glm" );
	laserTrap->s.modelGhoul2 = 1;
	laserTrap->s.g2radius = 40;

	laserTrap->s.genericenemyindex = owner->s.number+MAX_GENTITIES;

	laserTrap->health = 1;

	laserTrap->s.time = 0;

	laserTrap->s.pos.trTime = level.time;		// move a bit on the very first frame
	VectorCopy( start, laserTrap->s.pos.trBase );
	SnapVector( laserTrap->s.pos.trBase );			// save net bandwidth

	SnapVector( laserTrap->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, laserTrap->r.currentOrigin);

	laserTrap->s.apos.trType = TR_GRAVITY;
	laserTrap->s.apos.trTime = level.time;
	laserTrap->s.apos.trBase[YAW] = rand()%360;
	laserTrap->s.apos.trBase[PITCH] = rand()%360;
	laserTrap->s.apos.trBase[ROLL] = rand()%360;

	if (rand()%10 < 5)
	{
		laserTrap->s.apos.trBase[YAW] = -laserTrap->s.apos.trBase[YAW];
	}

	VectorCopy( start, laserTrap->pos2 );
	laserTrap->touch = touchLaserTrap;
	laserTrap->think = TrapThink;
	laserTrap->nextthink = level.time + 50;
}

void WP_PlaceLaserTrap( gentity_t *ent, bool alt_fire )
{
	gentity_t	*laserTrap;
	gentity_t	*found = nullptr;
	vec3_t		dir, start;
	int			trapcount = 0;
	int			foundLaserTraps[MAX_GENTITIES];
	int			trapcount_org;
	int			lowestTimeStamp;
	int			removeMe;
	int			i;

	foundLaserTraps[0] = ENTITYNUM_NONE;

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	laserTrap = G_Spawn();

	//limit to 10 placed at any one time
	//see how many there are now
	while ( (found = G_Find( found, FOFS(classname), "laserTrap" )) != nullptr )
	{
		if ( found->parent != ent )
		{
			continue;
		}
		foundLaserTraps[trapcount++] = found->s.number;
	}
	//now remove first ones we find until there are only 9 left
	found = nullptr;
	trapcount_org = trapcount;
	lowestTimeStamp = level.time;
	while ( trapcount > 9 )
	{
		removeMe = -1;
		for ( i = 0; i < trapcount_org; i++ )
		{
			if ( foundLaserTraps[i] == ENTITYNUM_NONE )
			{
				continue;
			}
			found = &g_entities[foundLaserTraps[i]];
			if ( laserTrap && found->setTime < lowestTimeStamp )
			{
				removeMe = i;
				lowestTimeStamp = found->setTime;
			}
		}
		if ( removeMe != -1 )
		{

			G_FreeEntity( &g_entities[foundLaserTraps[removeMe]] );

			foundLaserTraps[removeMe] = ENTITYNUM_NONE;
			trapcount--;
		}
		else
		{
			break;
		}
	}

	//now make the new one
	CreateLaserTrap( laserTrap, start, ent );

	//set player-created-specific fields
	laserTrap->setTime = level.time;//remember when we placed it

	if (!alt_fire)
	{//tripwire
		laserTrap->count = 1;
	}

	//move it
	laserTrap->s.pos.trType = TR_GRAVITY;

	if (alt_fire)
	{
		VectorScale( dir, 512, laserTrap->s.pos.trDelta );
	}
	else
	{
		VectorScale( dir, 256, laserTrap->s.pos.trDelta );
	}

	trap->LinkEntity((sharedEntity_t *)laserTrap);
}

// DET PACK

void VectorNPos(vec3_t in, vec3_t out)
{
	if (in[0] < 0) { out[0] = -in[0]; } else { out[0] = in[0]; }
	if (in[1] < 0) { out[1] = -in[1]; } else { out[1] = in[1]; }
	if (in[2] < 0) { out[2] = -in[2]; } else { out[2] = in[2]; }
}

void DetPackBlow(gentity_t *self)
{
	vec3_t v;

	self->pain = 0;
	self->die = 0;
	self->takedamage = false;

	if ( self->target_ent )
	{//we were attached to something, do *direct* damage to it!
		G_Damage( self->target_ent, self, &g_entities[self->r.ownerNum], v, self->r.currentOrigin, self->damage, 0, MOD_DET_PACK_SPLASH );
	}
	G_RadiusDamage( self->r.currentOrigin, self->parent, self->splashDamage, self->splashRadius, self, self, MOD_DET_PACK_SPLASH );
	v[0] = 0;
	v[1] = 0;
	v[2] = 1;

	if (self->count == -1)
	{
		VectorCopy(self->pos2, v);
	}

	G_PlayEffect(EFFECT_EXPLOSION_DETPACK, self->r.currentOrigin, v);

	self->think = G_FreeEntity;
	self->nextthink = level.time;
}

void charge_stick (gentity_t *self, gentity_t *other, trace_t *trace)
{
	gentity_t	*tent;

	if ( other
		&& (other->flags&FL_BBRUSH)
		&& other->s.pos.trType == TR_STATIONARY
		&& other->s.apos.trType == TR_STATIONARY )
	{//a perfectly still breakable brush, let us attach directly to it!
		self->target_ent = other;//remember them when we blow up
	}
	else if ( other
		&& other->s.number < ENTITYNUM_WORLD
		&& other->s.eType == ET_MOVER
		&& trace->plane.normal[2] > 0 )
	{//stick to it?
		self->s.groundEntityNum = other->s.number;
	}
	else if (other && other->s.number < ENTITYNUM_WORLD &&
		(other->client || !other->s.weapon))
	{ //hit another entity that is not stickable, "bounce" off
		vec3_t vNor, tN;

		VectorCopy(trace->plane.normal, vNor);
		VectorNormalize(vNor);
		VectorNPos(self->s.pos.trDelta, tN);
		self->s.pos.trDelta[0] += vNor[0]*(tN[0]*(((float)Q_irand(1, 10))*0.1));
		self->s.pos.trDelta[1] += vNor[1]*(tN[1]*(((float)Q_irand(1, 10))*0.1));
		self->s.pos.trDelta[2] += vNor[2]*(tN[2]*(((float)Q_irand(1, 10))*0.1));

		vectoangles(vNor, self->s.angles);
		vectoangles(vNor, self->s.apos.trBase);
		self->touch = charge_stick;
		return;
	}
	else if (other && other->s.number < ENTITYNUM_WORLD)
	{ //hit an entity that we just want to explode on (probably another projectile or something)
		vec3_t v;

		self->touch = 0;
		self->think = 0;
		self->nextthink = 0;

		self->takedamage = false;

		VectorClear(self->s.apos.trDelta);
		self->s.apos.trType = TR_STATIONARY;

		G_RadiusDamage( self->r.currentOrigin, self->parent, self->splashDamage, self->splashRadius, self, self, MOD_DET_PACK_SPLASH );
		VectorCopy(trace->plane.normal, v);
		VectorCopy(v, self->pos2);
		self->count = -1;
		G_PlayEffect(EFFECT_EXPLOSION_DETPACK, self->r.currentOrigin, v);

		self->think = G_FreeEntity;
		self->nextthink = level.time;
		return;
	}

	//if we get here I guess we hit hte world so we can stick to it

	// This requires a bit of explaining.
	// When you suicide, all of the detpacks you have placed (either on a wall, or still falling in the air) will have
	//	their ent->think() set to DetPackBlow and ent->nextthink will be between 100 <-> 300
	// If your detpacks land on a surface (i.e. charge_stick gets called) within that 100<->300 ms then ent->think()
	//	will be overwritten (set to DetpackBlow) and ent->nextthink will be 30000
	// The end result is your detpacks won't explode, but will be stuck to the wall for 30 seconds without being able to
	//	detonate them (or shoot them)
	// The fix Sil came up with is to check the think() function in charge_stick, and only overwrite it if they haven't
	//	been primed to detonate

	if ( self->think == G_RunObject ) {
		self->touch = 0;
		self->think = DetPackBlow;
		self->nextthink = level.time + 30000;
	}

	VectorClear(self->s.apos.trDelta);
	self->s.apos.trType = TR_STATIONARY;

	self->s.pos.trType = TR_STATIONARY;
	VectorCopy( self->r.currentOrigin, self->s.origin );
	VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
	VectorClear( self->s.pos.trDelta );

	VectorClear( self->s.apos.trDelta );

	VectorNormalize(trace->plane.normal);

	vectoangles(trace->plane.normal, self->s.angles);
	VectorCopy(self->s.angles, self->r.currentAngles );
	VectorCopy(self->s.angles, self->s.apos.trBase);

	VectorCopy(trace->plane.normal, self->pos2);
	self->count = -1;

	G_Sound(self, CHAN_WEAPON, G_SoundIndex("sound/weapons/detpack/stick.wav"));

	tent = G_TempEntity( self->r.currentOrigin, EV_MISSILE_MISS );
	tent->s.weapon = 0;
	tent->parent = self;
	tent->r.ownerNum = self->s.number;

	//so that the owner can blow it up with projectiles
	self->r.svFlags |= SVF_OWNERNOTSHARED;
}

void DetPackPain(gentity_t *self, gentity_t *attacker, int damage)
{
	self->think = DetPackBlow;
	self->nextthink = level.time + Q_irand(50, 100);
	self->takedamage = false;
}

void DetPackDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	self->think = DetPackBlow;
	self->nextthink = level.time + Q_irand(50, 100);
	self->takedamage = false;
}

void drop_charge (gentity_t *self, vec3_t start, vec3_t dir)
{
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "detpack";
	bolt->nextthink = level.time + FRAMETIME;
	bolt->think = G_RunObject;
	bolt->s.eType = ET_GENERAL;
	bolt->s.g2radius = 100;
	bolt->s.modelGhoul2 = 1;
	bolt->s.modelindex = G_ModelIndex("models/weapons/detpack/det_pack_proj.glm");

	bolt->parent = self;
	bolt->r.ownerNum = self->s.number;
	bolt->damage = 100;
	bolt->splashDamage = 200;
	bolt->splashRadius = 200;
	bolt->methodOfDeath = MOD_DET_PACK_SPLASH;
	bolt->splashMethodOfDeath = MOD_DET_PACK_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->s.solid = 2;
	bolt->r.contents = MASK_SHOT;
	bolt->touch = charge_stick;

	bolt->physicsObject = true;

	bolt->s.genericenemyindex = self->s.number+MAX_GENTITIES;
	//rww - so client prediction knows we own this and won't hit it

	VectorSet( bolt->r.mins, -2, -2, -2 );
	VectorSet( bolt->r.maxs, 2, 2, 2 );

	bolt->health = 1;
	bolt->takedamage = true;
	bolt->pain = DetPackPain;
	bolt->die = DetPackDie;

	bolt->s.weapon = WP_DET_PACK;

	bolt->setTime = level.time;

	G_SetOrigin(bolt, start);
	bolt->s.pos.trType = TR_GRAVITY;
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale(dir, 300, bolt->s.pos.trDelta );
	bolt->s.pos.trTime = level.time;

	bolt->s.apos.trType = TR_GRAVITY;
	bolt->s.apos.trTime = level.time;
	bolt->s.apos.trBase[YAW] = rand()%360;
	bolt->s.apos.trBase[PITCH] = rand()%360;
	bolt->s.apos.trBase[ROLL] = rand()%360;

	if (rand()%10 < 5)
	{
		bolt->s.apos.trBase[YAW] = -bolt->s.apos.trBase[YAW];
	}

	vectoangles(dir, bolt->s.angles);
	VectorCopy(bolt->s.angles, bolt->s.apos.trBase);
	VectorSet(bolt->s.apos.trDelta, 300, 0, 0 );
	bolt->s.apos.trTime = level.time;

	trap->LinkEntity((sharedEntity_t *)bolt);
}

void BlowDetpacks(gentity_t *ent)
{
	gentity_t *found = nullptr;

	if ( ent->client->ps.hasDetPackPlanted )
	{
		while ( (found = G_Find( found, FOFS(classname), "detpack") ) != nullptr )
		{//loop through all ents and blow the crap out of them!
			if ( found->parent == ent )
			{
				VectorCopy( found->r.currentOrigin, found->s.origin );
				found->think = DetPackBlow;
				found->nextthink = level.time + 100 + Q_flrand(0.0f, 1.0f) * 200;
				G_Sound( found, CHAN_BODY, G_SoundIndex("sound/weapons/detpack/warning.wav") );
			}
		}
		ent->client->ps.hasDetPackPlanted = false;
	}
}
void RemoveDetpacks(gentity_t *ent)
{
	gentity_t *found = nullptr;

	if ( ent->client->ps.hasDetPackPlanted )
	{
		while ( (found = G_Find( found, FOFS(classname), "detpack") ) != nullptr )
		{//loop through all ents and blow the crap out of them!
			if ( found->parent == ent )
			{
				VectorCopy( found->r.currentOrigin, found->s.origin );
				found->think = G_FreeEntity;
				found->nextthink = level.time;
			//	G_Sound( found, CHAN_BODY, G_SoundIndex("sound/weapons/detpack/warning.wav") );
			}
		}
		ent->client->ps.hasDetPackPlanted = false;
	}
}

bool CheatsOn(void)
{
	if ( !sv_cheats.integer )
	{
		return false;
	}
	return true;
}

void WP_DropDetPack( gentity_t *ent, bool alt_fire )
{
	gentity_t	*found = nullptr;
	int			trapcount = 0;
	int			foundDetPacks[MAX_GENTITIES] = {ENTITYNUM_NONE};
	int			trapcount_org;
	int			lowestTimeStamp;
	int			removeMe;
	int			i;

	if ( !ent || !ent->client )
	{
		return;
	}

	//limit to 10 placed at any one time
	//see how many there are now
	while ( (found = G_Find( found, FOFS(classname), "detpack" )) != nullptr )
	{
		if ( found->parent != ent )
		{
			continue;
		}
		foundDetPacks[trapcount++] = found->s.number;
	}
	//now remove first ones we find until there are only 9 left
	found = nullptr;
	trapcount_org = trapcount;
	lowestTimeStamp = level.time;
	while ( trapcount > 9 )
	{
		removeMe = -1;
		for ( i = 0; i < trapcount_org; i++ )
		{
			if ( foundDetPacks[i] == ENTITYNUM_NONE )
			{
				continue;
			}
			found = &g_entities[foundDetPacks[i]];
			if ( found->setTime < lowestTimeStamp )
			{
				removeMe = i;
				lowestTimeStamp = found->setTime;
			}
		}
		if ( removeMe != -1 )
		{
			//remove it... or blow it?
			if ( &g_entities[foundDetPacks[removeMe]] == nullptr )
			{
				break;
			}
			else
			{
				if (!CheatsOn())
				{ //Let them have unlimited if cheats are enabled
					G_FreeEntity( &g_entities[foundDetPacks[removeMe]] );
				}
			}
			foundDetPacks[removeMe] = ENTITYNUM_NONE;
			trapcount--;
		}
		else
		{
			break;
		}
	}

	if ( alt_fire  )
	{
		BlowDetpacks(ent);
	}
	else
	{
		AngleVectors( ent->client->ps.viewangles, forward, vright, up );

		CalcMuzzlePoint( ent, forward, vright, up, muzzle );

		VectorNormalize( forward );
		VectorMA( muzzle, -4, forward, muzzle );
		drop_charge( ent, muzzle, forward );

		ent->client->ps.hasDetPackPlanted = true;
	}
}

static void WP_FireConcussionAlt( gentity_t *ent )
{//a rail-gun-like beam
	int			damage = CONC_ALT_DAMAGE, skip, traces = DISRUPTOR_ALT_TRACES;
	bool	render_impact = true;
	vec3_t		start, end;
	vec3_t		/*muzzle2,*/ dir;
	trace_t		tr;
	gentity_t	*traceEnt, *tent;
	float		shotRange = 8192.0f;
	bool	hitDodged = false;
	vec3_t shot_mins, shot_maxs;
	int			i;

	//Shove us backwards for half a second
	VectorMA( ent->client->ps.velocity, -200, forward, ent->client->ps.velocity );
	ent->client->ps.groundEntityNum = ENTITYNUM_NONE;
	if ( (ent->client->ps.pm_flags&PMF_DUCKED) )
	{//hunkered down
		ent->client->ps.pm_time = 100;
	}
	else
	{
		ent->client->ps.pm_time = 250;
	}
//	ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK|PMF_TIME_NOFRICTION;
	//FIXME: only if on ground?  So no "rocket jump"?  Or: (see next FIXME)
	//FIXME: instead, set a forced ucmd backmove instead of this sliding

	//VectorCopy( muzzle, muzzle2 ); // making a backup copy

	VectorCopy( muzzle, start );
	WP_TraceSetStart( ent, start, vec3_origin, vec3_origin );

	skip = ent->s.number;

//	if ( ent->client && ent->client->ps.powerups[PW_WEAPON_OVERCHARGE] > 0 && ent->client->ps.powerups[PW_WEAPON_OVERCHARGE] > cg.time )
//	{
//		// in overcharge mode, so doing double damage
//		damage *= 2;
//	}

	//Make it a little easier to hit guys at long range
	VectorSet( shot_mins, -1, -1, -1 );
	VectorSet( shot_maxs, 1, 1, 1 );

	for ( i = 0; i < traces; i++ )
	{
		VectorMA( start, shotRange, forward, end );

		//NOTE: if you want to be able to hit guys in emplaced guns, use "G2_COLLIDE, 10" instead of "G2_RETURNONHIT, 0"
		//alternately, if you end up hitting an emplaced_gun that has a sitter, just redo this one trace with the "G2_COLLIDE, 10" to see if we it the sitter
		//trap->trace( &tr, start, nullptr, nullptr, end, skip, MASK_SHOT, G2_COLLIDE, 10 );//G2_RETURNONHIT, 0 );
		if (d_projectileGhoul2Collision.integer)
		{
			trap->Trace( &tr, start, shot_mins, shot_maxs, end, skip, MASK_SHOT, false, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_HITCORPSES, g_g2TraceLod.integer );
		}
		else
		{
			trap->Trace( &tr, start, shot_mins, shot_maxs, end, skip, MASK_SHOT, false, 0, 0 );
		}

		traceEnt = &g_entities[tr.entityNum];

		if (d_projectileGhoul2Collision.integer && traceEnt->inuse && traceEnt->client)
		{ //g2 collision checks -rww
			if (traceEnt->inuse && traceEnt->client && traceEnt->ghoul2)
			{ //since we used G2TRFLAG_GETSURFINDEX, tr.surfaceFlags will actually contain the index of the surface on the ghoul2 model we collided with.
				traceEnt->client->g2LastSurfaceHit = tr.surfaceFlags;
				traceEnt->client->g2LastSurfaceTime = level.time;
			}

			if (traceEnt->ghoul2)
			{
				tr.surfaceFlags = 0; //clear the surface flags after, since we actually care about them in here.
			}
		}
		if ( tr.surfaceFlags & SURF_NOIMPACT )
		{
			render_impact = false;
		}

		if ( tr.entityNum == ent->s.number )
		{
			// should never happen, but basically we don't want to consider a hit to ourselves?
			// Get ready for an attempt to trace through another person
			//VectorCopy( tr.endpos, muzzle2 );
			VectorCopy( tr.endpos, start );
			skip = tr.entityNum;
#ifdef _DEBUG
			Com_Printf( "BAD! Concussion gun shot somehow traced back and hit the owner!\n" );
#endif
			continue;
		}

		// always render a shot beam, doing this the old way because I don't much feel like overriding the effect.
		//NOTE: let's just draw one beam at the end
		//tent = G_TempEntity( tr.endpos, EV_CONC_ALT_SHOT );
		//tent->svFlags |= SVF_BROADCAST;

		//VectorCopy( muzzle2, tent->s.origin2 );

		if ( tr.fraction >= 1.0f )
		{
			// draw the beam but don't do anything else
			break;
		}

		if ( traceEnt->s.weapon == WP_SABER )//&& traceEnt->NPC
		{//FIXME: need a more reliable way to know we hit a jedi?
			hitDodged = Jedi_DodgeEvasion( traceEnt, ent, &tr, HL_NONE );
			//acts like we didn't even hit him
		}
		if ( !hitDodged )
		{
			if ( render_impact )
			{
				if (( tr.entityNum < ENTITYNUM_WORLD && traceEnt->takedamage )
					|| !Q_stricmp( traceEnt->classname, "misc_model_breakable" )
					|| traceEnt->s.eType == ET_MOVER )
				{
					bool noKnockBack;

					// Create a simple impact type mark that doesn't last long in the world
					//G_PlayEffectID( G_EffectIndex( "concussion/alt_hit" ), tr.endpos, tr.plane.normal );
					//no no no

					if ( traceEnt->client && LogAccuracyHit( traceEnt, ent ))
					{//NOTE: hitting multiple ents can still get you over 100% accuracy
						ent->client->accuracy_hits++;
					}

					noKnockBack = (traceEnt->flags&FL_NO_KNOCKBACK);//will be set if they die, I want to know if it was on *before* they died
					G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK|DAMAGE_NO_HIT_LOC, MOD_CONC_ALT );

					//do knockback and knockdown manually
					if ( traceEnt->client )
					{//only if we hit a client
						vec3_t pushDir;
						VectorCopy( forward, pushDir );
						if ( pushDir[2] < 0.2f )
						{
							pushDir[2] = 0.2f;
						}//hmm, re-normalize?  nah...
						/*
						if ( !noKnockBack )
						{//knock-backable
							G_Throw( traceEnt, pushDir, 200 );
						}
						*/
						if ( traceEnt->health > 0 )
						{//alive
							//if ( G_HasKnockdownAnims( traceEnt ) )
							if (!noKnockBack && !traceEnt->localAnimIndex && traceEnt->client->ps.forceHandExtend != HANDEXTEND_KNOCKDOWN &&
								BG_KnockDownable(&traceEnt->client->ps)) //just check for humanoids..
							{//knock-downable
								//G_Knockdown( traceEnt, ent, pushDir, 400, true );
								vec3_t plPDif;
								float pStr;

								//cap it and stuff, base the strength and whether or not we can knockdown on the distance
								//from the shooter to the target
								VectorSubtract(traceEnt->client->ps.origin, ent->client->ps.origin, plPDif);
								pStr = 500.0f-VectorLength(plPDif);
								if (pStr < 150.0f)
								{
									pStr = 150.0f;
								}
								if (pStr > 200.0f)
								{
									traceEnt->client->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
									traceEnt->client->ps.forceHandExtendTime = level.time + 1100;
									traceEnt->client->ps.forceDodgeAnim = 0; //this toggles between 1 and 0, when it's 1 we should play the get up anim
								}
								traceEnt->client->ps.otherKiller = ent->s.number;
								traceEnt->client->ps.otherKillerTime = level.time + 5000;
								traceEnt->client->ps.otherKillerDebounceTime = level.time + 100;

								traceEnt->client->ps.velocity[0] += pushDir[0]*pStr;
								traceEnt->client->ps.velocity[1] += pushDir[1]*pStr;
								traceEnt->client->ps.velocity[2] = pStr;
							}
						}
					}

					if ( traceEnt->s.eType == ET_MOVER )
					{//stop the traces on any mover
						break;
					}
				}
				else
				{
					 // we only make this mark on things that can't break or move
				//	tent = G_TempEntity(tr.endpos, EV_MISSILE_MISS);
				//	tent->s.eventParm = DirToByte(tr.plane.normal);
				//	tent->s.eFlags |= EF_ALT_FIRING;

					//tent->svFlags |= SVF_BROADCAST;
					//eh? why broadcast?
				//	VectorCopy( tr.plane.normal, tent->pos1 );

					//mmm..no..don't do this more than once for no reason whatsoever.
					break; // hit solid, but doesn't take damage, so stop the shot...we _could_ allow it to shoot through walls, might be cool?
				}
			}
			else // not rendering impact, must be a skybox or other similar thing?
			{
				break; // don't try anymore traces
			}
		}
		// Get ready for an attempt to trace through another person
		//VectorCopy( tr.endpos, muzzle2 );
		VectorCopy( tr.endpos, start );
		skip = tr.entityNum;
		hitDodged = false;
	}
	//just draw one beam all the way to the end
//	tent = G_TempEntity( tr.endpos, EV_CONC_ALT_SHOT );
//	tent->svFlags |= SVF_BROADCAST;
	//again, why broadcast?

//	tent = G_TempEntity(tr.endpos, EV_MISSILE_MISS);
//	tent->s.eventParm = DirToByte(tr.plane.normal);
//	tent->s.eFlags |= EF_ALT_FIRING;
//	VectorCopy( muzzle, tent->s.origin2 );

	// now go along the trail and make sight events
	VectorSubtract( tr.endpos, muzzle, dir );

//	shotDist = VectorNormalize( dir );

	//let's pack all this junk into a single tempent, and send it off.
	tent = G_TempEntity(tr.endpos, EV_CONC_ALT_IMPACT);
	tent->s.eventParm = DirToByte(tr.plane.normal);
	tent->s.owner = ent->s.number;
	VectorCopy(dir, tent->s.angles);
	VectorCopy(muzzle, tent->s.origin2);
	VectorCopy(forward, tent->s.angles2);
}

static void WP_FireConcussion( gentity_t *ent )
{//a fast rocket-like projectile
	vec3_t	start;
	int		damage	= CONC_DAMAGE;
	float	vel = CONC_VELOCITY;
	gentity_t *missile;

	//hold us still for a bit
	//ent->client->ps.pm_time = 300;
	//ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	//add viewkick
//	if ( ent->s.number < MAX_CLIENTS//player only
//		&& !cg.renderingThirdPerson )//gives an advantage to being in 3rd person, but would look silly otherwise
//	{//kick the view back
//		cg.kick_angles[PITCH] = Q_flrand( -10, -15 );
//		cg.kick_time = level.time;
//	}
	//mm..yeah..this needs some reworking for mp

	VectorCopy( muzzle, start );
	WP_TraceSetStart( ent, start, vec3_origin, vec3_origin );//make sure our start point isn't on the other side of a wall

	missile = CreateMissile( start, forward, vel, 10000, ent, false );

	missile->classname = "conc_proj";
	missile->s.weapon = WP_CONCUSSION;
	missile->mass = 10;

	// Make it easier to hit things
	VectorSet( missile->r.maxs, ROCKET_SIZE, ROCKET_SIZE, ROCKET_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );

	missile->damage = damage;
	missile->dflags = DAMAGE_EXTRA_KNOCKBACK;

	missile->methodOfDeath = MOD_CONC;
	missile->splashMethodOfDeath = MOD_CONC;

	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;
	missile->splashDamage = CONC_SPLASH_DAMAGE;
	missile->splashRadius = CONC_SPLASH_RADIUS;

	// we don't want it to ever bounce
	missile->bounceCount = 0;
}

void WP_FireStunBaton( gentity_t *ent, bool alt_fire )
{
	gentity_t	*tr_ent;
	trace_t		tr;
	vec3_t		mins, maxs, end;
	vec3_t		muzzleStun;

	if (!ent->client)
	{
		VectorCopy(ent->r.currentOrigin, muzzleStun);
		muzzleStun[2] += 8;
	}
	else
	{
		VectorCopy(ent->client->ps.origin, muzzleStun);
		muzzleStun[2] += ent->client->ps.viewheight-6;
	}

	VectorMA(muzzleStun, 20.0f, forward, muzzleStun);
	VectorMA(muzzleStun, 4.0f, vright, muzzleStun);

	VectorMA( muzzleStun, STUN_BATON_RANGE, forward, end );

	VectorSet( maxs, 6, 6, 6 );
	VectorScale( maxs, -1, mins );

	trap->Trace ( &tr, muzzleStun, mins, maxs, end, ent->s.number, MASK_SHOT, false, 0, 0 );

	if ( tr.entityNum >= ENTITYNUM_WORLD )
	{
		return;
	}

	tr_ent = &g_entities[tr.entityNum];

	if (tr_ent && tr_ent->takedamage && tr_ent->client)
	{ //see if either party is involved in a duel
		if (tr_ent->client->ps.duelInProgress &&
			tr_ent->client->ps.duelIndex != ent->s.number)
		{
			return;
		}

		if (ent->client &&
			ent->client->ps.duelInProgress &&
			ent->client->ps.duelIndex != tr_ent->s.number)
		{
			return;
		}
	}

	if ( tr_ent && tr_ent->takedamage )
	{
		G_PlayEffect( EFFECT_STUNHIT, tr.endpos, tr.plane.normal );

		G_Sound( tr_ent, CHAN_WEAPON, G_SoundIndex( va("sound/weapons/melee/punch%d", Q_irand(1, 4)) ) );
		G_Damage( tr_ent, ent, ent, forward, tr.endpos, STUN_BATON_DAMAGE, (DAMAGE_NO_KNOCKBACK|DAMAGE_HALF_ABSORB), MOD_STUN_BATON );

		if (tr_ent->client)
		{ //if it's a player then use the shock effect
			tr_ent->client->ps.electrifyTime = level.time + 700;
		}
	}
}

void WP_FireMelee( gentity_t *ent, bool alt_fire )
{
	if (ent == nullptr)
	{
		return;
	}

	gentity_t	*tr_ent;
	trace_t		tr;
	vec3_t		mins, maxs, end;
	vec3_t		muzzlePunch;

	if (ent->client)
	{
		if (ent->client->ps.torsoAnim == BOTH_MELEE2)
		{ //right
			if (ent->client->ps.brokenLimbs & (1 << BROKENLIMB_RARM))
			{
				return;
			}
		}
		else
		{ //left
			if (ent->client->ps.brokenLimbs & (1 << BROKENLIMB_LARM))
			{
				return;
			}
		}
	}
	
	if (!ent->client)
	{
		VectorCopy(ent->r.currentOrigin, muzzlePunch);
		muzzlePunch[2] += 8;
	}
	else
	{
		VectorCopy(ent->client->ps.origin, muzzlePunch);
		muzzlePunch[2] += ent->client->ps.viewheight-6;
	}

	VectorMA(muzzlePunch, 20.0f, forward, muzzlePunch);
	VectorMA(muzzlePunch, 4.0f, vright, muzzlePunch);

	VectorMA( muzzlePunch, MELEE_RANGE, forward, end );

	VectorSet( maxs, 6, 6, 6 );
	VectorScale( maxs, -1, mins );

	trap->Trace ( &tr, muzzlePunch, mins, maxs, end, ent->s.number, MASK_SHOT, false, 0, 0 );

	if (tr.entityNum != ENTITYNUM_NONE)
	{ //hit something
		tr_ent = &g_entities[tr.entityNum];

		G_Sound( ent, CHAN_AUTO, G_SoundIndex( va("sound/weapons/melee/punch%d", Q_irand(1, 4)) ) );

		if (tr_ent->takedamage && tr_ent->client)
		{ //special duel checks
			if (tr_ent->client->ps.duelInProgress &&
				tr_ent->client->ps.duelIndex != ent->s.number)
			{
				return;
			}

			if (ent->client &&
				ent->client->ps.duelInProgress &&
				ent->client->ps.duelIndex != tr_ent->s.number)
			{
				return;
			}
		}

		if ( tr_ent->takedamage )
		{ //damage them, do more damage if we're in the second right hook
			int dmg = MELEE_SWING1_DAMAGE;

			if (ent->client && ent->client->ps.torsoAnim == BOTH_MELEE2)
			{ //do a tad bit more damage on the second swing
				dmg = MELEE_SWING2_DAMAGE;
			}

			G_Damage( tr_ent, ent, ent, forward, tr.endpos, dmg, DAMAGE_NO_ARMOR, MOD_MELEE );
		}
	}
}

// Round a vector to integers for more efficient network transmission, but make sure that it rounds towards a given
//	point rather than blindly truncating.
// This prevents it from truncating into a wall.
void SnapVectorTowards( vec3_t v, vec3_t to ) {
	int		i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
			v[i] = floorf( v[i] );
		} else {
			v[i] = ceilf( v[i] );
		}
	}
}

bool LogAccuracyHit( gentity_t *target, gentity_t *attacker ) {
	if( !target->takedamage ) {
		return false;
	}

	if ( target == attacker ) {
		return false;
	}

	if( !target->client ) {
		return false;
	}

	if (!attacker)
	{
		return false;
	}

	if( !attacker->client ) {
		return false;
	}

	if( target->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return false;
	}

	if ( OnSameTeam( target, attacker ) ) {
		return false;
	}

	return true;
}

// set muzzle location relative to pivoting eye
// rwwFIXMEFIXME: Since ghoul2 models are on server and properly updated now, it may be reasonable to base muzzle point
//	off actual weapon bolt point.
// The down side would be that it does not necessarily look alright from a first person perspective.
void CalcMuzzlePoint ( gentity_t *ent, const vec3_t inForward, const vec3_t inRight, const vec3_t inUp, vec3_t muzzlePoint )
{
	int weapontype;
	vec3_t muzzleOffPoint;

	weapontype = ent->s.weapon;
	VectorCopy( ent->s.pos.trBase, muzzlePoint );

	VectorCopy(WP_MuzzlePoint[weapontype], muzzleOffPoint);

	if (weapontype > WP_NONE && weapontype < WP_NUM_WEAPONS)
	{	// Use the table to generate the muzzlepoint;
		{	// Crouching.  Use the add-to-Z method to adjust vertically.
			VectorMA(muzzlePoint, muzzleOffPoint[0], inForward, muzzlePoint);
			VectorMA(muzzlePoint, muzzleOffPoint[1], inRight, muzzlePoint);
			muzzlePoint[2] += ent->client->ps.viewheight + muzzleOffPoint[2];
		}
	}

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}

void G_EstimateCamPos( vec3_t viewAngles, vec3_t cameraFocusLoc, float viewheight, float thirdPersonRange,
					  float thirdPersonHorzOffset, float vertOffset, float pitchOffset,
					  int ignoreEntNum, vec3_t camPos )
{
	int			MASK_CAMERACLIP = (MASK_SOLID|CONTENTS_PLAYERCLIP);
	float		CAMERA_SIZE = 4;
	vec3_t		cameramins;
	vec3_t		cameramaxs;
	vec3_t		cameraFocusAngles, camerafwd, cameraup;
	vec3_t		cameraIdealTarget, cameraCurTarget;
	vec3_t		cameraIdealLoc, cameraCurLoc;
	vec3_t		diff;
	vec3_t		camAngles;
	matrix3_t	viewaxis;
	trace_t		trace;

	VectorSet( cameramins, -CAMERA_SIZE, -CAMERA_SIZE, -CAMERA_SIZE );
	VectorSet( cameramaxs, CAMERA_SIZE, CAMERA_SIZE, CAMERA_SIZE );

	VectorCopy( viewAngles, cameraFocusAngles );
	cameraFocusAngles[PITCH] += pitchOffset;
	if ( !bg_fighterAltControl.integer )
	{//clamp view pitch
		cameraFocusAngles[PITCH] = AngleNormalize180( cameraFocusAngles[PITCH] );
		if (cameraFocusAngles[PITCH] > 80.0)
		{
			cameraFocusAngles[PITCH] = 80.0;
		}
		else if (cameraFocusAngles[PITCH] < -80.0)
		{
			cameraFocusAngles[PITCH] = -80.0;
		}
	}
	AngleVectors(cameraFocusAngles, camerafwd, nullptr, cameraup);

	cameraFocusLoc[2] += viewheight;

	VectorCopy( cameraFocusLoc, cameraIdealTarget );
	cameraIdealTarget[2] += vertOffset;

	//NOTE: on cgame, this uses the thirdpersontargetdamp value, we ignore that here
	VectorCopy( cameraIdealTarget, cameraCurTarget );
	trap->Trace( &trace, cameraFocusLoc, cameramins, cameramaxs, cameraCurTarget, ignoreEntNum, MASK_CAMERACLIP, false, 0, 0 );
	if (trace.fraction < 1.0)
	{
		VectorCopy(trace.endpos, cameraCurTarget);
	}

	VectorMA(cameraIdealTarget, -(thirdPersonRange), camerafwd, cameraIdealLoc);
	//NOTE: on cgame, this uses the thirdpersoncameradamp value, we ignore that here
	VectorCopy( cameraIdealLoc, cameraCurLoc );
	trap->Trace(&trace, cameraCurTarget, cameramins, cameramaxs, cameraCurLoc, ignoreEntNum, MASK_CAMERACLIP, false, 0, 0);
	if (trace.fraction < 1.0)
	{
		VectorCopy( trace.endpos, cameraCurLoc );
	}

	VectorSubtract(cameraCurTarget, cameraCurLoc, diff);
	{
		float dist = VectorNormalize(diff);
		//under normal circumstances, should never be 0.00000 and so on.
		if ( !dist || (diff[0] == 0 || diff[1] == 0) )
		{//must be hitting something, need some value to calc angles, so use cam forward
			VectorCopy( camerafwd, diff );
		}
	}

	vectoangles(diff, camAngles);

	if ( thirdPersonHorzOffset != 0.0f )
	{
		AnglesToAxis( camAngles, viewaxis );
		VectorMA( cameraCurLoc, thirdPersonHorzOffset, viewaxis[1], cameraCurLoc );
	}

	VectorCopy(cameraCurLoc, camPos);
}

void FireWeapon( gentity_t *ent, bool altFire ) {
	// track shots taken for accuracy tracking. melee weapons are not tracked.
	if( ent->s.weapon != WP_SABER && ent->s.weapon != WP_STUN_BATON && ent->s.weapon != WP_MELEE )
	{
		if( ent->s.weapon == WP_FLECHETTE ) {
			ent->client->accuracy_shots += FLECHETTE_SHOTS;
		} else {
			ent->client->accuracy_shots++;
		}
	}

	// set aiming directions
	if (ent->s.weapon == WP_EMPLACED_GUN &&
		ent->client->ps.emplacedIndex)
	{ //if using emplaced then base muzzle point off of gun position/angles
		gentity_t *emp = &g_entities[ent->client->ps.emplacedIndex];

		if (emp->inuse)
		{
			float yaw;
			vec3_t viewAngCap;
			int override;

			VectorCopy(ent->client->ps.viewangles, viewAngCap);
			if (viewAngCap[PITCH] > 40)
			{
				viewAngCap[PITCH] = 40;
			}

			override = BG_EmplacedView(ent->client->ps.viewangles, emp->s.angles, &yaw,
				emp->s.origin2[0]);

			if (override)
			{
				viewAngCap[YAW] = yaw;
			}

			AngleVectors( viewAngCap, forward, vright, up );
		}
		else
		{
			AngleVectors( ent->client->ps.viewangles, forward, vright, up );
		}
	}
	else {
		AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	}

	CalcMuzzlePoint ( ent, forward, vright, up, muzzle );

	// fire the specific weapon
	switch( ent->s.weapon ) {
	case WP_STUN_BATON:
		WP_FireStunBaton( ent, altFire );
		break;

	case WP_MELEE:
		WP_FireMelee(ent, altFire);
		break;

	case WP_SABER:
		break;

	case WP_BRYAR_PISTOL:
		WP_FireBryarPistol( ent, altFire );
		break;

	case WP_CONCUSSION:
		if ( altFire )
			WP_FireConcussionAlt( ent );
		else
			WP_FireConcussion( ent );
		break;

	case WP_BRYAR_OLD:
		WP_FireBryarPistol( ent, altFire );
		break;

	case WP_BLASTER:
		WP_FireBlaster( ent, altFire );
		break;

	case WP_DISRUPTOR:
		WP_FireDisruptor( ent, altFire );
		break;

	case WP_BOWCASTER:
		WP_FireBowcaster( ent, altFire );
		break;

	case WP_REPEATER:
		WP_FireRepeater( ent, altFire );
		break;

	case WP_DEMP2:
		WP_FireDEMP2( ent, altFire );
		break;

	case WP_FLECHETTE:
		WP_FireFlechette( ent, altFire );
		break;

	case WP_ROCKET_LAUNCHER:
		WP_FireRocket( ent, altFire );
		break;

	case WP_THERMAL:
		WP_FireThermalDetonator( ent, altFire );
		break;

	case WP_TRIP_MINE:
		WP_PlaceLaserTrap( ent, altFire );
		break;

	case WP_DET_PACK:
		WP_DropDetPack( ent, altFire );
		break;

	case WP_EMPLACED_GUN:
		if (ent->client && ent->client->ewebIndex)
		{ //specially handled by the e-web itself
			break;
		}
		WP_FireEmplaced( ent, altFire );
		break;
	default:
//			assert(!"unknown weapon fire");
		break;
	}

	G_LogWeaponFire(ent->s.number, ent->s.weapon);
}

static void WP_FireEmplaced( gentity_t *ent, bool altFire )
{
	vec3_t	dir, angs, gunpoint;
	vec3_t	right;
	gentity_t *gun;
	int side;

	if (!ent->client)
	{
		return;
	}

	if (!ent->client->ps.emplacedIndex)
	{ //shouldn't be using WP_EMPLACED_GUN if we aren't on an emplaced weapon
		return;
	}

	gun = &g_entities[ent->client->ps.emplacedIndex];

	if (!gun->inuse || gun->health <= 0)
	{ //gun was removed or killed, although we should never hit this check because we should have been forced off it already
		return;
	}

	VectorCopy(gun->s.origin, gunpoint);
	gunpoint[2] += 46;

	AngleVectors(ent->client->ps.viewangles, nullptr, right, nullptr);

	if (gun->genericValue10)
	{ //fire out of the right cannon side
		VectorMA(gunpoint, 10.0f, right, gunpoint);
		side = 0;
	}
	else
	{ //the left
		VectorMA(gunpoint, -10.0f, right, gunpoint);
		side = 1;
	}

	gun->genericValue10 = side;
	G_AddEvent(gun, EV_FIRE_WEAPON, side);

	vectoangles( forward, angs );

	AngleVectors( angs, dir, nullptr, nullptr );

	WP_FireEmplacedMissile( gun, gunpoint, dir, altFire, ent );
}

#define EMPLACED_CANRESPAWN 1

/*QUAKED emplaced_gun (0 0 1) (-30 -20 8) (30 20 60) CANRESPAWN

 count - if CANRESPAWN spawnflag, decides how long it is before gun respawns (in ms)
 constraint - number of degrees gun is constrained from base angles on each side (default 60.0)

 showhealth - set to 1 to show health bar on this entity when crosshair is over it

  teamowner - crosshair shows green for this team, red for opposite team
	0 - none
	1 - red
	2 - blue

  alliedTeam - team that can use this
	0 - any
	1 - red
	2 - blue

  teamnodmg - team that turret does not take damage from or do damage to
	0 - none
	1 - red
	2 - blue
*/

void emplaced_gun_use( gentity_t *self, gentity_t *other, trace_t *trace )
{
	vec3_t fwd1, fwd2;
	float dot;
	int oldWeapon;
	gentity_t *activator = other;
	float zoffset = 50;
	vec3_t anglesToOwner;
	vec3_t vLen;
	float ownLen;

	if ( self->health <= 0 )
	{ //gun is destroyed
		return;
	}

	if (self->activator)
	{ //someone is already using me
		return;
	}

	if (!activator->client)
	{
		return;
	}

	if (activator->client->ps.emplacedTime > level.time)
	{ //last use attempt still too recent
		return;
	}

	if (activator->client->ps.forceHandExtend != HANDEXTEND_NONE)
	{ //don't use if busy doing something else
		return;
	}

	if (activator->client->ps.origin[2] > self->s.origin[2]+zoffset-8)
	{ //can't use it from the top
		return;
	}

	if (activator->client->ps.pm_flags & PMF_DUCKED)
	{ //must be standing
		return;
	}

	if (activator->client->ps.isJediMaster)
	{ //jm can't use weapons
		return;
	}

	VectorSubtract(self->s.origin, activator->client->ps.origin, vLen);
	ownLen = VectorLength(vLen);

	if (ownLen > 64.0f)
	{ //must be within 64 units of the gun to use at all
		return;
	}

	// Let's get some direction vectors for the user
	AngleVectors( activator->client->ps.viewangles, fwd1, nullptr, nullptr );

	// Get the guns direction vector
	AngleVectors( self->pos1, fwd2, nullptr, nullptr );

	dot = DotProduct( fwd1, fwd2 );

	// Must be reasonably facing the way the gun points ( 110 degrees or so ), otherwise we don't allow to use it.
	if ( dot < -0.2f )
	{
		return;
	}

	VectorSubtract(self->s.origin, activator->client->ps.origin, fwd1);
	VectorNormalize(fwd1);

	dot = DotProduct( fwd1, fwd2 );

	//check the positioning in relation to the gun as well
	if ( dot < 0.6f )
	{
		return;
	}

	self->genericValue1 = 1;

	oldWeapon = activator->s.weapon;

	// swap the users weapon with the emplaced gun
	activator->client->ps.weapon = self->s.weapon;
	activator->client->ps.weaponstate = WEAPON_READY;
	activator->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_EMPLACED_GUN );

	activator->client->ps.emplacedIndex = self->s.number;

	self->s.emplacedOwner = activator->s.number;
	self->s.activeForcePass = NUM_FORCE_POWERS+1;

	// the gun will track which weapon we used to have
	self->s.weapon = oldWeapon;

	//user's new owner becomes the gun ent
	activator->r.ownerNum = self->s.number;
	self->activator = activator;

	VectorSubtract(self->r.currentOrigin, activator->client->ps.origin, anglesToOwner);
	vectoangles(anglesToOwner, anglesToOwner);
	return;
}

void emplaced_gun_realuse( gentity_t *self, gentity_t *other, gentity_t *activator )
{
	emplaced_gun_use(self, other, nullptr);
}

void emplaced_gun_pain( gentity_t *self, gentity_t *attacker, int damage )
{
	self->s.health = self->health;

	if ( self->health <= 0 )
	{
		//death effect.. for now taken care of on cgame
	}
	else
	{
		//if we have a pain behavior set then use it I guess
		G_ActivateBehavior( self, BSET_PAIN );
	}
}

#define EMPLACED_GUN_HEALTH 800

void emplaced_gun_update(gentity_t *self)
{
	vec3_t	smokeOrg, puffAngle;
	int oldWeap;
	float ownLen = 0;

	if (self->health < 1 && !self->genericValue5)
	{ //we are dead, set our respawn delay if we have one
		if (self->spawnflags & EMPLACED_CANRESPAWN)
		{
			self->genericValue5 = level.time + 4000 + self->count;
		}
	}
	else if (self->health < 1 && self->genericValue5 < level.time)
	{ //we are dead, see if it's time to respawn
		self->s.time = 0;
		self->genericValue4 = 0;
		self->genericValue3 = 0;
		self->health = EMPLACED_GUN_HEALTH*0.4;
		self->s.health = self->health;
	}

	if (self->genericValue4 && self->genericValue4 < 2 && self->s.time < level.time)
	{ //we have finished our warning (red flashing) effect, it's time to finish dying
		vec3_t explOrg;

		VectorSet( puffAngle, 0, 0, 1 );

		VectorCopy(self->r.currentOrigin, explOrg);
		explOrg[2] += 16;

		//just use the detpack explosion effect
		G_PlayEffect(EFFECT_EXPLOSION_DETPACK, explOrg, puffAngle);

		self->genericValue3 = level.time + Q_irand(2500, 3500);

		G_RadiusDamage(self->r.currentOrigin, self, self->splashDamage, self->splashRadius, self, nullptr, MOD_UNKNOWN);

		self->s.time = -1;

		self->genericValue4 = 2;
	}

	if (self->genericValue3 > level.time)
	{ //see if we are freshly dead and should be smoking
		if (self->genericValue2 < level.time)
		{ //is it time yet to spawn another smoke puff?
			VectorSet( puffAngle, 0, 0, 1 );
			VectorCopy(self->r.currentOrigin, smokeOrg);

			smokeOrg[2] += 60;

			G_PlayEffect(EFFECT_SMOKE, smokeOrg, puffAngle);
			self->genericValue2 = level.time + Q_irand(250, 400);
		}
	}

	if (self->activator && self->activator->client && self->activator->inuse)
	{ //handle updating current user
		vec3_t vLen;
		VectorSubtract(self->s.origin, self->activator->client->ps.origin, vLen);
		ownLen = VectorLength(vLen);

		if (!(self->activator->client->pers.cmd.buttons & BUTTON_USE) && self->genericValue1)
		{
			self->genericValue1 = 0;
		}

		if ((self->activator->client->pers.cmd.buttons & BUTTON_USE) && !self->genericValue1)
		{
			self->activator->client->ps.emplacedIndex = 0;
			self->activator->client->ps.saberHolstered = 0;
			self->nextthink = level.time + 50;
			return;
		}
	}

	if ((self->activator && self->activator->client) &&
		(!self->activator->inuse || self->activator->client->ps.emplacedIndex != self->s.number || self->genericValue4 || ownLen > 64))
	{ //get the user off of me then
		self->activator->client->ps.stats[STAT_WEAPONS] &= ~(1<<WP_EMPLACED_GUN);

		oldWeap = self->activator->client->ps.weapon;
		self->activator->client->ps.weapon = self->s.weapon;
		self->s.weapon = oldWeap;
		self->activator->r.ownerNum = ENTITYNUM_NONE;
		self->activator->client->ps.emplacedTime = level.time + 1000;
		self->activator->client->ps.emplacedIndex = 0;
		self->activator->client->ps.saberHolstered = 0;
		self->activator = nullptr;

		self->s.activeForcePass = 0;
	}
	else if (self->activator && self->activator->client)
	{ //make sure the user is using the emplaced gun weapon
		self->activator->client->ps.weapon = WP_EMPLACED_GUN;
		self->activator->client->ps.weaponstate = WEAPON_READY;
	}
	self->nextthink = level.time + 50;
}

void emplaced_gun_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{ //set us up to flash and then explode
	if (self->genericValue4)
	{
		return;
	}

	self->genericValue4 = 1;

	self->s.time = level.time + 3000;

	self->genericValue5 = 0;
}

void SP_emplaced_gun( gentity_t *ent )
{
	const char *name = "models/map_objects/mp/turret_chair.glm";
	vec3_t down;
	trace_t tr;

	//make sure our assets are precached
	RegisterItem( BG_FindItemForWeapon(WP_EMPLACED_GUN) );

	ent->r.contents = CONTENTS_SOLID;
	ent->s.solid = SOLID_BBOX;

	ent->genericValue5 = 0;

	VectorSet( ent->r.mins, -30, -20, 8 );
	VectorSet( ent->r.maxs, 30, 20, 60 );

	VectorCopy(ent->s.origin, down);

	down[2] -= 1024;

	trap->Trace(&tr, ent->s.origin, ent->r.mins, ent->r.maxs, down, ent->s.number, MASK_SOLID, false, 0, 0);

	if (tr.fraction != 1 && !tr.allsolid && !tr.startsolid)
	{
		VectorCopy(tr.endpos, ent->s.origin);
	}

	ent->spawnflags |= 4; // deadsolid

	ent->health = EMPLACED_GUN_HEALTH;

	if (ent->spawnflags & EMPLACED_CANRESPAWN)
	{ //make it somewhat easier to kill if it can respawn
		ent->health *= 0.4;
	}

	ent->maxHealth = ent->health;
	G_ScaleNetHealth(ent);

	ent->genericValue4 = 0;

	ent->takedamage = true;
	ent->pain = emplaced_gun_pain;
	ent->die = emplaced_gun_die;

	// being caught in this thing when it blows would be really bad.
	ent->splashDamage = 80;
	ent->splashRadius = 128;

	// amount of ammo that this little poochie has
	G_SpawnInt( "count", "600", &ent->count );

	G_SpawnFloat( "constraint", "60", &ent->s.origin2[0] );

	ent->s.modelindex = G_ModelIndex( (char *)name );
	ent->s.modelGhoul2 = 1;
	ent->s.g2radius = 110;

	//so the cgame knows for sure that we're an emplaced weapon
	ent->s.weapon = WP_EMPLACED_GUN;

	G_SetOrigin( ent, ent->s.origin );

	// store base angles for later
	VectorCopy( ent->s.angles, ent->pos1 );
	VectorCopy( ent->s.angles, ent->r.currentAngles );
	VectorCopy( ent->s.angles, ent->s.apos.trBase );

	ent->think = emplaced_gun_update;
	ent->nextthink = level.time + 50;

	ent->use = emplaced_gun_realuse;

	ent->r.svFlags |= SVF_PLAYER_USABLE;

	ent->s.pos.trType = TR_STATIONARY;

	ent->s.owner = MAX_CLIENTS+1;
	ent->s.shouldtarget = true;
	//ent->s.teamowner = 0;

	trap->LinkEntity((sharedEntity_t *)ent);
}
