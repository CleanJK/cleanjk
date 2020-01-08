#include "cgame/cg_local.h"
#include "ui/ui_shared.h"
#include "cgame/cg_media.h"

extern displayContextDef_t cgDC;

cgMedia_t media;

typedef struct progress_s {
	size_t		numLoadedResources;
	bool	loadedMap;
} progress_t;
static progress_t progress;

// resource flags
#define RFL_NONE		(0x00000000u)
#define RFL_NOMIP		(0x00000001u)

typedef struct resource_s {
	void		*handle;
	const char	*location;
	uint32_t	 flags;
	uint32_t	 gametypes;
} resource_t;

static const resource_t efx[] = {
	{ &media.efx.null, "", RFL_NONE, GTB_ALL }
};

static const resource_t gfx[] = {
//	{ &media.gfx.null,                 "",                     RFL_NONE,  GTB_ALL },
	{ &media.gfx.hud.crosshairs[0],    "gfx/hud/crosshair/a",  RFL_NOMIP, GTB_ALL },
	{ &media.gfx.hud.crosshairs[1],    "gfx/hud/crosshair/b",  RFL_NOMIP, GTB_ALL },
	{ &media.gfx.hud.crosshairs[2],    "gfx/hud/crosshair/c",  RFL_NOMIP, GTB_ALL },
	{ &media.gfx.hud.crosshairs[3],    "gfx/hud/crosshair/d",  RFL_NOMIP, GTB_ALL },
	{ &media.gfx.hud.crosshairs[4],    "gfx/hud/crosshair/e",  RFL_NOMIP, GTB_ALL },
	{ &media.gfx.hud.flag.blue.normal, "gfx/hud/flag/blue",    RFL_NOMIP, GTB_CTF },
	{ &media.gfx.hud.flag.blue.taken,  "gfx/hud/flag/blue_x",  RFL_NOMIP, GTB_CTF|GTB_CTY },
	{ &media.gfx.hud.flag.blue.ysal,   "gfx/hud/flag/blue_ys", RFL_NOMIP, GTB_CTY },
	{ &media.gfx.hud.flag.red.normal,  "gfx/hud/flag/red",     RFL_NOMIP, GTB_CTF },
	{ &media.gfx.hud.flag.red.taken,   "gfx/hud/flag/red_x",   RFL_NOMIP, GTB_CTF|GTB_CTY },
	{ &media.gfx.hud.flag.red.ysal,    "gfx/hud/flag/red_ys",  RFL_NOMIP, GTB_CTY },
	{ &media.gfx.interface.cursor,     "gfx/interface/cursor", RFL_NOMIP, GTB_ALL },
};

static const resource_t models[] = {
	{ &media.models.null, "models/null.md3", RFL_NONE, GTB_ALL }
};

static const resource_t sounds[] = {
	{ &media.sounds.null, "sounds/null.wav", RFL_NONE, GTB_ALL }
};

static const size_t	numEffects	= ARRAY_LEN( efx );
static const size_t	numGraphics	= ARRAY_LEN( gfx );
static const size_t	numModels	= ARRAY_LEN( models );
static const size_t	numSounds	= ARRAY_LEN( sounds );

static void CG_UpdateLoadBar( void ) {
	const size_t numResources = numSounds + numGraphics + numEffects + numModels;
	const float mapPercent = 0.25f; // let's say maps take 25% of load times

	progress.numLoadedResources++;

	// factor in map load time
	cg.loadFrac = progress.loadedMap ? mapPercent : 0.0f;
	cg.loadFrac += (((float)progress.numLoadedResources / (float)numResources) * (1.0f - mapPercent));

	trap->UpdateScreen();
}

