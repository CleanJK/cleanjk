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

#pragma once


// @file RAII C++ bindings for filesystem operations

// ======================================================================
// INCLUDE
// ======================================================================

#include <cstddef>
#include <cassert>

#include "qcommon/q_platform.h"
#include "qcommon/safe/gsl.h"

// ======================================================================
// CLASS
// ======================================================================

namespace FS
{
	class FileBuffer
	{
		friend FileBuffer ReadFile( gsl::czstring );
		// called by ReadFile()
		FileBuffer( void* buffer, const long size ) NOEXCEPT;
	public:
		FileBuffer() NOEXCEPT = default;
		~FileBuffer() NOEXCEPT;
		// noncopyable
		FileBuffer( const FileBuffer& ) = delete;
		FileBuffer& operator=( const FileBuffer& ) = delete;
		// movable
		FileBuffer( FileBuffer&& rhs ) NOEXCEPT;
		FileBuffer& operator=( FileBuffer&& rhs ) NOEXCEPT;

		// nullptr if no such file
		const char* begin() const NOEXCEPT
		{
			return static_cast< const char* >( _buffer );
		}
		const char* end() const NOEXCEPT
		{
			return static_cast< const char* >( _buffer ) + _size;
		}
		long size() const NOEXCEPT
		{
			return _size;
		}
		bool valid() const NOEXCEPT
		{
			return _buffer != nullptr;
		}
		gsl::cstring_view view() const NOEXCEPT
		{
			return{ begin(), end() };
		}

	private:
		// TODO: ought to be const; would have to fix FS_ReadFile though.
		void* _buffer = nullptr;
		long _size = 0;
	};

	FileBuffer ReadFile( gsl::czstring path );

	// FileList only available in Client; Library exclusively uses FS_GetFileList(), which by supplying a buffer avoids dynamic allocations.
	// TODO: investigate making FS_ListFiles available in Library Code?
#if !defined( SP_GAME )
	class FileList
	{
		friend FileList ListFiles( const char*, const char* );
		// called by ListFiles()
		FileList( char** files, int numFiles ) NOEXCEPT;
	public:
		FileList() NOEXCEPT = default;
		~FileList() NOEXCEPT;
		// noncopyable
		FileList( const FileList& ) = delete;
		FileList& operator=( const FileList& ) = delete;
		// movable
		FileList( FileList&& rhs ) NOEXCEPT;
		FileList& operator=( FileList&& rhs ) NOEXCEPT;

		const char *const *begin() const NOEXCEPT
		{
			return _begin;
		}
		const char *const *end() const NOEXCEPT
		{
			return _end;
		}
		std::size_t size() const NOEXCEPT
		{
			return static_cast< std::size_t >( _end - begin() );
		}

	private:
		// TODO: this should really be const; it's a matter of making FS_ListFiles' result const.
		char** _begin = nullptr;
		const char* const* _end = nullptr;
	};

	/**
	@brief Returns a list of the files in a directory.

	The returned files will not include any directories or `/`.

	@note ERR_FATAL if called before file system initialization
	@todo Which is it? Returns no directories at all, or returns them when looking for extension "/"?
	@param directory should not have either a leading or trailing /
	@param extension if "/", only subdirectories will be returned
	*/
	FileList ListFiles( const char* directory, const char* extension );
#endif
}
