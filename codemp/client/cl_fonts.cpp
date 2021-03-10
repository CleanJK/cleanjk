#include <algorithm>

#include "qcommon/com_cvars.hpp"
#include "client/cl_local.hpp"
#include "client/cl_fonts.hpp"

float Text_Width( const Text &text, const char *str ) {
	return re->Font_StrLenPixels( str, text.font, text.scale );
}

float Text_Height( const Text &text, const char *str ) {
	return re->Font_HeightPixels( text.font, text.scale );
}

void Text_Paint( const Text &text, float x, float y, const char *str, const vec4_t colour, int style, int limit, float adjust, int cursorPos, bool overstrike ) {
	// style
	uint32_t iStyleOR = 0u;
	switch ( style ) {

	default:
	case ITEM_TEXTSTYLE_NORMAL: {
		// normal text
		iStyleOR = 0u;
	} break;

	case ITEM_TEXTSTYLE_BLINK: // fast blinking
	case ITEM_TEXTSTYLE_PULSE: { // slow pulsing
		iStyleOR = STYLE_BLINK;
	} break;

	case ITEM_TEXTSTYLE_SHADOWED: // drop shadow
	case ITEM_TEXTSTYLE_OUTLINED: // drop shadow
	case ITEM_TEXTSTYLE_OUTLINESHADOWED: // drop shadow
	case ITEM_TEXTSTYLE_SHADOWEDMORE: { // drop shadow
		iStyleOR = STYLE_DROPSHADOW;
	} break;

	}

	// alignment
	//TODO: merge UI_CENTER for sanity
	switch ( text.alignment ) {

		default:
		case ITEM_ALIGN_LEFT: {
		} break;

		case ITEM_ALIGN_CENTER: {
			x -= (Text_Width( text, str ) / 2.0f);
		} break;

		case ITEM_ALIGN_RIGHT: {
			x -= Text_Width( text, str );
		} break;

	}

	//TODO: styles (blink, pulse)

	const JKFont fontAndStyle = static_cast<JKFont>( iStyleOR | static_cast<uint32_t>( text.font ) ); // this is silly
	re->Font_DrawString( x, y, str, colour, fontAndStyle, limit ? limit : -1, text.scale );

	if ( cursorPos != -1 ) {
		// now print the cursor as well...
		char sTemp[1024];
		int copyCount = limit
			? std::min( (signed)strlen( str ), limit )
			: (signed)strlen( str );
		copyCount = std::min( copyCount, cursorPos );
		copyCount = std::min( static_cast<size_t>( copyCount ), sizeof(sTemp) );

		// copy text into temp buffer for pixel measure...
		strncpy( sTemp, str, copyCount );
		sTemp[copyCount] = '\0';

		const char *cursorStr = overstrike ? "|" : "_";
		float nextPosX = Text_Width( text, cursorStr );

		re->Font_DrawString( x + nextPosX, y, str, colour, fontAndStyle, limit ? limit : -1, text.scale );
		Text_Paint( text, x + nextPosX, y, cursorStr, colour, style | ITEM_TEXTSTYLE_BLINK, limit );
	}

	re->SetColor( nullptr );
}

// standalone

void TextHelper_Paint_Limit( float *maxX, float x, float y, float scale, const vec4_t color, const char *str, float adjust, int limit, JKFont font ) {
	bool bIsTrailingPunctuation;

	//float fMax = *maxX;
	const Text text{ font, scale };
	float iPixelLen = Text_Width( text, str );
	if ( x + iPixelLen > *maxX ) {
		// whole text won't fit, so we need to print just the amount that does...
		//  Ok, this is slow and tacky, but only called occasionally, and it works...
		char sTemp[4096] = { 0 };	// lazy assumption
		const char *psText = str;
		char *psOut = &sTemp[0];
		char *psOutLastGood = psOut;

		while ( *psText && (x + Text_Width( text, sTemp ) <= *maxX)
			&& psOut < &sTemp[sizeof(sTemp)-1] )
		{
			int iAdvanceCount;
			psOutLastGood = psOut;

			const unsigned int uiLetter = re->AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, &bIsTrailingPunctuation );
			psText += iAdvanceCount;

			if ( uiLetter > 255 ) {
				*psOut++ = uiLetter >> 8;
				*psOut++ = uiLetter & 0xFF;
			}
			else
				*psOut++ = uiLetter & 0xFF;
		}
		*psOutLastGood = '\0';

		*maxX = 0; // feedback
		Text_Paint( text, x, y, sTemp, color, ITEM_TEXTSTYLE_NORMAL, limit, adjust );
	}
	else {
		// whole text fits fine, so print it all...
		*maxX = x + iPixelLen;	// feedback the next position, as the caller expects
		Text_Paint( text, x, y, str, color, ITEM_TEXTSTYLE_NORMAL, limit, adjust );
	}
}

// having all these different style defines (1 for UI, one for CG, and now one for the re->font stuff) is dumb, but for now...
void TextHelper_Paint_Proportional( int x, int y, const char *str, int uiStyle, const vec4_t color, JKFont font, float scale ) {
	Text text{ font, scale };

	switch ( uiStyle ) {
		default:
		case UI_LEFT: {
		} break;

		case UI_CENTER: {
			x -= Text_Width( text, str ) / 2;
		} break;

		case UI_RIGHT: {
			x -= Text_Width( text, str ) / 2;
		} break;
	}

	uint32_t styleBits = 0u;
	if ( uiStyle & UI_DROPSHADOW ) {
		styleBits = ITEM_TEXTSTYLE_SHADOWED;
	}
	else if ( uiStyle & (UI_BLINK|UI_PULSE) ) {
		styleBits = ITEM_TEXTSTYLE_BLINK;
	}

	Text_Paint( text, x, y, str, color, styleBits );
}