static void CG_LoadResource( const resource_t *resource, qhandle_t( *registerFunc )(const char *location) ) {
	qhandle_t handle;

#ifdef _DEBUG
	//CG_LogPrintf( cg.log.debug, "Loading resource: %s\n", resource->location );
#endif
	CG_UpdateLoadBar();

	if ( !(resource->gametypes & (1 << cgs.gametype)) ) {
		Com_Printf( S_COLOR_GREY "[G] Skipping resource: %s\n", resource->location );
		return;
	}

	handle = registerFunc( resource->location );

	if ( !handle ) {
		Com_Printf( S_COLOR_YELLOW "[ ] Missing resource: %s\n", resource->location );
	}
	else {
		Com_Printf( S_COLOR_GREEN "[X] Loaded resource: %s\n", resource->location );
	}
	if ( resource->handle ) {
		*(qhandle_t *)resource->handle = handle;
	}
}


// the server says this item is used on this level
static void CG_RegisterItemSounds( int itemNum ) {
	const gitem_t *item = &bg_itemlist[itemNum];
	char data[MAX_QPATH];
	const char *s, *start;
	int len;

	if ( item->pickup_sound ) {
		trap->S_RegisterSound( item->pickup_sound );
	}

	// parse the space seperated precache string for other media
	s = item->sounds;
	if ( !s || !s[0] ) {
		return;
	}

	while ( *s ) {
		start = s;
		while ( *s && *s != ' ' ) {
			s++;
		}

		len = s - start;
		if ( len >= MAX_QPATH || len < 5 ) {
			trap->Error( ERR_DROP, "PrecacheItem: %s has bad precache string", item->classname );
			return;
		}
		memcpy( data, start, len );
		data[len] = '\0';
		if ( *s ) {
			s++;
		}

		trap->S_RegisterSound( data );
	}

	// parse the space seperated precache string for other media
	s = item->precaches;
	if ( !s || !s[0] )
		return;

	while ( *s ) {
		start = s;
		while ( *s && *s != ' ' ) {
			s++;
		}

		len = s - start;
		if ( len >= MAX_QPATH || len < 5 ) {
			trap->Error( ERR_DROP, "PrecacheItem: %s has bad precache string", item->classname );
			return;
		}
		memcpy( data, start, len );
		data[len] = '\0';
		if ( *s )
			s++;

		if ( !strcmp( data + len - 3, "efx" ) )
			trap->FX_RegisterEffect( data );
	}
}

static void CG_RegisterSounds( void ) {
	const resource_t *resource = nullptr;
	size_t i;
	const char *soundName = nullptr;

	for ( i = 0, resource = sounds; i < numSounds; i++, resource++ ) {
		CG_LoadResource( resource, trap->S_RegisterSound );
	}

	for ( i = 1; i < MAX_SOUNDS; i++ ) {
		soundName = CG_ConfigString( CS_SOUNDS + i );

		if ( !soundName[0] ) {
			break;
		}

		// custom sounds
		if ( soundName[0] == '*' ) {
			/*
			if ( soundName[1] == '$' ) {
				CG_PrecacheNPCSounds( soundName );
			}
			*/
			continue;
		}
		cgs.gameSounds[i] = trap->S_RegisterSound( soundName );
	}

	// load the ambient sets
	trap->AS_AddPrecacheEntry( "#clear" );

	for ( i = 1; i < MAX_AMBIENT_SETS; i++ ) {
		soundName = CG_ConfigString( CS_AMBIENT_SET + i );
		if ( !soundName || !soundName[0] )
			break;

		trap->AS_AddPrecacheEntry( soundName );
	}
	soundName = CG_ConfigString( CS_GLOBAL_AMBIENT_SET );
	if ( soundName && soundName[0] && Q_stricmp( soundName, "default" ) )
		trap->AS_AddPrecacheEntry( soundName );

	trap->AS_ParseSets();
}

static void CG_RegisterEffects( void ) {
	const resource_t *resource = nullptr;
	size_t i;
	const char *effectName = nullptr;

	for ( i = 0, resource = efx; i < numEffects; i++, resource++ ) {
		CG_LoadResource( resource, trap->FX_RegisterEffect );
	}

	for ( i = 1; i < MAX_FX; i++ ) {
		effectName = CG_ConfigString( CS_EFFECTS + i );

		if ( !effectName[0] ) {
			break;
		}

		if ( effectName[0] == '*' ) {
			// it's a special global weather effect
			trap->R_WorldEffectCommand( effectName+1 );
			cgs.gameEffects[i] = 0;
		}
		else {
			cgs.gameEffects[i] = trap->FX_RegisterEffect( effectName );
		}
	}

	CG_InitGlass();
}

