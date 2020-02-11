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

#pragma once

#ifdef BOTLIB_INTERNAL

// ======================================================================
// DEFINE
// ======================================================================

using aas_vertex_t = vec3_t;
using aas_edgeindex_t = int32_t;
using aas_faceindex_t = int32_t;
using aas_portalindex_t = int32_t;

#define MAX_AVOIDGOALS     256
#define MAX_AVOIDREACH     1
#define MAX_AVOIDSPOTS     32
#define MAX_GOALSTACK      8
#define MAX_STRINGFIELD    80
#define MAX_TRAVELTYPES    32
#define MAX_WEIGHTS        128
#define ON_EPSILON         0.0005
#define TRACEPLANE_EPSILON 0.125

// ======================================================================
// ENUM
// ======================================================================

enum areaContents_e {
	AREACONTENTS_WATER =         1,
	AREACONTENTS_LAVA =          2,
	AREACONTENTS_SLIME =         4,
	AREACONTENTS_CLUSTERPORTAL = 8,
	AREACONTENTS_TELEPORTAL =    16,
	AREACONTENTS_ROUTEPORTAL =   32,
	AREACONTENTS_TELEPORTER =    64,
	AREACONTENTS_JUMPPAD =       128,
	AREACONTENTS_DONOTENTER =    256,
	AREACONTENTS_VIEWPORTAL =    512,
	AREACONTENTS_MOVER =         1024,
	AREACONTENTS_NOTTEAM1 =      2048,
	AREACONTENTS_NOTTEAM2 =      4096,
	AREACONTENTS_MODELNUMSHIFT = 24,
	AREACONTENTS_MAXMODELNUM =   0xFF,
	AREACONTENTS_MODELNUM =      (AREACONTENTS_MAXMODELNUM << AREACONTENTS_MODELNUMSHIFT),
};

enum areaFlag_e {
	AREA_GROUNDED = 1,  // bot can stand on the ground
	AREA_LADDER =   2,  // area contains one or more ladder faces
	AREA_LIQUID =   4,  // area contains a liquid
	AREA_DISABLED = 8,  // area is disabled for routing when set
	AREA_BRIDGE =   16, // area ontop of a bridge
};

enum itemFlag_e {
	IFL_NOTFREE =   1,  // not in free for all
	IFL_NOTTEAM =   2,  // not in team play
	IFL_NOTSINGLE = 4,  // not in single player
	IFL_NOTBOT =    8,  // bot should never go for this
	IFL_ROAM =      16, // bot roam goal
};

enum presenceType_e {
	PRESENCE_NONE =   1,
	PRESENCE_NORMAL = 2,
	PRESENCE_CROUCH = 4,
};

// ======================================================================
// STRUCT
// ======================================================================

struct aas_area_t {
	int    areanum;   // number of this area
	//3d definition
	int    numfaces;  // number of faces used for the boundary of the area
	int    firstface; // first face in the face index used for the boundary of the area
	vec3_t mins;      // mins of the area
	vec3_t maxs;      // maxs of the area
	vec3_t center;    // 'center' of the area
};

struct aas_areasettings_t {
	//could also add all kind of statistic fields
	int contents;           // contents of the area
	int areaflags;          // several area flags
	int presencetype;       // how a bot can be present in this area
	int cluster;            // cluster the area belongs to, if negative it's a portal
	int clusterareanum;     // number of the area in the cluster
	int numreachableareas;  // number of reachable areas from this one
	int firstreachablearea; // first reachable area in the reachable area index
};

struct aas_bbox_t {
	int    presencetype;
	int    flags;
	vec3_t mins, maxs;
};

struct aas_cluster_t {
	int numareas;             // number of areas in the cluster
	int numreachabilityareas; // number of areas with reachabilities
	int numportals;           // number of cluster portals
	int firstportal;          // first cluster portal in the index
};

struct aas_edge_t {
	int v[2]; // numbers of the vertexes of this edge
};

struct aas_entityinfo_t {
	int    valid;         // true if updated this frame
	int    type;          // entity type
	int    flags;         // entity flags
	float  ltime;         // local time
	float  update_time;   // time between last and current update
	int    number;        // number of the entity
	vec3_t origin;        // origin of the entity
	vec3_t angles;        // angles of the model
	vec3_t old_origin;    // for lerping
	vec3_t lastvisorigin; // last visible origin
	vec3_t mins;          // bounding box minimums
	vec3_t maxs;          // bounding box maximums
	int    groundent;     // ground entity
	int    solid;         // solid type
	int    modelindex;    // model used
	int    modelindex2;   // weapons, CTF flags, etc
	int    frame;         // model frame number
	int    event;         // impulse events -- muzzle flashes, footsteps, etc
	int    eventParm;     // even parameter
	int    powerups;      // bit flags
	int    weapon;        // determines weapon and flash model, etc
	int    legsAnim;      // current legs anim
	int    torsoAnim;     // current torso anim
};

