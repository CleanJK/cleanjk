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

#include "client/cl_keycodes.hpp"
#include "qcommon/q_common.hpp"
#include "qcommon/com_inputField.hpp"

// ======================================================================
// DEFINE
// ======================================================================

#define COMMAND_HISTORY		32

// ======================================================================
// STRUCT
// ======================================================================

struct qkey_t {
	bool  down;
	int   repeats; // if > 1, it is autorepeating
	char *binding;
};

struct keyGlobals_t {
	bool   anykeydown;
	bool   overstrikeMode;
	int    keyDownCount;
	qkey_t keys[MAX_KEYS];
};

struct keyname_t {
	word        upper, lower;
	const char *name;
	int         keynum;
	bool        menukey;
};

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern InputField         chatField;
extern int             chat_playerNum;
extern keyGlobals_t    kg;
extern const keyname_t keynames[MAX_KEYS];
extern bool            chat_team;

// ======================================================================
// FUNCTION
// ======================================================================

void  Key_ClearStates       ( void );
char *Key_GetBinding        ( int keynum );
int   Key_GetKey            ( const char *binding );
bool  Key_GetOverstrikeMode ( void );
bool  Key_IsDown            ( int keynum );
void  Key_SetBinding        ( int keynum, const char *binding );
void  Key_SetOverstrikeMode ( bool state );
int   Key_StringToKeynum    ( char *str );
