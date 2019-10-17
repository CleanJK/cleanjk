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

// tr_init.c -- functions that are not called every frame

#include "tr_local.h"
#include "rd-common/tr_common.h"
#include "qcommon/MiniHeap.h"
#include "ghoul2/g2_local.h"
#include <algorithm>
#include "rd-dedicated/tr_cvars.h"

// the limits apply to the sum of all scenes in a frame - the main view, all the 3D icons, etc
#define	DEFAULT_MAX_POLYS		600
#define	DEFAULT_MAX_POLYVERTS	3000
int		max_polys;
int		max_polyverts;

typedef struct vidmode_s
{
    const char *description;
    int         width, height;
} vidmode_t;

const vidmode_t r_vidModes[] = {
    { "Mode  0: 320x240",		320,	240 },
    { "Mode  1: 400x300",		400,	300 },
    { "Mode  2: 512x384",		512,	384 },
    { "Mode  3: 640x480",		640,	480 },
    { "Mode  4: 800x600",		800,	600 },
    { "Mode  5: 960x720",		960,	720 },
    { "Mode  6: 1024x768",		1024,	768 },
    { "Mode  7: 1152x864",		1152,	864 },
    { "Mode  8: 1280x1024",		1280,	1024 },
    { "Mode  9: 1600x1200",		1600,	1200 },
    { "Mode 10: 2048x1536",		2048,	1536 },
    { "Mode 11: 856x480 (wide)", 856,	 480 },
    { "Mode 12: 2400x600(surround)",2400,600 }
};
static const int	s_numVidModes = ( sizeof( r_vidModes ) / sizeof( r_vidModes[0] ) );

qboolean R_GetModeInfo( int *width, int *height, int mode ) {
	const vidmode_t	*vm;

    if ( mode < -1 ) {
        return qfalse;
	}
	if ( mode >= s_numVidModes ) {
		return qfalse;
	}

	if ( mode == -1 ) {
		*width = r_customwidth->integer;
		*height = r_customheight->integer;
		return qtrue;
	}

	vm = &r_vidModes[mode];

    *width  = vm->width;
    *height = vm->height;

    return qtrue;
}

static void R_ModeList_f( void )
{
	int i;

	Com_Printf ("\n" );
	for ( i = 0; i < s_numVidModes; i++ )
	{
		Com_Printf ("%s\n", r_vidModes[i].description );
	}
	Com_Printf ("\n" );
}

typedef struct consoleCommand_s {
	const char	*cmd;
	xcommand_t	func;
} consoleCommand_t;

static consoleCommand_t	commands[] = {
	{ "modellist",			R_Modellist_f },
	{ "modelist",			R_ModeList_f },
	{ "modelcacheinfo",		RE_RegisterModels_Info_f },
};

static const size_t numCommands = ARRAY_LEN( commands );

void R_Register( void )
{
	R_InitCvars();

	if (ri.Sys_LowPhysicalMemory() )
		ri.Cvar_Set("r_modelpoolmegs", "0");

	for ( size_t i = 0; i < numCommands; i++ )
		ri.Cmd_AddCommand( commands[i].cmd, commands[i].func, "" );
}

extern void R_InitWorldEffects(void); //tr_WorldEffects.cpp
void R_Init( void ) {
	int i;
	byte *ptr;

//	Com_Printf ("----- R_Init -----\n" );
	// clear all our internal state
	memset( &tr, 0, sizeof( tr ) );
	memset( &backEnd, 0, sizeof( backEnd ) );

//	Swap_Init();

	// init function tables

	for ( i = 0; i < FUNCTABLE_SIZE; i++ )
	{
		tr.sinTable[i]		= sin( DEG2RAD( i * 360.0f / ( ( float ) ( FUNCTABLE_SIZE - 1 ) ) ) );
		tr.squareTable[i]	= ( i < FUNCTABLE_SIZE/2 ) ? 1.0f : -1.0f;
		tr.sawToothTable[i] = (float)i / FUNCTABLE_SIZE;
		tr.inverseSawToothTable[i] = 1.0f - tr.sawToothTable[i];

		if ( i < FUNCTABLE_SIZE / 2 )
		{
			if ( i < FUNCTABLE_SIZE / 4 )
			{
				tr.triangleTable[i] = ( float ) i / ( FUNCTABLE_SIZE / 4 );
			}
			else
			{
				tr.triangleTable[i] = 1.0f - tr.triangleTable[i-FUNCTABLE_SIZE / 4];
			}
		}
		else
		{
			tr.triangleTable[i] = -tr.triangleTable[i-FUNCTABLE_SIZE/2];
		}
	}
	R_Register();

	max_polys = Q_min( r_maxpolys->integer, DEFAULT_MAX_POLYS );
	max_polyverts = Q_min( r_maxpolyverts->integer, DEFAULT_MAX_POLYVERTS );

	ptr = (byte *)Hunk_Alloc( sizeof( *backEndData ) + sizeof(srfPoly_t) * max_polys + sizeof(polyVert_t) * max_polyverts, h_low);
	backEndData = (backEndData_t *) ptr;
	backEndData->polys = (srfPoly_t *) ((char *) ptr + sizeof( *backEndData ));
	backEndData->polyVerts = (polyVert_t *) ((char *) ptr + sizeof( *backEndData ) + sizeof(srfPoly_t) * max_polys);

	R_ModelInit();

//	Com_Printf ("----- finished R_Init -----\n" );
}

