/*
===========================================================================
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

#include "client/snd_local.hpp"

// ======================================================================
// STRUCT
// ======================================================================

typedef struct id3v1_1 {
    char id[3];
    char title[30];		// <file basename>
    char artist[30];	// "Raven Software"
    char album[30];		// "#UNCOMP %d"		// needed
    char year[4];		// "2000"
    char comment[28];	// "#MAXVOL %g"		// needed
    char zero;
    char track;
    char genre;
} id3v1_1;	// 128 bytes in size

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern const char sKEY_MAXVOL[];
extern const char sKEY_UNCOMP[];

// ======================================================================
// FUNCTION
// ======================================================================

// (so far, all these functions are only called from one place in snd_mem.cpp)
// (filenames are used purely for error reporting, all files should already be loaded before you get here)
float		MP3Stream_GetPlayingTimeInSeconds( LP_MP3STREAM lpMP3Stream );
float		MP3Stream_GetRemainingTimeInSeconds( LP_MP3STREAM lpMP3Stream );
int			MP3_GetUnpackedSize		( const char *psLocalFilename, void *pvData, int iDataLen, bool qbIgnoreID3Tag = false, bool bStereoDesired = false );
int			MP3_UnpackRawPCM		( const char *psLocalFilename, void *pvData, int iDataLen, byte *pbUnpackBuffer, bool bStereoDesired = false );
int			MP3Stream_Decode		( LP_MP3STREAM lpMP3Stream,  bool bDoingMusic );
bool	MP3_FakeUpWAVInfo		( const char *psLocalFilename, void *pvData, int iDataLen, int iUnpackedDataLength, int &format, int &rate, int &width,
                                        int &channels, int &samples, int &dataofs, bool bStereoDesired = false );
bool	MP3_IsValid				( const char *psLocalFilename, void *pvData, int iDataLen, bool bStereoDesired = false );
bool	MP3_ReadSpecialTagInfo	( byte *pbLoadedFile, int iLoadedFileLen, id3v1_1** ppTAG = nullptr, int *piUncompressedSize = nullptr, float *pfMaxVol = nullptr);
bool	MP3Stream_GetSamples	( channel_t *ch, int startingSampleNum, int count, short *buf, bool bStereo );
bool	MP3Stream_InitFromFile	( sfx_t* sfx, byte *pbSrcData, int iSrcDatalen, const char *psSrcDataFilename, int iMP3UnPackedSize, bool bStereoDesired = false );
bool	MP3Stream_InitPlayingTimeFields( LP_MP3STREAM lpMP3Stream, const char *psLocalFilename, void *pvData, int iDataLen, bool bStereoDesired = false);
bool	MP3Stream_Rewind		( channel_t *ch );
bool	MP3Stream_SeekTo		( channel_t *ch, float fTimeToSeekTo );
void		MP3_InitCvars			( void );

// the real worker code deep down in the MP3 C code...  (now externalised here so the music streamer can access one)
#ifdef __cplusplus
extern "C"
{
#endif

char*	C_MP3_GetHeaderData		(void *pvData, int iDataLen, int *piRate, int *piWidth, int *piChannels, int bStereoDesired);
char*	C_MP3_GetUnpackedSize	(void *pvData, int iDataLen, int *piUnpackedSize, int bStereoDesired);
char*	C_MP3_IsValid			(void *pvData, int iDataLen, int bStereoDesired);
char*	C_MP3_UnpackRawPCM		(void *pvData, int iDataLen, int *piUnpackedSize, void *pbUnpackBuffer, int bStereoDesired);
char*	C_MP3Stream_DecodeInit	(LP_MP3STREAM pSFX_MP3Stream, void *pvSourceData, int iSourceBytesRemaining, int iGameAudioSampleRate,
                                    int iGameAudioSampleBits, int bStereoDesired);
char*	C_MP3Stream_Rewind		(LP_MP3STREAM pSFX_MP3Stream);
unsigned int C_MP3Stream_Decode( LP_MP3STREAM pSFX_MP3Stream, int bFastForwarding );

#ifdef __cplusplus
}
#endif
