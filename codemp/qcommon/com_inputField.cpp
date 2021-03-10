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

#include <algorithm>

#include "qcommon/com_inputField.hpp"
#include "client/cl_keycodes.hpp"
#include "client/cl_fonts.hpp"

#if defined(BUILD_CGAME)
	#include "cgame/cg_local.hpp"
#elif defined(BUILD_UI)
	#include "ui/ui_local.hpp"
#endif

#if defined(BUILD_CGAME) || defined(BUILD_UI)
	#elif defined(BUILD_DEDICATED)
	// ...
#else
	#include "client/cl_local.hpp"
	#include "client/cl_keys.hpp"
	#include "client/cl_fonts.hpp"
#endif

// helpers for module/engine isolation
static bool IsKeyDown( int keycode ) {
	#if defined(BUILD_CGAME) || defined(BUILD_UI)
		return trap->Key_IsDown( keycode );
	#elif defined(BUILD_DEDICATED)
		return false;
	#else
		return kg.keys[A_SHIFT].down;
	#endif
}

static bool GetOverstrike( void ) {
	#if defined(BUILD_CGAME) || defined(BUILD_UI)
		return trap->Key_GetOverstrikeMode();
	#elif defined(BUILD_DEDICATED)
		return false;
	#else
		return kg.overstrikeMode;
	#endif
}

static void SetOverstrike( bool mode ) {
	#if defined(BUILD_CGAME) || defined(BUILD_UI)
		trap->Key_SetOverstrikeMode( mode );
	#elif defined(BUILD_DEDICATED)
		// ...
	#else
		kg.overstrikeMode = mode;
	#endif
}

#ifndef BUILD_DEDICATED
	static int Milliseconds( void ) {
		#if defined(BUILD_CGAME) || defined(BUILD_UI)
			return trap->Milliseconds();
		#else//if defined(BUILD_DEDICATED)
			return Sys_Milliseconds();
		#endif
	}
#endif

// updates and returns a local buffer of (MAX_STRING_CHARS*4) bytes
char *GetClipboardData( void ) {
	static char cbd[MAX_STRING_CHARS*4]{};

	#if defined(BUILD_CGAME) || defined(BUILD_UI)
		trap->GetClipboardData( cbd, sizeof(cbd) );
	#elif defined(BUILD_DEDICATED)
		//TODO: implement GetClipboardData via Sys_GetClipboardData wrapper
		cbd[0] = '\0';
	#else
		CL_GetClipboardData( cbd, sizeof(cbd) );
	#endif

	return cbd;
}

void InputField::Init( size_t inputLen, Text *_textProperties ) {
	assert( inputLen <= MAX_STRING_CHARS && "InputField::Init inputLen too large" XSTRING( MAX_STRING_CHARS ) );

	if ( _textProperties ) {
		textProperties = *_textProperties;
	}

	Clear();
}

void InputField::ClampHistory( void ) {
	const size_t historySize = history.size();
	if ( historyIndex > historySize ) {
		historyIndex = historySize;
	}
	// 0 clamp?
}

void InputField::Clear( void ) {
	buffer.clear();
	cursor = 0;
	scroll = 0;
}

void InputField::Delete( void ) {
	Clear();
}

// Handles prompt, cursor blinking, horizontal scrolling, units are in pixels
void InputField::Draw( int x, int y, int width, bool showCursor, const char *prompt, int promptColour ) {
	#ifndef BUILD_DEDICATED
		const size_t inputLen = buffer.length();
		const size_t prestep = 0, drawLen = inputLen;

		//TODO: make the scroll aware of proportional fonts
		//TODO: some kind of smart/bisecting string clipper to make as few strcat calls necessary to find a suitable length
		//		special case: linear strcat for very small strings
		// splice the visible part of the input string
		char str[MAX_STRING_CHARS];
		memcpy( str, buffer.c_str() + prestep, drawLen );
		str[drawLen] = '\0';

		// draw the prompt
		Text_Paint( textProperties, x, y, prompt, colorTable[promptColour] );
		x += Text_Width( textProperties, prompt );

		// draw the input string
		Text_Paint( textProperties, x, y, str );

		// draw the input cursor
		if ( showCursor && (Milliseconds() >> 8) & 1 ) {
			// work out the cursor position
			str[cursor] = '\0';
			x += Text_Width( textProperties, str );

			Text_Paint( textProperties, x, y, GetOverstrike() ? "_" : "|" );
		}
	#endif
}

