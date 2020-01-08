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

#include "server/server.h"

#include "ghoul2/ghoul2_shared.h"
#include "server/sv_gameapi.h"
#include "qcommon/com_cvar.h"
#include "qcommon/com_cvars.h"
#include "sys/sys_public.h"
#include "qcommon/huffman.h"

serverStatic_t	svs;				// persistant server info
server_t		sv;					// local server

serverBan_t serverBans[SERVER_MAXBANS];
int serverBansCount = 0;

// EVENT MESSAGES

// Converts newlines to "\n" so a line prints nicer
char	*SV_ExpandNewlines( char *in ) {
	static	char	string[1024];
	size_t	l;

	l = 0;
	while ( *in && l < sizeof(string) - 3 ) {
		if ( *in == '\n' ) {
			string[l++] = '\\';
			string[l++] = 'n';
		} else {
			string[l++] = *in;
		}
		in++;
	}
	string[l] = 0;

	return string;
}

// The given command will be transmitted to the client, and is guaranteed to not have future snapshot_t executed before it is executed
void SV_AddServerCommand( client_t *client, const char *cmd ) {
	int		index, i;

	// do not send commands until the gamestate has been sent
	if ( client->state < CS_PRIMED ) {
		return;
	}

	client->reliableSequence++;
	// if we would be losing an old command that hasn't been acknowledged,
	// we must drop the connection
	// we check == instead of >= so a broadcast print added by SV_DropClient()
	// doesn't cause a recursive drop client
	if ( client->reliableSequence - client->reliableAcknowledge == MAX_RELIABLE_COMMANDS + 1 ) {
		Com_Printf( "===== pending server commands =====\n" );
		for ( i = client->reliableAcknowledge + 1 ; i <= client->reliableSequence ; i++ ) {
			Com_Printf( "cmd %5d: %s\n", i, client->reliableCommands[ i & (MAX_RELIABLE_COMMANDS-1) ] );
		}
		Com_Printf( "cmd %5d: %s\n", i, cmd );
		SV_DropClient( client, "Server command overflow" );
		return;
	}
	index = client->reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
	Q_strncpyz( client->reliableCommands[ index ], cmd, sizeof( client->reliableCommands[ index ] ) );
}

// Sends a reliable command string to be interpreted by the client game module: "cp", "print", "chat", etc
// A nullptr client will broadcast to all clients
void QDECL SV_SendServerCommand(client_t *cl, const char *fmt, ...) {
	va_list		argptr;
	byte		message[MAX_MSGLEN];
	client_t	*client;
	int			j;

	va_start (argptr,fmt);
	Q_vsnprintf((char *)message, sizeof(message), fmt, argptr);
	va_end (argptr);

	// Fix to http://aluigi.altervista.org/adv/q3msgboom-adv.txt
	// The actual cause of the bug is probably further downstream
	// and should maybe be addressed later, but this certainly
	// fixes the problem for now
	if ( strlen ((char *)message) > 1022 ) {
		return;
	}

	if ( cl != nullptr ) {
		SV_AddServerCommand( cl, (char *)message );
		return;
	}

	// hack to echo broadcast prints to console
	if ( dedicated->integer && !Q_strncmp( (char *)message, "print", 5) ) {
		Com_Printf ("broadcast: %s\n", SV_ExpandNewlines((char *)message) );
	}

	// send the data to all relevent clients
	for (j = 0, client = svs.clients; j < sv_maxclients->integer ; j++, client++) {
		SV_AddServerCommand( client, (char *)message );
	}
}

// MASTER SERVER FUNCTIONS

#define NEW_RESOLVE_DURATION		86400000 //24 hours
static int g_lastResolveTime;

static inline bool SV_MasterNeedsResolving(int time)
{ //refresh every so often regardless of if the actual address was modified -rww
	if (g_lastResolveTime > time)
	{ //time flowed backwards?
		return true;
	}

	if ((time-g_lastResolveTime) > NEW_RESOLVE_DURATION)
	{ //it's time again
		return true;
	}

	return false;
}

