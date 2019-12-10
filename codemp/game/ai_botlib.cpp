#include "qcommon/qcommon.h"
#include "game/g_local.h"
#define BOTLIB_INTERNAL
#include "game/ai_botlib.h"

aas_t             aasworld;
bot_goalstate_t   *botgoalstates[MAX_CLIENTS+1];
bot_movestate_t   *botmovestates[MAX_CLIENTS+1];
bot_weaponstate_t *botweaponstates[MAX_CLIENTS+1];
itemconfig_t      *itemconfig = NULL;
levelitem_t       *freelevelitems = NULL;
levelitem_t       *levelitems = NULL;
int               numaaslinks = 0;
int               numlevelitems = 0;
/*
levelitem_t     *levelitemheap = NULL;
maplocation_t   *maplocations = NULL;
campspot_t      *campspots = NULL;
libvar_t        *droppedweight = NULL;
*/

aas_link_t *AAS_AASLinkEntity( vec3_t absmins, vec3_t absmaxs, int entnum ) {
	int side, nodenum;
	aas_linkstack_t linkstack[128];
	aas_linkstack_t *lstack_p;
	aas_node_t *aasnode;
	aas_plane_t *plane;
	aas_link_t *link, *areas;

	if ( !aasworld.loaded ) {
		Com_Printf( "[ERROR] AAS_LinkEntity: aas not loaded\n" );
		return NULL;
	}

	areas = NULL;

	lstack_p = linkstack;
	//we start with the whole line on the stack
	//start with node 1 because node zero is a dummy used for solid leafs
	lstack_p->nodenum = 1;		//starting at the root of the tree
	lstack_p++;

	while ( 1 ) {
		//pop up the stack
		lstack_p--;
		//if the trace stack is empty (ended up with a piece of the
		//line to be traced in an area)
		if ( lstack_p < linkstack ) {
			break;
		}
		//number of the current node to test the line against
		nodenum = lstack_p->nodenum;
		//if it is an area
		if ( nodenum < 0 ) {
			//NOTE: the entity might have already been linked into this area
			// because several node children can point to the same area
			for ( link = aasworld.arealinkedentities[-nodenum]; link; link = link->next_ent ) {
				if ( link->entnum == entnum ) {
					break;
				}
			}
			if ( link ) {
				continue;
			}

			link = AAS_AllocAASLink();
			if ( !link ) {
				return areas;
			}
			link->entnum = entnum;
			link->areanum = -nodenum;
			//put the link into the double linked area list of the entity
			link->prev_area = NULL;
			link->next_area = areas;
			if ( areas ) {
				areas->prev_area = link;
			}
			areas = link;
			//put the link into the double linked entity list of the area
			link->prev_ent = NULL;
			link->next_ent = aasworld.arealinkedentities[-nodenum];
			if ( aasworld.arealinkedentities[-nodenum] ) {
				aasworld.arealinkedentities[-nodenum]->prev_ent = link;
			}
			aasworld.arealinkedentities[-nodenum] = link;
			continue;
		}
		//if solid leaf
		if ( !nodenum ) {
			continue;
		}
		//the node to test against
		aasnode = &aasworld.nodes[nodenum];
		//the current node plane
		plane = &aasworld.planes[aasnode->planenum];
		//get the side(s) the box is situated relative to the plane
		side = AAS_BoxOnPlaneSide2(absmins, absmaxs, plane);
		//if on the front side of the node
		if ( side & 1 ) {
			lstack_p->nodenum = aasnode->children[0];
			lstack_p++;
		}
		if ( lstack_p >= &linkstack[127] ) {
			Com_Printf( "[ERROR] AAS_LinkEntity: stack overflow\n" );
			break;
		}
		//if on the back side of the node
		if ( side & 2 ) {
			lstack_p->nodenum = aasnode->children[1];
			lstack_p++;
		}
		if ( lstack_p >= &linkstack[127] ) {
			Com_Printf( "[ERROR] AAS_LinkEntity: stack overflow\n" );
			break;
		}
	}
	return areas;
}

aas_link_t *AAS_AllocAASLink( void ) {
	aas_link_t *link;

	link = aasworld.freelinks;
	if ( !link ) {
		if ( bot_developer.integer ) {
			Com_Printf( "[FATAL] empty aas link heap\n" );
		}
		return NULL;
	}
	if ( aasworld.freelinks ) {
		aasworld.freelinks = aasworld.freelinks->next_ent;
	}
	if ( aasworld.freelinks ) {
		aasworld.freelinks->prev_ent = NULL;
	}
	numaaslinks--;
	return link;
}

