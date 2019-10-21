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

// cg_weaponinit.c -- events and effects dealing with weapons
#include "cgame/cg_local.h"
#include "cgame/cg_weaponfx.h"
#include "cgame/cg_media.h"

// The server says this item is used on this level
void CG_RegisterWeapon( int weaponNum) {
	weaponInfo_t	*weaponInfo;
	gitem_t			*item, *ammo;
	char			path[MAX_QPATH];
	vec3_t			mins, maxs;
	int				i;

	if ( weaponNum <= WP_NONE || weaponNum >= WP_NUM_WEAPONS ) {
		return;
	}

	weaponInfo = &cg_weapons[weaponNum];

	if ( weaponInfo->registered ) {
		return;
	}

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
	weaponInfo->registered = qtrue;

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType == IT_WEAPON && item->giTag == weaponNum ) {
			weaponInfo->item = item;
			break;
		}
	}
	if ( !item->classname ) {
		trap->Error( ERR_DROP, "Couldn't find weapon %i", weaponNum );
	}
	CG_RegisterItemVisuals( item - bg_itemlist );

	// load cmodel before model so filecache works
	weaponInfo->weaponModel = trap->R_RegisterModel( item->world_model[0] );
	// load in-view model also
	weaponInfo->viewModel = trap->R_RegisterModel(item->view_model);

	// calc midpoint for rotation
	trap->R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
	for ( i = 0 ; i < 3 ; i++ ) {
		weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );
	}

	weaponInfo->weaponIcon = media.gfx.null; //item->icon;
	weaponInfo->ammoIcon = media.gfx.null; //item->icon;

	for ( ammo = bg_itemlist + 1 ; ammo->classname ; ammo++ ) {
		if ( ammo->giType == IT_AMMO && ammo->giTag == weaponNum ) {
			break;
		}
	}
	if ( ammo->classname && ammo->world_model[0] ) {
		weaponInfo->ammoModel = trap->R_RegisterModel( ammo->world_model[0] );
	}

	weaponInfo->flashModel = 0;

	if (weaponNum == WP_DISRUPTOR ||
		weaponNum == WP_FLECHETTE ||
		weaponNum == WP_REPEATER ||
		weaponNum == WP_ROCKET_LAUNCHER ||
		weaponNum == WP_CONCUSSION)
	{
		Q_strncpyz( path, item->view_model, sizeof(path) );
		COM_StripExtension( path, path, sizeof( path ) );
		Q_strcat( path, sizeof(path), "_barrel.md3" );
		weaponInfo->barrelModel = trap->R_RegisterModel( path );
	}
	else if (weaponNum != WP_STUN_BATON) {
		weaponInfo->barrelModel = 0;
	}

	if (weaponNum != WP_SABER)
	{
		Q_strncpyz( path, item->view_model, sizeof(path) );
		COM_StripExtension( path, path, sizeof( path ) );
		Q_strcat( path, sizeof(path), "_hand.md3" );
		weaponInfo->handsModel = trap->R_RegisterModel( path );
	}
	else
	{
		weaponInfo->handsModel = 0;
	}

	switch ( weaponNum ) {

	case WP_STUN_BATON:
	case WP_MELEE: {
		if (weaponNum == WP_STUN_BATON) {
			weaponInfo->flashSound[0] = trap->S_RegisterSound( "sound/weapons/baton/fire.mp3" );
			weaponInfo->altFlashSound[0] = trap->S_RegisterSound( "sound/weapons/baton/fire.mp3" );
		}
	} break;

	case WP_SABER: {
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->firingSound = trap->S_RegisterSound( "sound/weapons/saber/saberhum1.wav" );
		weaponInfo->missileModel		= trap->R_RegisterModel( DEFAULT_SABER_MODEL );
	} break;

	case WP_CONCUSSION: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/concussion/select.wav");
		weaponInfo->flashSound[0]		= NULL_SOUND;
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "concussion/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_ConcussionProjectileThink;
		weaponInfo->altFlashSound[0]	= NULL_SOUND;
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= trap->S_RegisterSound( "sound/weapons/bryar/altcharge.wav");
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "concussion/altmuzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_ConcussionProjectileThink;
	} break;

	case WP_BRYAR_PISTOL:
	case WP_BRYAR_OLD: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/bryar/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/bryar/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "bryar/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_BryarProjectileThink;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/bryar/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= trap->S_RegisterSound( "sound/weapons/bryar/altcharge.wav");
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "bryar/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_BryarAltProjectileThink;
	} break;

	case WP_BLASTER:
	case WP_EMPLACED_GUN: { //rww - just use the same as this for now..
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/blaster/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/blaster/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "blaster/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_BlasterProjectileThink;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/blaster/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "blaster/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_BlasterProjectileThink;
	} break;

	case WP_DISRUPTOR: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/disruptor/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/disruptor/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "disruptor/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/disruptor/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= trap->S_RegisterSound("sound/weapons/disruptor/altCharge.wav");
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "disruptor/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;
	} break;

	case WP_BOWCASTER: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/bowcaster/select.wav");
		weaponInfo->altFlashSound[0]		= trap->S_RegisterSound( "sound/weapons/bowcaster/fire.wav");
		weaponInfo->altFiringSound			= NULL_SOUND;
		weaponInfo->altChargeSound			= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "bowcaster/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight		= 0;
		weaponInfo->altMissileHitSound		= NULL_SOUND;
		weaponInfo->altMissileTrailFunc	= FX_BowcasterProjectileThink;
		weaponInfo->flashSound[0]	= trap->S_RegisterSound( "sound/weapons/bowcaster/fire.wav");
		weaponInfo->firingSound		= NULL_SOUND;
		weaponInfo->chargeSound		= trap->S_RegisterSound( "sound/weapons/bowcaster/altcharge.wav");
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "bowcaster/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight	= 0;
		weaponInfo->missileHitSound	= NULL_SOUND;
		weaponInfo->missileTrailFunc = FX_BowcasterAltProjectileThink;
	} break;

	case WP_REPEATER: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/repeater/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/repeater/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "repeater/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_RepeaterProjectileThink;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/repeater/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "repeater/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_RepeaterAltProjectileThink;
	} break;

	case WP_DEMP2: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/demp2/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound("sound/weapons/demp2/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect("demp2/muzzle_flash");
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_DEMP2_ProjectileThink;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound("sound/weapons/demp2/altfire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= trap->S_RegisterSound("sound/weapons/demp2/altCharge.wav");
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect("demp2/muzzle_flash");
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;
	} break;

	case WP_FLECHETTE: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/flechette/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/flechette/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "flechette/muzzle_flash" );
		weaponInfo->missileModel		= trap->R_RegisterModel("models/weapons/golan_arms/projectileMain.md3");
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_FlechetteProjectileThink;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/flechette/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "flechette/muzzle_flash" );
		weaponInfo->altMissileModel		= trap->R_RegisterModel( "models/weapons/golan_arms/projectile.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_FlechetteAltProjectileThink;
	} break;

	case WP_ROCKET_LAUNCHER: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/rocket/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/rocket/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= trap->FX_RegisterEffect( "rocket/muzzle_flash" ); //trap->FX_RegisterEffect( "rocket/muzzle_flash2" );
		//flash2 still looks crappy with the fx bolt stuff. Because the fx bolt stuff doesn't work entirely right.
		weaponInfo->missileModel		= trap->R_RegisterModel( "models/weapons/merr_sonn/projectile.md3" );
		weaponInfo->missileSound		= trap->S_RegisterSound( "sound/weapons/rocket/missleloop.wav");
		weaponInfo->missileDlight		= 125;
		VectorSet(weaponInfo->missileDlightColor, 1.0, 1.0, 0.5);
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_RocketProjectileThink;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/rocket/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= trap->FX_RegisterEffect( "rocket/altmuzzle_flash" );
		weaponInfo->altMissileModel		= trap->R_RegisterModel( "models/weapons/merr_sonn/projectile.md3" );
		weaponInfo->altMissileSound		= trap->S_RegisterSound( "sound/weapons/rocket/missleloop.wav");
		weaponInfo->altMissileDlight	= 125;
		VectorSet(weaponInfo->altMissileDlightColor, 1.0, 1.0, 0.5);
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_RocketAltProjectileThink;
	} break;

	case WP_THERMAL: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/thermal/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/thermal/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= trap->S_RegisterSound( "sound/weapons/thermal/charge.wav");
		weaponInfo->muzzleEffect		= NULL_FX;
		weaponInfo->missileModel		= trap->R_RegisterModel( "models/weapons/thermal/thermal_proj.md3" );
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;

		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/thermal/fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= trap->S_RegisterSound( "sound/weapons/thermal/charge.wav");
		weaponInfo->altMuzzleEffect		= NULL_FX;
		weaponInfo->altMissileModel		= trap->R_RegisterModel( "models/weapons/thermal/thermal_proj.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;
	} break;

	case WP_TRIP_MINE: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/detpack/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/laser_trap/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= NULL_FX;
		weaponInfo->missileModel		= 0;//trap->R_RegisterModel( "models/weapons/laser_trap/laser_trap_w.md3" );
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/laser_trap/fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= NULL_FX;
		weaponInfo->altMissileModel		= 0;//trap->R_RegisterModel( "models/weapons/laser_trap/laser_trap_w.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;
	} break;

	case WP_DET_PACK: {
		weaponInfo->selectSound			= trap->S_RegisterSound("sound/weapons/detpack/select.wav");
		weaponInfo->flashSound[0]		= trap->S_RegisterSound( "sound/weapons/detpack/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= NULL_FX;
		weaponInfo->missileModel		= trap->R_RegisterModel( "models/weapons/detpack/det_pack.md3" );
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;
		weaponInfo->altFlashSound[0]	= trap->S_RegisterSound( "sound/weapons/detpack/fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= NULL_FX;
		weaponInfo->altMissileModel		= trap->R_RegisterModel( "models/weapons/detpack/det_pack.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;
	} break;

	case WP_TURRET: {
		weaponInfo->flashSound[0]		= NULL_SOUND;
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= NULL_HANDLE;
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_TurretProjectileThink;
	} break;

	default: {
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
		weaponInfo->flashSound[0] = trap->S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav" );
	} break;

	}
}