#define	HEARTBEAT_MSEC	120*1000 // 2m
#define	HEARTBEAT_GAME	"QuakeArena-1"
// Send a message to the masters every few minutes to let it know we are alive, and log information.
// We will also have a heartbeat sent when a server changes from empty to non-empty, and full to non-full, but not on
//	every player enter or exit.
void SV_MasterHeartbeat( void ) {
	static netadr_t	adr;
	int			time;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !dedicated || dedicated->integer != 2 ) {
		return;		// only dedicated servers send heartbeats
	}

	// if not time yet, don't send anything
	if ( svs.time < svs.nextHeartbeatTime ) {
		return;
	}
	svs.nextHeartbeatTime = svs.time + HEARTBEAT_MSEC;

	// we need to use this instead of svs.time since svs.time resets over map changes (or rather every time the game
	//	restarts), and we don't really need to resolve every map change
	time = Com_Milliseconds();

	// send to group masters
	if ( !sv_master->string[0] ) {
		return;
	}

	// see if we haven't already resolved the name
	if ( sv_master->modified || SV_MasterNeedsResolving(time) ) {
		sv_master->modified = false;

		g_lastResolveTime = time;

		Com_Printf( "Resolving %s\n", sv_master->string );
		if ( !NET_StringToAdr( sv_master->string, &adr ) ) {
			// if the address failed to resolve, clear it
			// so we don't take repeated dns hits
			Com_Printf( "Couldn't resolve address: %s\n", sv_master->string );
			Cvar_Set( sv_master->name, "" );
			sv_master->modified = false;
			return;
		}
		if ( !strstr( ":", sv_master->string ) ) {
			adr.port = BigShort( PORT_MASTER );
		}
		Com_Printf( "%s resolved to %s\n", sv_master->string, NET_AdrToString(adr) );
	}

	Com_Printf ("Sending heartbeat to %s\n", sv_master->string );
	// this command should be changed if the server info / status format ever incompatably changes
	NET_OutOfBandPrint( NS_SERVER, adr, "heartbeat %s\n", HEARTBEAT_GAME );
}

// Informs all masters that this server is going down
void SV_MasterShutdown( void ) {
	// send a hearbeat right now
	svs.nextHeartbeatTime = -9999;
	SV_MasterHeartbeat();

	// send it again to minimize chance of drops
	svs.nextHeartbeatTime = -9999;
	SV_MasterHeartbeat();

	// when the master tries to poll the server, it won't respond, so
	// it will be removed from the list
}

// CONNECTIONLESS COMMANDS

// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS			16384
#define MAX_HASHES			1024

static leakyBucket_t buckets[ MAX_BUCKETS ];
static leakyBucket_t *bucketHashes[ MAX_HASHES ];
leakyBucket_t outboundLeakyBucket;

