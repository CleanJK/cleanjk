/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
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

#include "game/g_local.h"

// SESSION DATA
// Session data is the only data that stays persistant across level loads and tournament restarts.
//TODO: Replace with reading/writing to file(s)

// Called on game shutdown
void G_WriteClientSessionData( gclient_t *client )
{
	char		s[MAX_CVAR_VALUE_STRING] = {0},
				IP[NET_ADDRSTRMAXLEN] = {0};
	const char	*var;
	int			i = 0;

	// for the strings, replace ' ' with 1
	Q_strncpyz( IP, client->sess.IP, sizeof( IP ) );
	for ( i=0; IP[i]; i++ ) {
		if (IP[i] == ' ')
			IP[i] = 1;
	}

	// Make sure there is no space on the last entry
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.sessionTeam ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.spectatorNum ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.spectatorState ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.spectatorClient ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.wins ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.losses ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.teamLeader ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.setForce ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.saberLevel ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.selectedFP ) );
	Q_strcat( s, sizeof( s ), va( "%i ", client->sess.duelTeam ) );
	Q_strcat( s, sizeof( s ), va( "%s", IP ) );

	var = va( "session%i", client - level.clients );

	trap->Cvar_Set( var, s );
}

// Called on a reconnect
void G_ReadSessionData( gclient_t *client )
{
	char			s[MAX_CVAR_VALUE_STRING] = {0};
	const char		*var;
	int			i=0, tempSessionTeam=0, tempSpectatorState, tempTeamLeader, tempSelectedFP;

	var = va( "session%i", client - level.clients );
	trap->Cvar_VariableStringBuffer( var, s, sizeof(s) );

	sscanf( s, "%i %i %i %i %i %i %i %i %i %i %i %s",
		&tempSessionTeam, //&client->sess.sessionTeam,
		&client->sess.spectatorNum,
		&tempSpectatorState, //&client->sess.spectatorState,
		&client->sess.spectatorClient,
		&client->sess.wins,
		&client->sess.losses,
		&tempTeamLeader, //&client->sess.teamLeader,
		&client->sess.setForce,
		&client->sess.saberLevel,
		&tempSelectedFP,
		&client->sess.duelTeam,
		client->sess.IP
		);

	client->sess.selectedFP     = (forcePowers_e)tempSelectedFP;
	client->sess.sessionTeam	= (team_e)tempSessionTeam;
	client->sess.spectatorState	= (spectatorState_e)tempSpectatorState;
	client->sess.teamLeader		= (bool)tempTeamLeader;

	// convert back to spaces from unused chars, as session data is written that way.
	for ( i=0; client->sess.IP[i]; i++ )
	{
		if (client->sess.IP[i] == 1)
			client->sess.IP[i] = ' ';
	}

	client->ps.fd.saberAnimLevel = client->sess.saberLevel;
	client->ps.fd.saberDrawAnimLevel = client->sess.saberLevel;
	client->ps.fd.forcePowerSelected = client->sess.selectedFP;
}

// Called on a first-time connect
void G_InitSessionData( gclient_t *client, char *userinfo, bool isBot ) {
	clientSession_t	*sess;
	const char		*value;

	sess = &client->sess;

	// initial team determination
	if ( level.gametype >= GT_TEAM ) {
		if ( g_teamAutoJoin.integer && !(g_entities[client-level.clients].r.svFlags & SVF_BOT) ) {
			sess->sessionTeam = PickTeam( -1 );
			client->ps.fd.forceDoInit = 1; //every time we change teams make sure our force powers are set right
		} else {
			// always spawn as spectator in team games
			if (!isBot)
			{
				sess->sessionTeam = TEAM_SPECTATOR;
			}
			else
			{ //Bots choose their team on creation
				value = Info_ValueForKey( userinfo, "team" );
				if (value[0] == 'r' || value[0] == 'R')
				{
					sess->sessionTeam = TEAM_RED;
				}
				else if (value[0] == 'b' || value[0] == 'B')
				{
					sess->sessionTeam = TEAM_BLUE;
				}
				else
				{
					sess->sessionTeam = PickTeam( -1 );
				}
				client->ps.fd.forceDoInit = 1; //every time we change teams make sure our force powers are set right
			}
		}
	} else {
		value = Info_ValueForKey( userinfo, "team" );
		if ( value[0] == 's' ) {
			// a willing spectator, not a waiting-in-line
			sess->sessionTeam = TEAM_SPECTATOR;
		} else {
			switch ( level.gametype ) {
			default:
			case GT_FFA:
			case GT_HOLOCRON:
			case GT_JEDIMASTER:
				if ( g_maxGameClients.integer > 0 &&
					level.numNonSpectatorClients >= g_maxGameClients.integer ) {
					sess->sessionTeam = TEAM_SPECTATOR;
				} else {
					sess->sessionTeam = TEAM_FREE;
				}
				break;
			case GT_DUEL:
				// if the game is full, go into a waiting mode
				if ( level.numNonSpectatorClients >= 2 ) {
					sess->sessionTeam = TEAM_SPECTATOR;
				} else {
					sess->sessionTeam = TEAM_FREE;
				}
				break;
			case GT_POWERDUEL:
				//sess->duelTeam = DUELTEAM_LONE; //default
				{
					int loners = 0;
					int doubles = 0;

					G_PowerDuelCount(&loners, &doubles, true);

					if (!doubles || loners > (doubles/2))
					{
						sess->duelTeam = DUELTEAM_DOUBLE;
					}
					else
					{
						sess->duelTeam = DUELTEAM_LONE;
					}
				}
				sess->sessionTeam = TEAM_SPECTATOR;
				break;
			}
		}
	}

	sess->spectatorState = SPECTATOR_FREE;
	AddTournamentQueue(client);

	G_WriteClientSessionData( client );
}

void G_InitWorldSession( void ) {
	// if the gametype changed since the last session, don't use any client sessions
	if ( level.gametype != session.integer ) {
		level.newSession = true;
		trap->Print( "Gametype changed, clearing session data.\n" );
	}
}

void G_WriteSessionData( void ) {
	int		i;

	trap->Cvar_Set( "session", va("%i", level.gametype) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			G_WriteClientSessionData( &level.clients[i] );
		}
	}
}
