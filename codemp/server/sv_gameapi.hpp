/*
===========================================================================
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
// FUNCTION
// ======================================================================

bool GVM_ClientUserinfoChanged(int clientNum);
bool GVM_ConsoleCommand(void);
bool GVM_ICARUS_Set(void);
bool GVM_NAV_CheckNodeFailedForEnt(int entID, int nodeNum);
bool GVM_NAV_ClearPathToPoint(int entID, vec3_t pmins, vec3_t pmaxs, vec3_t point, int clipmask, int okToHitEnt);
bool GVM_NAV_EntIsBreakable(int entityNum);
bool GVM_NAV_EntIsDoor(int entityNum);
bool GVM_NAV_EntIsRemovableUsable(int entNum);
bool GVM_NAV_EntIsUnlockedDoor(int entityNum);
bool GVM_NPC_ClearLOS2(int entID, const vec3_t end);
char* GVM_ClientConnect(int clientNum, bool firstTime, bool isBot);
int GVM_BG_GetItemIndexByTag(int tag, int type);
int GVM_BotAIStartFrame(int time);
int GVM_ICARUS_GetFloat(void);
int GVM_ICARUS_GetSetIDForString(void);
int GVM_ICARUS_GetString(void);
int GVM_ICARUS_GetTag(void);
int GVM_ICARUS_GetVector(void);
int GVM_ICARUS_PlaySound(void);
int GVM_NAVNEW_ClearPathBetweenPoints(vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int ignore, int clipmask);
void GVM_ClientBegin(int clientNum);
void GVM_ClientCommand(int clientNum);
void GVM_ClientDisconnect(int clientNum);
void GVM_ClientThink(int clientNum, usercmd_t* ucmd);
void GVM_ICARUS_Kill(void);
void GVM_ICARUS_Lerp2Angles(void);
void GVM_ICARUS_Lerp2End(void);
void GVM_ICARUS_Lerp2Origin(void);
void GVM_ICARUS_Lerp2Pos(void);
void GVM_ICARUS_Lerp2Start(void);
void GVM_ICARUS_Play(void);
void GVM_ICARUS_Remove(void);
void GVM_ICARUS_SoundIndex(void);
void GVM_ICARUS_Use(void);
void GVM_InitGame(int levelTime, int randomSeed, int restart);
void GVM_NAV_FindCombatPointWaypoints(void);
void GVM_ROFF_NotetrackCallback(int entID, const char* notetrack);
void GVM_RunFrame(int levelTime);
void GVM_ShutdownGame(int restart);
void GVM_SpawnRMGEntity(void);

void SV_BindGame(void);
void SV_InitGame(bool restart);
void SV_RestartGame(void);
void SV_UnbindGame(void);
