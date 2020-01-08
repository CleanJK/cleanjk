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

// snd_local.h -- private sound definations

#include "client/snd_public.h"
#include "mp3code/mp3struct.h"

// Open AL Specific
#ifdef USE_OPENAL
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "eax/eax.h"
#include "eax/EaxMan.h"
/*#elif defined MACOS_X
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>*/
#endif

#define	PAINTBUFFER_SIZE			1024
#define	START_SAMPLE_IMMEDIATE	0x7fffffff

#define	NUM_STREAMING_BUFFERS	4
#define	STREAMING_BUFFER_SIZE	4608	// 4 decoded MP3 frames
#define	QUEUED			1
#define	UNQUEUED		2
#define	MAX_CHANNELS				32
#define	MAX_RAW_SAMPLES			16384

// keep this enum in sync with the table "sSoundCompressionMethodStrings"	-ste
enum SoundCompressionMethod_t {
	ct_16 = 0,		// formerly ct_NONE in EF1, now indicates 16-bit samples (the default)
	ct_MP3,
	ct_NUMBEROF		// used only for array sizing
};

// !!! if this is changed, the asm code must change !!!
struct portable_samplepair_t {
	int			left;	// the final values will be clamped to +/- 0x00ffff00 and shifted down
	int			right;
};

struct sfx_t {
	short			*pSoundData;
	bool			bDefaultSound;			// couldn't be loaded, so use buzz
	bool			bInMemory;				// not in Memory, set true when loaded, and false when its buffers are freed up because of being old, so can be reloaded
	SoundCompressionMethod_t eSoundCompressionMethod;
	MP3STREAM		*pMP3StreamHeader;		// nullptr ptr unless this sfx_t is an MP3. Use Z_Malloc and Z_Free
	int 			iSoundLengthInSamples;	// length in samples, always kept as 16bit now so this is #shorts (watch for stereo later for music?)
	char 			sSoundName[MAX_QPATH];
	int				iLastTimeUsed;
	float			fVolRange;				// used to set the highest volume this sample has at load time - used for lipsynching
	int				iLastLevelUsedOn;		// used for cacheing purposes

	// Open AL
#ifdef USE_OPENAL
	ALuint		Buffer;
#endif
	char		*lipSyncData;

	sfx_t *next;					// only used because of hash table when registering
};

// Open AL specific
#ifdef USE_OPENAL
struct STREAMINGBUFFER_t {
	ALuint	BufferID;
	ALuint	Status;
	char	*Data;
} STREAMINGBUFFER;
#endif

struct channel_t {
// back-indented fields new in TA codebase, will re-format when MP3 code finished -ste
// note: field missing in TA: bool	loopSound;		// from an S_AddLoopSound call, cleared each frame
	unsigned int startSample;	// START_SAMPLE_IMMEDIATE = set immediately on next mix
	int			entnum;			// to allow overriding a specific sound
	int			entchannel;		// to allow overriding a specific sound
	int			leftvol;		// 0-255 volume after spatialization
	int			rightvol;		// 0-255 volume after spatialization
	int			master_vol;		// 0-255 volume before spatialization

	vec3_t		origin;			// only use if fixed_origin is set

	bool		fixed_origin;	// use origin instead of fetching entnum's origin
	sfx_t		*thesfx;		// sfx structure
	bool		loopSound;		// from an S_AddLoopSound call, cleared each frame
	MP3STREAM	MP3StreamHeader;
	byte		MP3SlidingDecodeBuffer[50000/*12000*/];	// typical back-request = -3072, so roughly double is 6000 (safety), then doubled again so the 6K pos is in the middle of the buffer)
	int			iMP3SlidingDecodeWritePos;
	int			iMP3SlidingDecodeWindowPos;

	bool		doppler;
	float		dopplerScale;

	// Open AL specific
	bool	bLooping;	// Signifies if this channel / source is playing a looping sound
//	bool	bAmbient;	// Signifies if this channel / source is playing a looping ambient sound
	bool	bProcessed;	// Signifies if this channel / source has been processed
	bool	bStreaming;	// Set to true if the data needs to be streamed (MP3 or dialogue)
#ifdef USE_OPENAL
	STREAMINGBUFFER	buffers[NUM_STREAMING_BUFFERS];	// AL Buffers for streaming
	ALuint		alSource;		// Open AL Source
#endif
	bool		bPlaying;		// Set to true when a sound is playing on this channel / source
	int			iStartTime;		// Time playback of Source begins
	int			lSlotID;		// ID of Slot rendering Source's environment (enables a send to this FXSlot)
};

struct wavinfo_t {
	int			format;
	int			rate;
	int			width;
	int			channels;
	int			samples;
	int			dataofs;		// chunk starts this many bytes from file start
};

extern	channel_t				s_channels[MAX_CHANNELS];
extern	int						s_paintedtime;
extern	portable_samplepair_t	s_rawsamples[MAX_RAW_SAMPLES];
extern	vec3_t					listener_origin;

byte *SND_malloc(int iSize, sfx_t *sfx);
channel_t *S_PickChannel(int entnum, int entchannel);
const char* Music_GetLevelSetName(void);
float S_GetSampleLengthInMilliSeconds(sfxHandle_t sfxHandle);
int SND_FreeOldestSound(sfx_t *pButNotThisOne = nullptr);
portable_samplepair_t *S_GetRawSamplePointer();
bool S_FileExists( const char *psFilename );
bool S_LoadSound( sfx_t *sfx );
sfx_t* S_FindName(const char* name);
void AS_Free(void);
void AS_Init(void);
void S_AddAmbientLoopingSound(const vec3_t origin, unsigned char volume, sfxHandle_t sfxHandle);
void S_DisplayFreeMemory(void);
void S_FreeAllSFXMem(void);
void S_memoryLoad(sfx_t *sfx);
void S_MP3_CalcVols_f(void);
void S_PaintChannels(int endtime);
void S_StartAmbientSound(const vec3_t origin, int entityNum, unsigned char volume, sfxHandle_t sfxHandle);
void S_StartLocalLoopingSound(sfxHandle_t sfx);
void S_StopSounds(void);
void S_UnCacheDynamicMusic(void);
void S_Update_(void);
void SND_setup();
void SND_TouchSFX(sfx_t *sfx);
wavinfo_t GetWavinfo (const char *name, byte *wav, int wavlength);

#ifdef USE_OPENAL
// Open AL
bool LoadEALFile(char* szEALFilename);
channel_t* S_OpenALPickChannel(int entnum, int entchannel);
float CalcDistance(EMPOINT A, EMPOINT B);
int  S_MP3PreProcessLipSync(channel_t* ch, short* data);
void AL_UpdateRawSamples();
void EALFileInit(const char* level);
void InitEAXManager();
void ReleaseEAXManager();
void S_PreProcessLipSync(sfx_t* sfx);
void S_SetLipSyncs();
void UnloadEALFile();
void UpdateEAXBuffer(channel_t* ch);
void UpdateEAXListener();
void UpdateLoopingSounds();
void UpdateSingleShotSounds();
#endif

#include "client/snd_mp3.h"
