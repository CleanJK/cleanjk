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

// NOTENOTE This is not the best, DO NOT CHANGE THESE!
#define	FX_ALPHA_LINEAR		0x00000001
#define FX_ALPHA_WAVE		0x00000008
#define	FX_SIZE_LINEAR		0x00000100

// shoot
void FX_DisruptorAltShot(				vec3_t start, vec3_t end, qboolean fullCharge );
void FX_DisruptorMainShot(				vec3_t start, vec3_t end );

// hit wall
void FX_BlasterWeaponHitWall(			vec3_t origin, vec3_t normal );
void FX_BowcasterHitWall(				vec3_t origin, vec3_t normal );
void FX_BryarHitWall(					vec3_t origin, vec3_t normal );
void FX_DEMP2_HitWall(					vec3_t origin, vec3_t normal );
void FX_DisruptorAltHit(				vec3_t origin, vec3_t normal );
void FX_DisruptorAltMiss(				vec3_t origin, vec3_t normal );
void FX_DisruptorHitWall(				vec3_t origin, vec3_t normal );
void FX_FlechetteWeaponHitWall(			vec3_t origin, vec3_t normal );
void FX_RepeaterAltHitWall(				vec3_t origin, vec3_t normal );
void FX_RepeaterHitWall(				vec3_t origin, vec3_t normal );
void FX_RocketHitWall(					vec3_t origin, vec3_t normal );
void FX_BryarAltHitWall(				vec3_t origin, vec3_t normal, int power );

// hit player
void FX_BlasterWeaponHitPlayer(			vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_BowcasterHitPlayer(				vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_BryarAltHitPlayer(				vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_BryarHitPlayer(					vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_DEMP2_HitPlayer(				vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_DisruptorHitPlayer(				vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_FlechetteWeaponHitPlayer(		vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_RepeaterAltHitPlayer(			vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_RepeaterHitPlayer(				vec3_t origin, vec3_t normal, qboolean humanoid );
void FX_RocketHitPlayer(				vec3_t origin, vec3_t normal, qboolean humanoid );

// detonate
void FX_DEMP2_AltDetonate(				vec3_t org, float size );

// projectile think
void FX_BlasterAltFireThink(			centity_t *cent, const struct weaponInfo_s *weapon );
void FX_BlasterProjectileThink(			centity_t *cent, const struct weaponInfo_s *weapon );
void FX_BowcasterAltProjectileThink(	centity_t *cent, const struct weaponInfo_s *weapon );
void FX_BowcasterProjectileThink(		centity_t *cent, const struct weaponInfo_s *weapon );
void FX_BryarAltProjectileThink(		centity_t *cent, const struct weaponInfo_s *weapon );
void FX_BryarProjectileThink(			centity_t *cent, const struct weaponInfo_s *weapon );
void FX_DEMP2_ProjectileThink(			centity_t *cent, const struct weaponInfo_s *weapon );
void FX_FlechetteAltProjectileThink(	centity_t *cent, const struct weaponInfo_s *weapon );
void FX_FlechetteProjectileThink(		centity_t *cent, const struct weaponInfo_s *weapon );
void FX_RepeaterAltProjectileThink(		centity_t *cent, const struct weaponInfo_s *weapon );
void FX_RepeaterProjectileThink(		centity_t *cent, const struct weaponInfo_s *weapon );
void FX_RocketAltProjectileThink(		centity_t *cent, const struct weaponInfo_s *weapon );
void FX_RocketProjectileThink(			centity_t *cent, const struct weaponInfo_s *weapon );
