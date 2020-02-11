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

// font support
// This file is shared in the single and multiplayer codebases, so be CAREFUL WHAT YOU ADD/CHANGE!!!!!

// ======================================================================
// DEFINE
// ======================================================================

// Defines and structures required for fonts
#define GLYPH_COUNT			256

// ======================================================================
// STRUCT
// ======================================================================

typedef struct
{
	short		width;					// number of pixels wide
	short		height;					// number of scan lines
	short		horizAdvance;			// number of pixels to advance to the next char
	short		horizOffset;			// x offset into space to render glyph
	int			baseline;				// y offset
	float		s;						// x start tex coord
	float		t;						// y start tex coord
	float		s2;						// x end tex coord
	float		t2;						// y end tex coord
} glyphInfo_t;

// this file corresponds 1:1 with the "*.fontdat" files, so don't change it unless you're going to
//	recompile the fontgen util and regenerate all the fonts!
typedef struct dfontdat_s
{
	glyphInfo_t		mGlyphs[GLYPH_COUNT];

	short			mPointSize;
	short			mHeight;				// max height of font
	short			mAscender;
	short			mDescender;

	short			mKoreanHack;
} dfontdat_t;

// ======================================================================
// FUNCTION
// ======================================================================

bool Language_IsAsian(void);
bool Language_UsesSpaces(void);
float RE_Font_StrLenPixelsNew(const char* psText, const int iFontHandle, const float fScale = 1.0f);
int RE_Font_HeightPixels(const int iFontHandle, const float fScale = 1.0f);
int RE_Font_StrLenChars(const char* psText);
int RE_Font_StrLenPixels(const char* psText, const int iFontHandle, const float fScale = 1.0f);
int RE_RegisterFont(const char* psName);
void R_InitFonts(void);
void R_ShutdownFonts(void);
void RE_Font_DrawString(int ox, int oy, const char* psText, const float* rgba, const int iFontHandle, int iMaxPixelWidth, const float fScale = 1.0f);

// Dammit, I can't use this more elegant form because of !@#@!$%% VM code... (can't alter passed in ptrs, only contents of)
// unsigned int AnyLanguage_ReadCharFromString( const char **ppsText, bool *pbIsTrailingPunctuation = nullptr);
// so instead we have to use this messier method...
unsigned int AnyLanguage_ReadCharFromString(const char* psText, int* piAdvanceCount, bool* pbIsTrailingPunctuation = nullptr);