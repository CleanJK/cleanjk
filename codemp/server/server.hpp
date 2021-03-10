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
#include "qcommon/q_common.hpp"
#include "game/g_engine.hpp"
#include "game/bg_public.hpp"
#include "rd-common/tr_public.hpp"

#define MAX_ENT_CLUSTERS	16
#define PERS_SCORE			0 // !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
#define SERVER_MAXBANS		1024

enum serverState_e {
	SS_DEAD, // no map loaded
	SS_LOADING, // spawning level entities
	SS_GAME // actively running
};

enum clientState_e {
	CS_FREE, // can be reused for a new connection
	CS_ZOMBIE, // client has been disconnected, but don't reuse connection for a couple seconds
	CS_CONNECTED, // has been assigned to a client_t, but no gamestate yet
	CS_PRIMED, // gamestate has been sent, but client hasn't sent a usercmd
	CS_ACTIVE // client is fully in game
};

struct svEntity_t {
	struct worldSector_t *worldSector;
	svEntity_t           *nextEntityInWorldSector;
	entityState_t         baseline; // for delta compression of initial sighting
	int                   numClusters; // if -1, use headnode instead
	int                   clusternums[MAX_ENT_CLUSTERS];
	int                   lastCluster; // if all the clusters don't fit in clusternums
	int                   areanum, areanum2;
	int                   snapshotCounter; // used to prevent double adding from portal views
};

struct server_t {
	serverState_e   state;
	bool            restarting; // if true, send configstring changes during SS_LOADING
	int             serverId; // changes each server start
	int             restartedServerId; // serverId before a map_restart
	int             checksumFeed; //
	int             snapshotCounter; // incremented for each snapshot built
	int             timeResidual; // <= 1000 / sv_frame->value
	int             nextFrameTime; // when time > nextFrameTime, process world
	char           *configstrings[MAX_CONFIGSTRINGS];
	svEntity_t      svEntities[MAX_GENTITIES];
	char           *entityParsePoint; // used during game VM init
	sharedEntity_t *gentities; // the game virtual machine will update these on init and changes
	int             gentitySize;
	int             num_entities; // current number, <= MAX_GENTITIES
	playerState_t  *gameClients;
	int             gameClientSize; // will be > sizeof(playerState_t) due to game private data
	int             restartTime;
	int             time;
	int             mLocalSubBSPIndex;
	int             mLocalSubBSPModelOffset;
	char           *mLocalSubBSPEntityParsePoint;
	char           *mSharedMemory;
	time_t          realMapTimeStarted; // time the current map was started
	bool            demosPruned; // whether or not existing demos were cleaned up already
};

struct clientSnapshot_t {
	int             areabytes;
	byte            areabits[MAX_MAP_AREA_BYTES]; // portalarea visibility bits
	playerState_t   ps;
#ifdef _ONEBIT_COMBO
	int            *pDeltaOneBit;
	int            *pDeltaNumBit;
#endif
	int             num_entities;
	int             first_entity; // into the circular sv_packet_entities[] the entities MUST be in increasing state number order, otherwise the delta compression will fail
	int             messageSent; // time the message was transmitted
	int             messageAcked; // time the message was acked
	int             messageSize; // used to rate drop packets
};

struct demoInfo_t {
	char         demoName[MAX_OSPATH];
	bool         demorecording;
	bool         demowaiting; // don't record until a non-delta message is sent
	int          minDeltaFrame; // the first non-delta frame stored in the demo.  cannot delta against frames older than this
	fileHandle_t demofile;
	bool         isBot;
	int          botReliableAcknowledge; // for bots, need to maintain a separate reliableAcknowledge to record server messages into the demo file
};

