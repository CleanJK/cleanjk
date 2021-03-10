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

// string allocation/managment

#define __STDC_FORMAT_MACROS
#include <cinttypes>

#ifdef BUILD_UI
	#include "ui/ui_local.hpp"
#endif
#ifdef BUILD_CGAME
	#include "cgame/cg_local.hpp"
#endif

#include "ui/ui_shared.hpp"
#include "ui/menudef.h"
#include "game/bg_public.hpp"
#include "game/anims.hpp"
#include "ghoul2/G2.hpp"
#include "client/cl_fonts.hpp"

extern stringID_table_t animTable [MAX_ANIMATIONS+1];

const char *HolocronIcons[NUM_FORCE_POWERS] = {
	"gfx/mp/f_icon_lt_heal",		//FP_HEAL,
	"gfx/mp/f_icon_levitation",		//FP_LEVITATION,
	"gfx/mp/f_icon_speed",			//FP_SPEED,
	"gfx/mp/f_icon_push",			//FP_PUSH,
	"gfx/mp/f_icon_pull",			//FP_PULL,
	"gfx/mp/f_icon_lt_telepathy",	//FP_TELEPATHY,
	"gfx/mp/f_icon_dk_grip",		//FP_GRIP,
	"gfx/mp/f_icon_dk_l1",			//FP_LIGHTNING,
	"gfx/mp/f_icon_dk_rage",		//FP_RAGE,
	"gfx/mp/f_icon_lt_protect",		//FP_PROTECT,
	"gfx/mp/f_icon_lt_absorb",		//FP_ABSORB,
	"gfx/mp/f_icon_lt_healother",	//FP_TEAM_HEAL,
	"gfx/mp/f_icon_dk_forceother",	//FP_TEAM_FORCE,
	"gfx/mp/f_icon_dk_drain",		//FP_DRAIN,
	"gfx/mp/f_icon_sight",			//FP_SEE,
	"gfx/mp/f_icon_saber_attack",	//FP_SABER_OFFENSE,
	"gfx/mp/f_icon_saber_defend",	//FP_SABER_DEFENSE,
	"gfx/mp/f_icon_saber_throw"		//FP_SABERTHROW
};

#define SCROLL_TIME_START					500
#define SCROLL_TIME_ADJUST					150
#define SCROLL_TIME_ADJUSTOFFSET			40
#define SCROLL_TIME_FLOOR					20

struct scrollInfo_t {
	int nextScrollTime;
	int nextAdjustTime;
	int adjustValue;
	int scrollKey;
	float xStart;
	float yStart;
	itemDef_t *item;
	bool scrollDir;
};

static scrollInfo_t scrollInfo;

static void (*captureFunc) (void *p) = nullptr;
static void *captureData = nullptr;
static itemDef_t *itemCapture = nullptr;   // item that has the mouse captured ( if any )

displayContextDef_t *DC = nullptr;

static bool g_waitingForKey = false;
static bool g_editingField = false;

static itemDef_t *g_bindItem = nullptr;
static itemDef_t *g_editItem = nullptr;

menuDef_t Menus[MAX_MENUS];      // defined menus
int menuCount = 0;               // how many

menuDef_t *menuStack[MAX_OPEN_MENUS];
int openMenuCount = 0;

static bool debugMode = false;

#define DOUBLE_CLICK_DELAY 300
static int lastListBoxClickTime = 0;

static bool Menu_OverActiveItem(menuDef_t *menu, float x, float y);
static void Item_TextScroll_BuildLines ( itemDef_t* item );

#ifdef BUILD_CGAME
#define MEM_POOL_SIZE  (128 * 1024)
#else
#define MEM_POOL_SIZE  (4 * 1024 * 1024)
#endif

static char memoryPool[MEM_POOL_SIZE];
static int allocPoint;
static bool outOfMemory;

struct itemFlagsDef_t {
	const char *string;
	int value;
};

const itemFlagsDef_t itemFlags[] = {
	{ "WINDOW_INACTIVE", WINDOW_INACTIVE },
	{ nullptr,              0 }
};

void *UI_Alloc( int size ) {
	char	*p;

	if ( allocPoint + size > MEM_POOL_SIZE ) {
		outOfMemory = true;
		if (DC->Print) {
			DC->Print("UI_Alloc: Failure. Out of memory!\n");
		}
		return nullptr;
	}

	p = &memoryPool[allocPoint];

	allocPoint += ( size + 15 ) & ~15;

	return p;
}

void UI_InitMemory( void ) {
	allocPoint = 0;
	outOfMemory = false;
}

bool UI_OutOfMemory( void ) {
	return outOfMemory;
}

#define HASH_TABLE_SIZE 2048

// return a hash value for the string
static unsigned hashForString(const char *str) {
	int		i;
	unsigned	hash;
	char	letter;

	hash = 0;
	i = 0;
	while (str[i] != '\0') {
		letter = tolower((unsigned char)str[i]);
		hash+=(unsigned)(letter)*(i+119);
		i++;
	}
	hash &= (HASH_TABLE_SIZE-1);
	return hash;
}

struct stringDef_t {
	stringDef_t *next;
	const char *str;
};

static int strPoolIndex = 0;
static char strPool[STRING_POOL_SIZE];

static int strHandleCount = 0;
static stringDef_t *strHandle[HASH_TABLE_SIZE];

const char *String_Alloc(const char *p) {
	int len;
	unsigned hash;
	stringDef_t *str, *last;
	static const char *staticNULL = "";

	if (p == nullptr) {
		return nullptr;
	}

	if (*p == 0) {
		return staticNULL;
	}

	hash = hashForString(p);

	str = strHandle[hash];
	while (str) {
#ifdef _DEBUG
		if ( !str->str || !str->str[0] ) {
			Com_Printf( "String_Alloc: poisoned at 0x%" PRIxPTR " with 0x" PRIxPTR " (next: 0x%" PRIxPTR ")\n", str, str->str, str->next );
			str = str->next;
			continue;
		}
#endif

		if (strcmp(p, str->str) == 0) {
			return str->str;
		}
		str = str->next;
	}

	len = strlen(p);
	if (len + strPoolIndex + 1 < STRING_POOL_SIZE) {
		int ph = strPoolIndex;
		strcpy(&strPool[strPoolIndex], p);
		strPoolIndex += len + 1;

		str = strHandle[hash];
		last = str;
		while (last && last->next)
		{
			last = last->next;
		}

		str  = (stringDef_t *) UI_Alloc(sizeof(stringDef_t));
		str->next = nullptr;
		str->str = &strPool[ph];
		if (last) {
			last->next = str;
		} else {
			strHandle[hash] = str;
		}
#ifdef _DEBUG
		//Com_Printf( "String_Alloc(%i): \"%s\"\n", strPoolIndex, &strPool[ph] );
#endif
		return &strPool[ph];
	}

	//Increase STRING_POOL_SIZE.
	Com_Printf( S_COLOR_RED, "String pool has been exhausted.\n" );
	return nullptr;
}

void String_Report() {
	float f;
	Com_Printf("Memory/String Pool Info\n");
	Com_Printf("----------------\n");
	f = strPoolIndex;
	f /= STRING_POOL_SIZE;
	f *= 100;
	Com_Printf("String Pool is %.1f%% full, %i bytes out of %i used.\n", f, strPoolIndex, STRING_POOL_SIZE);
	f = allocPoint;
	f /= MEM_POOL_SIZE;
	f *= 100;
	Com_Printf("Memory Pool is %.1f%% full, %i bytes out of %i used.\n", f, allocPoint, MEM_POOL_SIZE);
}

#if 0
void PC_SourceWarning(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof( string ), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap->PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_YELLOW "WARNING: %s, line %d: %s\n", filename, line, string);
}
#endif

void PC_SourceError(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof( string ), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap->PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string);
}

void LerpColor(vec4_t a, vec4_t b, vec4_t c, float t)
{
	int i;

	// lerp and clamp each component
	for (i=0; i<4; i++)
	{
		c[i] = a[i] + t*(b[i]-a[i]);
		if (c[i] < 0)
			c[i] = 0;
		else if (c[i] > 1.0)
			c[i] = 1.0;
	}
}

bool Float_Parse(char **p, float *f) {
	char	*token;
	token = COM_ParseExt((const char **)p, false);
	if (token && token[0] != 0) {
		*f = atof(token);
		return true;
	} else {
		return false;
	}
}

bool PC_Float_Parse(int handle, float *f) {
	pc_token_t token;
	bool negative = false;

	if ( !trap->PC_ReadToken( handle, &token ) )
		return false;

	if ( token.string[0] == '-' ) {
		if ( !trap->PC_ReadToken( handle, &token ) )
			return false;
		negative = true;
	}
	if (token.type != TT_NUMBER) {
		PC_SourceError(handle, "expected float but found %s", token.string);
		return false;
	}
	if (negative)
		*f = -token.floatvalue;
	else
		*f = token.floatvalue;
	return true;
}

bool Color_Parse(char **p, vec4_t *c) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!Float_Parse(p, &f)) {
			return false;
		}
		(*c)[i] = f;
	}
	return true;
}

bool PC_Color_Parse(int handle, vec4_t *c) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		(*c)[i] = f;
	}
	return true;
}

bool Int_Parse(char **p, int *i) {
	char	*token;
	token = COM_ParseExt((const char **)p, false);

	if (token && token[0] != 0) {
		*i = atoi(token);
		return true;
	} else {
		return false;
	}
}

bool PC_Int_Parse(int handle, int *i) {
	pc_token_t token;
	bool negative = false;

	if (!trap->PC_ReadToken(handle, &token))
		return false;
	if (token.string[0] == '-') {
		if (!trap->PC_ReadToken(handle, &token))
			return false;
		negative = true;
	}
	if (token.type != TT_NUMBER) {
		PC_SourceError(handle, "expected integer but found %s", token.string);
		return false;
	}
	*i = token.intvalue;
	if (negative)
		*i = - *i;
	return true;
}

bool Rect_Parse(char **p, rectDef_t *r) {
	if (Float_Parse(p, &r->x)) {
		if (Float_Parse(p, &r->y)) {
			if (Float_Parse(p, &r->w)) {
				if (Float_Parse(p, &r->h)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool PC_Rect_Parse(int handle, rectDef_t *r) {
	if (PC_Float_Parse(handle, &r->x)) {
		if (PC_Float_Parse(handle, &r->y)) {
			if (PC_Float_Parse(handle, &r->w)) {
				if (PC_Float_Parse(handle, &r->h)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool String_Parse(char **p, const char **out) {
	char *token;

	token = COM_ParseExt((const char **)p, false);
	if (token && token[0] != 0) {
		*(out) = String_Alloc(token);
		return *(out)!=nullptr;
	}
	return false;
}

bool PC_String_Parse(int handle, const char **out)
{
	static char*	squiggy = "}";
	pc_token_t		token;

	if (!trap->PC_ReadToken(handle, &token))
	{
		return false;
	}

	// Save some memory by not return the end squiggy as an allocated string
	if ( !Q_stricmp ( token.string, "}" ) )
	{
		*(out) = squiggy;
	}
	else
	{
		*(out) = String_Alloc(token.string);
	}
	return true;
}

bool PC_Script_Parse(int handle, const char **out) {
	char script[2048];
	pc_token_t token;

	script[0] = 0;
	// scripts start with { and have ; separated command lists.. commands are command, arg..
	// basically we want everything between the { } as it will be interpreted at run time

	if (!trap->PC_ReadToken(handle, &token))
		return false;
	if (Q_stricmp(token.string, "{") != 0) {
	    return false;
	}

	while ( 1 ) {
		if (!trap->PC_ReadToken(handle, &token))
			return false;

		if (Q_stricmp(token.string, "}") == 0) {
			*out = String_Alloc(script);
			return true;
		}

		if (token.string[1] != '\0') {
			Q_strcat(script, 2048, va("\"%s\"", token.string));
		} else {
			Q_strcat(script, 2048, token.string);
		}
		Q_strcat(script, 2048, " ");
	}
	return false;
}

// display, window, menu, item code

// Initializes the display with a structure to all the drawing routines
void Init_Display(displayContextDef_t *dc) {
	DC = dc;
}

// type and style painting

void GradientBar_Paint(rectDef_t *rect, vec4_t color) {
	// gradient bar takes two paints
	DC->setColor( color );
	DC->drawHandlePic(rect->x, rect->y, rect->w, rect->h, DC->Assets.gradientBar);
	DC->setColor( nullptr );
}

// Initializes a window structure ( windowDef_t ) with defaults
void Window_Init(windowDef_t *w) {
	memset(w, 0, sizeof(windowDef_t));
	w->borderSize = 1;
	w->foreColor[0] = w->foreColor[1] = w->foreColor[2] = w->foreColor[3] = 1.0;
	w->cinematic = -1;
	w->flags |= WINDOW_VISIBLE;
}

void Fade(int *flags, float *f, float clamp, int *nextTime, int offsetTime, bool bFlags, float fadeAmount) {
	if ( *flags & ( WINDOW_FADINGOUT | WINDOW_FADINGIN ) ) {
		if ( DC->realTime > *nextTime ) {
			const double tAdjust = DC->frameTime / (1000.0/60.0); //(60.0 / DC->FPS);

			*nextTime = DC->realTime + (offsetTime * tAdjust);
			if ( *flags & WINDOW_FADINGOUT ) {
				*f -= fadeAmount * tAdjust;
				if ( bFlags && *f <= 0.0 )
					*flags &= ~( WINDOW_FADINGOUT | WINDOW_VISIBLE );
			}
			else {
				*f += fadeAmount * tAdjust;
				if ( *f >= clamp ) {
					*f = clamp;
					if ( bFlags )
						*flags &= ~WINDOW_FADINGIN;
				}
			}
		}
	}
}

void Window_Paint(windowDef_t *w, float fadeAmount, float fadeClamp, float fadeCycle)
{
	//float bordersize = 0;
	vec4_t color;
	rectDef_t fillRect;

	if ( w == nullptr )
		return;

	if ( debugMode ) {
		color[0] = color[1] = color[2] = color[3] = 1;
		DC->drawRect(w->rect.x, w->rect.y, w->rect.w, w->rect.h, 1, color);
	}

	if ( w->style == 0 && w->border == 0 )
		return;

	fillRect = w->rect;
	if ( w->border != 0 ) {
		fillRect.x += w->borderSize;
		fillRect.y += w->borderSize;
		fillRect.w -= w->borderSize + 1;
		fillRect.h -= w->borderSize + 1;
	}

	if ( w->style == WINDOW_STYLE_FILLED ) {
		// box, but possible a shader that needs filled
		if ( w->background ) {
			Fade( &w->flags, &w->backColor[3], fadeClamp, &w->nextTime, fadeCycle, true, fadeAmount );
			DC->setColor( w->backColor );
			DC->drawHandlePic( fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->background );
			DC->setColor( nullptr );
		}
		else {
			DC->fillRect( fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->backColor );
		}
	}
	else if ( w->style == WINDOW_STYLE_GRADIENT ) {
		GradientBar_Paint(&fillRect, w->backColor);
	}
	else if ( w->style == WINDOW_STYLE_SHADER ) {
#ifndef BUILD_CGAME
		if ( w->flags & WINDOW_PLAYERCOLOR ) {
			vec4_t	color;
			color[0] = ui_char_color_red.integer/255.0f;
			color[1] = ui_char_color_green.integer/255.0f;
			color[2] = ui_char_color_blue.integer/255.0f;
			color[3] = 1;
			DC->setColor( color );
		}
#endif //

		if ( w->flags & WINDOW_FORECOLORSET )
			DC->setColor(w->foreColor);
		DC->drawHandlePic( fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->background );
		DC->setColor( nullptr );
	}
	else if ( w->style == WINDOW_STYLE_TEAMCOLOR ) {
		if ( DC->getTeamColor ) {
			DC->getTeamColor( &color );
			DC->fillRect( fillRect.x, fillRect.y, fillRect.w, fillRect.h, color );
		}
	}
	else if ( w->style == WINDOW_STYLE_CINEMATIC ) {
		if ( w->cinematic == -1 ) {
			w->cinematic = DC->playCinematic( w->cinematicName, fillRect.x, fillRect.y, fillRect.w, fillRect.h );
			if ( w->cinematic == -1 )
				w->cinematic = -2;
		}
		if ( w->cinematic >= 0 ) {
			DC->runCinematicFrame( w->cinematic );
			DC->drawCinematic( w->cinematic, fillRect.x, fillRect.y, fillRect.w, fillRect.h );
		}
	}

	if ( w->border == WINDOW_BORDER_FULL ) {
		// full
		// HACK HACK HACK
		if ( w->style == WINDOW_STYLE_TEAMCOLOR ) {
			if ( color[0] > 0 ) {
				// red
				color[0] = 1;
				color[1] = color[2] = .5;
			}
			else {
				color[2] = 1;
				color[0] = color[1] = .5;
			}
			color[3] = 1;
			DC->drawRect( w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize, color );
		}
		else
			DC->drawRect( w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize, w->borderColor );
	}
	else if (w->border == WINDOW_BORDER_HORZ) {
		// top/bottom
		DC->setColor( w->borderColor );
		DC->drawTopBottom( w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize );
		DC->setColor( nullptr );
	}
	else if (w->border == WINDOW_BORDER_VERT) {
		// left right
		DC->setColor( w->borderColor );
		DC->drawSides( w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize );
		DC->setColor( nullptr );
	}
	else if ( w->border == WINDOW_BORDER_KCGRADIENT ) {
		// this is just two gradient bars along each horz edge
		rectDef_t r = w->rect;
		r.h = w->borderSize;
		GradientBar_Paint(&r, w->borderColor);
		r.y = w->rect.y + w->rect.h - 1;
		GradientBar_Paint(&r, w->borderColor);
	}
}

void Item_SetScreenCoords(itemDef_t *item, float x, float y)
{
	if (item == nullptr)
		return;

	if (item->window.border != 0) {
		x += item->window.borderSize;
		y += item->window.borderSize;
	}

	item->window.rect.x = x + item->window.rectClient.x;
	item->window.rect.y = y + item->window.rectClient.y;
	item->window.rect.w = item->window.rectClient.w;
	item->window.rect.h = item->window.rectClient.h;

	// force the text rects to recompute
	item->textRect.w = 0;
	item->textRect.h = 0;

	if ( item->type == ITEM_TYPE_TEXTSCROLL ) {
		textScrollDef_t *scrollPtr = item->typeData.textscroll;
		if ( scrollPtr ) {
			scrollPtr->startPos = 0;
			scrollPtr->endPos = 0;
		}

		Item_TextScroll_BuildLines ( item );
	}
}

// FIXME: consolidate this with nearby stuff
void Item_UpdatePosition(itemDef_t *item)
{
	float x, y;
	menuDef_t *menu;

	if (item == nullptr || item->parent == nullptr)
		return;

	menu = (menuDef_t *) item->parent;

	x = menu->window.rect.x;
	y = menu->window.rect.y;

	if ( menu->window.border != 0 ) {
		x += menu->window.borderSize;
		y += menu->window.borderSize;
	}

	Item_SetScreenCoords(item, x, y);
}

// menus
void Menu_UpdatePosition(menuDef_t *menu) {
	int i;
	float x, y;

	if (menu == nullptr) {
		return;
	}

	x = menu->window.rect.x;
	y = menu->window.rect.y;
	if ( menu->window.border != 0 ) {
		x += menu->window.borderSize;
		y += menu->window.borderSize;
	}

	for ( i=0; i<menu->itemCount; i++ ) {
		Item_SetScreenCoords( menu->items[i], x, y );
	}
}

void Menu_PostParse(menuDef_t *menu) {
	if ( menu == nullptr )
		return;

	if ( menu->fullScreen ) {
		menu->window.rect.x = 0;
		menu->window.rect.y = 0;
		menu->window.rect.w = SCREEN_WIDTH;
		menu->window.rect.h = SCREEN_HEIGHT;
	}
	Menu_UpdatePosition( menu );
}

itemDef_t *Menu_ClearFocus(menuDef_t *menu) {
	int i;
	itemDef_t *ret = nullptr;

	if ( menu == nullptr )
		return nullptr;

	for ( i=0; i<menu->itemCount; i++ ) {
		if ( menu->items[i]->window.flags & WINDOW_HASFOCUS )
			ret = menu->items[i];

		menu->items[i]->window.flags &= ~WINDOW_HASFOCUS;
		if ( menu->items[i]->leaveFocus )
			Item_RunScript( menu->items[i], menu->items[i]->leaveFocus );
	}

	return ret;
}

bool IsVisible(int flags) {
	return (flags & WINDOW_VISIBLE && !(flags & WINDOW_FADINGOUT));
}

bool Rect_ContainsPoint(rectDef_t *rect, float x, float y) {
	if ( rect ) {
		if ( x > rect->x && x < rect->x + rect->w && y > rect->y && y < rect->y + rect->h )
			return true;
	}
	return false;
}

int Menu_ItemsMatchingGroup(menuDef_t *menu, const char *name) {
	int i;
	int count = 0;

	for ( i=0; i<menu->itemCount; i++ ) {
		if ( !VALIDSTRING( menu->items[i]->window.name ) && !VALIDSTRING( menu->items[i]->window.group ) ) {
			Com_Printf( S_COLOR_YELLOW "WARNING: item has neither name or group\n" );
			continue;
		}

		if ( !Q_stricmp( menu->items[i]->window.name, name ) || ( VALIDSTRING( menu->items[i]->window.group ) && !Q_stricmp( menu->items[i]->window.group, name ) ) )
			count++;
	}

	return count;
}

itemDef_t *Menu_GetMatchingItemByNumber(menuDef_t *menu, int index, const char *name) {
	int i;
	int count = 0;
	for (i = 0; i < menu->itemCount; i++) {
		if (Q_stricmp(menu->items[i]->window.name, name) == 0 || (menu->items[i]->window.group && Q_stricmp(menu->items[i]->window.group, name) == 0)) {
			if (count == index) {
				return menu->items[i];
			}
			count++;
		}
	}
	return nullptr;
}

bool Script_SetColor ( itemDef_t *item, char **args )
{
	const char *name;
	int i;
	float f;
	vec4_t *out;

	// expecting type of color to set and 4 args for the color
	if (String_Parse(args, &name))
	{
		out = nullptr;
		if (Q_stricmp(name, "backcolor") == 0)
		{
			out = &item->window.backColor;
			item->window.flags |= WINDOW_BACKCOLORSET;
		}
		else if (Q_stricmp(name, "forecolor") == 0)
		{
			out = &item->window.foreColor;
			item->window.flags |= WINDOW_FORECOLORSET;
		}
		else if (Q_stricmp(name, "bordercolor") == 0)
		{
			out = &item->window.borderColor;
		}

		if (out)
		{
			for (i = 0; i < 4; i++)
			{
				if (!Float_Parse(args, &f))
				{
					return true;
				}
				(*out)[i] = f;
			}
		}
	}

	return true;
}

bool Script_SetAsset(itemDef_t *item, char **args)
{
	const char *name;
	// expecting name to set asset to
	if (String_Parse(args, &name))
	{
		// check for a model
		if (item->type == ITEM_TYPE_MODEL)
		{
		}
	}
	return true;
}

bool Script_SetBackground(itemDef_t *item, char **args)
{
	const char *name;
	// expecting name to set asset to
	if (String_Parse(args, &name))
	{
		item->window.background = DC->registerShaderNoMip(name);
	}
	return true;
}

bool Script_SetItemRectCvar(itemDef_t *item, char **args)
{
	const char	*itemName;
	const char	*cvarName;
	char		cvarBuf[1024];
	const char	*holdVal;
	char		*holdBuf;
	itemDef_t	*item2=0;
	menuDef_t	*menu;

	// expecting item group and cvar to get value from
	if (String_Parse(args, &itemName) && String_Parse(args, &cvarName))
	{
		item2 = Menu_FindItemByName((menuDef_t *) item->parent, itemName);

		if (item2)
		{
			// get cvar data
			DC->getCVarString(cvarName, cvarBuf, sizeof(cvarBuf));

			holdBuf = cvarBuf;
			if (String_Parse(&holdBuf,&holdVal))
			{
				menu = (menuDef_t *) item->parent;

				item2->window.rectClient.x = atof(holdVal) + menu->window.rect.x;
				if (String_Parse(&holdBuf,&holdVal))
				{
					item2->window.rectClient.y = atof(holdVal) + menu->window.rect.y;
					if (String_Parse(&holdBuf,&holdVal))
					{
						item2->window.rectClient.w = atof(holdVal);
						if (String_Parse(&holdBuf,&holdVal))
						{
							item2->window.rectClient.h = atof(holdVal);

							item2->window.rect.x = item2->window.rectClient.x;
							item2->window.rect.y = item2->window.rectClient.y;
							item2->window.rect.w = item2->window.rectClient.w;
							item2->window.rect.h = item2->window.rectClient.h;

							return true;
						}
					}
				}
			}
		}
	}

	// Default values in case things screw up
	if (item2)
	{
		item2->window.rectClient.x = 0;
		item2->window.rectClient.y = 0;
		item2->window.rectClient.w = 0;
		item2->window.rectClient.h = 0;
	}

//	Com_Printf(S_COLOR_YELLOW"WARNING: SetItemRectCvar: problems. Set cvar to 0's\n" );

	return true;
}

bool Script_SetItemBackground(itemDef_t *item, char **args)
{
	const char *itemName;
	const char *name;

	// expecting name of shader
	if (String_Parse(args, &itemName) && String_Parse(args, &name))
	{
		Menu_SetItemBackground((menuDef_t *) item->parent, itemName, name);
	}
	return true;
}

bool Script_SetItemText(itemDef_t *item, char **args)
{
	const char *itemName;
	const char *text;

	// expecting text
	if (String_Parse(args, &itemName) && String_Parse(args, &text))
	{
		Menu_SetItemText((menuDef_t *) item->parent, itemName, text);
	}
	return true;
}

itemDef_t *Menu_FindItemByName(menuDef_t *menu, const char *p) {
	int i;
	if (menu == nullptr || p == nullptr) {
		return nullptr;
	}

	for (i = 0; i < menu->itemCount; i++) {
		if (Q_stricmp(p, menu->items[i]->window.name) == 0) {
			return menu->items[i];
		}
	}

	return nullptr;
}

bool Script_SetTeamColor(itemDef_t *item, char **args)
{
	if (DC->getTeamColor)
	{
		int i;
		vec4_t color;
		DC->getTeamColor(&color);
		for (i = 0; i < 4; i++)
		{
			item->window.backColor[i] = color[i];
		}
	}
	return true;
}

bool Script_SetItemColor(itemDef_t *item, char **args)
{
	const char *itemname;
	const char *name;
	vec4_t color;
	int i;
	vec4_t *out;

	// expecting type of color to set and 4 args for the color
	if (String_Parse(args, &itemname) && String_Parse(args, &name))
	{
		itemDef_t	*item2;
		int			j,count;
		char buff[1024];

		// Is is specifying a cvar to get the item name from?
		if (itemname[0] == '*')
		{
			itemname += 1;
		    DC->getCVarString(itemname, buff, sizeof(buff));
			itemname = buff;
		}

		count = Menu_ItemsMatchingGroup((menuDef_t *) item->parent, itemname);

		if (!Color_Parse(args, &color))
		{
			return true;
		}

		for (j = 0; j < count; j++)
		{
			item2 = Menu_GetMatchingItemByNumber((menuDef_t *) item->parent, j, itemname);
			if (item2 != nullptr)
			{
				out = nullptr;
				if (Q_stricmp(name, "backcolor") == 0)
				{
					out = &item2->window.backColor;
				}
				else if (Q_stricmp(name, "forecolor") == 0)
				{
					out = &item2->window.foreColor;
					item2->window.flags |= WINDOW_FORECOLORSET;
				}
				else if (Q_stricmp(name, "bordercolor") == 0)
				{
					out = &item2->window.borderColor;
				}

				if (out)
				{
					for (i = 0; i < 4; i++)
					{
						(*out)[i] = color[i];
					}
				}
			}
		}
	}

	return true;
}

bool Script_SetItemColorCvar(itemDef_t *item, char **args)
{
	const char *itemname;
	char	*colorCvarName,*holdBuf,*holdVal;
	char cvarBuf[1024];
	const char *name;
	vec4_t color = { 0.0f };
	int i;
	vec4_t *out;

	// expecting type of color to set and 4 args for the color
	if (String_Parse(args, &itemname) && String_Parse(args, &name))
	{
		itemDef_t	*item2;
		int			j,count;
		char buff[1024];

		// Is is specifying a cvar to get the item name from?
		if (itemname[0] == '*')
		{
			itemname += 1;
		    DC->getCVarString(itemname, buff, sizeof(buff));
			itemname = buff;
		}

		count = Menu_ItemsMatchingGroup((menuDef_t *) item->parent, itemname);

		// Get the cvar with the color
		if (!String_Parse(args,(const char **) &colorCvarName))
		{
			return true;
		}
		else
		{
			DC->getCVarString(colorCvarName, cvarBuf, sizeof(cvarBuf));

			holdBuf = cvarBuf;
			if (String_Parse(&holdBuf,(const char **) &holdVal))
			{
				color[0] = atof(holdVal);
				if (String_Parse(&holdBuf,(const char **) &holdVal))
				{
					color[1] = atof(holdVal);
					if (String_Parse(&holdBuf,(const char **) &holdVal))
					{
						color[2] = atof(holdVal);
						if (String_Parse(&holdBuf,(const char **) &holdVal))
						{
							color[3] = atof(holdVal);
						}
					}
				}
			}
		}

		for (j = 0; j < count; j++)
		{
			item2 = Menu_GetMatchingItemByNumber((menuDef_t *) item->parent, j, itemname);
			if (item2 != nullptr)
			{
				out = nullptr;
				if (Q_stricmp(name, "backcolor") == 0)
				{
					out = &item2->window.backColor;
				}
				else if (Q_stricmp(name, "forecolor") == 0)
				{
					out = &item2->window.foreColor;
					item2->window.flags |= WINDOW_FORECOLORSET;
				}
				else if (Q_stricmp(name, "bordercolor") == 0)
				{
					out = &item2->window.borderColor;
				}

				if (out)
				{
					for (i = 0; i < 4; i++)
					{
						(*out)[i] = color[i];
					}
				}
			}
		}
	}

	return true;
}

bool Script_SetItemRect(itemDef_t *item, char **args)
{
	const char *itemname;
	rectDef_t *out;
	rectDef_t rect;
	menuDef_t	*menu;

	// expecting type of color to set and 4 args for the color
	if (String_Parse(args, &itemname))
	{
		itemDef_t *item2;
		int j;
		int count = Menu_ItemsMatchingGroup((menuDef_t *) item->parent, itemname);

		if (!Rect_Parse(args, &rect))
		{
			return true;
		}

		menu = (menuDef_t *) item->parent;

		for (j = 0; j < count; j++)
		{
			item2 = Menu_GetMatchingItemByNumber(menu, j, itemname);
			if (item2 != nullptr)
			{
				out = &item2->window.rect;

				if (out)
				{
					item2->window.rect.x = rect.x  + menu->window.rect.x;
					item2->window.rect.y = rect.y  + menu->window.rect.y;
					item2->window.rect.w = rect.w;
					item2->window.rect.h = rect.h;
				}
			}
		}
	}
	return true;
}

void Menu_ShowGroup (menuDef_t *menu, const char *groupName, bool showFlag)
{
	itemDef_t *item;
	int count,j;

	count = Menu_ItemsMatchingGroup( menu, groupName);
	for (j = 0; j < count; j++)
	{
		item = Menu_GetMatchingItemByNumber( menu, j, groupName);
		if (item != nullptr)
		{
			if (showFlag)
			{
				item->window.flags |= WINDOW_VISIBLE;
			}
			else
			{
				item->window.flags &= ~(WINDOW_VISIBLE | WINDOW_HASFOCUS);
			}
		}
	}
}

void Menu_ShowItemByName(menuDef_t *menu, const char *p, bool bShow) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != nullptr) {
			if (bShow) {
				item->window.flags |= WINDOW_VISIBLE;
			} else {
				item->window.flags &= ~WINDOW_VISIBLE;
				// stop cinematics playing in the window
				if (item->window.cinematic >= 0) {
					DC->stopCinematic(item->window.cinematic);
					item->window.cinematic = -1;
				}
			}
		}
	}
}

void Menu_FadeItemByName(menuDef_t *menu, const char *p, bool fadeOut) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != nullptr) {
			if (fadeOut) {
				item->window.flags |= (WINDOW_FADINGOUT | WINDOW_VISIBLE);
				item->window.flags &= ~WINDOW_FADINGIN;
			} else {
				item->window.flags |= (WINDOW_VISIBLE | WINDOW_FADINGIN);
				item->window.flags &= ~WINDOW_FADINGOUT;
			}
		}
	}
}

menuDef_t *Menus_FindByName(const char *p) {
	int i;
	for (i = 0; i < menuCount; i++) {
		if (Q_stricmp(Menus[i].window.name, p) == 0) {
			return &Menus[i];
		}
	}
	return nullptr;
}

void Menus_ShowByName(const char *p) {
	menuDef_t *menu = Menus_FindByName(p);
	if (menu) {
		Menus_Activate(menu);
	}
}

menuDef_t *Menus_OpenByName( const char *p ) {
	menuDef_t *result = Menus_ActivateByName( p );
	if ( !result ) {
		if ( developer.integer ) {
			DC->Print( va( "%s couldn't find menu \"%s\"\n", __FUNCTION__, p ) );
		}
	}
	return result;
}

static void Menu_RunCloseScript(menuDef_t *menu) {
	if (menu && menu->window.flags & WINDOW_VISIBLE && menu->onClose) {
		itemDef_t item;
		item.parent = menu;
		Item_RunScript(&item, menu->onClose);
	}
}

void Menus_CloseByName ( const char *p )
{
	menuDef_t *menu = Menus_FindByName(p);

	// If the menu wasnt found just exit
	if (menu == nullptr)
	{
		return;
	}

	// Run the close script for the menu
	Menu_RunCloseScript(menu);

	// If this window had the focus then take it away
	if ( menu->window.flags & WINDOW_HASFOCUS )
	{
		// If there is something still in the open menu list then
		// set it to have focus now
		if ( openMenuCount )
		{
			// Subtract one from the open menu count to prepare to
			// remove the top menu from the list
			openMenuCount -= 1;

			// Set the top menu to have focus now
			menuStack[openMenuCount]->window.flags |= WINDOW_HASFOCUS;

			// Remove the top menu from the list
			menuStack[openMenuCount] = nullptr;
		}
	}

	// Window is now invisible and doenst have focus
	menu->window.flags &= ~(WINDOW_VISIBLE | WINDOW_HASFOCUS);
}

int FPMessageTime = 0;

void Menus_CloseAll()
{
	int i;

	g_waitingForKey = false;

	for (i = 0; i < menuCount; i++)
	{
		Menu_RunCloseScript ( &Menus[i] );
		Menus[i].window.flags &= ~(WINDOW_HASFOCUS | WINDOW_VISIBLE);
	}

	// Clear the menu stack
	openMenuCount = 0;

	FPMessageTime = 0;
}

bool Script_Show(itemDef_t *item, char **args)
{
	const char *name;
	if (String_Parse(args, &name))
	{
		Menu_ShowItemByName((menuDef_t *) item->parent, name, true);
	}
	return true;
}

bool Script_Hide(itemDef_t *item, char **args)
{
	const char *name;
	if (String_Parse(args, &name))
	{
		Menu_ShowItemByName((menuDef_t *) item->parent, name, false);
	}
	return true;
}

bool Script_FadeIn(itemDef_t *item, char **args)
{
	const char *name;
	if (String_Parse(args, &name))
	{
		Menu_FadeItemByName((menuDef_t *) item->parent, name, false);
	}

	return true;
}

bool Script_FadeOut(itemDef_t *item, char **args)
{
	const char *name;
	if (String_Parse(args, &name))
	{
		Menu_FadeItemByName((menuDef_t *) item->parent, name, true);
	}
	return true;
}

bool Script_Open( itemDef_t *item, char **args ) {
	const char *name;
	if ( String_Parse( args, &name ) ) {
		Menus_OpenByName( name );
	}
	return true;
}

bool Script_Close(itemDef_t *item, char **args)
{
	const char *name;
	if ( String_Parse( args, &name ) ) {
		if ( !Q_stricmp( name, "all" ) ) {
			Menus_CloseAll();
		}
		else {
			Menus_CloseByName( name );
		}
	}
	return true;
}

void Menu_TransitionItemByName(menuDef_t *menu, const char *p, const rectDef_t *rectFrom, const rectDef_t *rectTo, int time, float amt)
{
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++)
	{
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != nullptr)
		{
			if (!rectFrom)
			{
				rectFrom = &item->window.rect;	//if there are more than one of these with the same name, they'll all use the FIRST one's FROM.
			}
			item->window.flags |= (WINDOW_INTRANSITION | WINDOW_VISIBLE);
			item->window.offsetTime = time;
			memcpy(&item->window.rectClient, rectFrom, sizeof(rectDef_t));
			memcpy(&item->window.rectEffects, rectTo, sizeof(rectDef_t));
			item->window.rectEffects2.x = fabs(rectTo->x - rectFrom->x) / amt;
			item->window.rectEffects2.y = fabs(rectTo->y - rectFrom->y) / amt;
			item->window.rectEffects2.w = fabs(rectTo->w - rectFrom->w) / amt;
			item->window.rectEffects2.h = fabs(rectTo->h - rectFrom->h) / amt;

			Item_UpdatePosition(item);
		}
	}
}

//JLF
#define _TRANS3
#ifdef _TRANS3
void Menu_Transition3ItemByName(menuDef_t *menu, const char *p, const float minx, const float miny, const float minz,
								const float maxx, const float maxy, const float maxz, const float fovtx, const float fovty,
								const int time, const float amt)
{
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	modelDef_t  * modelptr;
	for (i = 0; i < count; i++)
	{
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != nullptr)
		{
			if ( item->type == ITEM_TYPE_MODEL )
			{
				modelptr = item->typeData.model;

				item->window.flags |= (WINDOW_INTRANSITIONMODEL | WINDOW_VISIBLE);
				item->window.offsetTime = time;
				modelptr->fov_x2 = fovtx;
				modelptr->fov_y2 = fovty;
				VectorSet(modelptr->g2maxs2, maxx, maxy, maxz);
				VectorSet(modelptr->g2mins2, minx, miny, minz);

//				//modelptr->g2maxs2.x= maxx;
//				modelptr->g2maxs2.y= maxy;
//				modelptr->g2maxs2.z= maxz;
//				modelptr->g2mins2.x= minx;
//				modelptr->g2mins2.y= miny;
//				modelptr->g2mins2.z= minz;

//				VectorSet(modelptr->g2maxs2, maxx, maxy, maxz);

				modelptr->g2maxsEffect[0] = fabs(modelptr->g2maxs2[0] - modelptr->g2maxs[0]) / amt;
				modelptr->g2maxsEffect[1] = fabs(modelptr->g2maxs2[1] - modelptr->g2maxs[1]) / amt;
				modelptr->g2maxsEffect[2] = fabs(modelptr->g2maxs2[2] - modelptr->g2maxs[2]) / amt;

				modelptr->g2minsEffect[0] = fabs(modelptr->g2mins2[0] - modelptr->g2mins[0]) / amt;
				modelptr->g2minsEffect[1] = fabs(modelptr->g2mins2[1] - modelptr->g2mins[1]) / amt;
				modelptr->g2minsEffect[2] = fabs(modelptr->g2mins2[2] - modelptr->g2mins[2]) / amt;

				modelptr->fov_Effectx = fabs(modelptr->fov_x2 - modelptr->fov_x) / amt;
				modelptr->fov_Effecty = fabs(modelptr->fov_y2 - modelptr->fov_y) / amt;
			}
		}
	}
}

#endif

#define MAX_DEFERRED_SCRIPT		2048

char		ui_deferredScript [ MAX_DEFERRED_SCRIPT ];
itemDef_t*	ui_deferredScriptItem = nullptr;

// Defers the rest of the script based on the defer condition.
// The deferred portion of the script can later be run with the "rundeferred"
bool Script_Defer ( itemDef_t* item, char **args )
{
	// Should the script be deferred?
	if ( DC->deferScript ( (char**)args ) )
	{
		// Need the item the script was being run on
		ui_deferredScriptItem = item;

		// Save the rest of the script
		Q_strncpyz ( ui_deferredScript, *args, MAX_DEFERRED_SCRIPT );

		// No more running
		return false;
	}

	// Keep running the script, its ok
	return true;
}

// Runs the last deferred script, there can only be one script deferred at a time so be careful of recursion
bool Script_RunDeferred ( itemDef_t* item, char **args )
{
	// Make sure there is something to run.
	if ( !ui_deferredScript[0] || !ui_deferredScriptItem )
	{
		return true;
	}

	// Run the deferred script now
	Item_RunScript ( ui_deferredScriptItem, ui_deferredScript );

	return true;
}

bool Script_Transition(itemDef_t *item, char **args)
{
	const char *name;
	rectDef_t rectFrom, rectTo;
	int time;
	float amt;

	if (String_Parse(args, &name))
	{
	    if ( Rect_Parse(args, &rectFrom) && Rect_Parse(args, &rectTo) && Int_Parse(args, &time) && Float_Parse(args, &amt))
		{
			Menu_TransitionItemByName((menuDef_t *) item->parent, name, &rectFrom, &rectTo, time, amt);
		}
	}

	return true;
}

void Menu_OrbitItemByName(menuDef_t *menu, const char *p, float x, float y, float cx, float cy, int time)
{
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != nullptr) {
			item->window.flags |= (WINDOW_ORBITING | WINDOW_VISIBLE);
			item->window.offsetTime = time;
			item->window.rectEffects.x = cx;
			item->window.rectEffects.y = cy;
			item->window.rectClient.x = x;
			item->window.rectClient.y = y;
			Item_UpdatePosition(item);
		}
	}
}

