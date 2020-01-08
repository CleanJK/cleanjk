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

// Siege module, shared for game, cgame, and ui.

#include "qcommon/q_shared.h"
#include "game/bg_public.h"
#include "game/bg_weapons.h"

#ifdef _GAME
	#include "game/g_local.h"
#elif _CGAME
	#include "cgame/cg_local.h"
#elif UI_BUILD
	#include "ui/ui_local.h"
#endif

//saber stances
stringID_table_t StanceTable[] =
{
	ENUM2STRING(SS_NONE),
	ENUM2STRING(SS_FAST),
	ENUM2STRING(SS_MEDIUM),
	ENUM2STRING(SS_STRONG),
	ENUM2STRING(SS_DUAL),
	ENUM2STRING(SS_STAFF),
	{"", 0}
};

//Weapon and force power tables are also used in NPC parsing code and some other places.
stringID_table_t WPTable[] =
{
	{"NULL",WP_NONE},
	ENUM2STRING(WP_NONE),
	// Player weapons
	ENUM2STRING(WP_STUN_BATON),
	ENUM2STRING(WP_MELEE),
	ENUM2STRING(WP_SABER),
	ENUM2STRING(WP_BRYAR_PISTOL),
	{"WP_BLASTER_PISTOL", WP_BRYAR_PISTOL},
	ENUM2STRING(WP_BLASTER),
	ENUM2STRING(WP_DISRUPTOR),
	ENUM2STRING(WP_BOWCASTER),
	ENUM2STRING(WP_REPEATER),
	ENUM2STRING(WP_DEMP2),
	ENUM2STRING(WP_FLECHETTE),
	ENUM2STRING(WP_ROCKET_LAUNCHER),
	ENUM2STRING(WP_THERMAL),
	ENUM2STRING(WP_TRIP_MINE),
	ENUM2STRING(WP_DET_PACK),
	ENUM2STRING(WP_CONCUSSION),
	ENUM2STRING(WP_BRYAR_OLD),
	ENUM2STRING(WP_EMPLACED_GUN),
	ENUM2STRING(WP_TURRET),
	{"", 0}
};

stringID_table_t FPTable[] =
{
	ENUM2STRING(FP_HEAL),
	ENUM2STRING(FP_LEVITATION),
	ENUM2STRING(FP_SPEED),
	ENUM2STRING(FP_PUSH),
	ENUM2STRING(FP_PULL),
	ENUM2STRING(FP_TELEPATHY),
	ENUM2STRING(FP_GRIP),
	ENUM2STRING(FP_LIGHTNING),
	ENUM2STRING(FP_RAGE),
	ENUM2STRING(FP_PROTECT),
	ENUM2STRING(FP_ABSORB),
	ENUM2STRING(FP_TEAM_HEAL),
	ENUM2STRING(FP_TEAM_FORCE),
	ENUM2STRING(FP_DRAIN),
	ENUM2STRING(FP_SEE),
	ENUM2STRING(FP_SABER_OFFENSE),
	ENUM2STRING(FP_SABER_DEFENSE),
	ENUM2STRING(FP_SABERTHROW),
	{"",	-1}
};

stringID_table_t HoldableTable[] =
{
	ENUM2STRING(HI_NONE),

	ENUM2STRING(HI_SEEKER),
	ENUM2STRING(HI_SHIELD),
	ENUM2STRING(HI_MEDPAC),
	ENUM2STRING(HI_MEDPAC_BIG),
	ENUM2STRING(HI_BINOCULARS),
	ENUM2STRING(HI_SENTRY_GUN),
	ENUM2STRING(HI_JETPACK),
	ENUM2STRING(HI_HEALTHDISP),
	ENUM2STRING(HI_AMMODISP),
	ENUM2STRING(HI_EWEB),
	ENUM2STRING(HI_CLOAK),

	{"", -1}
};

stringID_table_t PowerupTable[] =
{
	ENUM2STRING(PW_NONE),
	ENUM2STRING(PW_QUAD),
	ENUM2STRING(PW_BATTLESUIT),
	ENUM2STRING(PW_PULL),
	ENUM2STRING(PW_REDFLAG),
	ENUM2STRING(PW_BLUEFLAG),
	ENUM2STRING(PW_NEUTRALFLAG),
	ENUM2STRING(PW_SHIELDHIT),
	ENUM2STRING(PW_SPEEDBURST),
	ENUM2STRING(PW_DISINT_4),
	ENUM2STRING(PW_SPEED),
	ENUM2STRING(PW_CLOAKED),
	ENUM2STRING(PW_FORCE_ENLIGHTENED_LIGHT),
	ENUM2STRING(PW_FORCE_ENLIGHTENED_DARK),
	ENUM2STRING(PW_FORCE_BOON),
	ENUM2STRING(PW_YSALAMIRI),

	{"", -1}
};
