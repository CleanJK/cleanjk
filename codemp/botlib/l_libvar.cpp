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

// bot library variables

#include "qcommon/q_shared.h"
#include "botlib/l_memory.h"
#include "botlib/l_libvar.h"

//list with library variables
libvar_t *libvarlist = nullptr;

float LibVarStringValue(char *string)
{
	int dotfound = 0;
	float value = 0;

	while(*string)
	{
		if (*string < '0' || *string > '9')
		{
			if (dotfound || *string != '.')
			{
				return 0;
			} //end if
			else
			{
				dotfound = 10;
				string++;
			} //end if
		} //end if
		if (dotfound)
		{
			value = value + (float) (*string - '0') / (float) dotfound;
			dotfound *= 10;
		} //end if
		else
		{
			value = value * 10.0 + (float) (*string - '0');
		} //end else
		string++;
	} //end while
	return value;
} //end of the function LibVarStringValue

libvar_t *LibVarAlloc(char *var_name)
{
	libvar_t *v;

	v = (libvar_t *) GetMemory(sizeof(libvar_t));
	Com_Memset(v, 0, sizeof(libvar_t));
	v->name = (char *) GetMemory(strlen(var_name)+1);
	strcpy(v->name, var_name);
	//add the variable in the list
	v->next = libvarlist;
	libvarlist = v;
	return v;
} //end of the function LibVarAlloc

void LibVarDeAlloc(libvar_t *v)
{
	if (v->string) FreeMemory(v->string);
	FreeMemory(v->name);
	FreeMemory(v);
} //end of the function LibVarDeAlloc

//removes all library variables
void LibVarDeAllocAll(void)
{
	libvar_t *v;

	for (v = libvarlist; v; v = libvarlist)
	{
		libvarlist = libvarlist->next;
		LibVarDeAlloc(v);
	} //end for
	libvarlist = nullptr;
} //end of the function LibVarDeAllocAll

//gets the library variable with the given name
libvar_t *LibVarGet(char *var_name)
{
	libvar_t *v;

	for (v = libvarlist; v; v = v->next)
	{
		if (!Q_stricmp(v->name, var_name))
		{
			return v;
		} //end if
	} //end for
	return nullptr;
} //end of the function LibVarGet

//gets the string of the library variable with the given name
char *LibVarGetString(char *var_name)
{
	libvar_t *v;

	v = LibVarGet(var_name);
	if (v)
	{
		return v->string;
	} //end if
	else
	{
		return "";
	} //end else
} //end of the function LibVarGetString

//gets the value of the library variable with the given name
float LibVarGetValue(char *var_name)
{
	libvar_t *v;

	v = LibVarGet(var_name);
	if (v)
	{
		return v->value;
	} //end if
	else
	{
		return 0;
	} //end else
} //end of the function LibVarGetValue

//creates the library variable if not existing already and returns it
libvar_t *LibVar(char *var_name, char *value)
{
	libvar_t *v;
	v = LibVarGet(var_name);
	if (v) return v;
	//create new variable
	v = LibVarAlloc(var_name);
	//variable string
	v->string = (char *) GetMemory(strlen(value) + 1);
	strcpy(v->string, value);
	//the value
	v->value = LibVarStringValue(v->string);
	//variable is modified
	v->modified = true;

	return v;
} //end of the function LibVar

//creates the library variable if not existing already and returns the value string
char *LibVarString(char *var_name, char *value)
{
	libvar_t *v;

	v = LibVar(var_name, value);
	return v->string;
} //end of the function LibVarString

//creates the library variable if not existing already and returns the value
float LibVarValue(char *var_name, char *value)
{
	libvar_t *v;

	v = LibVar(var_name, value);
	return v->value;
} //end of the function LibVarValue

//sets the library variable
void LibVarSet(char *var_name, char *value)
{
	libvar_t *v;

	v = LibVarGet(var_name);
	if (v)
	{
		FreeMemory(v->string);
	} //end if
	else
	{
		v = LibVarAlloc(var_name);
	} //end else
	//variable string
	v->string = (char *) GetMemory(strlen(value) + 1);
	strcpy(v->string, value);
	//the value
	v->value = LibVarStringValue(v->string);
	//variable is modified
	v->modified = true;
} //end of the function LibVarSet

//returns true if the library variable has been modified
bool LibVarChanged(char *var_name)
{
	libvar_t *v;

	v = LibVarGet(var_name);
	if (v)
	{
		return v->modified;
	} //end if
	else
	{
		return false;
	} //end else
} //end of the function LibVarChanged

//sets the library variable to unmodified
void LibVarSetNotModified(char *var_name)
{
	libvar_t *v;

	v = LibVarGet(var_name);
	if (v)
	{
		v->modified = false;
	} //end if
} //end of the function LibVarSetNotModified
