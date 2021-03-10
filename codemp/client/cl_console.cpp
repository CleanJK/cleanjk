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

#include "client/cl_cgameapi.hpp"
#include "client/cl_keys.hpp"
#include "client/cl_local.hpp"
#include "client/cl_fonts.hpp"
#include "game/bg_public.hpp"
#include "qcommon/q_shared.hpp"
#include "qcommon/com_cvars.hpp"
#include "qcommon/com_inputField.hpp"
#include "qcommon/game_version.hpp"
#include "qcommon/stringed_ingame.hpp"

#define DEFAULT_CONSOLE_WIDTH 78

const vec4_t console_color = { 0.509f, 0.609f, 0.847f, 1.0f };

static console_t   console;


void Console_ToggleConsole_f( void ) {
	if ( con_autoclear->integer ) {
		console.field->Clear();
	}

	Console_ClearNotify();
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_CONSOLE );
}

void Console_ToggleMenu_f( void ) {
	CL_KeyEvent( A_ESCAPE,  true, Sys_Milliseconds() );
	CL_KeyEvent( A_ESCAPE, false, Sys_Milliseconds() );
}

void Console_MessageMode_f( void ) {
	// yell
	chat_playerNum = -1;
	chat_team = false;
	chatField.Clear();

	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}

void Console_MessageMode2_f( void ) {
	// team chat
	chat_playerNum = -1;
	chat_team = true;
	chatField.Clear();
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}

void Console_MessageMode3_f( void ) {
	// target chat
	if ( !cls.cgameStarted ) {
		assert( !"null cgvm" );
		return;
	}

	chat_playerNum = CGVM_CrosshairPlayer();
	if ( chat_playerNum < 0 || chat_playerNum >= MAX_CLIENTS ) {
		chat_playerNum = -1;
		return;
	}
	chat_team = false;
	chatField.Clear();
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}

void Console_MessageMode4_f( void ) {
	//attacker
	if ( !cls.cgameStarted ) {
		assert( !"null cgvm" );
		return;
	}

	chat_playerNum = CGVM_LastAttacker();
	if ( chat_playerNum < 0 || chat_playerNum >= MAX_CLIENTS ) {
		chat_playerNum = -1;
		return;
	}
	chat_team = false;
	chatField.Clear();
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}

void Console_Clear_f( void ) {
	memset( console.text, 0, sizeof(console.text) );
	Console_Bottom(); // go to end
}

// Save the console contents out to a file
void Console_Dump_f( void ) {
	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "%s\n", SE_GetString( "CON_TEXT_DUMP_USAGE" ) );
		return;
	}

	char filename[MAX_QPATH];
	Q_strncpyz( filename, Cmd_Argv( 1 ), sizeof(filename) );
	COM_DefaultExtension( filename, sizeof(filename), ".txt" );

	if ( !COM_CompareExtension( filename, ".txt" ) ) {
		Com_Printf( "Console_Dump_f: Only the \".txt\" extension is supported by this command!\n" );
		return;
	}

	const fileHandle_t f = FS_FOpenFileWrite( filename );
	if ( !f ) {
		Com_Printf( "ERROR: couldn't open %s.\n", filename );
		return;
	}

	Com_Printf( "Dumped console text to %s.\n", filename );

	// skip empty lines
	for ( int l = console.current - console.totallines + 1; l <= console.current; l++ ) {
		const char *line = console.text + (l % console.totallines) * console.linewidth;
		int x;
		for ( x = 0; x < console.linewidth; x++ ) {
			if ( line[x] != ' ' ) {
				break;
			}
		}
		if ( x != console.linewidth ) {
			break;
		}
	}

#ifdef _WIN32
	const int bufferlen = console.linewidth + 3 * sizeof ( char );
#else
	const int bufferlen = console.linewidth + 2 * sizeof ( char );
