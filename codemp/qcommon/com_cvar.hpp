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

#include "qcommon/q_shared.hpp"
#include "qcommon/q_common.hpp"

/*
cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly in C code

The user can access cvars from the console in three ways:
	r_draworder			prints the current value
	r_draworder 0		sets the current value to 0
	set r_draworder 0	as above, but creates the cvar if not present

Cvars are restricted from having the same names as commands to keep this interface from being ambiguous.
The are also occasionally used to communicated information between different modules of the program.
*/

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

// whenever a cvar is modifed, its flags will be OR'd into this, so a single check can determine if any CVAR_USERINFO, CVAR_SERVERINFO, etc, variables have been
//	modified since the last check.
// The bit can then be cleared to allow another change detection.
extern uint32_t  cvar_modifiedFlags;
extern cvar_t   *cvar_vars;

// ======================================================================
// FUNCTION
// ======================================================================

void      Cvar_CheckRange          ( cvar_t *cv, float minVal, float maxVal, bool shouldBeIntegral );
bool      Cvar_Command             ( void );
void      Cvar_CompleteCvarName    ( char *args, int argNum );
char     *Cvar_DescriptionString   ( const char *var_name );
uint32_t  Cvar_Flags               ( const char *var_name );
void      Cvar_ForceReset          ( const char *var_name );
cvar_t   *Cvar_Get                 ( const char *var_name, const char *value, uint32_t flags, const char *var_desc = nullptr );
char     *Cvar_InfoString          ( int bit );
char     *Cvar_InfoString_Big      ( int bit );
void      Cvar_InfoStringBuffer    ( int bit, char *buff, int buffsize );
void      Cvar_Init                ( void );
void      Cvar_Register            ( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, uint32_t flags );
void      Cvar_Reset               ( const char *var_name );
void      Cvar_Restart             ( bool unsetVM );
void      Cvar_Restart_f           ( void );
void      Cvar_Server_Set          ( const char *var_name, const char *value );
cvar_t   *Cvar_Set                 ( const char *var_name, const char *value );
cvar_t   *Cvar_Set2                ( const char *var_name, const char *value, uint32_t defaultFlags, bool force );
void      Cvar_SetCheatState       ( void );
cvar_t   *Cvar_SetSafe             ( const char *var_name, const char *value );
cvar_t   *Cvar_SetValue            ( const char *var_name, float value );
cvar_t   *Cvar_Unset               ( cvar_t *cv );
void      Cvar_Update              ( vmCvar_t *vmCvar );
cvar_t   *Cvar_User_Set            ( const char *var_name, const char *value );
void      Cvar_User_SetValue       ( const char *var_name, float value );
int       Cvar_VariableIntegerValue( const char *var_name );
char     *Cvar_VariableString      ( const char *var_name );
void      Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
float     Cvar_VariableValue       ( const char *var_name );
void      Cvar_VM_Set              ( const char *var_name, const char *value, vmSlots_e vmslot );
void      Cvar_VM_SetValue         ( const char *var_name, float value, vmSlots_e vmslot );
void      Cvar_WriteVariables      ( fileHandle_t f );