struct aas_face_t {
	int planenum;  // number of the plane this face is in
	int faceflags; // face flags (no use to create face settings for just this field)
	int numedges;  // number of edges in the boundary of the face
	int firstedge; // first edge in the edge index
	int frontarea; // area at the front of this face
	int backarea;  // area at the back of this face
};

struct aas_link_t {
	int        entnum;
	int        areanum;
	aas_link_t *next_ent, *prev_ent;
	aas_link_t *next_area, *prev_area;
};

struct aas_linkstack_t {
	int nodenum; // node found after splitting
};

struct aas_node_t {
	int planenum;
	int children[2]; // child nodes of this node, or areas as leaves when negative. when a child is zero it's a solid leaf
};

struct aas_plane_t {
	vec3_t normal; // normal vector of the plane
	float  dist;   // distance of the plane (normal vector * distance = point in plane)
	int    type;
};

struct aas_portal_t {
	int areanum;           // area that is the actual portal
	int frontcluster;      // cluster at front of portal
	int backcluster;       // cluster at back of portal
	int clusterareanum[2]; // number of the area in the front and back cluster
};

struct aas_reachability_t {
	int      areanum;    // number of the reachable area
	int      facenum;    // number of the face towards the other area
	int      edgenum;    // number of the edge towards the other area
	vec3_t   start;      // start point of inter area movement
	vec3_t   end;        // end point of inter area movement
	int      traveltype; // type of travel required to get to the area
	uint16_t traveltime; // travel time of the inter area movement
};

struct aas_reachabilityareas_t {
	int firstarea, numareas;
};

struct aas_reversedlink_t {
	int                linknum; // the aas_areareachability_t
	int                areanum; // reachable from this area
	aas_reversedlink_t *next;   // next link
};

struct aas_reversedreachability_t {
	int                numlinks;
	aas_reversedlink_t *first;
};

struct aas_routingcache_t {
	byte               type;                   // portal or area cache
	float              time;                   // last time accessed or updated
	int                size;                   // size of the routing cache
	int                cluster;                // cluster the cache is for
	int                areanum;                // area the cache is created for
	vec3_t             origin;                 // origin within the area
	float              starttraveltime;        // travel time to start with
	int                travelflags;            // combinations of the travel flags
	aas_routingcache_t *prev, *next;
	aas_routingcache_t *time_prev, *time_next;
	byte               *reachabilities;        // reachabilities used for routing
	uint16_t           traveltimes[1];         // travel time for every area (variable sized)
};

struct aas_routingupdate_t {
	int                 cluster;
	int                 areanum;          // area number of the update
	vec3_t              start;            // start point the area was entered
	uint16_t            tmptraveltime;    // temporary travel time
	uint16_t            *areatraveltimes; // travel times within the area
	bool            inlist;           // true if the update is in the list
	aas_routingupdate_t *next;
	aas_routingupdate_t *prev;
};

struct aas_trace_t {
	bool startsolid; // if true, the initial point was in a solid area
	float    fraction;   // time completed, 1.0 = didn't hit anything
	vec3_t   endpos;     // final position
	int      ent;        // entity blocking the trace
	int      lastarea;   // last area the trace was in (zero if none)
	int      area;       // area blocking the trace (zero if none)
	int      planenum;   // number of the plane that was hit
};

struct aas_tracestack_t {
	vec3_t start;    // start point of the piece of line to trace
	vec3_t end;      // end point of the piece of line to trace
	int    planenum; // last plane used as splitter
	int    nodenum;  // node found after splitting with planenum
};

struct bot_avoidspot_t {
	vec3_t origin;
	float  radius;
	int    type;
};

struct bot_goal_t {
	vec3_t origin;     // origin of the goal
	int    areanum;    // area number of the goal
	vec3_t mins, maxs; // mins and maxs of the goal
	int    entitynum;  // number of the goal entity
	int    number;     // goal number
	int    flags;      // goal flags
	int    iteminfo;   // item information
};