qboolean AAS_AreaEntityCollision( int areanum, vec3_t start, vec3_t end, int presencetype, int passent, aas_trace_t *trace ) {
	int collision;
	vec3_t boxmins, boxmaxs;
	aas_link_t *link;
	bsp_trace_t bsptrace;

	AAS_PresenceTypeBoundingBox( presencetype, boxmins, boxmaxs );

	Com_Memset( &bsptrace, 0, sizeof(bsp_trace_t) ); //make compiler happy
	//assume no collision
	bsptrace.fraction = 1;
	collision = qfalse;
	for ( link = aasworld.arealinkedentities[areanum]; link; link = link->next_ent ) {
		//ignore the pass entity
		if ( link->entnum == passent ) {
			continue;
		}

		if ( AAS_EntityCollision( link->entnum, start, boxmins, boxmaxs, end, CONTENTS_SOLID|CONTENTS_PLAYERCLIP, &bsptrace ) ) {
			collision = qtrue;
		}
	}
	if ( collision ) {
		trace->startsolid = bsptrace.startsolid;
		trace->ent = bsptrace.ent;
		VectorCopy( bsptrace.endpos, trace->endpos );
		trace->area = 0;
		trace->planenum = 0;
		return qtrue;
	}
	return qfalse;
}

int AAS_AreaGrounded( int areanum ) {
	return !!(aasworld.areasettings[areanum].areaflags & AREA_GROUNDED);
}

int AAS_AreaJumpPad( int areanum ) {
	return !!(aasworld.areasettings[areanum].contents & AREACONTENTS_JUMPPAD);
}

int AAS_AreaReachability( int areanum ) {
	if ( areanum < 0 || areanum >= aasworld.numareas ) {
		Com_Printf( "AAS_AreaReachability: areanum %d out of range\n", areanum );
		return 0;
	}
	return aasworld.areasettings[areanum].numreachableareas;
}

int AAS_AreaSwim( int areanum ) {
	return !!(aasworld.areasettings[areanum].areaflags & AREA_LIQUID);
}

int AAS_BestReachableArea( vec3_t origin, vec3_t mins, vec3_t maxs, vec3_t goalorigin ) {
	vec3_t start, end;

	if ( !aasworld.loaded ) {
		Com_Printf( "[ERROR] AAS_BestReachableArea: aas not loaded\n");
		return 0;
	}
	//find a point in an area
	VectorCopy( origin, start );
	int areanum = AAS_PointAreaNum( start );
	//while no area found fudge around a little
	for ( int i=0; i<5 && !areanum; i++ ) {
		for ( int j=0; j<5 && !areanum; j++ ) {
			for ( int k=-1; k<=1 && !areanum; k++ ) {
				for ( int l=-1; l<=1 && !areanum; l++ ) {
					VectorCopy( origin, start );
					start[0] += (float) j * 4 * k;
					start[1] += (float) j * 4 * l;
					start[2] += (float) i * 4;
					areanum = AAS_PointAreaNum( start );
				}
			}
		}
	}

	//if an area was found
	if ( areanum ) {
		//drop client bbox down and try again
		VectorCopy( start, end );
		start[2] += 0.25;
		end[2] -= 50;
		aas_trace_t trace = AAS_TraceClientBBox( start, end, PRESENCE_CROUCH, -1 );
		if ( !trace.startsolid ) {
			areanum = AAS_PointAreaNum( trace.endpos );
			VectorCopy( trace.endpos, goalorigin );
			//FIXME: cannot enable next line right now because the reachability
			// does not have to be calculated when the level items are loaded
			//if the origin is in an area with reachability
			//if (AAS_AreaReachability(areanum)) return areanum;
			if ( areanum ) {
				return areanum;
			}
		}
		else {
			//it can very well happen that the AAS_PointAreaNum function tells that
			//a point is in an area and that starting an AAS_TraceClientBBox from that
			//point will return trace.startsolid qtrue
			VectorCopy( start, goalorigin );
			return areanum;
		}
	}

	//AAS_PresenceTypeBoundingBox(PRESENCE_CROUCH, bbmins, bbmaxs);
	//NOTE: the goal origin does not have to be in the goal area
	// because the bot will have to move towards the item origin anyway
	VectorCopy( origin, goalorigin );

	vec3_t absmins, absmaxs;
	VectorAdd( origin, mins, absmins );
	VectorAdd( origin, maxs, absmaxs );
	//add bounding box size
	//VectorSubtract( absmins, bbmaxs, absmins );
	//VectorSubtract( absmaxs, bbmins, absmaxs );
	//link an invalid (-1) entity
	aas_link_t *areas = AAS_LinkEntityClientBBox( absmins, absmaxs, -1, PRESENCE_CROUCH );
	//get the reachable link arae
	areanum = AAS_BestReachableLinkArea( areas );
	//unlink the invalid entity
	AAS_UnlinkFromAreas( areas );

	return areanum;
}

int AAS_BestReachableLinkArea( aas_link_t *areas ) {
	aas_link_t *link;

	for ( link = areas; link; link = link->next_area ) {
		if ( AAS_AreaGrounded( link->areanum ) || AAS_AreaSwim( link->areanum ) ) {
			return link->areanum;
		}
	}

	for ( link = areas; link; link = link->next_area ) {
		if ( link->areanum ) {
			return link->areanum;
		}
		//FIXME: this is a bad idea when the reachability is not yet calculated when the level items are loaded
		if ( AAS_AreaReachability( link->areanum ) ) {
			return link->areanum;
		}
	}
	return 0;
}

