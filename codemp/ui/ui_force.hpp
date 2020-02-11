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
// INCLUDE
// ======================================================================

#include "qcommon/q_files.hpp"

// ======================================================================
// DEFINE
// ======================================================================

#define NUM_FORCE_STAR_IMAGES  9
#define FORCE_NONJEDI	0
#define FORCE_JEDI		1

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern bool gTouchedForce;
extern bool uiForcePowersDisabled[NUM_FORCE_POWERS];
extern int uiForceAvailable;
extern int uiForcePowerDarkLight[NUM_FORCE_POWERS];
extern int uiForcePowersRank[NUM_FORCE_POWERS];
extern int uiForceRank;
extern int uiForceSide;
extern int uiForceUsed;
extern int uiJediNonJedi;
extern int uiMaxRank;
extern int uiSaberColorShaders[NUM_SABER_COLORS];

// ======================================================================
// FUNCTION
// ======================================================================

bool UI_ForceMaxRank_HandleKey(int flags, float* special, int key, int num, int min, int max, int type);
bool UI_ForcePowerRank_HandleKey(int flags, float* special, int key, int num, int min, int max, int type);
bool UI_ForceSide_HandleKey(int flags, float* special, int key, int num, int min, int max, int type);
bool UI_JediNonJedi_HandleKey(int flags, float* special, int key, int num, int min, int max, int type);
bool UI_SkinColor_HandleKey(int flags, float* special, int key, int num, int min, int max, int type);
void UI_DrawForceStars(rectDef_t* rect, float scale, vec4_t color, int textStyle, int findex, int val, int min, int max);
void UI_DrawTotalForceStars(rectDef_t* rect, float scale, vec4_t color, int textStyle);
void UI_ForceConfigHandle(int oldindex, int newindex);
void UI_InitForceShaders(void);
void UI_ReadLegalForce(void);
void UI_SaveForceTemplate();
void UI_UpdateClientForcePowers(const char* teamArg);
void UI_UpdateForcePowers();