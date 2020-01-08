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

#pragma once

// ======================================================================
// DEFINE
// ======================================================================

//Script flags
#define	SCF_CROUCHED		0x00000001	//Force ucmd.upmove to be -127
#define	SCF_WALKING			0x00000002	//Force BUTTON_WALKING to be pressed
#define	SCF_MORELIGHT		0x00000004	//NPC will have a minlight of 96
#define	SCF_LEAN_RIGHT		0x00000008	//Force rightmove+BUTTON_USE
#define	SCF_LEAN_LEFT		0x00000010	//Force leftmove+BUTTON_USE
#define	SCF_RUNNING			0x00000020	//Takes off walking button, overrides SCF_WALKING
#define	SCF_ALT_FIRE		0x00000040	//Force to use alt-fire when firing
#define	SCF_NO_RESPONSE		0x00000080	//NPC will not do generic responses to being used
#define	SCF_FFDEATH			0x00000100	//Just tells player_die to run the friendly fire deathscript
#define	SCF_NO_COMBAT_TALK	0x00000200	//NPC will not use their generic combat chatter stuff
#define	SCF_CHASE_ENEMIES	0x00000400	//NPC chase enemies - FIXME: right now this is synonymous with using combat points... should it be?
#define	SCF_LOOK_FOR_ENEMIES	0x00000800	//NPC be on the lookout for enemies
#define	SCF_FACE_MOVE_DIR	0x00001000	//NPC face direction it's moving - FIXME: not really implemented right now
#define	SCF_IGNORE_ALERTS	0x00002000	//NPC ignore alert events
#define SCF_DONT_FIRE		0x00004000	//NPC won't shoot
#define	SCF_DONT_FLEE		0x00008000	//NPC never flees
#define	SCF_FORCED_MARCH	0x00010000	//NPC that the player must aim at to make him walk
#define	SCF_NO_GROUPS		0x00020000	//NPC cannot alert groups or be part of a group
#define	SCF_FIRE_WEAPON		0x00040000	//NPC will fire his (her) weapon
#define	SCF_NO_MIND_TRICK	0x00080000	//Not succeptible to mind tricks
#define	SCF_USE_CP_NEAREST	0x00100000	//Will use combat point close to it, not next to player or try and flank player
#define	SCF_NO_FORCE		0x00200000	//Not succeptible to force powers
#define	SCF_NO_FALLTODEATH	0x00400000	//NPC will not scream and tumble and fall to hit death over large drops
#define	SCF_NO_ACROBATICS	0x00800000	//Jedi won't jump, roll or cartwheel
#define	SCF_USE_SUBTITLES	0x01000000	//Regardless of subtitle setting, this NPC will display subtitles when it speaks lines
#define	SCF_NO_ALERT_TALK	0x02000000	//Will not say alert sounds, but still can be woken up by alerts

#define	MAX_ENEMY_POS_LAG	2400
#define	ENEMY_POS_LAG_INTERVAL	100
#define	ENEMY_POS_LAG_STEPS	(MAX_ENEMY_POS_LAG/ENEMY_POS_LAG_INTERVAL)

// ======================================================================
// FUNCTION
// ======================================================================

void G_LoadBoltOns(void);
