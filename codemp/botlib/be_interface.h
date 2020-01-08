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
// DEFINE
// ======================================================================

//#define DEBUG			//debug code
#define RANDOMIZE		//randomize bot behaviour

// ======================================================================
// STRUCT
// ======================================================================

//FIXME: get rid of this global structure
typedef struct botlib_globals_s
{
	int botlibsetup;						//true when the bot library has been setup
	int maxentities;						//maximum number of entities
	int maxclients;							//maximum number of clients
	float time;								//the global time
#ifdef DEBUG
	bool debug;							//true if debug is on
	int goalareanum;
	vec3_t goalorigin;
	int runai;
#endif
} botlib_globals_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern botlib_globals_t botlibglobals;
extern botlib_import_t botimport;
extern int botDeveloper;					//true if developer is on

// ======================================================================
// FUNCTION
// ======================================================================

int Sys_MilliSeconds(void);
