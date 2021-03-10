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

// Filename:-	stringed_interface.cpp
// This file contains functions that StringEd wants to call to do things like load/save, they can be modified
//	for use ingame, but must remain functionally the same...
//  Please try and put modifications for whichever games this is used for inside #defines, so I can copy the same file
//		into each project.

// stuff common to all qcommon files...
#include "server/server.hpp"
#include "qcommon/q_shared.hpp"
#include "qcommon/q_common.hpp"
#include "qcommon/stringed_interface.hpp"
#include "qcommon/stringed_ingame.hpp"

#include <string>

// this just gets the binary of the file into memory, so I can parse it. Called by main SGE loader
//  returns either char * of loaded file, else nullptr for failed-to-open...
unsigned char *SE_LoadFileData( const char *psFileName, int *piLoadedLength /* = 0 */) {
	unsigned char *psReturn = nullptr;
	if ( piLoadedLength ) {
		*piLoadedLength = 0;
	}

	// local filename, so prepend the base dir etc according to game and load it however (from PAK?)

	unsigned char *pvLoadedData;
	int iLen = FS_ReadFile( psFileName, (void **)&pvLoadedData );

	if ( iLen > 0 ) {
		psReturn = pvLoadedData;
		if ( piLoadedLength ) {
			*piLoadedLength = iLen;
		}
	}

	return psReturn;
}

// called by main SGE code after loaded data has been parsedinto internal structures...
void SE_FreeFileDataAfterLoad( unsigned char *psLoadedFile ) {
	if ( psLoadedFile ) {
		FS_FreeFile( psLoadedFile );
	}
}

int giFilesFound;
static void SE_R_ListFiles( const char *psExtension, const char *psDir, std::string &strResults ) {
	int numdirs;
	char **dirFiles = FS_ListFiles( psDir, "/", &numdirs );
	for ( int i = 0; i < numdirs; i++ ) {
		if ( dirFiles[i][0] && dirFiles[i][0] != '.' ) {
			// skip blanks, plus ".", ".." etc
			char sDirName[MAX_QPATH];
			Com_sprintf( sDirName, sizeof(sDirName), "%s/%s", psDir, dirFiles[i] );

			// for some reason the quake filesystem in this game now returns an extra slash on the end, didn't used to. Sigh...
			if ( sDirName[strlen( sDirName )-1] == '/' ) {
				sDirName[strlen( sDirName )-1] = '\0';
			}
			SE_R_ListFiles( psExtension, sDirName, strResults );
		}
	}

	int numSysFiles;
	char **sysFiles = FS_ListFiles( psDir, psExtension, &numSysFiles );
	for ( int i = 0; i < numSysFiles; i++ ) {
		char sFilename[MAX_QPATH];
		Com_sprintf( sFilename, sizeof(sFilename), "%s/%s", psDir, sysFiles[i] );
		strResults += sFilename;
		strResults += ';';
		giFilesFound++;
	}
	FS_FreeFileList( sysFiles );
	FS_FreeFileList( dirFiles );
}

// replace this with a call to whatever your own code equivalent is.
// expected result is a ';'-delineated string (including last one) containing file-list search results
int SE_BuildFileList( const char *psStartDir, std::string &strResults ) {
	giFilesFound = 0;
	strResults = "";

	SE_R_ListFiles( sSE_INGAME_FILE_EXTENSION, psStartDir, strResults );

	return giFilesFound;
}
