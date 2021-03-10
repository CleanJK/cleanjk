/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2005 - 2015, ioquake3 contributors
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

#include "ui/ui_local.hpp"

uiImport_t *trap = nullptr;

// uniform functions

NORETURN_PTR void (*Com_Error)                    ( int level, const char *fmt, ... );
void              (*Com_Printf)                   ( const char *fmt, ... );

// shims
float Text_Width( const Text &text, const char *str ) {
	return trap->Text_Width( text, str );
}

float Text_Height( const Text &text, const char *str ) {
	return trap->Text_Height( text, str );
}

void Text_Paint( const Text &text, float x, float y, const char *str, const vec4_t colour, int style, int limit, float adjust, int cursorPos, bool overstrike ) {
	trap->Text_Paint( text, x, y, str, colour, style, limit, adjust, cursorPos, overstrike );
}

void TextHelper_Paint_Limit( float *maxX, float x, float y, float scale, const vec4_t color, const char *str, float adjust, int limit, JKFont font ) {
	trap->TextHelper_Paint_Limit( maxX, x, y, scale, color, str, adjust, limit, font );
}

void TextHelper_Paint_Proportional( int x, int y, const char *str, int style, const vec4_t color, JKFont font = JKFont::Medium, float scale = 1.0f ) {
	trap->TextHelper_Paint_Proportional( x, y, str, style, color, font, scale );
}

Q_CABI {
Q_EXPORT uiExport_t* QDECL GetModuleAPI( int apiVersion, uiImport_t *import )
{
	static uiExport_t uie = {0};

	assert( import );
	trap = import;
	Com_Printf =                    trap->Print;
	Com_Error =                     trap->Error;

	memset( &uie, 0, sizeof( uie ) );

	if ( apiVersion != UI_API_VERSION ) {
		trap->Print( "Mismatched UI_API_VERSION: expected %i, got %i\n", UI_API_VERSION, apiVersion );
		return nullptr;
	}

	uie.Init				= UI_Init;
	uie.Shutdown			= UI_Shutdown;
	uie.KeyEvent			= UI_KeyEvent;
	uie.MouseEvent			= UI_MouseEvent;
	uie.Refresh				= UI_Refresh;
	uie.IsFullscreen		= Menus_AnyFullScreenVisible;
	uie.SetActiveMenu		= UI_SetActiveMenu;
	uie.ConsoleCommand		= UI_ConsoleCommand;
	uie.DrawConnectScreen	= UI_DrawConnectScreen;
	uie.MenuReset			= Menu_Reset;

	return &uie;
}
}