static long SVC_HashForAddress( netadr_t address ) {
	byte 		*ip = nullptr;
	size_t	size = 0;
	long		hash = 0;

	switch ( address.type ) {
		case NA_IP:  ip = address.ip;  size = 4; break;
		default: break;
	}

	for ( size_t i = 0; i < size; i++ ) {
		hash += (long)( ip[ i ] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( MAX_HASHES - 1 );

	return hash;
}

// Find or allocate a bucket for an address
static leakyBucket_t *SVC_BucketForAddress( netadr_t address, int burst, int period ) {
	leakyBucket_t	*bucket = nullptr;
	int						i;
	long					hash = SVC_HashForAddress( address );
	int						now = Sys_Milliseconds();

	for ( bucket = bucketHashes[ hash ]; bucket; bucket = bucket->next ) {
		switch ( bucket->type ) {
			case NA_IP:
				if ( memcmp( bucket->ipv._4, address.ip, 4 ) == 0 ) {
					return bucket;
				}
				break;

			default:
				break;
		}
	}

	for ( i = 0; i < MAX_BUCKETS; i++ ) {
		int interval;

		bucket = &buckets[ i ];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
					interval < 0 ) ) {
			if ( bucket->prev != nullptr ) {
				bucket->prev->next = bucket->next;
			} else {
				bucketHashes[ bucket->hash ] = bucket->next;
			}

			if ( bucket->next != nullptr ) {
				bucket->next->prev = bucket->prev;
			}

			Com_Memset( bucket, 0, sizeof( leakyBucket_t ) );
		}

		if ( bucket->type == NA_BAD ) {
			bucket->type = address.type;
			switch ( address.type ) {
				case NA_IP:  Com_Memcpy( bucket->ipv._4, address.ip, 4 );   break;
				default: break;
			}

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = bucketHashes[ hash ];
			if ( bucketHashes[ hash ] != nullptr ) {
				bucketHashes[ hash ]->prev = bucket;
			}

			bucket->prev = nullptr;
			bucketHashes[ hash ] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return nullptr;
}

bool SVC_RateLimit( leakyBucket_t *bucket, int burst, int period ) {
	if ( bucket != nullptr ) {
		int now = Sys_Milliseconds();
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst || interval < 0 ) {
			bucket->burst = 0;
			bucket->lastTime = now;
		} else {
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst ) {
			bucket->burst++;

			return false;
		}
	}

	return true;
}

// Rate limit for a particular address
bool SVC_RateLimitAddress( netadr_t from, int burst, int period ) {
	leakyBucket_t *bucket = SVC_BucketForAddress( from, burst, period );

	return SVC_RateLimit( bucket, burst, period );
}

// Responds with all the info that qplug or qspy can see about the server and all connected players.
// Used for getting detailed information after the simple info query.
void SVC_Status( netadr_t from ) {
	char	player[1024];
	char	status[MAX_MSGLEN];
	int		i;
	client_t	*cl;
	playerState_t	*ps;
	int		statusLength;
	int		playerLength;
	char	infostring[MAX_INFO_STRING];

	// Prevent using getstatus as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) ) {
		if ( developer->integer ) {
			Com_Printf( "SVC_Status: rate limit from %s exceeded, dropping request\n",
				NET_AdrToString( from ) );
		}
		return;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) ) {
		Com_DPrintf( "SVC_Status: rate limit exceeded, dropping request\n" );
		return;
	}

	// A maximum challenge length of 128 should be more than plenty.
	if(strlen(Cmd_Argv(1)) > 128)
		return;

	Q_strncpyz( infostring, Cvar_InfoString( CVAR_SERVERINFO ), sizeof( infostring ) );

	// echo back the parameter to status. so master servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv(1) );

	status[0] = 0;
	statusLength = 0;

	for (i=0 ; i < sv_maxclients->integer ; i++) {
		cl = &svs.clients[i];
		if ( cl->state >= CS_CONNECTED ) {
			ps = SV_GameClientNum( i );
			Com_sprintf (player, sizeof(player), "%i %i \"%s\"\n",
				ps->persistant[PERS_SCORE], cl->ping, cl->name);
			playerLength = strlen(player);
			if (statusLength + playerLength >= (int)sizeof(status) ) {
				break;		// can't hold any more
			}
			strcpy (status + statusLength, player);
			statusLength += playerLength;
		}
	}

	NET_OutOfBandPrint( NS_SERVER, from, "statusResponse\n%s\n%s", infostring, status );
}

