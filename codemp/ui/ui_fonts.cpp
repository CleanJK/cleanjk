#include <algorithm>

#if defined(UI_BUILD)
	#include "ui/ui_local.h"
#elif defined(_CGAME)
	#include "cgame/cg_local.h"
#endif
#include "ui/ui_shared.h"
#include "qcommon/q_files.h"	// for STYLE_BLINK etc
#include "ui/ui_fonts.h"

#if defined(_CGAME)
	extern displayContextDef_t cgDC;
#endif

static qhandle_t GetRealHandle( int iMenuFont ) {
#if defined(UI_BUILD)
	const cachedAssets_t &assets = uiInfo.uiDC.Assets;
#elif defined(_CGAME)
	const cachedAssets_t &assets = cgDC.Assets;
#endif

	switch ( iMenuFont ) {

	case FONT_SMALL: {
		return assets.qhSmallFont;
	} break;

	default:
	case FONT_MEDIUM: {
		return assets.qhMediumFont;
	} break;

	case FONT_LARGE: {
		return assets.qhBigFont;
	} break;

	case FONT_SMALL2: {
		return assets.qhSmall2Font;
	} break;

	}
}

Font::Font( int fontNum, float scale, int alignment, bool isCustomFont )
	: menuFont( fontNum ), scale( scale ), alignment( alignment ), customFont( isCustomFont )
{
}

float Font::Width( const char *text ) const {
	qhandle_t realHandle = customFont ? menuFont : GetRealHandle( menuFont );

	return cjk_preciseFonts.integer
		? trap->ext.R_Font_StrLenPixels( text, realHandle, scale )
		: trap->R_Font_StrLenPixels( text, realHandle, scale );
}

float Font::Height( const char *text ) const {
	qhandle_t realHandle = customFont ? menuFont : GetRealHandle( menuFont );

	//TODO: trap->ext.***
	return trap->R_Font_HeightPixels( realHandle, scale );
}

void Font::Paint( float x, float y, const char *text, const vec4_t colour, int style, int limit, float adjust, int cursorPos, bool overstrike ) const {
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
	switch ( alignment ) {

		default:
		case ITEM_ALIGN_LEFT: {
		} break;

		case ITEM_ALIGN_CENTER: {
			x -= (Width( text ) / 2.0f);
		} break;

		case ITEM_ALIGN_RIGHT: {
			x -= Width( text );
		} break;

	}

	//TODO: styles (blink, pulse)

	trap->R_Font_DrawString(
		x, y, text, colour,
		iStyleOR | (customFont ? menuFont : GetRealHandle( menuFont )),
		limit ? limit : -1, // -1 = no limit
		scale
	);

	if ( cursorPos != -1 ) {
		// now print the cursor as well...
		char sTemp[1024];
		int copyCount = limit
			? std::min( (signed)strlen( text ), limit )
			: (signed)strlen( text );
		copyCount = std::min( copyCount, cursorPos );
		copyCount = std::min( static_cast<size_t>( copyCount ), sizeof(sTemp) );

		// copy text into temp buffer for pixel measure...
		strncpy( sTemp, text, copyCount );
		sTemp[copyCount] = '\0';

		const char *cursorStr = overstrike ? "|" : "_";
		float nextPosX = Width( cursorStr );

		trap->R_Font_DrawString(
			x + nextPosX, y, text, colour,
			iStyleOR | (customFont ? menuFont : GetRealHandle( menuFont )),
			limit ? limit : -1, // -1 = no limit
			scale
		);

		Paint( x + nextPosX, y, cursorStr, colour, style | ITEM_TEXTSTYLE_BLINK, limit );
	}

	trap->R_SetColor( nullptr );
}

void Text_Paint_Limit( float *maxX, float x, float y, float scale, const vec4_t color, const char *text, float adjust, int limit, int fontNum, bool customFont ) {
	bool bIsTrailingPunctuation;

	//float fMax = *maxX;
	const Font font( fontNum, scale, customFont );
	float iPixelLen = font.Width( text );
	if ( x + iPixelLen > *maxX ) {
		// whole text won't fit, so we need to print just the amount that does...
		//  Ok, this is slow and tacky, but only called occasionally, and it works...
		char sTemp[4096] = { 0 };	// lazy assumption
		const char *psText = text;
		char *psOut = &sTemp[0];
		char *psOutLastGood = psOut;
		unsigned int uiLetter;

		while ( *psText && (x + font.Width( sTemp ) <= *maxX)
			&& psOut < &sTemp[sizeof(sTemp)-1] ) {
			int iAdvanceCount;
			psOutLastGood = psOut;

			uiLetter = trap->R_AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, &bIsTrailingPunctuation );
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
		font.Paint( x, y, sTemp, color, ITEM_TEXTSTYLE_NORMAL, limit, adjust );
	}
	else {
		// whole text fits fine, so print it all...
		*maxX = x + iPixelLen;	// feedback the next position, as the caller expects
		font.Paint( x, y, text, color, ITEM_TEXTSTYLE_NORMAL, limit, adjust );
	}
}

// having all these different style defines (1 for UI, one for CG, and now one for the re->font stuff) is dumb, but for now...
void Text_Paint_Proportional( int x, int y, const char *str, int uiStyle, const vec4_t color, int fontNum, float scale ) {
	Font font( fontNum, scale );

	switch ( uiStyle ) {
		default:
		case UI_LEFT: {
		} break;

		case UI_CENTER: {
			x -= font.Width( str ) / 2;
		} break;

		case UI_RIGHT: {
			x -= font.Width( str ) / 2;
		} break;
	}

	uint32_t styleBits = 0u;
	if ( uiStyle & UI_DROPSHADOW ) {
		styleBits = ITEM_TEXTSTYLE_SHADOWED;
	}
	else if ( uiStyle & (UI_BLINK|UI_PULSE) ) {
		styleBits = ITEM_TEXTSTYLE_BLINK;
	}

	font.Paint( x, y, str, color, styleBits );
}
