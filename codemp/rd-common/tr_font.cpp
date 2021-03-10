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

#include "qcommon/sstring.hpp"	// stl string class won't compile in here (MS shite), so use Gil's.
#include "rd-common/tr_font.hpp"
#include "tr_local.hpp"
#include "tr_cvars.hpp"
#include "qcommon/stringed_ingame.hpp"
#include "qcommon/q_math.hpp"
#include "qcommon/com_cvars.hpp"

#define sFILENAME_THAI_WIDTHS "fonts/tha_widths.dat"
#define sFILENAME_THAI_CODES  "fonts/tha_codes.dat"

enum class CachedLanguage : uint32_t {
	None = 0,
	Russian,
	Polish,
	Korean,
	Taiwanese,
	Japanese,
	Chinese,
	Thai,
};

static CachedLanguage _GetCachedLanguage( void ) {
	static int modificationCount = -1234;  // any old silly value that won't match the cvar mod count
	static CachedLanguage cachedLanguage = CachedLanguage::None;

	// only re-compare string when it's changed
	if ( modificationCount != se_language->modificationCount ) {
		modificationCount = se_language->modificationCount;

			 if ( !strcmp( se_language->string, "russian" ) )   { cachedLanguage = CachedLanguage::Russian; }
		else if ( !strcmp( se_language->string, "polish" ) )    { cachedLanguage = CachedLanguage::Polish; }
		else if ( !strcmp( se_language->string, "korean" ) )    { cachedLanguage = CachedLanguage::Korean; }
		else if ( !strcmp( se_language->string, "taiwanese" ) ) { cachedLanguage = CachedLanguage::Taiwanese; }
		else if ( !strcmp( se_language->string, "japanese" ) )  { cachedLanguage = CachedLanguage::Japanese; }
		else if ( !strcmp( se_language->string, "chinese" ) )   { cachedLanguage = CachedLanguage::Chinese; }
		else if ( !strcmp( se_language->string, "thai" ) )      { cachedLanguage = CachedLanguage::Thai; }
		else                                                    { cachedLanguage = CachedLanguage::None; }
	}
	return cachedLanguage;
}

struct ThaiCodes_t {
	std::map<int, int> m_mapValidCodes;
	std::vector<int>   m_viGlyphWidths;
	sstring_t          m_strInitFailureReason; // so we don't have to keep retrying to work this out

	void Clear( void ) {
		m_mapValidCodes.clear();
		m_viGlyphWidths.clear();
		m_strInitFailureReason = ""; // if blank, never failed, else says don't bother re-trying
	}

	ThaiCodes_t() {
		Clear();
	}

	// convert a supplied 1,2 or 3-byte multiplied-up integer into a valid 0..n index, else -1...

	int GetValidIndex( int iCode ) {
		std::map<int, int>::iterator it = m_mapValidCodes.find( iCode );
		if ( it != m_mapValidCodes.end() ) {
			return (*it).second;
		}
		return -1;
	}

	int GetWidth( int iGlyphIndex ) {
		if ( iGlyphIndex < (int)m_viGlyphWidths.size() ) {
			return m_viGlyphWidths[iGlyphIndex];
		}

		assert( 0 );
		return 0;
	}

	// return is error message to display, or nullptr for success
	const char *Init( void ) {
		if ( m_mapValidCodes.empty() && m_viGlyphWidths.empty() ) {
			if ( m_strInitFailureReason.empty() ) {
				// never tried and failed already?
				int *piData = nullptr;	// note <int>, not <byte>, for []-access

				// read the valid-codes table in...
				int iBytesRead = ri.FS_ReadFile( sFILENAME_THAI_CODES, (void **) &piData );
				if ( iBytesRead > 0 && !(iBytesRead & 3) ) {
					// valid length and multiple of 4 bytes long
					int iTableEntries = iBytesRead / sizeof(int);

					for ( int i = 0; i < iTableEntries; i++ ) {
						m_mapValidCodes[piData[i]] = i; // convert MBCS code to sequential index...
					}
					ri.FS_FreeFile( piData );	// dispose of original

					// now read in the widths... (I'll keep these in a simple STL vector, so they'all disappear when the <map> entries do...

					iBytesRead = ri.FS_ReadFile( sFILENAME_THAI_WIDTHS, (void **) &piData );
					if ( iBytesRead > 0 && !(iBytesRead & 3) && iBytesRead >> 2/*sizeof(int)*/ == iTableEntries ) {
						for ( int i = 0; i < iTableEntries; i++ ) {
							m_viGlyphWidths.push_back( piData[i] );
						}
						ri.FS_FreeFile( piData );	// dispose of original
					}
					else {
						m_strInitFailureReason = va( "Error with file \"%s\", size = %d!\n", sFILENAME_THAI_WIDTHS, iBytesRead );
					}
				}
				else {
					m_strInitFailureReason = va( "Error with file \"%s\", size = %d!\n", sFILENAME_THAI_CODES, iBytesRead );
				}
			}
		}

		return m_strInitFailureReason.c_str();
	}
};

#define GLYPH_MAX_KOREAN_SHADERS	3
#define GLYPH_MAX_TAIWANESE_SHADERS 4
#define GLYPH_MAX_JAPANESE_SHADERS	3
#define GLYPH_MAX_CHINESE_SHADERS	3
#define GLYPH_MAX_THAI_SHADERS		3
#define GLYPH_MAX_ASIAN_SHADERS		4	// this MUST equal the larger of the above defines

class CFontInfo {
private:
	// From the fontdat file
	glyphInfo_t  mGlyphs[GLYPH_COUNT];
	int          mShader; // handle to the shader with the glyph
	int          m_hAsianShaders[GLYPH_MAX_ASIAN_SHADERS]; // shaders for Korean glyphs where applicable
	glyphInfo_t  m_AsianGlyph; // special glyph containing asian->western scaling info for all glyphs
	int          m_iAsianGlyphsAcross; // needed to dynamically calculate S,T coords
	int          m_iAsianPagesLoaded;
	bool         m_bAsianLastPageHalfHeight;
	int          m_iLanguageModificationCount; // doesn't matter what this is, so long as it's comparable as being changed
	ThaiCodes_t *m_pThaiData;

public:
	char   m_sFontName[MAX_QPATH]; // eg "fonts/lcd"	// needed for korean font-hint if we need >1 hangul set
	int    mPointSize;
	int    mHeight;
	int    mAscender;
	int    mDescender;
	bool   mbRoundCalcs;
	JKFont m_iThisFont; // handle to itself
	float  m_fAltSBCSFontScaleFactor; // -1, else amount to adjust returned values by to make them fit the master western font they're substituting for

	 CFontInfo( const char *fontName );
	~CFontInfo( void ) {}

	const glyphInfo_t *GetLetter            ( const unsigned int uiLetter, int *piShader = nullptr );
	const int          GetCollapsedAsianCode( ulong uiLetter ) const;
	const int          GetLetterWidth       ( const unsigned int uiLetter );
	const int          GetLetterHorizAdvance( const unsigned int uiLetter );
	void               UpdateAsianIfNeeded  ( bool bForceReEval = false );

	bool AsianGlyphsAvailable( void ) const {
		return !!m_hAsianShaders[0];
	}
	void FlagNoAsianGlyphs( void ) {
		// used during constructor
		m_hAsianShaders[0] = 0;
		m_iLanguageModificationCount = -1;
	}
	const int GetAscender( void ) const {
		return mAscender;
	}
	const int GetDescender( void ) const {
		return mDescender;
	}
	const int GetHeight( void ) const {
		return mHeight;
	}
	const int GetPointSize( void ) const {
		return mPointSize;
	}
	const int GetShader( void ) const {
		return mShader;
	}
};