const std::string &InputField::GetHistory( size_t index ) const {
	return history[history.size() - index];
}

const char *InputField::GetLine( void ) {
	return buffer.c_str();
}

const InputField::SelectionResult InputField::GetCurrentToken( void ) const {
	//FIXME: also quotes?
	const char *p1 = buffer.c_str() + cursor;
	while ( p1 > buffer.c_str() && p1[-1] != ' ' && p1[-1] != ';' ) {
		p1--;
	}

	const char *p2 = buffer.c_str() + cursor;
	while ( *p2 != '\0' && *p2 != ' ' && *p2 != ';' ) {
		p2++;
	}

	return SelectionResult{ p1, p2 };
}
// Performs the basic line editing functions for the console, in-game talk, and menu fields
// Key events are used for non-printable characters, others are gotten from char events.
void InputField::KeyDownEvent( int key ) {
	// shift-insert is paste
	if ( (key == A_INSERT || key == A_KP_0) && IsKeyDown( A_SHIFT ) ) {
		Paste();
		return;
	}

	key = tolower( key );
	const size_t inputLen = buffer.length();

	switch ( key ) {

	case A_DELETE: {
		if ( cursor != buffer.length() ) {
			buffer.erase( cursor, 1 );
		}
	} break;

	case A_CURSOR_RIGHT: {
		if ( cursor < inputLen ) {
			cursor++;
		}
	} break;

	case A_CURSOR_LEFT: {
		if ( cursor > 0 ) {
			cursor--;
		}
	} break;

	case A_HOME: {
		cursor = 0;
	} break;

	case A_END: {
		cursor = inputLen;
	} break;

	case A_INSERT: {
		SetOverstrike( !GetOverstrike() );
	} break;

	case A_CURSOR_UP: {
		if ( historySeeking ) {
			// if we're already seeking, it means history is not empty
			historyIndex++;
			ClampHistory();
		}
		else {
			// not already seeking, see if we should commit the current line before seeking
			if ( !buffer.empty() ) {
				history.push_back( buffer );
			}

			const size_t historySize = history.size();
			if ( historySize ) {
				historySeeking = true; // now we're seeking
				historyIndex++;
				ClampHistory();
			}
		}

		if ( historySeeking ) {
			Clear();
			buffer = GetHistory( historyIndex );
			// set the cursor pos to the end of the line
			cursor = buffer.length();
		}
	} break;

	case A_CURSOR_DOWN: {
		if ( historySeeking ) {
			// if we're already seeking, it means history is not empty
			historyIndex--;
			if ( historyIndex == 0u ) {
				historySeeking = false;
			}
			ClampHistory();
		}
		else {
			// not already seeking, see if we should commit the current line
			if ( !buffer.empty() ) {
				history.push_back( buffer );
			}
			Clear();
		}

		if ( historySeeking ) {
			buffer = GetHistory( historyIndex );
			// set the cursor pos to the end of the line
			cursor = buffer.length();
		}
		else {
			Clear();
		}
	} break;

	case A_ENTER:
	case A_KP_ENTER: {
		// commit the current line to history, pass it to the command buffer and print it to console

		// no action to take if it's empty...
		if ( buffer.empty() ) {
			return;
		}

		/* don't autocomplete
		if ( autoComplete ) {
			if ( const char *result = autoComplete( buffer.c_str() ) ) {
				buffer = result;
				cursor = buffer.length();
			}
		}
		*/

		// pass to command buffer
		if ( execute ) {
			execute( buffer.c_str() );
		}

		// commit to history
		history.push_back( buffer );

		// make sure history index is reset
		historyIndex = 0u;
		historySeeking = false;

		// clear the state
		Clear();
	} break;

	case A_TAB: {
		if ( !buffer.empty() && autoComplete ) {
			const auto selection = GetCurrentToken();
			std::string matchToken( selection.start, selection.end - selection.start );

			const auto result = autoComplete( matchToken.c_str() );
			if ( !result.empty() ) {
				// rebuild the input line with the current token swapped with the match
				const size_t tokenStartPos = selection.start - buffer.c_str();
				std::string finalStr = buffer.substr( 0, tokenStartPos );
				finalStr.append( result );
				finalStr.append( selection.end );
				buffer = finalStr;

				// move cursor to end of the replaced token
				cursor = tokenStartPos + result.length();
			}
		}
	} break;

	default: {
		// ...
	} break;

	}

	// Change scroll if cursor is no longer visible
	/*FIXME: redo this without knowing widthInChars
	if ( scroll >= cursor ) {
		scroll = cursor;
	}
	else if ( cursor >= scroll + widthInChars && cursor <= len ) {
		scroll = cursor - widthInChars + 1;
	}
	*/
}