// Responds with a short info message that should be enough to determine if a user is interested in a server to do a
//	full status
void SVC_Info( netadr_t from ) {
	int		i, count, humans, wDisable;
	char	infostring[MAX_INFO_STRING];

	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) ) {
		if ( developer->integer ) {
			Com_Printf( "SVC_Info: rate limit from %s exceeded, dropping request\n",
				NET_AdrToString( from ) );
		}
		return;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) ) {
		Com_DPrintf( "SVC_Info: rate limit exceeded, dropping request\n" );
		return;
	}

	// Check whether Cmd_Argv(1) has a sane length. This was not done in the original Quake3 version which led to the
	//	Infostring bug discovered by Luigi Auriemma. See http://aluigi.altervista.org/ for the advisory.

	// A maximum challenge length of 128 should be more than plenty.
	if(strlen(Cmd_Argv(1)) > 128)
		return;

	// don't count privateclients
	count = humans = 0;
	for ( i = sv_privateClients->integer ; i < sv_maxclients->integer ; i++ ) {
		if ( svs.clients[i].state >= CS_CONNECTED ) {
			count++;
			if ( svs.clients[i].netchan.remoteAddress.type != NA_BOT ) {
				humans++;
			}
		}
	}

	infostring[0] = 0;

	// echo back the parameter to status. so servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv(1) );

	Info_SetValueForKey( infostring, "protocol", va("%i", PROTOCOL_VERSION) );
	Info_SetValueForKey( infostring, "hostname", sv_hostname->string );
	Info_SetValueForKey( infostring, "mapname", mapname->string );
	Info_SetValueForKey( infostring, "clients", va("%i", count) );
	Info_SetValueForKey( infostring, "g_humanplayers", va("%i", humans) );
	Info_SetValueForKey( infostring, "sv_maxclients",
		va("%i", sv_maxclients->integer - sv_privateClients->integer ) );
	Info_SetValueForKey( infostring, "gametype", va("%i", g_gametype->integer ) );
	Info_SetValueForKey( infostring, "needpass", va("%i", g_needpass->integer ) );
	Info_SetValueForKey( infostring, "truejedi", va("%i", g_jediVmerc->integer ) );
	if ( g_gametype->integer == GT_DUEL || g_gametype->integer == GT_POWERDUEL )
	{
		wDisable = g_duelWeaponDisable->integer;
	}
	else
	{
		wDisable = g_weaponDisable->integer;
	}
	Info_SetValueForKey( infostring, "wdisable", va("%i", wDisable ) );
	Info_SetValueForKey( infostring, "fdisable", va("%i", g_forcePowerDisable->integer ) );
	//Info_SetValueForKey( infostring, "pure", va("%i", sv_pure->integer ) );
	Info_SetValueForKey( infostring, "autodemo", va("%i", sv_autoDemo->integer ) );

	if( sv_minPing->integer ) {
		Info_SetValueForKey( infostring, "minPing", va("%i", sv_minPing->integer) );
	}
	if( sv_maxPing->integer ) {
		Info_SetValueForKey( infostring, "maxPing", va("%i", sv_maxPing->integer) );
	}
	if( fs_game->string[0] ) {
		Info_SetValueForKey( infostring, "game", fs_game->string );
	}

	NET_OutOfBandPrint( NS_SERVER, from, "infoResponse\n%s", infostring );
}

void SV_FlushRedirect( char *outputbuf ) {
	NET_OutOfBandPrint( NS_SERVER, svs.redirectAddress, "print\n%s", outputbuf );
}

// An rcon packet arrived from the network.
// Shift down the remaining args
// Redirect all printfs
void SVC_RemoteCommand( netadr_t from, msg_t *msg ) {
	bool	valid;
	char		remaining[1024];
	// TTimo - scaled down to accumulate, but not overflow anything network wise, print wise etc.
	// (OOB messages are the bottleneck here)
#define	SV_OUTPUTBUF_LENGTH	(1024 - 16)
	char		sv_outputbuf[SV_OUTPUTBUF_LENGTH];
	char		*cmd_aux;

	// Prevent using rcon as an amplifier and make dictionary attacks impractical
	if ( SVC_RateLimitAddress( from, 10, 1000 ) ) {
		if ( developer->integer ) {
			Com_Printf( "SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n",
				NET_AdrToString( from ) );
		}
		return;
	}

	if ( !strlen( rconPassword->string ) ||
		strcmp (Cmd_Argv(1), rconPassword->string) ) {
		static leakyBucket_t bucket;

		// Make DoS via rcon impractical
		if ( SVC_RateLimit( &bucket, 10, 1000 ) ) {
			Com_DPrintf( "SVC_RemoteCommand: rate limit exceeded, dropping request\n" );
			return;
		}

		valid = false;
		Com_Printf ("Bad rcon from %s: %s\n", NET_AdrToString (from), Cmd_ArgsFrom(2) );
	} else {
		valid = true;
		Com_Printf ("Rcon from %s: %s\n", NET_AdrToString (from), Cmd_ArgsFrom(2) );
	}

	// start redirecting all print outputs to the packet
	svs.redirectAddress = from;
	Com_BeginRedirect (sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect);

	if ( !strlen( rconPassword->string ) ) {
		Com_Printf ("No rconpassword set.\n");
	} else if ( !valid ) {
		Com_Printf ("Bad rconpassword.\n");
	} else {
		remaining[0] = 0;

		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=543
		// get the command directly, "rcon <pass> <command>" to avoid quoting issues
		// extract the command by walking
		// since the cmd formatting can fuckup (amount of spaces), using a dumb step by step parsing
		cmd_aux = Cmd_Cmd();
		cmd_aux+=4;
		while(cmd_aux[0]==' ')
			cmd_aux++;
		while(cmd_aux[0] && cmd_aux[0]!=' ') // password
			cmd_aux++;
		while(cmd_aux[0]==' ')
			cmd_aux++;

		Q_strcat( remaining, sizeof(remaining), cmd_aux);

		Cmd_ExecuteString (remaining);
	}

	Com_EndRedirect ();
}