int AAS_BoxOnPlaneSide2( vec3_t absmins, vec3_t absmaxs, aas_plane_t *p ) {
	vec3_t corners[2];

	for ( int i=0; i<3; i++ ) {
		if ( p->normal[i] < 0 ) {
			corners[0][i] = absmins[i];
			corners[1][i] = absmaxs[i];
		}
		else {
			corners[1][i] = absmins[i];
			corners[0][i] = absmaxs[i];
		}
	}
	const float dist1 = DotProduct( p->normal, corners[0] ) - p->dist;
	const float dist2 = DotProduct( p->normal, corners[1] ) - p->dist;
	int sides = 0;
	if ( dist1 >= 0 ) {
		sides = 1;
	}
	if ( dist2 < 0 ) {
		sides |= 2;
	}

	return sides;
}

void AAS_DeAllocAASLink( aas_link_t *link ) {
	if ( aasworld.freelinks ) {
		aasworld.freelinks->prev_ent = link;
	}
	link->prev_ent = NULL;
	link->next_ent = aasworld.freelinks;
	link->prev_area = NULL;
	link->next_area = NULL;
	aasworld.freelinks = link;
	numaaslinks++;
}

qboolean AAS_EntityCollision( int entnum, vec3_t start, vec3_t boxmins, vec3_t boxmaxs, vec3_t end, int contentmask, bsp_trace_t *trace ) {
	bsp_trace_t enttrace;

	BotEntityTrace( &enttrace, start, boxmins, boxmaxs, end, entnum, contentmask );
	if ( enttrace.fraction < trace->fraction ) {
		Com_Memcpy( trace, &enttrace, sizeof(bsp_trace_t) );
		return qtrue;
	}
	return qfalse;
}

void AAS_EntityInfo( int entnum, aas_entityinfo_t *info ) {
	if ( !aasworld.initialized ) {
		Com_Printf( "[FATAL] AAS_EntityInfo: aasworld not initialized\n" );
		Com_Memset( info, 0, sizeof(aas_entityinfo_t) );
		return;
	}

	if ( entnum < 0 || entnum >= aasworld.maxentities ) {
		Com_Printf( "[FATAL] AAS_EntityInfo: entnum %d out of range\n", entnum );
		Com_Memset( info, 0, sizeof(aas_entityinfo_t) );
		return;
	}

	Com_Memcpy( info, &aasworld.entities[entnum].i, sizeof(aas_entityinfo_t) );
}

int AAS_EntityType( int entnum ) {
	if ( !aasworld.initialized ) {
		return 0;
	}

	if ( entnum < 0 || entnum >= aasworld.maxentities ) {
		Com_Printf( "[FATAL] AAS_EntityType: entnum %d out of range\n", entnum );
		return 0;
	}
	return aasworld.entities[entnum].i.type;
}

int AAS_EntityModelindex( int entnum ) {
	if ( entnum < 0 || entnum >= aasworld.maxentities ) {
		Com_Printf( "[FATAL] AAS_EntityModelindex: entnum %d out of range\n", entnum );
		return 0;
	}
	return aasworld.entities[entnum].i.modelindex;
}

aas_link_t *AAS_LinkEntityClientBBox( vec3_t absmins, vec3_t absmaxs, int entnum, int presencetype ) {
	vec3_t mins, maxs;
	vec3_t newabsmins, newabsmaxs;

	AAS_PresenceTypeBoundingBox( presencetype, mins, maxs );
	VectorSubtract( absmins, maxs, newabsmins );
	VectorSubtract( absmaxs, mins, newabsmaxs );
	//relink the entity
	return AAS_AASLinkEntity( newabsmins, newabsmaxs, entnum );
}

int AAS_NextEntity( int entnum ) {
	if ( !aasworld.loaded ) {
		return 0;
	}

	if ( entnum < 0 ) {
		entnum = -1;
	}
	while ( ++entnum < aasworld.maxentities ) {
		if ( aasworld.entities[entnum].i.valid ) {
			return entnum;
		}
	}
	return 0;
}

int AAS_PointAreaNum( vec3_t point ) {
	int nodenum;
	float	dist;
	aas_node_t *node;
	aas_plane_t *plane;

	if ( !aasworld.loaded ) {
		Com_Printf( "[ERROR] AAS_PointAreaNum: aas not loaded\n" );
		return 0;
	}

	//start with node 1 because node zero is a dummy used for solid leafs
	nodenum = 1;
	while ( nodenum > 0 ) {
//		Com_Printf( "[%d]", nodenum);
		node = &aasworld.nodes[nodenum];
		plane = &aasworld.planes[node->planenum];
		dist = DotProduct( point, plane->normal ) - plane->dist;
		if ( dist > 0 ) {
			nodenum = node->children[0];
		}
		else {
			nodenum = node->children[1];
		}
	}
	if ( !nodenum ) {
#ifdef AAS_SAMPLE_DEBUG
		Com_Printf( "in solid\n" );
#endif //AAS_SAMPLE_DEBUG
		return 0;
	}
	return -nodenum;
}

