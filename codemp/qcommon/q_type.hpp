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

#include <cstdint>

// ======================================================================
// DEFINE
// ======================================================================

using byte = unsigned char;
using word = unsigned short;
using ulong = unsigned long;

using qhandle_t = int32_t;
using thandle_t = int32_t;
using fxHandle_t = int32_t;
using sfxHandle_t = int32_t;
using fileHandle_t = int32_t;
using clipHandle_t = int32_t;

using vec_t = float;
using ivec_t = int32_t;

using vec2_t = vec_t[2];
using vec3_t = vec_t[3];
using vec4_t = vec_t[4];
using vec5_t = vec_t[5];

using ivec2_t = ivec_t[2];
using ivec3_t = ivec_t[3];
using ivec4_t = ivec_t[4];
using ivec5_t = ivec_t[5];

using vec3pair_t = vec3_t[2];
using matrix3_t = vec3_t[3];

using cvarHandle_t = int;

using color4ub_t = byte[4];

using memtag_t = unsigned;