#pragma once

typedef sfxHandle_t SoundID;
typedef qhandle_t ImageID;
typedef qhandle_t ModelID;
typedef fxHandle_t EffectID;

typedef struct cgMedia_s {
	struct efx_s {
		EffectID	null;
	} efx;

	struct gfx_s {
		ImageID		null;

		struct hud_s {
			ImageID		crosshairs[NUM_CROSSHAIRS];

			struct flag_s {
				struct flagSet_s {
					ImageID		normal;
					ImageID		taken;
					ImageID		ysal;
				} red, blue;
			} flag;
		} hud;

		struct interface_s {
			ImageID		charset;
			ImageID		cursor;

			struct loading_s {
				ImageID		tick;
				ImageID		cap;
				ImageID		background;
			} loading;
		} interface;

		struct misc_s {
			ImageID		white;
		} misc;
	} gfx;

	struct models_s {
		ModelID		null;
	} models;

	struct sounds_s {
		SoundID		null;
	} sounds;
} cgMedia_t;
extern cgMedia_t media;

void CG_PreloadMedia( void );
void CG_LoadMedia( void );