#endif

	char *buffer = (char *)Hunk_AllocateTempMemory( bufferlen );

	// write the remaining lines
	buffer[bufferlen - 1] = '\0';
	for ( int l = 0; l <= console.current ; l++ ) {
		const char *line = console.text + (l % console.totallines) * console.linewidth;
		for ( int i = 0; i < console.linewidth; i++ ) {
			buffer[i] = line[i];
		}
		for ( int x = console.linewidth-1; x >= 0; x-- ) {
			if ( buffer[x] == ' ' ) {
				buffer[x] = 0;
			}
			else {
				break;
			}
		}
#ifdef _WIN32
		Q_strcat( buffer, bufferlen, "\r\n" );
#else
		Q_strcat( buffer, bufferlen, "\n" );
#endif
		FS_Write( buffer, strlen( buffer ), f );
	}

	Hunk_FreeTempMemory( buffer );
	FS_FCloseFile( f );
}

void Console_ClearNotify( void ) {
	for ( int i = 0; i < NUM_CON_TIMES; i++ ) {
		console.times[i] = 0;
	}
}

// If the line width has changed, reformat the buffer.
void Console_CheckResize( void ) {
	const int width = (cls.glconfig.vidWidth / SMALLCHAR_WIDTH) - 2;
	if ( width == console.linewidth ) {
		return;
	}

	if ( width < 1 ) {
		// video hasn't been initialized yet
		console.xadjust = 1;
		console.yadjust = 1;
		console.linewidth = DEFAULT_CONSOLE_WIDTH;
		console.totallines = CON_TEXTSIZE / console.linewidth;
		memset( console.text, 0, sizeof(console.text) );
	}
	else {
		console.xadjust = SCREEN_WIDTH / cls.glconfig.vidWidth;
		console.yadjust = SCREEN_HEIGHT / cls.glconfig.vidHeight;

		const int oldwidth = console.linewidth;
		console.linewidth = width;
		const int oldtotallines = console.totallines;
		console.totallines = CON_TEXTSIZE / console.linewidth;
		int numlines = oldtotallines;

		if ( console.totallines < numlines ) {
			numlines = console.totallines;
		}

		int numchars = oldwidth;

		if ( console.linewidth < numchars ) {
			numchars = console.linewidth;
		}

		char tbuf[CON_TEXTSIZE];
		memcpy( tbuf, console.text, CON_TEXTSIZE );
		memset( console.text, 0, sizeof(console.text) );

		for ( int i = 0; i < numlines; i++ ) {
			for ( int j = 0; j < numchars; j++ ) {
				console.text[
					(console.totallines - 1 - i) * console.linewidth + j
				] = tbuf[
					(
						(console.current - i + oldtotallines)
							% oldtotallines
					) * oldwidth + j
				];
			}
		}

		Console_ClearNotify();
	}

	console.current = console.totallines - 1;
	console.display = console.current;
}

static void Console_Execute( const char *text ) {
	Com_Printf( CONSOLE_PROMPT_STR "%s\n", text );

	Cbuf_AddText( text );
	Cbuf_AddText( "\n" );

	if ( !text[0] ) {
		return; // empty lines just scroll the console without adding to history
	}

	if ( cls.state == CA_DISCONNECTED ) {
		// force an update, because the command may take some time
		SCR_UpdateScreen();
	}
}

static const std::string Console_Complete( const char *token ) {
	ShortestMatchFinder smf( token );

	// prefer matching commands
	for ( const cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next ) {
		const char *description = cmd->description ?
			va( S_COLOR_GREY " Cmd " S_COLOR_WHITE "%s " S_COLOR_GREY "(%s)", cmd->name, cmd->description ) :
			va( S_COLOR_GREY " Cmd " S_COLOR_WHITE "%s", cmd->name );

		smf.Feed( cmd->name, description );
	}

	// also try match from non-internal cvars
	for ( cvar_t *cv = cvar_vars; cv; cv = cv->next ) {
		if ( cv->flags & CVAR_INTERNAL ) {
			continue;
		}

		const char *description = cv->description ?
			va( S_COLOR_GREY "Cvar " S_COLOR_WHITE "%s " S_COLOR_GREY "= \"" S_COLOR_WHITE "%s" S_COLOR_GREY "\" (%s, default = \"" S_COLOR_WHITE "%s" S_COLOR_GREY "\")", cv->name, cv->string, cv->description, cv->resetString ) :
			va( S_COLOR_GREY "Cvar " S_COLOR_WHITE "%s " S_COLOR_GREY "= \"" S_COLOR_WHITE "%s" S_COLOR_GREY "\" (default = \"" S_COLOR_WHITE "%s" S_COLOR_GREY "\")", cv->name, cv->string, cv->resetString );

		smf.Feed( cv->name, description );
	}

	if ( !smf.IsComplete() ) {
		smf.PrintMatches();
	}

	return smf.GetShortestMatch();
}

