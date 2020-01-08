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

#include "qcommon/q_shared.h"
#include "qcommon/com_cvars.h"

// ======================================================================
// DEFINE
// ======================================================================

// alter these to suit your own game...
#define SE_BOOL					bool
#define SE_TRUE					true
#define SE_FALSE				false
#define	iSE_MAX_FILENAME_LENGTH	MAX_QPATH
#define sSE_STRINGS_DIR			"strings"
#define sSE_DEBUGSTR_PREFIX		"["		// any string you want prefixing onto the debug versions of strings (to spot hardwired english etc)
#define sSE_DEBUGSTR_SUFFIX		"]"		// ""

// some needed text-equates, do not alter these under any circumstances !!!! (unless you're me. Which you're not)
#define iSE_VERSION					1
#define sSE_KEYWORD_VERSION			"VERSION"
#define sSE_KEYWORD_CONFIG			"CONFIG"
#define sSE_KEYWORD_FILENOTES		"FILENOTES"
#define sSE_KEYWORD_REFERENCE		"REFERENCE"
#define sSE_KEYWORD_FLAGS 			"FLAGS"
#define sSE_KEYWORD_NOTES			"NOTES"
#define sSE_KEYWORD_LANG			"LANG_"
#define sSE_KEYWORD_ENDMARKER		"ENDMARKER"
#define sSE_FILE_EXTENSION			".st"	// editor-only NEVER used ingame, but I wanted all extensions together
#define sSE_EXPORT_FILE_EXTENSION	".ste"
#define sSE_INGAME_FILE_EXTENSION	".str"
#define sSE_EXPORT_SAME				"#same"

// ======================================================================
// FUNCTION
// ======================================================================

const char* SE_GetFlagName(int iFlagIndex);
const char* SE_GetLanguageDir(int iLangIndex);
const char* SE_GetLanguageName(int iLangIndex);
const char* SE_GetString(const char* psPackageAndStringReference);
const char* SE_GetString(const char* psPackageReference, const char* psStringReference);
const char* SE_LoadLanguage(const char* psLanguage, SE_BOOL bLoadDebug = SE_TRUE);
int SE_GetFlagMask(const char* psFlagName);
int	SE_GetFlags(const char* psPackageAndStringReference);
int	SE_GetFlags(const char* psPackageReference, const char* psStringReference);
int	SE_GetNumFlags(void);
int	SE_GetNumLanguages(void);
void SE_CheckForLanguageUpdates(void);
void SE_Init(void);
void SE_NewLanguage(void);
void SE_ShutDown(void);

// note that so far the only place in the game that needs to know these is the font system so it can know how to
// interpret char codes, for this reason I'm only exposing these simple bool queries...

static inline SE_BOOL Language_IsRussian(void)
{
	return (se_language && !Q_stricmp(se_language->string, "russian")) ? SE_TRUE : SE_FALSE;
}

static inline SE_BOOL Language_IsPolish(void)
{
	return (se_language && !Q_stricmp(se_language->string, "polish")) ? SE_TRUE : SE_FALSE;
}

static inline SE_BOOL Language_IsKorean(void)
{
	return (se_language && !Q_stricmp(se_language->string, "korean")) ? SE_TRUE : SE_FALSE;
}

static inline SE_BOOL Language_IsTaiwanese(void)
{
	return (se_language && !Q_stricmp(se_language->string, "taiwanese")) ? SE_TRUE : SE_FALSE;
}

static inline SE_BOOL Language_IsJapanese(void)
{
	return (se_language && !Q_stricmp(se_language->string, "japanese")) ? SE_TRUE : SE_FALSE;
}

static inline SE_BOOL Language_IsChinese(void)
{
	return (se_language && !Q_stricmp(se_language->string, "chinese")) ? SE_TRUE : SE_FALSE;
}

static inline SE_BOOL Language_IsThai(void)
{
	return (se_language && !Q_stricmp(se_language->string, "thai")) ? SE_TRUE : SE_FALSE;
}

