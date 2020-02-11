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

// ICARUS Public Header File
// TODO: refactor this?

// ======================================================================
// FUNCTION
// ======================================================================

void ICARUS_Free(void* pMem);
void* ICARUS_Malloc(int iSize);

// ======================================================================
// INCLUDE
// ======================================================================

// we never want to include an icarus file before sharedentity_t is declared.
#include "game/g_public.hpp"

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern class CSequencer* gSequencers[MAX_GENTITIES];
extern class CTaskManager* gTaskManagers[MAX_GENTITIES];

// ======================================================================
// INCLUDE
// ======================================================================

#include "icarus/tokenizer.hpp"
#include "icarus/blockstream.hpp"
#include "icarus/interpreter.hpp"
#include "icarus/sequencer.hpp"
#include "icarus/taskmanager.hpp"
#include "icarus/instance.hpp"

// ======================================================================
// DEFINE
// ======================================================================

#define STL_ITERATE( a, b )		for ( a = b.begin(); a != b.end(); ++a )
#define STL_INSERT( a, b )		a.insert( a.end(), b );