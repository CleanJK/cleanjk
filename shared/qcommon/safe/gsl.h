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

// ======================================================================
// INCLUDE
// ======================================================================

// central point of include to simplify possible future swap for Microsoft's implementation
#include <gsl/gsl-lite.h>

// ======================================================================
// DEFINE / FUNCTION
// ======================================================================

// the default cstring_view constructor from string literals includes the terminating null; this one does not.
#if defined( _MSC_VER ) && _MSC_VER < 1900
// VS2013 needs a workaround for its lack of user-defined literals. Fuck VS2013.
// TODO: eradicate VS2013
// The workaround is using CSTRING_VIEW("literal") instead of "literal"_v (for the time being).
# define CSTRING_VIEW(x) vs2013hack_cstring_view_literal(x)
template< int length >
inline gsl::cstring_view vs2013hack_cstring_view_literal( const char (&str)[length] )
{
	static_assert( length > 0, "CSTRING_VIEW expects a string literal argument." );
	return{ str, str + length - 1 };
}
#else
# define CSTRING_VIEW(x) x ## _v
/** gsl::cstring_view from string literal (without null-termination) */
inline gsl::cstring_view operator"" _v( const char* str, std::size_t length )
{
	return{ str, str + length };
}
#endif