void Console_Init( void ) {
	console.field = new InputField( Console_Execute, Console_Complete );

	Cmd_AddCommand( "clear",         Console_Clear_f,         "Clear console text" );
	Cmd_AddCommand( "condump",       Console_Dump_f,          "Dump console text to file" );
	Cmd_AddCommand( "messagemode",   Console_MessageMode_f,   "Global Chat" );
	Cmd_AddCommand( "messagemode2",  Console_MessageMode2_f,  "Team Chat" );
	Cmd_AddCommand( "messagemode3",  Console_MessageMode3_f,  "Private Chat with Target Player" );
	Cmd_AddCommand( "messagemode4",  Console_MessageMode4_f,  "Private Chat with Last Attacker" );
	Cmd_AddCommand( "toggleconsole", Console_ToggleConsole_f, "Show/hide console" );
	Cmd_AddCommand( "togglemenu",    Console_ToggleMenu_f,    "Show/hide the menu" );
}

void Console_Shutdown( void ) {
	Cmd_RemoveCommand( "toggleconsole" );
	Cmd_RemoveCommand( "togglemenu" );
	Cmd_RemoveCommand( "messagemode" );
	Cmd_RemoveCommand( "messagemode2" );
	Cmd_RemoveCommand( "messagemode3" );
	Cmd_RemoveCommand( "messagemode4" );
	Cmd_RemoveCommand( "clear" );
	Cmd_RemoveCommand( "condump" );

	delete console.field;
	console.field = nullptr;
}

static void Console_Linefeed( bool skipnotify ) {
	// mark time for transparent overlay
	if ( console.current >= 0 ) {
		console.times[console.current % NUM_CON_TIMES] = skipnotify ? 0 : cls.realtime;
	}

	console.x = 0;
	if ( console.display == console.current ) {
		console.display++;
	}
	console.current++;
	for ( int i = 0; i < console.linewidth; i++ ) {
		console.text[ (console.current % console.totallines) * console.linewidth + i ] = '\0';
	}
}

// Handles cursor positioning, line wrapping, etc
// All console printing must go through this in order to be logged to disk
// If no console is visible, the text will appear at the top of the game window
void CL_ConsolePrint( const char *txt ) {
	// TTimo - prefix for text that shows up in console but not in notify
	// backported from RTCW
	bool skipnotify = false;
	if ( !Q_strncmp( txt, "[skipnotify]", 12 ) ) {
		skipnotify = true;
		txt += 12;
	}
	if ( txt[0] == '*' ) {
		skipnotify = true;
		txt += 1;
	}

	// for some demos we don't want to ever show anything on the console
	if ( cl_noprint && cl_noprint->integer ) {
		return;
	}

	if ( !console.initialized ) {
		console.linewidth = -1;
		Console_CheckResize();
		console.initialized = true;
	}

	int c;
	while ( (c = (unsigned char)*txt) != 0 ) {
		if ( Q_IsColorString( (unsigned char *)txt ) ) {
			//color = ColorIndex( *(txt + 1) );
			//txt += 2;
			//continue;
		}

		// count word length
		int l;
		for ( l = 0; l < console.linewidth; l++ ) {
			if ( txt[l] <= ' ') {
				break;
			}
		}

		// word wrap
		if ( l != console.linewidth && console.x + l >= console.linewidth ) {
			Console_Linefeed( skipnotify );
		}

		txt++;

		switch ( c ) {

		case '\n': {
			Console_Linefeed( skipnotify );
		} break;

		case '\r': {
			console.x = 0;
		} break;

		default: {
			// display character and advance
			const int y = console.current % console.totallines;
			console.text[y * console.linewidth + console.x] = c;
			console.x++;
			if ( console.x >= console.linewidth ) {
				Console_Linefeed( skipnotify );
			}
		} break;

		}
	}

	// mark time for transparent overlay
	if ( console.current >= 0 ) {
		if ( skipnotify ) {
			int prev = console.current % NUM_CON_TIMES - 1;
			if ( prev < 0 ) {
				prev = NUM_CON_TIMES - 1;
			}
			console.times[prev] = 0;
		}
		else {
			console.times[console.current % NUM_CON_TIMES] = cls.realtime;
		}
	}
}

