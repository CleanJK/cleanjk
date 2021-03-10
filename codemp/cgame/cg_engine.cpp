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

#include "cgame/cg_local.hpp"

cgameImport_t *trap = nullptr;

// uniform functions
NORETURN_PTR void (*Com_Error)                    ( int level, const char *fmt, ... );
void              (*Com_Printf)                   ( const char *fmt, ... );

// shims
float Text_Width( const Text &text, const char *str ) {
	return trap->Text_Width( text, str );
}

float Text_Height( const Text &text, const char *str ) {
	return trap->Text_Height( text, str );
}

void Text_Paint( const Text &text, float x, float y, const char *str, const vec4_t colour, int style, int limit, float adjust, int cursorPos, bool overstrike ) {
	trap->Text_Paint( text, x, y, str, colour, style, limit, adjust, cursorPos, overstrike );
}

void TextHelper_Paint_Limit( float *maxX, float x, float y, float scale, const vec4_t color, const char *str, float adjust, int limit, JKFont font ) {
	trap->TextHelper_Paint_Limit( maxX, x, y, scale, color, str, adjust, limit, font );
}

void TextHelper_Paint_Proportional( int x, int y, const char *str, int style, const vec4_t color, JKFont font = JKFont::Medium, float scale = 1.0f ) {
	trap->TextHelper_Paint_Proportional( x, y, str, style, color, font, scale );
}

static int C_PointContents( void ) {
	TCGPointContents *data = &cg.sharedBuffer.pointContents;
	return CG_PointContents( data->mPoint, data->mPassEntityNum );
}

static void C_GetLerpOrigin( void ) {
	TCGVectorData *data = &cg.sharedBuffer.vectorData;
	VectorCopy( cg_entities[data->mEntityNum].lerpOrigin, data->mPoint );
}

// only used by FX system to pass to getboltmat
static void C_GetLerpData( void ) {
	TCGGetBoltData *data = &cg.sharedBuffer.getBoltData;

	VectorCopy( cg_entities[data->mEntityNum].lerpOrigin, data->mOrigin );
	VectorCopy( cg_entities[data->mEntityNum].modelScale, data->mScale );
	VectorCopy( cg_entities[data->mEntityNum].lerpAngles, data->mAngles );
	if ( cg_entities[data->mEntityNum].currentState.eType == ET_PLAYER ) {
		// normal player
		data->mAngles[PITCH] = 0.0f;
		data->mAngles[ROLL] = 0.0f;
	}
}

static void C_Trace( void ) {
	TCGTrace *td = &cg.sharedBuffer.trace;
	CG_Trace( &td->mResult, td->mStart, td->mMins, td->mMaxs, td->mEnd, td->mSkipNumber, td->mMask );
}

static void C_G2Trace( void ) {
	TCGTrace *td = &cg.sharedBuffer.trace;
	CG_G2Trace( &td->mResult, td->mStart, td->mMins, td->mMaxs, td->mEnd, td->mSkipNumber, td->mMask );
}

static void C_G2Mark( void ) {
	TCGG2Mark *td = &cg.sharedBuffer.g2Mark;
	trace_t tr;
	vec3_t end;

	VectorMA( td->start, 64.0f, td->dir, end );
	CG_G2Trace( &tr, td->start, nullptr, nullptr, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );

	if ( tr.entityNum < ENTITYNUM_WORLD && cg_entities[tr.entityNum].ghoul2 ) {
		// hit someone with a ghoul2 instance, let's project the decal on them then.
		centity_t *cent = &cg_entities[tr.entityNum];

	//	CG_TestLine( tr.endpos, end, 2000, 0x0000ff, 1 );

		CG_AddGhoul2Mark( td->shader, td->size, tr.endpos, end, tr.entityNum, cent->lerpOrigin, cent->lerpAngles[YAW],
			cent->ghoul2, cent->modelScale, Q_irand( 2000, 4000 ) );
		// I'm making fx system decals have a very short lifetime.
	}
}