// A connectionless packet has four leading 0xff characters to distinguish it from a game channel.
// Clients that are in the game can still send connectionless packets.
void SV_ConnectionlessPacket( netadr_t from, msg_t *msg ) {
	char	*s;
	char	*c;

	MSG_BeginReadingOOB( msg );
	MSG_ReadLong( msg );		// skip the -1 marker

	if (!Q_strncmp("connect", (const char *)&msg->data[4], 7)) {
		Huff_Decompress(msg, 12);
	}

	s = MSG_ReadStringLine( msg );
	Cmd_TokenizeString( s );

	c = Cmd_Argv(0);
	if ( developer->integer ) {
		Com_Printf( "SV packet %s : %s\n", NET_AdrToString( from ), c );
	}

	if (!Q_stricmp(c, "getstatus")) {
		SVC_Status( from  );
	} else if (!Q_stricmp(c, "getinfo")) {
		SVC_Info( from );
	} else if (!Q_stricmp(c, "getchallenge")) {
		SV_GetChallenge( from );
	} else if (!Q_stricmp(c, "connect")) {
		SV_DirectConnect( from );
	} else if (!Q_stricmp(c, "ipAuthorize")) {
		// ...
	} else if (!Q_stricmp(c, "rcon")) {
		SVC_RemoteCommand( from, msg );
	} else if (!Q_stricmp(c, "disconnect")) {
		// if a client starts up a local server, we may see some spurious
		// server disconnect messages when their new server sees our final
		// sequenced messages to the old client
	} else {
		if ( developer->integer ) {
			Com_Printf( "bad connectionless packet from %s:\n%s\n",
				NET_AdrToString( from ), s );
		}
	}
}

void SV_PacketEvent( netadr_t from, msg_t *msg ) {
	int			i;
	client_t	*cl;
	int			qport;

	// check for connectionless packet (0xffffffff) first
	if ( msg->cursize >= 4 && *(int *)msg->data == -1) {
		SV_ConnectionlessPacket( from, msg );
		return;
	}

	// read the qport out of the message so we can fix up
	// stupid address translating routers
	MSG_BeginReadingOOB( msg );
	MSG_ReadLong( msg );				// sequence number
	qport = MSG_ReadShort( msg ) & 0x7fff;

	// find which client the message is from
	for (i=0, cl=svs.clients ; i < sv_maxclients->integer ; i++,cl++) {
		if (cl->state == CS_FREE) {
			continue;
		}
		if ( !NET_CompareBaseAdr( from, cl->netchan.remoteAddress ) ) {
			continue;
		}
		// it is possible to have multiple clients from a single IP
		// address, so they are differentiated by the qport variable
		if (cl->netchan.qport != qport) {
			continue;
		}

		// the IP port can't be used to differentiate them, because
		// some address translating routers periodically change UDP
		// port assignments
		if (cl->netchan.remoteAddress.port != from.port) {
			Com_Printf( "SV_ReadPackets: fixing up a translated port\n" );
			cl->netchan.remoteAddress.port = from.port;
		}

		// make sure it is a valid, in sequence packet
		if (SV_Netchan_Process(cl, msg)) {
			// zombie clients still need to do the Netchan_Process
			// to make sure they don't need to retransmit the final
			// reliable message, but they don't do any other processing
			if (cl->state != CS_ZOMBIE) {
				cl->lastPacketTime = svs.time;	// don't timeout
				SV_ExecuteClientMessage( cl, msg );
			}
		}
		return;
	}

	// if we received a sequenced packet from an address we don't recognize,
	// send an out of band disconnect packet to it
	NET_OutOfBandPrint( NS_SERVER, from, "disconnect" );
}

