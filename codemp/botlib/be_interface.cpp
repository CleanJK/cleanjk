/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors

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

// bot library interface

#include "qcommon/q_shared.h"
#include "l_memory.h"
#include "l_log.h"
#include "l_libvar.h"
#include "l_script.h"
#include "l_precomp.h"
#include "l_struct.h"
#include "botlib.h"
#include "be_interface.h"

#include "be_ea.h"

//library globals in a structure
botlib_globals_t botlibglobals;

botlib_export_t be_botlib_export;
botlib_import_t botimport;
int botDeveloper;
//true if the library is setup
int botlibsetup = qfalse;

int Sys_MilliSeconds(void)
{
	return clock() * 1000 / CLOCKS_PER_SEC;
} //end of the function Sys_MilliSeconds
qboolean ValidClientNumber(int num, char *str)
{
	if (num < 0 || num > botlibglobals.maxclients)
	{
		//weird: the disabled stuff results in a crash
		botimport.Print(PRT_ERROR, "%s: invalid client number %d, [0, %d]\n",
										str, num, botlibglobals.maxclients);
		return qfalse;
	} //end if
	return qtrue;
} //end of the function BotValidateClientNumber
qboolean ValidEntityNumber(int num, char *str)
{
	if (num < 0 || num > botlibglobals.maxentities)
	{
		botimport.Print(PRT_ERROR, "%s: invalid entity number %d, [0, %d]\n",
										str, num, botlibglobals.maxentities);
		return qfalse;
	} //end if
	return qtrue;
} //end of the function BotValidateClientNumber
qboolean BotLibSetup(char *str)
{
	if (!botlibglobals.botlibsetup)
	{
		botimport.Print(PRT_ERROR, "%s: bot library used before being setup\n", str);
		return qfalse;
	} //end if
	return qtrue;
} //end of the function BotLibSetup

int Export_BotLibSetup(void)
{
	int		errnum;

	botDeveloper = LibVarGetValue("bot_developer");
  memset( &botlibglobals, 0, sizeof(botlibglobals) ); // bk001207 - init
	//initialize byte swapping (litte endian etc.)
//	Swap_Init();

	if(botDeveloper)
	{
		char *homedir, *gamedir, *basedir;
		char logfilename[MAX_OSPATH];

		homedir = LibVarGetString("homedir");
		gamedir = LibVarGetString("gamedir");
		basedir = LibVarGetString("com_basegame");

		if (*homedir)
		{
			if(*gamedir)
				Com_sprintf(logfilename, sizeof(logfilename), "%s%c%s%cbotlib.log", homedir, PATH_SEP, gamedir, PATH_SEP);
			else if(*basedir)
				Com_sprintf(logfilename, sizeof(logfilename), "%s%c%s%cbotlib.log", homedir, PATH_SEP, basedir, PATH_SEP);
			else
				Com_sprintf(logfilename, sizeof(logfilename), "%s%c" "base" "%cbotlib.log", homedir, PATH_SEP, PATH_SEP); //fixme use BASEGAME define
		}
		else
			Com_sprintf(logfilename, sizeof(logfilename), "botlib.log");

		Log_Open(logfilename);
	}
//	botimport.Print(PRT_MESSAGE, "------- BotLib Initialization -------\n");
	botlibglobals.maxclients = (int) LibVarValue("maxclients", "128");
	botlibglobals.maxentities = (int) LibVarValue("maxentities", "1024");

	errnum = EA_Setup();			//be_ea.c
	if (errnum != BLERR_NOERROR) return errnum;
	/*
	errnum = BotSetupWeaponAI();	//be_ai_weap.c
	if (errnum != BLERR_NOERROR)return errnum;
	errnum = BotSetupGoalAI();		//be_ai_goal.c
	if (errnum != BLERR_NOERROR) return errnum;
	errnum = BotSetupChatAI();		//be_ai_chat.c
	if (errnum != BLERR_NOERROR) return errnum;
	errnum = BotSetupMoveAI();		//be_ai_move.c
	if (errnum != BLERR_NOERROR) return errnum;
	*/
	botlibsetup = qtrue;
	botlibglobals.botlibsetup = qtrue;

	return BLERR_NOERROR;
} //end of the function Export_BotLibSetup
int Export_BotLibShutdown(void)
{
	if (!BotLibSetup("BotLibShutdown")) return BLERR_LIBRARYNOTSETUP;
#ifndef DEMO
	//DumpFileCRCs();
#endif //DEMO
	//shut down bot elemantary actions
	EA_Shutdown();
	//free all libvars
	LibVarDeAllocAll();
	//remove all global defines from the pre compiler
	PC_RemoveAllGlobalDefines();

	//dump all allocated memory
//	DumpMemory();
#ifdef DEBUG
	PrintMemoryLabels();
#endif
	//shut down library log file
	Log_Shutdown();

	botlibsetup = qfalse;
	botlibglobals.botlibsetup = qfalse;
	// print any files still open
	PC_CheckOpenSourceHandles();

	return BLERR_NOERROR;
} //end of the function Export_BotLibShutdown
int Export_BotLibVarSet(char *var_name, char *value)
{
	LibVarSet(var_name, value);
	return BLERR_NOERROR;
} //end of the function Export_BotLibVarSet

