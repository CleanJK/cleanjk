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

#include "qcommon/cm_polylib.h"
#include "qcommon/cm_public.h"
#include "qcommon/q_common.h"

// ======================================================================
// DEFINE
// ======================================================================

#define	MAX_SUBMODELS			512
#define	BOX_MODEL_HANDLE		(MAX_SUBMODELS-1)
#define CAPSULE_MODEL_HANDLE	(MAX_SUBMODELS-2)

// keep 1/8 unit away to keep the position valid before network snapping
// and to avoid various numeric issues
#define	SURFACE_CLIP_EPSILON	(0.125)

// ======================================================================
// STRUCT
// ======================================================================

typedef struct cNode_s {
	cplane_t	*plane;
	int			children[2];		// negative numbers are leafs
} cNode_t;

typedef struct cLeaf_s {
	int			cluster;
	int			area;

	ptrdiff_t	firstLeafBrush;
	int			numLeafBrushes;

	ptrdiff_t	firstLeafSurface;
	int			numLeafSurfaces;
} cLeaf_t;

typedef struct cmodel_s {
	vec3_t		mins, maxs;
	cLeaf_t		leaf;			// submodels don't reference the main tree
	int			firstNode;		// only for cmodel[0] (for the main and bsp instances)
} cmodel_t;

typedef struct cbrushside_s {
	cplane_t	*plane;
	int			shaderNum;
} cbrushside_t;

typedef struct cbrush_s {
	int					shaderNum;		// the shader that determined the contents
	int					contents;
	vec3_t				bounds[2];
	cbrushside_t		*sides;
	unsigned short		numsides;
	unsigned short		checkcount;		// to avoid repeated testings
} cbrush_t;

typedef struct cPatch_s
{
	int			checkcount;				// to avoid repeated testings
	int			surfaceFlags;
	int			contents;
	struct patchCollide_s* pc;
} cPatch_t;

typedef struct cArea_s
{
	int			floodnum;
	int			floodvalid;
} cArea_t;

// Used for oriented capsule collision detection
typedef struct sphere_s
{
	bool	use;
	float		radius;
	float		halfheight;
	vec3_t		offset;
} sphere_t;

typedef struct traceWork_s
{ //rwwRMG - modified
	vec3_t		start;
	vec3_t		end;
	vec3_t		size[2];	// size of the box being swept through the model
	vec3_t		offsets[8];	// [signbits][x] = either size[0][x] or size[1][x]
	float		maxOffset;	// longest corner length from origin
	vec3_t		extents;	// greatest of abs(size[0]) and abs(size[1])
	vec3_t		modelOrigin;// origin of the model tracing through
	int			contents;	// ored contents of the model tracing through
	bool	isPoint;	// optimized case
//	trace_t		trace;		// returned from trace call
	sphere_t	sphere;		// sphere for oriendted capsule collision

	//rwwRMG - added:
	vec3pair_t		bounds;			// enclosing box of start and end surrounding by size
	vec3pair_t		localBounds;	// enclosing box of start and end surrounding by size for a segment

	float			baseEnterFrac;	// global enter fraction (before processing subsections of the brush)
	float			baseLeaveFrac;	// global leave fraction (before processing subsections of the brush)
	float			enterFrac;		// fraction where the ray enters the brush
	float			leaveFrac;		// fraction where the ray leaves the brush
	cbrushside_t* leadside;
	cplane_t* clipplane;
	bool			startout;
	bool			getout;

} traceWork_t;

typedef struct leafList_s
{
	int		count;
	int		maxcount;
	bool	overflowed;
	int* list;
	vec3_t	bounds[2];
	int		lastLeaf;		// for overflows where each leaf can't be stored individually
	void	(*storeLeafs)(struct leafList_s* ll, int nodenum);
} leafList_t;

// ======================================================================
// CLASS / STRUCT
// ======================================================================

class CCMShader
{
public:
	char		shader[MAX_QPATH];
	class CCMShader	*mNext;
	int			surfaceFlags;
	int			contentFlags;

	const char	*GetName(void) const { return(shader); }
	class CCMShader *GetNext(void) const { return(mNext); }
	void	SetNext(class CCMShader *next) { mNext = next; }
	void	Destroy(void) { }
};

typedef struct clipMap_s
{
	char		name[MAX_QPATH];

	int			numShaders;
	CCMShader* shaders;

	int			numBrushSides;
	cbrushside_t* brushsides;

	int			numPlanes;
	cplane_t* planes;

	int			numNodes;
	cNode_t* nodes;

	int			numLeafs;
	cLeaf_t* leafs;

	int			numLeafBrushes;
	int* leafbrushes;

	int			numLeafSurfaces;
	int* leafsurfaces;

	int			numSubModels;
	cmodel_t* cmodels;

	int			numBrushes;
	cbrush_t* brushes;

	int			numClusters;
	int			clusterBytes;
	byte* visibility;
	bool	vised;			// if false, visibility is just a single cluster of ffs

	int			numEntityChars;
	char* entityString;

	int			numAreas;
	cArea_t* areas;
	int* areaPortals;	// [ numAreas*numAreas ] reference counts

	int			numSurfaces;
	cPatch_t** surfaces;			// non-patches will be nullptr

	int			floodvalid;
	int			checkcount;					// incremented on each trace
} clipMap_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern	clipMap_t	cmg; //rwwRMG - changed from cm
extern	int			c_pointcontents;
extern	int			c_traces, c_brush_traces, c_patch_traces;

// ======================================================================
// FUNCTION
// ======================================================================

CCMShader *CM_GetShaderInfo( const char *name );
CCMShader *CM_GetShaderInfo( int shaderNum );
cmodel_t *CM_ClipHandleToModel( clipHandle_t handle, clipMap_t **clipMap = 0 );
bool CM_PositionTestInPatchCollide( traceWork_t *tw, const struct patchCollide_s *pc );
struct patchCollide_s	*CM_GeneratePatchCollide( int width, int height, vec3_t *points );
void CM_BoxLeafnums_r( leafList_t *ll, int nodenum );
void CM_ClearLevelPatches( void );
void CM_FloodAreaConnections( clipMap_t &cm );
void CM_GetModelFormalName ( const char* model, const char* skin, char* name, int size );
void CM_GetWorldBounds ( vec3_t mins, vec3_t maxs );
void CM_InitBoxHull (void);
void CM_SetupShaderProperties( void );
void CM_ShutdownShaderProperties(void);
void CM_StoreBrushes( leafList_t *ll, int nodenum );
void CM_StoreLeafs( leafList_t *ll, int nodenum );
void CM_TraceThroughPatchCollide( traceWork_t *tw, trace_t &trace, const struct patchCollide_s *pc );