// round float to one decimal place...
float RoundTenth( float fValue ) {
	return (floorf( (fValue * 10.0f) + 0.5f )) / 10.0f;
}

int g_iCurrentFontIndex; // entry 0 is reserved index for missing/invalid, else ++ with each new font registered
std::vector<CFontInfo *> g_vFontArray;
typedef std::map<sstring_t, JKFont> FontIndexMap_t;
FontIndexMap_t g_mapFontIndexes;
int g_iNonScaledCharRange; // this is used with auto-scaling of asian fonts, anything below this number is preserved in scale, anything above is scaled down by 0.75f

// some korean stuff

#define KSC5601_HANGUL_HIBYTE_START		0xB0	// range is...
#define KSC5601_HANGUL_HIBYTE_STOP		0xC8	// ... inclusive
#define KSC5601_HANGUL_LOBYTE_LOBOUND	0xA0	// range is...
#define KSC5601_HANGUL_LOBYTE_HIBOUND	0xFF	// ...bounding (ie only valid in between these points, but NULLs in charsets for these codes)
#define KSC5601_HANGUL_CODES_PER_ROW	96		// 2 more than the number of glyphs

static inline bool Korean_ValidKSC5601Hangul( byte _iHi, byte _iLo ) {
	return
		_iHi >= KSC5601_HANGUL_HIBYTE_START &&
		_iHi <= KSC5601_HANGUL_HIBYTE_STOP &&
		_iLo >  KSC5601_HANGUL_LOBYTE_LOBOUND &&
		_iLo <  KSC5601_HANGUL_LOBYTE_HIBOUND;
}

static inline bool Korean_ValidKSC5601Hangul( unsigned int uiCode ) {
	return Korean_ValidKSC5601Hangul( uiCode >> 8, uiCode & 0xFF );
}

// takes a KSC5601 double-byte hangul code and collapses down to a 0..n glyph index...
// Assumes rows are 96 wide (glyph slots), not 94 wide (actual glyphs), so I can ignore boundary markers
// (invalid hangul codes will return 0)
static int Korean_CollapseKSC5601HangulCode( unsigned int uiCode ) {
	if ( Korean_ValidKSC5601Hangul( uiCode ) ) {
		uiCode -= (KSC5601_HANGUL_HIBYTE_START * 256) + KSC5601_HANGUL_LOBYTE_LOBOUND;	// sneaky maths on both bytes, reduce to 0x0000 onwards
		uiCode  = ((uiCode >> 8) * KSC5601_HANGUL_CODES_PER_ROW) + (uiCode & 0xFF);
		return uiCode;
	}
	return 0;
}

static int Korean_InitFields( int &iGlyphTPs, const char *&psLang ) {
	psLang = "kor";
	iGlyphTPs = GLYPH_MAX_KOREAN_SHADERS;
	g_iNonScaledCharRange = 255;
	return 32; // m_iAsianGlyphsAcross
}

// some taiwanese stuff

// (all ranges inclusive for Big5)...

#define BIG5_HIBYTE_START0		0xA1	// (misc chars + level 1 hanzi)
#define BIG5_HIBYTE_STOP0		0xC6	//
#define BIG5_HIBYTE_START1		0xC9	// (level 2 hanzi)
#define BIG5_HIBYTE_STOP1		0xF9	//
#define BIG5_LOBYTE_LOBOUND0	0x40	//
#define BIG5_LOBYTE_HIBOUND0	0x7E	//
#define BIG5_LOBYTE_LOBOUND1	0xA1	//
#define BIG5_LOBYTE_HIBOUND1	0xFE	//
#define BIG5_CODES_PER_ROW		160		// 3 more than the number of glyphs

static bool Taiwanese_ValidBig5Code( unsigned int uiCode ) {
	const byte _iHi = (uiCode >> 8) & 0xFF;
	if ( (_iHi >= BIG5_HIBYTE_START0 && _iHi <= BIG5_HIBYTE_STOP0) ||
		 (_iHi >= BIG5_HIBYTE_START1 && _iHi <= BIG5_HIBYTE_STOP1) )
	{
		const byte _iLo = uiCode & 0xFF;

		if ( (_iLo >= BIG5_LOBYTE_LOBOUND0 && _iLo <= BIG5_LOBYTE_HIBOUND0) ||
			 (_iLo >= BIG5_LOBYTE_LOBOUND1 && _iLo <= BIG5_LOBYTE_HIBOUND1) )
		{
			return true;
		}
	}

	return false;
}

// only call this when Taiwanese_ValidBig5Code() has already returned true...

static bool Taiwanese_IsTrailingPunctuation( unsigned int uiCode ) {
	// so far I'm just counting the first 21 chars, those seem to be all the basic punctuation...

	if ( uiCode >= ( (BIG5_HIBYTE_START0 << 8) | BIG5_LOBYTE_LOBOUND0) &&
		 uiCode <  (((BIG5_HIBYTE_START0 << 8) | BIG5_LOBYTE_LOBOUND0) + 20) )
	{
		return true;
	}

	return false;
}

// takes a BIG5 double-byte code (including level 2 hanzi) and collapses down to a 0..n glyph index...
// Assumes rows are 160 wide (glyph slots), not 157 wide (actual glyphs), so I can ignore boundary markers
// (invalid big5 codes will return 0)
static int Taiwanese_CollapseBig5Code( unsigned int uiCode ) {
	if ( Taiwanese_ValidBig5Code( uiCode ) ) {
		uiCode -= (BIG5_HIBYTE_START0 * 256) + BIG5_LOBYTE_LOBOUND0;	// sneaky maths on both bytes, reduce to 0x0000 onwards
		if ( (uiCode & 0xFF) >= (BIG5_LOBYTE_LOBOUND1 - 1) - BIG5_LOBYTE_LOBOUND0 ) {
			uiCode -= ((BIG5_LOBYTE_LOBOUND1 - 1) - (BIG5_LOBYTE_HIBOUND0 + 1)) -1;
		}
		uiCode = ((uiCode >> 8) * BIG5_CODES_PER_ROW) + (uiCode & 0xFF);
		return uiCode;
	}
	return 0;
}

static int Taiwanese_InitFields( int &iGlyphTPs, const char *&psLang ) {
	psLang = "tai";
	iGlyphTPs = GLYPH_MAX_TAIWANESE_SHADERS;
	g_iNonScaledCharRange = 255;
	return 64; // m_iAsianGlyphsAcross
}

// some Japanese stuff

// ( all ranges inclusive for Shift-JIS )

#define SHIFTJIS_HIBYTE_START0	0x81
#define SHIFTJIS_HIBYTE_STOP0	0x9F
#define SHIFTJIS_HIBYTE_START1	0xE0
#define SHIFTJIS_HIBYTE_STOP1	0xEF

#define SHIFTJIS_LOBYTE_START0	0x40
#define SHIFTJIS_LOBYTE_STOP0	0x7E
#define SHIFTJIS_LOBYTE_START1	0x80
#define SHIFTJIS_LOBYTE_STOP1	0xFC
#define SHIFTJIS_CODES_PER_ROW (                             \
	(                                                        \
		(SHIFTJIS_LOBYTE_STOP0 - SHIFTJIS_LOBYTE_START0) + 1 \
	) + (                                                    \
		(SHIFTJIS_LOBYTE_STOP1 - SHIFTJIS_LOBYTE_START1) + 1 \
	)                                                        \
)

