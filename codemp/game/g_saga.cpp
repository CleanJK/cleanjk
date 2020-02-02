/*
===========================================================================
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

// Game-side module for Siege gametype.

#include "game/g_local.h"

//bypass most of the normal checks in SetTeam
void SetTeamQuick( gentity_t *ent, team_e team, bool doBegin ) {
	char userinfo[MAX_INFO_STRING];

	trap->GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );

	ent->client->sess.sessionTeam = team;

	if (team == TEAM_SPECTATOR)
	{
		ent->client->sess.spectatorState = SPECTATOR_FREE;
		Info_SetValueForKey(userinfo, "team", "s");
	}
	else
	{
		ent->client->sess.spectatorState = SPECTATOR_NOT;
		if (team == TEAM_RED)
		{
			Info_SetValueForKey(userinfo, "team", "r");
		}
		else if (team == TEAM_BLUE)
		{
			Info_SetValueForKey(userinfo, "team", "b");
		}
		else
		{
			Info_SetValueForKey(userinfo, "team", "?");
		}
	}

	trap->SetUserinfo( ent->s.number, userinfo );

	ent->client->sess.spectatorClient = 0;

	ent->client->pers.teamState.state = TEAM_BEGIN;

	if ( !ClientUserinfoChanged( ent->s.number ) )
		return;

	if (doBegin)
	{
		ClientBegin( ent->s.number, false );
	}
}
