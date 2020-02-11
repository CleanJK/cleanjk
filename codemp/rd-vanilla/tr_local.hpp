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

#include "rd-common/tr_public.hpp"
#include "rd-common/tr_common.hpp"
#include "ghoul2/ghoul2_shared.hpp" //rwwRMG - added
#include "rd-vanilla/qgl.hpp"
#include "rd-common/tr_types.hpp"



#define RB_CHECKOVERFLOW( v, i )	if (tess.numVertexes + (v) >= SHADER_MAX_VERTEXES || tess.numIndexes + (i) >= SHADER_MAX_INDEXES ) {RB_CheckOverflow(v,i);}

#define CONTENTS_NODE            -1
#define DRAWSURF_MASK            (MAX_DRAWSURFS-1)
#define FOG_TABLE_SIZE           256
#define FUNCTABLE_MASK           (FUNCTABLE_SIZE-1)
#define FUNCTABLE_SIZE           1024
#define FUNCTABLE_SIZE2          10
#define GL_INDEX_TYPE            GL_UNSIGNED_INT
#define LIGHTMAP_2D              -4 // shader is for 2D rendering
#define LIGHTMAP_BY_VERTEX       -3 // pre-lit triangle models
#define LIGHTMAP_NONE            -1
#define LIGHTMAP_WHITEIMAGE      -2
#define MAX_DRAWIMAGES           2048
#define MAX_DRAWSURFS            0x10000
#define MAX_FACE_POINTS          64
#define MAX_GRID_SIZE            65 // max dimensions of a grid mesh in memory
#define MAX_HITMAT_ENTRIES       1000
#define MAX_LIGHTMAPS            256
#define MAX_MOD_KNOWN            1024
#define MAX_PATCH_SIZE           32 // max dimensions of a patch mesh in map file
#define MAX_RENDER_COMMANDS      0x40000
#define MAX_SHADER_DEFORMS       3
#define MAX_SHADER_STAGES        8
#define MAX_SHADERS              (1<<SHADERNUM_BITS)
#define MAX_SKINS                1024
#define MAX_STATE_NAME           32
#define MAX_STATES_PER_SHADER    32
#define MD3_XYZ_SCALE            (1.0/64)
#define NUM_SCRATCH_IMAGES       16
#define NUM_TEX_COORDS           (MAXLIGHTMAPS+1)
#define NUM_TEXTURE_BUNDLES      2
#define QSORT_FOGNUM_SHIFT       2
#define QSORT_REFENTITYNUM_SHIFT 7
#define QSORT_SHADERNUM_SHIFT    (QSORT_REFENTITYNUM_SHIFT+REFENTITYNUM_BITS)
#define SET_MASK                 0x00ffffff
#define SHADERNUM_BITS           14 // 14 bits. can't be increased without changing bit packing for drawsurfs (see QSORT_SHADERNUM_SHIFT)
#define TR_MAX_TEXMODS           4
#define VERTEX_COLOR             (5+(MAXLIGHTMAPS*2))
#define VERTEX_FINAL_COLOR       (5+(MAXLIGHTMAPS*3))
#define VERTEX_LM                5
#define VERTEXSIZE               (6+(MAXLIGHTMAPS*3))

#define GLS_SRCBLEND_ZERO						0x00000001
#define GLS_SRCBLEND_ONE						0x00000002
#define GLS_SRCBLEND_DST_COLOR					0x00000003
#define GLS_SRCBLEND_ONE_MINUS_DST_COLOR		0x00000004
#define GLS_SRCBLEND_SRC_ALPHA					0x00000005
#define GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA		0x00000006
#define GLS_SRCBLEND_DST_ALPHA					0x00000007
#define GLS_SRCBLEND_ONE_MINUS_DST_ALPHA		0x00000008
#define GLS_SRCBLEND_ALPHA_SATURATE				0x00000009
#define GLS_SRCBLEND_BITS						0x0000000f
#define GLS_DSTBLEND_ZERO						0x00000010
#define GLS_DSTBLEND_ONE						0x00000020
#define GLS_DSTBLEND_SRC_COLOR					0x00000030
#define GLS_DSTBLEND_ONE_MINUS_SRC_COLOR		0x00000040
#define GLS_DSTBLEND_SRC_ALPHA					0x00000050
#define GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA		0x00000060
#define GLS_DSTBLEND_DST_ALPHA					0x00000070
#define GLS_DSTBLEND_ONE_MINUS_DST_ALPHA		0x00000080
#define GLS_DSTBLEND_BITS						0x000000f0
#define GLS_DEPTHMASK_TRUE						0x00000100
#define GLS_POLYMODE_LINE						0x00001000
#define GLS_DEPTHTEST_DISABLE					0x00010000
#define GLS_DEPTHFUNC_EQUAL						0x00020000
#define GLS_ATEST_GT_0							0x10000000
#define GLS_ATEST_LT_80							0x20000000
#define GLS_ATEST_GE_80							0x40000000
#define GLS_ATEST_GE_C0							0x80000000
#define GLS_ATEST_BITS							0xF0000000
#define GLS_DEFAULT								GLS_DEPTHMASK_TRUE
#define GLS_ALPHA								(GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA)



#if (QSORT_SHADERNUM_SHIFT+SHADERNUM_BITS) > 32
	#error "Need to update sorting, too many bits."
#endif



using glIndex_t = unsigned int;
using color4ub_t = byte[4];



enum ssType_e {
	SURFSPRITE_NONE,
	SURFSPRITE_VERTICAL,
	SURFSPRITE_ORIENTED,
	SURFSPRITE_EFFECT,
	SURFSPRITE_WEATHERFX,
	SURFSPRITE_FLATTENED,
};

enum ssFacing_e {
	SURFSPRITE_FACING_NORMAL,
	SURFSPRITE_FACING_UP,
	SURFSPRITE_FACING_DOWN,
	SURFSPRITE_FACING_ANY,
};

enum cullStatus_e {
	CULL_IN, // completely unclipped
	CULL_CLIP, // clipped by one or more planes
	CULL_OUT, // completely outside the clipping planes
};

enum facing_e {
	SIDE_FRONT,
	SIDE_BACK,
	SIDE_ON,
};

enum eDLightTypes_e {
	DLIGHT_VERTICAL	= 0,
	DLIGHT_PROJECTED
};

