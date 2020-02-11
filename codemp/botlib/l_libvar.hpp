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

// botlib vars

#pragma once

//library variable
struct libvar_t {
	char     *name;
	char     *string;
	int       flags;
	bool      modified;	// set each time the cvar is changed
	float     value;
	libvar_t *next;
};

void      LibVarDeAllocAll    ( void );
libvar_t *LibVarGet           ( char *var_name );
char     *LibVarGetString     ( char *var_name );
float     LibVarGetValue      ( char *var_name );
libvar_t *LibVar              ( char *var_name, char *value );
float     LibVarValue         ( char *var_name, char *value );
char     *LibVarString        ( char *var_name, char *value );
void      LibVarSet           ( char *var_name, char *value );
bool      LibVarChanged       ( char *var_name );
void      LibVarSetNotModified( char *var_name );
