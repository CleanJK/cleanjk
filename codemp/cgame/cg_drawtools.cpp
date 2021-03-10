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

// cg_drawtools.c -- helper functions called by cg_draw, cg_scoreboard, cg_info, etc
#include "cgame/cg_local.hpp"
#include "qcommon/q_shared.hpp"
#include "ui/menudef.h"
#include "cgame/cg_media.hpp"
#include "client/cl_fonts.hpp"

// Coordinates are 640*480 virtual values
void CG_DrawRect( float x, float y, float width, float height, float size, const float *color ) {
	trap->R_SetColor( color );

	CG_DrawTopBottom(x, y, width, height, size);
	CG_DrawSides(x, y, width, height, size);

	trap->R_SetColor( nullptr );
}

void CG_GetColorForHealth( int health, int armor, vec4_t hcolor ) {
	int		count;
	int		max;

	// calculate the total points of damage that can
	// be sustained at the current health / armor level
	if ( health <= 0 ) {
		VectorClear( hcolor );	// black
		hcolor[3] = 1;
		return;
	}
	count = armor;
	max = health * ARMOR_PROTECTION / ( 1.0 - ARMOR_PROTECTION );
	if ( max < count ) {
		count = max;
	}
	health += count;

	// set the color based on health
	hcolor[0] = 1.0;
	hcolor[3] = 1.0;
	if ( health >= 100 ) {
		hcolor[2] = 1.0;
	} else if ( health < 66 ) {
		hcolor[2] = 0;
	} else {
		hcolor[2] = ( health - 66 ) / 33.0;
	}

	if ( health > 60 ) {
		hcolor[1] = 1.0;
	} else if ( health < 30 ) {
		hcolor[1] = 0;
	} else {
		hcolor[1] = ( health - 30 ) / 30.0;
	}
}

// Coords are virtual 640x480
void CG_DrawSides(float x, float y, float w, float h, float size) {
	size *= cgs.screenXScale;
	trap->R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, media.gfx.misc.white );
	trap->R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, media.gfx.misc.white );
}

void CG_DrawTopBottom(float x, float y, float w, float h, float size) {
	size *= cgs.screenYScale;
	trap->R_DrawStretchPic( x, y, w, size, 0, 0, 0, 0, media.gfx.misc.white );
	trap->R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, media.gfx.misc.white );
}

// real coords
void CG_FillRect2( float x, float y, float width, float height, const float *color ) {
	trap->R_SetColor( color );
	trap->R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, media.gfx.misc.white);
	trap->R_SetColor( nullptr );
}

// Coordinates are 640*480 virtual values
void CG_FillRect( float x, float y, float width, float height, const float *color ) {
	trap->R_SetColor( color );
	trap->R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, media.gfx.misc.white);
	trap->R_SetColor( nullptr );
}

// Coordinates are 640*480 virtual values
// A width of 0 will draw with the original image width
void CG_DrawPic( float x, float y, float width, float height, qhandle_t hShader ) {
	trap->R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}

// Coordinates are 640*480 virtual values
// A width of 0 will draw with the original image width
// rotates around the upper right corner of the passed in point
void CG_DrawRotatePic( float x, float y, float width, float height,float angle, qhandle_t hShader ) {
	trap->R_DrawRotatePic( x, y, width, height, 0, 0, 1, 1, angle, hShader );
}

// Coordinates are 640*480 virtual values
// A width of 0 will draw with the original image width
// Actually rotates around the center point of the passed in coordinates
void CG_DrawRotatePic2( float x, float y, float width, float height,float angle, qhandle_t hShader ) {
	trap->R_DrawRotatePic2( x, y, width, height, 0, 0, 1, 1, angle, hShader );
}

// Coordinates and size in 640*480 virtual screen size
void CG_DrawChar( int x, int y, int width, int height, int ch ) {
	int row, col;
	float frow, fcol;
	float size;
	float	ax, ay, aw, ah;
	float size2;

	ch &= 255;

	if ( ch == ' ' ) {
		return;
	}

	ax = x;
	ay = y;
	aw = width;
	ah = height;

	row = ch>>4;
	col = ch&15;

	frow = row*0.0625;
	fcol = col*0.0625;
	size = 0.03125;
	size2 = 0.0625;

	trap->R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + size, frow + size2, media.gfx.interface.charset );

}

