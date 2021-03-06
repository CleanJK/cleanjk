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

// These are the functions that get replaced by game-specific ones (or StringEd code) so SGE can access files etc

// ======================================================================
// INCLUDE
// ======================================================================

#include <string>

// ======================================================================
// FUNCTION
// ======================================================================

int	SE_BuildFileList(const char* psStartDir, std::string& strResults);
unsigned char* SE_LoadFileData(const char* psFileName, int* piLoadedLength = 0);
void SE_FreeFileDataAfterLoad(unsigned char* psLoadedFile);
