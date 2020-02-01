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
// INCLUDE
// ======================================================================

#include "cgame/cg_public.h"
#include "client/cl_public.h"
#include "qcommon/q_shared.h"
#include "sys/sys_public.h"

// ======================================================================
// DEFINE
// ======================================================================

// time between connection packet retransmits
#define	RETRANSMIT_TIMEOUT	3000

// file full of random crap that gets used to create ja_guid
#define QKEY_FILE "jakey"
#define QKEY_SIZE 2048

#define	CON_TEXTSIZE	0x30000 //was 32768
#define	NUM_CON_TIMES	4

// ======================================================================
// STRUCT
// ======================================================================

typedef struct ping_s {
	netadr_t	adr;
	int			start;
	int			time;
	char		info[MAX_INFO_STRING];
} ping_t;

typedef struct console_s {
	bool	initialized;

	short	text[CON_TEXTSIZE];
	int		current;		// line where next message will be printed
	int		x;				// offset in current line for next print
	int		display;		// bottom of console displays this line

	int 	linewidth;		// characters across screen
	int		totallines;		// total lines in console scrollback

	float	xadjust;		// for wide aspect screens
	float	yadjust;		// for wide aspect screens

	float	displayFrac;	// aproaches finalFrac at scr_conspeed
	float	finalFrac;		// 0.0 to 1.0 lines of console to display

	int		vislines;		// in scanlines

	int		times[NUM_CON_TIMES];	// cls.realtime time the line was generated
								// for transparent notify lines
	vec4_t	color;
} console_t;

typedef struct kbutton_s {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	bool	active;			// current state
	bool	wasPressed;		// set when down, not cleared when up
} kbutton_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern int cl_connectedToCheatServer;
extern int cl_connectedToPureServer;
extern int g_console_field_width;
extern vec3_t cl_windVec;

// ======================================================================
// FUNCTION
// ======================================================================

status_e CIN_RunCinematic(int handle);
status_e CIN_StopCinematic(int handle);
float CL_KeyState(kbutton_t* key);
int CIN_PlayCinematic(const char* arg0, int xpos, int ypos, int width, int height, int bits);
int CL_GetPingQueueCount(void);
int CL_ServerStatus(const char* serverAddress, char* serverStatusString, int maxLen);
int SCR_GetBigStringWidth(const char* str);
bool CL_CheckPaused(void);
bool CL_CloseAVI(void);
bool CL_GameCommand(void);
bool CL_GetDefaultState(int index, entityState_t* state);
bool CL_GetServerCommand(int serverCommandNumber);
bool CL_GetSnapshot(int snapshotNumber, snapshot_t* snapshot);
bool CL_GetUserCmd(int cmdNumber, usercmd_t* ucmd);
bool CL_Netchan_Process(netchan_t* chan, msg_t* msg);
bool CL_OpenAVIForWriting(const char* filename);
bool CL_UpdateVisiblePings_f(int source);
void CIN_DrawCinematic(int handle);
void CIN_SetExtents(int handle, int x, int y, int w, int h);
void CIN_SetLooping(int handle, bool loop);
void CIN_UploadCinematic(int handle);
void CL_AddReliableCommand(const char* cmd, bool isDisconnectCmd);
void CL_CGameRendering(stereoFrame_e stereo);
void CL_ClearPing(int n);
void CL_ClearState(void);
void CL_Disconnect_f(void);
void CL_FirstSnapshot(void);
void CL_FlushMemory(void);
void CL_GetPing(int n, char* buf, int buflen, int* pingtime);
void CL_GetPingInfo(int n, char* buf, int buflen);
void CL_GlobalServers_f(void);
void CL_Init(void);
void CL_InitCGame(void);
void CL_InitDownloads(void);
void CL_InitInput(void);
void CL_InitRef(void);
void CL_InitUI(void);
void CL_LocalServers_f(void);
void CL_Netchan_Transmit(netchan_t* chan, msg_t* msg);	//int length, const byte *data );
void CL_Netchan_TransmitNextFragment(netchan_t* chan);
void CL_NextDemo(void);
void CL_NextDownload(void);
void CL_ParseServerMessage(msg_t* msg);
void CL_Ping_f(void);
void CL_PlayCinematic_f(void);
void CL_ReadDemoMessage(void);
void CL_SendCmd(void);
void CL_ServerInfoPacket(netadr_t from, msg_t* msg);
void CL_SetCGameTime(void);
void CL_SetUserCmdValue(int userCmdValue, float sensitivityScale, float mPitchOverride, float mYawOverride, float mSensitivityOverride, int fpSel, int invenSel);
void CL_ShutdownInput(void);
void CL_Snd_Restart_f(void);
void CL_StartHunkUsers(void);
void CL_SystemInfoChanged(void);
void CL_TakeVideoFrame(void);
void CL_Vid_Restart_f(void);
void CL_WriteAVIVideoFrame(const byte* imageBuffer, int size);
void CL_WritePacket(void);
void Con_Bottom(void);
void Con_CheckResize(void);
void Con_Clear_f(void);
void Con_ClearNotify(void);
void Con_Close(void);
void Con_DrawConsole(void);
void Con_DrawNotify(void);
void Con_Init(void);
void Con_PageDown(void);
void Con_PageUp(void);
void Con_RunConsole(void);
void Con_Shutdown(void);
void Con_ToggleConsole_f(void);
void Con_Top(void);
void Key_SetCatcher(int catcher);
void LAN_LoadCachedServers();
void LAN_SaveServersToCache();
void SCR_DebugGraph(float value, int color);
void SCR_DrawBigString(int x, int y, const char* s, float alpha, bool noColorEscape);
void SCR_DrawCinematic(void);
void SCR_DrawNamedPic(float x, float y, float width, float height, const char* picname);
void SCR_DrawPic(float x, float y, float width, float height, qhandle_t hShader);
void SCR_DrawSmallChar(int x, int y, int ch);
void SCR_DrawSmallStringExt(int x, int y, const char* string, float* setColor, bool forceColor, bool noColorEscape);
void SCR_FillRect(float x, float y, float width, float height, const float* color);
void SCR_Init(void);
void SCR_RunCinematic(void);
void SCR_StopCinematic(void);
void SCR_UpdateScreen(void);
