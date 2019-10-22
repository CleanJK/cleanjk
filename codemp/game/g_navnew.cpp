/*
===========================================================================
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

#include "game/b_local.h"

qboolean NAV_CheckAhead( gentity_t *self, vec3_t end, trace_t *trace, int clipmask );
qboolean NAV_TestForBlocked( gentity_t *self, gentity_t *goal, gentity_t *blocker, float distance, int *flags );

void G_Line( vec3_t start, vec3_t end, vec3_t color, float alpha );
void G_Cube( vec3_t mins, vec3_t maxs, vec3_t color, float alpha );
void G_CubeOutline( vec3_t mins, vec3_t maxs, int time, unsigned int color, float alpha );
void G_DrawEdge( vec3_t start, vec3_t end, int type );
void G_DrawNode( vec3_t origin, int type );
void G_DrawCombatPoint( vec3_t origin, int type );
void TAG_ShowTags( int flags );

qboolean NAV_CheckNodeFailedForEnt( gentity_t *ent, int nodeNum )
{
	int j;

	//FIXME: must be a better way to do this
	for ( j = 0; j < MAX_FAILED_NODES; j++ )
	{
		if ( ent->failedWaypoints[j] == nodeNum+1 )//+1 because 0 is a valid nodeNum, but also the default
		{//we failed against this node
			return qtrue;
		}
	}
	return qfalse;
}

int NAVNEW_ClearPathBetweenPoints(vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int ignore, int clipmask)
{
	trace_t	trace;

	//Test if they're even conceivably close to one another
	if ( !trap->InPVS( start, end ) )
	{
		return ENTITYNUM_WORLD;
	}

	trap->Trace( &trace, start, mins, maxs, end, ignore, clipmask, qfalse, 0, 0 );

	//if( ( ( trace.startsolid == false ) && ( trace.allsolid == false ) ) && ( trace.fraction < 1.0f ) )
	//{//FIXME: check for drops?
	//FIXME: if startsolid or allsolid, then the path isn't clear... but returning ENTITYNUM_NONE indicates to CheckFailedEdge that is is clear...?
		return trace.entityNum;
	//}

	//return ENTITYNUM_NONE;
}
