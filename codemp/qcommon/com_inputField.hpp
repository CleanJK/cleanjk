#pragma once

#include <string>
#include <vector>

#include "qcommon/q_common.hpp"

class InputField {

public:
	using ExecuteFunc = void (*)( const char *text );
	using AutoCompleteFunc = const std::string (*)( const char *token );
	struct SelectionResult {
		const char *start, *end;
	};

private:
	AutoCompleteFunc          autoComplete;
	uint32_t                  cursor;
	ExecuteFunc               execute;
	uint32_t                  scroll; // char the scroll was last set at (usually 0)

	//TODO:   built in persistence?

	const std::string &GetHistory  ( size_t index ) const Q_WARN_UNUSED_RESULT; // retrieve the string at the given history index (reverse 1-indexed)
	void               ClampHistory( void ); // make sure the history index does not result in an overrun/underrun
	void               Paste       ( void );

public:
	std::string               buffer;
	std::vector<std::string>  history;
	size_t                    historyIndex; // reverse indexed
	bool                      historySeeking;
	Text                      textProperties;

	Q_NO_DEFAULT_CTOR( InputField )
	InputField(
		ExecuteFunc executeFunction,
		AutoCompleteFunc autoCompleteFunction
	)
	: autoComplete( autoCompleteFunction ),
	  cursor( 0 ),
	  execute( executeFunction ),
	  scroll( 0 ),
	  buffer( "" ),
	  historyIndex( 0u ),
	  historySeeking( false ),
	  textProperties{ JKFont::Medium, 1.0f }
	{
		// ...
	}
	~InputField() {}

	void                  CharEvent      ( int ch );
	void                  Clear          ( void );
	void                  Delete         ( void );
	void                  Draw           ( int x, int y, int width, bool showCursor, const char *prompt = nullptr, int promptColour = CT_MDGREY );
	const char           *GetLine        ( void ) Q_WARN_UNUSED_RESULT; // get a pointer to the current input field string
	const SelectionResult GetCurrentToken( void ) const Q_WARN_UNUSED_RESULT;
	void                  Init           ( size_t inputLen = MAX_STRING_CHARS, Text *textProperties = nullptr );
	void                  KeyDownEvent   ( int key );

private:

};

struct ShortestMatchFinder {
	struct Match {
		std::string string;
		std::string description;

		bool operator<( const Match &rhs ) const {
			return string < rhs.string;
		}
	};

private:
	std::vector<Match>  matches;
	const char         *needle;
	size_t              needleLen;
	std::string         shortestMatch;
	bool                trimmed;

public:
	Q_NO_DEFAULT_CTOR( ShortestMatchFinder )
	ShortestMatchFinder( const char *needle ) : needle( needle ), needleLen( strlen( needle ) ) {}

	void              Feed            ( const char *hay, const char *description );
	const std::string GetShortestMatch( void ) Q_WARN_UNUSED_RESULT;
	bool              IsComplete      ( void ) const Q_WARN_UNUSED_RESULT;
	void              PrintMatches    ( void );
	void              TrimMatches     ( void );
};
