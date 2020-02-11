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

#pragma once

// interface for the CQuickSprite class.

// this include must remain at the top of every CPP file
//#include "qcommon/q_math.hpp"

// ======================================================================
// CLASS
// ======================================================================

class CQuickSpriteSystem
{
private:
			textureBundle_t	*mTexBundle;
			unsigned long	mGLStateBits;
			int				mFogIndex;
			bool		mUseFog;
			vec4_t			mVerts[SHADER_MAX_VERTEXES];
			unsigned int	mIndexes[SHADER_MAX_VERTEXES];			// Ideally this would be static, cause it never changes
			vec2_t			mTextureCoords[SHADER_MAX_VERTEXES];	// Ideally this would be static, cause it never changes
			vec2_t			mFogTextureCoords[SHADER_MAX_VERTEXES];
			unsigned long	mColors[SHADER_MAX_VERTEXES];
			int				mNextVert;

			void Flush(void);

public:
			CQuickSpriteSystem();
	virtual ~CQuickSpriteSystem();

			void StartGroup(textureBundle_t *bundle, unsigned long glbits, int fogIndex = -1);
			void EndGroup(void);

			void Add(float *pointdata, color4ub_t color, vec2_t fog=nullptr);
};

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern CQuickSpriteSystem SQuickSprite;