struct client_t {
	clientState_e     state;
	char              userinfo[MAX_INFO_STRING]; // name, etc
	bool              sentGamedir; // see if he has been sent an svc_setgame
	char              reliableCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS];
	int               reliableSequence; // last added reliable message, not necesarily sent or acknowledged yet
	int               reliableAcknowledge; // last acknowledged reliable message
	int               reliableSent; // last sent reliable message, not necesarily acknowledged yet
	int               messageAcknowledge;
	int               gamestateMessageNum; // netchan->outgoingSequence of gamestate
	int               challenge;
	usercmd_t         lastUsercmd;
	int               lastMessageNum; // for delta compression
	int               lastClientCommand; // reliable client message sequence
	char              lastClientCommandString[MAX_STRING_CHARS];
	sharedEntity_t   *gentity; // SV_GentityNum(clientnum)
	char              name[MAX_NAME_LENGTH]; // extracted from userinfo, high bits masked
	char              downloadName[MAX_QPATH]; // if not empty string, we are downloading
	fileHandle_t      download; // file being downloaded
	int               downloadSize; // total bytes (can't use EOF because of paks)
	int               downloadCount; // bytes sent
	int               downloadClientBlock; // last block we sent to the client, awaiting ack
	int               downloadCurrentBlock; // current block number
	int               downloadXmitBlock; // last block we xmited
	unsigned char    *downloadBlocks[MAX_DOWNLOAD_WINDOW]; // the buffers for the download blocks
	int               downloadBlockSize[MAX_DOWNLOAD_WINDOW];
	bool              downloadEOF; // We have sent the EOF block
	int               downloadSendTime; // time we last got an ack from the client
	int               deltaMessage; // frame last client usercmd message
	int               lastReliableTime; // svs.time when reliable command was last received
	int               lastPacketTime; // svs.time when packet was last received
	int               lastConnectTime; // svs.time when connection started
	int               nextSnapshotTime; // send another snapshot when svs.time >= nextSnapshotTime
	bool              rateDelayed; // true if nextSnapshotTime was set based on rate instead of snapshotMsec
	int               timeoutCount; // must timeout a few frames in a row so debugging doesn't break
	clientSnapshot_t  frames[PACKET_BACKUP]; // updates can be delta'd from here
	int               ping;
	int               rate; // bytes / second
	int               snapshotMsec; // requests a snapshot every snapshotMsec unless rate choked
	int               wishSnaps; // requested snapshot/sec rate
	int               pureAuthentic;
	bool              gotCP; // TTimo - additional flag to distinguish between a bad pure checksum, and no cp command at all
	netchan_t         netchan;
	int               lastUserInfoChange; // if > svs.time && count > x, deny change -rww
	int               lastUserInfoCount; // allow a certain number of changes within a certain time period -rww
	int               oldServerTime;
	bool              csUpdated[MAX_CONFIGSTRINGS];
	demoInfo_t        demo;
};

// this structure will be cleared only when the game dll changes
struct serverStatic_t {
	bool           initialized; // sv_init has completed
	int            time; // will be strictly increasing across level changes
	time_t         startTime; // time since epoch the executable was started
	int            snapFlagServerBit; // ^= SNAPFLAG_SERVERCOUNT every SV_SpawnServer()
	client_t      *clients; // [sv_maxclients->integer];
	int            numSnapshotEntities; // sv_maxclients->integer*PACKET_BACKUP*MAX_SNAPSHOT_ENTITIES
	int            nextSnapshotEntities; // next snapshotEntities to use
	entityState_t *snapshotEntities; // [numSnapshotEntities]
	int            nextHeartbeatTime;
	netadr_t       redirectAddress; // for rcon return messages
	netadr_t       authorizeAddress; // for rcon return messages
	bool           gameStarted; // gvm is loaded
};

// Structure for managing bans
struct serverBan_t {
	netadr_t ip;
	int      subnet;
	bool isexception;
};

struct leakyBucket_t {
	netadrtype_e   type;
	union {
		byte _4[4];
	} ipv;
	int            lastTime;
	signed char    burst;
	long           hash;
	leakyBucket_t *prev, *next;
};



extern leakyBucket_t   outboundLeakyBucket;
extern refexport_t    *re; // interface to refresh .dll
extern serverBan_t     serverBans[SERVER_MAXBANS];
extern int             serverBansCount;
extern server_t        sv; // cleared each map
extern serverStatic_t  svs; // persistant server info across maps