void InputField::CharEvent( int ch ) {
	if ( ch == 'v' - 'a' + 1 ) {
		// ctrl-v is paste
		Paste();
		return;
	}

	if ( ch == 'c' - 'a' + 1 ) {
		// ctrl-c clears the field
		Clear();
		return;
	}

	const int inputLen = buffer.length();
	assert( inputLen < MAX_STRING_CHARS );
	assert( cursor >= 0 );
	assert( (int)cursor <= inputLen );

	if ( ch == 'h' - 'a' + 1 )	{
		// ctrl-h is backspace
		// remove the character before the cursor
		if ( cursor != 0 ) {
			buffer.erase( cursor - 1, 1 );
			cursor--;
		}
		return;
	}

	if ( ch == 1 ) {
		// ctrl-a is home
		cursor = 0;
		scroll = 0;
		return;
	}

	if ( ch == 'e' - 'a' + 1 ) {
		// ctrl-e is end
		cursor = inputLen;
		scroll = cursor;
		return;
	}

	// ignore any other non printable chars
	if ( ch < 32 ) {
		return;
	}

	if ( GetOverstrike() ) {
		// - 2 to leave room for trailing \0
		if ( cursor == MAX_STRING_CHARS - 1 ) {
			return;
		}
		buffer[cursor] = ch;
		cursor++;
	}
	else {
		// insert mode
		// - 2 to leave room for trailing \0
		if ( inputLen == MAX_STRING_CHARS - 1 ) {
			return; // all full
		}
		buffer.insert( cursor, 1, ch );
		cursor++;
	}

	/*FIXME: redo this without knowing widthInChars
	if ( cursor >= widthInChars ) {
		scroll++;
	}
	*/

	/*
	if ( (int)cursor == inputLen + 1) {
		buffer.resize( cursor );
	}
	*/
}



void InputField::Paste( void ) {
	// send as if typed, so insert / overstrike works properly
	for ( char *c = GetClipboardData(); *c; c++ ) {
		CharEvent( *c );
	}
}

void ShortestMatchFinder::Feed( const char *hay, const char *description ) {
	trimmed = false;

	// check if hay.startsWith( needle )
	if ( Q_stricmpn( hay, needle, needleLen ) ) {
		return;
	}

	matches.push_back( Match{ hay, description } );

	if ( matches.size() == 1 ) {
		shortestMatch = hay;
	}
}

void ShortestMatchFinder::TrimMatches( void ) {
	trimmed = true;

	if ( matches.size() <= 1 ) {
		// nothing to do
		return;
	}

	std::sort( matches.begin(), matches.end() );

	for ( size_t matchNum=1; matchNum < matches.size(); matchNum++ ) {
		const char *hay = matches[matchNum].string.c_str();
		// cut match to the amount common with s
		for ( int i = 0; hay[i]; i++ ) {
			if ( tolower( shortestMatch[i] ) != tolower( hay[i] ) ) {
				shortestMatch.resize( i );
				break;
			}
		}
	}
}

const std::string ShortestMatchFinder::GetShortestMatch( void ) {
	if ( !trimmed ) {
		TrimMatches();
	}

	return shortestMatch;
}

bool ShortestMatchFinder::IsComplete( void ) const {
	return matches.size() <= 1;
}

void ShortestMatchFinder::PrintMatches( void ) {
	if ( !trimmed ) {
		TrimMatches();
	}

	Com_Printf( "found %i matches...\n", matches.size() );
	for ( const auto &match : matches ) {
		Com_Printf( "  - %s\n", match.description.c_str() );
	}
}