void Menu_ItemDisable(menuDef_t *menu, const char *name, bool disableFlag)
{
	int	j,count;
	itemDef_t *itemFound;

	count = Menu_ItemsMatchingGroup(menu, name);
	// Loop through all items that have this name
	for (j = 0; j < count; j++)
	{
		itemFound = Menu_GetMatchingItemByNumber( menu, j, name);
		if (itemFound != nullptr)
		{
			itemFound->disabled = disableFlag;
			// Just in case it had focus
			itemFound->window.flags &= ~WINDOW_MOUSEOVER;
		}
	}
}

// Set item disable flags
bool Script_Disable(itemDef_t *item, char **args)
{
	char *name;
	int	value;
	menuDef_t *menu;

	if (String_Parse(args, (const char **)&name))
	{
		char buff[1024];

		// Is is specifying a cvar to get the item name from?
		if (name[0] == '*')
		{
			name += 1;
		    DC->getCVarString(name, buff, sizeof(buff));
			name = buff;
		}

		if ( Int_Parse(args, &value))
		{
			menu = Menu_GetFocused();
			Menu_ItemDisable(menu, name,value);
		}
	}

	return true;
}

// Scale the given item instantly.
bool Script_Scale(itemDef_t *item, char **args)
{
	const char *name;
	float scale;
	int	j,count;
	itemDef_t *itemFound;
	rectDef_t rectTo;

	if (String_Parse(args, &name))
	{
		char buff[1024];

		// Is is specifying a cvar to get the item name from?
		if (name[0] == '*')
		{
			name += 1;
		    DC->getCVarString(name, buff, sizeof(buff));
			name = buff;
		}

		count = Menu_ItemsMatchingGroup((menuDef_t *) item->parent, name);

		if ( Float_Parse(args, &scale))
		{
			for (j = 0; j < count; j++)
			{
				itemFound = Menu_GetMatchingItemByNumber( (menuDef_t *) item->parent, j, name);
				if (itemFound != nullptr)
				{
					rectTo.h = itemFound->window.rect.h * scale;
					rectTo.w = itemFound->window.rect.w * scale;

					rectTo.x = itemFound->window.rect.x + ((itemFound->window.rect.h - rectTo.h)/2);
					rectTo.y = itemFound->window.rect.y + ((itemFound->window.rect.w - rectTo.w)/2);

					Menu_TransitionItemByName((menuDef_t *) item->parent, name, 0, &rectTo, 1, 1);
				}
			}
		}
	}

	return true;
}

bool Script_Orbit(itemDef_t *item, char **args)
{
	const char *name;
	float cx, cy, x, y;
	int time;

	if (String_Parse(args, &name))
	{
		if ( Float_Parse(args, &x) && Float_Parse(args, &y) && Float_Parse(args, &cx) && Float_Parse(args, &cy) && Int_Parse(args, &time) )
		{
			Menu_OrbitItemByName((menuDef_t *) item->parent, name, x, y, cx, cy, time);
		}
	}

	return true;
}

bool Script_SetFocus(itemDef_t *item, char **args)
{
	const char *name;
	itemDef_t *focusItem;

	if (String_Parse(args, &name)) {
		focusItem = Menu_FindItemByName((menuDef_t *) item->parent, name);
		if (focusItem && !(focusItem->window.flags & WINDOW_DECORATION) && !(focusItem->window.flags & WINDOW_HASFOCUS)) {
			Menu_ClearFocus((menuDef_t *) item->parent);
			focusItem->window.flags |= WINDOW_HASFOCUS;

			if (focusItem->onFocus) {
				Item_RunScript(focusItem, focusItem->onFocus);
			}
			if (DC->Assets.itemFocusSound) {
				DC->startLocalSound( DC->Assets.itemFocusSound, CHAN_LOCAL_SOUND );
			}
		}
	}

	return true;
}

bool Script_SetPlayerModel(itemDef_t *item, char **args)
{
	const char *name;
	if (String_Parse(args, &name))
	{
		DC->setCVar("model", name);
	}

	return true;
}

bool ParseRect(const char **p, rectDef_t *r)
{
	if (!COM_ParseFloat(p, &r->x))
	{
		if (!COM_ParseFloat(p, &r->y))
		{
			if (!COM_ParseFloat(p, &r->w))
			{
				if (!COM_ParseFloat(p, &r->h))
				{
					return true;
				}
			}
		}
	}
	return false;
}

// uses current origin instead of specifing a starting origin
// transition2		lfvscr		25 0 202 264  20 25
bool Script_Transition2(itemDef_t *item, char **args)
{
	const char *name;
	rectDef_t rectTo;
	int time;
	float amt;

	if (String_Parse(args, &name))
	{
		if ( ParseRect((const char **) args, &rectTo) && Int_Parse(args, &time) && !COM_ParseFloat((const char **) args, &amt))
		{
			Menu_TransitionItemByName((menuDef_t *) item->parent, name, 0, &rectTo, time, amt);
		}
		else
		{
			Com_Printf(S_COLOR_YELLOW"WARNING: Script_Transition2: error parsing '%s'\n", name );
		}
	}

	return true;
}

#ifdef _TRANS3
/// used exclusively with model views
// uses current origin instead of specifing a starting origin
//	transition3		lfvscr		(min extent) (max extent) (fovx,y)  20 25
bool Script_Transition3(itemDef_t *item, char **args)
{
	const char *name = nullptr, *value = nullptr;
	float minx, miny, minz, maxx, maxy, maxz, fovtx, fovty;
	int time;
	float amt;

	if (String_Parse(args, &name))
	{
		if (String_Parse( args, &value))
		{
			minx = atof(value);
			if (String_Parse( args, &value))
			{
				miny = atof(value);
				if (String_Parse( args, &value))
				{
					minz = atof(value);
					if (String_Parse( args, &value))
					{
						maxx = atof(value);
						if (String_Parse( args, &value))
						{
							maxy = atof(value);
							if (String_Parse( args, &value))
							{
								maxz = atof(value);
								if (String_Parse( args, &value))
								{
									fovtx = atof(value);
									if (String_Parse( args, &value))
									{
										fovty = atof(value);

										if (String_Parse( args, &value))
										{
											time = atoi(value);
											if (String_Parse( args, &value))
											{
												amt = atof(value);
												//set up the variables
												Menu_Transition3ItemByName((menuDef_t *) item->parent,
														name,
														minx, miny, minz,
														maxx, maxy, maxz,
														fovtx, fovty,
														time, amt);

												return true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if ( name ) {
		Com_Printf( S_COLOR_YELLOW "WARNING: Script_Transition3: error parsing '%s'\n", name );
	}
	return true;
}
#endif

bool Script_SetCvar(itemDef_t *item, char **args)
{
	const char *cvar, *val;
	if (String_Parse(args, &cvar) && String_Parse(args, &val))
	{
		DC->setCVar(cvar, val);
	}
	return true;
}

bool Script_SetCvarToCvar(itemDef_t *item, char **args) {
	const char *cvar, *val;
	if (String_Parse(args, &cvar) && String_Parse(args, &val)) {
		char cvarBuf[1024];
		DC->getCVarString(val, cvarBuf, sizeof(cvarBuf));
		DC->setCVar(cvar, cvarBuf);
	}
	return true;
}

bool Script_Exec(itemDef_t *item, char **args) {
	const char *val;
	if (String_Parse(args, &val)) {
		DC->executeText(EXEC_APPEND, va("%s ; ", val));
	}
	return true;
}

bool Script_Play(itemDef_t *item, char **args) {
	const char *val;
	if (String_Parse(args, &val)) {
		DC->startLocalSound(DC->registerSound(val), CHAN_AUTO);
	}
	return true;
}

bool Script_playLooped(itemDef_t *item, char **args) {
	const char *val;
	if (String_Parse(args, &val)) {
		DC->stopBackgroundTrack();
		DC->startBackgroundTrack(val, val, false);
	}
	return true;
}

constexpr commandDef_t commandList[] = {
  { "close",             &Script_Close },             // menu
  { "defer",             &Script_Defer },             //
  { "disable",           &Script_Disable },           // group/name
  { "exec",              &Script_Exec },              // group/name
  { "fadein",            &Script_FadeIn },            // group/name
  { "fadeout",           &Script_FadeOut },           // group/name
  { "hide",              &Script_Hide },              // group/name
  { "open",              &Script_Open },              // menu
  { "orbit",             &Script_Orbit },             // group/name
  { "play",              &Script_Play },              // group/name
  { "playlooped",        &Script_playLooped },        // group/name
  { "rundeferred",       &Script_RunDeferred },       //
  { "scale",             &Script_Scale },             // group/name
  { "setasset",          &Script_SetAsset },          // works on this
  { "setbackground",     &Script_SetBackground },     // works on this
  { "setcolor",          &Script_SetColor },          // works on this
  { "setcvar",           &Script_SetCvar },           // name
  { "setcvartocvar",     &Script_SetCvarToCvar },     // name
  { "setfocus",          &Script_SetFocus },          // sets focus
  { "setitembackground", &Script_SetItemBackground }, // group/name
  { "setitemcolor",      &Script_SetItemColor },      // group/name
  { "setitemcolorcvar",  &Script_SetItemColorCvar },  // group/name
  { "setitemrect",       &Script_SetItemRect },       // group/name
  { "setitemrectcvar",   &Script_SetItemRectCvar },   // group/name
  { "setitemtext",       &Script_SetItemText },       // group/name
  { "setplayermodel",    &Script_SetPlayerModel },    // sets model
  { "setteamcolor",      &Script_SetTeamColor },      // sets this background color to team color
  { "show",              &Script_Show },              // group/name
  { "transition",        &Script_Transition },        // group/name
  { "transition2",       &Script_Transition2 },       // group/name
  { "transition3",       &Script_Transition3 },       // group/name
};

// Set all the items within a given menu, with the given itemName, to the given shader
void Menu_SetItemBackground(const menuDef_t *menu,const char *itemName, const char *background)
{
	itemDef_t	*item;
	int			j, count;

	if (!menu)	// No menu???
	{
		return;
	}

	count = Menu_ItemsMatchingGroup( (menuDef_t *) menu, itemName);

	for (j = 0; j < count; j++)
	{
		item = Menu_GetMatchingItemByNumber( (menuDef_t *) menu, j, itemName);
		if (item != nullptr)
		{
			item->window.background = DC->registerShaderNoMip(background);
		}
	}
}

// Set all the items within a given menu, with the given itemName, to the given text
void Menu_SetItemText(const menuDef_t *menu,const char *itemName, const char *text)
{
	itemDef_t	*item;
	int			j, count;

	if (!menu)	// No menu???
	{
		return;
	}

	count = Menu_ItemsMatchingGroup( (menuDef_t *) menu, itemName);

	for (j = 0; j < count; j++)
	{
		item = Menu_GetMatchingItemByNumber( (menuDef_t *) menu, j, itemName);
		if (item != nullptr)
		{
			if (text[0] == '*')
			{
				item->text = nullptr;		// Null this out because this would take presidence over cvar text.
				item->cvar = text+1;
				switch ( item->type )
				{
					case ITEM_TYPE_EDITFIELD:
					case ITEM_TYPE_NUMERICFIELD:
					case ITEM_TYPE_YESNO:
					case ITEM_TYPE_BIND:
					case ITEM_TYPE_SLIDER:
					case ITEM_TYPE_TEXT:
					{
						if ( item->typeData.edit )
						{
							item->typeData.edit->minVal = -1;
							item->typeData.edit->maxVal = -1;
							item->typeData.edit->defVal = -1;
						}
						break;
					}
					default:
						break;
				}
			}
			else
			{
				item->text = text;
				if ( item->type == ITEM_TYPE_TEXTSCROLL )
				{
					textScrollDef_t *scrollPtr = item->typeData.textscroll;
					if ( scrollPtr )
					{
						scrollPtr->startPos = 0;
						scrollPtr->endPos = 0;
					}

					Item_TextScroll_BuildLines ( item );
				}
			}
		}
	}
}

int scriptCommandCount = sizeof(commandList) / sizeof(commandDef_t);

void Item_RunScript(itemDef_t *item, const char *s)
{
	char script[2048], *p;
	int i;
	bool bRan;

	script[0] = 0;

	if (item && s && s[0])
	{
		Q_strcat(script, 2048, s);
		p = script;

		while (1)
		{
			const char *command;

			// expect command then arguments, ; ends command, nullptr ends script
			if (!String_Parse(&p, &command))
			{
				return;
			}

			if (command[0] == ';' && command[1] == '\0')
			{
				continue;
			}

			bRan = false;
			for (i = 0; i < scriptCommandCount; i++)
			{
				if (Q_stricmp(command, commandList[i].name) == 0)
				{
					// Allow a script command to stop processing the script
					if ( !commandList[i].handler(item, &p) )
					{
						return;
					}

					bRan = true;
					break;
				}
			}

			// not in our auto list, pass to handler
			if (!bRan)
			{
				DC->runScript(&p);
			}
		}
	}
}

bool Item_EnableShowViaCvar(itemDef_t *item, int flag) {
  char script[2048], *p;
  if (item && item->enableCvar && *item->enableCvar && item->cvarTest && *item->cvarTest) {
		char buff[2048];
	  DC->getCVarString(item->cvarTest, buff, sizeof(buff));

    Q_strncpyz(script, item->enableCvar, 2048);
    p = script;
    while (1) {
      const char *val;
      // expect value then ; or nullptr, nullptr ends list
      if (!String_Parse(&p, &val)) {
				return (item->cvarFlags & flag) ? false : true;
      }

      if (val[0] == ';' && val[1] == '\0') {
        continue;
      }

			// enable it if any of the values are true
			if (item->cvarFlags & flag) {
        if (Q_stricmp(buff, val) == 0) {
					return true;
				}
			} else {
				// disable it if any of the values are true
        if (Q_stricmp(buff, val) == 0) {
					return false;
				}
			}

    }
		return (item->cvarFlags & flag) ? false : true;
  }
	return true;
}

// will optionaly set focus to this item
bool Item_SetFocus(itemDef_t *item, float x, float y) {
	int i;
	itemDef_t *oldFocus;
	sfxHandle_t *sfx = &DC->Assets.itemFocusSound;
	bool playSound = false;
	menuDef_t *parent;
	// sanity check, non-null, not a decoration and does not already have the focus
	if (item == nullptr || item->window.flags & WINDOW_DECORATION || item->window.flags & WINDOW_HASFOCUS || !(item->window.flags & WINDOW_VISIBLE)) {
		return false;
	}

	parent = (menuDef_t*)item->parent;

	// items can be enabled and disabled
	if (item->disabled)
	{
		return false;
	}

	// items can be enabled and disabled based on cvars
	if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
		return false;
	}

	if (item->cvarFlags & (CVAR_SHOW | CVAR_HIDE) && !Item_EnableShowViaCvar(item, CVAR_SHOW)) {
		return false;
	}

	oldFocus = Menu_ClearFocus(parent);

	if (item->type == ITEM_TYPE_TEXT) {
		rectDef_t r;
		r = item->textRect;
		r.y -= r.h;

		if (Rect_ContainsPoint(&r, x, y))
		{
			item->window.flags |= WINDOW_HASFOCUS;
			if (item->focusSound) {
				sfx = &item->focusSound;
			}
			playSound = true;
		}
		else
		{
			if (oldFocus) {
				oldFocus->window.flags |= WINDOW_HASFOCUS;
				if (oldFocus->onFocus) {
					Item_RunScript(oldFocus, oldFocus->onFocus);
				}
			}
		}
	} else {
	    item->window.flags |= WINDOW_HASFOCUS;
		if (item->onFocus) {
			Item_RunScript(item, item->onFocus);
		}
		if (item->focusSound) {
			sfx = &item->focusSound;
		}
		playSound = true;
	}

	if (playSound && sfx) {
		DC->startLocalSound( *sfx, CHAN_LOCAL_SOUND );
	}

	for (i = 0; i < parent->itemCount; i++) {
		if (parent->items[i] == item) {
			parent->cursorItem = i;
			break;
		}
	}

	return true;
}

int Item_TextScroll_MaxScroll ( itemDef_t *item )
{
	textScrollDef_t *scrollPtr = item->typeData.textscroll;

	int count = scrollPtr->iLineCount;
	int max   = count - (int)(item->window.rect.h / scrollPtr->lineHeight) + 1;

	if (max < 0)
	{
		return 0;
	}

	return max;
}

int Item_TextScroll_ThumbPosition ( itemDef_t *item )
{
	float max, pos, size;
	textScrollDef_t *scrollPtr = item->typeData.textscroll;

	max  = Item_TextScroll_MaxScroll ( item );
	size = item->window.rect.h - (SCROLLBAR_SIZE * 2) - 2;

	if (max > 0)
	{
		pos = (size-SCROLLBAR_SIZE) / (float) max;
	}
	else
	{
		pos = 0;
	}

	pos *= scrollPtr->startPos;

	return item->window.rect.y + 1 + SCROLLBAR_SIZE + pos;
}

int Item_TextScroll_ThumbDrawPosition ( itemDef_t *item )
{
	int min, max;

	if (itemCapture == item)
	{
		min = item->window.rect.y + SCROLLBAR_SIZE + 1;
		max = item->window.rect.y + item->window.rect.h - 2*SCROLLBAR_SIZE - 1;

		if (DC->cursory >= min + SCROLLBAR_SIZE/2 && DC->cursory <= max + SCROLLBAR_SIZE/2)
		{
			return DC->cursory - SCROLLBAR_SIZE/2;
		}

		return Item_TextScroll_ThumbPosition(item);
	}

	return Item_TextScroll_ThumbPosition(item);
}

int Item_TextScroll_OverLB ( itemDef_t *item, float x, float y )
{
	rectDef_t		r;
	int				thumbstart;

	r.x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
	r.y = item->window.rect.y;
	r.h = r.w = SCROLLBAR_SIZE;
	if (Rect_ContainsPoint(&r, x, y))
	{
		return WINDOW_LB_LEFTARROW;
	}

	r.y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
	if (Rect_ContainsPoint(&r, x, y))
	{
		return WINDOW_LB_RIGHTARROW;
	}

	thumbstart = Item_TextScroll_ThumbPosition(item);
	r.y = thumbstart;
	if (Rect_ContainsPoint(&r, x, y))
	{
		return WINDOW_LB_THUMB;
	}

	r.y = item->window.rect.y + SCROLLBAR_SIZE;
	r.h = thumbstart - r.y;
	if (Rect_ContainsPoint(&r, x, y))
	{
		return WINDOW_LB_PGUP;
	}

	r.y = thumbstart + SCROLLBAR_SIZE;
	r.h = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
	if (Rect_ContainsPoint(&r, x, y))
	{
		return WINDOW_LB_PGDN;
	}

	return 0;
}

void Item_TextScroll_MouseEnter (itemDef_t *item, float x, float y)
{
	item->window.flags &= ~(WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN);
	item->window.flags |= Item_TextScroll_OverLB(item, x, y);
}

bool Item_TextScroll_HandleKey ( itemDef_t *item, int key, bool down, bool force)
{
	textScrollDef_t *scrollPtr = item->typeData.textscroll;
	int				max;
	int				viewmax;

	if (force || (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS))
	{
		max = Item_TextScroll_MaxScroll(item);

		viewmax = (item->window.rect.h / scrollPtr->lineHeight);
		if ( key == A_CURSOR_UP || key == A_KP_8 )
		{
			scrollPtr->startPos--;
			if (scrollPtr->startPos < 0)
			{
				scrollPtr->startPos = 0;
			}
			return true;
		}

		if ( key == A_CURSOR_DOWN || key == A_KP_2 )
		{
			scrollPtr->startPos++;
			if (scrollPtr->startPos > max)
			{
				scrollPtr->startPos = max;
			}

			return true;
		}

		if ( key == A_MWHEELUP )
		{
			int count = trap->Key_IsDown( A_CTRL ) ? 5 : 1;
			scrollPtr->startPos -= count;
			if (scrollPtr->startPos < 0)
			{
				scrollPtr->startPos = 0;
				Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
				return false;
			}
			Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
			return true;
		}
		if ( key == A_MWHEELDOWN )
		{
			int count = trap->Key_IsDown( A_CTRL ) ? 5 : 1;
			scrollPtr->startPos += count;
			if (scrollPtr->startPos > max)
			{
				scrollPtr->startPos = max;
				Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
				return false;
			}
			Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
			return true;
		}

		// mouse hit
		if (key == A_MOUSE1 || key == A_MOUSE2)
		{
			if (item->window.flags & WINDOW_LB_LEFTARROW)
			{
				scrollPtr->startPos--;
				if (scrollPtr->startPos < 0)
				{
					scrollPtr->startPos = 0;
				}
			}
			else if (item->window.flags & WINDOW_LB_RIGHTARROW)
			{
				// one down
				scrollPtr->startPos++;
				if (scrollPtr->startPos > max)
				{
					scrollPtr->startPos = max;
				}
			}
			else if (item->window.flags & WINDOW_LB_PGUP)
			{
				// page up
				scrollPtr->startPos -= viewmax;
				if (scrollPtr->startPos < 0)
				{
					scrollPtr->startPos = 0;
				}
			}
			else if (item->window.flags & WINDOW_LB_PGDN)
			{
				// page down
				scrollPtr->startPos += viewmax;
				if (scrollPtr->startPos > max)
				{
					scrollPtr->startPos = max;
				}
			}
			else if (item->window.flags & WINDOW_LB_THUMB)
			{
				// Display_SetCaptureItem(item);
			}

			return true;
		}

		if ( key == A_HOME || key == A_KP_7)
		{
			// home
			scrollPtr->startPos = 0;
			return true;
		}
		if ( key == A_END || key == A_KP_1)
		{
			// end
			scrollPtr->startPos = max;
			return true;
		}
		if (key == A_PAGE_UP || key == A_KP_9 )
		{
			scrollPtr->startPos -= viewmax;
			if (scrollPtr->startPos < 0)
			{
					scrollPtr->startPos = 0;
			}

			return true;
		}
		if ( key == A_PAGE_DOWN || key == A_KP_3 )
		{
			scrollPtr->startPos += viewmax;
			if (scrollPtr->startPos > max)
			{
				scrollPtr->startPos = max;
			}
			return true;
		}
	}

	return false;
}

int Item_ListBox_MaxScroll(itemDef_t *item) {
	listBoxDef_t *listPtr = item->typeData.listbox;
	int count = DC->feederCount(item->special);
	int max;

	if (item->window.flags & WINDOW_HORIZONTAL) {
		max = count - (item->window.rect.w / listPtr->elementWidth) + 1;
	}
	else {
		max = count - (item->window.rect.h / listPtr->elementHeight) + 1;
	}
	if (max < 0) {
		return 0;
	}
	return max;
}

int Item_ListBox_ThumbPosition(itemDef_t *item) {
	float max, pos, size;
	listBoxDef_t *listPtr = item->typeData.listbox;

	max = Item_ListBox_MaxScroll(item);
	if (item->window.flags & WINDOW_HORIZONTAL) {
		size = item->window.rect.w - (SCROLLBAR_SIZE * 2) - 2;
		if (max > 0) {
			pos = (size-SCROLLBAR_SIZE) / (float) max;
		} else {
			pos = 0;
		}
		pos *= listPtr->startPos;
		return item->window.rect.x + 1 + SCROLLBAR_SIZE + pos;
	}
	else {
		size = item->window.rect.h - (SCROLLBAR_SIZE * 2) - 2;
		if (max > 0) {
			pos = (size-SCROLLBAR_SIZE) / (float) max;
		} else {
			pos = 0;
		}
		pos *= listPtr->startPos;
		return item->window.rect.y + 1 + SCROLLBAR_SIZE + pos;
	}
}

int Item_ListBox_ThumbDrawPosition(itemDef_t *item)
{
	int min, max;

	if (itemCapture == item)
	{
		if (item->window.flags & WINDOW_HORIZONTAL)
		{
			min = item->window.rect.x + SCROLLBAR_SIZE + 1;
			max = item->window.rect.x + item->window.rect.w - 2*SCROLLBAR_SIZE - 1;
			if (DC->cursorx >= min + SCROLLBAR_SIZE/2 && DC->cursorx <= max + SCROLLBAR_SIZE/2)
			{
				return DC->cursorx - SCROLLBAR_SIZE/2;
			}
			else
			{
				return Item_ListBox_ThumbPosition(item);
			}
		}
		else
		{
			min = item->window.rect.y + SCROLLBAR_SIZE + 1;
			max = item->window.rect.y + item->window.rect.h - 2*SCROLLBAR_SIZE - 1;
			if (DC->cursory >= min + SCROLLBAR_SIZE/2 && DC->cursory <= max + SCROLLBAR_SIZE/2)
			{
				return DC->cursory - SCROLLBAR_SIZE/2;
			}
			else
			{
				return Item_ListBox_ThumbPosition(item);
			}
		}
	}
	else
	{
		return Item_ListBox_ThumbPosition(item);
	}
}

float Item_Slider_ThumbPosition(itemDef_t *item) {
	float value, range, x;
	editFieldDef_t *editDef = item->typeData.edit;

	if (item->text) {
		x = item->textRect.x + item->textRect.w + 8;
	} else {
		x = item->window.rect.x;
	}

	if (!editDef || !item->cvar) {
		return x;
	}

	value = DC->getCVarValue(item->cvar);

	if (value < editDef->minVal) {
		value = editDef->minVal;
	} else if (value > editDef->maxVal) {
		value = editDef->maxVal;
	}

	range = editDef->maxVal - editDef->minVal;
	value -= editDef->minVal;
	value /= range;
	//value /= (editDef->maxVal - editDef->minVal);
	value *= SLIDER_WIDTH;
	x += value;
	// vm fuckage
	//x = x + (((float)value / editDef->maxVal) * SLIDER_WIDTH);
	return x;
}

int Item_Slider_OverSlider(itemDef_t *item, float x, float y) {
	rectDef_t r;

	r.x = Item_Slider_ThumbPosition(item) - (SLIDER_THUMB_WIDTH / 2);
	r.y = item->window.rect.y - 2;
	r.w = SLIDER_THUMB_WIDTH;
	r.h = SLIDER_THUMB_HEIGHT;

	if (Rect_ContainsPoint(&r, x, y)) {
		return WINDOW_LB_THUMB;
	}
	return 0;
}

int Item_ListBox_OverLB(itemDef_t *item, float x, float y)
{
	rectDef_t r;
	listBoxDef_t *listPtr;
	int thumbstart;

	listPtr = item->typeData.listbox;
	if (item->window.flags & WINDOW_HORIZONTAL)
	{
		// check if on left arrow
		r.x = item->window.rect.x;
		r.y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
		r.h = r.w = SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y))
		{
			return WINDOW_LB_LEFTARROW;
		}

		// check if on right arrow
		r.x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y))
		{
			return WINDOW_LB_RIGHTARROW;
		}

		// check if on thumb
		thumbstart = Item_ListBox_ThumbPosition(item);
		r.x = thumbstart;
		if (Rect_ContainsPoint(&r, x, y))
		{
			return WINDOW_LB_THUMB;
		}

		r.x = item->window.rect.x + SCROLLBAR_SIZE;
		r.w = thumbstart - r.x;
		if (Rect_ContainsPoint(&r, x, y))
		{
			return WINDOW_LB_PGUP;
		}

		r.x = thumbstart + SCROLLBAR_SIZE;
		r.w = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y))
		{
			return WINDOW_LB_PGDN;
		}
	}
	// Vertical Scroll
	else
	{
		// Multiple rows and columns (since it's more than twice as wide as an element)
		if (( item->window.rect.w > (listPtr->elementWidth*2)) &&  (listPtr->elementStyle == LISTBOX_IMAGE))
		{
			r.x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
			r.y = item->window.rect.y;
			r.h = r.w = SCROLLBAR_SIZE;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_PGUP;
			}

			r.y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_PGDN;
			}

			thumbstart = Item_ListBox_ThumbPosition(item);
			r.y = thumbstart;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_THUMB;
			}

		}
		else
		{
			r.x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
			r.y = item->window.rect.y;
			r.h = r.w = SCROLLBAR_SIZE;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_LEFTARROW;
			}

			r.y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_RIGHTARROW;
			}

			thumbstart = Item_ListBox_ThumbPosition(item);
			r.y = thumbstart;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_THUMB;
			}

			r.y = item->window.rect.y + SCROLLBAR_SIZE;
			r.h = thumbstart - r.y;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_PGUP;
			}

			r.y = thumbstart + SCROLLBAR_SIZE;
			r.h = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
			if (Rect_ContainsPoint(&r, x, y))
			{
				return WINDOW_LB_PGDN;
			}
		}
	}
	return 0;
}

