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

#define UI_API_VERSION 4
#define UI_LEGACY_API_VERSION 7

struct uiClientState_t {
	connstate_e connState;
	int         connectPacketCount;
	int         clientNum;
	char        servername[MAX_STRING_CHARS];
	char        updateInfoString[MAX_STRING_CHARS];
	char        messageString[MAX_STRING_CHARS];
};

struct uiImport_t {
	void              (*Print)                            ( const char *msg, ... );
	NORETURN_PTR void (*Error)                            ( int level, const char *fmt, ... );
	int               (*Milliseconds)                     ( void );
	int               (*RealTime)                         ( qtime_t *qtime );
	int               (*MemoryRemaining)                  ( void );
	void              (*Cvar_Create)                      ( const char *var_name, const char *var_value, uint32_t flags );
	void              (*Cvar_InfoStringBuffer)            ( int bit, char *buffer, int bufsize );
	void              (*Cvar_Register)                    ( vmCvar_t *cvar, const char *var_name, const char *value, uint32_t flags );
	void              (*Cvar_Reset)                       ( const char *name );
	void              (*Cvar_Set)                         ( const char *var_name, const char *value );
	void              (*Cvar_SetValue)                    ( const char *var_name, float value );
	void              (*Cvar_Update)                      ( vmCvar_t *cvar );
	int               (*Cvar_VariableIntegerValue)        ( const char* var_name );
	void              (*Cvar_VariableStringBuffer)        ( const char *var_name, char *buffer, int bufsize );
	float             (*Cvar_VariableValue)               ( const char *var_name );
	int               (*Cmd_Argc)                         ( void );
	void              (*Cmd_Argv)                         ( int n, char *buffer, int bufferLength );
	void              (*Cmd_ExecuteText)                  ( int exec_when, const char *text );
	void              (*FS_Close)                         ( fileHandle_t f );
	int               (*FS_GetFileList)                   ( const char *path, const char *extension, char *listbuf, int bufsize );
	int               (*FS_Open)                          ( const char *qpath, fileHandle_t *f, fsMode_e mode );
	int               (*FS_Read)                          ( void *buffer, int len, fileHandle_t f );
	int               (*FS_Write)                         ( const void *buffer, int len, fileHandle_t f );
	void              (*GetClientState)                   ( uiClientState_t *state );
	void              (*GetClipboardData)                 ( char *buf, int bufsize );
	int               (*GetConfigString)                  ( int index, char *buff, int buffsize );
	void              (*GetGlconfig)                      ( glconfig_t *glconfig );
	void              (*UpdateScreen)                     ( void );
	void              (*Key_ClearStates)                  ( void );
	void              (*Key_GetBindingBuf)                ( int keynum, char *buf, int buflen );
	bool              (*Key_IsDown)                       ( int keynum );
	void              (*Key_KeynumToStringBuf)            ( int keynum, char *buf, int buflen );
	void              (*Key_SetBinding)                   ( int keynum, const char *binding );
	int               (*Key_GetCatcher)                   ( void );
	bool              (*Key_GetOverstrikeMode)            ( void );
	void              (*Key_SetCatcher)                   ( int catcher );
	void              (*Key_SetOverstrikeMode)            ( bool state );
	int               (*PC_AddGlobalDefine)               ( char *define );
	void              (*PC_Init)                          ( void );
	int               (*PC_FreeSource)                    ( int handle );
	int               (*PC_LoadGlobalDefines)             ( const char* filename );
	int               (*PC_LoadSource)                    ( const char *filename );
	int               (*PC_ReadToken)                     ( int handle, pc_token_t *pc_token );
	void              (*PC_RemoveAllGlobalDefines)        ( void );
	int               (*PC_SourceFileAndLine)             ( int handle, char *filename, int *line );
	void              (*CIN_DrawCinematic)                ( int handle );
	int               (*CIN_PlayCinematic)                ( const char *arg0, int xpos, int ypos, int width, int height, int bits );
	status_e          (*CIN_RunCinematic)                 ( int handle );
	void              (*CIN_SetExtents)                   ( int handle, int x, int y, int w, int h );
	status_e          (*CIN_StopCinematic)                ( int handle );
	int               (*LAN_AddServer)                    ( int source, const char *name, const char *addr );
	void              (*LAN_ClearPing)                    ( int n );
	int               (*LAN_CompareServers)               ( int source, int sortKey, int sortDir, int s1, int s2 );
	void              (*LAN_GetPing)                      ( int n, char *buf, int buflen, int *pingtime );
	void              (*LAN_GetPingInfo)                  ( int n, char *buf, int buflen );
	int               (*LAN_GetPingQueueCount)            ( void );
	void              (*LAN_GetServerAddressString)       ( int source, int n, char *buf, int buflen );
	int               (*LAN_GetServerCount)               ( int source );
	void              (*LAN_GetServerInfo)                ( int source, int n, char *buf, int buflen );
	int               (*LAN_GetServerPing)                ( int source, int n );
	void              (*LAN_LoadCachedServers)            ( void );
	void              (*LAN_MarkServerVisible)            ( int source, int n, bool visible );
	void              (*LAN_RemoveServer)                 ( int source, const char *addr );
	void              (*LAN_ResetPings)                   ( int n );
	void              (*LAN_SaveCachedServers)            ( void );
	int               (*LAN_ServerIsVisible)              ( int source, int n );
	int               (*LAN_ServerStatus)                 ( const char *serverAddress, char *serverStatus, int maxLen );
	bool              (*LAN_UpdateVisiblePings)           ( int source );
	void              (*S_StartBackgroundTrack)           ( const char *intro, const char *loop, bool bReturnWithoutStarting );
	void              (*S_StartLocalSound)                ( sfxHandle_t sfx, int channelNum );
	void              (*S_StopBackgroundTrack)            ( void );
	sfxHandle_t       (*S_RegisterSound)                  ( const char *sample );
	void              (*SE_GetLanguageName)               ( const int languageIndex, char *buffer );
	int               (*SE_GetNumLanguages)               ( void );
	bool              (*SE_GetStringTextString)           ( const char *text, char *buffer, int bufferLength );
	bool              (*R_Language_IsAsian)               ( void );
	bool              (*R_Language_UsesSpaces)            ( void );
	unsigned int      (*R_AnyLanguage_ReadCharFromString) ( const char *psText, int *piAdvanceCount, bool *pbIsTrailingPunctuation );
	void              (*R_AddLightToScene)                ( const vec3_t org, float intensity, float r, float g, float b );
	void              (*R_AddPolysToScene)                ( qhandle_t hShader, int numVerts, const polyVert_t *verts, int num );
	void              (*R_AddRefEntityToScene)            ( const refEntity_t *re );
	void              (*R_ClearScene)                     ( void );
	void              (*R_DrawStretchPic)                 ( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader );
	int               (*R_LerpTag)                        ( orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char *tagName );
	void              (*R_ModelBounds)                    ( clipHandle_t model, vec3_t mins, vec3_t maxs );
	qhandle_t         (*R_RegisterModel)                  ( const char *name );
	qhandle_t         (*R_RegisterSkin)                   ( const char *name );
	qhandle_t         (*R_RegisterShaderNoMip)            ( const char *name );
	void              (*R_RemapShader)                    ( const char *oldShader, const char *newShader, const char *timeOffset );
	void              (*R_RenderScene)                    ( const refdef_t *fd );
	void              (*R_SetColor)                       ( const float *rgba );
	void              (*R_ShaderNameFromIndex)            ( char *name, int index );
	void              (*G2_ListModelSurfaces)             ( void *ghlInfo );
	void              (*G2_ListModelBones)                ( void *ghlInfo, int frame );
	void              (*G2_SetGhoul2ModelIndexes)         ( void *ghoul2, qhandle_t *modelList, qhandle_t *skinList );
	bool              (*G2_HaveWeGhoul2Models)            ( void *ghoul2 );
	int               (*G2API_AddBolt)                    ( void *ghoul2, int modelIndex, const char *boneName );
	void              (*G2API_AnimateG2Models)            ( void *ghoul2, int time, sharedRagDollUpdateParams_t *params );
	bool              (*G2API_AttachG2Model)              ( void *ghoul2From, int modelIndexFrom, void *ghoul2To, int toBoltIndex, int toModel );
	void              (*G2API_CleanGhoul2Models)          ( void **ghoul2Ptr );
	void              (*G2API_CollisionDetect)            ( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius );
	void              (*G2API_CollisionDetectCache)       ( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius );
	int               (*G2API_CopyGhoul2Instance)         ( void *g2From, void *g2To, int modelIndex );
	void              (*G2API_CopySpecificGhoul2Model)    ( void *g2From, int modelFrom, void *g2To, int modelTo );
	void              (*G2API_DuplicateGhoul2Instance)    ( void *g2From, void **g2To );
	bool              (*G2API_GetBoltMatrix_NoRecNoRot)   ( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale );
	bool              (*G2API_GetBoltMatrix_NoReconstruct)( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale );
	bool              (*G2API_GetBoltMatrix)              ( void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale );
	bool              (*G2API_GetBoneAnim)                ( void *ghoul2, const char *boneName, const int currentTime, float *currentFrame, int *startFrame, int *endFrame, int *flags, float *animSpeed, int *modelList, const int modelIndex );
	bool              (*G2API_GetBoneFrame)               ( void *ghoul2, const char *boneName, const int currentTime, float *currentFrame, int *modelList, const int modelIndex );
	void              (*G2API_GetGLAName)                 ( void *ghoul2, int modelIndex, char *fillBuf );
	void              (*G2API_GetSurfaceName)             ( void *ghoul2, int surfNumber, int modelIndex, char *fillBuf );
	int               (*G2API_GetTime)                    ( void );
	bool              (*G2API_HasGhoul2ModelOnIndex)      ( void *ghlInfo, int modelIndex );
	bool              (*G2API_IKMove)                     ( void *ghoul2, int time, sharedIKMoveParams_t *params );
	int               (*G2API_InitGhoul2Model)            ( void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin, qhandle_t customShader, int modelFlags, int lodBias );
	bool              (*G2API_RemoveGhoul2Model)          ( void *ghlInfo, int modelIndex );
	void              (*G2API_SetBoltInfo)                ( void *ghoul2, int modelIndex, int boltInfo );
	bool              (*G2API_SetBoneAngles)              ( void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags, const int up, const int right, const int forward, qhandle_t *modelList, int blendTime , int currentTime );
	bool              (*G2API_SetBoneAnim)                ( void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime );
	bool              (*G2API_SetBoneIKState)             ( void *ghoul2, int time, const char *boneName, int ikState, sharedSetBoneIKStateParams_t *params );
	bool              (*G2API_SetNewOrigin)               ( void *ghoul2, const int boltIndex );
	void              (*G2API_SetRagDoll)                 ( void *ghoul2, sharedRagDollParams_t *params );
	bool              (*G2API_SetRootSurface)             ( void *ghoul2, const int modelIndex, const char *surfaceName );
	bool              (*G2API_SetSkin)                    ( void *ghoul2, int modelIndex, qhandle_t customSkin, qhandle_t renderSkin );
	bool              (*G2API_SetSurfaceOnOff)            ( void *ghoul2, const char *surfaceName, const int flags );
	void              (*G2API_SetTime)                    ( int time, int clock );
	float             (*Text_Height)                      ( const Text &text, const char *str );
	void              (*Text_Paint)                       ( const Text &text, float x, float y, const char *str, const vec4_t colour, int style, int limit, float adjust, int cursorPos, bool overstrike );
	float             (*Text_Width)                       ( const Text &text, const char *str );
	void              (*TextHelper_Paint_Limit)           ( float *maxX, float x, float y, float scale, const vec4_t color, const char *str, float adjust, int limit, JKFont font );
	void              (*TextHelper_Paint_Proportional)    ( int x, int y, const char *str, int style, const vec4_t color, JKFont font, float scale );

	struct {
		void			(*AddCommand)							( const char *cmd_name );
		void			(*RemoveCommand)						( const char *cmd_name );
	} ext;
};

struct uiExport_t {
	void (*Init)             ( bool inGameLoad );
	void (*Shutdown)         ( void );
	void (*KeyEvent)         ( int key, bool down );
	void (*MouseEvent)       ( int dx, int dy );
	void (*Refresh)          ( int realtime );
	bool (*IsFullscreen)     ( void );
	void (*SetActiveMenu)    ( uiMenuCommand_e menu );
	bool (*ConsoleCommand)   ( int realTime );
	void (*DrawConnectScreen)( bool overlay );
	void (*MenuReset)        ( void );
};

//linking of ui library
typedef uiExport_t* (QDECL *GetUIAPI_t)( int apiVersion, uiImport_t *import );

