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

#include "qcommon/q_shared.hpp"

#define	MAX_POINTS_ON_WINDING	64

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define	SIDE_CROSS	3

#define	CLIP_EPSILON	0.1f

#define MAX_MAP_BOUNDS			65535

// you can define on_epsilon in the makefile as tighter
#ifndef	ON_EPSILON
#define	ON_EPSILON	0.1f
#endif



// this is only used for visualization tools in cm_ debug functions
struct winding_t {
	int		numpoints;
	vec3_t	p[4];		// variable sized
};



winding_t	*AllocWinding (int points);
void		 AddWindingToConvexHull( winding_t *w, winding_t **hull, vec3_t normal );
winding_t	*BaseWindingForPlane (vec3_t normal, float dist);
void		 ChopWindingInPlace (winding_t **w, vec3_t normal, float dist, float epsilon);
winding_t	*CopyWinding (winding_t *w);
void		 FreeWinding (winding_t *w);
void		 WindingBounds (winding_t *w, vec3_t mins, vec3_t maxs);
void		 pw(winding_t *w);

