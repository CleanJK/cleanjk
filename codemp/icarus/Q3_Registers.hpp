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
// ENUM
// ======================================================================

enum
{
	VTYPE_NONE = 0,
	VTYPE_FLOAT,
	VTYPE_STRING,
	VTYPE_VECTOR,
};

#if !defined( BUILD_GAME ) && !defined( BUILD_CGAME ) && !defined( BUILD_UI )

// ======================================================================
// DEFINE
// ======================================================================

#define	MAX_VARIABLES	32

using varString_m = std::map<std::string, std::string>;
using varFloat_m = std::map<std::string, float>;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern varFloat_m varFloats;
extern varString_m varStrings;
extern varString_m varVectors;

// ======================================================================
// FUNCTION
// ======================================================================

int Q3_GetFloatVariable(const char* name, float* value);
int Q3_GetStringVariable(const char* name, const char** value);
int Q3_GetVectorVariable(const char* name, vec3_t value);
int Q3_SetFloatVariable(const char* name, float value);
int Q3_SetStringVariable(const char* name, const char* value);
int Q3_SetVectorVariable(const char* name, const char* value);
int Q3_VariableDeclared(const char* name);
void Q3_DeclareVariable(int type, const char* name);
void Q3_FreeVariable(const char* name);
void Q3_InitVariables(void);

#endif // !defined( BUILD_GAME ) && !defined( BUILD_CGAME ) && !defined( BUILD_UI )