void RE_Shutdown( qboolean destroyWindow, qboolean restarting ) {

//	Com_Printf ("RE_Shutdown( %i )\n", destroyWindow );

	for ( size_t i = 0; i < numCommands; i++ )
		ri.Cmd_RemoveCommand( commands[i].cmd );

	tr.registered = qfalse;
}

static void G2API_BoltMatrixReconstruction( qboolean reconstruct ) { gG2_GBMNoReconstruct = (qboolean)!reconstruct; }
static void G2API_BoltMatrixSPMethod( qboolean spMethod ) { gG2_GBMUseSPMethod = spMethod; }

extern void R_SVModelInit( void ); //tr_model.cpp
extern qboolean gG2_GBMNoReconstruct;
extern qboolean gG2_GBMUseSPMethod;
extern qhandle_t RE_RegisterServerSkin( const char *name );

refexport_t *GetRefAPI ( int apiVersion, refimport_t *rimp ) {
	static refexport_t re;

	assert( rimp );
	ri = *rimp;

	memset( &re, 0, sizeof( re ) );

	if ( apiVersion != REF_API_VERSION ) {
		Com_Printf ( "Mismatched REF_API_VERSION: expected %i, got %i\n", REF_API_VERSION, apiVersion );
		return NULL;
	}

	// the RE_ functions are Renderer Entry points

	re.Shutdown = RE_Shutdown;

	re.RegisterMedia_LevelLoadBegin			= RE_RegisterMedia_LevelLoadBegin;
	re.RegisterMedia_LevelLoadEnd			= RE_RegisterMedia_LevelLoadEnd;
	re.RegisterMedia_GetLevel				= RE_RegisterMedia_GetLevel;
	re.RegisterModels_LevelLoadEnd			= RE_RegisterModels_LevelLoadEnd;

	re.RegisterServerSkin					= RE_RegisterServerSkin;

	// G2 stuff
	re.InitSkins							= R_InitSkins;
	re.InitShaders							= R_InitShaders;
	re.SVModelInit							= R_SVModelInit;
	re.HunkClearCrap						= RE_HunkClearCrap;

	// G2API
	re.G2API_AddBolt						= G2API_AddBolt;
	re.G2API_AddBoltSurfNum					= G2API_AddBoltSurfNum;
	re.G2API_AddSurface						= G2API_AddSurface;
	re.G2API_AnimateG2ModelsRag				= G2API_AnimateG2ModelsRag;
	re.G2API_AttachEnt						= G2API_AttachEnt;
	re.G2API_AttachG2Model					= G2API_AttachG2Model;
	re.G2API_AttachInstanceToEntNum			= G2API_AttachInstanceToEntNum;
	re.G2API_AbsurdSmoothing				= G2API_AbsurdSmoothing;
	re.G2API_BoltMatrixReconstruction		= G2API_BoltMatrixReconstruction;
	re.G2API_BoltMatrixSPMethod				= G2API_BoltMatrixSPMethod;
	re.G2API_CleanEntAttachments			= G2API_CleanEntAttachments;
	re.G2API_CleanGhoul2Models				= G2API_CleanGhoul2Models;
	re.G2API_ClearAttachedInstance			= G2API_ClearAttachedInstance;
	re.G2API_CollisionDetect				= G2API_CollisionDetect;
	re.G2API_CollisionDetectCache			= G2API_CollisionDetectCache;
	re.G2API_CopyGhoul2Instance				= G2API_CopyGhoul2Instance;
	re.G2API_CopySpecificG2Model			= G2API_CopySpecificG2Model;
	re.G2API_DetachG2Model					= G2API_DetachG2Model;
	re.G2API_DoesBoneExist					= G2API_DoesBoneExist;
	re.G2API_DuplicateGhoul2Instance		= G2API_DuplicateGhoul2Instance;
	re.G2API_FreeSaveBuffer					= G2API_FreeSaveBuffer;
	re.G2API_GetAnimFileName				= G2API_GetAnimFileName;
	re.G2API_GetAnimFileNameIndex			= G2API_GetAnimFileNameIndex;
	re.G2API_GetAnimRange					= G2API_GetAnimRange;
	re.G2API_GetBoltMatrix					= G2API_GetBoltMatrix;
	re.G2API_GetBoneAnim					= G2API_GetBoneAnim;
	re.G2API_GetBoneIndex					= G2API_GetBoneIndex;
	re.G2API_GetGhoul2ModelFlags			= G2API_GetGhoul2ModelFlags;
	re.G2API_GetGLAName						= G2API_GetGLAName;
	re.G2API_GetModelName					= G2API_GetModelName;
	re.G2API_GetParentSurface				= G2API_GetParentSurface;
	re.G2API_GetRagBonePos					= G2API_GetRagBonePos;
	re.G2API_GetSurfaceIndex				= G2API_GetSurfaceIndex;
	re.G2API_GetSurfaceName					= G2API_GetSurfaceName;
	re.G2API_GetSurfaceOnOff				= G2API_GetSurfaceOnOff;
	re.G2API_GetSurfaceRenderStatus			= G2API_GetSurfaceRenderStatus;
	re.G2API_GetTime						= G2API_GetTime;
	re.G2API_Ghoul2Size						= G2API_Ghoul2Size;
	re.G2API_GiveMeVectorFromMatrix			= G2API_GiveMeVectorFromMatrix;
	re.G2API_HasGhoul2ModelOnIndex			= G2API_HasGhoul2ModelOnIndex;
	re.G2API_HaveWeGhoul2Models				= G2API_HaveWeGhoul2Models;
	re.G2API_IKMove							= G2API_IKMove;
	re.G2API_InitGhoul2Model				= G2API_InitGhoul2Model;
	re.G2API_IsGhoul2InfovValid				= G2API_IsGhoul2InfovValid;
	re.G2API_IsPaused						= G2API_IsPaused;
	re.G2API_ListBones						= G2API_ListBones;
	re.G2API_ListSurfaces					= G2API_ListSurfaces;
	re.G2API_LoadGhoul2Models				= G2API_LoadGhoul2Models;
	re.G2API_LoadSaveCodeDestructGhoul2Info	= G2API_LoadSaveCodeDestructGhoul2Info;
	re.G2API_OverrideServerWithClientData	= G2API_OverrideServerWithClientData;
	re.G2API_PauseBoneAnim					= G2API_PauseBoneAnim;
	re.G2API_PrecacheGhoul2Model			= G2API_PrecacheGhoul2Model;
	re.G2API_RagEffectorGoal				= G2API_RagEffectorGoal;
	re.G2API_RagEffectorKick				= G2API_RagEffectorKick;
	re.G2API_RagForceSolve					= G2API_RagForceSolve;
	re.G2API_RagPCJConstraint				= G2API_RagPCJConstraint;
	re.G2API_RagPCJGradientSpeed			= G2API_RagPCJGradientSpeed;
	re.G2API_RemoveBolt						= G2API_RemoveBolt;
	re.G2API_RemoveBone						= G2API_RemoveBone;
	re.G2API_RemoveGhoul2Model				= G2API_RemoveGhoul2Model;
	re.G2API_RemoveGhoul2Models				= G2API_RemoveGhoul2Models;
	re.G2API_RemoveSurface					= G2API_RemoveSurface;
	re.G2API_ResetRagDoll					= G2API_ResetRagDoll;
	re.G2API_SaveGhoul2Models				= G2API_SaveGhoul2Models;
	re.G2API_SetBoltInfo					= G2API_SetBoltInfo;
	re.G2API_SetBoneAngles					= G2API_SetBoneAngles;
	re.G2API_SetBoneAnglesIndex				= G2API_SetBoneAnglesIndex;
	re.G2API_SetBoneAnglesMatrix			= G2API_SetBoneAnglesMatrix;
	re.G2API_SetBoneAnglesMatrixIndex		= G2API_SetBoneAnglesMatrixIndex;
	re.G2API_SetBoneAnim					= G2API_SetBoneAnim;
	re.G2API_SetBoneAnimIndex				= G2API_SetBoneAnimIndex;
	re.G2API_SetBoneIKState					= G2API_SetBoneIKState;
	re.G2API_SetGhoul2ModelIndexes			= G2API_SetGhoul2ModelIndexes;
	re.G2API_SetGhoul2ModelFlags			= G2API_SetGhoul2ModelFlags;
	re.G2API_SetLodBias						= G2API_SetLodBias;
	re.G2API_SetNewOrigin					= G2API_SetNewOrigin;
	re.G2API_SetRagDoll						= G2API_SetRagDoll;
	re.G2API_SetRootSurface					= G2API_SetRootSurface;
	re.G2API_SetShader						= G2API_SetShader;
	re.G2API_SetSkin						= G2API_SetSkin;
	re.G2API_SetSurfaceOnOff				= G2API_SetSurfaceOnOff;
	re.G2API_SetTime						= G2API_SetTime;
	re.G2API_SkinlessModel					= G2API_SkinlessModel;
	re.G2API_StopBoneAngles					= G2API_StopBoneAngles;
	re.G2API_StopBoneAnglesIndex			= G2API_StopBoneAnglesIndex;
	re.G2API_StopBoneAnim					= G2API_StopBoneAnim;
	re.G2API_StopBoneAnimIndex				= G2API_StopBoneAnimIndex;

	#ifdef _G2_GORE
	re.G2API_GetNumGoreMarks				= G2API_GetNumGoreMarks;
	re.G2API_AddSkinGore					= G2API_AddSkinGore;
	re.G2API_ClearSkinGore					= G2API_ClearSkinGore;
	#endif // _SOF2

	return &re;
}