// Updates the cl->ping variables
void SV_CalcPings( void ) {
	int			i, j;
	client_t	*cl;
	int			total, count;
	int			delta;
	playerState_t	*ps;

	for (i=0 ; i < sv_maxclients->integer ; i++) {
		cl = &svs.clients[i];
		if ( cl->state != CS_ACTIVE ) {
			cl->ping = 999;
			continue;
		}
		if ( !cl->gentity ) {
			cl->ping = 999;
			continue;
		}
		if ( cl->gentity->r.svFlags & SVF_BOT ) {
			cl->ping = 0;
			continue;
		}

		total = 0;
		count = 0;
		for ( j = 0 ; j < PACKET_BACKUP ; j++ ) {
			if ( cl->frames[j].messageAcked <= 0 ) {
				continue;
			}
			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}
		if (!count) {
			cl->ping = 999;
		} else {
			cl->ping = total/count;
			if ( cl->ping > 999 ) {
				cl->ping = 999;
			}
		}

		// let the game dll know about the ping
		ps = SV_GameClientNum( i );
		ps->ping = cl->ping;
	}
}

// If a packet has not been received from a client for timeout->integer seconds, drop the connection.
// Server time is used instead of realtime to avoid dropping the local client while debugging.
// When a client is normally dropped, the client_t goes into a zombie state for a few seconds to make sure any final
//	reliable message gets resent if necessary
void SV_CheckTimeouts( void ) {
	int		i;
	client_t	*cl;
	int			droppoint;
	int			zombiepoint;

	droppoint = svs.time - 1000 * sv_timeout->integer;
	zombiepoint = svs.time - 1000 * sv_zombietime->integer;

	for (i=0,cl=svs.clients ; i < sv_maxclients->integer ; i++,cl++) {
		// message times may be wrong across a changelevel
		if (cl->lastPacketTime > svs.time) {
			cl->lastPacketTime = svs.time;
		}

		if (cl->state == CS_ZOMBIE
		&& cl->lastPacketTime < zombiepoint) {
			Com_DPrintf( "Going from CS_ZOMBIE to CS_FREE for %s\n", cl->name );
			cl->state = CS_FREE;	// can now be reused
			continue;
		}
		if ( cl->state >= CS_CONNECTED && cl->lastPacketTime < droppoint) {
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 ) {
				SV_DropClient (cl, "timed out");
				cl->state = CS_FREE;	// don't bother with zombie state
			}
		} else {
			cl->timeoutCount = 0;
		}
	}
}

bool SV_CheckPaused( void ) {
	int		count;
	client_t	*cl;
	int		i;

	if ( !cl_paused->integer ) {
		return false;
	}

	// only pause if there is just a single client connected
	count = 0;
	for (i=0,cl=svs.clients ; i < sv_maxclients->integer ; i++,cl++) {
		if ( cl->state >= CS_CONNECTED && cl->netchan.remoteAddress.type != NA_BOT ) {
			count++;
		}
	}

	if ( count > 1 ) {
		// don't pause
		if (sv_paused->integer)
			Cvar_Set("sv_paused", "0");
		return false;
	}

	if (!sv_paused->integer)
		Cvar_Set("sv_paused", "1");
	return true;
}