void AAS_PresenceTypeBoundingBox( int presencetype, vec3_t mins, vec3_t maxs ) {
	int index;
	//bounding box size for each presence type
	vec3_t boxmins[3] = {{0, 0, 0}, {-15, -15, -24}, {-15, -15, -24}};
	vec3_t boxmaxs[3] = {{0, 0, 0}, { 15,  15,  32}, { 15,  15,   8}};

	if ( presencetype == PRESENCE_NORMAL ) {
		index = 1;
	}
	else if ( presencetype == PRESENCE_CROUCH ) {
		index = 2;
	}
	else {
		Com_Printf( "[FATAL] AAS_PresenceTypeBoundingBox: unknown presence type\n" );
		index = 2;
	}
	VectorCopy( boxmins[index], mins );
	VectorCopy( boxmaxs[index], maxs );
}

float AAS_Time( void ) {
	return aasworld.time;
}

aas_trace_t AAS_TraceClientBBox( vec3_t start, vec3_t end, int presencetype, int passent ) {
	int side, nodenum, tmpplanenum;
	float front, back, frac;
	vec3_t cur_start, cur_end, cur_mid, v1, v2;
	aas_tracestack_t tracestack[127];
	aas_tracestack_t *tstack_p;
	aas_node_t *aasnode;
	aas_plane_t *plane;
	aas_trace_t trace;

	//clear the trace structure
	Com_Memset( &trace, 0, sizeof(aas_trace_t) );

	if ( !aasworld.loaded ) {
		return trace;
	}

	tstack_p = tracestack;
	//we start with the whole line on the stack
	VectorCopy( start, tstack_p->start );
	VectorCopy( end, tstack_p->end );
	tstack_p->planenum = 0;
	//start with node 1 because node zero is a dummy for a solid leaf
	tstack_p->nodenum = 1;		//starting at the root of the tree
	tstack_p++;

	while ( 1 ) {
		//pop up the stack
		tstack_p--;
		//if the trace stack is empty (ended up with a piece of the
		//line to be traced in an area)
		if ( tstack_p < tracestack ) {
			tstack_p++;
			//nothing was hit
			trace.startsolid = qfalse;
			trace.fraction = 1.0;
			//endpos is the end of the line
			VectorCopy( end, trace.endpos );
			//nothing hit
			trace.ent = 0;
			trace.area = 0;
			trace.planenum = 0;
			return trace;
		}
		//number of the current node to test the line against
		nodenum = tstack_p->nodenum;
		//if it is an area
		if ( nodenum < 0 ) {
			//Com_Printf(PRT_MESSAGE, "areanum = %d, must be %d\n", -nodenum, AAS_PointAreaNum(start));
			//if can't enter the area because it hasn't got the right presence type
			if ( !(aasworld.areasettings[-nodenum].presencetype & presencetype) ) {
				//if the start point is still the initial start point
				//NOTE: no need for epsilons because the points will be
				//exactly the same when they're both the start point
				if ( tstack_p->start[0] == start[0]
					&& tstack_p->start[1] == start[1]
					&& tstack_p->start[2] == start[2] )
				{
					trace.startsolid = qtrue;
					trace.fraction = 0.0;
					VectorClear( v1 );
				}
				else {
					trace.startsolid = qfalse;
					VectorSubtract( end, start, v1 );
					VectorSubtract( tstack_p->start, start, v2 );
					trace.fraction = VectorLength( v2 ) / VectorNormalize( v1 );
					VectorMA( tstack_p->start, -0.125, v1, tstack_p->start );
				}
				VectorCopy( tstack_p->start, trace.endpos );
				trace.ent = 0;
				trace.area = -nodenum;
//				VectorSubtract( end, start, v1 );
				trace.planenum = tstack_p->planenum;
				//always take the plane with normal facing towards the trace start
				plane = &aasworld.planes[trace.planenum];
				if ( DotProduct( v1, plane->normal ) > 0 ) {
					trace.planenum ^= 1;
				}
				return trace;
			}
			else {
				if ( passent >= 0 ) {
					if ( AAS_AreaEntityCollision( -nodenum, tstack_p->start, tstack_p->end, presencetype, passent, &trace ) ) {
						if ( !trace.startsolid ) {
							VectorSubtract( end, start, v1 );
							VectorSubtract( trace.endpos, start, v2 );
							trace.fraction = VectorLength( v2 ) / VectorLength( v1 );
						}
						return trace;
					}
				}
			}
			trace.lastarea = -nodenum;
			continue;
		}
		//if it is a solid leaf
		if ( !nodenum ) {
			//if the start point is still the initial start point
			//NOTE: no need for epsilons because the points will be
			//exactly the same when they're both the start point
			if ( tstack_p->start[0] == start[0]
				&& tstack_p->start[1] == start[1]
				&& tstack_p->start[2] == start[2] )
			{
				trace.startsolid = qtrue;
				trace.fraction = 0.0;
				VectorClear( v1 );
			}
			else {
				trace.startsolid = qfalse;
				VectorSubtract( end, start, v1 );
				VectorSubtract( tstack_p->start, start, v2 );
				trace.fraction = VectorLength( v2 ) / VectorNormalize( v1 );
				VectorMA( tstack_p->start, -0.125, v1, tstack_p->start );
			}
			VectorCopy( tstack_p->start, trace.endpos );
			trace.ent = 0;
			trace.area = 0;	//hit solid leaf
//			VectorSubtract(end, start, v1);
			trace.planenum = tstack_p->planenum;
			//always take the plane with normal facing towards the trace start
			plane = &aasworld.planes[trace.planenum];
			if ( DotProduct( v1, plane->normal ) > 0 ) {
				trace.planenum ^= 1;
			}
			return trace;
		}
		//the node to test against
		aasnode = &aasworld.nodes[nodenum];
		//start point of current line to test against node
		VectorCopy( tstack_p->start, cur_start );
		//end point of the current line to test against node
		VectorCopy( tstack_p->end, cur_end );
		//the current node plane
		plane = &aasworld.planes[aasnode->planenum];

		front = DotProduct( cur_start, plane->normal ) - plane->dist;
		back = DotProduct( cur_end, plane->normal ) - plane->dist;
		// bk010221 - old location of FPE hack and divide by zero expression
		//if the whole to be traced line is totally at the front of this node
		//only go down the tree with the front child
		if ( (front >= -ON_EPSILON && back >= -ON_EPSILON) ) {
			//keep the current start and end point on the stack
			//and go down the tree with the front child
			tstack_p->nodenum = aasnode->children[0];
			tstack_p++;
			if ( tstack_p >= &tracestack[127] ) {
				Com_Printf( "[ERROR] AAS_TraceBoundingBox: stack overflow\n" );
				return trace;
			}
		}
		//if the whole to be traced line is totally at the back of this node
		//only go down the tree with the back child
		else if ( (front < ON_EPSILON && back < ON_EPSILON) ) {
			//keep the current start and end point on the stack
			//and go down the tree with the back child
			tstack_p->nodenum = aasnode->children[1];
			tstack_p++;
			if ( tstack_p >= &tracestack[127] ) {
				Com_Printf( "[ERROR] AAS_TraceBoundingBox: stack overflow\n" );
				return trace;
			}
		}
		//go down the tree both at the front and back of the node
		else {
			tmpplanenum = tstack_p->planenum;
			// bk010221 - new location of divide by zero (see above)
			if ( front == back ) {
				front -= 0.001f; // bk0101022 - hack/FPE
			}
			//calculate the hitpoint with the node (split point of the line)
			//put the crosspoint TRACEPLANE_EPSILON pixels on the near side
			if ( front < 0 ) {
				frac = (front + TRACEPLANE_EPSILON)/(front-back);
			}
			else {
				frac = (front - TRACEPLANE_EPSILON)/(front-back); // bk010221
			}

			if (frac < 0) {
				frac = 0.001f; //0
			}
			else if (frac > 1) {
				frac = 0.999f; //1
			}
			//frac = front / (front-back);

			cur_mid[0] = cur_start[0] + (cur_end[0] - cur_start[0]) * frac;
			cur_mid[1] = cur_start[1] + (cur_end[1] - cur_start[1]) * frac;
			cur_mid[2] = cur_start[2] + (cur_end[2] - cur_start[2]) * frac;

//			AAS_DrawPlaneCross(cur_mid, plane->normal, plane->dist, plane->type, LINECOLOR_RED);
			//side the front part of the line is on
			side = front < 0;
			//first put the end part of the line on the stack (back side)
			VectorCopy( cur_mid, tstack_p->start );
			//not necesary to store because still on stack
			//VectorCopy(cur_end, tstack_p->end);
			tstack_p->planenum = aasnode->planenum;
			tstack_p->nodenum = aasnode->children[!side];
			tstack_p++;
			if ( tstack_p >= &tracestack[127] ) {
				Com_Printf( "[ERROR] AAS_TraceBoundingBox: stack overflow\n" );
				return trace;
			}
			//now put the part near the start of the line on the stack so we will
			//continue with thats part first. This way we'll find the first
			//hit of the bbox
			VectorCopy( cur_start, tstack_p->start );
			VectorCopy( cur_mid, tstack_p->end );
			tstack_p->planenum = tmpplanenum;
			tstack_p->nodenum = aasnode->children[side];
			tstack_p++;
			if ( tstack_p >= &tracestack[127] ) {
				Com_Printf( "[ERROR] AAS_TraceBoundingBox: stack overflow\n" );
				return trace;
			}
		}
	}
}