// Draws a multi-colored string with a drop shadow, optionally forcing to a fixed color.
// Coordinates are at 640 by 480 virtual resolution
void CG_DrawStringExt( int x, int y, const char *string, const float *setColor, bool forceColor, bool shadow, int charWidth, int charHeight, int maxChars )
{
	if (trap->R_Language_IsAsian())
	{
		// hack-a-doodle-do (post-release quick fix code)...

		vec4_t color;
		memcpy(color,setColor, sizeof(color));	// de-const it
		Text text{ JKFont::Medium, 1.0f };
		Text_Paint( text, x, y, string, color, shadow ? ITEM_TEXTSTYLE_SHADOWED : 0 );
	}
	else
	{
		vec4_t		color;
		const char	*s;
		int			xx;

		// draw the drop shadow
		if (shadow) {
			color[0] = color[1] = color[2] = 0;
			color[3] = setColor[3];
			trap->R_SetColor( color );
			s = string;
			xx = x;
			while ( *s ) {
				if ( Q_IsColorString( s ) ) {
					s += 2;
					continue;
				}
				CG_DrawChar( xx + 2, y + 2, charWidth, charHeight, *s );
				xx += charWidth;
				s++;
			}
		}

		// draw the colored text
		s = string;
		xx = x;
		trap->R_SetColor( setColor );
		while ( *s ) {
			if ( Q_IsColorString( s ) ) {
				if ( !forceColor ) {
					memcpy( color, g_color_table[ColorIndex(*(s+1))], sizeof( color ) );
					color[3] = setColor[3];
					trap->R_SetColor( color );
				}
				s += 2;
				continue;
			}
			CG_DrawChar( xx, y, charWidth, charHeight, *s );
			xx += charWidth;
			s++;
		}
		trap->R_SetColor( nullptr );
	}
}

void CG_DrawBigString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawStringExt( x, y, s, color, false, true, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
}

void CG_DrawBigStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawStringExt( x, y, s, color, true, true, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
}

void CG_DrawSmallString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawStringExt( x, y, s, color, false, false, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
}

void CG_DrawSmallStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawStringExt( x, y, s, color, true, false, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
}

// Returns character count, skiping color escape codes
int CG_DrawStrlen( const char *str ) {
	const char *s = str;
	int count = 0;

	while ( *s ) {
		if ( Q_IsColorString( s ) ) {
			s += 2;
		} else {
			count++;
			s++;
		}
	}

	return count;
}

// This repeats a 64*64 tile graphic to fill the screen around a sized down refresh window.
static void CG_TileClearBox( int x, int y, int w, int h, qhandle_t hShader ) {
	float	s1, t1, s2, t2;

	s1 = x/64.0;
	t1 = y/64.0;
	s2 = (x+w)/64.0;
	t2 = (y+h)/64.0;
	trap->R_DrawStretchPic( x, y, w, h, s1, t1, s2, t2, hShader );
}

// Clear around a sized down screen
void CG_TileClear( void ) {
	int		top, bottom, left, right;
	int		w, h;

	w = cgs.glconfig.vidWidth;
	h = cgs.glconfig.vidHeight;

	if ( cg.refdef.x == 0 && cg.refdef.y == 0 &&
		cg.refdef.width == w && cg.refdef.height == h ) {
		return;		// full screen rendering
	}

	top = cg.refdef.y;
	bottom = top + cg.refdef.height-1;
	left = cg.refdef.x;
	right = left + cg.refdef.width-1;

	// clear above view screen
	CG_TileClearBox( 0, 0, w, top, media.gfx.null );

	// clear below view screen
	CG_TileClearBox( 0, bottom, w, h - bottom, media.gfx.null );

	// clear left of view screen
	CG_TileClearBox( 0, top, left, bottom - top + 1, media.gfx.null );

	// clear right of view screen
	CG_TileClearBox( right, top, w - right, bottom - top + 1, media.gfx.null );
}

