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

#include "b_local.h"

void G_Line( vec3_t start, vec3_t end, vec3_t color, float alpha ) {
}
void G_Cube( vec3_t mins, vec3_t maxs, vec3_t color, float alpha ) {
}
void G_CubeOutline( vec3_t mins, vec3_t maxs, int time, unsigned int color, float alpha ) {
}
void G_DrawEdge( vec3_t start, vec3_t end, int type ) {
}
void G_DrawNode( vec3_t origin, int type ) {
}
void G_DrawCombatPoint( vec3_t origin, int type ) {
}
void TAG_ShowTags( int flags ) {
}

void SP_waypoint( gentity_t *ent )				{ G_FreeEntity( ent ); }
void SP_waypoint_small( gentity_t *ent )		{ G_FreeEntity( ent ); }
void SP_waypoint_navgoal( gentity_t *ent )		{ G_FreeEntity( ent ); }
void SP_waypoint_navgoal_8( gentity_t *ent )	{ G_FreeEntity( ent ); }
void SP_waypoint_navgoal_4( gentity_t *ent )	{ G_FreeEntity( ent ); }
void SP_waypoint_navgoal_2( gentity_t *ent )	{ G_FreeEntity( ent ); }
void SP_waypoint_navgoal_1( gentity_t *ent )	{ G_FreeEntity( ent ); }
