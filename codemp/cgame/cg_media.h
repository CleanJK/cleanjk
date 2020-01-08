/*
===========================================================================
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

#pragma once

// ======================================================================
// DEFINE
// ======================================================================

using SoundID = sfxHandle_t;
using ImageID = qhandle_t;
using ModelID = qhandle_t;
using EffectID = fxHandle_t;

// ======================================================================
// STRUCT
// ======================================================================

typedef struct cgMedia_s
{
	struct efx_s
	{
		EffectID	null;
	} efx;

	struct gfx_s
	{
		ImageID		null;

		struct hud_s
		{
			ImageID		crosshairs[NUM_CROSSHAIRS];

			struct flag_s
			{
				struct flagSet_s
				{
					ImageID		normal;
					ImageID		taken;
					ImageID		ysal;
				} red, blue;
			} flag;
		} hud;

		struct interface_s
		{
			ImageID		charset;
			ImageID		cursor;

			struct loading_s
			{
				ImageID		tick;
				ImageID		cap;
				ImageID		background;
			} loading;
		} interface;

		struct misc_s
		{
			ImageID		white;
		} misc;
	} gfx;

	struct models_s
	{
		ModelID		null;
	} models;

	struct sounds_s
	{
		SoundID		null;
	} sounds;
} cgMedia_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern cgMedia_t media;

// ======================================================================
// FUNCTION
// ======================================================================

void CG_LoadMedia(void);
void CG_PreloadMedia(void);