static void CG_DebugBoxLines( vec3_t mins, vec3_t maxs, int duration ) {
	vec3_t start, end, vert;
	float x = maxs[0] - mins[0];
	float y = maxs[1] - mins[1];

	start[2] = maxs[2];
	vert[2] = mins[2];

	vert[0] = mins[0];
	vert[1] = mins[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine( start, vert, duration, 0x00000ff, 1 );

	vert[0] = mins[0];
	vert[1] = maxs[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine( start, vert, duration, 0x00000ff, 1 );

	vert[0] = maxs[0];
	vert[1] = mins[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine( start, vert, duration, 0x00000ff, 1 );

	vert[0] = maxs[0];
	vert[1] = maxs[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine( start, vert, duration, 0x00000ff, 1 );

	// top of box
	VectorCopy( maxs, start );
	VectorCopy( maxs, end );
	start[0] -= x;
	CG_TestLine( start, end, duration, 0x00000ff, 1 );
	end[0] = start[0];
	end[1] -= y;
	CG_TestLine( start, end, duration, 0x00000ff, 1 );
	start[1] = end[1];
	start[0] += x;
	CG_TestLine( start, end, duration, 0x00000ff, 1 );
	CG_TestLine( start, maxs, duration, 0x00000ff, 1 );
	// bottom of box
	VectorCopy( mins, start );
	VectorCopy( mins, end );
	start[0] += x;
	CG_TestLine( start, end, duration, 0x00000ff, 1 );
	end[0] = start[0];
	end[1] += y;
	CG_TestLine( start, end, duration, 0x00000ff, 1 );
	start[1] = end[1];
	start[0] -= x;
	CG_TestLine( start, end, duration, 0x00000ff, 1 );
	CG_TestLine( start, mins, duration, 0x00000ff, 1 );
}

//handle ragdoll callbacks, for events and debugging -rww
static int CG_RagCallback( int callType ) {
	switch ( callType ) {

	case RAG_CALLBACK_DEBUGBOX: {
		ragCallbackDebugBox_t *callData = &cg.sharedBuffer.rcbDebugBox;
		CG_DebugBoxLines( callData->mins, callData->maxs, callData->duration );
	} break;

	case RAG_CALLBACK_DEBUGLINE: {
		ragCallbackDebugLine_t *callData = &cg.sharedBuffer.rcbDebugLine;
		CG_TestLine( callData->start, callData->end, callData->time, callData->color, callData->radius );
	} break;

	case RAG_CALLBACK_BONESNAP: {
		ragCallbackBoneSnap_t *callData = &cg.sharedBuffer.rcbBoneSnap;
		centity_t *cent = &cg_entities[callData->entNum];
		int snapSound = trap->S_RegisterSound( va( "sound/player/bodyfall_human%i.wav", Q_irand( 1, 3 ) ) );
		trap->S_StartSound( cent->lerpOrigin, callData->entNum, CHAN_AUTO, snapSound );
	}

	case RAG_CALLBACK_BONEIMPACT: {
	} break;

	case RAG_CALLBACK_BONEINSOLID: {
	} break;

	case RAG_CALLBACK_TRACELINE: {
		ragCallbackTraceLine_t *callData = &cg.sharedBuffer.rcbTraceLine;
		CG_Trace( &callData->tr, callData->start, callData->mins, callData->maxs, callData->end, callData->ignore, callData->mask );
	} break;

	default: {
		Com_Error( ERR_DROP, "Invalid callType in CG_RagCallback" );
	} break;

	}

	return 0;
}

static void _CG_MouseEvent( int x, int y ) {
	cgDC.cursorx = cgs.cursorX;
	cgDC.cursory = cgs.cursorY;
	CG_MouseEvent( x, y );
}

static void CG_GetOrigin( int entID, vec3_t out ) {
	VectorCopy( cg_entities[entID].currentState.pos.trBase, out );
}

static void CG_GetAngles( int entID, vec3_t out ) {
	VectorCopy( cg_entities[entID].currentState.apos.trBase, out );
}

static trajectory_t *CG_GetOriginTrajectory( int entID ) {
	return &cg_entities[entID].nextState.pos;
}

static trajectory_t *CG_GetAngleTrajectory( int entID ) {
	return &cg_entities[entID].nextState.apos;
}

static void _CG_ROFF_NotetrackCallback( int entID, const char *notetrack ) {
	CG_ROFF_NotetrackCallback( &cg_entities[entID], notetrack );
}

static void CG_MapChange( void ) {
	// this may be called more than once for a given map change, as the server is going to attempt to send out
	// multiple broadcasts in hopes that the client will receive one of them
	cg.mMapChange = true;
}

static void CG_AutomapInput( void ) {
	autoMapInput_t *autoInput = &cg.sharedBuffer.autoMapInput;

	memcpy( &cg_autoMapInput, autoInput, sizeof( autoMapInput_t ) );

	if ( cg_autoMapInput.yaw ) {
		cg_autoMapAngle[YAW] += cg_autoMapInput.yaw;
	}
	if ( cg_autoMapInput.pitch ) {
		cg_autoMapAngle[PITCH] += cg_autoMapInput.pitch;
	}
	cg_autoMapInput.yaw = 0.0f;
	cg_autoMapInput.pitch = 0.0f;
}

static void CG_FX_CameraShake( void ) {
	TCGCameraShake *data = &cg.sharedBuffer.cameraShake;
	CG_DoCameraShake( data->mOrigin, data->mIntensity, data->mRadius, data->mTime );
}

Q_CABI {
Q_EXPORT cgameExport_t* QDECL GetModuleAPI( int apiVersion, cgameImport_t *import )
{
	static cgameExport_t cge = {0};

	assert( import );
	trap = import;
	Com_Printf	= trap->Print;
	Com_Error	= trap->Error;

	memset( &cge, 0, sizeof( cge ) );

	if ( apiVersion != CGAME_API_VERSION ) {
		trap->Print( "Mismatched CGAME_API_VERSION: expected %i, got %i\n", CGAME_API_VERSION, apiVersion );
		return nullptr;
	}

	cge.Init					= CG_Init;
	cge.Shutdown				= CG_Shutdown;
	cge.ConsoleCommand			= CG_ConsoleCommand;
	cge.DrawActiveFrame			= CG_DrawActiveFrame;
	cge.CrosshairPlayer			= CG_CrosshairPlayer;
	cge.LastAttacker			= CG_LastAttacker;
	cge.KeyEvent				= CG_KeyEvent;
	cge.MouseEvent				= _CG_MouseEvent;
	cge.EventHandling			= CG_EventHandling;
	cge.PointContents			= C_PointContents;
	cge.GetLerpOrigin			= C_GetLerpOrigin;
	cge.GetLerpData				= C_GetLerpData;
	cge.Trace					= C_Trace;
	cge.G2Trace					= C_G2Trace;
	cge.G2Mark					= C_G2Mark;
	cge.RagCallback				= CG_RagCallback;
	cge.NoUseableForce			= CG_NoUseableForce;
	cge.GetOrigin				= CG_GetOrigin;
	cge.GetAngles				= CG_GetAngles;
	cge.GetOriginTrajectory		= CG_GetOriginTrajectory;
	cge.GetAngleTrajectory		= CG_GetAngleTrajectory;
	cge.ROFF_NotetrackCallback	= _CG_ROFF_NotetrackCallback;
	cge.MapChange				= CG_MapChange;
	cge.AutomapInput			= CG_AutomapInput;
	cge.MiscEnt					= CG_MiscEnt;
	cge.CameraShake				= CG_FX_CameraShake;

	return &cge;
}
}
