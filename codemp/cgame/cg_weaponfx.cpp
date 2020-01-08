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

// Blaster Weapon

#include "cgame/cg_local.h"
#include "cgame/cg_weaponfx.h"
#include "cgame/cg_media.h"

static const vec3_t WHITE =		{ 1.00f,	1.0f,	1.0f };
static const vec3_t BRIGHT =	{ 0.75f,	0.5f,	1.0f };
static const vec3_t	YELLER =	{ 0.80f,	0.7f,	0.0f };

void FX_BlasterProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_BlasterAltFireThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_BlasterWeaponHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_BlasterWeaponHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( humanoid ? media.efx.null : media.efx.null, origin, normal, -1, -1, false );
}

void FX_BowcasterProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_BowcasterHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_BowcasterHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_BowcasterAltProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_BryarProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_BryarHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_BryarHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( humanoid ? media.efx.null : media.efx.null, origin, normal, -1, -1, false );
}

void FX_BryarAltProjectileThink(  centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;
	int t;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	// see if we have some sort of extra charge going on
	for ( t=1; t<cent->currentState.generic1; t++ ) {
		// just add ourselves over, and over, and over when we are charged
		trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_BryarAltHitWall( vec3_t origin, vec3_t normal, int power ) {
	switch ( power ) {

	case 4:
	case 5: {
		trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
	} break;

	case 2:
	case 3: {
		trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
	} break;

	default: {
		trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
	} break;

	}
}

void FX_BryarAltHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( humanoid ? media.efx.null : media.efx.null, origin, normal, -1, -1, false );
}

void FX_TurretProjectileThink(  centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_TurretHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_TurretHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( humanoid ? media.efx.null : media.efx.null, origin, normal, -1, -1, false );
}

void FX_ConcussionHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_ConcussionHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_ConcussionProjectileThink(  centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_ConcAltShot( vec3_t start, vec3_t end ) {
	//CJKFIXME: don't register at runtime
	const qhandle_t blueLineShader = media.gfx.null /*"gfx/effects/blueLine"*/;
	const qhandle_t whiteLineShader = media.gfx.null /*"gfx/misc/whiteline2"*/;

	// "concussion/beam"
	trap->FX_AddLine( start, end, 0.1f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, WHITE, WHITE, 0.0f, 175, blueLineShader, FX_SIZE_LINEAR | FX_ALPHA_LINEAR );

	// add some beef
	trap->FX_AddLine( start, end, 0.1f, 7.0f, 0.0f, 1.0f, 0.0f, 0.0f, BRIGHT, BRIGHT, 0.0f, 150, whiteLineShader, FX_SIZE_LINEAR | FX_ALPHA_LINEAR );
}

void FX_DEMP2_ProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_DEMP2_HitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_DEMP2_HitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_DEMP2_AltBeam( vec3_t start, vec3_t end, vec3_t normal, /*bool spark,*/ vec3_t targ1, vec3_t targ2 ) {
	// ...
}

void FX_DEMP2_AltDetonate( vec3_t org, float size ) {
	localEntity_t	*ex;

	ex = CG_AllocLocalEntity();
	ex->leType = LE_FADE_SCALE_MODEL;
	memset( &ex->refEntity, 0, sizeof( refEntity_t ));

	ex->refEntity.renderfx |= RF_VOLUMETRIC;

	ex->startTime = cg.time;
	ex->endTime = ex->startTime + 800;//1600;

	ex->radius = size;
	ex->refEntity.customShader = media.gfx.null;
	ex->refEntity.hModel = media.models.null;
	VectorCopy( org, ex->refEntity.origin );

	ex->color[0] = ex->color[1] = ex->color[2] = 255.0f;
}

void FX_DisruptorMainShot( vec3_t start, vec3_t end ) {
	//CJKFIXME: don't register at runtime
	const qhandle_t redLineShader = media.gfx.null /*"gfx/effects/redLine"*/;
	//const qhandle_t spiralShader = media.gfx.null /*"gfx/effects/spiral"*/;
	//vec3_t	dir;
	//float	len;

	trap->FX_AddLine( start, end, 0.1f, 6.0f, 0.0f, 1.0f, 0.0f, 0.0f, WHITE, WHITE, 0.0f, 150, redLineShader, FX_SIZE_LINEAR | FX_ALPHA_LINEAR );

	//VectorSubtract( end, start, dir );
	//len = VectorNormalize( dir );

	//trap->FX_AddCylinder( start, dir, 5.0f, 5.0f, 0.0f, 5.0f, 5.0f, 0.0f, len, len, 0.0f, 1.0f, 1.0f, 0.0f, WHITE, WHITE, 0.0f, 400, spiralShader, 0 );
}

void FX_DisruptorAltShot( vec3_t start, vec3_t end, bool fullCharge ) {
	//CJKFIXME: don't register at runtime
	const qhandle_t redLineShader = media.gfx.null /*"gfx/effects/redLine"*/;
	const qhandle_t whiteLineShader = media.gfx.null /*"gfx/misc/whiteline2"*/;
	trap->FX_AddLine( start, end, 0.1f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, WHITE, WHITE, 0.0f, 175, redLineShader, FX_SIZE_LINEAR | FX_ALPHA_LINEAR );

	if ( fullCharge ) {
		// add some beef
		trap->FX_AddLine( start, end, 0.1f, 7.0f, 0.0f, 1.0f, 0.0f, 0.0f, YELLER, YELLER, 0.0f, 150, whiteLineShader, FX_SIZE_LINEAR | FX_ALPHA_LINEAR );
	}
}

void FX_DisruptorAltMiss( vec3_t origin, vec3_t normal ) {
	vec3_t pos, c1, c2;
	addbezierArgStruct_t b;
	//CJKFIXME: don't register at runtime
	const qhandle_t smokeTrailShader = media.gfx.null /*"gfx/effects/smokeTrail"*/;

	VectorMA( origin, 4.0f, normal, c1 );
	VectorCopy( c1, c2 );
	c1[2] += 4;
	c2[2] += 12;

	VectorAdd( origin, normal, pos );
	pos[2] += 28;

	VectorCopy( origin, b.start );
	VectorCopy( pos, b.end );
	VectorCopy( c1, b.control1 );
	VectorCopy( vec3_origin, b.control1Vel );
	VectorCopy( c2, b.control2 );
	VectorCopy( vec3_origin, b.control2Vel );

	b.size1 = 6.0f;
	b.size2 = 6.0f;
	b.sizeParm = 0.0f;
	b.alpha1 = 0.0f;
	b.alpha2 = 0.2f;
	b.alphaParm = 0.5f;

	VectorCopy( WHITE, b.sRGB );
	VectorCopy( WHITE, b.eRGB );

	b.rgbParm = 0.0f;
	b.killTime = 4000;
	b.shader = smokeTrailShader;
	b.flags = FX_ALPHA_WAVE;

	trap->FX_AddBezier( &b );

	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_DisruptorAltHit( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_DisruptorHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_DisruptorHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_FlechetteProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_FlechetteWeaponHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_FlechetteWeaponHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_FlechetteAltProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

// This effect is not generic because of possible enhancements
void FX_ForceDrained( vec3_t origin, vec3_t dir ) {
	VectorScale( dir, -1.0, dir );
	trap->FX_PlayEffectID( media.efx.null, origin, dir, -1, -1, false );
}

void FX_RepeaterProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_RepeaterHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_RepeaterHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

static void CG_DistortionOrb( centity_t *cent ) {
	refEntity_t ent{};
	vec3_t ang;
	float scale = 0.5f;
	float vLen;

	if ( !cg_renderToTextureFX.integer ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, ent.origin );

	VectorSubtract( ent.origin, cg.refdef.vieworg, ent.axis[0] );
	vLen = VectorLength( ent.axis[0] );
	if ( VectorNormalize( ent.axis[0] ) <= 0.1f ) {
		// Entity is right on vieworg.  quit.
		return;
	}

//	VectorCopy( cg.refdef.viewaxis[2], ent.axis[2] );
//	CrossProduct( ent.axis[0], ent.axis[2], ent.axis[1] );
	vectoangles( ent.axis[0], ang );
	ang[ROLL] = cent->trickAlpha;
	cent->trickAlpha += 16; //spin the half-sphere to give a "screwdriver" effect
	AnglesToAxis( ang, ent.axis );

	//radius must be a power of 2, and is the actual captured texture size
	if ( vLen < 128 ) {
		ent.radius = 256;
	}
	else if ( vLen < 256 ) {
		ent.radius = 128;
	}
	else if ( vLen < 512 ) {
		ent.radius = 64;
	}
	else {
		ent.radius = 32;
	}

	VectorScale( ent.axis[0],  scale, ent.axis[0] );
	VectorScale( ent.axis[1],  scale, ent.axis[1] );
	VectorScale( ent.axis[2], -scale, ent.axis[2] );

	ent.hModel = media.models.null;
	ent.customShader = 0;//media.gfx.null;

	ent.renderfx = (RF_DISTORTION|RF_RGB_TINT);

	//tint the whole thing a shade of blue
	ent.shaderRGBA[0] = 200.0f;
	ent.shaderRGBA[1] = 200.0f;
	ent.shaderRGBA[2] = 255.0f;

	trap->R_AddRefEntityToScene( &ent );
}

void FX_RepeaterAltProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	if ( cg_repeaterOrb.integer ) {
		CG_DistortionOrb( cent );
	}
	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_RepeaterAltHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_RepeaterAltHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_RocketProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}

void FX_RocketHitWall( vec3_t origin, vec3_t normal ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_RocketHitPlayer( vec3_t origin, vec3_t normal, bool humanoid ) {
	trap->FX_PlayEffectID( media.efx.null, origin, normal, -1, -1, false );
}

void FX_RocketAltProjectileThink( centity_t *cent, const weaponInfo_t *weapon ) {
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f ) {
		forward[2] = 1.0f;
	}

	trap->FX_PlayEffectID( media.efx.null, cent->lerpOrigin, forward, -1, -1, false );
}