static bool Japanese_ValidShiftJISCode( byte _iHi, byte _iLo ) {
	if ( (_iHi >= SHIFTJIS_HIBYTE_START0 && _iHi <= SHIFTJIS_HIBYTE_STOP0) ||
		 (_iHi >= SHIFTJIS_HIBYTE_START1 && _iHi <= SHIFTJIS_HIBYTE_STOP1) )
	{
		if ( (_iLo >= SHIFTJIS_LOBYTE_START0 && _iLo <= SHIFTJIS_LOBYTE_STOP0) ||
			 (_iLo >= SHIFTJIS_LOBYTE_START1 && _iLo <= SHIFTJIS_LOBYTE_STOP1)
			)
		{
			return true;
		}
	}

	return false;
}

static inline bool Japanese_ValidShiftJISCode( unsigned int uiCode ) {
	return Japanese_ValidShiftJISCode( uiCode >> 8, uiCode & 0xFF );
}

// only call this when Japanese_ValidShiftJISCode() has already returned true...

static bool Japanese_IsTrailingPunctuation( unsigned int uiCode ) {
	// so far I'm just counting the first 18 chars, those seem to be all the basic punctuation...

	if ( uiCode >= ( (SHIFTJIS_HIBYTE_START0 << 8) | SHIFTJIS_LOBYTE_START0) &&
		 uiCode <  (((SHIFTJIS_HIBYTE_START0 << 8) | SHIFTJIS_LOBYTE_START0) + 18 ) )
	{
		return true;
	}

	return false;
}

// takes a ShiftJIS double-byte code and collapse down to a 0..n glyph index...
// (invalid codes will return 0)
static int Japanese_CollapseShiftJISCode( unsigned int uiCode ) {
	if ( Japanese_ValidShiftJISCode( uiCode ) ) {
		uiCode -= ((SHIFTJIS_HIBYTE_START0 << 8) | SHIFTJIS_LOBYTE_START0); // sneaky maths on both bytes, reduce to 0x0000 onwards

		if ( (uiCode & 0xFF) >= (SHIFTJIS_LOBYTE_START1) - SHIFTJIS_LOBYTE_START0 ) {
			uiCode -= ((SHIFTJIS_LOBYTE_START1) - SHIFTJIS_LOBYTE_STOP0) - 1;
		}

		if ( ((uiCode >> 8) & 0xFF) >= (SHIFTJIS_HIBYTE_START1) - SHIFTJIS_HIBYTE_START0 ) {
			uiCode -= (((SHIFTJIS_HIBYTE_START1) - SHIFTJIS_HIBYTE_STOP0) - 1) << 8;
		}

		uiCode = ((uiCode >> 8) * SHIFTJIS_CODES_PER_ROW) + (uiCode & 0xFF);

		return uiCode;
	}
	return 0;
}

static int Japanese_InitFields( int &iGlyphTPs, const char *&psLang ) {
	psLang = "jap";
	iGlyphTPs = GLYPH_MAX_JAPANESE_SHADERS;
	g_iNonScaledCharRange = 255;
	return 64; // m_iAsianGlyphsAcross
}

// some Chinese stuff

#define GB_HIBYTE_START		0xA1	// range is...
#define GB_HIBYTE_STOP		0xF7	// ... inclusive
#define GB_LOBYTE_LOBOUND	0xA0	// range is...
#define GB_LOBYTE_HIBOUND	0xFF	// ...bounding (ie only valid in between these points, but NULLs in charsets for these codes)
#define GB_CODES_PER_ROW	95		// 1 more than the number of glyphs

static inline bool Chinese_ValidGBCode( byte _iHi, byte _iLo ) {
	return
		_iHi >= GB_HIBYTE_START &&
		_iHi <= GB_HIBYTE_STOP &&
		_iLo >  GB_LOBYTE_LOBOUND &&
		_iLo <  GB_LOBYTE_HIBOUND;
}

static inline bool Chinese_ValidGBCode( unsigned int uiCode ) {
	return Chinese_ValidGBCode( uiCode >> 8, uiCode & 0xFF );
}

// only call this when Chinese_ValidGBCode() has already returned true...

static bool Chinese_IsTrailingPunctuation( unsigned int uiCode ) {
	// so far I'm just counting the first 13 chars, those seem to be all the basic punctuation...

	if ( uiCode > ( (GB_HIBYTE_START << 8) | GB_LOBYTE_LOBOUND) &&
		 uiCode < (((GB_HIBYTE_START << 8) | GB_LOBYTE_LOBOUND) + 14) )
	{
		return true;
	}

	return false;
}

// takes a GB double-byte code and collapses down to a 0..n glyph index...
// Assumes rows are 96 wide (glyph slots), not 94 wide (actual glyphs), so I can ignore boundary markers
// (invalid GB codes will return 0)
static int Chinese_CollapseGBCode( unsigned int uiCode ) {
	if ( Chinese_ValidGBCode( uiCode ) ) {
		uiCode -= (GB_HIBYTE_START * 256) + GB_LOBYTE_LOBOUND;	// sneaky maths on both bytes, reduce to 0x0000 onwards
		uiCode  = ((uiCode >> 8) * GB_CODES_PER_ROW) + (uiCode & 0xFF);
		return uiCode;
	}

	return 0;
}

static int Chinese_InitFields( int &iGlyphTPs, const char *&psLang ) {
	psLang = "chi";
	iGlyphTPs = GLYPH_MAX_CHINESE_SHADERS;
	g_iNonScaledCharRange = 255;
	return 64; // m_iAsianGlyphsAcross
}

// some Thai stuff

//TIS 620-2533

#define TIS_GLYPHS_START	160
#define TIS_SARA_AM			0xD3		// special case letter, both a new letter and a trailing accent for the prev one
ThaiCodes_t g_ThaiCodes;	// the one and only instance of this object

// returns a valid Thai code (or 0), based on taking 1,2 or 3 bytes from the supplied byte stream
//	Fills in <iThaiBytes> with 1,2 or 3
static int Thai_ValidTISCode( const byte *psString, int &iThaiBytes ) {
	// try a 1-byte code first...

	if (psString[0] >= 160)	// so western letters drop through and use normal font
	{
		// this code is heavily little-endian, so someone else will need to port for Mac etc... (not my problem ;-)

		union CodeToTry_t
		{
            char sChars[4];
			unsigned int uiCode;
		};

		CodeToTry_t CodeToTry;
		CodeToTry.uiCode = 0;	// important that we clear all 4 bytes in sChars here

		// thai codes can be up to 3 bytes long, so see how high we can get...

		int i;
		for (i=0; i<3; i++)
		{
			CodeToTry.sChars[i] = psString[i];

            int iIndex = g_ThaiCodes.GetValidIndex( CodeToTry.uiCode );
			if (iIndex == -1)
			{
				// failed, so return previous-longest code...

				CodeToTry.sChars[i] = 0;
				break;
			}
		}
		iThaiBytes = i;
		assert(i);	// if 'i' was 0, then this may be an error, trying to get a thai accent as standalone char?
		return CodeToTry.uiCode;
	}

	return 0;
}

// special case, thai can only break on certain letters, and since the rules are complicated then
//	we tell the translators to put an underscore ('_') between each word even though in Thai they're
//	all jammed together at final output onscreen...