static void CG_RegisterGraphics( void ) {
	const resource_t *resource = nullptr;
	size_t i;

	for ( i = 0, resource = gfx; i < numGraphics; i++, resource++ ) {
		if ( resource->flags & RFL_NOMIP ) {
			CG_LoadResource( resource, trap->R_RegisterShaderNoMip );
		}
		else {
			CG_LoadResource( resource, trap->R_RegisterShader );
		}
	}

	for ( i = 1; i < MAX_ICONS; i++ ) {
		const char *iconName = CG_ConfigString( CS_ICONS + i );

		if ( !iconName[0] ) {
			break;
		}

		cgs.gameIcons[i] = trap->R_RegisterShaderNoMip( iconName );
	}
}

static void CG_RegisterModels( void ) {
	const resource_t *resource = nullptr;
	size_t i;
	const char *terrainInfo = nullptr;
	int breakPoint;

	for ( i = 0, resource = models; i < numModels; i++, resource++ ) {
		CG_LoadResource( resource, trap->R_RegisterModel );
	}

	CG_LoadingString( "Inline models" );
	breakPoint = cgs.numInlineModels = (size_t)trap->CM_NumInlineModels();
	for ( i = 1; i < cgs.numInlineModels; i++ ) {
		char name[10];
		vec3_t mins, maxs;
		int j;

		Com_sprintf( name, sizeof(name), "*%i", i );
		cgs.inlineDrawModel[i] = trap->R_RegisterModel( name );
		if ( !cgs.inlineDrawModel[i] ) {
			breakPoint = i;
			break;
		}

		trap->R_ModelBounds( cgs.inlineDrawModel[i], mins, maxs );
		for ( j = 0; j < 3; j++ ) {
			cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5f * (maxs[j] - mins[j]);
		}
	}

	CG_LoadingString( "Server models" );
	for ( i = 1; i < MAX_MODELS; i++ ) {
		const char *cModelName;
		char modelName[MAX_QPATH];

		cModelName = CG_ConfigString( CS_MODELS + i );
		if ( !cModelName[0] )
			break;

		Q_strncpyz( modelName, cModelName, sizeof(modelName) );

		// Check to see if it has a custom skin attached.
		if ( strstr( modelName, ".glm" ) || modelName[0] == '$' ) {
			CG_HandleAppendedSkin( modelName );
			CG_CacheG2AnimInfo( modelName );
		}

		// don't register vehicle names and saber names as models.
		if ( modelName[0] != '$' && modelName[0] != '@' )
			cgs.gameModels[i] = trap->R_RegisterModel( modelName );
		else
			cgs.gameModels[i] = 0; //FIXME: register here so that stuff gets precached!!!
	}

	size_t numBSPInstances = 0;
	for ( i = 1; i < MAX_SUB_BSP; i++ ) {
		const char *bspName = CG_ConfigString( CS_BSP_MODELS + i );
		if ( bspName[0] ) {
			numBSPInstances++;
		}
	}
	for ( i = 1; i < numBSPInstances + 1; i++ ) {
		const char *bspName = CG_ConfigString( CS_BSP_MODELS + i );
		if ( !bspName[0] ) {
			break;
		}

		char loadingStr[1024];
		Com_sprintf( loadingStr, sizeof(loadingStr), "Loading BSP instance %i/%i: %s",
			i, numBSPInstances, bspName+1
		);
		CG_LoadingString( loadingStr );

		trap->CM_LoadMap( bspName, true );
		cgs.inlineDrawModel[breakPoint] = trap->R_RegisterModel( bspName );
		vec3_t mins, maxs;
		trap->R_ModelBounds( cgs.inlineDrawModel[breakPoint], mins, maxs );
		for ( int j = 0; j < 3; j++ ) {
			cgs.inlineModelMidpoints[breakPoint][j] = mins[j] + 0.5f * (maxs[j] - mins[j]);
		}
		breakPoint++;

		for ( int sub = 1; sub < MAX_MODELS; sub++ ) {
			char temp[MAX_QPATH];
			Com_sprintf( temp, MAX_QPATH, "*%d-%d", i, sub );
			cgs.inlineDrawModel[breakPoint] = trap->R_RegisterModel( temp );
			if ( !cgs.inlineDrawModel[breakPoint] ) {
				break;
			}

			trap->R_ModelBounds( cgs.inlineDrawModel[breakPoint], mins, maxs );
			for ( int j = 0; j < 3; j++ ) {
				cgs.inlineModelMidpoints[breakPoint][j] = mins[j] + 0.5f * (maxs[j] - mins[j]);
			}
			breakPoint++;
		}
	}

	CG_LoadingString( "Creating terrain" );
	for ( i = 0; i < MAX_TERRAINS; i++ ) {
		terrainInfo = CG_ConfigString( CS_TERRAINS + i );
		if ( !terrainInfo[0] )
			break;

		trap->RMG_Init( trap->CM_RegisterTerrain( terrainInfo ), terrainInfo );

		// Send off the terrainInfo to the renderer
		trap->RE_InitRendererTerrain( terrainInfo );
	}
}

