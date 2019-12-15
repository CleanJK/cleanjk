/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
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

// bg_local.h -- local definitions for the bg (both games) files

#define	MIN_WALK_NORMAL 0.7f // can't walk on very steep slopes
#define	TIMER_LAND      130
#define	TIMER_GESTURE   (34*66+50)
#define	OVERCLIP        1.001f

// all of the locals will be zeroed before each pmove, just to make damn sure we don't have any differences when running on client or server
extern struct pml_t {
	vec3_t		forward, right, up;
	float		frametime;

	int			msec;

	qboolean	walking;
	qboolean	groundPlane;
	trace_t		groundTrace;

	float		impactSpeed;

	vec3_t		previous_origin;
	vec3_t		previous_velocity;
	int			previous_waterlevel;
} pml;

// variables
extern int   c_pmove;
extern int   forcePowerNeeded[NUM_FORCE_POWER_LEVELS][NUM_FORCE_POWERS];
extern float pm_accelerate;
extern float pm_airaccelerate;
extern float pm_duckScale;
extern float pm_flightfriction;
extern float pm_flyaccelerate;
extern float pm_friction;
extern float pm_stopspeed;
extern float pm_swimScale;
extern float pm_wadeScale;
extern float pm_wateraccelerate;
extern float pm_waterfriction;

// functions
qboolean         BG_InSlopeAnim             ( int anim );
saberInfo_t     *BG_MySaber                 ( int clientNum, int saberNum );
qboolean         BG_SaberLockBreakAnim      ( int anim );
void             PM_AddEvent                ( int newEvent );
void             PM_AddTouchEnt             ( int entityNum );
int              PM_AnimLength              ( int index, animNumber_t anim );
void             PM_BeginWeaponChange       ( int weapon );
qboolean         PM_CanRollFromSoulCal      ( playerState_t *ps );
void             PM_ClipVelocity            ( vec3_t in, vec3_t normal, vec3_t out, float overbounce );
void             PM_ContinueLegsAnim        ( int anim );
void             PM_FinishWeaponChange      ( void );
void             PM_ForceLegsAnim           ( int anim );
int              PM_GetSaberStance          ( void );
float            PM_GroundDistance          ( void );
qboolean         PM_GroundSlideOkay         ( float zNormal );
qboolean         PM_InOnGroundAnim          ( int anim );
qboolean         PM_InRollComplete          ( playerState_t *ps, int anim );
qboolean         PM_InSaberAnim             ( int anim );
qboolean         PM_JumpingAnim             ( int anim );
int              PM_KickMoveForConditions   ( void );
qboolean         PM_LandingAnim             ( int anim );
qboolean         PM_PainAnim                ( int anim );
saberMoveName_t  PM_SaberFlipOverAttackMove ( void );
qboolean         PM_SaberInKnockaway        ( int move );
qboolean         PM_SaberInParry            ( int move );
qboolean         PM_SaberInReflect          ( int move );
saberMoveName_t  PM_SaberJumpAttackMove     ( void );
void             PM_SetAnim                 ( int setAnimParts,int anim,int setAnimFlags );
void             PM_SetForceJumpZStart      ( float value );
void             PM_SetSaberMove            ( short newMove );
qboolean         PM_SlideMove               ( qboolean gravity );
qboolean         PM_SomeoneInFront          ( trace_t *tr );
qboolean         PM_SpinningAnim            ( int anim );
void             PM_StartTorsoAnim          ( int anim );
void             PM_StepSlideMove           ( qboolean gravity );
qboolean         PM_SwimmingAnim            ( int anim );
float            PM_WalkableGroundDistance  ( void );
void             PM_WeaponLightsaber        ( void );