static inline bool Thai_IsTrailingPunctuation( unsigned int uiCode )
{
	return uiCode == '_';
}

// takes a TIS 1,2 or 3 byte code and collapse down to a 0..n glyph index...
// (invalid codes will return 0)
static int Thai_CollapseTISCode( unsigned int uiCode )
{
	if (uiCode >= TIS_GLYPHS_START)	// so western letters drop through as invalid
	{
		int iCollapsedIndex = g_ThaiCodes.GetValidIndex( uiCode );
		if (iCollapsedIndex != -1)
		{
			return iCollapsedIndex;
		}
	}

	return 0;
}

static int Thai_InitFields(int &iGlyphTPs, const char *&psLang)
{
	psLang		= "tha";
	iGlyphTPs	= GLYPH_MAX_THAI_SHADERS;
	g_iNonScaledCharRange = INT_MAX;	// in other words, don't scale any thai chars down
	return 32;	// m_iAsianGlyphsAcross
}

// takes char *, returns integer char at that point, and advances char * on by enough bytes to move past the letter
//	(either western 1 byte or Asian multi-byte)...
// looks messy, but the actual execution route is quite short, so it's fast...
// Note that I have to have this 3-param form instead of advancing a passed-in "const char **psText" because of VM-crap
//	where you can only change ptr-contents, not ptrs themselves. Bleurgh.
// Ditto the true:false crap instead of just returning stuff straight through.
unsigned int AnyLanguage_ReadCharFromString( const char *psText, int *piAdvanceCount, bool *pbIsTrailingPunctuation /* = nullptr */)
{
	const byte *psString = (const byte *) psText;	// avoid sign-promote bug
	unsigned int uiLetter;

	switch ( _GetCachedLanguage() ) {
		case CachedLanguage::Korean: {
			if ( Korean_ValidKSC5601Hangul( psString[0], psString[1] ) ) {
				uiLetter = (psString[0] * 256) + psString[1];
				*piAdvanceCount = 2;

				// not going to bother testing for korean punctuation here, since korean already
				//	uses spaces, and I don't have the punctuation glyphs defined, only the basic 2350 hanguls

				if ( pbIsTrailingPunctuation ) {
					*pbIsTrailingPunctuation = false;
				}

				return uiLetter;
			}
		} break;

		case CachedLanguage::Taiwanese: {
			if ( Taiwanese_ValidBig5Code( (psString[0] * 256) + psString[1] ) ) {
				uiLetter = (psString[0] * 256) + psString[1];
				*piAdvanceCount = 2;

				// need to ask if this is a trailing (ie like a comma or full-stop) punctuation?...

				if ( pbIsTrailingPunctuation ) {
					*pbIsTrailingPunctuation = !!Taiwanese_IsTrailingPunctuation( uiLetter );
				}

				return uiLetter;
			}
		} break;

		case CachedLanguage::Japanese: {
			if ( Japanese_ValidShiftJISCode( psString[0], psString[1] ) ) {
				uiLetter = (psString[0] * 256) + psString[1];
				*piAdvanceCount = 2;

				// need to ask if this is a trailing (ie like a comma or full-stop) punctuation?...

				if ( pbIsTrailingPunctuation)
				{
					*pbIsTrailingPunctuation = Japanese_IsTrailingPunctuation( uiLetter ) ? true : false;
				}

				return uiLetter;
			}
		} break;

		case CachedLanguage::Chinese: {
			if ( Chinese_ValidGBCode( (psString[0] * 256) + psString[1] ))
			{
				uiLetter = (psString[0] * 256) + psString[1];
				*piAdvanceCount = 2;

				// need to ask if this is a trailing (ie like a comma or full-stop) punctuation?...

				if ( pbIsTrailingPunctuation)
				{
					*pbIsTrailingPunctuation = Chinese_IsTrailingPunctuation( uiLetter ) ? true : false;
				}

				return uiLetter;
			}
		} break;

		case CachedLanguage::Thai: {
			int iThaiBytes;
			uiLetter = Thai_ValidTISCode( psString, iThaiBytes );
			if ( uiLetter )
			{
				*piAdvanceCount = iThaiBytes;

				if ( pbIsTrailingPunctuation )
				{
					*pbIsTrailingPunctuation = Thai_IsTrailingPunctuation( uiLetter ) ? true : false;
				}

				return uiLetter;
			}
		}
		break;

		default:
		break;
	}

	// ... must not have been an MBCS code...

	uiLetter = psString[0];
	*piAdvanceCount = 1;

	if (pbIsTrailingPunctuation)
	{
		*pbIsTrailingPunctuation = (uiLetter == '!' ||
									uiLetter == '?' ||
									uiLetter == ',' ||
									uiLetter == '.' ||
									uiLetter == ';' ||
									uiLetter == ':'
									) ? true : false;
	}

	return uiLetter;
}

// needed for subtitle printing since original code no longer worked once camera bar height was changed to 480/10
//	rather than refdef height / 10. I now need to bodge the coords to come out right.
bool Language_IsAsian( void ) {
	switch ( _GetCachedLanguage() ) {
		case CachedLanguage::Korean:
		case CachedLanguage::Taiwanese:
		case CachedLanguage::Japanese:
		case CachedLanguage::Chinese:
		case CachedLanguage::Thai: {
			// this is asian, but the query is normally used for scaling
			return true;
		}
		default: {
			break;
		}
	}

	return false;
}

bool Language_UsesSpaces( void ) {
	// ( korean uses spaces )
	switch ( _GetCachedLanguage() ) {
		case CachedLanguage::Taiwanese:
		case CachedLanguage::Japanese:
		case CachedLanguage::Chinese:
		case CachedLanguage::Thai: {
			return false;
		}
		default: {
			break;
		}
	}

	return true;
}