struct bot_movestate_t {
	//input vars (all set outside the movement code)
	vec3_t origin;       // origin of the bot
	vec3_t velocity;     // velocity of the bot
	vec3_t viewoffset;   // view offset
	int    entitynum;    // entity number of the bot
	int    client;       // client number of the bot
	float  thinktime;    // time the bot thinks
	int    presencetype; // presencetype of the bot
	vec3_t viewangles;   // view angles of the bot
	//state vars
	int    areanum;                             // area the bot is in
	int    lastareanum;                         // last area the bot was in
	int    lastgoalareanum;                     // last goal area number
	int    lastreachnum;                        // last reachability number
	vec3_t lastorigin;                          // origin previous cycle
	int    reachareanum;                        // area number of the reachabilty
	int    moveflags;                           // movement flags
	int    jumpreach;                           // set when jumped
	float  grapplevisible_time;                 // last time the grapple was visible
	float  lastgrappledist;                     // last distance to the grapple end
	float  reachability_time;                   // time to use current reachability
	int    avoidreach[MAX_AVOIDREACH];          // reachabilities to avoid
	float  avoidreachtimes[MAX_AVOIDREACH];     // times to avoid the reachabilities
	int    avoidreachtries[MAX_AVOIDREACH];     // number of tries before avoiding
	bot_avoidspot_t avoidspots[MAX_AVOIDSPOTS]; // spots to avoid
	int numavoidspots;
};

struct bsp_link_t {
	int        entnum;
	int        leafnum;
	bsp_link_t *next_ent, *prev_ent;
	bsp_link_t *next_leaf, *prev_leaf;
};

struct aas_entity_t {
	aas_entityinfo_t i;
	aas_link_t       *areas;
	bsp_link_t       *leaves;
};

struct bsp_surface_t {
	char name[16];
	int  flags;
	int  value;
};

struct bsp_trace_t {
	bool      allsolid;   // if true, plane is not valid
	bool      startsolid; // if true, the initial point was in a solid area
	float         fraction;   // time completed, 1.0 = didn't hit anything
	vec3_t        endpos;     // final position
	cplane_t      plane;      // surface normal at impact
	float         exp_dist;   // expanded plane distance
	int           sidenum;    // number of the brush side hit
	bsp_surface_t surface;    // the hit point surface
	int           contents;   // contents on other side of surface hit
	int           ent;        // number of entity hit
};

struct iteminfo_t {
	char   classname[32];          // classname of the item
	char   name[MAX_STRINGFIELD];  // name of the item
	char   model[MAX_STRINGFIELD]; // model of the item
	int    modelindex;             // model index
	int    type;                   // item type
	int    index;                  // index in the inventory
	float  respawntime;            // respawn time
	vec3_t mins;                   // mins of the item
	vec3_t maxs;                   // maxs of the item
	int    number;                 // number of the item info
};

struct itemconfig_t {
	int        numiteminfo;
	iteminfo_t *iteminfo;
};

struct levelitem_t {
	int         number;      // number of the level item
	int         iteminfo;    // index into the item info
	int         flags;       // item flags
	float       weight;      // fixed roam weight
	vec3_t      origin;      // origin of the item
	int         goalareanum; // area the item is in
	vec3_t      goalorigin;  // goal origin within the area
	int         entitynum;   // entity number
	float       timeout;     // item is removed after this time
	levelitem_t *prev, *next;
};

struct aas_t {
	int                        loaded; // true when an AAS file is loaded
	int                        initialized; // true when AAS has been initialized
	int                        savefile; // set true when file should be saved
	int                        bspchecksum;
	float                      time;
	int                        numframes;
	char                       filename[MAX_QPATH];
	char                       mapname[MAX_QPATH];
	int                        numbboxes;
	aas_bbox_t                 *bboxes;
	int                        numvertexes;
	aas_vertex_t               *vertexes;
	int                        numplanes;
	aas_plane_t                *planes;
	int                        numedges;
	aas_edge_t                 *edges;
	int                        edgeindexsize;
	aas_edgeindex_t            *edgeindex;
	int                        numfaces;
	aas_face_t                 *faces;
	int                        faceindexsize;
	aas_faceindex_t            *faceindex;
	int                        numareas;
	aas_area_t                 *areas;
	int                        numareasettings;
	aas_areasettings_t         *areasettings;
	int                        reachabilitysize;
	aas_reachability_t         *reachability;
	int                        numnodes;
	aas_node_t                 *nodes;
	int                        numportals;
	aas_portal_t               *portals;
	int                        portalindexsize;
	aas_portalindex_t          *portalindex;
	int                        numclusters;
	aas_cluster_t              *clusters;
	int                        numreachabilityareas;
	float                      reachabilitytime;
	aas_link_t                 *linkheap; // heap with link structures
	int                        linkheapsize; // size of the link heap
	aas_link_t                 *freelinks; // first free link
	aas_link_t                 **arealinkedentities; // entities linked into areas
	int                        maxentities;
	int                        maxclients;
	aas_entity_t               *entities;
	int                        travelflagfortype[MAX_TRAVELTYPES];
	int                        *areacontentstravelflags;
	aas_routingupdate_t        *areaupdate;
	aas_routingupdate_t        *portalupdate;
	int                        frameroutingupdates;
	aas_reversedreachability_t *reversedreachability;
	unsigned short             ***areatraveltimes;
	aas_routingcache_t         ***clusterareacache;
	aas_routingcache_t         **portalcache;
	aas_routingcache_t         *oldestcache; // start of cache list sorted on time
	aas_routingcache_t         *newestcache; // end of cache list sorted on time
	int                        *portalmaxtraveltimes;
	int                        *reachabilityareaindex;
	aas_reachabilityareas_t    *reachabilityareas;
};

