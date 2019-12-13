#pragma once

#include "ui/menudef.h"

struct Font {
	int   menuFont;
	float scale;
	bool  customFont;

	Font(
		int fontNum,
		float fontScale,
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
		float adjust = 0.0f
	) const;
};