// name is (eg) "ergo" or "lcd", no extension.
// If path present, it's a special language hack for SBCS override languages, eg: "lcd/russian", which means just treat
//	the file as "russian", but with the "lcd" part ensuring we don't find a different registered russian font
CFontInfo::CFontInfo( const char *_fontName ) {
	int			len, i;
	void		*buff;
	dfontdat_t	*fontdat;

	// remove any special hack name insertions...

	char fontName[MAX_QPATH];
	sprintf( fontName, "fonts/%s.fontdat", COM_SkipPath( const_cast<char*>( _fontName ) ) );

	// clear some general things...

	m_pThaiData = nullptr;
	m_iThisFont = JKFont::Invalid;
	m_fAltSBCSFontScaleFactor = -1;

	len = ri.FS_ReadFile( fontName, nullptr );
	if ( len == sizeof(dfontdat_t) ) {
		ri.FS_ReadFile( fontName, &buff );
		fontdat = (dfontdat_t *)buff;

		for ( i=0; i<GLYPH_COUNT; i++ ) {
#ifdef Q3_BIG_ENDIAN
			mGlyphs[i].width = LittleShort( fontdat->mGlyphs[i].width );
			mGlyphs[i].height = LittleShort( fontdat->mGlyphs[i].height );
			mGlyphs[i].horizAdvance = LittleShort( fontdat->mGlyphs[i].horizAdvance );
			mGlyphs[i].horizOffset = LittleShort( fontdat->mGlyphs[i].horizOffset );
			mGlyphs[i].baseline = LittleLong( fontdat->mGlyphs[i].baseline );
			mGlyphs[i].s = LittleFloat( fontdat->mGlyphs[i].s );
			mGlyphs[i].t = LittleFloat( fontdat->mGlyphs[i].t );
			mGlyphs[i].s2 = LittleFloat( fontdat->mGlyphs[i].s2 );
			mGlyphs[i].t2 = LittleFloat( fontdat->mGlyphs[i].t2 );
#else
			mGlyphs[i] = fontdat->mGlyphs[i];
#endif
		}
		mPointSize = LittleShort( fontdat->mPointSize );
		mHeight = LittleShort( fontdat->mHeight );
		mAscender = LittleShort( fontdat->mAscender );
		mDescender = LittleShort( fontdat->mDescender );
//		mAsianHack = LittleShort( fontdat->mKoreanHack );	// ignore this crap, it's some junk in the fontdat file that no-one uses
		mbRoundCalcs = false /*!!strstr( fontName, "ergo" )*/;

		// cope with bad fontdat headers...

		if ( mHeight == 0 ) {
			mHeight = mPointSize;
            mAscender = mPointSize - Round( ((float)mPointSize / 10.0f) + 2 ); // have to completely guess at the baseline... sigh.
            mDescender = mHeight - mAscender;
		}

		ri.FS_FreeFile( buff );
	}
	else {
		mHeight = 0;
		mShader = 0;
	}

	Q_strncpyz( m_sFontName, fontName, sizeof(m_sFontName) );
	COM_StripExtension( m_sFontName, m_sFontName, sizeof(m_sFontName) );	// so we get better error printing if failed to load shader (ie lose ".fontdat")
	mShader = RE_RegisterShaderNoMip( m_sFontName );

	FlagNoAsianGlyphs();
	UpdateAsianIfNeeded( true );

	// finished...
	g_vFontArray.resize( g_iCurrentFontIndex + 1 );
	g_vFontArray[g_iCurrentFontIndex++] = this;
}

void CFontInfo::UpdateAsianIfNeeded( bool bForceReEval ) {
	// if asian language, then provide an alternative glyph set and fill in relevant fields...

	if ( mHeight ) {
		// western charset exists in first place, and isn't alien rubbish?
		CachedLanguage cachedLanguage = _GetCachedLanguage();

		if ( cachedLanguage == CachedLanguage::Korean ||
			 cachedLanguage == CachedLanguage::Taiwanese ||
			 cachedLanguage == CachedLanguage::Japanese ||
			 cachedLanguage == CachedLanguage::Chinese ||
			 cachedLanguage == CachedLanguage::Thai )
		{
			int iCappedHeight = mHeight < 16 ? 16: mHeight;	// arbitrary limit on small char sizes because Asian chars don't squash well

			if ( m_iLanguageModificationCount != se_language->modificationCount || !AsianGlyphsAvailable() || bForceReEval ) {
				m_iLanguageModificationCount  = se_language->modificationCount;

				int iGlyphTPs = 0;
				const char *psLang = nullptr;

				switch ( cachedLanguage ) {
					case CachedLanguage::Korean:    { m_iAsianGlyphsAcross = Korean_InitFields( iGlyphTPs, psLang );    } break;
					case CachedLanguage::Taiwanese: { m_iAsianGlyphsAcross = Taiwanese_InitFields( iGlyphTPs, psLang ); } break;
					case CachedLanguage::Japanese:  { m_iAsianGlyphsAcross = Japanese_InitFields( iGlyphTPs, psLang );  } break;
					case CachedLanguage::Chinese:   { m_iAsianGlyphsAcross = Chinese_InitFields( iGlyphTPs, psLang );   } break;
					case CachedLanguage::Thai: {
						m_iAsianGlyphsAcross = Thai_InitFields( iGlyphTPs, psLang );

						if ( !m_pThaiData ) {
							const char *psFailureReason = g_ThaiCodes.Init();
							if ( !psFailureReason[0] ) {
								m_pThaiData = &g_ThaiCodes;
							}
							else {
								// failed to load a needed file, reset to English...
								ri.Cvar_Set( "se_language", "english" );
								Com_Error( ERR_DROP, psFailureReason );
							}
						}
					} break;

					default: {
					} break;
				}

				// textures need loading...

				if ( m_sFontName[0] ) {
					// Use this sometime if we need to do logic to load alternate-height glyphs to better fit other fonts.
					// (but for now, we just use the one glyph set)
				}

				for ( int i = 0; i < iGlyphTPs; i++ ) {
					// (Note!!  assumption for S,T calculations: all Asian glyph textures pages are square except for last one)

					char sTemp[MAX_QPATH];
					Com_sprintf( sTemp, sizeof(sTemp), "fonts/%s_%d_1024_%d", psLang, 1024/m_iAsianGlyphsAcross, i );

					// returning 0 here will automatically inhibit Asian glyph calculations at runtime...

					m_hAsianShaders[i] = RE_RegisterShaderNoMip( sTemp );
				}

				// for now I'm hardwiring these, but if we ever have more than one glyph set per language then they'll be changed...

				m_iAsianPagesLoaded = iGlyphTPs;	// not necessarily true, but will be safe, and show up obvious if something missing
				m_bAsianLastPageHalfHeight = true;

				bForceReEval = true;
			}

			if ( bForceReEval ) {
				// now init the Asian member glyph fields to make them come out the same size as the western ones
				//	that they serve as an alternative for...

				m_AsianGlyph.width			= iCappedHeight;	// square Asian chars same size as height of western set
				m_AsianGlyph.height			= iCappedHeight;	// ""
				switch ( cachedLanguage ) {
					default:                       { m_AsianGlyph.horizAdvance = iCappedHeight;     } break;
					case CachedLanguage::Korean:   { m_AsianGlyph.horizAdvance = iCappedHeight - 1; } break; // korean has a small amount of space at the edge of the glyph
					case CachedLanguage::Taiwanese:
					case CachedLanguage::Japanese:
					case CachedLanguage::Chinese:  { m_AsianGlyph.horizAdvance = iCappedHeight + 3; } // need to force some spacing for these
				//	case CachedLanguage::Thai: // this is done dynamically elsewhere, since Thai glyphs are variable width
				}
				m_AsianGlyph.horizOffset = 0;				// ""
				m_AsianGlyph.baseline    = mAscender + ((iCappedHeight - mHeight) >> 1);
			}
		}
		else {
			// not using Asian...
			FlagNoAsianGlyphs();
		}
	}
	else {
		// no western glyphs available, so don't attempt to match asian...
		FlagNoAsianGlyphs();
	}
}

static CFontInfo *GetFont_Actual( JKFont font ) {
	int index = static_cast<int>( font );
	index &= SET_MASK;
	if ( index >= 1 && index < g_iCurrentFontIndex ) {
		CFontInfo *pFont = g_vFontArray[index];
		if ( pFont ) {
			pFont->UpdateAsianIfNeeded();
		}

		return pFont;
	}
	return nullptr;
}

// needed to add *piShader param because of multiple TPs,
//	if not passed in, then I also skip S,T calculations for re-usable static asian glyphinfo struct...