// DRAWING

// Draws the last few lines of output transparently over the game top
void Console_DrawNotify( void ) {
	int currentColor = 7;
	re->SetColor( g_color_table[currentColor] );

	int v = 0;
	char *text;
	for ( int i = console.current - NUM_CON_TIMES + 1; i <= console.current; i++ ) {
		if ( i < 0 ) {
			continue;
		}
		int time = console.times[i % NUM_CON_TIMES];
		if ( time == 0 ) {
			continue;
		}
		time = cls.realtime - time;
		if ( time > con_notifytime->value * 1000 ) {
			continue;
		}
		text = console.text + (i % console.totallines) * console.linewidth;

		if ( cl.snap.ps.pm_type != PM_INTERMISSION && Key_GetCatcher() & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
			continue;
		}

		// concat the text to be printed...
		char sTemp[MAX_STRING_CHARS] = { 0 };
		Q_strncpyz( sTemp, text, console.linewidth );

		// and print...
		re->Font_DrawString( 2, v, sTemp, g_color_table[currentColor], console.field->textProperties.font, -1, console.field->textProperties.scale );
		v += re->Font_HeightPixels( console.field->textProperties.font, console.field->textProperties.scale );
	}

	re->SetColor( nullptr );

	if ( Key_GetCatcher() & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
		return;
	}

	// draw the chat line
	if ( Key_GetCatcher() & KEYCATCH_MESSAGE ) {
		const char *prompt = SE_GetString( "MP_SVGAME", chat_team ? "SAY_TEAM" : "SAY" );
		//const int skip = strlen( prompt );
		chatField.Draw( 0, v, SCREEN_WIDTH, true, prompt );
		v += BIGCHAR_HEIGHT;
	}

}

// Draws the console with the solid background
static void Console_DrawSolidConsole( void ) {
	console.field->textProperties = { JKFont::Small, Com_Clamp( 0.0f, 128.0f, con_fontScale->value ) }; // this seems naughty
	const float lineHeight = Text_Height( console.field->textProperties, " " );

	// everything is drawn from the bottom-up
	const float midpoint = (SCREEN_HEIGHT / 2.0f);
	console.vislines = midpoint / lineHeight;
	const int numLogRows = console.vislines - 2; // reserve for input line + version, scroll markers

	vec4_t con_color;
	MAKERGBA( con_color, 1.0f, 1.0f, 1.0f, Com_Clamp( 0.0f, 1.0f, con_opacity->value ) );

	float y = midpoint;

	// background
	re->SetColor( con_color );
	SCR_DrawPic( 0, 0, SCREEN_WIDTH, y, cls.consoleShader );

	// bottom border
	re->SetColor( console_color );
	re->DrawStretchPic( 0, y, SCREEN_WIDTH, 2, 0, 0, 0, 0, cls.whiteShader );

	// version number
	y -= lineHeight;
	Text_Paint( console.field->textProperties, SCREEN_WIDTH - Text_Width( console.field->textProperties, JK_VERSION ), y, JK_VERSION, colorTable[CT_MDGREY] );

	// input prompt, user text, and cursor
	if ( cls.state == CA_DISCONNECTED || (Key_GetCatcher() & KEYCATCH_CONSOLE) ) {
		console.field->Draw( 0, y, SCREEN_WIDTH, true, CONSOLE_PROMPT_STR );
	}

	// scroll markers
	y -= lineHeight;
	if ( console.display != console.current ) {
		const char *fill = "^ ";
		const float fillWidth = Text_Width( console.field->textProperties, fill );
		char fillBuf[MAX_STRING_CHARS] = {};
		for ( int i = 0; i < SCREEN_WIDTH / fillWidth; i++ ) {
			Q_strcat( fillBuf, sizeof(fillBuf), fill );
		}
		Text_Paint( console.field->textProperties, 0, y, fillBuf, colorTable[CT_RED] );
	}
	else {
		// ...
	}

	// console log
	y = numLogRows * lineHeight;
	for ( int i = 0; i < numLogRows; i++ ) {
		char sTemp[4096] = {0};
		//Q_strncpyz( sTemp, &console.text[(i % console.totallines) * console.linewidth], console.linewidth );
		Q_strncpyz( sTemp, &console.text[((console.display - i - 1) % console.totallines) * console.linewidth], console.linewidth );

		y -= lineHeight;
		Text_Paint( console.field->textProperties, 0, y, sTemp, colorTable[CT_WHITE] );
	}

	re->SetColor( nullptr );
}

