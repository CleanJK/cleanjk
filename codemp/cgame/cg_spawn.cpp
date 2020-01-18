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

// Client-side functions for parsing entity data.

#include "cgame/cg_local.h"

bool CG_SpawnString( const char *key, const char *defaultString, char **out ) {
	int i;

	if( !cg.spawning ) {
		*out = (char*)defaultString;
		// trap->Error( ERR_DROP, "CG_SpawnString() called while not spawning" );
	}

	for( i = 0; i < cg.numSpawnVars; i++ ) {
		if( !Q_stricmp( key, cg.spawnVars[i][0] ) ) {
			*out = cg.spawnVars[i][1];
			return true;
		}
	}

	*out = (char*)defaultString;
	return false;
}
bool CG_SpawnFloat( const char *key, const char *defaultString, float *out ) {
	char *s;
	bool present;

	present = CG_SpawnString( key, defaultString, &s );
	*out	= atof( s );
	return present;
}
bool CG_SpawnInt( const char *key, const char *defaultString, int *out ) {
	char *s;
	bool present;

	present = CG_SpawnString( key, defaultString, &s );
	*out	= atoi( s );
	return present;
}
bool CG_SpawnBoolean( const char *key, const char *defaultString, bool *out ) {
	char *s;
	bool present;

	present = CG_SpawnString( key, defaultString, &s );
	if( !Q_stricmp( s, "false" ) || !Q_stricmp( s, "false" ) || !Q_stricmp( s, "no" ) || !Q_stricmp( s, "0" ) ) {
		*out = false;
	}
	else if( !Q_stricmp( s, "true" ) || !Q_stricmp( s, "true" ) || !Q_stricmp( s, "yes" ) || !Q_stricmp( s, "1" ) ) {
		*out = true;
	}
	else {
		*out = false;
	}

	return present;
}
bool CG_SpawnVector( const char *key, const char *defaultString, float *out ) {
	char *s;
	bool present;

	present = CG_SpawnString( key, defaultString, &s );
	if ( sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] ) != 3 ) {
		trap->Print( "CG_SpawnVector: Failed sscanf on %s (default: %s)\n", key, defaultString );
		VectorClear( out );
		return false;
	}
	return present;
}

// This is just a convenience function for printing vectors
char    *vtos( const vec3_t v ) {
	static int index;
	static char str[8][32];
	char *s;

	// use an array so that multiple vtos won't collide
	s	= str[index];
	index	= ( index + 1 ) & 7;

	Com_sprintf( s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2] );

	return s;
}
void SP_misc_model_static( void ) {
	char* model;
	float angle;
	vec3_t angles;
	float scale;
	vec3_t vScale;
	vec3_t org;
	float zoffset;
	int i;
	int modelIndex;
	cg_staticmodel_t *staticmodel;

	if( cgs.numMiscStaticModels >= MAX_STATIC_MODELS ) {
		trap->Error( ERR_DROP, "MAX_STATIC_MODELS(%i) hit", MAX_STATIC_MODELS );
	}

	CG_SpawnString( "model", "", &model );

	if( !model || !model[0] ) {
		trap->Error( ERR_DROP, "misc_model_static with no model." );
	}

	CG_SpawnVector( "origin", "0 0 0", org );
	CG_SpawnFloat( "zoffset", "0", &zoffset );

	if( !CG_SpawnVector( "angles", "0 0 0", angles ) ) {
		if( CG_SpawnFloat( "angle", "0", &angle ) ) {
			angles[YAW] = angle;
		}
	}

	if( !CG_SpawnVector( "modelscale_vec", "1 1 1", vScale ) ) {
		if( CG_SpawnFloat( "modelscale", "1", &scale ) ) {
			VectorSet( vScale, scale, scale, scale );
		}
	}

	modelIndex = trap->R_RegisterModel( model );
	if( modelIndex == 0 ) {
		trap->Error( ERR_DROP, "misc_model_static failed to load model '%s'", model );
		return;
	}

	staticmodel		= &cgs.miscStaticModels[cgs.numMiscStaticModels++];
	staticmodel->model	= modelIndex;
	AnglesToAxis( angles, staticmodel->axes );
	for( i = 0; i < 3; i++ ) {
		VectorScale( staticmodel->axes[i], vScale[i], staticmodel->axes[i] );
	}

	VectorCopy( org, staticmodel->org );
	staticmodel->zoffset = zoffset;

	if( staticmodel->model ) {
		vec3_t mins, maxs;

		trap->R_ModelBounds( staticmodel->model, mins, maxs );

		VectorScaleVector( mins, vScale, mins );
		VectorScaleVector( maxs, vScale, maxs );

		staticmodel->radius = RadiusFromBounds( mins, maxs );
	}
	else {
		staticmodel->radius = 0;
	}
}
bool cg_noFogOutsidePortal = false;
void SP_misc_skyportal( void ) {
	bool onlyfoghere;

	CG_SpawnBoolean( "onlyfoghere", "0", &onlyfoghere );

	if( onlyfoghere )
		cg_noFogOutsidePortal = true;
}
bool cg_skyOri = false;
vec3_t cg_skyOriPos;
float cg_skyOriScale = 0.0f;
void SP_misc_skyportal_orient( void ) {
	if( cg_skyOri )
		trap->Print( S_COLOR_YELLOW "WARNING: multiple misc_skyportal_orients found.\n" );

	cg_skyOri = true;
	CG_SpawnVector( "origin", "0 0 0", cg_skyOriPos );
	CG_SpawnFloat( "modelscale", "0", &cg_skyOriScale );
}
void SP_misc_weather_zone( void ) {
	char *model;
	vec3_t mins, maxs;

	CG_SpawnString( "model", "", &model );

	if( !model || !model[0] ) {
		trap->Error( ERR_DROP, "misc_weather_zone with invalid brush model data." );
		return;
	}

	trap->R_ModelBounds( trap->R_RegisterModel( model ), mins, maxs );

	trap->WE_AddWeatherZone( mins, maxs );
}
typedef struct spawn_s {
	const char	*name;
	void		(*spawn)( void );
} spawn_t;

