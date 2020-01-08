#pragma once

using SoundID = sfxHandle_t;
using ImageID = qhandle_t;
using ModelID = qhandle_t;
using EffectID = fxHandle_t;

struct cgMedia_t {
	struct efx_t {
		EffectID	null;
	} efx;

	struct gfx_t {
		ImageID		null;

		struct hud_t {
			ImageID		crosshairs[NUM_CROSSHAIRS];

			struct flag_t {
				struct flagSet_t {
					ImageID		normal;
					ImageID		taken;
					ImageID		ysal;
				} red, blue;
			} flag;
		} hud;

		struct interface_t {
			ImageID		charset;
			ImageID		cursor;

			struct loading_t {
				ImageID		tick;
				ImageID		cap;
				ImageID		background;
			} loading;
		} interface;

		struct misc_t {
			ImageID		white;
		} misc;
	} gfx;

	struct models_t {
		ModelID		null;
	} models;

	struct sounds_t {
		SoundID		null;
	} sounds;
};
extern cgMedia_t media;

void CG_LoadMedia( void );
void CG_PreloadMedia( void );