enum shaderSort_e {
	SS_BAD,
	SS_PORTAL, // mirrors, portals, viewscreens
	SS_ENVIRONMENT, // sky box
	SS_OPAQUE, // opaque
	SS_DECAL, // scorch marks, etc.
	SS_SEE_THROUGH, // ladders, grates, grills that may have small blended edges in addition to alpha test
	SS_BANNER,
	SS_INSIDE, // inside body parts (i.e. heart)
	SS_MID_INSIDE,
	SS_MIDDLE,
	SS_MID_OUTSIDE,
	SS_OUTSIDE, // outside body parts (i.e. ribs)
	SS_FOG,
	SS_UNDERWATER, // for items that should be drawn in front of the water plane
	SS_BLEND0, // regular transparency and filters
	SS_BLEND1, // generally only used for additive type effects
	SS_BLEND2,
	SS_BLEND3,
	SS_BLEND6,
	SS_STENCIL_SHADOW,
	SS_ALMOST_NEAREST, // gun smoke puffs
	SS_NEAREST // blood blobs
};

enum genFunc_e {
	GF_NONE,
	GF_SIN,
	GF_SQUARE,
	GF_TRIANGLE,
	GF_SAWTOOTH,
	GF_INVERSE_SAWTOOTH,
	GF_NOISE,
	GF_RAND
};

enum deform_e {
	DEFORM_NONE,
	DEFORM_WAVE,
	DEFORM_NORMALS,
	DEFORM_BULGE,
	DEFORM_MOVE,
	DEFORM_PROJECTION_SHADOW,
	DEFORM_AUTOSPRITE,
	DEFORM_AUTOSPRITE2,
	DEFORM_TEXT0,
	DEFORM_TEXT1,
	DEFORM_TEXT2,
	DEFORM_TEXT3,
	DEFORM_TEXT4,
	DEFORM_TEXT5,
	DEFORM_TEXT6,
	DEFORM_TEXT7
};

enum alphaGen_e {
	AGEN_IDENTITY,
	AGEN_SKIP,
	AGEN_ENTITY,
	AGEN_ONE_MINUS_ENTITY,
	AGEN_VERTEX,
	AGEN_ONE_MINUS_VERTEX,
	AGEN_LIGHTING_SPECULAR,
	AGEN_WAVEFORM,
	AGEN_PORTAL,
	AGEN_BLEND,
	AGEN_CONST,
	AGEN_DOT,
	AGEN_ONE_MINUS_DOT
};

enum colorGen_e {
	CGEN_BAD,
	CGEN_IDENTITY_LIGHTING, // tr.identityLight
	CGEN_IDENTITY, // always (1,1,1,1)
	CGEN_ENTITY, // grabbed from entity's modulate field
	CGEN_ONE_MINUS_ENTITY, // grabbed from 1 - entity.modulate
	CGEN_EXACT_VERTEX, // tess.vertexColors
	CGEN_VERTEX, // tess.vertexColors * tr.identityLight
	CGEN_ONE_MINUS_VERTEX,
	CGEN_WAVEFORM, // programmatically generated
	CGEN_LIGHTING_DIFFUSE,
	CGEN_LIGHTING_DIFFUSE_ENTITY, //diffuse lighting * entity
	CGEN_FOG, // standard fog
	CGEN_CONST, // fixed color
	CGEN_LIGHTMAPSTYLE,
};

enum texCoordGen_e {
	TCGEN_BAD,
	TCGEN_IDENTITY, // clear to 0,0
	TCGEN_LIGHTMAP,
	TCGEN_LIGHTMAP1,
	TCGEN_LIGHTMAP2,
	TCGEN_LIGHTMAP3,
	TCGEN_TEXTURE,
	TCGEN_ENVIRONMENT_MAPPED,
	TCGEN_FOG,
	TCGEN_VECTOR // S and T from world coordinates
};

enum acff_e {
	ACFF_NONE,
	ACFF_MODULATE_RGB,
	ACFF_MODULATE_RGBA,
	ACFF_MODULATE_ALPHA
};

enum EGLFogOverride_e {
	GLFOGOVERRIDE_NONE = 0,
	GLFOGOVERRIDE_BLACK,
	GLFOGOVERRIDE_WHITE,
	GLFOGOVERRIDE_MAX
};

enum texMod_e {
	TMOD_NONE,
	TMOD_TRANSFORM,
	TMOD_TURBULENT,
	TMOD_SCROLL,
	TMOD_SCALE,
	TMOD_STRETCH,
	TMOD_ROTATE,
	TMOD_ENTITY_TRANSLATE
};

enum cullType_e {
	CT_FRONT_SIDED,
	CT_BACK_SIDED,
	CT_TWO_SIDED
};

enum fogPass_e {
	FP_NONE, // surface is translucent and will just be adjusted properly
	FP_EQUAL, // surface is opaque but possibly alpha tested
	FP_LE, // surface is trnaslucent, but still needs a fog pass (fog surface)
	FP_GLFOG
};

// any changes in surfaceType must be mirrored in rb_surfaceTable[]
enum surfaceType_e {
	SF_BAD,
	SF_SKIP,				// ignore
	SF_FACE,
	SF_GRID,
	SF_TRIANGLES,
	SF_POLY,
	SF_MD3,
	SF_MDX,
	SF_FLARE,
	SF_ENTITY,				// beams, rails, lightning, etc that can be determined by entity
	SF_DISPLAY_LIST,
	SF_NUM_SURFACE_TYPES,
	SF_MAX = 0xffffffff			// ensures that sizeof( surfaceType_e ) == sizeof( int )
};

enum renderCommand_e {
	RC_END_OF_LIST=0,
	RC_SET_COLOR,
	RC_STRETCH_PIC,
	RC_ROTATE_PIC,
	RC_ROTATE_PIC2,
	RC_DRAW_SURFS,
	RC_DRAW_BUFFER,
	RC_SWAP_BUFFERS,
	RC_WORLD_EFFECTS,
	RC_AUTO_MAP,
	RC_VIDEOFRAME
};



class CRenderableSurface {
public:
#ifdef _G2_GORE
	int				ident;
#else
	const int		ident;			// ident of this surface - required so the materials renderer knows what sort of surface this refers to
#endif
	CBoneCache 		*boneCache;
	mdxmSurface_t	*surfaceData;	// pointer to surface data loaded into file - only used by client renderer DO NOT USE IN GAME SIDE - if there is a vid restart this will be out of wack on the game
#ifdef _G2_GORE
	float			*alternateTex;		// alternate texture coordinates.
	void			*goreChain;