static void CG_RegisterItems( void ) {
	char items[MAX_ITEMS + 1];
	size_t i;

	memset( cg_items, 0, sizeof(cg_items) );
	memset( cg_weapons, 0, sizeof(cg_weapons) );

	Q_strncpyz( items, CG_ConfigString( CS_ITEMS ), sizeof(items) );

	for ( i = 1; i < bg_numItems; i++ ) {
		if ( items[i] == '1' ) {
			CG_LoadingItem( i );
			CG_RegisterItemVisuals( i );
			CG_RegisterItemSounds( i );
		}
	}

	CG_InitG2Weapons();
}

void CG_LoadMedia( void ) {
	// sounds
	CG_LoadingString( "Sounds" );
	CG_RegisterSounds();

	// graphics
	CG_LoadingString( "Graphics" );
	CG_RegisterGraphics();

	memset( &cg.refdef, 0, sizeof(cg.refdef) );
	trap->R_ClearScene();
	CG_LoadingString( va( "Collision map (%s)", cgs.mapname ) );
	trap->CM_LoadMap( cgs.mapname, false );
	CG_LoadingString( va( "Map (%s)", cgs.mapname ) );
	trap->R_LoadWorld( cgs.mapname );
	progress.loadedMap = true;
	CG_LoadingString( "Automap" );
	trap->R_InitializeWireframeAutomap();

	// effects
	CG_LoadingString( "Effects subsystem" );
	trap->FX_InitSystem( &cg.refdef );
	CG_LoadingString( "Effects" );
	CG_RegisterEffects();

	// models
	CG_LoadingString( "Models" );
	CG_RegisterModels();

	// items
	CG_LoadingString( "Items" );
	CG_RegisterItems();
}

void CG_PreloadMedia( void ) {
	//FIXME: these are registered in UI first, we can just grab the handle.
	cgDC.Assets.qhSmallFont  = trap->R_RegisterFont( "ocr_a" );
	cgDC.Assets.qhMediumFont = trap->R_RegisterFont( "ergoec" );
	cgDC.Assets.qhBigFont =    trap->R_RegisterFont( "arialnb" );
	/*
	cgDC.Assets.qhSmallFont = CG_Cvar_Get( "font_small" );
	cgDC.Assets.qhMediumFont = CG_Cvar_Get( "font_medium" );
	cgDC.Assets.qhBigFont = CG_Cvar_Get( "font_big" );
	*/

	media.gfx.misc.white					= trap->R_RegisterShader( "white" );
	media.gfx.interface.charset				= trap->R_RegisterShaderNoMip( "charset" );
	media.gfx.interface.loading.tick		= trap->R_RegisterShaderNoMip( "gfx/interface/loading/tick" );
	media.gfx.interface.loading.cap			= trap->R_RegisterShaderNoMip( "gfx/interface/loading/cap" );
	media.gfx.interface.loading.background	= trap->R_RegisterShaderNoMip( "gfx/interface/loading/background" );
}