void AAS_UnlinkFromAreas( aas_link_t *areas ) {
	aas_link_t *link, *nextlink;

	for ( link = areas; link; link = nextlink ) {
		//next area the entity is linked in
		nextlink = link->next_area;
		//remove the entity from the linked list of this area
		if ( link->prev_ent ) {
			link->prev_ent->next_ent = link->next_ent;
		}
		else {
			aasworld.arealinkedentities[link->areanum] = link->next_ent;
		}
		if ( link->next_ent ) {
			link->next_ent->prev_ent = link->prev_ent;
		}
		//deallocate the link structure
		AAS_DeAllocAASLink( link );
	}
}

void AddLevelItemToList( levelitem_t *li ) {
	if ( levelitems ) {
		levelitems->prev = li;
	}
	li->prev = NULL;
	li->next = levelitems;
	levelitems = li;
}

levelitem_t *AllocLevelItem( void ) {
	levelitem_t *li;

	li = freelevelitems;
	if ( !li ) {
		Com_Printf( "[FATAL] out of level items\n" );
		return NULL;
	}

	freelevelitems = freelevelitems->next;
	Com_Memset( li, 0, sizeof(levelitem_t) );
	return li;
}

int BotAllocGoalState( int client ) {
	for ( int i=1; i<=MAX_CLIENTS; i++ ) {
		if ( !botgoalstates[i] ) {
			botgoalstates[i] = (bot_goalstate_t *)malloc( sizeof(bot_goalstate_t) );
			botgoalstates[i]->client = client;
			return i;
		}
	}
	return 0;
}