void Console_DrawConsole( void ) {
	// check for console width changes from a vid mode change
	Console_CheckResize();

	// if disconnected, render console full screen
	if ( cls.state == CA_DISCONNECTED ) {
		if ( !(Key_GetCatcher() & (KEYCATCH_UI | KEYCATCH_CGAME)) ) {
			Console_DrawSolidConsole();
			return;
		}
	}

	if ( console.active ) {
		Console_DrawSolidConsole();
	}
	else {
		// draw notify lines
		if ( cls.state == CA_ACTIVE ) {
			Console_DrawNotify();
		}
	}
}

// Scroll it up or down
void Console_RunConsole( void ) {
	// decide on the destination height of the console
	console.active = !!(Key_GetCatcher() & KEYCATCH_CONSOLE);
}

void Console_PageUp( int mode ) {
	switch ( mode ) {

	default:
	case 0: {
		console.display -= 1;
	} break;

	case 1: {
		console.display -= 3;
	} break;

	case 2: {
		console.display -= console.vislines - 2;
	} break;

	}

	if ( console.current - console.display >= console.totallines ) {
		console.display = console.current - console.totallines + 1;
	}
}

void Console_PageDown( int mode ) {
	switch ( mode ) {

	default:
	case 0: {
		console.display += 1;
	} break;

	case 1: {
		console.display += 3;
	} break;

	case 2: {
		console.display += console.vislines - 2;
	} break;

	}
	if (console.display > console.current) {
		console.display = console.current;
	}
}

void Console_Top( void ) {
	console.display = console.totallines;
	if ( console.current - console.display >= console.totallines ) {
		console.display = console.current - console.totallines + 1;
	}
}

void Console_Bottom( void ) {
	console.display = console.current;
}

void Console_Close( void ) {
	if ( !cl_running->integer ) {
		return;
	}
	console.field->Clear();
	Console_ClearNotify();
	Key_SetCatcher( Key_GetCatcher() & ~KEYCATCH_CONSOLE );
	console.active = false;
}

// Handles history and console scrollback
void Console_KeyDownEvent( int key ) {
	// console-specific key handling
	if ( keynames[key].lower == 'l' && kg.keys[A_CTRL].down ) {
		// ctrl-L clears screen
		Cbuf_AddText( "clear\n" );
	}
	else if ( key == A_PAGE_UP ) {
		Console_PageUp( 2 );
	}
	else if ( key == A_MWHEELUP ) {
		Console_PageUp( kg.keys[A_CTRL].down ? 1 : 0 );
	}
	else if ( key == A_PAGE_DOWN ) {
		Console_PageDown( 2 );
	}
	else if ( key == A_MWHEELDOWN ) {
		Console_PageDown( kg.keys[A_CTRL].down ? 1 : 0 );
	}
	else if ( key == A_HOME && kg.keys[A_CTRL].down ) {
		// ctrl-home = top of console
		Console_Top();
	}
	else if ( key == A_END && kg.keys[A_CTRL].down ) {
		// ctrl-end = bottom of console
		Console_Bottom();
	}
	else {
		// pass to the normal editline routine
		console.field->KeyDownEvent( key );
	}
}

void Console_CharEvent( int key ) {
	console.field->CharEvent( key );
}