	float			scale;
	float			fade;
	float			impactTime; // this is a number between 0 and 1 that dictates the progression of the bullet impact
#endif

#ifdef _G2_GORE
	CRenderableSurface& operator= ( const CRenderableSurface& src )
	{
		ident	 = src.ident;
		boneCache = src.boneCache;
		surfaceData = src.surfaceData;
		alternateTex = src.alternateTex;
		goreChain = src.goreChain;

		return *this;
	}
#endif

CRenderableSurface():
	ident(SF_MDX),
	boneCache(0),
#ifdef _G2_GORE
	surfaceData(0),
	alternateTex(0),
	goreChain(0)
#else
	surfaceData(0)
#endif
	{}

#ifdef _G2_GORE
	void Init()
	{
		ident = SF_MDX;
		boneCache=0;
		surfaceData=0;
		alternateTex=0;
		goreChain=0;
	}
#endif
};



struct mapVert_t {
	vec3_t xyz;
	float  st[2];
	float  lightmap[MAXLIGHTMAPS][2];
	vec3_t normal;
	byte   color[MAXLIGHTMAPS][4];
};

struct md3Shader_t {
	char name[MAX_QPATH];
	int  shaderIndex;    // for in-game use
};

struct md3Triangle_t {
	int indexes[3];
};

struct md3St_t {
	float st[2];
};

struct md3XyzNormal_t {
	short xyz[3];
	short normal;
};

struct dfog_t {
	char shader[MAX_QPATH];
	int  brushNum;
	int  visibleSide;    // the brush side that ray tests need to clip against (-1 == none)
};

struct dlight_t {
	eDLightTypes_e	mType;
	vec3_t			origin;
	vec3_t			mProjOrigin; // projected light's origin
	vec3_t			color; // range from 0.0 to 1.0, should be color normalized
	float			radius;
	float			mProjRadius; // desired radius of light
	int				additive; // texture detail is lost tho when the lightmap is dark
	vec3_t			transformed; // origin in local coordinate system
	vec3_t			mProjTransformed; // projected light's origin in local coordinate system
	vec3_t			mDirection;
	vec3_t			mBasis2;
	vec3_t			mBasis3;
	vec3_t			mTransDirection;
	vec3_t			mTransBasis2;
	vec3_t			mTransBasis3;
};

// a trMiniRefEntity_t has all the information passed in by
// the client game, other info will come from it's parent main ref entity
struct trMiniRefEntity_t {
	miniRefEntity_t	e;
};

// a trRefEntity_t has all the information passed in by
// the client game, as well as some locally derived info
struct trRefEntity_t {
	refEntity_t	e;
	float		axisLength;		// compensate for non-normalized axis
	bool		needDlights;	// true for bmodels that touch a dlight
	bool		lightingCalculated;
	vec3_t		lightDir;		// normalized direction towards light
	vec3_t		ambientLight;	// color normalized to 0-255
	int			ambientLightInt;	// 32 bit rgba packed
	vec3_t		directedLight;
	int			dlightBits;
};

struct orientationr_t {
	vec3_t		origin;			// in world coordinates
	matrix3_t	axis;		// orientation in world
	vec3_t		viewOrigin;		// viewParms->or.origin in local coordinates
	float		modelMatrix[16];
};

struct image_t {
	char		imgName[MAX_QPATH];		// game path, including extension
	word		width, height;	// after power of two and picmip but not including clamp to MAX_TEXTURE_SIZE
	GLuint		texnum;					// gl texture binding
	int			frameUsed;			// for texture usage in frame statistics
	int			internalFormat;
	int			wrapClampMode;		// GL_CLAMP or GL_REPEAT
	bool		mipmap;
	bool		allowPicmip;
	short		iLastLevelUsedOn;

};

struct waveForm_t {
	genFunc_e	func;
	float		base;
	float		amplitude;
	float		phase;
	float		frequency;
};

struct deformStage_t {
	deform_e	deformation;			// vertex coordinate modification type
	vec3_t		moveVector;
	waveForm_t	deformationWave;
	float		deformationSpread;
	float		bulgeWidth;
	float		bulgeHeight;
	float		bulgeSpeed;
};

struct texModInfo_t {
	texMod_e		type;
	waveForm_t		wave;
	float			matrix[2][2];		// s' = s * m[0][0] + t * m[1][0] + trans[0]
	float			translate[2];		// t' = s * m[0][1] + t * m[0][1] + trans[1]
};

struct surfaceSprite_t {
	int			surfaceSpriteType;
	float		width, height, density, wind, windIdle, fadeDist, fadeMax, fadeScale;
	float		fxAlphaStart, fxAlphaEnd, fxDuration, vertSkew;
	vec2_t		variance, fxGrow;
	int			facing;		// Hangdown on vertical sprites, faceup on others.
};

struct textureBundle_t {
	image_t			*image;
	texCoordGen_e	tcGen;
	vec3_t			*tcGenVectors;
	texModInfo_t	*texMods;
	short			numTexMods;
	short			numImageAnimations;
	float			imageAnimationSpeed;
	bool			isLightmap;
	bool			oneShotAnimMap;
	bool			vertexLightmap;
	bool			isVideoMap;
	int				videoMapHandle;
};

struct shaderStage_t {
	bool			active;
	bool			isDetail;
	byte			index;						// index of stage
	byte			lightmapStyle;
	textureBundle_t	bundle[NUM_TEXTURE_BUNDLES];
	waveForm_t		rgbWave;
	colorGen_e		rgbGen;
	waveForm_t		alphaWave;
	alphaGen_e		alphaGen;
	byte			constantColor[4];			// for CGEN_CONST and AGEN_CONST
	uint32_t		stateBits;					// GLS_xxxx mask
	acff_e			adjustColorsForFog;
	EGLFogOverride_e	mGLFogColorOverride;
	surfaceSprite_t	*ss;
	bool			glow;
};

struct skyParms_t {
	float		cloudHeight;
	image_t		*outerbox[6];
};

struct fogParms_t {
	vec3_t	color;
	float	depthForOpaque;
};