int Export_BotLibVarGet(char *var_name, char *value, int size)
{
	char *varvalue;

	varvalue = LibVarGetString(var_name);
	strncpy(value, varvalue, size-1);
	value[size-1] = '\0';
	return BLERR_NOERROR;
} //end of the function Export_BotLibVarGet

int BotFuzzyPointReachabilityArea(vec3_t origin);

float BotGapDistance(vec3_t origin, vec3_t hordir, int entnum);

static void Init_EA_Export( ea_export_t *ea ) {
	//ClientCommand elementary actions
	ea->EA_Command = EA_Command;
	ea->EA_Say = EA_Say;
	ea->EA_SayTeam = EA_SayTeam;

	ea->EA_Action = EA_Action;
	ea->EA_Gesture = EA_Gesture;
	ea->EA_Talk = EA_Talk;
	ea->EA_Attack = EA_Attack;
	ea->EA_Alt_Attack = EA_Alt_Attack;
	ea->EA_ForcePower = EA_ForcePower;
	ea->EA_Use = EA_Use;
	ea->EA_Respawn = EA_Respawn;
	ea->EA_Crouch = EA_Crouch;
	ea->EA_MoveUp = EA_MoveUp;
	ea->EA_MoveDown = EA_MoveDown;
	ea->EA_MoveForward = EA_MoveForward;
	ea->EA_MoveBack = EA_MoveBack;
	ea->EA_MoveLeft = EA_MoveLeft;
	ea->EA_MoveRight = EA_MoveRight;

	ea->EA_SelectWeapon = EA_SelectWeapon;
	ea->EA_Jump = EA_Jump;
	ea->EA_DelayedJump = EA_DelayedJump;
	ea->EA_Move = EA_Move;
	ea->EA_View = EA_View;
	ea->EA_GetInput = EA_GetInput;
	ea->EA_EndRegular = EA_EndRegular;
	ea->EA_ResetInput = EA_ResetInput;
}

botlib_export_t *GetBotLibAPI(int apiVersion, botlib_import_t *import) {
	assert(import);   // bk001129 - this wasn't set for base/
  botimport = *import;
  assert(botimport.Print);   // bk001129 - pars pro toto

	Com_Memset( &be_botlib_export, 0, sizeof( be_botlib_export ) );

	if ( apiVersion != BOTLIB_API_VERSION ) {
		botimport.Print( PRT_ERROR, "Mismatched BOTLIB_API_VERSION: expected %i, got %i\n", BOTLIB_API_VERSION, apiVersion );
		return NULL;
	}

	Init_EA_Export(&be_botlib_export.ea);

	be_botlib_export.BotLibSetup = Export_BotLibSetup;
	be_botlib_export.BotLibShutdown = Export_BotLibShutdown;
	be_botlib_export.BotLibVarSet = Export_BotLibVarSet;
	be_botlib_export.BotLibVarGet = Export_BotLibVarGet;

	be_botlib_export.PC_AddGlobalDefine = PC_AddGlobalDefine;
	be_botlib_export.PC_LoadSourceHandle = PC_LoadSourceHandle;
	be_botlib_export.PC_FreeSourceHandle = PC_FreeSourceHandle;
	be_botlib_export.PC_ReadTokenHandle = PC_ReadTokenHandle;
	be_botlib_export.PC_SourceFileAndLine = PC_SourceFileAndLine;
	be_botlib_export.PC_LoadGlobalDefines = PC_LoadGlobalDefines;
	be_botlib_export.PC_RemoveAllGlobalDefines = PC_RemoveAllGlobalDefines;

	return &be_botlib_export;
}