void Item_ListBox_MouseEnter(itemDef_t *item, float x, float y)
{
	rectDef_t r;
	listBoxDef_t *listPtr = item->typeData.listbox;

	item->window.flags &= ~(WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN);
	item->window.flags |= Item_ListBox_OverLB(item, x, y);

	if (item->window.flags & WINDOW_HORIZONTAL)
	{
		if (!(item->window.flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN)))
		{
			// check for selection hit as we have exausted buttons and thumb
			if (listPtr->elementStyle == LISTBOX_IMAGE)
			{
				r.x = item->window.rect.x;
				r.y = item->window.rect.y;
				r.h = item->window.rect.h - SCROLLBAR_SIZE;
				r.w = item->window.rect.w - listPtr->drawPadding;
				if (Rect_ContainsPoint(&r, x, y))
				{
					listPtr->cursorPos =  (int)((x - r.x) / listPtr->elementWidth)  + listPtr->startPos;
					if (listPtr->cursorPos >= listPtr->endPos)
					{
						listPtr->cursorPos = listPtr->endPos;
					}
				}
			}
			else
			{
				// text hit..
			}
		}
	}
	// Window Vertical Scroll
	else if (!(item->window.flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN)))
	{
		// Calc which element the mouse is over
		r.x = item->window.rect.x;
		r.y = item->window.rect.y;
		r.w = item->window.rect.w - SCROLLBAR_SIZE;
		r.h = item->window.rect.h - listPtr->drawPadding;
		if (Rect_ContainsPoint(&r, x, y))
		{
			// Multiple rows and columns (since it's more than twice as wide as an element)
			if (( item->window.rect.w > (listPtr->elementWidth*2)) &&  (listPtr->elementStyle == LISTBOX_IMAGE))
			{
				int row,column,rowLength;

				row = (int)((y - 2 - r.y) / listPtr->elementHeight);
				rowLength = (int) r.w / listPtr->elementWidth;
				column = (int)((x - r.x) / listPtr->elementWidth);

				listPtr->cursorPos = (row * rowLength)+column  + listPtr->startPos;
				if (listPtr->cursorPos >= listPtr->endPos)
				{
					listPtr->cursorPos = listPtr->endPos;
				}
			}
			// single column
			else
			{
				listPtr->cursorPos =  (int)((y - 2 - r.y) / listPtr->elementHeight)  + listPtr->startPos;
				if (listPtr->cursorPos > listPtr->endPos)
				{
                    listPtr->cursorPos = listPtr->endPos;
				}
			}
		}
	}
}

void Item_MouseEnter(itemDef_t *item, float x, float y) {

	rectDef_t r;
	if (item) {
		r = item->textRect;
		r.y -= r.h;
		// in the text rect?

		// items can be enabled and disabled
		if (item->disabled)
		{
			return;
		}

		// items can be enabled and disabled based on cvars
		if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
			return;
		}

		if (item->cvarFlags & (CVAR_SHOW | CVAR_HIDE) && !Item_EnableShowViaCvar(item, CVAR_SHOW)) {
			return;
		}

		if (Rect_ContainsPoint(&r, x, y))
		{
			if (!(item->window.flags & WINDOW_MOUSEOVERTEXT)) {
				Item_RunScript(item, item->mouseEnterText);
				item->window.flags |= WINDOW_MOUSEOVERTEXT;
			}
			if (!(item->window.flags & WINDOW_MOUSEOVER)) {
				Item_RunScript(item, item->mouseEnter);
				item->window.flags |= WINDOW_MOUSEOVER;
			}

		} else {
			// not in the text rect
			if (item->window.flags & WINDOW_MOUSEOVERTEXT) {
				// if we were
				Item_RunScript(item, item->mouseExitText);
				item->window.flags &= ~WINDOW_MOUSEOVERTEXT;
			}
			if (!(item->window.flags & WINDOW_MOUSEOVER)) {
				Item_RunScript(item, item->mouseEnter);
				item->window.flags |= WINDOW_MOUSEOVER;
			}

			if (item->type == ITEM_TYPE_LISTBOX) {
				Item_ListBox_MouseEnter(item, x, y);
			}
			else if ( item->type == ITEM_TYPE_TEXTSCROLL )
			{
				Item_TextScroll_MouseEnter ( item, x, y );
			}
		}
	}
}

void Item_MouseLeave(itemDef_t *item) {
  if (item) {
    if (item->window.flags & WINDOW_MOUSEOVERTEXT) {
      Item_RunScript(item, item->mouseExitText);
      item->window.flags &= ~WINDOW_MOUSEOVERTEXT;
    }
    Item_RunScript(item, item->mouseExit);
    item->window.flags &= ~(WINDOW_LB_RIGHTARROW | WINDOW_LB_LEFTARROW);
  }
}

itemDef_t *Menu_HitTest(menuDef_t *menu, float x, float y) {
  int i;
  for (i = 0; i < menu->itemCount; i++) {
    if (Rect_ContainsPoint(&menu->items[i]->window.rect, x, y)) {
      return menu->items[i];
    }
  }
  return nullptr;
}

void Item_SetMouseOver(itemDef_t *item, bool focus) {
  if (item) {
    if (focus) {
      item->window.flags |= WINDOW_MOUSEOVER;
    } else {
      item->window.flags &= ~WINDOW_MOUSEOVER;
    }
  }
}

bool Item_OwnerDraw_HandleKey(itemDef_t *item, int key) {
  if (item && DC->ownerDrawHandleKey)
  {

	  // yep this is an ugly hack
	  if( key == A_MOUSE1 || key == A_MOUSE2 )
	  {
		switch( item->window.ownerDraw )
		{
			case UI_FORCE_SIDE:
			case UI_FORCE_RANK_HEAL:
			case UI_FORCE_RANK_LEVITATION:
			case UI_FORCE_RANK_SPEED:
			case UI_FORCE_RANK_PUSH:
			case UI_FORCE_RANK_PULL:
			case UI_FORCE_RANK_TELEPATHY:
			case UI_FORCE_RANK_GRIP:
			case UI_FORCE_RANK_LIGHTNING:
			case UI_FORCE_RANK_RAGE:
			case UI_FORCE_RANK_PROTECT:
			case UI_FORCE_RANK_ABSORB:
			case UI_FORCE_RANK_TEAM_HEAL:
			case UI_FORCE_RANK_TEAM_FORCE:
			case UI_FORCE_RANK_DRAIN:
			case UI_FORCE_RANK_SEE:
			case UI_FORCE_RANK_SABERATTACK:
			case UI_FORCE_RANK_SABERDEFEND:
			case UI_FORCE_RANK_SABERTHROW:
				if(!Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) )
				{
					return false;
				}
				break;
		}
	  }

    return DC->ownerDrawHandleKey(item->window.ownerDraw, item->window.ownerDrawFlags, &item->special, key);
  }
  return false;
}

bool Item_ListBox_HandleKey(itemDef_t *item, int key, bool down, bool force) {
	listBoxDef_t *listPtr = item->typeData.listbox;
	int count = DC->feederCount(item->special);
	int max, viewmax;
	if (force || (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS))
	{
		max = Item_ListBox_MaxScroll(item);
		if (item->window.flags & WINDOW_HORIZONTAL) {
			viewmax = (item->window.rect.w / listPtr->elementWidth);
			if ( key == A_CURSOR_LEFT || key == A_KP_4 )
			{
				if (!listPtr->notselectable) {
					listPtr->cursorPos--;
					if (listPtr->cursorPos < 0) {
						listPtr->cursorPos = 0;
						return false;
					}
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
						return false;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos, nullptr);
				}
				else {
					listPtr->startPos--;
					if (listPtr->startPos < 0)
						listPtr->startPos = 0;
				}
				return true;
			}
			if ( key == A_CURSOR_RIGHT || key == A_KP_6 )
			{
				if (!listPtr->notselectable) {
					listPtr->cursorPos++;
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
						return false;
					}
					if (listPtr->cursorPos >= count) {
						listPtr->cursorPos = count-1;
						return false;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos, nullptr);
				}
				else {
					listPtr->startPos++;
					if (listPtr->startPos >= count)
						listPtr->startPos = count-1;
				}
				return true;
			}
		}
		// Vertical scroll
		else {

			// Multiple rows and columns (since it's more than twice as wide as an element)
			if (( item->window.rect.w > (listPtr->elementWidth*2)) &&  (listPtr->elementStyle == LISTBOX_IMAGE))
			{
				viewmax = (item->window.rect.w / listPtr->elementWidth);
			}
			else
			{
				viewmax = (item->window.rect.h / listPtr->elementHeight);
			}

			if ( key == A_CURSOR_UP || key == A_KP_8 )
			{
				if (!listPtr->notselectable) {
					listPtr->cursorPos--;
					if (listPtr->cursorPos < 0) {
						listPtr->cursorPos = 0;
						return false;
					}
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
						return false;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos, nullptr);
				}
				else {
					listPtr->startPos--;
					if (listPtr->startPos < 0)
						listPtr->startPos = 0;
				}
				return true;
			}
			if ( key == A_CURSOR_DOWN || key == A_KP_2 )
			{
				if (!listPtr->notselectable) {
					listPtr->cursorPos++;
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
						return false;
					}
					if (listPtr->cursorPos >= count) {
						listPtr->cursorPos = count-1;
						return false;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos, nullptr);
				}
				else {
					listPtr->startPos++;
					if (listPtr->startPos > max)
						listPtr->startPos = max;
				}
				return true;
			}

			if ( key == A_MWHEELUP )
			{
				int count = trap->Key_IsDown( A_CTRL ) ? 5 : 1;
				listPtr->startPos -= ((int)item->special == FEEDER_Q3HEADS) ? viewmax : count;
				if (listPtr->startPos < 0)
				{
					listPtr->startPos = 0;
					Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
					return false;
				}
				Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
				return true;
			}
			if ( key == A_MWHEELDOWN )
			{
				int count = trap->Key_IsDown( A_CTRL ) ? 5 : 1;
				listPtr->startPos += ((int)item->special == FEEDER_Q3HEADS) ? viewmax : count;
				if (listPtr->startPos > max)
				{
					listPtr->startPos = max;
					Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
					return false;
				}
				Display_MouseMove(nullptr, DC->cursorx, DC->cursory);
				return true;
			}
		}
		// mouse hit
		if (key == A_MOUSE1 || key == A_MOUSE2) {
			if (item->window.flags & WINDOW_LB_LEFTARROW) {
				listPtr->startPos--;
				if (listPtr->startPos < 0) {
					listPtr->startPos = 0;
				}
			} else if (item->window.flags & WINDOW_LB_RIGHTARROW) {
				// one down
				listPtr->startPos++;
				if (listPtr->startPos > max) {
					listPtr->startPos = max;
				}
			} else if (item->window.flags & WINDOW_LB_PGUP) {
				// page up
				listPtr->startPos -= viewmax;
				if (listPtr->startPos < 0) {
					listPtr->startPos = 0;
				}
			} else if (item->window.flags & WINDOW_LB_PGDN) {
				// page down
				listPtr->startPos += viewmax;
				if (listPtr->startPos > (max)) {
					listPtr->startPos = (max);
				}
			} else if (item->window.flags & WINDOW_LB_THUMB) {
				// Display_SetCaptureItem(item);
			} else {
				// select an item
				if (DC->realTime < lastListBoxClickTime && listPtr->doubleClick) {
					Item_RunScript(item, listPtr->doubleClick);
				}
				lastListBoxClickTime = DC->realTime + DOUBLE_CLICK_DELAY;
//				if (item->cursorPos != listPtr->cursorPos)
				{
					int prePos = item->cursorPos;

					item->cursorPos = listPtr->cursorPos;

					if (!DC->feederSelection(item->special, item->cursorPos, item))
					{
						item->cursorPos = listPtr->cursorPos = prePos;
					}
				}
			}
			return true;
		}
		if ( key == A_HOME || key == A_KP_7) {
			// home
			listPtr->startPos = 0;
			return true;
		}
		if ( key == A_END || key == A_KP_1) {
			// end
			listPtr->startPos = max;
			return true;
		}
		if (key == A_PAGE_UP || key == A_KP_9 ) {
			// page up
			if (!listPtr->notselectable) {
				listPtr->cursorPos -= viewmax;
				if (listPtr->cursorPos < 0) {
					listPtr->cursorPos = 0;
				}
				if (listPtr->cursorPos < listPtr->startPos) {
					listPtr->startPos = listPtr->cursorPos;
				}
				if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
					listPtr->startPos = listPtr->cursorPos - viewmax + 1;
				}
				item->cursorPos = listPtr->cursorPos;
				DC->feederSelection(item->special, item->cursorPos, nullptr);
			}
			else {
				listPtr->startPos -= viewmax;
				if (listPtr->startPos < 0) {
					listPtr->startPos = 0;
				}
			}
			return true;
		}
		if ( key == A_PAGE_DOWN || key == A_KP_3 ) {
			// page down
			if (!listPtr->notselectable) {
				listPtr->cursorPos += viewmax;
				if (listPtr->cursorPos < listPtr->startPos) {
					listPtr->startPos = listPtr->cursorPos;
				}
				if (listPtr->cursorPos >= count) {
					listPtr->cursorPos = count-1;
				}
				if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
					listPtr->startPos = listPtr->cursorPos - viewmax + 1;
				}
				item->cursorPos = listPtr->cursorPos;
				DC->feederSelection(item->special, item->cursorPos, nullptr);
			}
			else {
				listPtr->startPos += viewmax;
				if (listPtr->startPos > max) {
					listPtr->startPos = max;
				}
			}
			return true;
		}
	}
	return false;
}

bool Item_YesNo_HandleKey(itemDef_t *item, int key) {
  if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS && item->cvar)
	{
		if (key == A_MOUSE1 || key == A_ENTER || key == A_MOUSE2 || key == A_MOUSE3)
		{
	    DC->setCVar(item->cvar, va("%i", !DC->getCVarValue(item->cvar)));
		  return true;
		}
  }

  return false;

}

int Item_Multi_CountSettings(itemDef_t *item) {
	multiDef_t *multiPtr = item->typeData.multi;
	if (multiPtr == nullptr) {
		return 0;
	}
	return multiPtr->count;
}

int Item_Multi_FindCvarByValue(itemDef_t *item) {
	char buff[2048];
	float value = 0;
	int i;
	multiDef_t *multiPtr = item->typeData.multi;
	if (multiPtr) {
		if (multiPtr->strDef) {
	    DC->getCVarString(item->cvar, buff, sizeof(buff));
		} else {
			value = DC->getCVarValue(item->cvar);
		}
		for (i = 0; i < multiPtr->count; i++) {
			if (multiPtr->strDef) {
				if (Q_stricmp(buff, multiPtr->cvarStr[i]) == 0) {
					return i;
				}
			} else {
				if (multiPtr->cvarValue[i] == value) {
					return i;
				}
			}
		}
	}
	return 0;
}

const char *Item_Multi_Setting(itemDef_t *item) {
	char buff[2048];
	float value = 0;
	int i;
	multiDef_t *multiPtr = item->typeData.multi;
	if (multiPtr)
	{
		if (multiPtr->strDef)
		{
			if (item->cvar)
			{
			    DC->getCVarString(item->cvar, buff, sizeof(buff));
			}
		}
		else
		{
			if (item->cvar)	// Was a cvar given?
			{
				value = DC->getCVarValue(item->cvar);
			}
		}

		for (i = 0; i < multiPtr->count; i++)
		{
			if (multiPtr->strDef)
			{
				if (Q_stricmp(buff, multiPtr->cvarStr[i]) == 0)
				{
					return multiPtr->cvarList[i];
				}
			}
			else
			{
				if (multiPtr->cvarValue[i] == value)
				{
					return multiPtr->cvarList[i];
				}
			}
		}
	}
	return "@MENUS_CUSTOM";
}

bool Item_Multi_HandleKey(itemDef_t *item, int key)
{
	multiDef_t *multiPtr = item->typeData.multi;
	if (multiPtr)
	{
		if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS)
		{
			if (key == A_MOUSE1 || key == A_ENTER || key == A_MOUSE2 || key == A_MOUSE3 || key == A_MWHEELDOWN || key == A_MWHEELUP)
			{
				int current = Item_Multi_FindCvarByValue(item);
				int max = Item_Multi_CountSettings(item);

				if (key == A_MOUSE2 || key == A_CURSOR_LEFT || key == A_MWHEELDOWN)	// Xbox uses CURSOR_LEFT
			//	if (key == A_MOUSE2 || key == A_CURSOR_LEFT)	// Xbox uses CURSOR_LEFT
				{
					current--;
					if ( current < 0 )
					{
						current = max-1;
					}
				}
				else
				{
					current++;
					if ( current >= max )
					{
						current = 0;
					}
				}

				if (multiPtr->strDef)
				{
					DC->setCVar(item->cvar, multiPtr->cvarStr[current]);
				}
				else
				{
					float value = multiPtr->cvarValue[current];
					if (((float)((int) value)) == value)
					{
						DC->setCVar(item->cvar, va("%i", (int) value ));
					}
					else
					{
						DC->setCVar(item->cvar, va("%f", value ));
					}
				}
				if (item->special)
				{//its a feeder?
					DC->feederSelection(item->special, current, item);
				}

				return true;
			}
		}
	}
  return false;
}

// Leaving an edit field so reset it so it prints from the beginning.
void Leaving_EditField(itemDef_t *item)
{
	// switching fields so reset printed text of edit field
	if ((g_editingField==true) && (item->type==ITEM_TYPE_EDITFIELD))
	{
		editFieldDef_t *editPtr = item->typeData.edit;
		if (editPtr)
		{
			editPtr->paintOffset = 0;
		}
	}
}

itemDef_t *Menu_SetNextCursorItem(menuDef_t *menu) {

  bool wrapped = false;
	int oldCursor = menu->cursorItem;

  if (menu->cursorItem == -1) {
    menu->cursorItem = 0;
    wrapped = true;
  }

  while (menu->cursorItem < menu->itemCount) {

    menu->cursorItem++;
    if (menu->cursorItem >= menu->itemCount && !wrapped) {
      wrapped = true;
      menu->cursorItem = 0;
    }
		if (Item_SetFocus(menu->items[menu->cursorItem], DC->cursorx, DC->cursory)) {
			Menu_HandleMouseMove(menu, menu->items[menu->cursorItem]->window.rect.x + 1, menu->items[menu->cursorItem]->window.rect.y + 1);
      return menu->items[menu->cursorItem];
    }

  }

	menu->cursorItem = oldCursor;
	return nullptr;
}

itemDef_t *Menu_SetPrevCursorItem(menuDef_t *menu) {
	bool wrapped = false;
	int oldCursor = menu->cursorItem;

	if (menu->cursorItem < 0) {
		menu->cursorItem = menu->itemCount-1;
		wrapped = true;
	}

	while (menu->cursorItem > -1)
	{
		menu->cursorItem--;
		if (menu->cursorItem < 0) {
			if (wrapped)
			{
				break;
			}
			wrapped = true;
			menu->cursorItem = menu->itemCount -1;
		}

		if (Item_SetFocus(menu->items[menu->cursorItem], DC->cursorx, DC->cursory)) {
			Menu_HandleMouseMove(menu, menu->items[menu->cursorItem]->window.rect.x + 1, menu->items[menu->cursorItem]->window.rect.y + 1);
			return menu->items[menu->cursorItem];
		}
	}
	menu->cursorItem = oldCursor;
	return nullptr;

}

#ifdef BUILD_UI
bool Item_TextField_HandleKey( itemDef_t *item, int key );
static void Item_TextField_Paste( itemDef_t *item ) {
	int		pasteLen, i;
	char	buff[2048] = { 0 };

	trap->GetClipboardData( buff, sizeof(buff) );

	if ( !*buff ) {
		return;
	}

	// send as if typed, so insert / overstrike works properly
	pasteLen = strlen( buff );
	for ( i = 0; i < pasteLen; i++ ) {
		Item_TextField_HandleKey( item, buff[i]|K_CHAR_FLAG );
	}
}
#endif

bool Item_TextField_HandleKey(itemDef_t *item, int key) {
	char buff[2048];
	int len;
	itemDef_t *newItem = nullptr;
	editFieldDef_t *editPtr = item->typeData.edit;

	if (item->cvar) {

		buff[0] = 0;
		DC->getCVarString(item->cvar, buff, sizeof(buff));
		len = strlen(buff);
		if (editPtr->maxChars && len > editPtr->maxChars) {
			len = editPtr->maxChars;
		}
		if ( key & K_CHAR_FLAG ) {
			key &= ~K_CHAR_FLAG;

#ifdef BUILD_UI
			if ( key == 'v' - 'a' + 1 ) {	// ctrl-v is paste
				Item_TextField_Paste( item );
				return true;
			}
#endif

			if (key == 'h' - 'a' + 1 )	{	// ctrl-h is backspace
				if ( item->cursorPos > 0 ) {
					memmove( &buff[item->cursorPos - 1], &buff[item->cursorPos], len + 1 - item->cursorPos);
					item->cursorPos--;
					if (item->cursorPos < editPtr->paintOffset) {
						editPtr->paintOffset--;
					}
				}
				DC->setCVar(item->cvar, buff);
				return true;
			}

			// ignore any non printable chars

			if ( key < 32 || !item->cvar) {
			    return true;
		    }

			if (item->type == ITEM_TYPE_NUMERICFIELD) {
				if (key < '0' || key > '9') {
					return false;
				}
			}

			if (!DC->getOverstrikeMode()) {
				if (( len == MAX_EDITFIELD - 1 ) || (editPtr->maxChars && len >= editPtr->maxChars)) {
					return true;
				}
				memmove( &buff[item->cursorPos + 1], &buff[item->cursorPos], len + 1 - item->cursorPos );
			} else {
				if (editPtr->maxChars && item->cursorPos >= editPtr->maxChars) {
					return true;
				}
			}

			buff[item->cursorPos] = key;

			//rww - nul-terminate!
			if (item->cursorPos+1 < 2048)
			{
				buff[item->cursorPos+1] = 0;
			}
			else
			{
				buff[item->cursorPos] = 0;
			}

			DC->setCVar(item->cvar, buff);

			if (item->cursorPos < len + 1) {
				item->cursorPos++;
				if (editPtr->maxPaintChars && item->cursorPos > editPtr->maxPaintChars) {
					editPtr->paintOffset++;
				}
			}

		} else {

			if ( key == A_DELETE || key == A_KP_PERIOD ) {
				if ( item->cursorPos < len ) {
					memmove( buff + item->cursorPos, buff + item->cursorPos + 1, len - item->cursorPos);
					DC->setCVar(item->cvar, buff);
				}
				return true;
			}

			if ( key == A_CURSOR_RIGHT || key == A_KP_6 )
			{
				if (editPtr->maxPaintChars && item->cursorPos >= editPtr->maxPaintChars && item->cursorPos < len) {
					item->cursorPos++;
					editPtr->paintOffset++;
					return true;
				}
				if (item->cursorPos < len) {
					item->cursorPos++;
				}
				return true;
			}

			if ( key == A_CURSOR_LEFT || key == A_KP_4 )
			{
				if ( item->cursorPos > 0 ) {
					item->cursorPos--;
				}
				if (item->cursorPos < editPtr->paintOffset) {
					editPtr->paintOffset--;
				}
				return true;
			}

			if ( key == A_HOME || key == A_KP_7) {// || ( tolower(key) == 'a' && trap->Key_IsDown( K_CTRL ) ) ) {
				item->cursorPos = 0;
				editPtr->paintOffset = 0;
				return true;
			}

			if ( key == A_END || key == A_KP_1)  {// ( tolower(key) == 'e' && trap->Key_IsDown( K_CTRL ) ) ) {
				item->cursorPos = len;
				if(item->cursorPos > editPtr->maxPaintChars) {
					editPtr->paintOffset = len - editPtr->maxPaintChars;
				}
				return true;
			}

			if ( key == A_INSERT || key == A_KP_0 ) {
				DC->setOverstrikeMode(!DC->getOverstrikeMode());
				return true;
			}
		}

		if (key == A_TAB || key == A_CURSOR_DOWN || key == A_KP_2)
		{
			// switching fields so reset printed text of edit field
			Leaving_EditField(item);
			g_editingField = false;
			newItem = Menu_SetNextCursorItem((menuDef_t *) item->parent);
			if (newItem && (newItem->type == ITEM_TYPE_EDITFIELD || newItem->type == ITEM_TYPE_NUMERICFIELD))
			{
				g_editItem = newItem;
				g_editingField = true;
			}
		}

		if (key == A_CURSOR_UP || key == A_KP_8)
		{
			// switching fields so reset printed text of edit field
			Leaving_EditField(item);

			g_editingField = false;
			newItem = Menu_SetPrevCursorItem((menuDef_t *) item->parent);
			if (newItem && (newItem->type == ITEM_TYPE_EDITFIELD || newItem->type == ITEM_TYPE_NUMERICFIELD))
			{
				g_editItem = newItem;
				g_editingField = true;
			}
		}

	//	if ( key == A_ENTER || key == A_KP_ENTER || key == A_ESCAPE)  {
		if ( key == A_ENTER || key == A_KP_ENTER || key == A_ESCAPE || (key == A_MOUSE1 && !Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) )) {
			DC->setOverstrikeMode( false );
			return false;
		}

		return true;
	}
	return false;

}

static void Scroll_TextScroll_AutoFunc (void *p)
{
	scrollInfo_t *si = (scrollInfo_t*)p;

	if (DC->realTime > si->nextScrollTime)
	{
		// need to scroll which is done by simulating a click to the item
		// this is done a bit sideways as the autoscroll "knows" that the item is a listbox
		// so it calls it directly
		Item_TextScroll_HandleKey(si->item, si->scrollKey, true, false);
		si->nextScrollTime = DC->realTime + si->adjustValue;
	}

	if (DC->realTime > si->nextAdjustTime)
	{
		si->nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
		if (si->adjustValue > SCROLL_TIME_FLOOR)
		{
			si->adjustValue -= SCROLL_TIME_ADJUSTOFFSET;
		}
	}
}