int BotAllocMoveState( void ) {
	for ( int i=1; i<=MAX_CLIENTS; i++ ) {
		if ( !botmovestates[i] ) {
			botmovestates[i] = (bot_movestate_t *)malloc( sizeof(bot_movestate_t) );
			return i;
		}
	}
	return 0;
}

int BotAllocWeaponState( void ) {
	for ( int i=1; i<=MAX_CLIENTS; i++ ) {
		if ( !botweaponstates[i] ) {
			botweaponstates[i] = (bot_weaponstate_t *)malloc( sizeof(bot_weaponstate_t) );
			return i;
		}
	}
	return 0;
}

void BotEntityTrace( bsp_trace_t *bsptrace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int entnum, int contentmask ) {
	trace_t trace;

	trap->TraceEntity( &trace, start, mins, maxs, end, entnum, contentmask, qfalse );
	//copy the trace information
	bsptrace->allsolid = (qboolean)trace.allsolid;
	bsptrace->startsolid = (qboolean)trace.startsolid;
	bsptrace->fraction = trace.fraction;
	VectorCopy( trace.endpos, bsptrace->endpos );
	bsptrace->plane.dist = trace.plane.dist;
	VectorCopy( trace.plane.normal, bsptrace->plane.normal );
	bsptrace->plane.signbits = trace.plane.signbits;
	bsptrace->plane.type = trace.plane.type;
	bsptrace->surface.value = trace.surfaceFlags;
	bsptrace->ent = trace.entityNum;
	bsptrace->exp_dist = 0;
	bsptrace->sidenum = 0;
	bsptrace->contents = 0;
}

void BotFreeGoalState( int handle ) {
	if ( handle <= 0 || handle > MAX_CLIENTS ) {
		Com_Printf( "[FATAL] goal state handle %d out of range\n", handle );
		return;
	}

	if ( !botgoalstates[handle] ) {
		Com_Printf( "[FATAL] invalid goal state handle %d\n", handle );
		return;
	}
	BotFreeItemWeights( handle );
	FreeMemory( botgoalstates[handle] );
	botgoalstates[handle] = NULL;
}

void BotFreeItemWeights( int goalstate ) {
	bot_goalstate_t *gs;

	gs = BotGoalStateFromHandle( goalstate );
	if ( !gs ) {
		return;
	}
	if ( gs->itemweightconfig ) {
		FreeWeightConfig( gs->itemweightconfig );
	}
	if ( gs->itemweightindex ) {
		FreeMemory( gs->itemweightindex );
	}
}

void BotFreeMoveState( int handle ) {
	if ( handle <= 0 || handle > MAX_CLIENTS ) {
		Com_Printf( "[FATAL move state handle %d out of range\n", handle );
		return;
	}
	if ( !botmovestates[handle] ) {
		Com_Printf( "[FATAL] invalid move state %d\n", handle );
		return;
	}
	FreeMemory( botmovestates[handle] );
	botmovestates[handle] = NULL;
}

void BotFreeWeaponState( int handle ) {
	if ( handle <= 0 || handle > MAX_CLIENTS ) {
		Com_Printf( "[FATAL] weapon state handle %d out of range\n", handle );
		return;
	}
	if ( !botweaponstates[handle] ) {
		Com_Printf( "[FATAL] invalid weapon state %d\n", handle );
		return;
	}
	BotFreeWeaponWeights( handle );
	FreeMemory( botweaponstates[handle] );
	botweaponstates[handle] = NULL;
}

void BotFreeWeaponWeights( int weaponstate ) {
	bot_weaponstate_t *ws;

	ws = BotWeaponStateFromHandle( weaponstate );
	if ( !ws ) {
		return;
	}
	if ( ws->weaponweightconfig ) {
		FreeWeightConfig( ws->weaponweightconfig );
	}
	if ( ws->weaponweightindex ) {
		FreeMemory( ws->weaponweightindex );
	}
}

