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
// STRUCT
// ======================================================================

//library variable
typedef struct libvar_s
{
	char* name;
	char* string;
	int		flags;
	bool	modified;	// set each time the cvar is changed
	float		value;
	struct	libvar_s* next;
} libvar_t;

// ======================================================================
// FUNCTION
// ======================================================================

bool LibVarChanged(char* var_name);
char* LibVarGetString(char* var_name);
char* LibVarString(char* var_name, char* value);
float LibVarGetValue(char* var_name);
float LibVarValue(char* var_name, char* value);
libvar_t* LibVar(char* var_name, char* value);
libvar_t* LibVarGet(char* var_name);
void LibVarDeAllocAll(void);
void LibVarSet(char* var_name, char* value);
void LibVarSetNotModified(char* var_name);