static void Scroll_TextScroll_ThumbFunc(void *p)
{
	scrollInfo_t *si = (scrollInfo_t*)p;
	rectDef_t	 r;
	int			 pos;
	int			 max;

	textScrollDef_t *scrollPtr = si->item->typeData.textscroll;

	if (DC->cursory != si->yStart)
	{
		r.x = si->item->window.rect.x + si->item->window.rect.w - SCROLLBAR_SIZE - 1;
		r.y = si->item->window.rect.y + SCROLLBAR_SIZE + 1;
		r.h = si->item->window.rect.h - (SCROLLBAR_SIZE*2) - 2;
		r.w = SCROLLBAR_SIZE;
		max = Item_TextScroll_MaxScroll(si->item);

		pos = (DC->cursory - r.y - SCROLLBAR_SIZE/2) * max / (r.h - SCROLLBAR_SIZE);
		if (pos < 0)
		{
			pos = 0;
		}
		else if (pos > max)
		{
			pos = max;
		}

		scrollPtr->startPos = pos;
		si->yStart = DC->cursory;
	}

	if (DC->realTime > si->nextScrollTime)
	{
		// need to scroll which is done by simulating a click to the item
		// this is done a bit sideways as the autoscroll "knows" that the item is a listbox
		// so it calls it directly
		Item_TextScroll_HandleKey(si->item, si->scrollKey, true, false);
		si->nextScrollTime = DC->realTime + si->adjustValue;
	}

	if (DC->realTime > si->nextAdjustTime)
	{
		si->nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
		if (si->adjustValue > SCROLL_TIME_FLOOR)
		{
			si->adjustValue -= SCROLL_TIME_ADJUSTOFFSET;
		}
	}
}

static void Scroll_ListBox_AutoFunc(void *p) {
	scrollInfo_t *si = (scrollInfo_t*)p;
	if (DC->realTime > si->nextScrollTime) {
		// need to scroll which is done by simulating a click to the item
		// this is done a bit sideways as the autoscroll "knows" that the item is a listbox
		// so it calls it directly
		Item_ListBox_HandleKey(si->item, si->scrollKey, true, false);
		si->nextScrollTime = DC->realTime + si->adjustValue;
	}

	if (DC->realTime > si->nextAdjustTime) {
		si->nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
		if (si->adjustValue > SCROLL_TIME_FLOOR) {
			si->adjustValue -= SCROLL_TIME_ADJUSTOFFSET;
		}
	}
}

static void Scroll_ListBox_ThumbFunc(void *p) {
	scrollInfo_t *si = (scrollInfo_t*)p;
	rectDef_t r;
	int pos, max;

	listBoxDef_t *listPtr = si->item->typeData.listbox;
	if (si->item->window.flags & WINDOW_HORIZONTAL) {
		if (DC->cursorx == si->xStart) {
			return;
		}
		r.x = si->item->window.rect.x + SCROLLBAR_SIZE + 1;
		r.y = si->item->window.rect.y + si->item->window.rect.h - SCROLLBAR_SIZE - 1;
		r.h = SCROLLBAR_SIZE;
		r.w = si->item->window.rect.w - (SCROLLBAR_SIZE*2) - 2;
		max = Item_ListBox_MaxScroll(si->item);

		pos = (DC->cursorx - r.x - SCROLLBAR_SIZE/2) * max / (r.w - SCROLLBAR_SIZE);
		if (pos < 0) {
			pos = 0;
		}
		else if (pos > max) {
			pos = max;
		}
		listPtr->startPos = pos;
		si->xStart = DC->cursorx;
	}
	else if (DC->cursory != si->yStart) {

		r.x = si->item->window.rect.x + si->item->window.rect.w - SCROLLBAR_SIZE - 1;
		r.y = si->item->window.rect.y + SCROLLBAR_SIZE + 1;
		r.h = si->item->window.rect.h - (SCROLLBAR_SIZE*2) - 2;
		r.w = SCROLLBAR_SIZE;
		max = Item_ListBox_MaxScroll(si->item);

		// Multiple rows and columns (since it's more than twice as wide as an element)
		if (( si->item->window.rect.w > (listPtr->elementWidth*2)) &&  (listPtr->elementStyle == LISTBOX_IMAGE))
		{
			int rowLength, rowMax;
			rowLength = si->item->window.rect.w / listPtr->elementWidth;
			rowMax = max / rowLength;

			pos = (DC->cursory - r.y - SCROLLBAR_SIZE/2) * rowMax / (r.h - SCROLLBAR_SIZE);
			pos *= rowLength;
		}
		else
		{
			pos = (DC->cursory - r.y - SCROLLBAR_SIZE/2) * max / (r.h - SCROLLBAR_SIZE);
		}

		if (pos < 0) {
			pos = 0;
		}
		else if (pos > max) {
			pos = max;
		}
		listPtr->startPos = pos;
		si->yStart = DC->cursory;
	}

	if (DC->realTime > si->nextScrollTime) {
		// need to scroll which is done by simulating a click to the item
		// this is done a bit sideways as the autoscroll "knows" that the item is a listbox
		// so it calls it directly
		Item_ListBox_HandleKey(si->item, si->scrollKey, true, false);
		si->nextScrollTime = DC->realTime + si->adjustValue;
	}

	if (DC->realTime > si->nextAdjustTime) {
		si->nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
		if (si->adjustValue > SCROLL_TIME_FLOOR) {
			si->adjustValue -= SCROLL_TIME_ADJUSTOFFSET;
		}
	}
}

static void Scroll_Slider_ThumbFunc(void *p) {
	float x, value, cursorx;
	scrollInfo_t *si = (scrollInfo_t*)p;
	editFieldDef_t *editDef = si->item->typeData.edit;

	if (si->item->text) {
		x = si->item->textRect.x + si->item->textRect.w + 8;
	} else {
		x = si->item->window.rect.x;
	}

	cursorx = DC->cursorx;

	if (cursorx < x) {
		cursorx = x;
	} else if (cursorx > x + SLIDER_WIDTH) {
		cursorx = x + SLIDER_WIDTH;
	}
	value = cursorx - x;
	value /= SLIDER_WIDTH;
	value *= (editDef->maxVal - editDef->minVal);
	value += editDef->minVal;
	DC->setCVar(si->item->cvar, va("%f", value));
}

void Item_StartCapture(itemDef_t *item, int key)
{
	int flags;
	switch (item->type)
	{
	    case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
		case ITEM_TYPE_LISTBOX:
		{
			flags = Item_ListBox_OverLB(item, DC->cursorx, DC->cursory);
			if (flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW)) {
				scrollInfo.nextScrollTime = DC->realTime + SCROLL_TIME_START;
				scrollInfo.nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
				scrollInfo.adjustValue = SCROLL_TIME_START;
				scrollInfo.scrollKey = key;
				scrollInfo.scrollDir = (flags & WINDOW_LB_LEFTARROW) ? true : false;
				scrollInfo.item = item;
				captureData = &scrollInfo;
				captureFunc = &Scroll_ListBox_AutoFunc;
				itemCapture = item;
			} else if (flags & WINDOW_LB_THUMB) {
				scrollInfo.scrollKey = key;
				scrollInfo.item = item;
				scrollInfo.xStart = DC->cursorx;
				scrollInfo.yStart = DC->cursory;
				captureData = &scrollInfo;
				captureFunc = &Scroll_ListBox_ThumbFunc;
				itemCapture = item;
			}
			break;
		}

		case ITEM_TYPE_TEXTSCROLL:
			flags = Item_TextScroll_OverLB (item, DC->cursorx, DC->cursory);
			if (flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW))
			{
				scrollInfo.nextScrollTime = DC->realTime + SCROLL_TIME_START;
				scrollInfo.nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
				scrollInfo.adjustValue = SCROLL_TIME_START;
				scrollInfo.scrollKey = key;
				scrollInfo.scrollDir = (flags & WINDOW_LB_LEFTARROW) ? true : false;
				scrollInfo.item = item;
				captureData = &scrollInfo;
				captureFunc = &Scroll_TextScroll_AutoFunc;
				itemCapture = item;
			}
			else if (flags & WINDOW_LB_THUMB)
			{
				scrollInfo.scrollKey = key;
				scrollInfo.item = item;
				scrollInfo.xStart = DC->cursorx;
				scrollInfo.yStart = DC->cursory;
				captureData = &scrollInfo;
				captureFunc = &Scroll_TextScroll_ThumbFunc;
				itemCapture = item;
			}
			break;

		case ITEM_TYPE_SLIDER:
		{
			flags = Item_Slider_OverSlider(item, DC->cursorx, DC->cursory);
			if (flags & WINDOW_LB_THUMB) {
				scrollInfo.scrollKey = key;
				scrollInfo.item = item;
				scrollInfo.xStart = DC->cursorx;
				scrollInfo.yStart = DC->cursory;
				captureData = &scrollInfo;
				captureFunc = &Scroll_Slider_ThumbFunc;
				itemCapture = item;
			}
			break;
		}
	}
}

void Item_StopCapture(itemDef_t *item) {

}

bool Item_Slider_HandleKey(itemDef_t *item, int key, bool down) {
	float x, value, width, work;

	//DC->Print("slider handle key\n");
	if (item->window.flags & WINDOW_HASFOCUS && item->cvar && Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory)) {
		if (key == A_MOUSE1 || key == A_ENTER || key == A_MOUSE2 || key == A_MOUSE3) {
			editFieldDef_t *editDef = item->typeData.edit;
			if (editDef) {
				rectDef_t testRect;
				width = SLIDER_WIDTH;
				if (item->text) {
					x = item->textRect.x + item->textRect.w + 8;
				} else {
					x = item->window.rect.x;
				}

				testRect = item->window.rect;
				testRect.x = x;
				value = (float)SLIDER_THUMB_WIDTH / 2;
				testRect.x -= value;
				//DC->Print("slider x: %f\n", testRect.x);
				testRect.w = (SLIDER_WIDTH + (float)SLIDER_THUMB_WIDTH / 2);
				//DC->Print("slider w: %f\n", testRect.w);
				if (Rect_ContainsPoint(&testRect, DC->cursorx, DC->cursory)) {
					work = DC->cursorx - x;
					value = work / width;
					value *= (editDef->maxVal - editDef->minVal);
					// vm fuckage
					// value = (((float)(DC->cursorx - x)/ SLIDER_WIDTH) * (editDef->maxVal - editDef->minVal));
					value += editDef->minVal;
					DC->setCVar(item->cvar, va("%f", value));
					return true;
				}
			}
		}
	}
	//DC->Print("slider handle key exit\n");
	return false;
}

static constexpr const char *g_bindCommands[] = {
	"+altattack",
	"+attack",
	"+back",
	"+button2",
	"+force_drain",
	"+force_grip",
	"+force_lightning",
	"+forward",
	"+left",
	"+lookdown",
	"+lookup",
	"+mlook",
	"+movedown",
	"+moveleft",
	"+moveright",
	"+moveup",
	"+right",
	"+scores",
	"+speed",
	"+strafe",
	"+use",
	"+useforce",
	"automap_button",
	"automap_toggle",
	"bow",
	"centerview",
	"cg_thirdperson !",
	"engage_duel",
	"flourish",
	"force_absorb",
	"force_distract",
	"force_forcepowerother",
	"force_heal",
	"force_healother",
	"force_protect",
	"force_pull",
	"force_rage",
	"force_seeing",
	"force_speed",
	"force_throw",
	"forcenext",
	"forceprev",
	"gloat",
	"invnext",
	"invprev",
	"meditate",
	"messagemode",
	"messagemode2",
	"messagemode3",
	"messagemode4",
	"saberAttackCycle",
	"taunt",
	"use_bacta",
	"use_electrobinoculars",
	"use_field",
	"use_seeker",
	"use_sentry",
	"voicechat",
	"weapnext",
	"weapon 1",
	"weapon 10",
	"weapon 11",
	"weapon 12",
	"weapon 13",
	"weapon 2",
	"weapon 3",
	"weapon 4",
	"weapon 5",
	"weapon 6",
	"weapon 7",
	"weapon 8",
	"weapon 9",
	"weapprev",
	"zoom"
};

#define g_bindCount ARRAY_LEN(g_bindCommands)

static int g_bindKeys[g_bindCount][2];

int BindingIDFromName( const char *name ) {
	size_t i;

	// iterate each command, set its default binding
	for ( i=0; i < g_bindCount; i++ ) {
		if ( !Q_stricmp( name, g_bindCommands[i] ) )
			return i;
	}
	return -1;
}

bool Item_Bind_HandleKey(itemDef_t *item, int key, bool down) {
	int id;

	if ( key == A_MOUSE1 && Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && !g_waitingForKey ) {
		if (down) {
			g_waitingForKey = true;
			g_bindItem = item;
		}
		return true;
	}
	else if ( key == A_ENTER && !g_waitingForKey ) {
		if ( down ) {
			g_waitingForKey = true;
			g_bindItem = item;
		}
		return true;
	}
	else {
		if ( !g_waitingForKey || g_bindItem == nullptr ) {
			return false;
		}

		if ( key & K_CHAR_FLAG ) {
			return true;
		}

		switch ( key ) {
			case A_ESCAPE:
				g_waitingForKey = false;
				return true;

			case A_BACKSPACE:
				id = BindingIDFromName(item->cvar);
				if (id != -1)
				{
					if ( g_bindKeys[id][0] != -1 )
						DC->setBinding(g_bindKeys[id][0], "");

					if ( g_bindKeys[id][1] != -1 )
						DC->setBinding(g_bindKeys[id][1], "");

					g_bindKeys[id][0] = -1;
					g_bindKeys[id][1] = -1;
				}
				Controls_SetConfig();
				g_waitingForKey = false;
				g_bindItem = nullptr;
				return true;

			case '`':
				return true;
		}
	}

	if ( key != -1 ) {
		size_t	i;

		for ( i=0; i<g_bindCount; i++ )
		{
			if ( g_bindKeys[i][1] == key )
				g_bindKeys[i][1] = -1;

			if ( g_bindKeys[i][0] == key ) {
				g_bindKeys[i][0] = g_bindKeys[i][1];
				g_bindKeys[i][1] = -1;
			}
		}
	}

	id = BindingIDFromName( item->cvar );

	if ( id != -1 ) {
		if ( key == -1 ) {
			if( g_bindKeys[id][0] != -1 ) {
				DC->setBinding( g_bindKeys[id][0], "" );
				g_bindKeys[id][0] = -1;
			}
			if( g_bindKeys[id][1] != -1 ) {
				DC->setBinding( g_bindKeys[id][1], "" );
				g_bindKeys[id][1] = -1;
			}
		}
		else if ( g_bindKeys[id][0] == -1 )
			g_bindKeys[id][0] = key;
		else if ( g_bindKeys[id][0] != key && g_bindKeys[id][1] == -1 )
			g_bindKeys[id][1] = key;
		else {
			DC->setBinding( g_bindKeys[id][0], "" );
			DC->setBinding( g_bindKeys[id][1], "" );
			g_bindKeys[id][0] = key;
			g_bindKeys[id][1] = -1;
		}
	}

	Controls_SetConfig();
	g_waitingForKey = false;

	return true;
}

bool Item_HandleKey(itemDef_t *item, int key, bool down) {
	if (itemCapture) {
		Item_StopCapture(itemCapture);
		itemCapture = nullptr;
		captureFunc = 0;
		captureData = nullptr;
	} else {
		if ( down && ( key == A_MOUSE1 || key == A_MOUSE2 || key == A_MOUSE3 ) ) {
			Item_StartCapture(item, key);
		}
	}

	if (!down) {
		return false;
	}

	switch (item->type) {
	case ITEM_TYPE_BUTTON:
		return false;
		break;
	case ITEM_TYPE_RADIOBUTTON:
		return false;
		break;
	case ITEM_TYPE_CHECKBOX:
		return false;
		break;
	case ITEM_TYPE_EDITFIELD:
	case ITEM_TYPE_NUMERICFIELD:
		if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER)
		{
			editFieldDef_t *editPtr = item->typeData.edit;

			if (item->cvar && editPtr)
			{
				editPtr->paintOffset = 0;
			}

			//return Item_TextField_HandleKey(item, key);
		}
		return false;
		break;
	case ITEM_TYPE_COMBO:
		return false;
		break;
	case ITEM_TYPE_LISTBOX:
		return Item_ListBox_HandleKey(item, key, down, false);
		break;
	case ITEM_TYPE_TEXTSCROLL:
		return Item_TextScroll_HandleKey(item, key, down, false);
		break;
	case ITEM_TYPE_YESNO:
		return Item_YesNo_HandleKey(item, key);
		break;
	case ITEM_TYPE_MULTI:
		return Item_Multi_HandleKey(item, key);
		break;
	case ITEM_TYPE_OWNERDRAW:
		return Item_OwnerDraw_HandleKey(item, key);
		break;
	case ITEM_TYPE_BIND:
		return Item_Bind_HandleKey(item, key, down);
		break;
	case ITEM_TYPE_SLIDER:
		return Item_Slider_HandleKey(item, key, down);
		break;
		//case ITEM_TYPE_IMAGE:
		//  Item_Image_Paint(item);
		//  break;
	default:
		return false;
		break;
	}
}

//JLFACCEPT MPMOVED
// If Item has an accept script, run it.
bool Item_HandleAccept(itemDef_t * item)
{
	if (item->accept)
	{
		Item_RunScript(item, item->accept);
		return true;
	}
	return false;
}

void Item_Action(itemDef_t *item) {
  if (item) {
    Item_RunScript(item, item->action);
  }
}

static void Window_CloseCinematic(windowDef_t *window) {
	if (window->style == WINDOW_STYLE_CINEMATIC && window->cinematic >= 0) {
		DC->stopCinematic(window->cinematic);
		window->cinematic = -1;
	}
}

static void Menu_CloseCinematics(menuDef_t *menu) {
	if (menu) {
		int i;
		Window_CloseCinematic(&menu->window);
	  for (i = 0; i < menu->itemCount; i++) {
		  Window_CloseCinematic(&menu->items[i]->window);
			if (menu->items[i]->type == ITEM_TYPE_OWNERDRAW) {
				DC->stopCinematic(0-menu->items[i]->window.ownerDraw);
			}
	  }
	}
}

static void Display_CloseCinematics() {
	int i;
	for (i = 0; i < menuCount; i++) {
		Menu_CloseCinematics(&Menus[i]);
	}
}

void  Menus_Activate(menuDef_t *menu) {

	menu->window.flags |= (WINDOW_HASFOCUS | WINDOW_VISIBLE);
	if (menu->onOpen) {
		itemDef_t item;
		item.parent = menu;
		Item_RunScript(&item, menu->onOpen);
	}

	if (menu->soundName && *menu->soundName) {
//		DC->stopBackgroundTrack();					// you don't want to do this since it will reset s_rawend
		DC->startBackgroundTrack(menu->soundName, menu->soundName, false);
	}

	menu->appearanceTime = 0;
	Display_CloseCinematics();

}

int Display_VisibleMenuCount() {
	int i, count;
	count = 0;
	for (i = 0; i < menuCount; i++) {
		if (Menus[i].window.flags & (WINDOW_FORCED | WINDOW_VISIBLE)) {
			count++;
		}
	}
	return count;
}

void Menus_HandleOOBClick(menuDef_t *menu, int key, bool down) {
	if (menu) {
		int i;
		// basically the behaviour we are looking for is if there are windows in the stack.. see if
		// the cursor is within any of them.. if not close them otherwise activate them and pass the
		// key on.. force a mouse move to activate focus and script stuff
		if (down && menu->window.flags & WINDOW_OOB_CLICK) {
			Menu_RunCloseScript(menu);
			menu->window.flags &= ~(WINDOW_HASFOCUS | WINDOW_VISIBLE);
		}

		for (i = 0; i < menuCount; i++) {
			if (Menu_OverActiveItem(&Menus[i], DC->cursorx, DC->cursory)) {
				Menu_RunCloseScript(menu);
				menu->window.flags &= ~(WINDOW_HASFOCUS | WINDOW_VISIBLE);
			//	Menus_Activate(&Menus[i]);
				Menu_HandleMouseMove(&Menus[i], DC->cursorx, DC->cursory);
				Menu_HandleKey(&Menus[i], key, down);
			}
		}

		if (Display_VisibleMenuCount() == 0) {
			if (DC->Pause) {
				DC->Pause(false);
			}
		}
		Display_CloseCinematics();
	}
}

void Menu_HandleKey(menuDef_t *menu, int key, bool down) {
	if ( g_waitingForKey && down ) {
		Item_Bind_HandleKey( g_bindItem, key, down );
		return;
	}

	if ( g_editingField && down ) {
		if ( !Item_TextField_HandleKey( g_editItem, key ) ) {
			g_editingField = false;
			g_editItem = nullptr;
			return;
		}
		else if ( key == A_MOUSE1 || key == A_MOUSE2 || key == A_MOUSE3 ) {
			// switching fields so reset printed text of edit field
			Leaving_EditField( g_editItem );
			g_editingField = false;
			g_editItem = nullptr;
			Display_MouseMove( nullptr, DC->cursorx, DC->cursory );
		}
		else if ( key == A_TAB || key == A_CURSOR_UP || key == A_CURSOR_DOWN ) {
			return;
		}
	}

	if ( menu == nullptr ) {
		return;
	}

	// see if the mouse is within the window bounds and if so is this a mouse click
	if ( down && !(menu->window.flags & WINDOW_POPUP) && !Rect_ContainsPoint( &menu->window.rect, DC->cursorx, DC->cursory ) ) {
		static bool inHandleKey = false;
		if ( !inHandleKey && (key == A_MOUSE1 || key == A_MOUSE2 || key == A_MOUSE3) ) {
			inHandleKey = true;
			Menus_HandleOOBClick( menu, key, down );
			inHandleKey = false;
			return;
		}
	}

	// get the item with focus
	itemDef_t *item = nullptr;
	for ( int i=0; i<menu->itemCount; i++ ) {
		if ( menu->items[i]->window.flags & WINDOW_HASFOCUS ) {
			item = menu->items[i];
		}
	}

	// Ignore if disabled
	if ( item && item->disabled ) {
		return;
	}

	if ( item != nullptr ) {
		if ( Item_HandleKey( item, key, down ) ) {
			// It is possible for an item to be disable after Item_HandleKey is run (like in Voice Chat)
			if ( !item->disabled ) {
				Item_Action( item );
			}
			return;
		}
	}

	if ( !down ) {
		return;
	}

	// default handling
	switch ( key ) {

		case A_F11: {
			if ( developer.integer ) {
				debugMode ^= 1;
			}
		} break;

		case A_F12: {
			if ( developer.integer ) {
				switch ( DC->screenshotFormat ) {
					case SSF_JPEG:
						DC->executeText( EXEC_APPEND, "screenshot\n" );
						break;
					case SSF_TGA:
						DC->executeText( EXEC_APPEND, "screenshot_tga\n" );
						break;
					case SSF_PNG:
						DC->executeText( EXEC_APPEND, "screenshot_png\n" );
						break;
					default:
						if ( DC->Print ) {
							DC->Print( S_COLOR_YELLOW "Menu_HandleKey[F12]: Unknown screenshot format assigned! This should not happen.\n" );
						}
						break;
				}
			}
		} break;

		case A_KP_8:
		case A_CURSOR_UP: {
			Menu_SetPrevCursorItem( menu );
		} break;

		case A_ESCAPE: {
			if ( !g_waitingForKey && menu->onESC ) {
				itemDef_t it;
				it.parent = menu;
				Item_RunScript( &it, menu->onESC );
			}
		    g_waitingForKey = false;
		} break;

		case A_TAB:
		case A_KP_2:
		case A_CURSOR_DOWN: {
			Menu_SetNextCursorItem( menu );
		} break;

		case A_MOUSE1:
		case A_MOUSE2: {
			if ( item ) {
				// add new types here as needed
				/* Notes:
					Most controls will use the dpad to move through the selection possibilies. Buttons are the only exception. Buttons will be assumed to all be
					on one menu together. If the start or A button is pressed on a control focus, that means that the menu is accepted and move onto the next
					menu. If the start or A button is pressed on a button focus it should just process the action and not support the accept functionality.
				*/
				switch ( item->type ) {

				case ITEM_TYPE_EDITFIELD:
				case ITEM_TYPE_NUMERICFIELD: {
					if ( Rect_ContainsPoint( &item->window.rect, DC->cursorx, DC->cursory ) ) {
						Item_Action( item );
						item->cursorPos = 0;
						g_editingField = true;
						g_editItem = item;
						//DC->setOverstrikeMode( true );
					}
				} break;

				case ITEM_TYPE_MULTI:
				case ITEM_TYPE_YESNO:
				case ITEM_TYPE_SLIDER: {
					if ( Item_HandleAccept( item ) ) {
						return; // Item processed it overriding the menu processing
					}
					else if ( menu->onAccept ) {
						itemDef_t it;
						it.parent = menu;
						Item_RunScript( &it, menu->onAccept );
					}
				} break;

				default: {
					if ( Rect_ContainsPoint( &item->window.rect, DC->cursorx, DC->cursory ) ) {
						Item_Action( item );
					}
				} break;

				}
			}
		} break;

		case A_KP_ENTER:
		case A_ENTER: {
			if ( item ) {
				if ( item->type == ITEM_TYPE_EDITFIELD || item->type == ITEM_TYPE_NUMERICFIELD ) {
					item->cursorPos = 0;
					g_editingField = true;
					g_editItem = item;
					//DC->setOverstrikeMode( true );
				}
				else {
					Item_Action( item );
				}
			}
		} break;

		default: {
			// ...
		} break;
	}
}

void ToWindowCoords(float *x, float *y, windowDef_t *window) {
	if (window->border != 0) {
		*x += window->borderSize;
		*y += window->borderSize;
	}
	*x += window->rect.x;
	*y += window->rect.y;
}

void Rect_ToWindowCoords(rectDef_t *rect, windowDef_t *window) {
	ToWindowCoords(&rect->x, &rect->y, window);
}

void Item_SetTextExtents(itemDef_t *item, int *width, int *height, const char *text) {
	const char *textPtr = (text) ? text : item->text;

	if (textPtr == nullptr ) {
		return;
	}

	*width = item->textRect.w;
	*height = item->textRect.h;

	// keeps us from computing the widths and heights more than once
	if (*width == 0 || (item->type == ITEM_TYPE_OWNERDRAW && item->textalignment == ITEM_ALIGN_CENTER)
#ifndef BUILD_CGAME
		|| (item->text && item->text[0]=='@' && item->asset != se_language.modificationCount )	//string package language changed
#endif
		)
	{
		Text text{ item->font, item->textscale };
		int originalWidth = Text_Width( text, textPtr);

		if (item->type == ITEM_TYPE_OWNERDRAW && (item->textalignment == ITEM_ALIGN_CENTER || item->textalignment == ITEM_ALIGN_RIGHT))
		{
			originalWidth += DC->ownerDrawWidth(item->window.ownerDraw, item->textscale);
		}
		else if (item->type == ITEM_TYPE_EDITFIELD && item->textalignment == ITEM_ALIGN_CENTER && item->cvar)
		{
			char buff[256];
			DC->getCVarString(item->cvar, buff, 256);
			originalWidth += Text_Width( text, buff);
		}

		*width = Text_Width( text, textPtr);
		*height = Text_Height( text, textPtr);

		item->textRect.w = *width;
		item->textRect.h = *height;
		item->textRect.x = item->textalignx;
		item->textRect.y = item->textaligny;
		if (item->textalignment == ITEM_ALIGN_RIGHT) {
			item->textRect.x = item->textalignx - originalWidth;
		} else if (item->textalignment == ITEM_ALIGN_CENTER) {
			item->textRect.x = item->textalignx - originalWidth / 2;
		}

		ToWindowCoords(&item->textRect.x, &item->textRect.y, &item->window);
#ifndef BUILD_CGAME
		if (item->text && item->text[0]=='@' )//string package
		{//mark language
			item->asset = se_language.modificationCount;
		}
#endif
	}
}

void Item_TextColor(itemDef_t *item, vec4_t *newColor) {
	vec4_t lowLight;
	menuDef_t *parent = (menuDef_t*)item->parent;

	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, true, parent->fadeAmount);

	if (item->window.flags & WINDOW_HASFOCUS) {
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor,lowLight,*newColor,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
	} else if (item->textStyle == ITEM_TEXTSTYLE_BLINK && !((DC->realTime/BLINK_DIVISOR) & 1)) {
		lowLight[0] = 0.8 * item->window.foreColor[0];
		lowLight[1] = 0.8 * item->window.foreColor[1];
		lowLight[2] = 0.8 * item->window.foreColor[2];
		lowLight[3] = 0.8 * item->window.foreColor[3];
		LerpColor(item->window.foreColor,lowLight,*newColor,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
	} else {
		memcpy(newColor, &item->window.foreColor, sizeof(vec4_t));
		// items can be enabled and disabled based on cvars
	}

	if (item->disabled)
	{
		memcpy(newColor, &parent->disableColor, sizeof(vec4_t));
	}

	if (item->enableCvar && *item->enableCvar && item->cvarTest && *item->cvarTest) {
		if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
			memcpy(newColor, &parent->disableColor, sizeof(vec4_t));
		}
	}
}

void Item_Text_AutoWrapped_Paint(itemDef_t *item) {
	const char *p, *textPtr, *newLinePtr;
	char buff[2048];
	int height, len, textWidth, newLine, newLineWidth;	//int width;
	float y;
	vec4_t color;

	textWidth = 0;
	newLinePtr = nullptr;

	char str[2048];
	if (item->text == nullptr) {
		if (item->cvar == nullptr) {
			return;
		}
		else {
			DC->getCVarString(item->cvar, str, sizeof(str));
			textPtr = str;
		}
	}
	else {
		textPtr = item->text;
	}
	if (*textPtr == '@')	// string reference
	{
		trap->SE_GetStringTextString( &textPtr[1], str, sizeof(str));
		textPtr = str;
	}
	if (*textPtr == '\0') {
		return;
	}
	Item_TextColor(item, &color);
	Text text{ item->font, item->textscale };
	height = Text_Height( text, textPtr);

	y = item->textaligny;
	len = 0;
	buff[0] = '\0';
	newLine = 0;
	newLineWidth = 0;
	p = textPtr;
	while (p) {	//findmeste (this will break widechar languages)!
		if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\0') {
			newLine = len;
			newLinePtr = p+1;
			newLineWidth = textWidth;
		}
		textWidth = Text_Width( text, buff);
		if ( (newLine && textWidth > item->window.rect.w) || *p == '\n' || *p == '\0') {
			if (len) {
				if (item->textalignment == ITEM_ALIGN_LEFT) {
					item->textRect.x = item->textalignx;
				} else if (item->textalignment == ITEM_ALIGN_RIGHT) {
					item->textRect.x = item->textalignx - newLineWidth;
				} else if (item->textalignment == ITEM_ALIGN_CENTER) {
					item->textRect.x = item->textalignx - newLineWidth / 2;
				}
				item->textRect.y = y;
				ToWindowCoords(&item->textRect.x, &item->textRect.y, &item->window);

				buff[newLine] = '\0';
				Text_Paint( text, item->textRect.x, item->textRect.y, buff, color, item->textStyle);
			}
			if (*p == '\0') {
				break;
			}

			y += height + 5;
			p = newLinePtr;
			len = 0;
			newLine = 0;
			newLineWidth = 0;
			continue;
		}
		buff[len++] = *p++;
		buff[len] = '\0';
	}
}

