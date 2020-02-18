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
// DEFINE
// ======================================================================

#ifndef MAX_PATH
	#define MAX_PATH			MAX_QPATH
#endif

#ifndef PATH_SEPERATORSTR
	#ifdef _WIN32
		#define PATHSEPERATOR_STR		"\\"
	#else
		#define PATHSEPERATOR_STR		"/"
	#endif
#endif
#ifndef PATH_SEPERATORCHAR
	#ifdef _WIN32
		#define PATHSEPERATOR_CHAR		'\\'
	#else
		#define PATHSEPERATOR_CHAR		'/'
	#endif
#endif

#define DEFINE_FIXED			0x0001
#define	DEFINE_GLOBAL			0x0002

#define BUILTIN_LINE			1
#define BUILTIN_FILE			2
#define BUILTIN_DATE			3
#define BUILTIN_TIME			4
#define BUILTIN_STDC			5

#define INDENT_IF				0x0001
#define INDENT_ELSE				0x0002
#define INDENT_ELIF				0x0004
#define INDENT_IFDEF			0x0008
#define INDENT_IFNDEF			0x0010

// ======================================================================
// STRUCT
// ======================================================================

//macro definitions
struct define_t {
	char     *name;       // define name
	int       flags;      // define flags
	int       builtin;    //  > 0 if builtin define
	int       numparms;   // number of define parameters
	token_t  *parms;      // define parameters
	token_t  *tokens;     // macro tokens (possibly containing parm tokens)
	define_t *next;       // next defined macro in a list
	define_t *hashnext;   // next define in the hash chain
	define_t *globalnext; // used to link up the globald defines
};

//indents
//used for conditional compilation directives:
//#if, #else, #elif, #ifdef, #ifndef
struct indent_t {
	int type;								//indent type
	int skip;								//true if skipping current indent
	script_t *script;						//script the indent was in
	indent_t *next;					//next indent on the indent stack
};

//source file
struct source_t {
	char filename[1024];					//file name of the script
	char includepath[1024];					//path to include files
	punctuation_t *punctuations;			//punctuations to use
	script_t *scriptstack;					//stack with scripts of the source
	token_t *tokens;						//tokens to read first
	define_t *defines;						//list with macro definitions
	define_t **definehash;					//hash chain with defines
	indent_t *indentstack;					//stack with indents
	int skip;								// > 0 if skipping conditional code
	token_t token;							//last read token
};

// ======================================================================
// STRUCT
// ======================================================================

int PC_AddDefine(source_t* source, char* string);
int PC_AddGlobalDefine(char* string);
int PC_CheckTokenString(source_t* source, char* string);
int PC_CheckTokenType(source_t* source, int type, int subtype, token_t* token);
int PC_ExpectAnyToken(source_t* source, token_t* token);
int PC_ExpectTokenString(source_t* source, char* string);
int PC_ExpectTokenType(source_t* source, int type, int subtype, token_t* token);
int PC_FreeSourceHandle(int handle);
int PC_LoadGlobalDefines(const char* filename);
int PC_LoadSourceHandle(const char* filename);
int PC_ReadLine(source_t* source, token_t* token);
int PC_ReadToken(source_t* source, token_t* token);
int PC_ReadTokenHandle(int handle, pc_token_t* pc_token);
int PC_RemoveGlobalDefine(char* name);
int PC_SkipUntilString(source_t* source, char* string);
int PC_SourceFileAndLine(int handle, char* filename, int* line, int sizeFilename);
int PC_WhiteSpaceBeforeToken(token_t* token);
source_t* LoadSourceFile(const char* filename);
source_t* LoadSourceMemory(char* ptr, int length, char* name);
void FreeSource(source_t* source);
void PC_AddBuiltinDefines(source_t* source);
void PC_CheckOpenSourceHandles(void);
void PC_Init(void);
void PC_RemoveAllGlobalDefines(void);
void PC_RemoveAllGlobalDefines(void);
void PC_SetBaseFolder(char* path);
void PC_SetIncludePath(source_t* source, char* path);
void PC_SetPunctuations(source_t* source, punctuation_t* p);
void PC_UnreadLastToken(source_t* source);
void PC_UnreadToken(source_t* source, token_t* token);
void QDECL SourceError(source_t* source, char* str, ...);
void QDECL SourceWarning(source_t* source, char* str, ...);