void            Bot_FreeMemoryGame             ( void *ptr );
void           *Bot_GetMemoryGame              ( int size );
int             BotImport_DebugPolygonCreate   ( int color, int numPoints, vec3_t *points );
void            BotImport_DebugPolygonDelete   ( int id );
void            SV_AddOperatorCommands         ( void );
void            SV_AddServerCommand            ( client_t *client, const char *cmd );
int             SV_AreaEntities                ( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
void            SV_AutoRecordDemo              ( client_t *cl );
void            SV_BeginAutoRecordDemos        ( void );
int             SV_BotAllocateClient           ( void );
void            SV_BotCalculatePaths           ( int /*rmg*/ );
void            SV_BotFrame                    ( int time );
void            SV_BotFreeClient               ( int clientNum );
int             SV_BotGetConsoleMessage        ( int client, char *buf, int size );
int             SV_BotGetSnapshotEntity        ( int client, int ent );
void            SV_BotInitBotLib               ( void );
void            SV_BotInitCvars                ( void );
void            SV_BotWaypointReception        ( int wpnum, wpobject_t **wps );
void            SV_ChallengeInit               ( void );
void            SV_ChallengeShutdown           ( void );
void            SV_ChangeMaxClients            ( void );
void            SV_ClearWorld                  ( void );
void            SV_ClientEnterWorld            ( client_t *client, usercmd_t *cmd );
void            SV_ClientThink                 ( client_t *cl, usercmd_t *cmd );
clipHandle_t    SV_ClipHandleForEntity         ( const sharedEntity_t *ent );
void            SV_ClipToEntity                ( trace_t *trace, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int entityNum, int contentmask, int capsule );
int             SV_CreateChallenge             ( netadr_t from );
void            SV_CreateClientGameStateMessage( client_t *client, msg_t *msg );
void            SV_DirectConnect               ( netadr_t from );
void            SV_DropClient                  ( client_t *drop, const char *reason );
void            SV_ExecuteClientCommand        ( client_t *cl, const char *s, bool clientOK );
void            SV_ExecuteClientMessage        ( client_t *cl, msg_t *msg );
char           *SV_ExpandNewlines              ( char *in );
void            SV_FinalMessage                ( char *message );
playerState_t  *SV_GameClientNum               ( int num );
sharedEntity_t *SV_GEntityForSvEntity          ( svEntity_t *svEnt );
sharedEntity_t *SV_GentityNum                  ( int num );
void            SV_GetChallenge                ( netadr_t from );
void            SV_GetConfigstring             ( int index, char *buffer, int bufferSize );
const char     *SV_GetStringEdString           ( char *refSection, char *refName );
void            SV_GetUserinfo                 ( int index, char *buffer, int bufferSize );
void            SV_Heartbeat_f                 ( void );
void            SV_InitGameProgs               ( void );
bool            SV_inPVS                       ( const vec3_t p1, const vec3_t p2 );
void            SV_LinkEntity                  ( sharedEntity_t *ent );
void            SV_MasterHeartbeat             ( void );
void            SV_MasterShutdown              ( void );
bool            SV_Netchan_Process             ( client_t *client, msg_t *msg );
void            SV_Netchan_Transmit            ( client_t *client, msg_t *msg ); // int length, const byte *data );
void            SV_Netchan_TransmitNextFragment( netchan_t *chan );
int             SV_NumForGentity               ( sharedEntity_t *ent );
int             SV_PointContents               ( const vec3_t p, int passEntityNum );
void            SV_RecordDemo                  ( client_t *cl, char *demoName );
void            SV_RemoveOperatorCommands      ( void );
void            SV_SectorList_f                ( void );
void            SV_SendClientGameState         ( client_t *client );
void            SV_SendClientMapChange         ( client_t *client );
void            SV_SendClientMessages          ( void );
void            SV_SendClientSnapshot          ( client_t *client );
void            SV_SendMessageToClient         ( msg_t *msg, client_t *client );
void            SV_SendServerCommand           ( client_t *cl, const char *fmt, ...);
void            SV_SetConfigstring             ( int index, const char *val );
void            SV_SetUserinfo                 ( int index, const char *val );
void            SV_SpawnServer                 ( char *server, bool killBots, ForceReload_e eForceReload );
void            SV_StopAutoRecordDemos         ( void );
void            SV_StopRecordDemo              ( client_t *cl );
svEntity_t     *SV_SvEntityForGentity          ( sharedEntity_t *gEnt );
void            SV_Trace                       ( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, int capsule, int traceFlags, int useLod );
void            SV_UnlinkEntity                ( sharedEntity_t *ent );
void            SV_UpdateConfigstrings         ( client_t *client );
void            SV_UpdateServerCommandsToClient( client_t *client, msg_t *msg );
void            SV_UserinfoChanged             ( client_t *cl );
bool            SV_VerifyChallenge             ( int receivedChallenge, netadr_t from );
void            SV_WriteDemoMessage            ( client_t *cl, msg_t *msg, int headerBytes );
void            SV_WriteDownloadToClient       ( client_t *cl , msg_t *msg );
void            SV_WriteFrameToClient          ( client_t *client, msg_t *msg );
bool            SVC_RateLimit                  ( leakyBucket_t *bucket, int burst, int period );
bool            SVC_RateLimitAddress           ( netadr_t from, int burst, int period );