void Item_Text_Wrapped_Paint(itemDef_t *item) {
	char str[1024];
	const char *p, *start, *textPtr;
	char buff[1024];
	int width, height;
	float x, y;
	vec4_t color;

	// now paint the text and/or any optional images
	// default to left

	if (item->text == nullptr) {
		if (item->cvar == nullptr) {
			return;
		}
		else {
			DC->getCVarString(item->cvar, str, sizeof(str));
			textPtr = str;
		}
	}
	else {
		textPtr = item->text;
	}
	if (*textPtr == '@')	// string reference
	{
		trap->SE_GetStringTextString( &textPtr[1], str, sizeof(str));
		textPtr = str;
	}
	if (*textPtr == '\0') {
		return;
	}

	Item_TextColor(item, &color);
	Item_SetTextExtents(item, &width, &height, textPtr);

	x = item->textRect.x;
	y = item->textRect.y;
	start = textPtr;
	p = strchr(textPtr, '\r');
	Text text{ item->font, item->textscale };
	while (p && *p) {
		strncpy(buff, start, p-start+1);
		buff[p-start] = '\0';
		Text_Paint( text, x, y, buff, color, item->textStyle);
		y += height + 2;
		start += p - start + 1;
		p = strchr(p+1, '\r');
	}
	Text_Paint( text, x, y, start, color, item->textStyle);
}

void Item_Text_Paint(itemDef_t *item) {
	char str[1024];
	const char *textPtr;
	int height, width;
	vec4_t color;

	if (item->window.flags & WINDOW_WRAPPED) {
		Item_Text_Wrapped_Paint(item);
		return;
	}
	if (item->window.flags & WINDOW_AUTOWRAPPED) {
		Item_Text_AutoWrapped_Paint(item);
		return;
	}

	if (item->text == nullptr) {
		if (item->cvar == nullptr) {
			return;
		}
		else {
			DC->getCVarString(item->cvar, str, sizeof(str));
			textPtr = str;
		}
	}
	else {
		textPtr = item->text;
	}
	if (*textPtr == '@')	// string reference
	{
		trap->SE_GetStringTextString( &textPtr[1], str, sizeof(str));
		textPtr = str;
	}

	// this needs to go here as it sets extents for cvar types as well
	Item_SetTextExtents(item, &width, &height, textPtr);

	if (*textPtr == '\0') {
		return;
	}

	Item_TextColor(item, &color);

	Text text{ item->font, item->textscale };
	Text_Paint( text, item->textRect.x, item->textRect.y, textPtr, color, item->textStyle);

	if (item->text2)	// Is there a second line of text?
	{
		textPtr = item->text2;
		if (*textPtr == '@')	// string reference
		{
			trap->SE_GetStringTextString( &textPtr[1], str, sizeof(str));
			textPtr = str;
		}
		Item_TextColor(item, &color);
		Text_Paint( text, item->textRect.x + item->text2alignx, item->textRect.y + item->text2aligny, textPtr, color, item->textStyle);
	}
}

void Item_TextField_Paint(itemDef_t *item) {
	char buff[1024];
	vec4_t newColor, lowLight;
	int offset;
	menuDef_t *parent = (menuDef_t*)item->parent;
	editFieldDef_t *editPtr = item->typeData.edit;

	Item_Text_Paint(item);

	buff[0] = '\0';

	if (item->cvar) {
		DC->getCVarString(item->cvar, buff, sizeof(buff));
		if (buff[0] == '@')	// string reference
		{
			trap->SE_GetStringTextString( &buff[1], buff, sizeof(buff));
		}
	}

	parent = (menuDef_t*)item->parent;

	if (item->window.flags & WINDOW_HASFOCUS) {
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor,lowLight,newColor,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
	} else {
		memcpy(&newColor, &item->window.foreColor, sizeof(vec4_t));
	}

	Text text{ item->font, item->textscale };
	offset = (item->text && *item->text) ? 8 : 0;
	if (item->window.flags & WINDOW_HASFOCUS && g_editingField) {
		Text_Paint( text, item->textRect.x + item->textRect.w + offset, item->textRect.y, buff + editPtr->paintOffset, newColor, item->textStyle, item->window.rect.w, 0.0f,
			item->cursorPos - editPtr->paintOffset, DC->getOverstrikeMode()
		);
	} else {
		Text_Paint( text, item->textRect.x + item->textRect.w + offset, item->textRect.y, buff + editPtr->paintOffset, newColor, item->textStyle, item->window.rect.w
		);
	}
}

void Item_YesNo_Paint(itemDef_t *item) {
	char	sYES[20];
	char	sNO[20];
	vec4_t color;
	float value;
	const char *yesnovalue;

	value = (item->cvar) ? DC->getCVarValue(item->cvar) : 0;

	trap->SE_GetStringTextString("MENUS_YES",sYES, sizeof(sYES));
	trap->SE_GetStringTextString("MENUS_NO", sNO,  sizeof(sNO));

//JLFYESNO MPMOVED
	if (item->invertYesNo)
		yesnovalue = (value == 0) ? sYES : sNO;
	else
		yesnovalue = (value != 0) ? sYES : sNO;

	Item_TextColor(item, &color);
	Text text{ item->font, item->textscale };
	if (item->text)
	{
		Item_Text_Paint(item);
		Text_Paint( text, item->textRect.x + item->textRect.w + 8, item->textRect.y, yesnovalue, color, item->textStyle);
	}
	else
	{
		Text_Paint( text, item->textRect.x, item->textRect.y, yesnovalue, color, item->textStyle);
	}
}

void Item_Multi_Paint(itemDef_t *item) {
	vec4_t color;
	char	temp[MAX_STRING_CHARS];

	const char *str = Item_Multi_Setting(item);
	if (*str == '@')	// string reference
	{
		trap->SE_GetStringTextString( &str[1]  , temp, sizeof(temp));
		str = temp;
	}
	// Is is specifying a cvar to get the item name from?
	else if (*str == '*')
	{
		DC->getCVarString(&str[1], temp, sizeof(temp));
		str = temp;
	}

	Item_TextColor(item, &color);
	Text text{ item->font, item->textscale };
	if (item->text) {
		Item_Text_Paint(item);
		Text_Paint( text, item->textRect.x + item->textRect.w + 8, item->textRect.y, str, color, item->textStyle);
	} else {
		//JLF added xoffset
		Text_Paint( text, item->textRect.x+item->xoffset, item->textRect.y, str, color, item->textStyle);
	}
}

static void Controls_GetKeyAssignment( const char *command, int *twokeys )
{
	int		count;
	int		j;
	char	b[256];

	twokeys[0] = twokeys[1] = -1;
	count = 0;

	for ( j=0; j<MAX_KEYS; j++ ) {
		DC->getBindingBuf( j, b, sizeof( b ) );
		if ( *b && !Q_stricmp( b, command ) ) {
			twokeys[count] = j;
			count++;
			if ( count == 2 )
				break;
		}
	}
}

void Controls_GetConfig( void )
{
	size_t	i;

	// iterate each command, get its numeric binding
	for ( i=0; i < g_bindCount; i++ )
		Controls_GetKeyAssignment( g_bindCommands[i], g_bindKeys[i] );
}

void Controls_SetConfig( void )
{
	size_t	i;

	// iterate each command, get its numeric binding
	for ( i=0; i<g_bindCount; i++ ) {
		if ( g_bindKeys[i][0] != -1 ) {
			DC->setBinding( g_bindKeys[i][0], g_bindCommands[i] );

			if ( g_bindKeys[i][1] != -1 )
				DC->setBinding( g_bindKeys[i][1], g_bindCommands[i] );
		}
	}
}

void Controls_SetDefaults( void )
{
	size_t	i;

	for ( i=0; i<g_bindCount; i++ ) {
		g_bindKeys[i][0] = -1;
		g_bindKeys[i][1] = -1;
	}
}

char g_nameBind[96];

void BindingFromName( const char *cvar ) {
	size_t	i;
	int		b1, b2;
	char	sOR[32];

	// iterate each command, set its default binding
	for ( i=0; i < g_bindCount; i++ ) {
		if ( !Q_stricmp(cvar, g_bindCommands[i] ) ) {
			b2 = g_bindKeys[i][1];
			b1 = g_bindKeys[i][0];
			if ( b1 == -1 )
				break;

			if ( b2 != -1 ) {
				char keyname[2][32];

				DC->keynumToStringBuf( b1, keyname[0], sizeof( keyname[0] ) );
// do NOT do this or it corrupts asian text!!!					Q_strupr(keyname[0]);
				DC->keynumToStringBuf( b2, keyname[1], sizeof( keyname[1] ) );
// do NOT do this or it corrupts asian text!!!					Q_strupr(keyname[1]);

				trap->SE_GetStringTextString( "MENUS_KEYBIND_OR", sOR, sizeof( sOR ) );

				Com_sprintf( g_nameBind, sizeof( g_nameBind ), "%s %s %s", keyname[0], sOR, keyname[1] );
			}
			else {
				DC->keynumToStringBuf( b1, g_nameBind, sizeof( g_nameBind ) );
// do NOT do this or it corrupts asian text!!!					Q_strupr(g_nameBind);
			}
			return;
		}
	}
	Q_strncpyz( g_nameBind, "???", sizeof( g_nameBind ) );
}

void Item_Slider_Paint(itemDef_t *item) {
	vec4_t newColor, lowLight;
	float x, y;
	menuDef_t *parent = (menuDef_t*)item->parent;

	if (item->window.flags & WINDOW_HASFOCUS) {
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor,lowLight,newColor,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
	} else {
		memcpy(&newColor, &item->window.foreColor, sizeof(vec4_t));
	}

	y = item->window.rect.y;
	if (item->text) {
		Item_Text_Paint(item);
		x = item->textRect.x + item->textRect.w + 8;
	} else {
		x = item->window.rect.x;
	}
	DC->setColor(newColor);
	DC->drawHandlePic( x, y, SLIDER_WIDTH, SLIDER_HEIGHT, DC->Assets.sliderBar );

	x = Item_Slider_ThumbPosition(item);
	DC->drawHandlePic( x - (SLIDER_THUMB_WIDTH / 2), y - 2, SLIDER_THUMB_WIDTH, SLIDER_THUMB_HEIGHT, DC->Assets.sliderThumb );
}