bot_goalstate_t *BotGoalStateFromHandle( int handle ) {
	if ( handle <= 0 || handle > MAX_CLIENTS ) {
		Com_Printf( "[FATAL] goal state handle %d out of range\n", handle );
		return NULL;
	}
	if ( !botgoalstates[handle] ) {
		Com_Printf( "[FATAL] invalid goal state %d\n", handle );
		return NULL;
	}
	return botgoalstates[handle];
}

bot_movestate_t *BotMoveStateFromHandle( int handle ) {
	if ( handle <= 0 || handle > MAX_CLIENTS ) {
		Com_Printf( "[FATAL] move state handle %d out of range\n", handle );
		return NULL;
	}
	if ( !botmovestates[handle] ) {
		Com_Printf( "[FATAL] invalid move state %d\n", handle );
		return NULL;
	}
	return botmovestates[handle];
}

void BotResetAvoidGoals( int goalstate ) {
	bot_goalstate_t *gs = BotGoalStateFromHandle( goalstate );
	if ( !gs ) {
		return;
	}
	Com_Memset( gs->avoidgoals, 0, MAX_AVOIDGOALS * sizeof(int) );
	Com_Memset( gs->avoidgoaltimes, 0, MAX_AVOIDGOALS * sizeof(float) );
}

void BotResetAvoidReach( int movestate ) {
	bot_movestate_t *ms = BotMoveStateFromHandle( movestate );
	if ( !ms ) {
		return;
	}
	Com_Memset( ms->avoidreach, 0, MAX_AVOIDREACH * sizeof(int) );
	Com_Memset( ms->avoidreachtimes, 0, MAX_AVOIDREACH * sizeof(float) );
	Com_Memset( ms->avoidreachtries, 0, MAX_AVOIDREACH * sizeof(int) );
}

void BotResetGoalState( int goalstate ) {
	bot_goalstate_t *gs = BotGoalStateFromHandle( goalstate );
	if ( !gs ) {
		return;
	}
	Com_Memset( gs->goalstack, 0, MAX_GOALSTACK * sizeof(bot_goal_t) );
	gs->goalstacktop = 0;
	BotResetAvoidGoals( goalstate );
}

void BotResetMoveState( int movestate ) {
	bot_movestate_t *ms = BotMoveStateFromHandle( movestate );
	if ( !ms ) {
		return;
	}
	Com_Memset( ms, 0, sizeof(bot_movestate_t) );
}

void BotResetWeaponState( int weaponstate ) {
	bot_weaponstate_t *ws = BotWeaponStateFromHandle( weaponstate );
	if ( !ws ) {
		return;
	}

	weightconfig_t *weaponweightconfig = ws->weaponweightconfig;
	int *weaponweightindex = ws->weaponweightindex;

	//Com_Memset( ws, 0, sizeof(bot_weaponstate_t) );
	ws->weaponweightconfig = weaponweightconfig;
	ws->weaponweightindex = weaponweightindex;
}