struct shader_t {
	char			name[MAX_QPATH]; // game path, including extension
	int				lightmapIndex[MAXLIGHTMAPS]; // for a shader to match, both name and lightmapIndex must match
	byte			styles[MAXLIGHTMAPS];
	int				index; // this shader == tr.shaders[index]
	int				sortedIndex; // this shader == tr.sortedShaders[sortedIndex]
	float			sort; // lower numbered shaders draw before higher numbered
	int				surfaceFlags; // if explicitlyDefined, this will have SURF_* flags
	int				contentFlags;
	bool			defaultShader; // we want to return index 0 if the shader failed to load for some reason, but R_FindShader should still keep a name allocated for it, so if something calls RE_RegisterShader again with the same name, we don't try looking for it again found in a .shader file
	bool			explicitlyDefined; // found in a .shader file
	bool			entityMergable; // merge across entites optimizable (smoke, blood)
	skyParms_t		*sky;
	fogParms_t		*fogParms;
	float			portalRange; // distance to fog out at
	int				multitextureEnv; // 0, GL_MODULATE, GL_ADD (FIXME: put in stage)
	cullType_e		cullType; // CT_FRONT_SIDED, CT_BACK_SIDED, or CT_TWO_SIDED
	bool			polygonOffset; // set for decals and other items that must be offset
	bool			noMipMaps; // for console fonts, 2D elements, etc.
	bool			noPicMip; // for images that must always be full resolution
	bool			noTC; // for images that don't want to be texture compressed (eg skies)
	fogPass_e		fogPass; // draw a blended pass, possibly with depth test equals
	deformStage_t	*deforms[MAX_SHADER_DEFORMS];
	short			numDeforms;
	short			numUnfoggedPasses;
	shaderStage_t	*stages;
	float			clampTime; // time this shader is clamped to
	float			timeOffset; // current time offset for this shader
	bool			hasGlow;
	shader_t		*remappedShader; // current shader this one is remapped too
	shader_t		*next;
#ifdef _DEBUG
	int				numLoadedTimes;
#endif
};

// bogus little registration system for hit and location based damage files in hunk memory
struct hitMatReg_t {
	byte	*loc;
	int		width;
	int		height;
	char	name[MAX_QPATH];
};

// when cgame directly specifies a polygon, it becomes a srfPoly_t as soon as it is called
struct srfPoly_t {
	surfaceType_e	surfaceType;
	qhandle_t		hShader;
	int				fogIndex;
	int				numVerts;
	polyVert_t		*verts;
};

struct drawSurf_t {
	unsigned			sort;			// bit combination for fast compares
	surfaceType_e		*surface;		// any of surface*_t
};

// trRefdef_t holds everything that comes in refdef_t, as well as the locally generated scene information
struct trRefdef_t {
	int					x, y, width, height;
	float				fov_x, fov_y;
	vec3_t				vieworg;
	matrix3_t			viewaxis;		// transformation matrix
	int					time;				// time in milliseconds for shader effects and other time dependent rendering issues
	int					frametime;
	int					rdflags;			// RDF_NOWORLDMODEL, etc
	byte				areamask[MAX_MAP_AREA_BYTES]; // 1 bits will prevent the associated area from rendering at all
	bool				areamaskModified;	// true if areamask changed since last scene
	float				floatTime;			// tr.refdef.time / 1000.0
	char				text[MAX_RENDER_STRINGS][MAX_RENDER_STRING_LENGTH];
	int					num_entities;
	trRefEntity_t		*entities;
	trMiniRefEntity_t	*miniEntities;
	int					num_dlights;
	dlight_t			*dlights;
	int					numPolys;
	srfPoly_t			*polys;
	int					numDrawSurfs;
	drawSurf_t			*drawSurfs;
};

// skins allow models to be retextured without modifying the model file
struct skinSurface_t {
	char		name[MAX_QPATH];
	shader_t	*shader;
};

struct fog_t {
	int			originalBrushNumber;
	vec3_t		bounds[2];
	unsigned	colorInt; // in packed byte format
	float		tcScale; // texture coordinate vector scales
	fogParms_t	parms;
	bool		hasSurface;
	float		surface[4];
};

struct viewParms_t {
	orientationr_t	ori;				// Can't use "or" as it is a reserved word with gcc DREWS 2/2/2002
	orientationr_t	world;
	vec3_t			pvsOrigin;			// may be different than or.origin for portals
	bool			isPortal;			// true if this view is through a portal
	bool			isMirror;			// the portal is a mirror, invert the face culling
	int				frameSceneNum;		// copied from tr.frameSceneNum
	int				frameCount;			// copied from tr.frameCount
	cplane_t		portalPlane;		// clip anything behind this if mirroring
	int				viewportX, viewportY, viewportWidth, viewportHeight;
	float			fovX, fovY;
	float			projectionMatrix[16];
	cplane_t		frustum[4];
	vec3_t			visBounds[2];
	float			zFar;
};

struct srfDisplayList_t {
	surfaceType_e	surfaceType;
	int				listNum;
};

struct srfFlare_t {
	surfaceType_e	surfaceType;
	vec3_t			origin;
	vec3_t			normal;
	vec3_t			color;
};

struct srfGridMesh_t {
	surfaceType_e	surfaceType;
	int				dlightBits;
	vec3_t			meshBounds[2];
	vec3_t			localOrigin;
	float			meshRadius;
	vec3_t			lodOrigin;
	float			lodRadius;
	int				lodFixed;
	int				lodStitched;
	int				width, height;
	float			*widthLodError;
	float			*heightLodError;
	drawVert_t		verts[1];		// variable sized
};

struct srfSurfaceFace_t {
	surfaceType_e	surfaceType;
	cplane_t	plane;
	int			dlightBits;
	int			numPoints;
	int			numIndices;
	int			ofsIndices;
	float		points[1][VERTEXSIZE];	// variable sized. there is a variable length list of indices here also
};

// misc_models in maps are turned into direct geometry by q3map
struct srfTriangles_t {
	surfaceType_e	surfaceType;
	int				dlightBits;
	vec3_t			bounds[2];
	int				numIndexes;
	int				*indexes;
	int				numVerts;
	drawVert_t		*verts;
};

struct msurface_t {
	int				viewCount; // if == tr.viewCount, already added
	shader_t		*shader;
	int				fogIndex;
	surfaceType_e	*data; // any of srf*_t
};

struct mnode_t {
	// common with leaf and node
	int			contents;		// -1 for nodes, to differentiate from leafs
	int			visframe;		// node needs to be traversed if current
	vec3_t		mins, maxs;		// for bounding box culling
	mnode_t		*parent;
	cplane_t	*plane;
	mnode_t		*children[2];
	int			cluster;
	int			area;
	msurface_t	**firstmarksurface;
	int			nummarksurfaces;
};

struct bmodel_t {
	vec3_t		bounds[2]; // for culling
	msurface_t	*firstSurface;
	int			numSurfaces;
};

struct mgrid_t {
	byte		ambientLight[MAXLIGHTMAPS][3];
	byte		directLight[MAXLIGHTMAPS][3];
	byte		styles[MAXLIGHTMAPS];
	byte		latLong[2];
//	byte		pad[2]; // to align to a cache line
};

