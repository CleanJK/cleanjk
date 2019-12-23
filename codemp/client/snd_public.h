/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors

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

#include "qcommon/qcommon.h"

#define	WAV_FORMAT_PCM		1
#define WAV_FORMAT_ADPCM	2	// not actually implemented, but is the value that you get in a header
#define WAV_FORMAT_MP3		3	// not actually used this way, but just ensures we don't match one of the legit formats

typedef struct dma_s {
	int			channels;
	int			samples;				// mono samples in buffer
	int			submission_chunk;		// don't mix less than this #
	int			samplebits;
	int			speed;
	byte* buffer;
} dma_t;

extern	dma_t	dma;
extern	int		s_rawend;
extern qboolean s_shutUp;

qboolean SND_RegisterAudio_LevelLoadEnd(qboolean bDeleteEverythingNotUsedThisLevel /* 99% qfalse */);
sfxHandle_t	S_RegisterSound(const char* sample);
void S_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);

#ifdef USE_OPENAL
void S_AL_MuteAllSounds(qboolean bMute);
#endif // USE_OPENAL

void S_BeginRegistration(void);
void S_ClearLoopingSounds(void);
void S_DisableSounds(void);
void S_Init(void);
void S_MuteSound(int entityNum, int entchannel);
void S_RawSamples(int samples, int rate, int width, int channels, const byte* data, float volume, int bFirstOrOnlyUpdateThisFrame);
void S_Respatialize(int entityNum, const vec3_t head, matrix3_t axis, int inwater);
void S_RestartMusic(void);
void S_Shutdown(void);
void S_StartBackgroundTrack(const char* intro, const char* loop, qboolean bCalledByCGameStart);
void S_StartLocalSound(sfxHandle_t sfx, int channelNum);
void S_StartSound(const vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx);
void S_StopAllSounds(void);
void S_StopBackgroundTrack(void);
void S_StopLoopingSound(int entityNum);
void S_Update(void);
void S_UpdateEntityPosition(int entityNum, const vec3_t origin);