void BotUpdateEntityItems( void ) {
	int ent, i, modelindex;
	vec3_t dir;
	levelitem_t *li, *nextli;
	aas_entityinfo_t entinfo;
	itemconfig_t *ic;

	//timeout current entity items if necessary
	for ( li = levelitems; li; li = nextli ) {
		nextli = li->next;
		//if it is an item that will time out
		if ( li->timeout ) {
			//timeout the item
			if ( li->timeout < AAS_Time() ) {
				RemoveLevelItemFromList( li );
				FreeLevelItem( li );
			}
		}
	}
	//find new entity items
	ic = itemconfig;
	if ( !itemconfig ) {
		return;
	}

	for ( ent = AAS_NextEntity( 0 ); ent; ent = AAS_NextEntity( ent ) ) {
		if ( AAS_EntityType( ent ) != ET_ITEM ) {
			continue;
		}
		//get the model index of the entity
		modelindex = AAS_EntityModelindex( ent );

		if ( !modelindex ) {
			continue;
		}
		//get info about the entity
		AAS_EntityInfo( ent, &entinfo );
		//FIXME: don't do this
		//skip all floating items for now
		//if (entinfo.groundent != ENTITYNUM_WORLD) continue;
		//if the entity is still moving
		if ( entinfo.origin[0] != entinfo.lastvisorigin[0]
			|| entinfo.origin[1] != entinfo.lastvisorigin[1]
			|| entinfo.origin[2] != entinfo.lastvisorigin[2] )
		{
			continue;
		}
		//check if the entity is already stored as a level item
		for ( li = levelitems; li; li = li->next ) {
			//if the level item is linked to an entity
			if ( li->entitynum && li->entitynum == ent ) {
				//the entity is re-used if the models are different
				if ( ic->iteminfo[li->iteminfo].modelindex != modelindex ) {
					//remove this level item
					RemoveLevelItemFromList( li );
					FreeLevelItem( li );
					li = NULL;
					break;
				}
				else {
					if (entinfo.origin[0] != li->origin[0]
						|| entinfo.origin[1] != li->origin[1]
						|| entinfo.origin[2] != li->origin[2] )
					{
						VectorCopy( entinfo.origin, li->origin );
						//also update the goal area number
						li->goalareanum = AAS_BestReachableArea( li->origin, ic->iteminfo[li->iteminfo].mins, ic->iteminfo[li->iteminfo].maxs, li->goalorigin );
					}
					break;
				}
			}
		}
		if ( li ) {
			continue;
		}
		//try to link the entity to a level item
		for ( li = levelitems; li; li = li->next ) {
			//if this level item is already linked
			if ( li->entitynum ) {
				continue;
			}

			else if ( g_gametype.integer >= GT_TEAM ) {
				if ( li->flags & IFL_NOTTEAM ) {
					continue;
				}
			}
			else {
				if ( li->flags & IFL_NOTFREE ) {
					continue;
				}
			}
			//if the model of the level item and the entity are the same
			if ( ic->iteminfo[li->iteminfo].modelindex == modelindex ) {
				//check if the entity is very close
				VectorSubtract( li->origin, entinfo.origin, dir );
				if ( VectorLength( dir ) < 30 ) {
					//found an entity for this level item
					li->entitynum = ent;
					//if the origin is different
					if (entinfo.origin[0] != li->origin[0]
						|| entinfo.origin[1] != li->origin[1]
						|| entinfo.origin[2] != li->origin[2] )
					{
						//update the level item origin
						VectorCopy( entinfo.origin, li->origin );
						//also update the goal area number
						li->goalareanum = AAS_BestReachableArea( li->origin, ic->iteminfo[li->iteminfo].mins, ic->iteminfo[li->iteminfo].maxs, li->goalorigin );
					}
#ifdef DEBUG
					Log_Write( "linked item %s to an entity", ic->iteminfo[li->iteminfo].classname );
#endif //DEBUG
					break;
				}
			}
		}
		if ( li ) {
			continue;
		}
		//check if the model is from a known item
		for ( i = 0; i < ic->numiteminfo; i++ ) {
			if ( ic->iteminfo[i].modelindex == modelindex ) {
				break;
			}
		}
		//if the model is not from a known item
		if ( i >= ic->numiteminfo ) {
			continue;
		}
		//allocate a new level item
		li = AllocLevelItem();

		if ( !li ) {
			continue;
		}
		//entity number of the level item
		li->entitynum = ent;
		//number for the level item
		li->number = numlevelitems + ent;
		//set the item info index for the level item
		li->iteminfo = i;
		//origin of the item
		VectorCopy( entinfo.origin, li->origin );
		//get the item goal area and goal origin
		li->goalareanum = AAS_BestReachableArea( li->origin, ic->iteminfo[i].mins, ic->iteminfo[i].maxs, li->goalorigin );
		//never go for items dropped into jumppads
		if ( AAS_AreaJumpPad( li->goalareanum ) ) {
			FreeLevelItem( li );
			continue;
		}
		//time this item out after 30 seconds
		//dropped items disappear after 30 seconds
		li->timeout = AAS_Time() + 30;
		//add the level item to the list
		AddLevelItemToList(li);
	}
}

bot_weaponstate_t *BotWeaponStateFromHandle( int handle ) {
	if ( handle <= 0 || handle > MAX_CLIENTS ) {
		Com_Printf( "[FATAL] weapon state handle %d out of range\n", handle );
		return NULL;
	}
	if ( !botweaponstates[handle] ) {
		Com_Printf( "[FATAL] invalid weapon state %d\n", handle );
		return NULL;
	}
	return botweaponstates[handle];
}

void FreeFuzzySeperators_r( fuzzyseperator_t *fs ) {
	if ( !fs ) {
		return;
	}
	if ( fs->child ) {
		FreeFuzzySeperators_r( fs->child );
	}
	if ( fs->next ) {
		FreeFuzzySeperators_r( fs->next );
	}
	FreeMemory( fs );
}

void FreeLevelItem( levelitem_t *li ) {
	li->next = freelevelitems;
	freelevelitems = li;
}

void FreeMemory( void *ptr ) {
#if 0
	memoryblock_t *block;

	block = BlockFromPointer( ptr, "FreeMemory" );
	if ( !block ) {
		return;
	}
	UnlinkMemoryBlock( block );
	allocatedmemory -= block->size;
	totalmemorysize -= block->size + sizeof(memoryblock_t);
	numblocks--;

	if ( block->id == MEM_ID ) {
		Z_Free( block );
	}
#else
	free( ptr );
#endif
}

void FreeWeightConfig( weightconfig_t *config ) {
	for ( int i=0; i<config->numweights; i++ ) {
		FreeFuzzySeperators_r( config->weights[i].firstseperator );
		if ( config->weights[i].name ) {
			FreeMemory( config->weights[i].name );
		}
	}
	FreeMemory( config );
}

void RemoveLevelItemFromList( levelitem_t *li ) {
	if ( li->prev ) {
		li->prev->next = li->next;
	}
	else {
		levelitems = li->next;
	}
	if ( li->next ) {
		li->next->prev = li->prev;
	}
}
