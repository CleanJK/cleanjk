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


#include "qcommon/q_shared.hpp"
#include "qcommon/q_color.hpp"
#include "ui/menudef.h"

//NOTE: this file is public to cgame/ui

float Text_Width(
	const Text &text,
	const char *str
);

float Text_Height(
	const Text &text,
	const char *str = nullptr // doesn't not actually read `text`
);

void Text_Paint(
	const Text &text,
	float x,
	float y,
	const char *str,
	const vec4_t colour = colorTable[CT_WHITE],
	int style = ITEM_TEXTSTYLE_NORMAL,
	int limit = 0,
	float adjust = 0.0f,
	int cursorPos = -1,
	bool overstrike = false
);

// maxX param is initially an X limit, but is also used as feedback. 0 = text was clipped to fit within, else maxX = next pos
void TextHelper_Paint_Limit(
	float *maxX,
	float x,
	float y,
	float scale,
	const vec4_t color,
	const char *str,
	float adjust,
	int limit,
	JKFont font
);

void TextHelper_Paint_Proportional(
	int x,
	int y,
	const char *str,
	int style,
	const vec4_t color,
	JKFont font = JKFont::Medium,
	float scale = 1.0f
);
