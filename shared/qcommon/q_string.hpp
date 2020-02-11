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

// ======================================================================
// INCLUDE
// ======================================================================

#include "qcommon/q_platform.h"

// ======================================================================
// FUNCTION
// ======================================================================

#if defined(__cplusplus)
extern "C" {
#endif

bool Q_isanumber(const char* s);
bool Q_isintegral(float f);
char* Q_CleanStr(char* string);
char* Q_strlwr(char* s1);
char* Q_strrchr(const char* string, int c);
char* Q_strupr(char* s1);
const char* Q_strchrs(const char* string, const char* search);
const char* Q_stristr(const char* s, const char* find);
int Q_isalpha(int c);
int Q_isgraph(int c);
int Q_islower(int c);
int Q_isprint(int c);
int Q_isprintext(int c);
int Q_isupper(int c);
int Q_PrintStrlen(const char* string);
int Q_stricmp(const char* s1, const char* s2);
int	Q_stricmpn(const char* s1, const char* s2, int n);
int	Q_strncmp(const char* s1, const char* s2, int n);
void Q_strcat(char* dest, int size, const char* src);
void Q_StripColor(char* text);
void Q_strncpyz(char* dest, const char* src, int destsize);
void Q_strstrip(char* string, const char* strip, const char* repl);

#if defined (_MSC_VER)
	int Q_vsnprintf( char *str, size_t size, const char *format, va_list args );
#else // not using MSVC
	#define Q_vsnprintf vsnprintf
#endif

#if defined(__cplusplus)
} // extern "C"
#endif
