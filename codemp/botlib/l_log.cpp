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

// log file

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qcommon/q_shared.hpp"
#include "botlib/botlib.hpp"
#include "botlib/be_interface.hpp"			//for botimport.Print
#include "botlib/l_libvar.hpp"
#include "botlib/l_log.hpp"

#define MAX_LOGFILENAMESIZE		1024

struct logfile_t {
	char filename[MAX_LOGFILENAMESIZE];
	FILE *fp;
	int numwrites;
};

static logfile_t logfile;

//open a log file
void Log_Open(char *filename)
{
	if (!LibVarValue("log", "0")) return;
	if (!filename || !strlen(filename))
	{
		botimport.Print(PRT_MESSAGE, "openlog <filename>\n");
		return;
	} //end if
	if (logfile.fp)
	{
		botimport.Print(PRT_ERROR, "log file %s is already opened\n", logfile.filename);
		return;
	} //end if
	logfile.fp = fopen(filename, "wb");
	if (!logfile.fp)
	{
		botimport.Print(PRT_ERROR, "can't open the log file %s\n", filename);
		return;
	} //end if
	Q_strncpyz(logfile.filename, filename, sizeof(logfile.filename));
	botimport.Print(PRT_MESSAGE, "Opened log %s\n", logfile.filename);
} //end of the function Log_Create

//close the current log file
void Log_Close(void)
{
	if (!logfile.fp) return;
	if (fclose(logfile.fp))
	{
		botimport.Print(PRT_ERROR, "can't close log file %s\n", logfile.filename);
		return;
	} //end if
	logfile.fp = nullptr;
	botimport.Print(PRT_MESSAGE, "Closed log %s\n", logfile.filename);
} //end of the function Log_Close

//close log file if present
void Log_Shutdown(void)
{
	if (logfile.fp) Log_Close();
} //end of the function Log_Shutdown

//write to the current opened log file
void QDECL Log_Write(char *fmt, ...)
{
	va_list ap;

	if (!logfile.fp) return;
	va_start(ap, fmt);
	vfprintf(logfile.fp, fmt, ap);
	va_end(ap);
	//fprintf(logfile.fp, "\r\n");
	fflush(logfile.fp);
} //end of the function Log_Write

//write to the current opened log file with a time stamp
void QDECL Log_WriteTimeStamped(char *fmt, ...)
{
	va_list ap;

	if (!logfile.fp) return;
	fprintf(logfile.fp, "%d   %02d:%02d:%02d:%02d   ",
					logfile.numwrites,
					(int) (botlibglobals.time / 60 / 60),
					(int) (botlibglobals.time / 60),
					(int) (botlibglobals.time),
					(int) ((int) (botlibglobals.time * 100)) -
							((int) botlibglobals.time) * 100);
	va_start(ap, fmt);
	vfprintf(logfile.fp, fmt, ap);
	va_end(ap);
	fprintf(logfile.fp, "\r\n");
	logfile.numwrites++;
	fflush(logfile.fp);
} //end of the function Log_Write

//returns a pointer to the log file
FILE *Log_FilePointer(void)
{
	return logfile.fp;
} //end of the function Log_FilePointer

//flush log file
void Log_Flush(void)
{
	if (logfile.fp) fflush(logfile.fp);
} //end of the function Log_Flush