void Item_Bind_Paint(itemDef_t *item)
{
	vec4_t newColor, lowLight;
	float value;
	int maxChars = 0;
	float	textScale,textWidth;
	int		textHeight,yAdj,startingXPos;

	menuDef_t *parent = (menuDef_t*)item->parent;
	editFieldDef_t *editPtr = item->typeData.edit;
	if (editPtr)
	{
		maxChars = editPtr->maxPaintChars;
	}

	value = (item->cvar) ? DC->getCVarValue(item->cvar) : 0;

	if (item->window.flags & WINDOW_HASFOCUS)
	{
		if (g_bindItem == item)
		{
			lowLight[0] = 0.8f * 1.0f;
			lowLight[1] = 0.8f * 0.0f;
			lowLight[2] = 0.8f * 0.0f;
			lowLight[3] = 0.8f * 1.0f;
		}
		else
		{
			lowLight[0] = 0.8f * parent->focusColor[0];
			lowLight[1] = 0.8f * parent->focusColor[1];
			lowLight[2] = 0.8f * parent->focusColor[2];
			lowLight[3] = 0.8f * parent->focusColor[3];
		}
		LerpColor(parent->focusColor,lowLight,newColor,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
	}
	else
	{
		memcpy(&newColor, &item->window.foreColor, sizeof(vec4_t));
	}

	Text text{ item->font, item->textscale };
	if (item->text)
	{
		Item_Text_Paint(item);
		BindingFromName(item->cvar);

		// If the text runs past the limit bring the scale down until it fits.
		textScale = item->textscale;
		textWidth = Text_Width( text, g_nameBind);
		startingXPos = (item->textRect.x + item->textRect.w + 8);

		while ((startingXPos + textWidth) >= SCREEN_WIDTH)
		{
			textScale -= .05f;
			textWidth = Text_Width( text, g_nameBind);
		}

		// Try to adjust it's y placement if the scale has changed.
		yAdj = 0;
		if (textScale != item->textscale)
		{
			textHeight = Text_Height( text, g_nameBind);
			yAdj = textHeight - Text_Height( text, g_nameBind);
		}

		Text_Paint( text, startingXPos, item->textRect.y + yAdj, g_nameBind, newColor, maxChars, item->textStyle);
	}
	else
	{
		Text_Paint( text, item->textRect.x, item->textRect.y, (value != 0) ? "FIXME" : "FIXME", newColor, maxChars, item->textStyle);
	}
}

bool Display_KeyBindPending( void ) {
	return g_waitingForKey;
}

// scale the model should we need to
void UI_ScaleModelAxis( refEntity_t *ent )
{
	if ( ent->modelScale[0] && ent->modelScale[0] != 1.0f ) {
		VectorScale( ent->axis[0], ent->modelScale[0] , ent->axis[0] );
		ent->nonNormalizedAxes = true;
	}
	if ( ent->modelScale[1] && ent->modelScale[1] != 1.0f ) {
		VectorScale( ent->axis[1], ent->modelScale[1] , ent->axis[1] );
		ent->nonNormalizedAxes = true;
	}
	if ( ent->modelScale[2] && ent->modelScale[2] != 1.0f ) {
		VectorScale( ent->axis[2], ent->modelScale[2] , ent->axis[2] );
		ent->nonNormalizedAxes = true;
	}
}

void Item_Model_Paint(itemDef_t *item)
{
	float x, y, w, h;
	refdef_t refdef;
	refEntity_t		ent;
	vec3_t			mins, maxs, origin;
	vec3_t			angles;
	modelDef_t *modelPtr = item->typeData.model;

	if (modelPtr == nullptr)
	{
		return;
	}

	// a moves datapad anim is playing
#ifdef BUILD_UI
	if (uiInfo.moveAnimTime && (uiInfo.moveAnimTime < uiInfo.uiDC.realTime))
	{
		if (modelPtr)
		{
			char modelPath[MAX_QPATH];

			Com_sprintf( modelPath, sizeof( modelPath ), "models/players/%s/model.glm", UI_Cvar_VariableString ( "ui_char_model" ) );
			//HACKHACKHACK: check for any multi-part anim sequences, and play the next anim, if needbe
			switch( modelPtr->g2anim )
			{
			case BOTH_FORCEWALLREBOUND_FORWARD:
			case BOTH_FORCEJUMP1:
				ItemParse_model_g2anim_go( item, animTable[BOTH_FORCEINAIR1].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				if ( !uiInfo.moveAnimTime )
				{
					uiInfo.moveAnimTime = 500;
				}
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			case BOTH_FORCEINAIR1:
				ItemParse_model_g2anim_go( item, animTable[BOTH_FORCELAND1].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			case BOTH_FORCEWALLRUNFLIP_START:
				ItemParse_model_g2anim_go( item, animTable[BOTH_FORCEWALLRUNFLIP_END].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			case BOTH_FORCELONGLEAP_START:
				ItemParse_model_g2anim_go( item, animTable[BOTH_FORCELONGLEAP_LAND].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			case BOTH_KNOCKDOWN3://on front - into force getup
				trap->S_StartLocalSound( uiInfo.uiDC.Assets.moveJumpSound, CHAN_LOCAL );
				ItemParse_model_g2anim_go( item, animTable[BOTH_FORCE_GETUP_F1].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			case BOTH_KNOCKDOWN2://on back - kick forward getup
				trap->S_StartLocalSound( uiInfo.uiDC.Assets.moveJumpSound, CHAN_LOCAL );
				ItemParse_model_g2anim_go( item, animTable[BOTH_GETUP_BROLL_F].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			case BOTH_KNOCKDOWN1://on back - roll-away
				trap->S_StartLocalSound( uiInfo.uiDC.Assets.moveRollSound, CHAN_LOCAL );
				ItemParse_model_g2anim_go( item, animTable[BOTH_GETUP_BROLL_R].name );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime += uiInfo.uiDC.realTime;
				break;
			default:
				ItemParse_model_g2anim_go( item,  uiInfo.movesBaseAnim );
				ItemParse_asset_model_go( item, modelPath, &uiInfo.moveAnimTime );
				uiInfo.moveAnimTime = 0;
				break;
			}

			UI_UpdateCharacterSkin();

			//update saber models
			UI_SaberAttachToChar( item );
		}
	}
#endif

	// setup the refdef
	memset( &refdef, 0, sizeof( refdef ) );

	refdef.rdflags = RDF_NOWORLDMODEL;
	AxisClear( refdef.viewaxis );
	x = item->window.rect.x+1;
	y = item->window.rect.y+1;
	w = item->window.rect.w-2;
	h = item->window.rect.h-2;

	refdef.x = x * DC->xscale;
	refdef.y = y * DC->yscale;
	refdef.width = w * DC->xscale;
	refdef.height = h * DC->yscale;

	if (item->ghoul2)
	{ //ghoul2 models don't have bounds, so we have to parse them.
		VectorCopy(modelPtr->g2mins, mins);
		VectorCopy(modelPtr->g2maxs, maxs);

		if (!mins[0] && !mins[1] && !mins[2] &&
			!maxs[0] && !maxs[1] && !maxs[2])
		{ //we'll use defaults then I suppose.
			VectorSet(mins, -16, -16, -24);
			VectorSet(maxs, 16, 16, 32);
		}
	}
	else
	{
		DC->modelBounds( item->asset, mins, maxs );
	}

	origin[2] = -0.5 * ( mins[2] + maxs[2] );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );

	// calculate distance so the model nearly fills the box
	if (true)
	{
		float len = 0.5 * ( maxs[2] - mins[2] );
		origin[0] = len / 0.268;	// len / tan( fov/2 )
		//origin[0] = len / tan(w/2);
	}
	else
	{
		origin[0] = item->textscale;
	}
	refdef.fov_x = (modelPtr->fov_x) ? modelPtr->fov_x : (int)((float)refdef.width / 640.0f * 90.0f);
	refdef.fov_y = (modelPtr->fov_y) ? modelPtr->fov_y : atan2( refdef.height, refdef.width / tan( refdef.fov_x / 360 * M_PI ) ) * ( 360 / M_PI );

	//refdef.fov_x = (int)((float)refdef.width / 640.0f * 90.0f);
	//refdef.fov_y = atan2( refdef.height, refdef.width / tan( refdef.fov_x / 360 * M_PI ) ) * ( 360 / M_PI );

	DC->clearScene();

	refdef.time = DC->realTime;

	// add the model

	memset( &ent, 0, sizeof(ent) );

	// use item storage to track
	if ( (item->flags&ITF_ISANYSABER) && !(item->flags&ITF_ISCHARACTER) )
	{//hack to put saber on it's side
		if (modelPtr->rotationSpeed)
		{
			VectorSet( angles, modelPtr->angle+(float)refdef.time/modelPtr->rotationSpeed, 0, 90 );
		}
		else
		{
			VectorSet( angles, modelPtr->angle, 0, 90 );
		}
	}
	else if (modelPtr->rotationSpeed)
	{
		VectorSet( angles, 0, modelPtr->angle + (float)refdef.time/modelPtr->rotationSpeed, 0 );
	}
	else
	{
		VectorSet( angles, 0, modelPtr->angle, 0 );
	}

	AnglesToAxis( angles, ent.axis );

	if (item->ghoul2)
	{
		ent.ghoul2 = item->ghoul2;
		ent.radius = 1000;
		ent.customSkin = modelPtr->g2skin;

		VectorCopy(modelPtr->g2scale, ent.modelScale);
		UI_ScaleModelAxis(&ent);
#ifndef BUILD_CGAME
		if ( (item->flags&ITF_ISCHARACTER) )
		{
			ent.shaderRGBA[0] = ui_char_color_red.integer;
			ent.shaderRGBA[1] = ui_char_color_green.integer;
			ent.shaderRGBA[2] = ui_char_color_blue.integer;
			ent.shaderRGBA[3] = 255;
//			UI_TalkingHead(item);
		}
		if ( item->flags&ITF_ISANYSABER )
		{//UGH, draw the saber blade!
			UI_SaberDrawBlades( item, origin, angles );
		}
#endif
	}
	else
	{
		ent.hModel = item->asset;
	}
	VectorCopy( origin, ent.origin );
	VectorCopy( ent.origin, ent.oldorigin );

	// Set up lighting
	VectorCopy( origin, ent.lightingOrigin );
	ent.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;

	DC->addRefEntityToScene( &ent );
	DC->renderScene( &refdef );

}

void Item_Image_Paint(itemDef_t *item) {
	if (item == nullptr) {
		return;
	}
	DC->drawHandlePic(item->window.rect.x+1, item->window.rect.y+1, item->window.rect.w-2, item->window.rect.h-2, item->asset);
}

void Item_TextScroll_Paint(itemDef_t *item)
{
	char cvartext[1024];
	float x, y, size, count, thumb;
	int	  i;
	textScrollDef_t *scrollPtr = item->typeData.textscroll;

	count = scrollPtr->iLineCount;

	// draw scrollbar to right side of the window
	x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE - 1;
	y = item->window.rect.y + 1;
	DC->drawHandlePic(x, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarArrowUp);
	y += SCROLLBAR_SIZE - 1;

	scrollPtr->endPos = scrollPtr->startPos;
	size = item->window.rect.h - (SCROLLBAR_SIZE * 2);
	DC->drawHandlePic(x, y, SCROLLBAR_SIZE, size+1, DC->Assets.scrollBar);
	y += size - 1;
	DC->drawHandlePic(x, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarArrowDown);

	// thumb
	thumb = Item_TextScroll_ThumbDrawPosition(item);
	if (thumb > y - SCROLLBAR_SIZE - 1)
	{
		thumb = y - SCROLLBAR_SIZE - 1;
	}
	DC->drawHandlePic(x, thumb, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarThumb);

	if (item->cvar)
	{
		DC->getCVarString(item->cvar, cvartext, sizeof(cvartext));
		item->text = cvartext;
		Item_TextScroll_BuildLines ( item );
	}

	// adjust size for item painting
	size = item->window.rect.h - 2;
	x	 = item->window.rect.x + item->textalignx + 1;
	y	 = item->window.rect.y + item->textaligny + 1;

	Text text{ item->font, item->textscale };

	for (i = scrollPtr->startPos; i < count; i++)
	{
		const char *str = scrollPtr->pLines[i];
		if (!str)
		{
			continue;
		}

		Text_Paint( text, x + 4, y, str, item->window.foreColor, item->textStyle);

		size -= scrollPtr->lineHeight;
		if (size < scrollPtr->lineHeight)
		{
			scrollPtr->drawPadding = scrollPtr->lineHeight - size;
			break;
		}

		scrollPtr->endPos++;
		y += scrollPtr->lineHeight;
	}
}

#define COLOR_MAX 255.0f

// Draw routine for list boxes
void Item_ListBox_Paint(itemDef_t *item) {
	float x, y, sizeWidth, count, i, i2,sizeHeight, thumb;
	int startPos;
	qhandle_t image;
	qhandle_t optionalImage1, optionalImage2, optionalImage3;
	listBoxDef_t *listPtr = item->typeData.listbox;
//JLF MPMOVED

	// the listbox is horizontal or vertical and has a fixed size scroll bar going either direction
	// elements are enumerated from the DC and either text or image handles are acquired from the DC as well
	// textscale is used to size the text, textalignx and textaligny are used to size image elements
	// there is no clipping available so only the last completely visible item is painted
	count = DC->feederCount(item->special);

	if (listPtr->startPos > (count?count-1:count))
	{//probably changed feeders, so reset
		listPtr->startPos = 0;
	}
//JLF END
	if (item->cursorPos > (count?count-1:count))
	{//probably changed feeders, so reset
		item->cursorPos = (count?count-1:count);
		// NOTE : might consider moving this to any spot in here we change the cursor position
		DC->feederSelection( item->special, item->cursorPos, nullptr );
	}

	// default is vertical if horizontal flag is not here
	if (item->window.flags & WINDOW_HORIZONTAL)
	{
//JLF new variable (code just indented) MPMOVED
		if (!listPtr->scrollhidden)
		{
		// draw scrollbar in bottom of the window
		// bar
			if (Item_ListBox_MaxScroll(item) > 0)
			{
				x = item->window.rect.x + 1;
				y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE - 1;
				DC->drawHandlePic(x, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarArrowLeft);
				x += SCROLLBAR_SIZE - 1;
				sizeWidth = item->window.rect.w - (SCROLLBAR_SIZE * 2);
				DC->drawHandlePic(x, y, sizeWidth+1, SCROLLBAR_SIZE, DC->Assets.scrollBar);
				x += sizeWidth - 1;
				DC->drawHandlePic(x, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarArrowRight);
				// thumb
				thumb = Item_ListBox_ThumbDrawPosition(item);//Item_ListBox_ThumbPosition(item);
				if (thumb > x - SCROLLBAR_SIZE - 1) {
					thumb = x - SCROLLBAR_SIZE - 1;
				}
				DC->drawHandlePic(thumb, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarThumb);
			}
			else if (listPtr->startPos > 0)
			{
				listPtr->startPos = 0;
			}
		}
//JLF end

		listPtr->endPos = listPtr->startPos;
		sizeWidth = item->window.rect.w - 2;
		// items
		// size contains max available space
		if (listPtr->elementStyle == LISTBOX_IMAGE)
		{
			// fit = 0;
			x = item->window.rect.x + 1;
			y = item->window.rect.y + 1;
			for (i = listPtr->startPos; i < count; i++)
			{
				// always draw at least one
				// which may overdraw the box if it is too small for the element
				image = DC->feederItemImage(item->special, i);
				if (image)
				{
#ifndef BUILD_CGAME
					if (item->window.flags & WINDOW_PLAYERCOLOR)
					{
						vec4_t	color;

						color[0] = ui_char_color_red.integer/COLOR_MAX;
						color[1] = ui_char_color_green.integer/COLOR_MAX;
						color[2] = ui_char_color_blue.integer/COLOR_MAX;
						color[3] = 1.0f;
						DC->setColor(color);
					}
#endif
					DC->drawHandlePic(x+1, y+1, listPtr->elementWidth - 2, listPtr->elementHeight - 2, image);
				}

				if (i == item->cursorPos)
				{
					DC->drawRect(x, y, listPtr->elementWidth-1, listPtr->elementHeight-1, item->window.borderSize, item->window.borderColor);
				}

				sizeWidth -= listPtr->elementWidth;
				if (sizeWidth < listPtr->elementWidth)
				{
					listPtr->drawPadding = sizeWidth; //listPtr->elementWidth - size;
					break;
				}
				x += listPtr->elementWidth;
				listPtr->endPos++;
				// fit++;
			}
		}
		else
		{

		}

#ifdef	_DEBUG
		// Show pic name
		const char *str = DC->feederItemText( item->special, item->cursorPos, 0, &optionalImage1, &optionalImage2, &optionalImage3 );
		if ( str ) {
			Text text{ item->font, item->textscale };
			Text_Paint( text, item->window.rect.x, item->window.rect.y+item->window.rect.h, str, item->window.foreColor, item->textStyle);
		}
#endif

	}
	// A vertical list box
	else
	{
		//JLF new variable (code idented with if)
		if (!listPtr->scrollhidden)
		{

			// draw scrollbar to right side of the window
			x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE - 1;

			if ( (int)item->special == FEEDER_Q3HEADS )
				x -= 2;

			y = item->window.rect.y + 1;
			DC->drawHandlePic(x, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarArrowUp);
			y += SCROLLBAR_SIZE - 1;

			listPtr->endPos = listPtr->startPos;
			sizeHeight = item->window.rect.h - (SCROLLBAR_SIZE * 2);
			DC->drawHandlePic(x, y, SCROLLBAR_SIZE, sizeHeight+1, DC->Assets.scrollBar);
			y += sizeHeight - 1;
			DC->drawHandlePic(x, y, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarArrowDown);
			// thumb
			thumb = Item_ListBox_ThumbDrawPosition(item);//Item_ListBox_ThumbPosition(item);
			if (thumb > y - SCROLLBAR_SIZE - 1) {
				thumb = y - SCROLLBAR_SIZE - 1;
			}
			DC->drawHandlePic(x, thumb, SCROLLBAR_SIZE, SCROLLBAR_SIZE, DC->Assets.scrollBarThumb);
		}
//JLF end

		// adjust size for item painting
		sizeWidth = item->window.rect.w - 2;
		sizeHeight = item->window.rect.h - 2;

		if (listPtr->elementStyle == LISTBOX_IMAGE)
		{
			// Multiple rows and columns (since it's more than twice as wide as an element)
			if ( item->window.rect.w > (listPtr->elementWidth*2) )
			{
				startPos = listPtr->startPos;
				x = item->window.rect.x + 1;
				y = item->window.rect.y + 1;
				// Next row
				for (i2 = startPos; i2 < count; i2++)
				{
					x = item->window.rect.x + 1;
					sizeWidth = item->window.rect.w - 2;
					// print a row
					for (i = startPos; i < count; i++)
					{
						// always draw at least one
						// which may overdraw the box if it is too small for the element
						image = DC->feederItemImage(item->special, i);
						if (image)
						{
		#ifndef BUILD_CGAME
							if (item->window.flags & WINDOW_PLAYERCOLOR)
							{
								vec4_t	color;

								color[0] = ui_char_color_red.integer/COLOR_MAX;
								color[1] = ui_char_color_green.integer/COLOR_MAX;
								color[2] = ui_char_color_blue.integer/COLOR_MAX;
								color[3] = 1.0f;
								DC->setColor(color);
							}
		#endif
							DC->drawHandlePic(x+1, y+1, listPtr->elementWidth - 2, listPtr->elementHeight - 2, image);
						}

						if (i == item->cursorPos)
						{
							DC->drawRect(x, y, listPtr->elementWidth-1, listPtr->elementHeight-1, item->window.borderSize, item->window.borderColor);
						}

						sizeWidth -= listPtr->elementWidth;
						if (sizeWidth < listPtr->elementWidth)
						{
							listPtr->drawPadding = sizeWidth; //listPtr->elementWidth - size;
							break;
						}
						x += listPtr->elementWidth;
						listPtr->endPos++;
					}

					sizeHeight -= listPtr->elementHeight;
					if (sizeHeight < listPtr->elementHeight)
					{
						listPtr->drawPadding = sizeHeight; //listPtr->elementWidth - size;
						break;
					}
					// NOTE : Is endPos supposed to be valid or not? It was being used as a valid entry but I changed those
					// few spots that were causing bugs
					listPtr->endPos++;
					startPos = listPtr->endPos;
					y += listPtr->elementHeight;

				}
			}
			// single column
			else
			{
				x = item->window.rect.x + 1;
				y = item->window.rect.y + 1;
				for (i = listPtr->startPos; i < count; i++)
				{
					// always draw at least one
					// which may overdraw the box if it is too small for the element
					image = DC->feederItemImage(item->special, i);
					if (image)
					{
						DC->drawHandlePic(x+1, y+1, listPtr->elementWidth - 2, listPtr->elementHeight - 2, image);
					}

					if (i == item->cursorPos)
					{
						DC->drawRect(x, y, listPtr->elementWidth - 1, listPtr->elementHeight - 1, item->window.borderSize, item->window.borderColor);
					}

					listPtr->endPos++;
					sizeHeight -= listPtr->elementHeight;
					if (sizeHeight < listPtr->elementHeight)
					{
						listPtr->drawPadding = listPtr->elementHeight - sizeHeight;
						break;
					}
					y += listPtr->elementHeight;
					// fit++;
				}
			}
		}
		else
		{
			x = item->window.rect.x + 1;
			y = item->window.rect.y + 1;
//JLF MPMOVED
			y = item->window.rect.y + 1 - listPtr->elementHeight;
			i = listPtr->startPos;
			Text text{ item->font, item->textscale };

			for (; i < count; i++)
//JLF END
			{
				const char *str;
				// always draw at least one
				// which may overdraw the box if it is too small for the element
				if (listPtr->numColumns > 0) {
					int j;//, subX = listPtr->elementHeight;

					for (j = 0; j < listPtr->numColumns; j++)
					{
						char	temp[MAX_STRING_CHARS];
						int imageStartX = listPtr->columnInfo[j].pos;
						str = DC->feederItemText(item->special, i, j, &optionalImage1, &optionalImage2, &optionalImage3);

						if( !str )
						{
							continue;
						}

						if (str[0]=='@')
						{
							trap->SE_GetStringTextString( &str[1]  , temp, sizeof(temp));
							str = temp;
						}

						/*
						if (optionalImage >= 0) {
							DC->drawHandlePic(x + 4 + listPtr->columnInfo[j].pos, y - 1 + listPtr->elementHeight / 2, listPtr->columnInfo[j].width, listPtr->columnInfo[j].width, optionalImage);
						}
						else
						*/
						if ( str )
						{
//JLF MPMOVED
							int textyOffset;
							textyOffset = 0;
//							Text_Paint( text, x + 4 + listPtr->columnInfo[j].pos, y + listPtr->elementHeight, item->textscale, item->window.foreColor, str, 0, listPtr->columnInfo[j].maxChars, item->textStyle);
	//WAS LAST						Text_Paint( text, x + 4 + listPtr->columnInfo[j].pos, y, item->textscale, item->window.foreColor, str, 0, listPtr->columnInfo[j].maxChars, item->textStyle);
							Text_Paint( text, x + 4 + listPtr->columnInfo[j].pos, y + listPtr->elementHeight+ textyOffset + item->textaligny, str, item->window.foreColor, listPtr->columnInfo[j].maxChars, item->textStyle);

//JLF END
						}
						if ( j < listPtr->numColumns - 1 )
						{
							imageStartX = listPtr->columnInfo[j+1].pos;
						}
						DC->setColor( nullptr );
						if (optionalImage3 >= 0)
						{
							DC->drawHandlePic(imageStartX - listPtr->elementHeight*3, y+listPtr->elementHeight+2, listPtr->elementHeight, listPtr->elementHeight, optionalImage3);
						}
						if (optionalImage2 >= 0)
						{
							DC->drawHandlePic(imageStartX - listPtr->elementHeight*2, y+listPtr->elementHeight+2, listPtr->elementHeight, listPtr->elementHeight, optionalImage2);
						}
						if (optionalImage1 >= 0)
						{
							DC->drawHandlePic(imageStartX - listPtr->elementHeight, y+listPtr->elementHeight+2, listPtr->elementHeight, listPtr->elementHeight, optionalImage1);
						}
					}
				}
				else
				{
					str = DC->feederItemText(item->special, i, 0, &optionalImage1, &optionalImage2, &optionalImage3 );
					if ( optionalImage1 >= 0 || optionalImage2 >= 0 || optionalImage3 >= 0)
					{
						//DC->drawHandlePic(x + 4 + listPtr->elementHeight, y, listPtr->columnInfo[j].width, listPtr->columnInfo[j].width, optionalImage);
					}
					else if (str)
					{
//						Text_Paint( text, x + 4, y + listPtr->elementHeight, item->textscale, item->window.foreColor, str, 0, 0, item->textStyle);
						Text_Paint( text, x + 4, y + item->textaligny, str, item->window.foreColor, item->textStyle);
					}
				}

				if (i == item->cursorPos)
				{
					DC->fillRect(x + 2, y + listPtr->elementHeight + 2, item->window.rect.w - SCROLLBAR_SIZE - 4, listPtr->elementHeight, item->window.outlineColor);
				}

				sizeHeight -= listPtr->elementHeight;
				if (sizeHeight < listPtr->elementHeight)
				{
					listPtr->drawPadding = listPtr->elementHeight - sizeHeight;
					break;
				}
				listPtr->endPos++;
				y += listPtr->elementHeight;
				// fit++;
			}
		}
	}
}

void Item_OwnerDraw_Paint(itemDef_t *item) {

	if (item == nullptr) {
		return;
	}

	if (DC->ownerDrawItem) {
		vec4_t color, lowLight;
		menuDef_t *parent = (menuDef_t*)item->parent;
		Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, true, parent->fadeAmount);
		memcpy(&color, &item->window.foreColor, sizeof(color));
		if (item->numColors > 0 && DC->getValue) {
			// if the value is within one of the ranges then set color to that, otherwise leave at default
			int i;
			float f = DC->getValue(item->window.ownerDraw);
			for (i = 0; i < item->numColors; i++) {
				if (f >= item->colorRanges[i].low && f <= item->colorRanges[i].high) {
					memcpy(&color, &item->colorRanges[i].color, sizeof(color));
					break;
				}
			}
		}

		if (item->window.flags & WINDOW_HASFOCUS) {
			lowLight[0] = 0.8 * parent->focusColor[0];
			lowLight[1] = 0.8 * parent->focusColor[1];
			lowLight[2] = 0.8 * parent->focusColor[2];
			lowLight[3] = 0.8 * parent->focusColor[3];
			LerpColor(parent->focusColor,lowLight,color,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
		} else if (item->textStyle == ITEM_TEXTSTYLE_BLINK && !((DC->realTime/BLINK_DIVISOR) & 1)) {
			lowLight[0] = 0.8 * item->window.foreColor[0];
			lowLight[1] = 0.8 * item->window.foreColor[1];
			lowLight[2] = 0.8 * item->window.foreColor[2];
			lowLight[3] = 0.8 * item->window.foreColor[3];
			LerpColor(item->window.foreColor,lowLight,color,0.5+0.5*sin((float)(DC->realTime / PULSE_DIVISOR)));
		}

		if ( item->disabled )
			memcpy( color, parent->disableColor, sizeof( vec4_t ) );

		if ( item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar( item, CVAR_ENABLE ) )
			memcpy( color, parent->disableColor, sizeof( vec4_t ) );

		if (item->text) {
			Item_Text_Paint(item);
				if (item->text[0]) {
					// +8 is an offset kludge to properly align owner draw items that have text combined with them
					DC->ownerDrawItem(item->textRect.x + item->textRect.w + 8, item->window.rect.y, item->window.rect.w, item->window.rect.h, 0, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags, item->alignment, item->special, item->textscale, color, item->window.background, item->textStyle,item->font );
				} else {
					DC->ownerDrawItem(item->textRect.x + item->textRect.w, item->window.rect.y, item->window.rect.w, item->window.rect.h, 0, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags, item->alignment, item->special, item->textscale, color, item->window.background, item->textStyle,item->font );
				}
			} else {
			DC->ownerDrawItem(item->window.rect.x, item->window.rect.y, item->window.rect.w, item->window.rect.h, item->textalignx, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags, item->alignment, item->special, item->textscale, color, item->window.background, item->textStyle,item->font );
		}
	}
}

void Item_Paint(itemDef_t *item)
{
	vec4_t		red;
	menuDef_t *parent;
	int			xPos,textWidth;
	vec4_t		color = {1, 1, 1, 1};

	red[0] = red[3] = 1;
	red[1] = red[2] = 0;

	if (item == nullptr)
	{
		return;
	}

	parent = (menuDef_t*)item->parent;

	const double tAdjust = DC->frameTime / (1000.0/60.0); //(60.0 / DC->FPS);

	if (item->window.flags & WINDOW_ORBITING)
	{
		if (DC->realTime > item->window.nextTime)
		{
			float rx, ry, a, c, s, w, h;

			item->window.nextTime = DC->realTime + (item->window.offsetTime * tAdjust);
			// translate
			w = item->window.rectClient.w / 2;
			h = item->window.rectClient.h / 2;
			rx = item->window.rectClient.x + ((w - item->window.rectEffects.x) * tAdjust);
			ry = item->window.rectClient.y + ((h - item->window.rectEffects.y) * tAdjust);
			a = 3 * M_PI / 180;
			c = cos(a);
			s = sin(a);
			item->window.rectClient.x = (rx * c - ry * s) + ((item->window.rectEffects.x - w) * tAdjust);
			item->window.rectClient.y = (rx * s + ry * c) + ((item->window.rectEffects.y - h) * tAdjust);
			Item_UpdatePosition(item);
		}
	}

	if (item->window.flags & WINDOW_INTRANSITION)
	{
		if (DC->realTime > item->window.nextTime)
		{
			int done = 0;

			item->window.nextTime = DC->realTime + (item->window.offsetTime * tAdjust);


			// transition the x,y
			if (item->window.rectClient.x == item->window.rectEffects.x)
			{
				done++;
			}
			else
			{
				if (item->window.rectClient.x < item->window.rectEffects.x)
				{
					item->window.rectClient.x += item->window.rectEffects2.x * tAdjust;
					if (item->window.rectClient.x > item->window.rectEffects.x)
					{
						item->window.rectClient.x = item->window.rectEffects.x;
						done++;
					}
				}
				else
				{
					item->window.rectClient.x -= item->window.rectEffects2.x * tAdjust;
					if (item->window.rectClient.x < item->window.rectEffects.x)
					{
						item->window.rectClient.x = item->window.rectEffects.x;
						done++;
					}
				}
			}

			if (item->window.rectClient.y == item->window.rectEffects.y)
			{
				done++;
			}
			else
			{
				if (item->window.rectClient.y < item->window.rectEffects.y)
				{
					item->window.rectClient.y += item->window.rectEffects2.y * tAdjust;
					if (item->window.rectClient.y > item->window.rectEffects.y)
					{
						item->window.rectClient.y = item->window.rectEffects.y;
						done++;
					}
				}
				else
				{
					item->window.rectClient.y -= item->window.rectEffects2.y * tAdjust;
					if (item->window.rectClient.y < item->window.rectEffects.y)
					{
						item->window.rectClient.y = item->window.rectEffects.y;
						done++;
					}
				}
			}

			if (item->window.rectClient.w == item->window.rectEffects.w)
			{
				done++;
			}
			else
			{
				if (item->window.rectClient.w < item->window.rectEffects.w)
				{
					item->window.rectClient.w += item->window.rectEffects2.w * tAdjust;
					if (item->window.rectClient.w > item->window.rectEffects.w)
					{
						item->window.rectClient.w = item->window.rectEffects.w;
						done++;
					}
				}
				else
				{
					item->window.rectClient.w -= item->window.rectEffects2.w * tAdjust;
					if (item->window.rectClient.w < item->window.rectEffects.w)
					{
						item->window.rectClient.w = item->window.rectEffects.w;
						done++;
					}
				}
			}

			if (item->window.rectClient.h == item->window.rectEffects.h)
			{
				done++;
			}
			else
			{
				if (item->window.rectClient.h < item->window.rectEffects.h)
				{
					item->window.rectClient.h += item->window.rectEffects2.h * tAdjust;
					if (item->window.rectClient.h > item->window.rectEffects.h)
					{
						item->window.rectClient.h = item->window.rectEffects.h;
						done++;
					}
				}
				else
				{
					item->window.rectClient.h -= item->window.rectEffects2.h * tAdjust;
					if (item->window.rectClient.h < item->window.rectEffects.h)
					{
						item->window.rectClient.h = item->window.rectEffects.h;
						done++;
					}
				}
			}

			Item_UpdatePosition(item);

			if (done == 4)
			{
				item->window.flags &= ~WINDOW_INTRANSITION;
			}

		}
	}

#ifdef _TRANS3

//JLF begin model transition stuff
	if (item->window.flags & WINDOW_INTRANSITIONMODEL)
	{
		if ( item->type == ITEM_TYPE_MODEL)
		{
//fields ing modelptr
//				vec3_t g2mins2, g2maxs2, g2minsEffect, g2maxsEffect;
//	float fov_x2, fov_y2, fov_Effectx, fov_Effecty;

			modelDef_t  * modelptr = item->typeData.model;

			if (DC->realTime > item->window.nextTime)
			{
				int done = 0;
				item->window.nextTime = DC->realTime + (item->window.offsetTime * tAdjust);

// transition the x,y,z max
				if (modelptr->g2maxs[0] == modelptr->g2maxs2[0])
				{
					done++;
				}
				else
				{
					if (modelptr->g2maxs[0] < modelptr->g2maxs2[0])
					{
						modelptr->g2maxs[0] += modelptr->g2maxsEffect[0] * tAdjust;
						if (modelptr->g2maxs[0] > modelptr->g2maxs2[0])
						{
							modelptr->g2maxs[0] = modelptr->g2maxs2[0];
							done++;
						}
					}
					else
					{
						modelptr->g2maxs[0] -= modelptr->g2maxsEffect[0] * tAdjust;
						if (modelptr->g2maxs[0] < modelptr->g2maxs2[0])
						{
							modelptr->g2maxs[0] = modelptr->g2maxs2[0];
							done++;
						}
					}
				}
//y
				if (modelptr->g2maxs[1] == modelptr->g2maxs2[1])
				{
					done++;
				}
				else
				{
					if (modelptr->g2maxs[1] < modelptr->g2maxs2[1])
					{
						modelptr->g2maxs[1] += modelptr->g2maxsEffect[1] * tAdjust;
						if (modelptr->g2maxs[1] > modelptr->g2maxs2[1])
						{
							modelptr->g2maxs[1] = modelptr->g2maxs2[1];
							done++;
						}
					}
					else
					{
						modelptr->g2maxs[1] -= modelptr->g2maxsEffect[1] * tAdjust;
						if (modelptr->g2maxs[1] < modelptr->g2maxs2[1])
						{
							modelptr->g2maxs[1] = modelptr->g2maxs2[1];
							done++;
						}
					}
				}

//z

				if (modelptr->g2maxs[2] == modelptr->g2maxs2[2])
				{
					done++;
				}
				else
				{
					if (modelptr->g2maxs[2] < modelptr->g2maxs2[2])
					{
						modelptr->g2maxs[2] += modelptr->g2maxsEffect[2] * tAdjust;
						if (modelptr->g2maxs[2] > modelptr->g2maxs2[2])
						{
							modelptr->g2maxs[2] = modelptr->g2maxs2[2];
							done++;
						}
					}
					else
					{
						modelptr->g2maxs[2] -= modelptr->g2maxsEffect[2] * tAdjust;
						if (modelptr->g2maxs[2] < modelptr->g2maxs2[2])
						{
							modelptr->g2maxs[2] = modelptr->g2maxs2[2];
							done++;
						}
					}
				}

// transition the x,y,z min
				if (modelptr->g2mins[0] == modelptr->g2mins2[0])
				{
					done++;
				}
				else
				{
					if (modelptr->g2mins[0] < modelptr->g2mins2[0])
					{
						modelptr->g2mins[0] += modelptr->g2minsEffect[0] * tAdjust;
						if (modelptr->g2mins[0] > modelptr->g2mins2[0])
						{
							modelptr->g2mins[0] = modelptr->g2mins2[0];
							done++;
						}
					}
					else
					{
						modelptr->g2mins[0] -= modelptr->g2minsEffect[0] * tAdjust;
						if (modelptr->g2mins[0] < modelptr->g2mins2[0])
						{
							modelptr->g2mins[0] = modelptr->g2mins2[0];
							done++;
						}
					}
				}
//y
				if (modelptr->g2mins[1] == modelptr->g2mins2[1])
				{
					done++;
				}
				else
				{
					if (modelptr->g2mins[1] < modelptr->g2mins2[1])
					{
						modelptr->g2mins[1] += modelptr->g2minsEffect[1] * tAdjust;
						if (modelptr->g2mins[1] > modelptr->g2mins2[1])
						{
							modelptr->g2mins[1] = modelptr->g2mins2[1];
							done++;
						}
					}
					else
					{
						modelptr->g2mins[1] -= modelptr->g2minsEffect[1] * tAdjust;
						if (modelptr->g2mins[1] < modelptr->g2mins2[1])
						{
							modelptr->g2mins[1] = modelptr->g2mins2[1];
							done++;
						}
					}
				}

//z

				if (modelptr->g2mins[2] == modelptr->g2mins2[2])
				{
					done++;
				}
				else
				{
					if (modelptr->g2mins[2] < modelptr->g2mins2[2])
					{
						modelptr->g2mins[2] += modelptr->g2minsEffect[2] * tAdjust;
						if (modelptr->g2mins[2] > modelptr->g2mins2[2])
						{
							modelptr->g2mins[2] = modelptr->g2mins2[2];
							done++;
						}
					}
					else
					{
						modelptr->g2mins[2] -= modelptr->g2minsEffect[2] * tAdjust;
						if (modelptr->g2mins[2] < modelptr->g2mins2[2])
						{
							modelptr->g2mins[2] = modelptr->g2mins2[2];
							done++;
						}
					}
				}

//fovx
				if (modelptr->fov_x == modelptr->fov_x2)
				{
					done++;
				}
				else
				{
					if (modelptr->fov_x < modelptr->fov_x2)
					{
						modelptr->fov_x += modelptr->fov_Effectx * tAdjust;
						if (modelptr->fov_x > modelptr->fov_x2)
						{
							modelptr->fov_x = modelptr->fov_x2;
							done++;
						}
					}
					else
					{
						modelptr->fov_x -= modelptr->fov_Effectx * tAdjust;
						if (modelptr->fov_x < modelptr->fov_x2)
						{
							modelptr->fov_x = modelptr->fov_x2;
							done++;
						}
					}
				}

//fovy
				if (modelptr->fov_y == modelptr->fov_y2)
				{
					done++;
				}
				else
				{
					if (modelptr->fov_y < modelptr->fov_y2)
					{
						modelptr->fov_y += modelptr->fov_Effecty * tAdjust;
						if (modelptr->fov_y > modelptr->fov_y2)
						{
							modelptr->fov_y = modelptr->fov_y2;
							done++;
						}
					}
					else
					{
						modelptr->fov_y -= modelptr->fov_Effecty * tAdjust;
						if (modelptr->fov_y < modelptr->fov_y2)
						{
							modelptr->fov_y = modelptr->fov_y2;
							done++;
						}
					}
				}

				if (done == 5)
				{
					item->window.flags &= ~WINDOW_INTRANSITIONMODEL;
				}

			}
		}
	}
#endif
//JLF end transition stuff for models

	if (item->window.ownerDrawFlags && DC->ownerDrawVisible) {
		if (!DC->ownerDrawVisible(item->window.ownerDrawFlags)) {
			item->window.flags &= ~WINDOW_VISIBLE;
		} else {
			item->window.flags |= WINDOW_VISIBLE;
		}
	}

	if (item->disabled && item->disabledHidden)
	{
		return;
	}

	if (item->cvarFlags & (CVAR_SHOW | CVAR_HIDE)) {
		if (!Item_EnableShowViaCvar(item, CVAR_SHOW)) {
			return;
		}
	}

  if (item->window.flags & WINDOW_TIMEDVISIBLE) {

	}

	if (!(item->window.flags & WINDOW_VISIBLE))
	{
		return;
	}

	if (item->window.flags & WINDOW_MOUSEOVER)
	{
		if (item->descText && !Display_KeyBindPending())
		{
			// Make DOUBLY sure that this item should have desctext.
			// NOTE : we can't just check the mouse position on this, what if we TABBED
			// to the current menu item -- in that case our mouse isn't over the item.
			// Removing the WINDOW_MOUSEOVER flag just prevents the item's OnExit script from running
	//	    if (!Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory))
	//		{	// It isn't something that should, because it isn't live anymore.
	//			item->window.flags &= ~WINDOW_MOUSEOVER;
	//		}
	//		else
			{	// Draw the desctext
				const char *textPtr = item->descText;
				char temp[MAX_STRING_CHARS] = {0};
				if (*textPtr == '@')	// string reference
				{
					trap->SE_GetStringTextString( &textPtr[1], temp, sizeof(temp));
					textPtr = temp;
				}

				Item_TextColor(item, &color);

				{// stupid C language
					float fDescScale = parent->descScale ? parent->descScale : 1;
					float fDescScaleCopy = fDescScale;
					int iYadj = 0;
					Text text{ JKFont::Small2, fDescScale };
					while (1)
					{
						textWidth = Text_Width( text, textPtr);

						if (parent->descAlignment == ITEM_ALIGN_RIGHT)
						{
							xPos = parent->descX - textWidth;	// Right justify
						}
						else if (parent->descAlignment == ITEM_ALIGN_CENTER)
						{
							xPos = parent->descX - (textWidth/2);	// Center justify
						}
						else										// Left justify
						{
							xPos = parent->descX;
						}

						if (parent->descAlignment == ITEM_ALIGN_CENTER)
						{
							// only this one will auto-shrink the scale until we eventually fit...

							if (xPos + textWidth > (SCREEN_WIDTH-4)) {
								fDescScale -= 0.001f;
								continue;
							}
						}

						// Try to adjust it's y placement if the scale has changed...

						if (fDescScale != fDescScaleCopy)
						{
							int iOriginalTextHeight = Text_Height( text, textPtr);
							iYadj = iOriginalTextHeight - Text_Height( text, textPtr);
						}

						Text_Paint( text, xPos, parent->descY + iYadj, textPtr, parent->descColor, item->textStyle);
						break;
					}
				}
			}
		}
	}

	// paint the rect first..
	Window_Paint( &item->window, parent->fadeAmount , parent->fadeClamp, parent->fadeCycle );

	// Draw box to show rectangle extents, in debug mode
	if ( debugMode ) {
		vec4_t color;
		color[1] = color[3] = 1;
		color[0] = color[2] = 0;
		DC->drawRect(
			item->window.rect.x,
			item->window.rect.y,
			item->window.rect.w,
			item->window.rect.h,
			1,
			color
		);
	}

	//DC->drawRect(item->window.rect.x, item->window.rect.y, item->window.rect.w, item->window.rect.h, 1, red);

	switch ( item->type ) {

	case ITEM_TYPE_OWNERDRAW: {
		Item_OwnerDraw_Paint( item );
	} break;

	case ITEM_TYPE_TEXT:
	case ITEM_TYPE_BUTTON: {
		Item_Text_Paint( item );
	} break;

	case ITEM_TYPE_EDITFIELD:
	case ITEM_TYPE_NUMERICFIELD: {
		Item_TextField_Paint( item );
	} break;

	case ITEM_TYPE_LISTBOX: {
		Item_ListBox_Paint( item );
	} break;

	case ITEM_TYPE_TEXTSCROLL: {
		Item_TextScroll_Paint( item );
	} break;

	/*
	case ITEM_TYPE_IMAGE: {
		Item_Image_Paint( item );
	} break;
	*/

	case ITEM_TYPE_MODEL: {
		Item_Model_Paint( item );
	} break;

	case ITEM_TYPE_YESNO: {
		Item_YesNo_Paint( item );
	} break;

	case ITEM_TYPE_MULTI: {
		Item_Multi_Paint( item );
	} break;

	case ITEM_TYPE_BIND: {
		Item_Bind_Paint( item );
	} break;

	case ITEM_TYPE_SLIDER: {
		Item_Slider_Paint( item );
	} break;

	case ITEM_TYPE_RADIOBUTTON:
	case ITEM_TYPE_CHECKBOX:
	case ITEM_TYPE_COMBO:
	case ITEM_TYPE_SHADER:
	default: {
	} break;

	}

	//FIXME: this might be bad
	DC->setColor( nullptr );
}

void Menu_Init(menuDef_t *menu) {
	memset(menu, 0, sizeof(menuDef_t));
	menu->cursorItem = -1;
	menu->fadeAmount = DC->Assets.fadeAmount;
	menu->fadeClamp = DC->Assets.fadeClamp;
	menu->fadeCycle = DC->Assets.fadeCycle;
	Window_Init(&menu->window);
}

itemDef_t *Menu_GetFocusedItem(menuDef_t *menu) {
	int i;
	if (menu) {
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->window.flags & WINDOW_HASFOCUS) {
				return menu->items[i];
			}
		}
	}
	return nullptr;
}

menuDef_t *Menu_GetFocused() {
	int i;
	for (i = 0; i < menuCount; i++) {
		if (Menus[i].window.flags & WINDOW_HASFOCUS && Menus[i].window.flags & WINDOW_VISIBLE) {
			return &Menus[i];
		}
	}
	return nullptr;
}

void Menu_ScrollFeeder(menuDef_t *menu, int feeder, bool down) {
	if (menu) {
		int i;
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->special == feeder) {
				Item_ListBox_HandleKey(menu->items[i], (down) ? A_CURSOR_DOWN : A_CURSOR_UP, true, true);
				return;
			}
		}
	}
}

void Menu_SetFeederSelection(menuDef_t *menu, int feeder, int index, const char *name) {
	if (menu == nullptr) {
		if (name == nullptr) {
			menu = Menu_GetFocused();
		} else {
			menu = Menus_FindByName(name);
		}
	}

	if (menu) {
		int i;
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->special == feeder) {
				if (index == 0) {
					listBoxDef_t *listPtr = menu->items[i]->typeData.listbox;
					listPtr->cursorPos = 0;
					listPtr->startPos = 0;
				}
				menu->items[i]->cursorPos = index;
				DC->feederSelection(menu->items[i]->special, menu->items[i]->cursorPos, nullptr);
				return;
			}
		}
	}
}

bool Menus_AnyFullScreenVisible( void ) {
  int i;
  for (i = 0; i < menuCount; i++) {
    if (Menus[i].window.flags & WINDOW_VISIBLE && Menus[i].fullScreen) {
			return true;
    }
  }
  return false;
}

menuDef_t *Menus_ActivateByName(const char *p) {
	menuDef_t *focus = Menu_GetFocused();

	menuDef_t *m = nullptr;
	for ( int i=0; i<menuCount; i++ ) {
		if ( !Q_stricmp( Menus[i].window.name, p ) ) {
			m = &Menus[i];
			Menus_Activate( m );
			if ( openMenuCount < MAX_OPEN_MENUS && focus != nullptr ) {
				menuStack[openMenuCount++] = focus;
			}
		}
		else {
			Menus[i].window.flags &= ~WINDOW_HASFOCUS;
		}
	}

	Display_CloseCinematics();

	// Want to handle a mouse move on the new menu in case your already over an item
	Menu_HandleMouseMove( m, DC->cursorx, DC->cursory );

	return m;
}

void Item_Init(itemDef_t *item) {
	memset(item, 0, sizeof(itemDef_t));
	item->textscale = 1.0f;
	item->font = JKFont::Small;
	Window_Init(&item->window);
}

void Menu_HandleMouseMove(menuDef_t *menu, float x, float y) {
  int i, pass;
  bool focusSet = false;

  itemDef_t *overItem;
  if (menu == nullptr) {
    return;
  }

  if (!(menu->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED))) {
    return;
  }

	if (itemCapture) {
		//Item_MouseMove(itemCapture, x, y);
		return;
	}

	if (g_waitingForKey || g_editingField) {
		return;
	}

  // FIXME: this is the whole issue of focus vs. mouse over..
  // need a better overall solution as i don't like going through everything twice
  for (pass = 0; pass < 2; pass++) {
    for (i = 0; i < menu->itemCount; i++) {
      // turn off focus each item
      // menu->items[i].window.flags &= ~WINDOW_HASFOCUS;

      if (!(menu->items[i]->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED))) {
        continue;
      }

			if (menu->items[i]->disabled)
			{
				continue;
			}

			// items can be enabled and disabled based on cvars
			if (menu->items[i]->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(menu->items[i], CVAR_ENABLE)) {
				continue;
			}

			if (menu->items[i]->cvarFlags & (CVAR_SHOW | CVAR_HIDE) && !Item_EnableShowViaCvar(menu->items[i], CVAR_SHOW)) {
				continue;
			}

      if (Rect_ContainsPoint(&menu->items[i]->window.rect, x, y)) {
				if (pass == 1) {
					overItem = menu->items[i];
					if (overItem->type == ITEM_TYPE_TEXT && overItem->text) {
						if (!Rect_ContainsPoint(&overItem->window.rect, x, y)) {
							continue;
						}
					}
					// if we are over an item
					if (IsVisible(overItem->window.flags)) {
						// different one
						Item_MouseEnter(overItem, x, y);
						// Item_SetMouseOver(overItem, true);

						// if item is not a decoration see if it can take focus
						if (!focusSet) {
							focusSet = Item_SetFocus(overItem, x, y);
						}
					}
				}
      } else if (menu->items[i]->window.flags & WINDOW_MOUSEOVER) {
          Item_MouseLeave(menu->items[i]);
          Item_SetMouseOver(menu->items[i], false);
      }
    }
  }

}

void Menu_Paint(menuDef_t *menu, bool forcePaint) {
	int i;

	if (menu == nullptr) {
		return;
	}

	if (!(menu->window.flags & WINDOW_VISIBLE) &&  !forcePaint) {
		return;
	}

	if (menu->window.ownerDrawFlags && DC->ownerDrawVisible && !DC->ownerDrawVisible(menu->window.ownerDrawFlags)) {
		return;
	}

	if (forcePaint) {
		menu->window.flags |= WINDOW_FORCED;
	}

	// draw the background if necessary
	if (menu->fullScreen) {
		// implies a background shader
		// FIXME: make sure we have a default shader if fullscreen is set with no background
		DC->drawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, menu->window.background );
	} else if (menu->window.background) {
		// this allows a background shader without being full screen
		//UI_DrawHandlePic(menu->window.rect.x, menu->window.rect.y, menu->window.rect.w, menu->window.rect.h, menu->backgroundShader);
	}

	// paint the background and or border
	Window_Paint(&menu->window, menu->fadeAmount, menu->fadeClamp, menu->fadeCycle );

	// Loop through all items for the menu and paint them
	for (i = 0; i < menu->itemCount; i++)
	{
		if (!menu->items[i]->appearanceSlot)
		{
			Item_Paint(menu->items[i]);
		}
		else // Timed order of appearance
		{
			if (menu->appearanceTime < DC->realTime)	// Time to show another item
			{
				menu->appearanceTime = DC->realTime + menu->appearanceIncrement;
				menu->appearanceCnt++;
			}

			if (menu->items[i]->appearanceSlot<=menu->appearanceCnt)
			{
				Item_Paint(menu->items[i]);
			}
		}
	}

	if (debugMode) {
		vec4_t color;
		color[0] = color[2] = color[3] = 1;
		color[1] = 0;
		DC->drawRect(menu->window.rect.x, menu->window.rect.y, menu->window.rect.w, menu->window.rect.h, 1, color);
	}
}

void Item_ValidateTypeData(itemDef_t *item)
{
	if (item->typeData.data)
	{
		return;
	}

	switch ( item->type )
	{
		case ITEM_TYPE_LISTBOX:
		{
			item->typeData.listbox = (listBoxDef_t *)UI_Alloc(sizeof(listBoxDef_t));
			memset(item->typeData.listbox, 0, sizeof(listBoxDef_t));
			break;
		}
		case ITEM_TYPE_TEXT:
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
		case ITEM_TYPE_YESNO:
		case ITEM_TYPE_BIND:
		case ITEM_TYPE_SLIDER:
		{
			item->typeData.edit = (editFieldDef_t *)UI_Alloc(sizeof(editFieldDef_t));
			memset(item->typeData.edit, 0, sizeof(editFieldDef_t));

			if ( item->type == ITEM_TYPE_EDITFIELD || item->type == ITEM_TYPE_NUMERICFIELD )
				item->typeData.edit->maxPaintChars = MAX_EDITFIELD;
			break;
		}
		case ITEM_TYPE_MULTI:
		{
			item->typeData.multi = (multiDef_t *)UI_Alloc(sizeof(multiDef_t));
			memset(item->typeData.multi, 0, sizeof(multiDef_t));
			break;
		}
		case ITEM_TYPE_MODEL:
		{
			item->typeData.model = (modelDef_t *)UI_Alloc(sizeof(modelDef_t));
			memset(item->typeData.model, 0, sizeof(modelDef_t));
			break;
		}
		case ITEM_TYPE_TEXTSCROLL:
		{
			item->typeData.textscroll = (textScrollDef_t *)UI_Alloc(sizeof(textScrollDef_t));
			memset(item->typeData.textscroll, 0, sizeof(textScrollDef_t));
			break;
		}
		default:
			break;
	}
}

#define KEYWORDHASH_SIZE	512

struct keywordHash_t {
	char *keyword;
	bool (*func)(itemDef_t *item, int handle);
	keywordHash_t *next;
};

static int KeywordHash_Key(char *keyword) {
	int hash, i;

	hash = 0;
	for (i = 0; keyword[i] != '\0'; i++) {
		if (keyword[i] >= 'A' && keyword[i] <= 'Z')
			hash += (keyword[i] + ('a' - 'A')) * (119 + i);
		else
			hash += keyword[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20)) & (KEYWORDHASH_SIZE-1);
	return hash;
}

static void KeywordHash_Add(keywordHash_t *table[], keywordHash_t *key) {
	int hash;

	hash = KeywordHash_Key(key->keyword);
/*
	if (table[hash]) {
		bool collision = true;
	}
*/
	key->next = table[hash];
	table[hash] = key;
}

static keywordHash_t *KeywordHash_Find(keywordHash_t *table[], char *keyword)
{
	keywordHash_t *key;
	int hash;

	hash = KeywordHash_Key(keyword);
	for (key = table[hash]; key; key = key->next) {
		if (!Q_stricmp(key->keyword, keyword))
			return key;
	}
	return nullptr;
}

