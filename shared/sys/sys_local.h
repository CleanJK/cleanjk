/*
===========================================================================
Copyright (C) 2005 - 2015, ioquake3 contributors
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

#include "qcommon/q_common.h"

// ======================================================================
// STRUCT
// ======================================================================

struct UnpackDLLResult
{
	bool succeeded;
	char* tempDLLPath;
};

// ======================================================================
// FUNCTION
// ======================================================================

bool Sys_DLLNeedsUnpacking();
bool Sys_GetPacket(netadr_t* net_from, msg_t* net_message);
char* Sys_ConsoleInput(void);
UnpackDLLResult Sys_UnpackDLL(const char* name);
void IN_Frame(void);
void IN_Init(void* windowData);
void IN_Restart(void);
void IN_Shutdown(void);
void Sys_PlatformExit(void);
void Sys_PlatformInit(void);
void Sys_QueEvent(int time, sysEventType_t type, int value, int value2, int ptrLength, void* ptr);
void Sys_SigHandler(int signal);

#ifndef _WIN32
void Sys_AnsiColorPrint(const char* msg);
#endif
