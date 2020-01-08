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

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

#ifdef _DEBUG
extern cvar_t *r_noPrecacheGLA;
#endif

extern cvar_t* broadsword;
extern cvar_t* broadsword_dircap;
extern cvar_t* broadsword_dontstopanim;
extern cvar_t* broadsword_effcorr;
extern cvar_t* broadsword_extra1;
extern cvar_t* broadsword_extra2;
extern cvar_t* broadsword_kickbones;
extern cvar_t* broadsword_kickorigin;
extern cvar_t* broadsword_playflop;
extern cvar_t* broadsword_ragtobase;
extern cvar_t* broadsword_smallbbox;
extern cvar_t* broadsword_waitforshot;
extern cvar_t* cg_g2MarksAllModels;
extern cvar_t* cg_g2MarksAllModels;
extern cvar_t* cg_g2MarksAllModels;
extern cvar_t* cg_shadows;
extern cvar_t* r_allowExtensions;
extern cvar_t* r_ambientScale;
extern cvar_t* r_autolodscalevalue;
extern cvar_t* r_autoMap;
extern cvar_t* r_autoMapBackAlpha;
extern cvar_t* r_autoMapDisable;
extern cvar_t* r_centerWindow;
extern cvar_t* r_clear;
extern cvar_t* r_colorbits;
extern cvar_t* r_colorMipLevels;
extern cvar_t* r_cullRoofFaces;
extern cvar_t* r_customheight;
extern cvar_t* r_customwidth;
extern cvar_t* r_debugLight;
extern cvar_t* r_debugSort;
extern cvar_t* r_depthbits;
extern cvar_t* r_detailTextures;
extern cvar_t* r_directedScale;
extern cvar_t* r_dlightStyle;
extern cvar_t* r_drawentities;
extern cvar_t* r_drawfog;
extern cvar_t* r_drawSun;
extern cvar_t* r_drawworld;
extern cvar_t* r_DynamicGlow;
extern cvar_t* r_DynamicGlowDelta;
extern cvar_t* r_DynamicGlowHeight;
extern cvar_t* r_DynamicGlowIntensity;
extern cvar_t* r_DynamicGlowPasses;
extern cvar_t* r_DynamicGlowSoft;
extern cvar_t* r_DynamicGlowWidth;
extern cvar_t* r_dynamiclight;
extern cvar_t* r_ext_compiled_vertex_array;
extern cvar_t* r_ext_compress_lightmaps;
extern cvar_t* r_ext_compress_textures;
extern cvar_t* r_ext_gamma_control;
extern cvar_t* r_ext_multitexture;
extern cvar_t* r_ext_preferred_tc_method;
extern cvar_t* r_ext_texture_env_add;
extern cvar_t* r_ext_texture_filter_anisotropic;
extern cvar_t* r_facePlaneCull;
extern cvar_t* r_fastsky;
extern cvar_t* r_finish;
extern cvar_t* r_flares;
extern cvar_t* r_fullbright;
extern cvar_t* r_gamma;
extern cvar_t* r_Ghoul2AnimSmooth;
extern cvar_t* r_Ghoul2UnSqashAfterSmooth;
extern cvar_t* r_ignore;
extern cvar_t* r_ignoreGLErrors;
extern cvar_t* r_ignorehwgamma;
extern cvar_t* r_intensity;
extern cvar_t* r_lightmap;
extern cvar_t* r_lockpvs;
extern cvar_t* r_lodbias;
extern cvar_t* r_lodCurveError;
extern cvar_t* r_lodscale;
extern cvar_t* r_logFile;
extern cvar_t* r_markcount;
extern cvar_t* r_maxpolys;
extern cvar_t* r_maxpolyverts;
extern cvar_t* r_measureOverdraw;
extern cvar_t* r_mode;
extern cvar_t* r_modelpoolmegs;
extern cvar_t* r_nobind;
extern cvar_t* r_noborder;
extern cvar_t* r_nocull;
extern cvar_t* r_nocurves;
extern cvar_t* r_noportals;
extern cvar_t* r_norefresh;
extern cvar_t* r_noServerGhoul2;
extern cvar_t* r_novis;
extern cvar_t* r_offsetFactor;
extern cvar_t* r_offsetUnits;
extern cvar_t* r_overBrightBits;
extern cvar_t* r_picmip;
extern cvar_t* r_portalOnly;
extern cvar_t* r_primitives;
extern cvar_t* r_roofCeilFloorDist;
extern cvar_t* r_roofCullCeilDist;
extern cvar_t* r_shadowRange;
extern cvar_t* r_showcluster;
extern cvar_t* r_showImages;
extern cvar_t* r_shownormals;
extern cvar_t* r_showsky;
extern cvar_t* r_showtris;
extern cvar_t* r_simpleMipMaps;
extern cvar_t* r_singleShader;
extern cvar_t* r_skipBackEnd;
extern cvar_t* r_speeds;
extern cvar_t* r_stencilbits;
extern cvar_t* r_stereo;
extern cvar_t* r_subdivisions;
extern cvar_t* r_surfaceSprites;
extern cvar_t* r_surfaceWeather;
extern cvar_t* r_swapInterval;
extern cvar_t* r_texturebits;
extern cvar_t* r_texturebitslm;
extern cvar_t* r_textureMode;
extern cvar_t* r_uiFullScreen;
extern cvar_t* r_verbose;
extern cvar_t* r_vertexLight;
extern cvar_t* r_windAngle;
extern cvar_t* r_windDampFactor;
extern cvar_t* r_windGust;
extern cvar_t* r_windPointForce;
extern cvar_t* r_windPointX;
extern cvar_t* r_windPointY;
extern cvar_t* r_windSpeed;
extern cvar_t* r_znear;

// ======================================================================
// FUNCTION
// ======================================================================

void R_InitCvars(void);