float *CG_FadeColor( int startMsec, int totalMsec ) {
	static vec4_t		color;
	int			t;

	if ( startMsec == 0 ) {
		return nullptr;
	}

	t = cg.time - startMsec;

	if ( t >= totalMsec ) {
		return nullptr;
	}

	// this color shouldn't be visible yet
	if (t < 0){
		return nullptr;
	}

	// fade out
	if ( totalMsec - t < FADE_TIME ) {
		color[3] = ( totalMsec - t ) * 1.0/FADE_TIME;
	} else {
		color[3] = 1.0;
	}
	color[0] = color[1] = color[2] = 1;

	return color;
}

void CG_ColorForGivenHealth( vec4_t hcolor, int health )
{
	// set the color based on health
	hcolor[0] = 1.0;
	if ( health >= 100 )
	{
		hcolor[2] = 1.0;
	}
	else if ( health < 66 )
	{
		hcolor[2] = 0;
	}
	else
	{
		hcolor[2] = ( health - 66 ) / 33.0;
	}

	if ( health > 60 )
	{
		hcolor[1] = 1.0;
	}
	else if ( health < 30 )
	{
		hcolor[1] = 0;
	}
	else
	{
		hcolor[1] = ( health - 30 ) / 30.0;
	}
}

void CG_ColorForHealth( vec4_t hcolor )
{
	int		health;
	int		count;
	int		max;

	// calculate the total points of damage that can
	// be sustained at the current health / armor level
	health = cg.snap->ps.stats[STAT_HEALTH];

	if ( health <= 0 )
	{
		VectorClear( hcolor );	// black
		hcolor[3] = 1;
		return;
	}

	count = cg.snap->ps.stats[STAT_ARMOR];
	max = health * ARMOR_PROTECTION / ( 1.0 - ARMOR_PROTECTION );
	if ( max < count )
	{
		count = max;
	}
	health += count;

	hcolor[3] = 1.0;
	CG_ColorForGivenHealth( hcolor, health );
}

// Take x,y positions as if 640 x 480 and scales them to the proper resolution
void CG_DrawNumField (int x, int y, int width, int value,int charWidth,int charHeight,int style,bool zeroFill)
{
	char	num[16], *ptr;
	int		l;
	int		frame;
	int		xWidth;
	int		i = 0;

	if (width < 1) {
		return;
	}

	// draw number string
	if (width > 5) {
		width = 5;
	}

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;

	// FIXME: Might need to do something different for the chunky font??
	switch(style)
	{
	case NUM_FONT_SMALL:
		xWidth = charWidth;
		break;
	case NUM_FONT_CHUNKY:
		xWidth = (charWidth/1.2f) + 2;
		break;
	default:
	case NUM_FONT_BIG:
		xWidth = (charWidth/2) + 7;//(charWidth/6);
		break;
	}

	if ( zeroFill )
	{
		for (i = 0; i < (width - l); i++ )
		{
			switch(style)
			{
			case NUM_FONT_SMALL:
				CG_DrawPic( x,y, charWidth, charHeight, media.gfx.null/* [0] */ );
				break;
			case NUM_FONT_CHUNKY:
				CG_DrawPic( x,y, charWidth, charHeight, media.gfx.null/* [0] */ );
				break;
			default:
			case NUM_FONT_BIG:
				CG_DrawPic( x,y, charWidth, charHeight, media.gfx.null/* [0] */ );
				break;
			}
			x += 2 + (xWidth);
		}
	}
	else
	{
		x += 2 + (xWidth)*(width - l);
	}

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		switch(style)
		{
		case NUM_FONT_SMALL:
			CG_DrawPic( x,y, charWidth, charHeight, media.gfx.null/* [frame] */ );
			x++;	// For a one line gap
			break;
		case NUM_FONT_CHUNKY:
			CG_DrawPic( x,y, charWidth, charHeight, media.gfx.null/* [frame] */ );
			break;
		default:
		case NUM_FONT_BIG:
			CG_DrawPic( x,y, charWidth, charHeight, media.gfx.null/* [frame] */ );
			break;
		}

		x += (xWidth);
		ptr++;
		l--;
	}

}