void SV_CheckCvars( void ) {
	static int lastModHostname = -1, lastModFramerate = -1, lastModSnapsMin = -1, lastModSnapsMax = -1;
	static int lastModSnapsPolicy = -1, lastModRatePolicy = -1, lastModClientRate = -1;
	static int lastModMaxRate = -1, lastModMinRate = -1;
	bool changed = false;

	if ( sv_hostname->modificationCount != lastModHostname ) {
		char hostname[MAX_INFO_STRING];
		char *c = hostname;
		lastModHostname = sv_hostname->modificationCount;

		strcpy( hostname, sv_hostname->string );
		while( *c )
		{
			if ( (*c == '\\') || (*c == ';') || (*c == '"'))
			{
				*c = '.';
				changed = true;
			}
			c++;
		}
		if( changed )
		{
			Cvar_Set("sv_hostname", hostname );
		}
	}

	// check limits on client "rate" values based on server settings
	if ( sv_clientRate->modificationCount != lastModClientRate ||
		 sv_minRate->modificationCount != lastModMinRate ||
		 sv_maxRate->modificationCount != lastModMaxRate ||
		 sv_ratePolicy->modificationCount != lastModRatePolicy )
	{
		sv_clientRate->modificationCount = lastModClientRate;
		sv_maxRate->modificationCount = lastModMaxRate;
		sv_minRate->modificationCount = lastModMinRate;
		sv_ratePolicy->modificationCount = lastModRatePolicy;

		if (sv_ratePolicy->integer == 1)
		{
			// NOTE: what if server sets some dumb sv_clientRate value?
			client_t *cl = nullptr;
			int i = 0;

			for (i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++) {
				// if the client is on the same subnet as the server and we aren't running an
				// internet public server, assume they don't need a rate choke
				if (Sys_IsLANAddress(cl->netchan.remoteAddress) && dedicated->integer != 2 && sv_lanForceRate->integer == 1) {
					cl->rate = 100000;	// lans should not rate limit
				}
				else {
					int val = sv_clientRate->integer;
					if (val != cl->rate) {
						cl->rate = val;
					}
				}
			}
		}
		else if (sv_ratePolicy->integer == 2)
		{
			// NOTE: what if server sets some dumb sv_clientRate value?
			client_t *cl = nullptr;
			int i = 0;

			for (i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++) {
				// if the client is on the same subnet as the server and we aren't running an
				// internet public server, assume they don't need a rate choke
				if (Sys_IsLANAddress(cl->netchan.remoteAddress) && dedicated->integer != 2 && sv_lanForceRate->integer == 1) {
					cl->rate = 100000;	// lans should not rate limit
				}
				else {
					int val = cl->rate;
					if (!val) {
						val = sv_maxRate->integer;
					}
					val = Com_Clampi( 1000, 90000, val );
					val = Com_Clampi( sv_minRate->integer, sv_maxRate->integer, val );
					if (val != cl->rate) {
						cl->rate = val;
					}
				}
			}
		}
	}

	// check limits on client "snaps" value based on server framerate and snapshot rate
	if ( sv_fps->modificationCount != lastModFramerate ||
		 sv_snapsMin->modificationCount != lastModSnapsMin ||
		 sv_snapsMax->modificationCount != lastModSnapsMax ||
		 sv_snapsPolicy->modificationCount != lastModSnapsPolicy )
	{
		lastModFramerate = sv_fps->modificationCount;
		lastModSnapsMin = sv_snapsMin->modificationCount;
		lastModSnapsMax = sv_snapsMax->modificationCount;
		lastModSnapsPolicy = sv_snapsPolicy->modificationCount;

		if (sv_snapsPolicy->integer == 1)
		{
			client_t *cl = nullptr;
			int i = 0;

			for (i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++) {
				int val = 1000 / sv_fps->integer;
				if (val != cl->snapshotMsec) {
					// Reset last sent snapshot so we avoid desync between server frame time and snapshot send time
					cl->nextSnapshotTime = -1;
					cl->snapshotMsec = val;
				}
			}
		}
		else if (sv_snapsPolicy->integer == 2)
		{
			client_t *cl = nullptr;
			int i = 0;
			int minSnaps = Com_Clampi(1, sv_snapsMax->integer, sv_snapsMin->integer); // between 1 and sv_snapsMax ( 1 <-> 40 )
			int maxSnaps = Q_min(sv_fps->integer, sv_snapsMax->integer); // can't produce more than sv_fps snapshots/sec, but can send less than sv_fps snapshots/sec

			for (i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++) {
				int val = 1000 / Com_Clampi(minSnaps, maxSnaps, cl->wishSnaps);
				if (val != cl->snapshotMsec) {
					// Reset last sent snapshot so we avoid desync between server frame time and snapshot send time
					cl->nextSnapshotTime = -1;
					cl->snapshotMsec = val;
				}
			}
		}
	}
}