struct world_t {
	char		name[MAX_QPATH];		// ie: maps/tim_dm2.bsp
	char		baseName[MAX_QPATH];	// ie: tim_dm2
	int			dataSize;
	int			numShaders;
	dshader_t	*shaders;
	bmodel_t	*bmodels;
	int			numplanes;
	cplane_t	*planes;
	int			numnodes;		// includes leafs
	int			numDecisionNodes;
	mnode_t		*nodes;
	int			numsurfaces;
	msurface_t	*surfaces;
	int			nummarksurfaces;
	msurface_t	**marksurfaces;
	int			numfogs;
	fog_t		*fogs;
	int			globalFog;
	vec3_t		lightGridOrigin;
	vec3_t		lightGridSize;
	vec3_t		lightGridInverseSize;
	int			lightGridBounds[3];
	int			lightGridOffsets[8];
	vec3_t		lightGridStep;
	mgrid_t		*lightGridData;
	word		*lightGridArray;
	int			numGridArrayElements;
	int			numClusters;
	int			clusterBytes;
	const byte	*vis;			// may be passed in by CM_LoadMap to save space
	byte		*novis;			// clusterBytes of 0xff
	char		*entityString;
	char		*entityParsePoint;
};

struct frontEndCounters_t {
	int		c_sphere_cull_patch_in, c_sphere_cull_patch_clip, c_sphere_cull_patch_out;
	int		c_box_cull_patch_in, c_box_cull_patch_clip, c_box_cull_patch_out;
	int		c_sphere_cull_md3_in, c_sphere_cull_md3_clip, c_sphere_cull_md3_out;
	int		c_box_cull_md3_in, c_box_cull_md3_clip, c_box_cull_md3_out;
	int		c_leafs;
	int		c_dlightSurfaces;
	int		c_dlightSurfacesCulled;
};

// the renderer front end should never modify glstate_t
struct glstate_t {
	int			currenttextures[2];
	int			currenttmu;
	bool		finishCalled;
	int			texEnv[2];
	int			faceCulling;
	uint32_t	glStateBits;
};

struct backEndCounters_t {
	int		c_surfaces, c_shaders, c_vertexes, c_indexes, c_totalIndexes;
	float	c_overDraw;
	int		c_dlightVertexes;
	int		c_dlightIndexes;
	int		c_flareAdds;
	int		c_flareTests;
	int		c_flareRenders;
	int		msec; // total msec for backend run
};

// all state modified by the back end is seperated from the front end state
struct backEndState_t {
	trRefdef_t			refdef;
	viewParms_t			viewParms;
	orientationr_t		ori; // Can't use or as it is a c++ reserved word DREWS 2/2/2002
	backEndCounters_t	pc;
	bool				isHyperspace;
	trRefEntity_t		*currentEntity;
	bool				skyRenderedThisView; // flag for drawing sun
	bool				projection2D; // if true, drawstretchpic doesn't need to change modes
	byte				color2D[4];
	bool				vertexes2D; // shader needs to be finished
	trRefEntity_t		entity2D; // currentEntity will point at this when doing 2D rendering
};

// Most renderer globals are defined here.
// backend functions should never modify any of these fields, but may read fields that aren't dynamically modified by
//	the frontend.
struct trGlobals_t {
	bool					registered;		// cleared at shutdown, set at beginRegistration
	window_t				window;
	int						visCount;		// incremented every time a new vis cluster is entered
	int						frameCount;		// incremented every frame
	int						sceneCount;		// incremented every scene
	int						viewCount;		// incremented every view (twice a scene if portaled) and every R_MarkFragments call
	int						frameSceneNum;	// zeroed at RE_BeginFrame
	bool					worldMapLoaded;
	world_t					*world;
	char					worldDir[MAX_QPATH];		// ie: maps/tim_dm2 (copy of world_t::name sans extension but still includes the path)
	const byte				*externalVisData;	// from RE_SetWorldVisData, shared with CM_Load
	image_t					*defaultImage;
	image_t					*scratchImage[NUM_SCRATCH_IMAGES];
	image_t					*fogImage;
	image_t					*dlightImage;	// inverse-quare highlight for projective adding
	image_t					*flareImage;
	image_t					*whiteImage;			// full of 0xff
	image_t					*identityLightImage;	// full of tr.identityLightByte
	image_t					*screenImage; //reserve us a gl texnum to use with RF_DISTORTION
	GLuint					glowVShader;
	GLuint					glowPShader;
	GLuint					screenGlow;
	GLuint					sceneImage;
	GLuint					blurImage;
	GLuint					gammaCorrectLUTImage;
	GLuint					gammaCorrectVtxShader;
	GLuint					gammaCorrectPxShader;
	shader_t				*defaultShader;
	shader_t				*shadowShader;
	shader_t				*distortionShader;
	shader_t				*projectionShadowShader;
	shader_t				*sunShader;
	int						numLightmaps;
	image_t					*lightmaps[MAX_LIGHTMAPS];
	trRefEntity_t			*currentEntity;
	trRefEntity_t			worldEntity;		// point currentEntity at this when rendering world
	int						currentEntityNum;
	int						shiftedEntityNum;	// currentEntityNum << QSORT_REFENTITYNUM_SHIFT
	model_t					*currentModel;
	viewParms_t				viewParms;
	float					identityLight;		// 1.0 / ( 1 << overbrightBits )
	int						identityLightByte;	// identityLight * 255
	int						overbrightBits;		// r_overbrightBits->integer, but set to 0 if no hw gamma
	orientationr_t			ori;					// for current entity
	trRefdef_t				refdef;
	int						viewCluster;
	vec3_t					sunLight;			// from the sky shader for this level
	vec3_t					sunDirection;
	int						sunSurfaceLight;	// from the sky shader for this level
	vec3_t					sunAmbient;			// from the sky shader	(only used for John's terrain system)
	frontEndCounters_t		pc;
	int						frontEndMsec;		// not in pc due to clearing issue
	// put large tables at the end, so most elements will be within the +/32K indexed range on risc processors
	model_t					*models[MAX_MOD_KNOWN];
	int						numModels;
	world_t					bspModels[MAX_SUB_BSP];
	int						numBSPModels;
	// shader indexes from other modules will be looked up in tr.shaders[]
	// shader indexes from drawsurfs will be looked up in sortedShaders[]
	// lower indexed sortedShaders must be rendered first (opaque surfaces before translucent)
	int						numShaders;
	shader_t				*shaders[MAX_SHADERS];
	shader_t				*sortedShaders[MAX_SHADERS];
	int						numSkins;
	skin_t					*skins[MAX_SKINS];
	float					sinTable[FUNCTABLE_SIZE];
	float					squareTable[FUNCTABLE_SIZE];
	float					triangleTable[FUNCTABLE_SIZE];
	float					sawToothTable[FUNCTABLE_SIZE];
	float					inverseSawToothTable[FUNCTABLE_SIZE];
	float					fogTable[FOG_TABLE_SIZE];
	float					rangedFog;
	float					distanceCull;
};

