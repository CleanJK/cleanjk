#pragma once

#include "qcommon/q_shared.h"
#include "qcommon/qcommon.h"

// CVAR

/*

cvar_t variables are used to hold scalar or string variables that can be changed
or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present

Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

The are also occasionally used to communicated information between different
modules of the program.

*/

cvar_t *Cvar_Get( const char *var_name, const char *value, uint32_t flags, const char *var_desc=NULL );
// creates the variable if it doesn't exist, or returns the existing one
// if it exists, the value will not be changed, but flags will be ORed in
// that allows variables to be unarchived without needing bitflags
// if value is "", the value will not override a previously set value.

void	Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, uint32_t flags );
// basically a slightly modified Cvar_Get for the interpreted modules

void	Cvar_Update( vmCvar_t *vmCvar );
// updates an interpreted modules' version of a cvar

cvar_t	*Cvar_Set2(const char *var_name, const char *value, uint32_t defaultFlags, qboolean force);

cvar_t	*Cvar_Set( const char *var_name, const char *value );
// will create the variable with no flags if it doesn't exist

cvar_t	*Cvar_SetSafe( const char *var_name, const char *value );
// same as Cvar_Set, but doesn't force setting the value (respects CVAR_ROM, etc)

cvar_t	*Cvar_User_Set( const char *var_name, const char *value );
// same as Cvar_SetSafe, but defaults to CVAR_USER_CREATED

void	Cvar_Server_Set( const char *var_name, const char *value );
void	Cvar_VM_Set( const char *var_name, const char *value, vmSlots_t vmslot );
// sometimes we set variables from an untrusted source: fail if flags & CVAR_PROTECTED

cvar_t	*Cvar_SetValue( const char *var_name, float value );
void	Cvar_User_SetValue( const char *var_name, float value );
void	Cvar_VM_SetValue( const char *var_name, float value, vmSlots_t vmslot );
// expands value to a string and calls Cvar_Set/Cvar_User_Set/Cvar_VM_Set

float	Cvar_VariableValue( const char *var_name );
int		Cvar_VariableIntegerValue( const char *var_name );
// returns 0 if not defined or non numeric

char	*Cvar_VariableString( const char *var_name );
void	Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
// returns an empty string if not defined

uint32_t	Cvar_Flags(const char *var_name);
// returns CVAR_NONEXISTENT if cvar doesn't exist or the flags of that particular CVAR.

void	Cvar_CommandCompletion( completionCallback_t callback );
// callback with each valid string

void 	Cvar_Reset( const char *var_name );
void 	Cvar_ForceReset( const char *var_name );

void	Cvar_SetCheatState( void );
// reset all testing vars to a safe value

qboolean Cvar_Command( void );
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables( fileHandle_t f );
// writes lines containing "set variable value" for all variables
// with the archive flag set to true.

cvar_t *Cvar_Unset(cvar_t *cv);

void	Cvar_Init( void );

char	*Cvar_InfoString( int bit );
char	*Cvar_InfoString_Big( int bit );
// returns an info string containing all the cvars that have the given bit set
// in their flags ( CVAR_USERINFO, CVAR_SERVERINFO, CVAR_SYSTEMINFO, etc )
void	Cvar_InfoStringBuffer( int bit, char *buff, int buffsize );
void Cvar_CheckRange( cvar_t *cv, float minVal, float maxVal, qboolean shouldBeIntegral );

void	Cvar_Restart(qboolean unsetVM);
void	Cvar_Restart_f( void );

void Cvar_CompleteCvarName( char *args, int argNum );

char *Cvar_DescriptionString( const char *var_name );

extern uint32_t cvar_modifiedFlags;
// whenever a cvar is modifed, its flags will be OR'd into this, so
// a single check can determine if any CVAR_USERINFO, CVAR_SERVERINFO,
// etc, variables have been modified since the last check.  The bit
// can then be cleared to allow another change detection.