// Return time in millseconds until processing of the next server frame.
int SV_FrameMsec()
{
	if(sv_fps)
	{
		int frameMsec;

		frameMsec = 1000.0f / sv_fps->value;

		if(frameMsec < sv.timeResidual)
			return 0;
		else
			return frameMsec - sv.timeResidual;
	}
	else
		return 1;
}

// Player movement occurs as a result of packet events, which happen before SV_Frame is called
void SV_Frame( int msec ) {
	int		frameMsec;
	int		startTime;

	// the menu kills the server with this cvar
	if ( sv_killserver->integer ) {
		SV_Shutdown ("Server was killed.\n");
		Cvar_Set( "sv_killserver", "0" );
		return;
	}

	if ( !sv_running->integer ) {
		return;
	}

	// allow pause if only the local client is connected
	if ( SV_CheckPaused() ) {
		return;
	}

	// if it isn't time for the next frame, do nothing
	if ( sv_fps->integer < 1 ) {
		Cvar_Set( "sv_fps", "10" );
	}
	frameMsec = 1000 / sv_fps->integer * timescale->value;
	// don't let it scale below 1ms
	if(frameMsec < 1)
	{
		Cvar_Set("timescale", va("%f", sv_fps->integer / 1000.0f));
		frameMsec = 1;
	}

	sv.timeResidual += msec;

	if (!dedicated->integer) SV_BotFrame( sv.time + sv.timeResidual );

	// if time is about to hit the 32nd bit, kick all clients
	// and clear sv.time, rather
	// than checking for negative time wraparound everywhere.
	// 2giga-milliseconds = 23 days, so it won't be too often
	if ( svs.time > 0x70000000 ) {
		SV_Shutdown( "Restarting server due to time wrapping" );
		Cbuf_AddText( va( "map %s\n", mapname->string ) );
		return;
	}
	// this can happen considerably earlier when lots of clients play and the map doesn't change
	if ( svs.nextSnapshotEntities >= 0x7FFFFFFE - svs.numSnapshotEntities ) {
		SV_Shutdown( "Restarting server due to numSnapshotEntities wrapping" );
		Cbuf_AddText( va( "map %s\n", mapname->string ) );
		return;
	}

	if( sv.restartTime && sv.time >= sv.restartTime ) {
		sv.restartTime = 0;
		Cbuf_AddText( "map_restart 0\n" );
		return;
	}

	// update infostrings if anything has been changed
	if ( cvar_modifiedFlags & CVAR_SERVERINFO ) {
		SV_SetConfigstring( CS_SERVERINFO, Cvar_InfoString( CVAR_SERVERINFO ) );
		cvar_modifiedFlags &= ~CVAR_SERVERINFO;
	}
	if ( cvar_modifiedFlags & CVAR_SYSTEMINFO ) {
		SV_SetConfigstring( CS_SYSTEMINFO, Cvar_InfoString_Big( CVAR_SYSTEMINFO ) );
		cvar_modifiedFlags &= ~CVAR_SYSTEMINFO;
	}

	if ( com_speeds->integer ) {
		startTime = Sys_Milliseconds ();
	} else {
		startTime = 0;	// quite a compiler warning
	}

	// update ping based on the all received frames
	SV_CalcPings();

	if (dedicated->integer) SV_BotFrame( sv.time );

	// run the game simulation in chunks
	while ( sv.timeResidual >= frameMsec ) {
		sv.timeResidual -= frameMsec;
		svs.time += frameMsec;
		sv.time += frameMsec;

		// let everything in the world think and move
		GVM_RunFrame( sv.time );
	}

	//rww - RAGDOLL_BEGIN
	re->G2API_SetTime(sv.time,0);
	//rww - RAGDOLL_END

	if ( com_speeds->integer ) {
		time_game = Sys_Milliseconds () - startTime;
	}

	// check timeouts
	SV_CheckTimeouts();

	// send messages back to the clients
	SV_SendClientMessages();

	SV_CheckCvars();

	// send a heartbeat to the master if needed
	SV_MasterHeartbeat();
}