struct glconfigExt_t {
	glconfig_t	*glConfig;
	bool		doGammaCorrectionWithShaders;
	const char	*originalExtensionString;
};


struct stageVars_t {
	color4ub_t	colors[SHADER_MAX_VERTEXES];
	vec2_t		texcoords[NUM_TEXTURE_BUNDLES][SHADER_MAX_VERTEXES];
};

struct shaderCommands_t {
	glIndex_t		indexes[SHADER_MAX_INDEXES] QALIGN(16);
	vec4_t			xyz[SHADER_MAX_VERTEXES] QALIGN(16);
	vec4_t			normal[SHADER_MAX_VERTEXES] QALIGN(16);
	vec2_t			texCoords[SHADER_MAX_VERTEXES][NUM_TEX_COORDS] QALIGN(16);
	color4ub_t		vertexColors[SHADER_MAX_VERTEXES] QALIGN(16);
	byte			vertexAlphas[SHADER_MAX_VERTEXES][4]; QALIGN(16) //rwwRMG - added support
	int				vertexDlightBits[SHADER_MAX_VERTEXES]; QALIGN(16)
	stageVars_t		svars QALIGN(16);
	shader_t		*shader;
	float			shaderTime;
	int				fogNum;
	int				dlightBits;	// or together of all vertexDlightBits
	int				numIndexes;
	int				numVertexes;
	int				numPasses;
	void			(*currentStageIteratorFunc)( void );
	shaderStage_t	*xstages;
	int				registration;
	bool			SSInitializedWind;
	bool			fading; //rww - doing a fade, don't compute shader color/alpha overrides
};

struct renderCommandList_t {
	byte	cmds[MAX_RENDER_COMMANDS];
	int		used;
};

struct setColorCommand_t {
	int		commandId;
	float	color[4];
};

struct drawBufferCommand_t {
	int		commandId;
	int		buffer;
};

struct subImageCommand_t {
	int		commandId;
	image_t	*image;
	int		width;
	int		height;
	void	*data;
};

struct swapBuffersCommand_t {
	int		commandId;
};

struct endFrameCommand_t {
	int		commandId;
	int		buffer;
};

struct stretchPicCommand_t {
	int			commandId;
	shader_t	*shader;
	float		x, y;
	float		w, h;
	float		s1, t1;
	float		s2, t2;
};

struct rotatePicCommand_t {
	int			commandId;
	shader_t	*shader;
	float		x, y;
	float		w, h;
	float		s1, t1;
	float		s2, t2;
	float		a;
};

struct drawSurfsCommand_t {
	int			commandId;
	trRefdef_t	refdef;
	viewParms_t	viewParms;
	drawSurf_t	*drawSurfs;
	int			numDrawSurfs;
};

struct videoFrameCommand_t {
	int		commandId;
	int		width;
	int		height;
	byte	*captureBuffer;
	byte	*encodeBuffer;
	bool	motionJpeg;
};

// all of the information needed by the back end must be contained in a backEndData_t.
struct backEndData_t {
	drawSurf_t			drawSurfs[MAX_DRAWSURFS];
	dlight_t			dlights[MAX_DLIGHTS];
	trRefEntity_t		entities[MAX_REFENTITIES];
	trMiniRefEntity_t	miniEntities[MAX_MINI_ENTITIES];
	srfPoly_t			*polys;//[MAX_POLYS];
	polyVert_t			*polyVerts;//[MAX_POLYVERTS];
	renderCommandList_t	commands;
};



extern backEndState_t    backEnd;
extern backEndData_t    *backEndData;
extern int               gl_filter_min, gl_filter_max;
extern glconfig_t        glConfig;		// outside of TR since it shouldn't be cleared during ref re-init
extern glconfigExt_t     glConfigExt;
extern glstate_t         glState;		// outside of TR since it shouldn't be cleared during ref re-init
extern hitMatReg_t       hitMatReg[MAX_HITMAT_ENTRIES];
extern const int         lightmaps2d[MAXLIGHTMAPS];
extern const int         lightmapsFullBright[MAXLIGHTMAPS];
extern const int         lightmapsNone[MAXLIGHTMAPS];
extern const int         lightmapsVertex[MAXLIGHTMAPS];
extern int               max_polys;
extern int               max_polyverts;
extern color4ub_t        styleColors[MAX_LIGHT_STYLES];
extern const byte        stylesDefault[MAXLIGHTMAPS];
extern shaderCommands_t  tess;
extern trGlobals_t       tr;
extern window_t          window;

extern void              (*rb_surfaceTable[SF_NUM_SURFACE_TYPES])( void * );



static QINLINE void	GLimp_LogComment( char *comment ) {}



