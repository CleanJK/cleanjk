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

#include <map>
#include <string>

struct pscript_t {
	char	*buffer;
	long	length;
};

using entlist_t = std::map <std::string, int>;
using bufferlist_t = std::map <std::string, pscript_t*>;

//ICARUS includes
#include "interface.hpp"

extern interface_export_t	interface_export;
extern int		ICARUS_entFilter;
extern class ICARUS_Instance	*iICARUS;
extern bufferlist_t		ICARUS_BufferList;
extern entlist_t		ICARUS_EntList;

int  ICARUS_RunScript( sharedEntity_t *ent, const char *name );
bool ICARUS_RegisterScript( const char *name, bool bCalledDuringInterrogate = false);
void ICARUS_Init( void );
bool ICARUS_ValidEnt( sharedEntity_t *ent );
void ICARUS_InitEnt( sharedEntity_t *ent );
void ICARUS_FreeEnt( sharedEntity_t *ent );
void ICARUS_AssociateEnt( sharedEntity_t *ent );
void ICARUS_Shutdown( void );
int ICARUS_GetScript( const char *name, char **buf );
int ICARUS_LinkEntity( int entID, class CSequencer *sequencer, class CTaskManager *taskManager );
void Interface_Init( interface_export_t *pe );
void Svcmd_ICARUS_f( void );

