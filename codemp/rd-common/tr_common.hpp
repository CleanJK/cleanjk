/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
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

#include "rd-common/tr_public.hpp"
#include "rd-common/tr_font.hpp"

// ======================================================================
// DEFINE
// ======================================================================

typedef void (*ImageLoaderFn)(const char* filename, byte** pic, int* width, int* height);

#ifdef _WIN32
	#define SWAPINTERVAL_FLAGS (CVAR_ARCHIVE_ND)
#else
	#define SWAPINTERVAL_FLAGS (CVAR_ARCHIVE_ND | CVAR_LATCH)
#endif

#ifdef _DEBUG
	#define MIN_PRIMITIVES -1
#else
	#define MIN_PRIMITIVES 0
#endif
#define MAX_PRIMITIVES 3

// surface geometry should not exceed these limits
#define	SHADER_MAX_VERTEXES	1000
#define	SHADER_MAX_INDEXES	(6*SHADER_MAX_VERTEXES)

// .MD3 triangle model file format
#define MD3_IDENT			(('3'<<24)+('P'<<16)+('D'<<8)+'I')
#define MD3_VERSION			15

// Light Style Constants
#define LS_NORMAL		0x00
#define LS_UNUSED		0xfe
#define	LS_LSNONE		0xff //rww - changed name because it unhappily conflicts with a lightsaber state name and changing this is just easier

// ======================================================================
// STRUCT
// ======================================================================

typedef struct md3Frame_s
{
	vec3_t		bounds[2];
	vec3_t		localOrigin;
	float		radius;
	char		name[16];
} md3Frame_t;

typedef struct md3Tag_s
{
	char		name[MAX_QPATH];	// tag name
	vec3_t		origin;
	matrix3_t	axis;
} md3Tag_t;

//	CHUNK			SIZE
//	header			sizeof( md3Surface_t )
//	shaders			sizeof( md3Shader_t ) * numShaders
//	triangles[0]	sizeof( md3Triangle_t ) * numTriangles
//	st				sizeof( md3St_t ) * numVerts
//	XyzNormals		sizeof( md3XyzNormal_t ) * numVerts * numFrames
typedef struct md3Surface_s
{
	int		ident;				//

	char	name[MAX_QPATH];	// polyset name

	int		flags;
	int		numFrames;			// all surfaces in a model should have the same

	int		numShaders;			// all surfaces in a model should have the same
	int		numVerts;

	int		numTriangles;
	int		ofsTriangles;

	int		ofsShaders;			// offset from start of md3Surface_t
	int		ofsSt;				// texture coords are common for all frames
	int		ofsXyzNormals;		// numVerts * numFrames

	int		ofsEnd;				// next surface follows
} md3Surface_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern refimport_t ri;

// ======================================================================
// FUNCTION
// ======================================================================

bool R_ImageLoader_Add(const char* extension, ImageLoaderFn imageLoader);
float GetNoiseTime(int t);
float R_NoiseGet4f(float x, float y, float z, float t);
int RE_SavePNG(const char* filename, byte* buf, size_t width, size_t height, int byteDepth);
size_t RE_SaveJPGToBuffer(byte* buffer, size_t bufSize, int quality, int image_width, int image_height, byte* image_buffer, int padding);
void LoadJPG(const char* filename, byte** pic, int* width, int* height);
void LoadPNG(const char* filename, byte** data, int* width, int* height);
void LoadTGA(const char* name, byte** pic, int* width, int* height);
void R_ImageLoader_Init();
void R_LoadImage(const char* shortname, byte** pic, int* width, int* height);
void R_NoiseInit(void);
void RE_SaveJPG(const char* filename, int quality, int image_width, int image_height, byte* image_buffer, int padding);