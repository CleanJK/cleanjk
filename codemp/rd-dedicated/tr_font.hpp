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

// font support
// This file is shared in the single and multiplayer codebases, so be CAREFUL WHAT YOU ADD/CHANGE!!!!!



bool         Language_IsAsian              ( void );
bool         Language_UsesSpaces           ( void );
float        RE_Font_HeightPixels          ( const JKFont font = JKFont::Medium, const float fScale = 1.0f );
int          RE_Font_StrLenChars           ( const char *psText );
float        RE_Font_StrLenPixels          ( const char *psText, const JKFont font = JKFont::Medium, const float fScale = 1.0f );
JKFont       RE_RegisterFont               ( const JKFont font, const char *path );
void         R_InitFonts                   ( void );
void         R_ShutdownFonts               ( void );
void         RE_Font_DrawString            ( int ox, int oy, const char *psText, const float *rgba, const JKFont font, int iMaxPixelWidth, const float fScale = 1.0f );
unsigned int AnyLanguage_ReadCharFromString( const char *psText, int *piAdvanceCount, bool *pbIsTrailingPunctuation = nullptr );
