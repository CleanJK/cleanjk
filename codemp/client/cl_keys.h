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

#include "client/cl_keycodes.h"
#include "qcommon/q_common.h"

// ======================================================================
// DEFINE
// ======================================================================

#define COMMAND_HISTORY		32

// ======================================================================
// STRUCT
// ======================================================================

typedef struct qkey_s {
	bool	down;
	int			repeats; // if > 1, it is autorepeating
	char		*binding;
} qkey_t;

typedef struct keyGlobals_s {
	bool	anykeydown;
	bool	key_overstrikeMode;
	int			keyDownCount;

	qkey_t		keys[MAX_KEYS];
} keyGlobals_t;

typedef struct keyname_s {
	word		upper, lower;
	const char	*name;
	int			keynum;
	bool		menukey;
} keyname_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern field_t		chatField;
extern field_t		g_consoleField;
extern field_t		historyEditLines[COMMAND_HISTORY];
extern int			chat_playerNum;
extern int			historyLine;
extern int			nextHistoryLine;
extern keyGlobals_t	kg;
extern keyname_t	keynames[MAX_KEYS];
extern bool		chat_team;

// ======================================================================
// FUNCTION
// ======================================================================

char *		Key_GetBinding			( int keynum );
int			Key_GetKey				( const char *binding );
int			Key_StringToKeynum		( char *str );
bool	Key_GetOverstrikeMode	( void );
bool	Key_IsDown				( int keynum );
void		Key_ClearStates			( void );
void		Key_SetBinding			( int keynum, const char *binding );
void		Key_SetOverstrikeMode	( bool state );
void	Field_BigDraw		( field_t *edit, int x, int y, int width, bool showCursor, bool noColorEscape );
void	Field_CharEvent		( field_t *edit, int ch );
void	Field_Draw			( field_t *edit, int x, int y, int width, bool showCursor, bool noColorEscape );
void	Field_KeyDownEvent	( field_t *edit, int key );
