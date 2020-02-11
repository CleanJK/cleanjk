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

// local definitions for the bg (both games) files

// ======================================================================
// DEFINE
// ======================================================================

#define	MIN_WALK_NORMAL 0.7F // can't walk on very steep slopes
#define	TIMER_LAND      130
#define	TIMER_GESTURE   (34*66+50)
#define	OVERCLIP        1.001F

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

// all of the locals will be zeroed before each pmove, just to make damn sure we don't have any differences when running on client or server
extern struct pml_t {
	vec3_t		forward, right, up;
	float		frametime;

	int			msec;

	bool	walking;
	bool	groundPlane;
	trace_t		groundTrace;

	float		impactSpeed;

	vec3_t		previous_origin;
	vec3_t		previous_velocity;
	int			previous_waterlevel;
} pml;

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
extern int c_pmove;
extern int forcePowerNeeded[NUM_FORCE_POWER_LEVELS][NUM_FORCE_POWERS];

// ======================================================================
// FUNCTION
// ======================================================================

bool BG_InSlopeAnim(int anim);
bool BG_SaberLockBreakAnim(int anim);
bool PM_CanRollFromSoulCal(playerState_t* ps);
bool PM_GroundSlideOkay(float zNormal);
bool PM_InOnGroundAnim(int anim);
bool PM_InRollComplete(playerState_t* ps, int anim);
bool PM_InSaberAnim(int anim);
bool PM_JumpingAnim(int anim);
bool PM_LandingAnim(int anim);
bool PM_PainAnim(int anim);
bool PM_SaberInKnockaway(int move);
bool PM_SaberInParry(int move);
bool PM_SaberInReflect(int move);
bool PM_SlideMove(bool gravity);
bool PM_SomeoneInFront(trace_t* tr);
bool PM_SpinningAnim(int anim);
bool PM_SwimmingAnim(int anim);
float PM_GroundDistance(void);
float PM_WalkableGroundDistance(void);
int PM_AnimLength(int index, animNumber_e anim);
int PM_GetSaberStance(void);
int PM_KickMoveForConditions(void);
saberInfo_t* BG_MySaber(int clientNum, int saberNum);
saberMoveName_e  PM_SaberFlipOverAttackMove(void);
saberMoveName_e  PM_SaberJumpAttackMove(void);
void PM_AddEvent(int newEvent);
void PM_AddTouchEnt(int entityNum);
void PM_BeginWeaponChange(int weapon);
void PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce);
void PM_ContinueLegsAnim(int anim);
void PM_FinishWeaponChange(void);
void PM_ForceLegsAnim(int anim);
void PM_SetAnim(int setAnimParts, int anim, int setAnimFlags);
void PM_SetForceJumpZStart(float value);
void PM_SetSaberMove(short newMove);
void PM_StartTorsoAnim(int anim);
void PM_StepSlideMove(bool gravity);
void PM_WeaponLightsaber(void);