const glyphInfo_t *CFontInfo::GetLetter( const unsigned int uiLetter, int *piShader /* = nullptr */ ) {
	if ( AsianGlyphsAvailable() ) {
		int iCollapsedAsianCode = GetCollapsedAsianCode( uiLetter );
		if ( iCollapsedAsianCode ) {
			if ( piShader ) {
				// (Note!!  assumption for S,T calculations: all asian glyph textures pages are square except for last one
				//			which may or may not be half height) - but not for Thai

				int iTexturePageIndex = iCollapsedAsianCode / (m_iAsianGlyphsAcross * m_iAsianGlyphsAcross);
				if ( iTexturePageIndex > m_iAsianPagesLoaded ) {
					assert(0);				// should never happen
					iTexturePageIndex = 0;
				}

				int iOriginalCollapsedAsianCode = iCollapsedAsianCode;	// need to back this up (if Thai) for later
				iCollapsedAsianCode -= iTexturePageIndex *  (m_iAsianGlyphsAcross * m_iAsianGlyphsAcross);

				const int iColumn	= iCollapsedAsianCode % m_iAsianGlyphsAcross;
				const int iRow		= iCollapsedAsianCode / m_iAsianGlyphsAcross;
				const bool bHalfT	= (iTexturePageIndex == (m_iAsianPagesLoaded - 1) && m_bAsianLastPageHalfHeight);
				const int iAsianGlyphsDown = (bHalfT) ? m_iAsianGlyphsAcross / 2 : m_iAsianGlyphsAcross;

				switch ( _GetCachedLanguage() ) {

				case CachedLanguage::Korean:
				default: {
					m_AsianGlyph.s  = (float)( iColumn    ) / (float)m_iAsianGlyphsAcross;
					m_AsianGlyph.t  = (float)( iRow       ) / (float)  iAsianGlyphsDown;
					m_AsianGlyph.s2 = (float)( iColumn + 1) / (float)m_iAsianGlyphsAcross;
					m_AsianGlyph.t2 = (float)( iRow + 1   ) / (float)  iAsianGlyphsDown;
				} break;

				case CachedLanguage::Taiwanese: {
					m_AsianGlyph.s  = (float)(((1024 / m_iAsianGlyphsAcross) * ( iColumn    ))+1) / 1024.0f;
					m_AsianGlyph.t  = (float)(((1024 / iAsianGlyphsDown    ) * ( iRow       ))+1) / 1024.0f;
					m_AsianGlyph.s2 = (float)(((1024 / m_iAsianGlyphsAcross) * ( iColumn+1  ))  ) / 1024.0f;
					m_AsianGlyph.t2 = (float)(((1024 / iAsianGlyphsDown    ) * ( iRow+1     ))  ) / 1024.0f;
				} break;

				case CachedLanguage::Japanese:
				case CachedLanguage::Chinese: {
					m_AsianGlyph.s  = (float)(((1024 / m_iAsianGlyphsAcross) * ( iColumn    ))  ) / 1024.0f;
					m_AsianGlyph.t  = (float)(((1024 / iAsianGlyphsDown    ) * ( iRow       ))  ) / 1024.0f;
					m_AsianGlyph.s2 = (float)(((1024 / m_iAsianGlyphsAcross) * ( iColumn+1  ))-1) / 1024.0f;
					m_AsianGlyph.t2 = (float)(((1024 / iAsianGlyphsDown    ) * ( iRow+1     ))-1) / 1024.0f;
				} break;

				case CachedLanguage::Thai: {
					int iGlyphXpos = (1024 / m_iAsianGlyphsAcross) * ( iColumn );
					int iGlyphWidth = g_ThaiCodes.GetWidth( iOriginalCollapsedAsianCode );

					// very thai-specific language-code...

					if (uiLetter == TIS_SARA_AM)
					{
						iGlyphXpos += 9;	// these are pixel coords on the source TP, so don't affect scaled output
						iGlyphWidth= 20;	//
					}
					m_AsianGlyph.s  = (float)(iGlyphXpos) / 1024.0f;
					m_AsianGlyph.t  = (float)(((1024 / iAsianGlyphsDown    ) * ( iRow       ))  ) / 1024.0f;
					// technically this .s2 line should be modified to blit only the correct width, but since
					//	all Thai glyphs are up against the left edge of their cells and have blank to the cell
					//	boundary then it's better to keep these calculations simpler...

					m_AsianGlyph.s2 = (float)(iGlyphXpos+iGlyphWidth) / 1024.0f;
					m_AsianGlyph.t2 = (float)(((1024 / iAsianGlyphsDown    ) * ( iRow+1     ))-1) / 1024.0f;

					// special addition for Thai, need to bodge up the width and advance fields...

					m_AsianGlyph.width = iGlyphWidth;
					m_AsianGlyph.horizAdvance = iGlyphWidth + 1;
				} break;

				}

				*piShader = m_hAsianShaders[ iTexturePageIndex ];
			}
			return &m_AsianGlyph;
		}
	}

	if ( piShader ) {
		*piShader = GetShader();
	}

	const glyphInfo_t *pGlyph = &mGlyphs[ uiLetter & 0xff ];

	// SBCS language substitution?...

	if ( m_fAltSBCSFontScaleFactor != -1 ) {
		// sod it, use the asian glyph, that's fine...

		memcpy(&m_AsianGlyph,pGlyph,sizeof(m_AsianGlyph));	// *before* changin pGlyph!

//		CFontInfo *pOriginalFont = GetFont_Actual( this->m_iOriginalFontWhenSBCSOverriden );
//		pGlyph = &pOriginalFont->mGlyphs[ uiLetter & 0xff ];

		#define ASSIGN_WITH_ROUNDING(_dst,_src) _dst = mbRoundCalcs ? Round( m_fAltSBCSFontScaleFactor * _src ) : m_fAltSBCSFontScaleFactor * (float)_src;

		ASSIGN_WITH_ROUNDING( m_AsianGlyph.baseline,	pGlyph->baseline );
		ASSIGN_WITH_ROUNDING( m_AsianGlyph.height,		pGlyph->height );
		ASSIGN_WITH_ROUNDING( m_AsianGlyph.horizAdvance,pGlyph->horizAdvance );
//		m_AsianGlyph.horizOffset	= /*Round*/( m_fAltSBCSFontScaleFactor * pGlyph->horizOffset );
		ASSIGN_WITH_ROUNDING( m_AsianGlyph.width,		pGlyph->width );

		pGlyph = &m_AsianGlyph;
	}

	return pGlyph;
}

const int CFontInfo::GetCollapsedAsianCode( ulong uiLetter ) const {
	int iCollapsedAsianCode = 0;

	if ( AsianGlyphsAvailable() ) {
		switch ( _GetCachedLanguage() ) {
		case CachedLanguage::Korean:    { iCollapsedAsianCode = Korean_CollapseKSC5601HangulCode( uiLetter ); } break;
		case CachedLanguage::Taiwanese: { iCollapsedAsianCode = Taiwanese_CollapseBig5Code( uiLetter );       } break;
		case CachedLanguage::Japanese:  { iCollapsedAsianCode = Japanese_CollapseShiftJISCode( uiLetter );    } break;
		case CachedLanguage::Chinese:   { iCollapsedAsianCode = Chinese_CollapseGBCode( uiLetter );           } break;
		case CachedLanguage::Thai:      { iCollapsedAsianCode = Thai_CollapseTISCode( uiLetter );             } break;
		default:                        { assert( 0 ); /* unhandled asian language */                         } break;
		}
	}

	return iCollapsedAsianCode;
}

const int CFontInfo::GetLetterWidth( unsigned int uiLetter ) {
	const glyphInfo_t *pGlyph = GetLetter( uiLetter );
	return pGlyph->width ? pGlyph->width : mGlyphs[(unsigned)'.'].width;
}

const int CFontInfo::GetLetterHorizAdvance( unsigned int uiLetter ) {
	const glyphInfo_t *pGlyph = GetLetter( uiLetter );
	return pGlyph->horizAdvance ? pGlyph->horizAdvance : mGlyphs[(unsigned)'.'].horizAdvance;
}

