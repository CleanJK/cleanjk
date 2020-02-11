#include "qcommon/q_shared.hpp"
#include "qcommon/q_string.hpp"

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "qcommon/q_color.hpp"

int Q_isprint( int c )
{
	if ( c >= 0x20 && c <= 0x7E )
		return ( 1 );
	return ( 0 );
}

int Q_isprintext( int c )
{
	if ( c >= 0x20 && c <= 0x7E )
		return (1);
	if ( c >= 0x80 && c <= 0xFE )
		return (1);
	return (0);
}

int Q_isgraph( int c )
{
	if ( c >= 0x21 && c <= 0x7E )
		return (1);
	if ( c >= 0x80 && c <= 0xFE )
		return (1);
	return (0);
}

int Q_islower( int c )
{
	if (c >= 'a' && c <= 'z')
		return ( 1 );
	return ( 0 );
}

int Q_isupper( int c )
{
	if (c >= 'A' && c <= 'Z')
		return ( 1 );
	return ( 0 );
}

int Q_isalpha( int c )
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return ( 1 );
	return ( 0 );
}

bool Q_isanumber( const char *s )
{
	char *p;
	double ret;

	if( *s == '\0' )
		return false;

	ret = strtod( s, &p );

	if ( ret == HUGE_VAL || errno == ERANGE )
		return false;

	return (bool)(*p == '\0');
}

bool Q_isintegral( float f )
{
	return (bool)( (int)f == f );
}

char* Q_strrchr( const char* string, int c )
{
	char cc = c;
	char *s;
	char *sp=(char *)0;

	s = (char*)string;

	while (*s)
	{
		if (*s == cc)
			sp = s;
		s++;
	}
	if (cc == 0)
		sp = s;

	return sp;
}

// Safe strncpy that ensures a trailing zero
void Q_strncpyz( char *dest, const char *src, int destsize ) {
	assert(src);
	assert(dest);
	assert(destsize);

	strncpy( dest, src, destsize-1 );
	dest[destsize-1] = 0;
}

int Q_stricmpn (const char *s1, const char *s2, int n) {
	int		c1, c2;

	if ( s1 == nullptr ) {
		if ( s2 == nullptr )
			return 0;
		else
			return -1;
	}
	else if ( s2==nullptr )
		return 1;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}

		if (c1 != c2) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);

	return 0;		// strings are equal
}

int Q_stricmp (const char *s1, const char *s2) {
	return (s1 && s2) ? Q_stricmpn (s1, s2, 99999) : -1;
}

int Q_strncmp (const char *s1, const char *s2, int n) {
	int		c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}

		if (c1 != c2) {
			return c1 < c2 ? -1 : 1;
		}
	} while (c1);

	return 0;		// strings are equal
}

char *Q_strlwr( char *s1 ) {
	char	*s;

	s = s1;
	while ( *s ) {
		*s = tolower(*s);
		s++;
	}
	return s1;
}

char *Q_strupr( char *s1 ) {
	char	*s;

	s = s1;
	while ( *s ) {
		*s = toupper(*s);
		s++;
	}
	return s1;
}

// never goes past bounds or leaves without a terminating 0
void Q_strcat( char *dest, int size, const char *src ) {
	int		l1;

	l1 = strlen( dest );
	if ( l1 >= size ) {
		//Com_Error( ERR_FATAL, "Q_strcat: already overflowed" );
		return;
	}
	if ( strlen(src)+1 > (size_t)(size - l1))
	{	//do the error here instead of in Q_strncpyz to get a meaningful msg
		//Com_Error(ERR_FATAL,"Q_strcat: cannot append \"%s\" to \"%s\"", src, dest);
		return;
	}
	Q_strncpyz( dest + l1, src, size - l1 );
}