// Item Keyword Parse functions

// name <string>
bool ItemParse_name( itemDef_t *item, int handle ) {
	if (!PC_String_Parse(handle, &item->window.name)) {
		return false;
	}
	return true;
}

// name <string>
bool ItemParse_focusSound( itemDef_t *item, int handle ) {
	pc_token_t token;
	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}
	item->focusSound = DC->registerSound(token.string);
	return true;
}

// text <string>
bool ItemParse_text( itemDef_t *item, int handle ) {
	if (!PC_String_Parse(handle, &item->text)) {
		return false;
	}
	return true;
}

// text <string>
bool ItemParse_descText( itemDef_t *item, int handle)
{

	if (!PC_String_Parse(handle, &item->descText))
	{
		return false;
	}

	return true;

}

// text <string>
bool ItemParse_text2( itemDef_t *item, int handle)
{

	if (!PC_String_Parse(handle, &item->text2))
	{
		return false;
	}

	return true;

}

bool ItemParse_text2alignx( itemDef_t *item, int handle)
{
	if (!PC_Float_Parse(handle, &item->text2alignx))
	{
		return false;
	}
	return true;
}

bool ItemParse_text2aligny( itemDef_t *item, int handle)
{
	if (!PC_Float_Parse(handle, &item->text2aligny))
	{
		return false;
	}
	return true;
}

// group <string>
bool ItemParse_group( itemDef_t *item, int handle ) {
	if (!PC_String_Parse(handle, &item->window.group)) {
		return false;
	}
	return true;
}

struct uiG2PtrTracker_t {
	void *ghoul2;
	uiG2PtrTracker_t *next;
};

uiG2PtrTracker_t *ui_G2PtrTracker = nullptr;

//rww - UI G2 shared management functions.

//Insert the pointer into our chain so we can keep track of it for freeing.
void UI_InsertG2Pointer(void *ghoul2)
{
	uiG2PtrTracker_t **nextFree = &ui_G2PtrTracker;

	while ((*nextFree) && (*nextFree)->ghoul2)
	{ //check if it has a ghoul2, if not we can reuse it.
		nextFree = &((*nextFree)->next);
	}

	if (!nextFree)
	{ //shouldn't happen
		assert(0);
		return;
	}

	if (!(*nextFree))
	{ //if we aren't reusing a chain then allocate space for it.
		(*nextFree) = (uiG2PtrTracker_t *)BG_Alloc(sizeof(uiG2PtrTracker_t));
		(*nextFree)->next = nullptr;
	}

	(*nextFree)->ghoul2 = ghoul2;
}

//Remove a ghoul2 pointer from the chain if it's there.
void UI_ClearG2Pointer(void *ghoul2)
{
	uiG2PtrTracker_t *next = ui_G2PtrTracker;

	if (!ghoul2)
	{
		return;
	}

	while (next)
	{
		if (next->ghoul2 == ghoul2)
		{ //found it, set it to null so we can reuse this link.
			next->ghoul2 = nullptr;
			break;
		}

		next = next->next;
	}
}

//Called on shutdown, cleans up all the ghoul2 instances laying around.
void UI_CleanupGhoul2(void)
{
	uiG2PtrTracker_t *next = ui_G2PtrTracker;

	while (next)
	{
		if (next->ghoul2 && trap->G2_HaveWeGhoul2Models(next->ghoul2)) //found a g2 instance, clean it.
			trap->G2API_CleanGhoul2Models(&next->ghoul2);

		next = next->next;
	}
}

// asset_model <string>
bool ItemParse_asset_model_go( itemDef_t *item, const char *name,int *runTimeLength )
{
#ifdef BUILD_UI
	int g2Model;
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;
	*runTimeLength =0.0f;

	if (!Q_stricmp(&name[strlen(name) - 4], ".glm"))
	{ //it's a ghoul2 model then
		if ( item->ghoul2 )
		{
			UI_ClearG2Pointer(item->ghoul2);	//remove from tracking list
			trap->G2API_CleanGhoul2Models(&item->ghoul2);	//remove ghoul info
			item->flags &= ~ITF_G2VALID;
		}

		g2Model = trap->G2API_InitGhoul2Model(&item->ghoul2, name, 0, modelPtr->g2skin, 0, 0, 0);
		if (g2Model >= 0)
		{
			UI_InsertG2Pointer(item->ghoul2); //remember it so we can free it when the ui shuts down.
			item->flags |= ITF_G2VALID;

			if (modelPtr->g2anim)
			{ //does the menu request this model be playing an animation?
//					DC->g2hilev_SetAnim(&item->ghoul2[0], "model_root", modelPtr->g2anim);

				char GLAName[MAX_QPATH];

				GLAName[0] = 0;
				trap->G2API_GetGLAName(item->ghoul2, 0, GLAName);

				if (GLAName[0])
				{
					int animIndex;
					char *slash;

					slash = Q_strrchr( GLAName, '/' );

					if ( slash )
					{ //If this isn't true the gla path must be messed up somehow.
						strcpy(slash, "/animation.cfg");

						animIndex = UI_ParseAnimationFile(GLAName, nullptr, false);
						if (animIndex != -1)
						{ //We parsed out the animation info for whatever model this is
							animation_t *anim = &bgAllAnims[animIndex].anims[modelPtr->g2anim];

							int sFrame = anim->firstFrame;
							int eFrame = anim->firstFrame + anim->numFrames;
							int flags = BONE_ANIM_OVERRIDE_FREEZE;
							int time = DC->realTime;
							float animSpeed = 50.0f / anim->frameLerp;
							int blendTime = 150;

							if (anim->loopFrames != -1)
							{
								flags |= BONE_ANIM_OVERRIDE_LOOP;
							}

							trap->G2API_SetBoneAnim(item->ghoul2, 0, "model_root", sFrame, eFrame, flags, animSpeed, time, -1, blendTime);
							*runTimeLength =((anim->frameLerp * (anim->numFrames-2)));
						}
					}
				}
			}

			if ( modelPtr->g2skin )
			{
//					DC->g2_SetSkin( &item->ghoul2[0], 0, modelPtr->g2skin );//this is going to set the surfs on/off matching the skin file
				//trap->G2API_InitGhoul2Model(&item->ghoul2, name, 0, modelPtr->g2skin, 0, 0, 0);
				//ahh, what are you doing?!
				trap->G2API_SetSkin(item->ghoul2, 0, modelPtr->g2skin, modelPtr->g2skin);
			}
		}
		/*
		else
		{
			Com_Error(ERR_FATAL, "%s does not exist.", name);
		}
		*/
	}
	else if(!(item->asset))
	{ //guess it's just an md3
		item->asset = DC->registerModel(name);
		item->flags &= ~ITF_G2VALID;
	}
#endif
	return true;
}

bool ItemParse_asset_model( itemDef_t *item, int handle ) {
	int animRunLength;
	pc_token_t token;

	Item_ValidateTypeData(item);

	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}

#ifdef BUILD_UI
	if (!Q_stricmp(token.string,"ui_char_model") )
	{
		char modelPath[MAX_QPATH] = {0};
		char ui_char_model[MAX_QPATH] = {0};
		trap->Cvar_VariableStringBuffer("ui_char_model", ui_char_model, sizeof(ui_char_model) );
		Com_sprintf( modelPath, sizeof( modelPath ), "models/players/%s/model.glm", ui_char_model );
		return (ItemParse_asset_model_go( item, modelPath, &animRunLength ));
	}
#endif
	return (ItemParse_asset_model_go( item, token.string, &animRunLength ));
}

// asset_shader <string>
bool ItemParse_asset_shader( itemDef_t *item, int handle ) {
	pc_token_t token;
	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}
	item->asset = DC->registerShaderNoMip(token.string);
	return true;
}

// model_origin <number> <number> <number>
bool ItemParse_model_origin( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (PC_Float_Parse(handle, &modelPtr->origin[0])) {
		if (PC_Float_Parse(handle, &modelPtr->origin[1])) {
			if (PC_Float_Parse(handle, &modelPtr->origin[2])) {
				return true;
			}
		}
	}
	return false;
}

// model_fovx <number>
bool ItemParse_model_fovx( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!PC_Float_Parse(handle, &modelPtr->fov_x)) {
		return false;
	}
	return true;
}

// model_fovy <number>
bool ItemParse_model_fovy( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!PC_Float_Parse(handle, &modelPtr->fov_y)) {
		return false;
	}
	return true;
}

// model_rotation <integer>
bool ItemParse_model_rotation( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!PC_Int_Parse(handle, &modelPtr->rotationSpeed)) {
		return false;
	}
	return true;
}

// model_angle <integer>
bool ItemParse_model_angle( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!PC_Int_Parse(handle, &modelPtr->angle)) {
		return false;
	}
	return true;
}

// model_g2mins <number> <number> <number>
bool ItemParse_model_g2mins( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (PC_Float_Parse(handle, &modelPtr->g2mins[0])) {
		if (PC_Float_Parse(handle, &modelPtr->g2mins[1])) {
			if (PC_Float_Parse(handle, &modelPtr->g2mins[2])) {
				return true;
			}
		}
	}
	return false;
}

// model_g2maxs <number> <number> <number>
bool ItemParse_model_g2maxs( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (PC_Float_Parse(handle, &modelPtr->g2maxs[0])) {
		if (PC_Float_Parse(handle, &modelPtr->g2maxs[1])) {
			if (PC_Float_Parse(handle, &modelPtr->g2maxs[2])) {
				return true;
			}
		}
	}
	return false;
}

// model_g2scale <number> <number> <number>
bool ItemParse_model_g2scale( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (PC_Float_Parse(handle, &modelPtr->g2scale[0])) {
		if (PC_Float_Parse(handle, &modelPtr->g2scale[1])) {
			if (PC_Float_Parse(handle, &modelPtr->g2scale[2])) {
				return true;
			}
		}
	}
	return false;
}

bool ItemParse_model_g2skin( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	pc_token_t token;

	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}

	if (!token.string[0])
	{ //it was parsed correctly so still return true.
		return true;
	}

	modelPtr->g2skin = trap->R_RegisterSkin(token.string);

	return true;
}

// model_g2anim <number>
bool ItemParse_model_g2anim( itemDef_t *item, int handle ) {
	modelDef_t *modelPtr;
	pc_token_t token;
	int i = 0;

	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}

	if ( !token.string[0])
	{ //it was parsed correctly so still return true.
		return true;
	}

	while (i < MAX_ANIMATIONS)
	{
		if (!Q_stricmp(token.string, animTable[i].name))
		{ //found it
			modelPtr->g2anim = i;
			return true;
		}
		i++;
	}

	Com_Printf("Could not find '%s' in the anim table\n", token.string);
	return true;
}

// model_g2skin <string>
bool ItemParse_model_g2skin_go( itemDef_t *item, const char *skinName )
{
	modelDef_t *modelPtr;
	int defSkin;

	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!skinName || !skinName[0])
	{ //it was parsed correctly so still return true.
		modelPtr->g2skin = 0;
		trap->G2API_SetSkin(item->ghoul2, 0, 0, 0);

		return true;
	}

	// set skin
	if ( item->ghoul2 )
	{
		defSkin = trap->R_RegisterSkin(skinName);
		trap->G2API_SetSkin(item->ghoul2, 0, defSkin, defSkin);
	}

	return true;
}

// model_g2anim <number>
bool ItemParse_model_g2anim_go( itemDef_t *item, const char *animName )
{
	modelDef_t *modelPtr;
	int i = 0;

	Item_ValidateTypeData(item);
	modelPtr = item->typeData.model;

	if (!animName || !animName[0])
	{ //it was parsed correctly so still return true.
		return true;
	}

	while (i < MAX_ANIMATIONS)
	{
		if (!Q_stricmp(animName, animTable[i].name))
		{ //found it
			modelPtr->g2anim = animTable[i].id;
			return true;
		}
		i++;
	}

	Com_Printf("Could not find '%s' in the anim table\n", animName);
	return true;
}

// Get the cvar, get the values and stuff them in the rect structure.
bool ItemParse_rectcvar( itemDef_t *item, int handle )
{
	char	cvarBuf[1024];
	const char	*holdVal;
	char	*holdBuf;

	// get Cvar name
	pc_token_t token;
	if (!trap->PC_ReadToken(handle, &token))
	{
		return false;
	}

	// get cvar data
	DC->getCVarString(token.string, cvarBuf, sizeof(cvarBuf));

	holdBuf = cvarBuf;
	if (String_Parse(&holdBuf,&holdVal))
	{
		item->window.rectClient.x = atof(holdVal);
		if (String_Parse(&holdBuf,&holdVal))
		{
			item->window.rectClient.y = atof(holdVal);
			if (String_Parse(&holdBuf,&holdVal))
			{
				item->window.rectClient.w = atof(holdVal);
				if (String_Parse(&holdBuf,&holdVal))
				{
					item->window.rectClient.h = atof(holdVal);
					return true;
				}
			}
		}
	}

	// There may be no cvar built for this, and that's okay. . . I guess.
	return true;
}

// rect <rectangle>
bool ItemParse_rect( itemDef_t *item, int handle ) {
	if (!PC_Rect_Parse(handle, &item->window.rectClient)) {
		return false;
	}
	return true;
}

// flag <flag string>
bool ItemParse_flag( itemDef_t *item, int handle)
{
	int i;
	pc_token_t token;

	if (!trap->PC_ReadToken(handle, &token))
	{
		return false;
	}

	for ( i=0; itemFlags[i].string; i++ )
	{
		if (Q_stricmp(token.string,itemFlags[i].string)==0)
		{
			item->window.flags |= itemFlags[i].value;
			break;
		}
	}

	if (itemFlags[i].string == nullptr)
	{
		Com_Printf( S_COLOR_YELLOW "Unknown item style value '%s'\n", token.string );
	}

	return true;
}

// style <integer>
bool ItemParse_style( itemDef_t *item, int handle)
{
	if (!PC_Int_Parse(handle, &item->window.style))
	{
		Com_Printf(S_COLOR_YELLOW "Unknown item style value\n");
		return false;
	}

	return true;
}

// decoration
bool ItemParse_decoration( itemDef_t *item, int handle ) {
	item->window.flags |= WINDOW_DECORATION;
	return true;
}

// notselectable
bool ItemParse_notselectable( itemDef_t *item, int handle ) {
	listBoxDef_t *listPtr;
	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;
	if (item->type == ITEM_TYPE_LISTBOX && listPtr) {
		listPtr->notselectable = true;
	}
	return true;
}

// scrollhidden
bool ItemParse_scrollhidden( itemDef_t *item , int handle)
{
	listBoxDef_t *listPtr;
	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;

	if (item->type == ITEM_TYPE_LISTBOX && listPtr)
	{
		listPtr->scrollhidden = true;
	}
	return true;
}

// manually wrapped
bool ItemParse_wrapped( itemDef_t *item, int handle ) {
	item->window.flags |= WINDOW_WRAPPED;
	return true;
}

// auto wrapped
bool ItemParse_autowrapped( itemDef_t *item, int handle ) {
	item->window.flags |= WINDOW_AUTOWRAPPED;
	return true;
}

// horizontalscroll
bool ItemParse_horizontalscroll( itemDef_t *item, int handle ) {
	item->window.flags |= WINDOW_HORIZONTAL;
	return true;
}

// type <integer>
bool ItemParse_type( itemDef_t *item, int handle  )
{
	if (!PC_Int_Parse(handle, &item->type))
	{
		return false;
	}
	Item_ValidateTypeData(item);
	return true;
}

// elementwidth, used for listbox image elements
// uses textalignx for storage
bool ItemParse_elementwidth( itemDef_t *item, int handle ) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;
	if (!listPtr || !PC_Float_Parse(handle, &listPtr->elementWidth)) {
		return false;
	}
	return true;
}

// elementheight, used for listbox image elements
// uses textaligny for storage
bool ItemParse_elementheight( itemDef_t *item, int handle ) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;
	if (!listPtr || !PC_Float_Parse(handle, &listPtr->elementHeight)) {
		return false;
	}
	return true;
}

// feeder <float>
bool ItemParse_feeder( itemDef_t *item, int handle ) {
	if (!PC_Float_Parse(handle, &item->special)) {
		return false;
	}
	return true;
}

// elementtype, used to specify what type of elements a listbox contains
// uses textstyle for storage
bool ItemParse_elementtype( itemDef_t *item, int handle ) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;
	if (!listPtr || !PC_Int_Parse(handle, &listPtr->elementStyle)) {
		return false;
	}
	return true;
}

// columns sets a number of columns and an x pos and width per..
bool ItemParse_columns( itemDef_t *item, int handle ) {
	int i;
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;
	if (listPtr && PC_Int_Parse(handle, &listPtr->numColumns)) {
		if (listPtr->numColumns > MAX_LB_COLUMNS) {
			listPtr->numColumns = MAX_LB_COLUMNS;
		}
		for (i = 0; i < listPtr->numColumns; i++) {
			int pos, width, maxChars;

			if (PC_Int_Parse(handle, &pos) && PC_Int_Parse(handle, &width) && PC_Int_Parse(handle, &maxChars)) {
				listPtr->columnInfo[i].pos = pos;
				listPtr->columnInfo[i].width = width;
				listPtr->columnInfo[i].maxChars = maxChars;
			} else {
				return false;
			}
		}
	} else {
		return false;
	}
	return true;
}

bool ItemParse_border( itemDef_t *item, int handle ) {
	if (!PC_Int_Parse(handle, &item->window.border)) {
		return false;
	}
	return true;
}

bool ItemParse_bordersize( itemDef_t *item, int handle ) {
	if (!PC_Float_Parse(handle, &item->window.borderSize)) {
		return false;
	}
	return true;
}

bool ItemParse_visible( itemDef_t *item, int handle ) {
	int i;

	if (!PC_Int_Parse(handle, &i)) {
		return false;
	}
	if (i) {
		item->window.flags |= WINDOW_VISIBLE;
	}
	return true;
}

bool ItemParse_ownerdraw( itemDef_t *item, int handle ) {
	if (!PC_Int_Parse(handle, &item->window.ownerDraw)) {
		return false;
	}
	item->type = ITEM_TYPE_OWNERDRAW;
	return true;
}

bool ItemParse_align( itemDef_t *item, int handle ) {
	if (!PC_Int_Parse(handle, &item->alignment)) {
		return false;
	}
	return true;
}

// Sets item flag showing this is a character
bool ItemParse_isCharacter( itemDef_t *item, int handle  )
{
	int flag;

	if (PC_Int_Parse(handle, &flag))
	{
		if ( flag )
		{
			item->flags |= ITF_ISCHARACTER;
		}
		else
		{
			item->flags &= ~ITF_ISCHARACTER;
		}
		return true;
	}
	return false;
}

bool ItemParse_textalign( itemDef_t *item, int handle )
{
	if (!PC_Int_Parse(handle, &item->textalignment))
	{
		Com_Printf(S_COLOR_YELLOW "Unknown text alignment value\n");

		return false;
	}

	return true;
}

bool ItemParse_textalignx( itemDef_t *item, int handle ) {
	if (!PC_Float_Parse(handle, &item->textalignx)) {
		return false;
	}
	return true;
}

bool ItemParse_textaligny( itemDef_t *item, int handle ) {
	if (!PC_Float_Parse(handle, &item->textaligny)) {
		return false;
	}
	return true;
}

bool ItemParse_textscale( itemDef_t *item, int handle ) {
	if (!PC_Float_Parse(handle, &item->textscale)) {
		return false;
	}
	return true;
}

bool ItemParse_textstyle( itemDef_t *item, int handle ) {
	if (!PC_Int_Parse(handle, &item->textStyle)) {
		return false;
	}
	return true;
}

//JLFYESNO MPMOVED
bool ItemParse_invertyesno( itemDef_t *item, int handle)
{
	if (!PC_Int_Parse(handle, &item->invertYesNo))
	{
		return false;
	}
	return true;
}

// used for yes/no and multi
bool ItemParse_xoffset( itemDef_t *item, int handle)
{
	if (PC_Int_Parse(handle, &item->xoffset))
	{
		return false;
	}
	return true;
}

bool ItemParse_backcolor( itemDef_t *item, int handle ) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		item->window.backColor[i]  = f;
	}
	return true;
}

bool ItemParse_forecolor( itemDef_t *item, int handle ) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}

		if (f < 0)
		{	//special case for player color
			item->window.flags |= WINDOW_PLAYERCOLOR;
			return true;
		}

		item->window.foreColor[i]  = f;
		item->window.flags |= WINDOW_FORECOLORSET;
	}
	return true;
}

bool ItemParse_bordercolor( itemDef_t *item, int handle ) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		item->window.borderColor[i]  = f;
	}
	return true;
}

bool ItemParse_outlinecolor( itemDef_t *item, int handle ) {
	if (!PC_Color_Parse(handle, &item->window.outlineColor)){
		return false;
	}
	return true;
}

bool ItemParse_background( itemDef_t *item, int handle ) {
	pc_token_t token;

	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}
	item->window.background = DC->registerShaderNoMip(token.string);
	return true;
}

bool ItemParse_cinematic( itemDef_t *item, int handle ) {
	if (!PC_String_Parse(handle, &item->window.cinematicName)) {
		return false;
	}
	return true;
}

bool ItemParse_doubleClick( itemDef_t *item, int handle ) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = item->typeData.listbox;

	if (!listPtr || !PC_Script_Parse(handle, &listPtr->doubleClick)) {
		return false;
	}
	return true;
}

bool ItemParse_onFocus( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->onFocus)) {
		return false;
	}
	return true;
}

bool ItemParse_leaveFocus( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->leaveFocus)) {
		return false;
	}
	return true;
}

bool ItemParse_mouseEnter( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->mouseEnter)) {
		return false;
	}
	return true;
}

bool ItemParse_mouseExit( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->mouseExit)) {
		return false;
	}
	return true;
}

bool ItemParse_mouseEnterText( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->mouseEnterText)) {
		return false;
	}
	return true;
}

bool ItemParse_mouseExitText( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->mouseExitText)) {
		return false;
	}
	return true;
}

bool ItemParse_action( itemDef_t *item, int handle ) {
	if (!PC_Script_Parse(handle, &item->action)) {
		return false;
	}
	return true;
}

bool ItemParse_special( itemDef_t *item, int handle ) {
	if (!PC_Float_Parse(handle, &item->special)) {
		return false;
	}
	return true;
}

bool ItemParse_cvarTest( itemDef_t *item, int handle ) {
	if (!PC_String_Parse(handle, &item->cvarTest)) {
		return false;
	}
	return true;
}

bool ItemParse_cvar( itemDef_t *item, int handle )
{
	Item_ValidateTypeData(item);
	if (!PC_String_Parse(handle, &item->cvar))
	{
		return false;
	}

	switch ( item->type )
	{
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
		case ITEM_TYPE_YESNO:
		case ITEM_TYPE_BIND:
		case ITEM_TYPE_SLIDER:
		case ITEM_TYPE_TEXT:
		{
			if ( item->typeData.edit )
			{
				item->typeData.edit->minVal = -1;
				item->typeData.edit->maxVal = -1;
				item->typeData.edit->defVal = -1;
			}
			break;
		}
	}
	return true;
}

bool ItemParse_font( itemDef_t *item, int handle ) {
	Item_ValidateTypeData( item );
	int tmp;
	if ( !PC_Int_Parse( handle, &tmp ) ) {
		return false;
	}
	item->font = static_cast<JKFont>( tmp );
	return true;
}

bool ItemParse_maxChars( itemDef_t *item, int handle ) {
	editFieldDef_t *editPtr;

	Item_ValidateTypeData(item);
	editPtr = (editFieldDef_t*)item->typeData.edit;

	if (!editPtr || !PC_Int_Parse(handle, &editPtr->maxChars)) {
		return false;
	}
	return true;
}

bool ItemParse_maxPaintChars( itemDef_t *item, int handle ) {
	editFieldDef_t *editPtr;

	Item_ValidateTypeData(item);
	editPtr = (editFieldDef_t*)item->typeData.edit;

	if (!editPtr || !PC_Int_Parse(handle, &editPtr->maxPaintChars)) {
		return false;
	}
	return true;
}

bool ItemParse_maxLineChars( itemDef_t *item, int handle ) {
	textScrollDef_t *scrollPtr;

	Item_ValidateTypeData(item);
	scrollPtr = item->typeData.textscroll;

	if (!scrollPtr || !PC_Int_Parse(handle, &scrollPtr->maxLineChars)) {
		return false;
	}
	return true;
}

bool ItemParse_lineHeight( itemDef_t *item, int handle ) {
	textScrollDef_t *scrollPtr;
	int height;

	Item_ValidateTypeData(item);
	scrollPtr = item->typeData.textscroll;

	if (!scrollPtr || !PC_Int_Parse(handle, &height)) {
		return false;
	}
	scrollPtr->lineHeight = height;
	return true;
}

bool ItemParse_cvarFloat( itemDef_t *item, int handle ) {
	editFieldDef_t *editPtr;

	Item_ValidateTypeData(item);
	editPtr = item->typeData.edit;

	if (!editPtr)
		return false;

	if (PC_String_Parse(handle, &item->cvar) &&
		PC_Float_Parse(handle, &editPtr->defVal) &&
		PC_Float_Parse(handle, &editPtr->minVal) &&
		PC_Float_Parse(handle, &editPtr->maxVal)) {
		return true;
	}
	return false;
}

#ifdef BUILD_UI
char currLanguage[32][128];
static const char languageString[32] = "@MENUS_MYLANGUAGE";
#endif

bool ItemParse_cvarStrList( itemDef_t *item, int handle ) {
	pc_token_t token;
	multiDef_t *multiPtr;
	int pass;

	Item_ValidateTypeData(item);
	multiPtr = item->typeData.multi;

	if (!multiPtr)
		return false;

	multiPtr->count = 0;
	multiPtr->strDef = true;

	if (!trap->PC_ReadToken(handle, &token))
	{
		return false;
	}

	if (!Q_stricmp(token.string,"feeder") && item->special == FEEDER_PLAYER_SPECIES)
	{
#ifndef BUILD_CGAME
		for (; multiPtr->count < uiInfo.playerSpeciesCount; multiPtr->count++)
		{
			multiPtr->cvarList[multiPtr->count] = String_Alloc(Q_strupr(va("@MENUS_%s",uiInfo.playerSpecies[multiPtr->count].Name )));	//look up translation
			multiPtr->cvarStr[multiPtr->count] = uiInfo.playerSpecies[multiPtr->count].Name;	//value
		}
#endif
		return true;
	}
	// languages
	if (!Q_stricmp(token.string,"feeder") && item->special == FEEDER_LANGUAGES)
	{
#ifdef BUILD_UI
		for (; multiPtr->count < uiInfo.languageCount; multiPtr->count++)
		{
			// The displayed text
			trap->SE_GetLanguageName( (const int) multiPtr->count,(char *) currLanguage[multiPtr->count]  );	// eg "English"
			multiPtr->cvarList[multiPtr->count] = languageString;
			// The cvar value that goes into se_language
			trap->SE_GetLanguageName( (const int) multiPtr->count,(char *) currLanguage[multiPtr->count] );
			multiPtr->cvarStr[multiPtr->count] = currLanguage[multiPtr->count];
		}
#endif
		return true;
	}

	if (*token.string != '{') {
		return false;
	}

	pass = 0;
	while ( 1 ) {
		char* psString;

		if (!PC_String_Parse(handle, (const char **)&psString))
		{
			PC_SourceError(handle, "end of file inside menu item");
			return false;
		}

		//a normal StringAlloc ptr
		if (psString)
		{
			if (*psString == '}') {
				return true;
			}

			if (*psString == ',' || *psString == ';') {
				continue;
			}
		}

		if (pass == 0) {
			multiPtr->cvarList[multiPtr->count] = psString;
			pass = 1;
		} else {
			multiPtr->cvarStr[multiPtr->count] = psString;
			pass = 0;
			multiPtr->count++;
			if (multiPtr->count >= MAX_MULTI_CVARS) {
				return false;
			}
		}

	}
	return false;
}

bool ItemParse_cvarFloatList( itemDef_t *item, int handle )
{
	pc_token_t token;
	multiDef_t *multiPtr;

	Item_ValidateTypeData(item);
	multiPtr = item->typeData.multi;

	if (!multiPtr)
		return false;

	multiPtr->count = 0;
	multiPtr->strDef = false;

	if (!trap->PC_ReadToken(handle, &token))
	{
		return false;
	}

	if (*token.string != '{')
	{
		return false;
	}

	while ( 1 )
	{
		char* string;

		if ( !PC_String_Parse ( handle, (const char **)&string ) )
		{
			PC_SourceError(handle, "end of file inside menu item");
			return false;
		}

		//a normal StringAlloc ptr
		if (string)
		{
			if (*string == '}')
			{
				return true;
			}

			if (*string == ',' || *string == ';')
			{
				continue;
			}
		}

		multiPtr->cvarList[multiPtr->count] = string;
		if (!PC_Float_Parse(handle, &multiPtr->cvarValue[multiPtr->count]))
		{
			return false;
		}

		multiPtr->count++;
		if (multiPtr->count >= MAX_MULTI_CVARS)
		{
			return false;
		}

	}
	return false;
}

bool ItemParse_addColorRange( itemDef_t *item, int handle ) {
	colorRangeDef_t color;

	if (PC_Float_Parse(handle, &color.low) &&
		PC_Float_Parse(handle, &color.high) &&
		PC_Color_Parse(handle, &color.color) ) {
		if (item->numColors < MAX_COLOR_RANGES) {
			memcpy(&item->colorRanges[item->numColors], &color, sizeof(color));
			item->numColors++;
		}
		return true;
	}
	return false;
}

bool ItemParse_ownerdrawFlag( itemDef_t *item, int handle ) {
	int i;
	if (!PC_Int_Parse(handle, &i)) {
		return false;
	}
	item->window.ownerDrawFlags |= i;
	return true;
}

bool ItemParse_enableCvar( itemDef_t *item, int handle ) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_ENABLE;
		return true;
	}
	return false;
}

bool ItemParse_disableCvar( itemDef_t *item, int handle ) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_DISABLE;
		return true;
	}
	return false;
}

bool ItemParse_showCvar( itemDef_t *item, int handle ) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_SHOW;
		return true;
	}
	return false;
}

bool ItemParse_hideCvar( itemDef_t *item, int handle ) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_HIDE;
		return true;
	}
	return false;
}

bool ItemParse_Appearance_slot( itemDef_t *item, int handle )
{
	if (!PC_Int_Parse(handle, &item->appearanceSlot))
	{
		return false;
	}
	return true;
}

bool ItemParse_isSaber( itemDef_t *item, int handle  )
{
#ifndef BUILD_CGAME

	int	i;

	if (PC_Int_Parse(handle, &i))
	{
		if ( i )
		{
			item->flags |= ITF_ISSABER;
			UI_CacheSaberGlowGraphics();
			if ( !ui_saber_parms_parsed )
			{
				UI_SaberLoadParms();
			}
		}
		else
		{
			item->flags &= ~ITF_ISSABER;
		}

		return true;
	}
#endif
	return false;
}

bool ItemParse_isSaber2( itemDef_t *item, int handle  )
{
#ifndef BUILD_CGAME
	int	i;
	if (PC_Int_Parse(handle, &i))
	{
		if ( i )
		{
			item->flags |= ITF_ISSABER2;
			UI_CacheSaberGlowGraphics();
			if ( !ui_saber_parms_parsed )
			{
				UI_SaberLoadParms();
			}
		}
		else
		{
			item->flags &= ~ITF_ISSABER2;
		}

		return true;
	}
#endif

	return false;
}