struct fuzzyseperator_t {
	int              index;
	int              value;
	int              type;
	float            weight;
	float            minweight;
	float            maxweight;
	fuzzyseperator_t *child;
	fuzzyseperator_t *next;
};

struct weight_t {
	char             *name;
	fuzzyseperator_t *firstseperator;
};

struct weightconfig_t {
	int      numweights;
	weight_t weights[MAX_WEIGHTS];
	char     filename[MAX_QPATH];
};

struct bot_goalstate_t {
	weightconfig_t *itemweightconfig;              // weight config
	int            *itemweightindex;               // index from item to weight
	int            client;                         // client using this goal state
	int            lastreachabilityarea;           // last area with reachabilities the bot was in
	bot_goal_t     goalstack[MAX_GOALSTACK];       // goal stack
	int            goalstacktop;                   // the top of the goal stack
	int            avoidgoals[MAX_AVOIDGOALS];     // goals to avoid
	float          avoidgoaltimes[MAX_AVOIDGOALS]; // times to avoid the goals
};

struct bot_weaponstate_t {
	weightconfig_t *weaponweightconfig; // weapon weight configuration
	int            *weaponweightindex;  // weapon weight index
};

// ======================================================================
// FUNCTION
// ======================================================================

aas_link_t* AAS_AASLinkEntity(vec3_t absmins, vec3_t absmaxs, int entnum);
aas_link_t* AAS_AllocAASLink(void);
aas_link_t* AAS_LinkEntityClientBBox(vec3_t absmins, vec3_t absmaxs, int entnum, int presencetype);
aas_trace_t AAS_TraceClientBBox(vec3_t start, vec3_t end, int presencetype, int passent);
bool AAS_AreaEntityCollision(int areanum, vec3_t start, vec3_t end, int presencetype, int passent, aas_trace_t* trace);
bool AAS_EntityCollision(int entnum, vec3_t start, vec3_t boxmins, vec3_t boxmaxs, vec3_t end, int contentmask, bsp_trace_t* trace);
bot_goalstate_t* BotGoalStateFromHandle(int handle);
bot_weaponstate_t* BotWeaponStateFromHandle(int handle);
int AAS_BestReachableLinkArea(aas_link_t* areas);
int AAS_BoxOnPlaneSide2(vec3_t absmins, vec3_t absmaxs, aas_plane_t* p);
levelitem_t* AllocLevelItem(void);
void AAS_DeAllocAASLink(aas_link_t* link);
void AAS_EntityInfo(int entnum, aas_entityinfo_t* info);
void AAS_UnlinkFromAreas(aas_link_t* areas);
void AddLevelItemToList(levelitem_t* li);
void BotEntityTrace(bsp_trace_t* bsptrace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int entnum, int contentmask);
void FreeLevelItem(levelitem_t* li);
void FreeWeightConfig(weightconfig_t* config);
void RemoveLevelItemFromList(levelitem_t* li);

#endif // BOTLIB_INTERNAL

float AAS_Time(void);
int AAS_AreaGrounded(int areanum);
int AAS_AreaJumpPad(int areanum);
int AAS_AreaReachability(int areanum);
int AAS_AreaSwim(int areanum);
int AAS_BestReachableArea(vec3_t origin, vec3_t mins, vec3_t maxs, vec3_t goalorigin);
int AAS_EntityModelindex(int entnum);
int AAS_EntityType(int entnum);
int AAS_NextEntity(int entnum);
int AAS_PointAreaNum(vec3_t point);
int BotAllocGoalState(int client);
int BotAllocMoveState(void);
int BotAllocWeaponState(void);
void AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
void BotFreeGoalState(int handle);
void BotFreeItemWeights(int goalstate);
void BotFreeMoveState(int handle);
void BotFreeWeaponState(int handle);
void BotFreeWeaponWeights(int weaponstate);
void BotResetAvoidGoals(int goalstate);
void BotResetAvoidReach(int movestate);
void BotResetGoalState(int goalstate);
void BotResetMoveState(int movestate);
void BotResetWeaponState(int weaponstate);
void BotUpdateEntityItems(void);
void FreeMemory(void* ptr);