// Find the first occurrence of find in s.
const char *Q_stristr( const char *s, const char *find )
{
	char c, sc;
	size_t len;

	if ((c = *find++) != 0)
	{
		if (c >= 'a' && c <= 'z')
		{
			c -= ('a' - 'A');
		}
		len = strlen(find);
		do
		{
			do
			{
				if ((sc = *s++) == 0)
					return nullptr;
				if (sc >= 'a' && sc <= 'z')
				{
					sc -= ('a' - 'A');
				}
			} while (sc != c);
		} while (Q_stricmpn(s, find, len) != 0);
		s--;
	}
	return s;
}

// strlen that discounts Quake color sequences
int Q_PrintStrlen( const char *string ) {
	int			len;
	const char	*p;

	if( !string ) {
		return 0;
	}

	len = 0;
	p = string;
	while( *p ) {
		if( Q_IsColorString( p ) ) {
			p += 2;
			continue;
		}
		p++;
		len++;
	}

	return len;
}

char *Q_CleanStr( char *string ) {
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( Q_IsColorString( s ) ) {
			s++;
		}
		else if ( c >= 0x20 && c <= 0x7E ) {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

// Strips coloured strings in-place using multiple passes: "fgs^^56fds" -> "fgs^6fds" -> "fgsfds"
// This function modifies INPUT (is mutable)
// (Also strips ^8 and ^9)
void Q_StripColor( char *text ) {
	bool doPass = true;
	char *read;
	char *write;

	while ( doPass ) {
		doPass = false;
		read = write = text;
		while ( *read ) {
			if ( Q_IsColorStringExt( read ) ) {
				doPass = true;
				read += 2;
			}
			else {
				// Avoid writing the same data over itself
				if ( write != read ) {
					*write = *read;
				}
				write++;
				read++;
			}
		}
		if ( write < read )  {
			// Add trailing NUL byte if string has shortened
			*write = '\0';
		}
	}
}

// Replace strip[x] in string with repl[x] or remove characters entirely
// mutates `string`
//Examples:
//	Q_strstrip( "Bo\nb is h\rairy!!", "\n\r!", "123" )	--> "Bo1b is h2airy33"
//	Q_strstrip( "Bo\nb is h\rairy!!", "\n\r!", "12" )	--> "Bo1b is h2airy"
//	Q_strstrip( "Bo\nb is h\rairy!!", "\n\r!", nullptr )	--> "Bob is hairy"
void Q_strstrip( char *string, const char *strip, const char *repl )
{
	char		*out=string, *p=string, c;
	const char	*s=strip;
	int			replaceLen = repl?strlen( repl ):0, offset=0;
	bool	recordChar = true;

	while ( (c = *p++) != '\0' ) {
		recordChar = true;
		for ( s=strip; *s; s++ ) {
			offset = s-strip;
			if ( c == *s ) {
				if ( !repl || offset >= replaceLen ) {
					recordChar = false;
				}
				else {
					c = repl[offset];
				}
				break;
			}
		}
		if ( recordChar ) {
			*out++ = c;
		}
	}
	*out = '\0';
}

// Find any characters in a string. Think of it as a shorthand strchr loop.
// returns first instance of any characters found, otherwise nullptr
const char *Q_strchrs( const char *string, const char *search ) {
	const char *p = string, *s = search;

	while ( *p != '\0' ) {
		for ( s=search; *s; s++ ) {
			if ( *p == *s ) {
				return p;
			}
		}
		p++;
	}

	return nullptr;
}

#if defined(_MSC_VER)
// Special wrapper function for Microsoft's broken _vsnprintf() function.
// MinGW comes with its own snprintf() which is not broken.
// vsnprintf is ISO/IEC 9899:1999
// abstracting this to make it portable
int Q_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	int retval;

	retval = _vsnprintf(str, size, format, ap);

	if(retval < 0 || retval == size)
	{
		// Microsoft doesn't adhere to the C99 standard of vsnprintf, which states that the return value must be the number of bytes written if the output
		//	string had sufficient length.
		// Obviously we cannot determine that value from Microsoft's implementation, so we have no choice but to return size.

		str[size - 1] = '\0';
		return size;
	}

	return retval;
}
#endif