static CFontInfo *GetFont( JKFont index ) {
	switch ( _GetCachedLanguage() ) {
		// se_language overrides
		//TODO: override with parent font properties
		// newFont->m_fAltSBCSFontScaleFactor = RoundTenth( (float)oldFont->GetPointSize() / (float)newFont->GetPointSize() );
		// newFont->mPointSize = oldFont->GetPointSize();
		// newFont->mHeight    = oldFont->GetHeight();
		// newFont->mAscender  = oldFont->GetAscender();
		// newFont->mDescender = oldFont->GetDescender();
		case CachedLanguage::Polish:  { return GetFont_Actual( JKFont::Polish );  } break;
		case CachedLanguage::Russian: { return GetFont_Actual( JKFont::Russian ); } break;
		default:                      { return GetFont_Actual( index );           } break;
	}
}

float RE_Font_StrLenPixels( const char *psText, const JKFont font, const float fScale ) {
	CFontInfo *curfont = GetFont( font );
	if ( !curfont ) {
		return 0.0f;
	}

	float fScaleAsian = fScale;
	if ( Language_IsAsian() && fScale > 0.7f ) {
		fScaleAsian = fScale * 0.75f;
	}

	float maxLineWidth = 0.0f;
	float thisLineWidth = 0.0f;
	while ( *psText ) {
		int iAdvanceCount;
		unsigned int uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, nullptr );
		psText += iAdvanceCount;

		if ( uiLetter == '^' ) {
			if ( *psText >= '0' && *psText <= '9' ) {
				uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, nullptr );
				psText += iAdvanceCount;
				continue;
			}
		}

		if ( uiLetter == '\n' ) {
			thisLineWidth = 0.0f;
		}
		else {
			float iPixelAdvance = (float)curfont->GetLetterHorizAdvance( uiLetter );

			float fValue = iPixelAdvance * ((uiLetter > (unsigned)g_iNonScaledCharRange) ? fScaleAsian : fScale);

			if ( r_aspectCorrectFonts->integer == 1 ) {
				fValue *= ((float)(SCREEN_WIDTH * glConfig.vidHeight) / (float)(SCREEN_HEIGHT * glConfig.vidWidth));
			}
			else if ( r_aspectCorrectFonts->integer == 2 ) {
				fValue = ceilf(
					fValue * ((float)(SCREEN_WIDTH * glConfig.vidHeight) / (float)(SCREEN_HEIGHT * glConfig.vidWidth))
				);
			}
			thisLineWidth += curfont->mbRoundCalcs
				? roundf( fValue )
				: (r_aspectCorrectFonts->integer == 2)
					? ceilf( fValue )
					: fValue;
			if ( thisLineWidth > maxLineWidth ) {
				maxLineWidth = thisLineWidth;
			}
		}
	}
	//return (int)ceilf( maxLineWidth );
	return maxLineWidth;
}

// not really a font function, but keeps naming consistent...

int RE_Font_StrLenChars( const char *psText ) {
	// logic for this function's letter counting must be kept same in this function and RE_Font_DrawString()

	int iCharCount = 0;

	while ( *psText ) {
		// in other words, colour codes and CR/LF don't count as chars, all else does...

		int iAdvanceCount;
		unsigned int uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, nullptr );
		psText += iAdvanceCount;

		switch ( uiLetter ) {

		case '^': {
			// colour code (note next-char skip)
			if ( *psText >= '0' && *psText <= '9' ) {
				psText++;
			}
			else {
				iCharCount++;
			}
		} break;

		case 10: {
			// linefeed
		} break;

		case 13: {
			// return
		} break;

		case '_': {
			// special word-break hack
			iCharCount += (_GetCachedLanguage() == CachedLanguage::Thai && ((unsigned char *)psText)[0] >= TIS_GLYPHS_START) ? 0 : 1;
		} break;

		default: {
			iCharCount++;
		} break;

		}
	}

	return iCharCount;
}

float RE_Font_HeightPixels( const JKFont font, const float fScale ) {
	const CFontInfo *curfont = GetFont( font );
	if ( curfont ) {
		float fValue = curfont->GetPointSize() * fScale;
		return curfont->mbRoundCalcs ? Round( fValue ) : fValue;
	}
	return 0.0f;
}

// iMaxPixelWidth is -1 for "all of string", else pixel display count...