void           ARB_InitGPUShaders                  ( void );
void           G2_TransformBone                    ( int child,CBoneCache &BC );
void           GL_Bind                             ( image_t *image );
void           GL_CheckErrors                      ( void );
void           GL_Cull                             ( int cullType );
void           GL_SelectTexture                    ( int unit );
void           GL_SetDefaultState                  ( void );
void           GL_State                            ( uint32_t stateVector );
void           GL_TexEnv                           ( int env );
void           GL_TextureMode                      ( const char *string );
void           KillTheShaderHashTable              ( void );
void           Multiply_3x4Matrix                  ( mdxaBone_t *out, mdxaBone_t *in2, mdxaBone_t *in );
float          ProjectRadius                       ( float r, vec3_t location );
bool           ShaderHashTableExists               ( void );
void           R_AddBeamSurfaces                   ( trRefEntity_t *e );
void           R_AddBrushModelSurfaces             ( trRefEntity_t *e );
void           R_AddDecals                         ( void );
void           R_AddDrawSurf                       ( surfaceType_e *surface, shader_t *shader, int fogIndex, int dlightMap );
void           R_AddDrawSurfCmd                    ( drawSurf_t *drawSurfs, int numDrawSurfs );
void           R_AddGhoulSurfaces                  ( trRefEntity_t *ent );
void           R_AddLightningBoltSurfaces          ( trRefEntity_t *e );
void           R_AddMD3Surfaces                    ( trRefEntity_t *e );
void           R_AddNullModelSurfaces              ( trRefEntity_t *e );
void           R_AddPolygonSurfaces                ( void );
void           R_AddRailSurfaces                   ( trRefEntity_t *e, bool isUnderwater );
void           R_AddWorldSurfaces                  ( void );
model_t       *R_AllocModel                        ( void );
void           R_AutomapElevationAdjustment        ( float newHeight );
void           R_BindAnimatedImage                 ( textureBundle_t *bundle );
void           R_BuildCloudData                    ( shaderCommands_t *shader );
void           R_ClearFlares                       ( void );
void           R_ColorShiftLightingBytes           ( byte in[4], byte out[4] ); //rwwRMG - added
image_t       *R_CreateImage                       ( const char *name, const byte *pic, int width, int height, GLenum format, bool mipmap, bool allowPicmip, bool allowTC, int wrapClampMode, bool bRectangle = false );
int            R_CullLocalBox                      ( const vec3_t bounds[2]);
int            R_CullLocalPointAndRadius           ( const vec3_t origin, float radius );
int            R_CullPointAndRadius                ( const vec3_t origin, float radius );
void           R_DecomposeSort                     ( unsigned sort, int *entityNum, shader_t **shader, int *fogNum, int *dlightMap );
void           R_DeleteTextures                    ( void );
void           R_DlightBmodel                      ( bmodel_t *bmodel, bool NoLight );
void           R_DrawSkyBox                        ( shaderCommands_t *shader );
const void    *R_DrawWireframeAutomap              ( const void *data );
image_t       *R_FindImageFile                     ( const char *name, bool mipmap, bool allowPicmip, bool allowTC, int glWrapClampMode );
shader_t      *R_FindServerShader                  ( const char *name, const int *lightmapIndex, const byte *styles, bool mipRawImage );
shader_t      *R_FindShader                        ( const char *name, const int *lightmapIndex, const byte *styles, bool mipRawImage );
shader_t      *R_FindShaderByName                  ( const char *name );
float          R_FogFactor                         ( float s, float t );
void           R_FontList_f                        ( void );
void           R_FreeSurfaceGridMesh               ( srfGridMesh_t *grid );
void           R_GammaCorrect                      ( byte *buffer, int bufSize );
void          *R_GetCommandBuffer                  ( int bytes );
bool           R_GetEntityToken                    ( char *buffer, int size );
model_t       *R_GetModelByHandle                  ( qhandle_t hModel );
bool           R_GetModeInfo                       ( int *width, int *height, int mode );
shader_t      *R_GetShaderByHandle                 ( qhandle_t hShader );
shader_t      *R_GetShaderByState                  ( int index, long *cycleTime );
skin_t        *R_GetSkinByHandle                   ( qhandle_t hSkin );
srfGridMesh_t *R_GridInsertColumn                  ( srfGridMesh_t *grid, int column, int row, vec3_t point, float loderror );
srfGridMesh_t *R_GridInsertRow                     ( srfGridMesh_t *grid, int row, int column, vec3_t point, float loderror );
void           R_ImageList_f                       ( void );
void           R_Images_Clear                      ( void );
void           R_Images_DeleteImage                ( image_t *pImage );
void           R_Images_DeleteLightMaps            ( void );
image_t       *R_Images_GetNextIteration           ( void );
int            R_Images_StartIteration             ( void );
void           R_Init                              ( void );
void           R_InitDecals                        ( void );
void           R_InitFogTable                      ( void );
bool           R_InitializeWireframeAutomap        ( void );
void           R_InitImages                        ( void );
void           R_InitNextFrame                     ( void );
void           R_InitShaders                       ( bool server );
void           R_InitSkins                         ( void );
void           R_InitSkyTexCoords                  ( float cloudLayerHeight );
bool           R_inPVS                             ( const vec3_t p1, const vec3_t p2, byte *mask );
void           R_IssuePendingRenderCommands        ( void );
int            R_LerpTag                           ( orientation_t *tag, qhandle_t handle, int startFrame, int endFrame, float frac, const char *tagName );
int            R_LightForPoint                     ( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir );
bool           R_LoadMDXA                          ( model_t *mod, void *buffer, const char *name, bool &bAlreadyCached  );
bool           R_LoadMDXM                          ( model_t *mod, void *buffer, const char *name, bool &bAlreadyCached  );
void           R_LocalNormalToWorld                ( const vec3_t local, vec3_t world );
void           R_LocalPointToWorld                 ( const vec3_t local, vec3_t world );
int            R_MarkFragments                     ( int numPoints, const vec3_t *points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer );
void           R_ModelBounds                       ( qhandle_t handle, vec3_t mins, vec3_t maxs );
void           R_ModelInit                         ( void );
void           R_Modellist_f                       ( void );
void           R_RemapShader                       ( const char *oldShader, const char *newShader, const char *timeOffset );
void           R_RenderView                        ( viewParms_t *parms );
void           R_RotateForEntity                   ( const trRefEntity_t *ent, const viewParms_t *viewParms, orientationr_t *ori );
void           R_SetColorMappings                  ( void );
void           R_SetGammaCorrectionLUT             ( void );
void           R_SetupEntityLighting               ( const trRefdef_t *refdef, trRefEntity_t *ent );
void           R_ShaderList_f                      ( void );
void           R_SkinList_f                        ( void );
srfGridMesh_t *R_SubdividePatchToGrid              ( int width, int height, drawVert_t points[MAX_PATCH_SIZE*MAX_PATCH_SIZE] );
float          R_SumOfUsedImages                   ( bool bUseFormat );
void           R_SVModelInit                       ( void );
void           R_SwapBuffers                       ( int );
void           R_TransformClipToWindow             ( const vec4_t clip, const viewParms_t *view, vec4_t normalized, vec4_t window );
void           R_TransformDlights                  ( int count, dlight_t *dl, orientationr_t *ori );
void           R_TransformModelToClip              ( const vec3_t src, const float *modelMatrix, const float *projectionMatrix, vec4_t eye, vec4_t dst );
void           R_WorldNormalToEntity               ( const vec3_t localVec, vec3_t world );
void           RB_AddDlightFlares                  ( void );
void           RB_AddFlare                         ( void *surface, int fogNum, vec3_t point, vec3_t color, vec3_t normal );
void           RB_AddQuadStamp                     ( vec3_t origin, vec3_t left, vec3_t up, byte *color );
void           RB_AddQuadStampExt                  ( vec3_t origin, vec3_t left, vec3_t up, byte *color, float s1, float t1, float s2, float t2 );
void           RB_BeginSurface                     ( shader_t *shader, int fogNum );
void           RB_CalcAlphaFromEntity              ( unsigned char *dstColors );
void           RB_CalcAlphaFromOneMinusEntity      ( unsigned char *dstColors );
void           RB_CalcColorFromEntity              ( unsigned char *dstColors );
void           RB_CalcColorFromOneMinusEntity      ( unsigned char *dstColors );
void           RB_CalcDiffuseColor                 ( unsigned char *colors );
void           RB_CalcDiffuseEntityColor           ( unsigned char *colors );
void           RB_CalcDisintegrateColors           ( unsigned char *colors );
void           RB_CalcDisintegrateVertDeform       ( void );
void           RB_CalcEnvironmentTexCoords         ( float *dstTexCoords );
void           RB_CalcFogTexCoords                 ( float *dstTexCoords );
void           RB_CalcModulateAlphasByFog          ( unsigned char *dstColors );
void           RB_CalcModulateColorsByFog          ( unsigned char *dstColors );
void           RB_CalcModulateRGBAsByFog           ( unsigned char *dstColors );
void           RB_CalcRotateTexCoords              ( float rotSpeed, float *dstTexCoords );
void           RB_CalcScaleTexCoords               ( const float scale[2], float *dstTexCoords );
void           RB_CalcScrollTexCoords              ( const float scroll[2], float *dstTexCoords );
void           RB_CalcSpecularAlpha                ( unsigned char *alphas );
void           RB_CalcStretchTexCoords             ( const waveForm_t *wf, float *texCoords );
void           RB_CalcTransformTexCoords           ( const texModInfo_t *tmi, float *dstTexCoords );
void           RB_CalcTurbulentTexCoords           ( const waveForm_t *wf, float *dstTexCoords );
void           RB_CalcWaveAlpha                    ( const waveForm_t *wf, unsigned char *dstColors );
void           RB_CalcWaveColor                    ( const waveForm_t *wf, unsigned char *dstColors );
void           RB_CaptureScreenImage               ( void );
void           RB_CheckOverflow                    ( int verts, int indexes );
void           RB_ClipSkyPolygons                  ( shaderCommands_t *shader );
void           RB_DeformTessGeometry               ( void );
void           RB_DistortionFill                   ( void );
void           RB_DrawSun                          ( void );
void           RB_DrawSurfaceSprites               ( shaderStage_t *stage, shaderCommands_t *input );
void           RB_EndSurface                       ( void );
void           RB_ExecuteRenderCommands            ( const void *data );
void           RB_ExecuteRenderCommands            ( const void *data );
void           RB_ProjectionShadowDeform           ( void );
void           RB_RenderFlares                     ( void );
void           RB_SetGL2D                          ( void );
void           RB_ShadowFinish                     ( void );
void           RB_ShadowTessEnd                    ( void );
void           RB_ShowImages                       ( void );
void           RB_StageIteratorGeneric             ( void );
void           RB_StageIteratorSky                 ( void );
void           RB_SurfaceGhoul                     ( CRenderableSurface *surface );
const void    *RB_TakeVideoFrameCmd                ( const void *data );
void           RE_AddAdditiveLightToScene          ( const vec3_t org, float intensity, float r, float g, float b );
void           RE_AddDecalToScene                  ( qhandle_t shader, const vec3_t origin, const vec3_t dir, float orientation, float r, float g, float b, float a, bool alphaFade, float radius, bool temporary );
void           RE_AddLightToScene                  ( const vec3_t org, float intensity, float r, float g, float b );
void           RE_AddMiniRefEntityToScene          ( const miniRefEntity_t *ent );
void           RE_AddPolyToScene                   ( qhandle_t hShader , int numVerts, const polyVert_t *verts, int num );
void           RE_AddRefEntityToScene              ( const refEntity_t *ent );
void           RE_BeginFrame                       ( stereoFrame_e stereoFrame );
void           RE_BeginFrame                       ( stereoFrame_e stereoFrame );
void           RE_BeginRegistration                ( glconfig_t *glconfig );
void           RE_ClearDecals                      ( void );
void           RE_ClearScene                       ( void );
void           RE_EndFrame                         ( int *frontEndMsec, int *backEndMsec );
void           RE_GetBModelVerts                   ( int bmodelIndex, vec3_t *verts, vec3_t normal );
void           RE_HunkClearCrap                    ( void );
void           RE_InsertModelIntoHash              ( const char *name, model_t *mod );
void           RE_LoadWorldMap                     ( const char *mapname );
void           RE_LoadWorldMap_Actual              ( const char *name, world_t &worldData, int index );
void           RE_RegisterImages_Info_f            ( void );
bool           RE_RegisterImages_LevelLoadEnd      ( void );
int            RE_RegisterMedia_GetLevel           ( void );
void           RE_RegisterMedia_LevelLoadBegin     ( const char *psMapName, ForceReload_e eForceReload );
void           RE_RegisterMedia_LevelLoadEnd       ( void );
qhandle_t      RE_RegisterModel                    ( const char *name );
void           RE_RegisterModels_Info_f            ( void );
bool           RE_RegisterModels_LevelLoadEnd      ( bool bDeleteEverythingNotUsedThisLevel = false );
void          *RE_RegisterModels_Malloc            ( int iSize, void *pvDiskBufferIfJustLoaded, const char *psModelFileName, bool *pqbAlreadyFound, memtag_t eTag );
void           RE_RegisterModels_StoreShaderRequest( const char *psModelFileName, const char *psShaderName, int *piShaderIndexPoke );
qhandle_t      RE_RegisterServerModel              ( const char *name );
qhandle_t      RE_RegisterServerSkin               ( const char *name );
qhandle_t      RE_RegisterShader                   ( const char *name );
qhandle_t      RE_RegisterShaderFromImage          ( const char *name, int *lightmapIndex, byte *styles, image_t *image, bool mipRawImage );
qhandle_t      RE_RegisterShaderLightMap           ( const char *name, const int *lightmapIndex, const byte *styles ) ;
qhandle_t      RE_RegisterShaderNoMip              ( const char *name );
qhandle_t      RE_RegisterSkin                     ( const char *name );
void           RE_RenderAutoMap                    ( void );
void           RE_RenderScene                      ( const refdef_t *fd );
void           RE_RenderWorldEffects               ( void );
void           RE_RotatePic                        ( float x, float y, float w, float h, float s1, float t1, float s2, float t2,float a, qhandle_t hShader );
void           RE_RotatePic2                       ( float x, float y, float w, float h, float s1, float t1, float s2, float t2,float a, qhandle_t hShader );
void           RE_SetColor                         ( const float *rgba );
void           RE_SetWorldVisData                  ( const byte *vis );
const char    *RE_ShaderNameFromIndex              ( int index );
void           RE_Shutdown                         ( bool destroyWindow );
void           RE_StretchPic                       ( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader );
void           RE_StretchRaw                       ( int x, int y, int w, int h, int cols, int rows, const byte *data, int client, bool dirty );
void           RE_TakeVideoFrame                   ( int wfidth, int height, byte *captureBuffer, byte *encodeBuffer, bool motionJpeg );
void           RE_UploadCinematic                  ( int cols, int rows, const byte *data, int client, bool dirty );
void           SetViewportAndScissor               ( void );