constexpr spawn_t spawns [] = {
	{ "misc_model_static",     SP_misc_model_static },
	{ "misc_skyportal",        SP_misc_skyportal },
	{ "misc_skyportal_orient", SP_misc_skyportal_orient },
	{ "misc_weather_zone",     SP_misc_weather_zone },
};

// Spawn an entity and fill in all of the level fields from cg.spawnVars[], then call the class specfic spawn function
static int spawncmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, ((spawn_t*)b)->name );
}

void CG_ParseEntityFromSpawnVars( void ) {
	spawn_t *s;
	int i;
	char *classname;
	char *p, *value;
	const char *gametypeName;
	static constexpr const char *gametypeNames [GT_MAX_GAME_TYPE] = { "ffa", "holocron", "jedimaster", "duel", "powerduel", "team", "ctf", "cty" };

	// check for "notteam" flag (GT_FFA, GT_DUEL)
	if( cgs.gametype >= GT_TEAM ) {
		CG_SpawnInt( "notteam", "0", &i );
		if( i ) {
			return;
		}
	}
	else {
		CG_SpawnInt( "notfree", "0", &i );
		if( i ) {
			return;
		}
	}

	if( CG_SpawnString( "gametype", nullptr, &value ) ) {
		if( cgs.gametype >= GT_FFA && cgs.gametype < GT_MAX_GAME_TYPE ) {
			gametypeName = gametypeNames[cgs.gametype];

			p = strstr( value, gametypeName );
			if( !p ) {
				return;
			}
		}
	}

	if( CG_SpawnString( "classname", "", &classname ) ) {
		s = (spawn_t *)Q_LinearSearch( classname, spawns, ARRAY_LEN( spawns ), sizeof( spawn_t ), spawncmp );
		if ( s )
			s->spawn();
	}
}
char *CG_AddSpawnVarToken( const char *string ) {
	int l;
	char *dest;

	l = strlen( string );
	if( cg.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS ) {
		trap->Error( ERR_DROP, "CG_AddSpawnVarToken: MAX_SPAWN_VARS_CHARS" );
	}

	dest = cg.spawnVarChars + cg.numSpawnVarChars;
	memcpy( dest, string, l + 1 );

	cg.numSpawnVarChars += l + 1;

	return dest;
}

// Parses a brace bounded set of key / value pairs out of the level's entity strings into cg.spawnVars[]
// This does not actually spawn an entity.
bool CG_ParseSpawnVars( void ) {
	char keyname[MAX_TOKEN_CHARS];
	char com_token[MAX_TOKEN_CHARS];

	cg.numSpawnVars		= 0;
	cg.numSpawnVarChars	= 0;

	// parse the opening brace
	if( !trap->R_GetEntityToken( com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return false;
	}

	if( com_token[0] != '{' ) {
		trap->Error( ERR_DROP, "CG_ParseSpawnVars: found %s when expecting {", com_token );
	}

	// go through all the key / value pairs
	while( 1 ) {
		// parse key
		if( !trap->R_GetEntityToken( keyname, sizeof( keyname ) ) ) {
			trap->Error( ERR_DROP, "CG_ParseSpawnVars: EOF without closing brace" );
		}

		if( keyname[0] == '}' ) {
			break;
		}

		// parse value
		if( !trap->R_GetEntityToken( com_token, sizeof( com_token ) ) ) {
			trap->Error( ERR_DROP, "CG_ParseSpawnVars: EOF without closing brace" );
		}

		if( com_token[0] == '}' ) {
			trap->Error( ERR_DROP, "CG_ParseSpawnVars: closing brace without data" );
		}

		if( cg.numSpawnVars == MAX_SPAWN_VARS ) {
			trap->Error( ERR_DROP, "CG_ParseSpawnVars: MAX_SPAWN_VARS" );
		}

		cg.spawnVars[cg.numSpawnVars][0]	= CG_AddSpawnVarToken( keyname );
		cg.spawnVars[cg.numSpawnVars][1]	= CG_AddSpawnVarToken( com_token );
		cg.numSpawnVars++;
	}

	return true;
}
extern float cg_linearFogOverride;      // cg_view.c
extern float cg_radarRange;             // cg_draw.c
void SP_worldspawn( void ) {
	char *s;

	CG_SpawnString( "classname", "", &s );
	if( Q_stricmp( s, "worldspawn" ) ) {
		trap->Error( ERR_DROP, "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	CG_SpawnFloat( "fogstart", "0", &cg_linearFogOverride );
	CG_SpawnFloat( "radarrange", "2500", &cg_radarRange );
}

// Parses textual entity definitions out of an entstring
void CG_ParseEntitiesFromString( void ) {
	// make sure it is reset
	trap->R_GetEntityToken( nullptr, -1 );

	// allow calls to CG_Spawn*()
	cg.spawning	= true;
	cg.numSpawnVars = 0;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if( !CG_ParseSpawnVars() ) {
		trap->Error( ERR_DROP, "ParseEntities: no entities" );
	}

	SP_worldspawn();

	// parse ents
	while( CG_ParseSpawnVars() ) {
		CG_ParseEntityFromSpawnVars();
	}

	cg.spawning = false; // any future calls to CG_Spawn*() will be errors
}
