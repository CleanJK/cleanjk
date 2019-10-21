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

//B_local.h
//re-added by MCG

#include "game/g_local.h"
#include "game/b_public.h"
#include "game/say.h"

#define	AI_TIMERS 0//turn on to see print-outs of AI/nav timing
// Navigation susbsystem

#define NAVF_DUCK			0x00000001
#define NAVF_JUMP			0x00000002
#define NAVF_HOLD			0x00000004
#define NAVF_SLOW			0x00000008

#define DEBUG_LEVEL_DETAIL	4
#define DEBUG_LEVEL_INFO	3
#define DEBUG_LEVEL_WARNING	2
#define DEBUG_LEVEL_ERROR	1
#define DEBUG_LEVEL_NONE	0

#define MAX_GOAL_REACHED_DIST_SQUARED	256//16 squared
#define MIN_ANGLE_ERROR 0.01f

#define MIN_ROCKET_DIST_SQUARED 16384//128*128

void Debug_Printf( vmCvar_t *cv, int level, char *fmt, ... );

//NPC_combat
int ChooseBestWeapon( void );
void ShootThink( void );
void WeaponThink( qboolean inCombat );
qboolean HaveWeapon( int weapon );
qboolean CanShoot ( gentity_t *ent, gentity_t *shooter );
qboolean EntIsGlass (gentity_t *check);
qboolean ShotThroughGlass (trace_t *tr, gentity_t *target, vec3_t spot, int mask);
qboolean ValidEnemy (gentity_t *ent);

#define COLLISION_RADIUS 32
#define NUM_POSITIONS 30

//NPC spawnflags
#define SFB_RIFLEMAN	2
#define SFB_PHASER		4

#define	SFB_CINEMATIC	32
#define	SFB_NOTSOLID	64
#define	SFB_STARTINSOLID 128

//NPC_senses
#define	ALERT_CLEAR_TIME	200
#define CHECK_PVS		1
#define CHECK_360		2
#define CHECK_FOV		4
#define CHECK_SHOOT		8
#define CHECK_VISRANGE	16
qboolean CanSee ( gentity_t *ent );
qboolean InFOV ( gentity_t *ent, gentity_t *from, int hFOV, int vFOV );
qboolean InFOV2( vec3_t origin, gentity_t *from, int hFOV, int vFOV );
qboolean InFOV3( vec3_t spot, vec3_t from, vec3_t fromAngles, int hFOV, int vFOV );
qboolean InVisrange ( gentity_t *ent );

void SetTeamNumbers (void);
qboolean G_ActivateBehavior (gentity_t *self, int bset );

//NPC_combat
float IdealDistance ( gentity_t *self );

//g_utils
qboolean G_CheckInSolid (gentity_t *self, qboolean fix);

//MCG - End

void ClearPlayerAlertEvents( void );

qboolean G_BoundsOverlap(const vec3_t mins1, const vec3_t maxs1, const vec3_t mins2, const vec3_t maxs2);
