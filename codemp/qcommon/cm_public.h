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

#include "qcommon/q_shared.h"
#include "qcommon/q_files.h"

void          CM_AdjustAreaPortalState    ( int area1, int area2, bool open );
bool          CM_AreasConnected           ( int area1, int area2 );
int           CM_BoxLeafnums              ( const vec3_t mins, const vec3_t maxs, int *boxList, int listsize, int *lastLeaf );
void          CM_BoxTrace                 ( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, int capsule );
void          CM_CalcExtents              ( const vec3_t start, const vec3_t end, const struct traceWork_s* tw, vec3pair_t bounds );
void          CM_ClearMap                 ( void );
byte         *CM_ClusterPVS               ( int cluster );
bool          CM_CullWorldBox             ( const cplane_t *frustum, const vec3pair_t bounds );
bool          CM_DeleteCachedMap          ( bool bGuaranteedOkToDelete );
void          CM_DrawDebugSurface         ( void (*drawPoly)(int color, int numPoints, float *points) );
char         *CM_EntityString             ( void );
int           CM_FindSubBSP               ( int modelIndex );
bool          CM_GenericBoxCollide        ( const vec3pair_t abounds, const vec3pair_t bbounds );
clipHandle_t  CM_InlineModel              ( int index );
int           CM_LeafArea                 ( int leafnum );
int           CM_LeafCluster              ( int leafnum );
int           CM_LerpTag                  ( orientation_t *tag, clipHandle_t model, int startFrame, int endFrame, float frac, const char *tagName );
void          CM_LoadMap                  ( const char *name, bool clientload, int *checksum );
int           CM_LoadSubBSP               ( const char *name, bool clientload );
int           CM_MarkFragments            ( int numPoints, const vec3_t* points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer );
void          CM_ModelBounds              ( clipHandle_t model, vec3_t mins, vec3_t maxs );
int           CM_ModelContents            ( clipHandle_t model, int subBSPIndex );
int           CM_NumInlineModels          ( void );
int           CM_PointContents            ( const vec3_t p, clipHandle_t model );
int           CM_PointLeafnum             ( const vec3_t p );
char         *CM_SubBSPEntityString       ( int index );
clipHandle_t  CM_TempBoxModel             ( const vec3_t mins, const vec3_t maxs, int capsule );
int           CM_TransformedPointContents ( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles );
void          CM_TransformedBoxTrace      ( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles, int capsule );
int           CM_WriteAreaBits            ( byte *buffer, int area );

