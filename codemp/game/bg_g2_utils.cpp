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

// bg_g2_utils.c -- both games misc functions, all completely stateless
// only in game and cgame, NOT ui

#include "qcommon/q_shared.hpp"
#include "game/bg_public.hpp"

#if defined(BUILD_GAME)
	#include "game/g_local.hpp"
#elif defined(BUILD_CGAME)
	#include "cgame/cg_local.hpp"
#endif

#define	MAX_VARIANTS 8
bool BG_GetRootSurfNameWithVariant( void *ghoul2, const char *rootSurfName, char *returnSurfName, int returnSize )
{
#if defined(BUILD_GAME)
	if ( !ghoul2 || !trap->G2API_GetSurfaceRenderStatus( ghoul2, 0, rootSurfName ) )
#elif defined(BUILD_CGAME)
	if ( !ghoul2 || !trap->G2API_GetSurfaceRenderStatus( ghoul2, 0, rootSurfName ) )
#endif
	{//see if the basic name without variants is on
		Q_strncpyz( returnSurfName, rootSurfName, returnSize );
		return true;
	}
	else
	{//check variants
		int i;
		for ( i = 0; i < MAX_VARIANTS; i++ )
		{
			Com_sprintf( returnSurfName, returnSize, "%s%c", rootSurfName, 'a'+i );
		#if defined(BUILD_GAME)
			if ( !trap->G2API_GetSurfaceRenderStatus( ghoul2, 0, returnSurfName ) )
		#elif defined(BUILD_CGAME)
			if ( !trap->G2API_GetSurfaceRenderStatus( ghoul2, 0, returnSurfName ) )
		#endif
			{
				return true;
			}
		}
	}
	Q_strncpyz( returnSurfName, rootSurfName, returnSize );
	return false;
}