keywordHash_t itemParseKeywords[] = {
	{ "action",           ItemParse_action,           nullptr },
	{ "addColorRange",    ItemParse_addColorRange,    nullptr },
	{ "align",            ItemParse_align,            nullptr },
	{ "appearance_slot",  ItemParse_Appearance_slot,  nullptr },
	{ "asset_model",      ItemParse_asset_model,      nullptr },
	{ "asset_shader",     ItemParse_asset_shader,     nullptr },
	{ "autowrapped",      ItemParse_autowrapped,      nullptr },
	{ "backcolor",        ItemParse_backcolor,        nullptr },
	{ "background",       ItemParse_background,       nullptr },
	{ "border",           ItemParse_border,           nullptr },
	{ "bordercolor",      ItemParse_bordercolor,      nullptr },
	{ "bordersize",       ItemParse_bordersize,       nullptr },
	{ "cinematic",        ItemParse_cinematic,        nullptr },
	{ "columns",          ItemParse_columns,          nullptr },
	{ "cvar",             ItemParse_cvar,             nullptr },
	{ "cvarFloat",        ItemParse_cvarFloat,        nullptr },
	{ "cvarFloatList",    ItemParse_cvarFloatList,    nullptr },
	{ "cvarStrList",      ItemParse_cvarStrList,      nullptr },
	{ "cvarTest",         ItemParse_cvarTest,         nullptr },
	{ "decoration",       ItemParse_decoration,       nullptr },
	{ "desctext",         ItemParse_descText,         nullptr },
	{ "disableCvar",      ItemParse_disableCvar,      nullptr },
	{ "doubleclick",      ItemParse_doubleClick,      nullptr },
	{ "elementheight",    ItemParse_elementheight,    nullptr },
	{ "elementtype",      ItemParse_elementtype,      nullptr },
	{ "elementwidth",     ItemParse_elementwidth,     nullptr },
	{ "enableCvar",       ItemParse_enableCvar,       nullptr },
	{ "feeder",           ItemParse_feeder,           nullptr },
	{ "flag",             ItemParse_flag,             nullptr },
	{ "focusSound",       ItemParse_focusSound,       nullptr },
	{ "font",             ItemParse_font,             nullptr },
	{ "forecolor",        ItemParse_forecolor,        nullptr },
	{ "group",            ItemParse_group,            nullptr },
	{ "hideCvar",         ItemParse_hideCvar,         nullptr },
	{ "horizontalscroll", ItemParse_horizontalscroll, nullptr },
	{ "invertyesno",      ItemParse_invertyesno,      nullptr },
	{ "isCharacter",      ItemParse_isCharacter,      nullptr },
	{ "isSaber",          ItemParse_isSaber,          nullptr },
	{ "isSaber2",         ItemParse_isSaber2,         nullptr },
	{ "leaveFocus",       ItemParse_leaveFocus,       nullptr },
	{ "lineHeight",       ItemParse_lineHeight,       nullptr },
	{ "maxChars",         ItemParse_maxChars,         nullptr },
	{ "maxLineChars",     ItemParse_maxLineChars,     nullptr },
	{ "maxPaintChars",    ItemParse_maxPaintChars,    nullptr },
	{ "model_angle",      ItemParse_model_angle,      nullptr },
	{ "model_fovx",       ItemParse_model_fovx,       nullptr },
	{ "model_fovy",       ItemParse_model_fovy,       nullptr },
	{ "model_g2anim",     ItemParse_model_g2anim,     nullptr },
	{ "model_g2maxs",     ItemParse_model_g2maxs,     nullptr },
	{ "model_g2mins",     ItemParse_model_g2mins,     nullptr },
	{ "model_g2scale",    ItemParse_model_g2scale,    nullptr },
	{ "model_g2skin",     ItemParse_model_g2skin,     nullptr },
	{ "model_origin",     ItemParse_model_origin,     nullptr },
	{ "model_rotation",   ItemParse_model_rotation,   nullptr },
	{ "mouseEnter",       ItemParse_mouseEnter,       nullptr },
	{ "mouseEnterText",   ItemParse_mouseEnterText,   nullptr },
	{ "mouseExit",        ItemParse_mouseExit,        nullptr },
	{ "mouseExitText",    ItemParse_mouseExitText,    nullptr },
	{ "name",             ItemParse_name,             nullptr },
	{ "notselectable",    ItemParse_notselectable,    nullptr },
	{ "onFocus",          ItemParse_onFocus,          nullptr },
	{ "outlinecolor",     ItemParse_outlinecolor,     nullptr },
	{ "ownerdraw",        ItemParse_ownerdraw,        nullptr },
	{ "ownerdrawFlag",    ItemParse_ownerdrawFlag,    nullptr },
	{ "rect",             ItemParse_rect,             nullptr },
	{ "rectcvar",         ItemParse_rectcvar,         nullptr },
	{ "scrollhidden",     ItemParse_scrollhidden,     nullptr },
	{ "showCvar",         ItemParse_showCvar,         nullptr },
	{ "special",          ItemParse_special,          nullptr },
	{ "style",            ItemParse_style,            nullptr },
	{ "text",             ItemParse_text,             nullptr },
	{ "text2",            ItemParse_text2,            nullptr },
	{ "text2alignx",      ItemParse_text2alignx,      nullptr },
	{ "text2aligny",      ItemParse_text2aligny,      nullptr },
	{ "textalign",        ItemParse_textalign,        nullptr },
	{ "textalignx",       ItemParse_textalignx,       nullptr },
	{ "textaligny",       ItemParse_textaligny,       nullptr },
	{ "textscale",        ItemParse_textscale,        nullptr },
	{ "textstyle",        ItemParse_textstyle,        nullptr },
	{ "type",             ItemParse_type,             nullptr },
	{ "visible",          ItemParse_visible,          nullptr },
	{ "wrapped",          ItemParse_wrapped,          nullptr },
	{ "xoffset",          ItemParse_xoffset,          nullptr },
	{ 0,                  0,                          0 }
};

keywordHash_t *itemParseKeywordHash[KEYWORDHASH_SIZE];

static void Item_SetupKeywordHash( void ) {
	int i;

	memset(itemParseKeywordHash, 0, sizeof(itemParseKeywordHash));
	for (i = 0; itemParseKeywords[i].keyword; i++) {
		KeywordHash_Add(itemParseKeywordHash, &itemParseKeywords[i]);
	}
}

// Hacks to fix issues with Team Arena menu scripts
static void Item_ApplyHacks( itemDef_t *item ) {
#if !defined(_WIN32) || ( defined(_WIN32) && defined(idx64) )
	if ( item->type == ITEM_TYPE_MULTI && item->cvar && !Q_stricmp( item->cvar, "s_useOpenAL" ) ) {
		if( item->parent )
		{
			menuDef_t *parent = (menuDef_t *)item->parent;
			VectorSet4( parent->disableColor, 0.5f, 0.5f, 0.5f, 1.0f );
			item->disabled = true;
			// Just in case it had focus
			item->window.flags &= ~WINDOW_MOUSEOVER;
			Com_Printf( "Disabling eax field because current platform does not support EAX.\n");
		}
	}

	if ( item->type == ITEM_TYPE_TEXT && item->window.name && !Q_stricmp( item->window.name, "eax_icon") && item->cvarTest && !Q_stricmp( item->cvarTest, "s_useOpenAL" ) && item->enableCvar && (item->cvarFlags & CVAR_HIDE) ) {
		if( item->parent )
		{
			menuDef_t *parent = (menuDef_t *)item->parent;
			VectorSet4( parent->disableColor, 0.5f, 0.5f, 0.5f, 1.0f );
			item->disabled = item->disabledHidden = true;
			// Just in case it had focus
			item->window.flags &= ~WINDOW_MOUSEOVER;
			Com_Printf( "Hiding eax_icon object because current platform does not support EAX.\n");
		}
	}
#endif

	// Fix length of favorite address in createfavorite.menu
	if ( item->type == ITEM_TYPE_EDITFIELD && item->cvar && !Q_stricmp( item->cvar, "ui_favoriteAddress" ) ) {
		editFieldDef_t *editField = item->typeData.edit;

		// enough to hold an IPv6 address plus null
		if ( editField->maxChars < 48 ) {
			Com_Printf( "Extended create favorite address edit field length to hold an IPv6 address\n" );
			editField->maxChars = 48;
		}
	}

	if ( item->type == ITEM_TYPE_EDITFIELD && item->cvar && ( !Q_stricmp( item->cvar, "ui_Name" ) || !Q_stricmp( item->cvar, "ui_findplayer" ) ) ) {
		editFieldDef_t *editField = item->typeData.edit;

		// enough to hold a full player name
		if ( editField->maxChars < MAX_NAME_LENGTH ) {
			if ( editField->maxPaintChars > editField->maxChars ) {
				editField->maxPaintChars = editField->maxChars;
			}

			Com_Printf( "Extended player name field using cvar %s to %d characters\n", item->cvar, MAX_NAME_LENGTH );
			editField->maxChars = MAX_NAME_LENGTH;
		}
	}

	if ( item->type == ITEM_TYPE_MULTI && item->window.name && !Q_stricmp( item->window.name, "sound_quality") ) {
		multiDef_t *multiPtr = item->typeData.multi;
		int i;
		bool found = false;
		for( i = 0; i < multiPtr->count; i++ )
		{
			if ( multiPtr->cvarValue[i] == 44 )
			{
				found = true;
				break;
			}
		}
		if ( !found && multiPtr->count < MAX_MULTI_CVARS )
		{
			multiPtr->cvarList[multiPtr->count] = String_Alloc("@MENUS_VERY_HIGH");
			multiPtr->cvarValue[multiPtr->count] = 44;
			multiPtr->count++;
			Com_Printf( "Extended sound quality field to contain very high setting.\n");
		}
	}
}

bool Item_Parse(int handle, itemDef_t *item) {
	pc_token_t token;
	keywordHash_t *key;

	if (!trap->PC_ReadToken(handle, &token))
		return false;

	if (*token.string != '{') {
		return false;
	}
	while ( 1 ) {
		if (!trap->PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu item");
			return false;
		}

		if (*token.string == '}') {
			return true;
		}

		key = KeywordHash_Find(itemParseKeywordHash, token.string);
		if (!key) {
			PC_SourceError(handle, "unknown menu item keyword %s", token.string);
			continue;
		}
		if ( !key->func(item, handle) ) {
			PC_SourceError(handle, "couldn't parse menu item keyword %s", token.string);
			return false;
		}
	}
	return false;
}

static void Item_TextScroll_BuildLines ( itemDef_t* item )
{
	char str[2048];

#if 1

	// new asian-aware line breaker...  (pasted from elsewhere late @ night, hence aliasing-vars ;-)

	textScrollDef_t* scrollPtr = item->typeData.textscroll;
	const char *psText = item->text;	// for copy/paste ease
	int iBoxWidth = item->window.rect.w - SCROLLBAR_SIZE - 10;

	// this could probably be simplified now, but it was converted from something else I didn't originally write,
	//	and it works anyway so wtf...

	const char *psCurrentTextReadPos;
	const char *psReadPosAtLineStart;
	const char *psBestLineBreakSrcPos;
	const char *psLastGood_s;	// needed if we get a full screen of chars with no punctuation or space (see usage notes)
	bool bIsTrailingPunctuation;
	unsigned int uiLetter;

	if (*psText == '@')	// string reference
	{
		trap->SE_GetStringTextString( &psText[1], str, sizeof(str));
		psText = str;
	}

	psCurrentTextReadPos = psText;
	psReadPosAtLineStart = psCurrentTextReadPos;
	psBestLineBreakSrcPos = psCurrentTextReadPos;

	scrollPtr->iLineCount = 0;
	memset((char*)scrollPtr->pLines,0,sizeof(scrollPtr->pLines));
	Text text{ item->font, item->textscale };

	while (*psCurrentTextReadPos && (scrollPtr->iLineCount < MAX_TEXTSCROLL_LINES) )
	{
		char sLineForDisplay[2048];	// ott

		// construct a line...

		psCurrentTextReadPos = psReadPosAtLineStart;
		sLineForDisplay[0] = '\0';
		while ( *psCurrentTextReadPos )
		{
			int iAdvanceCount;
			psLastGood_s = psCurrentTextReadPos;

			// read letter...

			uiLetter = trap->R_AnyLanguage_ReadCharFromString(psCurrentTextReadPos, &iAdvanceCount, &bIsTrailingPunctuation);
			psCurrentTextReadPos += iAdvanceCount;

			// concat onto string so far...

			if (uiLetter == 32 && sLineForDisplay[0] == '\0')
			{
				psReadPosAtLineStart++;
				continue;	// unless it's a space at the start of a line, in which case ignore it.
			}

			if (uiLetter > 255)
			{
				Q_strcat(sLineForDisplay, sizeof(sLineForDisplay),va("%c%c",uiLetter >> 8, uiLetter & 0xFF));
			}
			else
			{
				Q_strcat(sLineForDisplay, sizeof(sLineForDisplay),va("%c",uiLetter & 0xFF));
			}

			if (uiLetter == '\n')
			{
				// explicit new line...

				sLineForDisplay[ strlen(sLineForDisplay)-1 ] = '\0';	// kill the CR
				psReadPosAtLineStart = psCurrentTextReadPos;
				psBestLineBreakSrcPos = psCurrentTextReadPos;

				// hack it to fit in with this code...

				scrollPtr->pLines[ scrollPtr->iLineCount ] = String_Alloc ( sLineForDisplay );
				break;	// print this line
			}
			else if ( Text_Width( text, sLineForDisplay ) >= iBoxWidth )
			{
				// reached screen edge, so cap off string at bytepos after last good position...

				if (uiLetter > 255 && bIsTrailingPunctuation && !trap->R_Language_UsesSpaces())
				{
					// Special case, don't consider line breaking if you're on an asian punctuation char of
					//	a language that doesn't use spaces...

				}
				else
				{
					if (psBestLineBreakSrcPos == psReadPosAtLineStart)
					{
						//  aarrrggh!!!!!   we'll only get here is someone has fed in a (probably) garbage string,
						//		since it doesn't have a single space or punctuation mark right the way across one line
						//		of the screen.  So far, this has only happened in testing when I hardwired a taiwanese
						//		string into this function while the game was running in english (which should NEVER happen
						//		normally).  On the other hand I suppose it's entirely possible that some taiwanese string
						//		might have no punctuation at all, so...

						psBestLineBreakSrcPos = psLastGood_s;	// force a break after last good letter
					}

					sLineForDisplay[ psBestLineBreakSrcPos - psReadPosAtLineStart ] = '\0';
					psReadPosAtLineStart = psCurrentTextReadPos = psBestLineBreakSrcPos;

					// hack it to fit in with this code...

					scrollPtr->pLines[ scrollPtr->iLineCount ] = String_Alloc( sLineForDisplay );
					break;	// print this line
				}
			}

			// record last-good linebreak pos...  (ie if we've just concat'd a punctuation point (western or asian) or space)

			if (bIsTrailingPunctuation || uiLetter == ' ' || (uiLetter > 255 && !trap->R_Language_UsesSpaces()))
			{
				psBestLineBreakSrcPos = psCurrentTextReadPos;
			}
		}

		/// arrgghh, this is gettng horrible now...

		if (scrollPtr->pLines[ scrollPtr->iLineCount ] == nullptr && strlen(sLineForDisplay))
		{
			// then this is the last line and we've just run out of text, no CR, no overflow etc...

			scrollPtr->pLines[ scrollPtr->iLineCount ] = String_Alloc( sLineForDisplay );
		}

		scrollPtr->iLineCount++;
	}

#else
	// old version...

	int				 width;
	char*			 lineStart;
	char*			 lineEnd;
	float			 w;
	float			 cw;

	textScrollDef_t* scrollPtr = item->typeData.textscroll;

	scrollPtr->iLineCount = 0;
	width = scrollPtr->maxLineChars;
	lineStart = (char*)item->text;
	lineEnd   = lineStart;
	w		  = 0;

	// Keep going as long as there are more lines
	while ( scrollPtr->iLineCount < MAX_TEXTSCROLL_LINES )
	{
		// End of the road
		if ( *lineEnd == '\0')
		{
			if ( lineStart < lineEnd )
			{
				scrollPtr->pLines[ scrollPtr->iLineCount++ ] = lineStart;
			}

			break;
		}

		// Force a line end if its a '\n'
		else if ( *lineEnd == '\n' )
		{
			*lineEnd = '\0';
			scrollPtr->pLines[ scrollPtr->iLineCount++ ] = lineStart;
			lineStart = lineEnd + 1;
			lineEnd   = lineStart;
			w = 0;
			continue;
		}

		// Get the current character width
		cw = Text_Width( text, va("%c", *lineEnd), item->textscale, item->font );

		// Past the end of the boundary?
		if ( w + cw > (item->window.rect.w - SCROLLBAR_SIZE - 10) )
		{
			// Past the end so backtrack to the word boundary
			while ( *lineEnd != ' ' && *lineEnd != '\t' && lineEnd > lineStart )
			{
				lineEnd--;
			}

			*lineEnd = '\0';
			scrollPtr->pLines[ scrollPtr->iLineCount++ ] = lineStart;

			// Skip any whitespaces
			lineEnd++;
			while ( (*lineEnd == ' ' || *lineEnd == '\t') && *lineEnd )
			{
				lineEnd++;
			}

			lineStart = lineEnd;
			w = 0;
		}
		else
		{
			w += cw;
			lineEnd++;
		}
	}
#endif
}

// Item_InitControls
// init's special control types
void Item_InitControls(itemDef_t *item)
{
	if (item == nullptr)
	{
		return;
	}

	if ( item->type == ITEM_TYPE_LISTBOX )
	{
		listBoxDef_t *listPtr = item->typeData.listbox;
		item->cursorPos = 0;
		if (listPtr)
		{
			listPtr->cursorPos = 0;
			listPtr->startPos = 0;
			listPtr->endPos = 0;
			listPtr->cursorPos = 0;
		}
	}
}

// Menu Keyword Parse functions

bool MenuParse_name( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_String_Parse(handle, &menu->window.name)) {
		return false;
	}
	if (Q_stricmp(menu->window.name, "main") == 0) {
		// default main as having focus
		//menu->window.flags |= WINDOW_HASFOCUS;
	}
	return true;
}

bool MenuParse_fullscreen( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	byteAlias_t fullscreen;
	if (!PC_Int_Parse(handle, &fullscreen.i)) {
		return false;
	}
	menu->fullScreen = !!fullscreen.i;
	return true;
}

bool MenuParse_rect( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Rect_Parse(handle, &menu->window.rect)) {
		return false;
	}
	return true;
}

bool MenuParse_style( itemDef_t *item, int handle)
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &menu->window.style))
	{
		Com_Printf(S_COLOR_YELLOW "Unknown menu style value\n");
		return false;
	}

	return true;
}

bool MenuParse_visible( itemDef_t *item, int handle ) {
	int i;
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &i)) {
		return false;
	}
	if (i) {
		menu->window.flags |= WINDOW_VISIBLE;
	}
	return true;
}

bool MenuParse_onOpen( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Script_Parse(handle, &menu->onOpen)) {
		return false;
	}
	return true;
}

bool MenuParse_onClose( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Script_Parse(handle, &menu->onClose)) {
		return false;
	}
	return true;
}

//JLFACCEPT MPMOVED
bool MenuParse_onAccept( itemDef_t *item, int handle )
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Script_Parse(handle, &menu->onAccept))
	{
		return false;
	}
	return true;
}

bool MenuParse_onESC( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Script_Parse(handle, &menu->onESC)) {
		return false;
	}
	return true;
}

bool MenuParse_border( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Int_Parse(handle, &menu->window.border)) {
		return false;
	}
	return true;
}

bool MenuParse_borderSize( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Float_Parse(handle, &menu->window.borderSize)) {
		return false;
	}
	return true;
}

bool MenuParse_backcolor( itemDef_t *item, int handle ) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*)item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		menu->window.backColor[i]  = f;
	}
	return true;
}

bool MenuParse_descAlignment( itemDef_t *item, int handle )
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &menu->descAlignment))
	{
		Com_Printf(S_COLOR_YELLOW "Unknown desc alignment value\n");
		return false;
	}

	return true;
}

bool MenuParse_descX( itemDef_t *item, int handle )
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &menu->descX))
	{
		return false;
	}
	return true;
}

bool MenuParse_descY( itemDef_t *item, int handle )
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &menu->descY))
	{
		return false;
	}
	return true;
}

bool MenuParse_descScale( itemDef_t *item, int handle)
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Float_Parse(handle, &menu->descScale))
	{
		return false;
	}
	return true;
}

bool MenuParse_descColor( itemDef_t *item, int handle)
{
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*)item;

	for (i = 0; i < 4; i++)
	{
		if (!PC_Float_Parse(handle, &f))
		{
			return false;
		}
		menu->descColor[i]  = f;
	}
	return true;
}

bool MenuParse_forecolor( itemDef_t *item, int handle ) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*)item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		if (f < 0)
		{	//special case for player color
			menu->window.flags |= WINDOW_PLAYERCOLOR;
			return true;
		}
		menu->window.foreColor[i]  = f;
		menu->window.flags |= WINDOW_FORECOLORSET;
	}
	return true;
}

bool MenuParse_bordercolor( itemDef_t *item, int handle ) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*)item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		menu->window.borderColor[i]  = f;
	}
	return true;
}

bool MenuParse_focuscolor( itemDef_t *item, int handle ) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*)item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		menu->focusColor[i]  = f;
	}
	return true;
}

bool MenuParse_disablecolor( itemDef_t *item, int handle ) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*)item;
	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return false;
		}
		menu->disableColor[i]  = f;
	}
	return true;
}

bool MenuParse_outlinecolor( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (!PC_Color_Parse(handle, &menu->window.outlineColor)){
		return false;
	}
	return true;
}

bool MenuParse_background( itemDef_t *item, int handle ) {
	pc_token_t token;
	menuDef_t *menu = (menuDef_t*)item;

	if (!trap->PC_ReadToken(handle, &token)) {
		return false;
	}
	menu->window.background = DC->registerShaderNoMip(token.string);
	return true;
}

bool MenuParse_cinematic( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_String_Parse(handle, &menu->window.cinematicName)) {
		return false;
	}
	return true;
}

bool MenuParse_ownerdrawFlag( itemDef_t *item, int handle ) {
	int i;
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &i)) {
		return false;
	}
	menu->window.ownerDrawFlags |= i;
	return true;
}

bool MenuParse_ownerdraw( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &menu->window.ownerDraw)) {
		return false;
	}
	return true;
}

// decoration
bool MenuParse_popup( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	menu->window.flags |= WINDOW_POPUP;
	return true;
}

bool MenuParse_outOfBounds( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	menu->window.flags |= WINDOW_OOB_CLICK;
	return true;
}

bool MenuParse_soundLoop( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_String_Parse(handle, &menu->soundName)) {
		return false;
	}
	return true;
}

bool MenuParse_fadeClamp( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Float_Parse(handle, &menu->fadeClamp)) {
		return false;
	}
	return true;
}

bool MenuParse_fadeAmount( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Float_Parse(handle, &menu->fadeAmount)) {
		return false;
	}
	return true;
}

bool MenuParse_fadeCycle( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Int_Parse(handle, &menu->fadeCycle)) {
		return false;
	}
	return true;
}

bool MenuParse_itemDef( itemDef_t *item, int handle ) {
	menuDef_t *menu = (menuDef_t*)item;
	if (menu->itemCount < MAX_MENUITEMS) {
		itemDef_t *newItem = menu->items[menu->itemCount] = (itemDef_t *) UI_Alloc(sizeof(itemDef_t));
		Item_Init(newItem);
		if (!Item_Parse(handle, newItem))
		{
			return false;
		}
		Item_InitControls( newItem );
		newItem->parent = menu->items[menu->itemCount]->parent = menu;
		menu->itemCount++;
		Item_ApplyHacks( newItem );
	}
	return true;
}

bool MenuParse_appearanceIncrement( itemDef_t *item, int handle )
{
	menuDef_t *menu = (menuDef_t*)item;

	if (!PC_Float_Parse(handle, &menu->appearanceIncrement))
	{
		return false;
	}
	return true;
}

keywordHash_t menuParseKeywords[] = {
	{ "appearanceIncrement", MenuParse_appearanceIncrement, nullptr },
	{ "backcolor",           MenuParse_backcolor,           nullptr },
	{ "background",          MenuParse_background,          nullptr },
	{ "border",              MenuParse_border,              nullptr },
	{ "bordercolor",         MenuParse_bordercolor,         nullptr },
	{ "borderSize",          MenuParse_borderSize,          nullptr },
	{ "cinematic",           MenuParse_cinematic,           nullptr },
	{ "descAlignment",       MenuParse_descAlignment,       nullptr },
	{ "desccolor",           MenuParse_descColor,           nullptr },
	{ "descX",               MenuParse_descX,               nullptr },
	{ "descY",               MenuParse_descY,               nullptr },
	{ "descScale",           MenuParse_descScale,           nullptr },
	{ "disablecolor",        MenuParse_disablecolor,        nullptr },
	{ "fadeAmount",          MenuParse_fadeAmount,          nullptr },
	{ "fadeClamp",           MenuParse_fadeClamp,           nullptr },
	{ "fadeCycle",           MenuParse_fadeCycle,           nullptr },
	{ "focuscolor",          MenuParse_focuscolor,          nullptr },
	{ "forecolor",           MenuParse_forecolor,           nullptr },
	{ "fullscreen",          MenuParse_fullscreen,          nullptr },
	{ "itemDef",             MenuParse_itemDef,             nullptr },
	{ "name",                MenuParse_name,                nullptr },
	{ "onClose",             MenuParse_onClose,             nullptr },
	//JLFACCEPT MPMOVED
	{ "onAccept",            MenuParse_onAccept,            nullptr },
	{ "onESC",               MenuParse_onESC,               nullptr },
	{ "outOfBoundsClick",    MenuParse_outOfBounds,         nullptr },
	{ "onOpen",              MenuParse_onOpen,              nullptr },
	{ "outlinecolor",        MenuParse_outlinecolor,        nullptr },
	{ "ownerdraw",           MenuParse_ownerdraw,           nullptr },
	{ "ownerdrawFlag",       MenuParse_ownerdrawFlag,       nullptr },
	{ "popup",               MenuParse_popup,               nullptr },
	{ "rect",                MenuParse_rect,                nullptr },
	{ "soundLoop",           MenuParse_soundLoop,           nullptr },
	{ "style",               MenuParse_style,               nullptr },
	{ "visible",             MenuParse_visible,             nullptr },
	{ 0,                     0,                             0 }
};

keywordHash_t *menuParseKeywordHash[KEYWORDHASH_SIZE];

void Menu_SetupKeywordHash(void) {
	int i;

	memset(menuParseKeywordHash, 0, sizeof(menuParseKeywordHash));
	for (i = 0; menuParseKeywords[i].keyword; i++) {
		KeywordHash_Add(menuParseKeywordHash, &menuParseKeywords[i]);
	}
}

bool Menu_Parse(int handle, menuDef_t *menu) {
	pc_token_t token;
	keywordHash_t *key;

	if (!trap->PC_ReadToken(handle, &token))
		return false;
	if (*token.string != '{') {
		return false;
	}

	while ( 1 ) {
		if (!trap->PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu");
			return false;
		}

		if (*token.string == '}') {
			return true;
		}

		key = KeywordHash_Find(menuParseKeywordHash, token.string);
		if (!key) {
			PC_SourceError(handle, "unknown menu keyword %s", token.string);
			continue;
		}
		if ( !key->func((itemDef_t*)menu, handle) ) {
			PC_SourceError(handle, "couldn't parse menu keyword %s", token.string);
			return false;
		}
	}
	return false;
}

void Menu_New(int handle) {
	menuDef_t *menu = &Menus[menuCount];

	if (menuCount < MAX_MENUS) {
		Menu_Init(menu);
		if (Menu_Parse(handle, menu)) {
			Menu_PostParse(menu);
			menuCount++;
		}
	}
}

int Menu_Count() {
	return menuCount;
}

void Menu_PaintAll() {
	int i;
	if (captureFunc) {
		captureFunc(captureData);
	}

	for (i = 0; i < Menu_Count(); i++) {
		Menu_Paint(&Menus[i], false);
	}

	if (debugMode) {
		vec4_t v = {1, 1, 1, 1};
		Text text{ JKFont::Small2, 0.75f };
		Text_Paint( text, 5, 25, va("fps: %f", DC->FPS), v);
		Text_Paint( text, 5, 45, va("x: %d  y:%d", DC->cursorx,DC->cursory), v);
	}
}

void Menu_Reset(void) {
	menuCount = 0;
}

displayContextDef_t *Display_GetContext() {
	return DC;
}

void *Display_CaptureItem(int x, int y) {
	int i;

	for (i = 0; i < menuCount; i++) {
		// turn off focus each item
		// menu->items[i].window.flags &= ~WINDOW_HASFOCUS;
		if (Rect_ContainsPoint(&Menus[i].window.rect, x, y)) {
			return &Menus[i];
		}
	}
	return nullptr;
}

// FIXME:
bool Display_MouseMove(void *p, int x, int y) {
	int i;
	menuDef_t *menu = (menuDef_t *) p;

	if (menu == nullptr) {
    menu = Menu_GetFocused();
		if (menu) {
			if (menu->window.flags & WINDOW_POPUP) {
				Menu_HandleMouseMove(menu, x, y);
				return true;
			}
		}
		for (i = 0; i < menuCount; i++) {
			Menu_HandleMouseMove(&Menus[i], x, y);
		}
	} else {
		menu->window.rect.x += x;
		menu->window.rect.y += y;
		Menu_UpdatePosition(menu);
	}
	return true;

}

int Display_CursorType(int x, int y) {
	int i;
	for (i = 0; i < menuCount; i++) {
		rectDef_t r2;
		r2.x = Menus[i].window.rect.x - 3;
		r2.y = Menus[i].window.rect.y - 3;
		r2.w = r2.h = 7;
		if (Rect_ContainsPoint(&r2, x, y)) {
			return CURSOR_SIZER;
		}
	}
	return CURSOR_ARROW;
}

void Display_HandleKey(int key, bool down, int x, int y) {
	menuDef_t *menu = (menuDef_t *) Display_CaptureItem(x, y);
	if (menu == nullptr) {
		menu = Menu_GetFocused();
	}
	if (menu) {
		Menu_HandleKey(menu, key, down );
	}
}

static void Window_CacheContents(windowDef_t *window) {
	if (window) {
		if (window->cinematicName) {
			int cin = DC->playCinematic(window->cinematicName, 0, 0, 0, 0);
			DC->stopCinematic(cin);
		}
	}
}

static void Item_CacheContents(itemDef_t *item) {
	if (item) {
		Window_CacheContents(&item->window);
	}

}

static void Menu_CacheContents(menuDef_t *menu) {
	if (menu) {
		int i;
		Window_CacheContents(&menu->window);
		for (i = 0; i < menu->itemCount; i++) {
			Item_CacheContents(menu->items[i]);
		}

		if (menu->soundName && *menu->soundName) {
			DC->registerSound(menu->soundName);
		}
	}

}

void Display_CacheAll() {
	int i;
	for (i = 0; i < menuCount; i++) {
		Menu_CacheContents(&Menus[i]);
	}
}

static bool Menu_OverActiveItem(menuDef_t *menu, float x, float y) {
	if (menu && menu->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED)) {
		if (Rect_ContainsPoint(&menu->window.rect, x, y)) {
			int i;
			for (i = 0; i < menu->itemCount; i++) {
				// turn off focus each item
				// menu->items[i].window.flags &= ~WINDOW_HASFOCUS;

				if (!(menu->items[i]->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED))) {
					continue;
				}

				if (menu->items[i]->window.flags & WINDOW_DECORATION) {
					continue;
				}

				if (Rect_ContainsPoint(&menu->items[i]->window.rect, x, y)) {
					itemDef_t *overItem = menu->items[i];
					if (overItem->type == ITEM_TYPE_TEXT && overItem->text) {
						if (Rect_ContainsPoint(&overItem->window.rect, x, y)) {
							return true;
						} else {
							continue;
						}
					} else {
						return true;
					}
				}
			}

		}
	}
	return false;
}

void String_Init() {
	int i;
	for (i = 0; i < HASH_TABLE_SIZE; i++) {
		strHandle[i] = 0;
	}
	strHandleCount = 0;
	strPoolIndex = 0;
	menuCount = 0;
	openMenuCount = 0;
	UI_InitMemory();
	Item_SetupKeywordHash();
	Menu_SetupKeywordHash();
	if (DC && DC->getBindingBuf) {
		Controls_GetConfig();
	}
}

