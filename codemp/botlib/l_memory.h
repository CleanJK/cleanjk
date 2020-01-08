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

//#define MEMDEBUG

// ======================================================================
// FUNCTION
// ======================================================================

#ifdef MEMDEBUG
#define GetMemory(size)				GetMemoryDebug(size, #size, __FILE__, __LINE__);
#define GetClearedMemory(size)		GetClearedMemoryDebug(size, #size, __FILE__, __LINE__);

void *GetMemoryDebug(unsigned long size, char *label, char *file, int line);
void *GetClearedMemoryDebug(unsigned long size, char *label, char *file, int line);

#define GetHunkMemory(size)			GetHunkMemoryDebug(size, #size, __FILE__, __LINE__);
#define GetClearedHunkMemory(size)	GetClearedHunkMemoryDebug(size, #size, __FILE__, __LINE__);

void *GetHunkMemoryDebug(unsigned long size, char *label, char *file, int line);
void *GetClearedHunkMemoryDebug(unsigned long size, char *label, char *file, int line);
#else
void *GetClearedHunkMemory(unsigned long size);
void *GetClearedMemory(unsigned long size);
void *GetHunkMemory(unsigned long size);
void *GetMemory(unsigned long size);
#endif

int AvailableMemory(void);
int MemoryByteSize(void* ptr);
void DumpMemory(void);
void FreeMemory(void* ptr);
void PrintMemoryLabels(void);
void PrintUsedMemorySize(void);