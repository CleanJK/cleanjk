#pragma once

#include "qcommon/q_color.h"
#include "ui/menudef.h"

struct Font {
	int   menuFont;
	float scale;
	int   alignment;
	bool  customFont;

	Font(
		int fontNum,
		float fontScale,
		int alignment = ITEM_ALIGN_LEFT,
		bool isCustomFont = false
	);

	float Width(
		const char *text
	) const;

	float Height(
		const char *text = nullptr // doesn't not actually read `text`
	) const;

	void Paint(
		float x,
		float y,
		const char *text,
		const vec4_t colour = colorTable[CT_WHITE],
		int style = ITEM_TEXTSTYLE_NORMAL,
		int limit = 0,
		float adjust = 0.0f,
		int cursorPos = -1,
		bool overstrike = false
	) const;
};

// helpers

// maxX param is initially an X limit, but is also used as feedback. 0 = text was clipped to fit within, else maxX = next pos
void Text_Paint_Limit(
	float *maxX,
	float x,
	float y,
	float scale,
	const vec4_t color,
	const char *text,
	float adjust,
	int limit,
	int fontNum,
	bool customFont = false
);

void Text_Paint_Proportional(
	int x,
	int y,
	const char *str,
	int style,
	const vec4_t color,
	int fontNum = FONT_MEDIUM,
	float scale = 1.0f
);