void RE_Font_DrawString( int ox, int oy, const char *psText, const float *rgba, const JKFont font, int iMaxPixelWidth, const float fScale ) {
	static bool gbInShadow = false; // MUST default to this
	qhandle_t hShader;

	assert( psText );

	const int iFont = static_cast<int>( font );

	if ( iFont & STYLE_BLINK ) {
		if ( (ri.Milliseconds() >> 7) & 1 ) {
			return;
		}
	}

	CFontInfo *curfont = GetFont( font );
	if ( !curfont || !psText ) {
		return;
	}

	float fScaleAsian = fScale;
	float fAsianYAdjust = 0.0f;
	if ( Language_IsAsian() && fScale > 0.7f ) {
		fScaleAsian = fScale * 0.75f;
		fAsianYAdjust = ((curfont->GetPointSize() * fScale) - (curfont->GetPointSize() * fScaleAsian)) / 2.0f;
	}

	// Draw a dropshadow if required
	if ( iFont & STYLE_DROPSHADOW ) {
		const int offset = Round( curfont->GetPointSize() * fScale * 0.075f );
		const vec4_t v4DKGREY2 = {0.15f, 0.15f, 0.15f, rgba?rgba[3]:1.0f};

		gbInShadow = true;
		RE_Font_DrawString( ox + offset, oy + offset, psText, v4DKGREY2, static_cast<JKFont>( iFont & SET_MASK ), iMaxPixelWidth, fScale );
		gbInShadow = false;
	}

	RE_SetColor( rgba );

	// Now we take off the training wheels and become a big font renderer
	// It's all floats from here on out
	const float fox = ox;
	float foy = oy;
	float fx = fox;

	foy += curfont->mbRoundCalcs ?
		Round( (curfont->GetHeight() - (curfont->GetDescender() >> 1)) * fScale ) :
		(curfont->GetHeight() - (curfont->GetDescender() >> 1)) * fScale;

	bool bNextTextWouldOverflow = false;
	const glyphInfo_t *pLetter;
	while ( *psText && !bNextTextWouldOverflow ) {
		int iAdvanceCount;
		unsigned int uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, nullptr );
		psText += iAdvanceCount;

		switch ( uiLetter ) {
		case 10: {
			//linefeed
			fx = fox;
			foy += curfont->mbRoundCalcs ? Round( curfont->GetPointSize() * fScale ) : curfont->GetPointSize() * fScale;
			if ( Language_IsAsian() ) {
				foy += 4.0f;	// this only comes into effect when playing in asian for "A long time ago in a galaxy" etc, all other text is line-broken in feeder functions
			}
		} break;

		case 13: {
			// Return
		} break;

		case 32: {
			// Space
			pLetter = curfont->GetLetter(' ');
			fx += curfont->mbRoundCalcs ? Round(pLetter->horizAdvance * fScale) : pLetter->horizAdvance * fScale;
			bNextTextWouldOverflow = ( iMaxPixelWidth != -1 && ((fx-fox) > (float)iMaxPixelWidth) ) ? true : false; // yeuch
		} break;

		case '_': {
			// has a special word-break usage if in Thai (and followed by a thai char), and should not be displayed, else treat as normal
			if ( _GetCachedLanguage() == CachedLanguage::Thai && ((unsigned char *)psText)[0] >= TIS_GLYPHS_START ) {
				break;
			}
		}
		// else fall through and display as normal...

		case '^': {
			// necessary because of fallthrough above
			if ( uiLetter != '_' ) {
				if ( *psText >= '0' && *psText <= '9' ) {
					const int colour = ColorIndex( *psText++ );
					if ( !gbInShadow ) {
						vec4_t color;
						Com_Memcpy( color, g_color_table[colour], sizeof(color) );
						color[3] = rgba ? rgba[3] : 1.0f;
						RE_SetColor( color );
					}
					break;
				}
			}
		}
		// purposely falls thrugh

		default: {
			pLetter = curfont->GetLetter( uiLetter, &hShader );			// Description of pLetter
			if( !pLetter->width ) {
				pLetter = curfont->GetLetter( '.' );
			}

			float fThisScale = uiLetter > (unsigned)g_iNonScaledCharRange ? fScaleAsian : fScale;

			// sigh, super-language-specific hack...

			if ( uiLetter == TIS_SARA_AM && _GetCachedLanguage() == CachedLanguage::Thai ) {
				fx -= curfont->mbRoundCalcs ? Round( 7.0f * fThisScale ) : 7.0f * fThisScale;
			}

			float fAdvancePixels = curfont->mbRoundCalcs ? Round( pLetter->horizAdvance * fThisScale ) : pLetter->horizAdvance * fThisScale;
			bNextTextWouldOverflow = !!(iMaxPixelWidth != -1 && (((fx + fAdvancePixels) - fox) > (float)iMaxPixelWidth)); // yeuch
			if ( !bNextTextWouldOverflow ) {
				// this 'mbRoundCalcs' stuff is crap, but the only way to make the font code work. Sigh...

				float fy = foy - (curfont->mbRoundCalcs ? Round( pLetter->baseline * fThisScale ) : pLetter->baseline * fThisScale);
				if ( curfont->m_fAltSBCSFontScaleFactor != -1 ) {
					fy += 3.0f; // I'm sick and tired of going round in circles trying to do this legally, so bollocks to it
				}

				RE_StretchPic(
					curfont->mbRoundCalcs ?
						fx + Round( pLetter->horizOffset * fThisScale ) :
						fx + pLetter->horizOffset * fThisScale,
					(uiLetter > (unsigned)g_iNonScaledCharRange) ? fy - fAsianYAdjust : fy,
					curfont->mbRoundCalcs ? Round( pLetter->width * fThisScale) : pLetter->width * fThisScale,
					curfont->mbRoundCalcs ? Round( pLetter->height * fThisScale) : pLetter->height * fThisScale,
					pLetter->s, pLetter->t, pLetter->s2, pLetter->t2, hShader
				);
				if ( r_aspectCorrectFonts->integer == 1 ) {
					fx += fAdvancePixels * ((float)(SCREEN_WIDTH * glConfig.vidHeight) / (float)(SCREEN_HEIGHT * glConfig.vidWidth));
				}
				else if ( r_aspectCorrectFonts->integer == 2 ) {
					fx += ceilf( fAdvancePixels * ((float)(SCREEN_WIDTH * glConfig.vidHeight) / (float)(SCREEN_HEIGHT * glConfig.vidWidth)) );
				}
				else {
					fx += fAdvancePixels;
				}
			}
		} break;

		}
	}
	//let it remember the old color
	//RE_SetColor( nullptr );
}

bool RE_RegisterFont( const JKFont font, const char *path ) {
	FontIndexMap_t::iterator it = g_mapFontIndexes.find( path );
	if ( it != g_mapFontIndexes.end() ) {
		JKFont found = (*it).second;
		ri.Printf( PRINT_DEVELOPER, "[debug:font] returning existing font %i for %i (%s)\n", static_cast<int>( found ), static_cast<int>( font ), path );
		return true;
	}

	// not registered, so...

	CFontInfo *pFont = new CFontInfo( path );
	if ( pFont->GetPointSize() > 0 ) {
		JKFont newFont = static_cast<JKFont>( g_iCurrentFontIndex - 1 );
		g_mapFontIndexes[path] = newFont;
		pFont->m_iThisFont = newFont;
		ri.Printf( PRINT_DEVELOPER, S_COLOR_YELLOW "[debug:font] registering new font %i for %s\n", static_cast<int>( newFont ), path );
		return true;
	}

	ri.Printf( PRINT_DEVELOPER, S_COLOR_RED "[debug:font] invalid font for %s\n", path );
	g_mapFontIndexes[path] = JKFont::Invalid; // missing/invalid
	return false;
}

void R_InitFonts( void ) {
	g_iCurrentFontIndex = 1;			// entry 0 is reserved for "missing/invalid"
	g_iNonScaledCharRange = INT_MAX;	// default all chars to have no special scaling (other than user supplied)
}

void R_FontList_f( void ) {
	Com_Printf( "------------------------------------\n" );

	FontIndexMap_t::iterator it;
	for ( const auto &it : g_mapFontIndexes ) {
		CFontInfo *font = GetFont( it.second );
		if ( font ) {
			Com_Printf(
				"%3i:%s  ps:%hi h:%hi a:%hi d:%hi\n",
				it.second, font->m_sFontName, font->mPointSize, font->mHeight, font->mAscender, font->mDescender
			);
		}
	}
	Com_Printf( "------------------------------------\n" );
}

void R_ShutdownFonts( void ) {
	for( int i = 1; i < g_iCurrentFontIndex; i++ ) {
		// entry 0 is reserved for "missing/invalid"
		delete g_vFontArray[i];
	}
	g_mapFontIndexes.clear();
	g_vFontArray.clear();
	g_iCurrentFontIndex = 1; // entry 0 is reserved for "missing/invalid"

	g_ThaiCodes.Clear();
}

// this is only really for debugging while tinkering with fonts, but harmless to leave in...

void R_ReloadFonts_f( void ) {
	// first, grab all the currently-registered fonts IN THE ORDER THEY WERE REGISTERED...
	std::vector <sstring_t> vstrFonts;

	int iFontToFind;
	for ( iFontToFind = 1; iFontToFind < g_iCurrentFontIndex; iFontToFind++ ) {
		FontIndexMap_t::iterator it;
		for ( it = g_mapFontIndexes.begin(); it != g_mapFontIndexes.end(); ++it ) {
			if ( static_cast<JKFont>( iFontToFind ) == (*it).second ) {
				vstrFonts.push_back( (*it).first );
				break;
			}
		}
		if ( it == g_mapFontIndexes.end() ) {
			break; // couldn't find this font
		}
	}
	if ( iFontToFind == g_iCurrentFontIndex ) {
		// found all of them?
		// now restart the font system...

		R_ShutdownFonts();
		R_InitFonts();

		// and re-register our fonts in the same order as before (note that some menu items etc cache the string lengths so really a vid_restart is better, but this is just for my testing)

		for ( size_t iFont = 0; iFont < vstrFonts.size(); iFont++ ) {
			RE_RegisterFont( static_cast<JKFont>( iFont ), vstrFonts[iFont].c_str() );
		}
		Com_Printf( "Done.\n" );
	}
	else {
		Com_Printf( "Problem encountered finding current fonts, ignoring.\n" );	// poo. Oh well, forget it.
	}
}
