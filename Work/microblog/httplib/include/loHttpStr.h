#pragma once

#if defined(WIN32) || defined(_WIN32)

#include <windows.h>			// for generic types, .. etc
#include <tchar.h>

#else


#include <wchar.h>
#include <string.h>
#include "bbiconv.h"

//
//  Code Page Default Values.
//
#define CP_ACP                    0           // default to ANSI code page

#define stricmp(lhs,rhs)						strcmp(lhs,rhs)
#define _strnicmp(lhs,rhs,count)				strncmp(lhs,rhs,count)
#define _stricmp(lhs,rhs)						stricmp(lhs,rhs)
#define _vsnprintf(buffer,count,format,args) 	vsnprintf(buffer,count,format,args)

#define wcsicmp(lhs,rhs)						wcscmp(lhs,rhs)
#define _wcsnicmp(lhs,rhs,count)				wcsicmp(lhs,rhs)
#define _wcsicmp(lhs,rhs)						wcsicmp(lhs,rhs)
#define _vsnwprintf(buffer,count,format,args) 	vswprintf(buffer,count,format,args)

#endif

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libhttp/loHttpDef.h"

#pragma warning (push)
#pragma warning (disable: 4290)	// avoids 'C++ Exception Specification ignored' message
#pragma warning (disable: 4996)	// avoids 'This function or variable may be unsafe' message

/*!
* \brief	The namespace of the lohttp's library
*
* This is the namespace for source codes written by Jo Hyeong-ryeol.
*/
namespace lohttp
{

#ifndef SAFEFREE
#define SAFEFREE(x) if(x){ ::free((void *)x); x = NULL; }
#endif

	class CloHttpStrA
	{
	public:
		typedef char	    CharType ;
		typedef char*	    PSZ ;
		typedef const char*	PCSZ ;

		static PCSZ	szEmptyString ;

		static inline BOOL IsAnsi (void) throw ()
		{
			return TRUE ;
		}

		// Wrapper methods for CRT string functions
		static inline size_t StringLen (PCSZ szStr) throw ()
		{
			if( szStr == NULL )
				return 0;
			return ::strlen (szStr) ;
		}

		static inline PSZ StringCopy (PSZ szDest, PCSZ szSrc) throw ()
		{
			return ::strcpy (szDest, szSrc) ;
		}

		static inline PSZ StringCat (PSZ szDest, PCSZ szSrc) throw ()
		{
			return ::strcat (szDest, szSrc) ;
		}

		static inline PSZ StringNCopy (PSZ szDest, PCSZ szSrc, size_t count) throw ()
		{
			return ::strncpy (szDest, szSrc, count) ;
		}

		static inline int StringNICmp (PCSZ szStr1, PCSZ szStr2, size_t count) throw ()
		{
			return ::_strnicmp (szStr1, szStr2, count) ;
		}

		static inline int StringCmp (PCSZ szStr1, PCSZ szStr2) throw ()
		{
			return ::strcmp (szStr1, szStr2) ;
		}

		static inline PCSZ StringChr (PCSZ szStr, int Chr) throw ()
		{
			return ::strchr (szStr, Chr) ;
		}

		static inline PCSZ StringRChr (PCSZ szStr, int Chr) throw ()
		{
			return ::strrchr (szStr, Chr) ;
		}

		static int SNPrintf (PSZ buffer, size_t count, PCSZ format, ...) throw ()
		{
			int			cchWritten ;
			va_list		args ;

			va_start (args, format) ;
			cchWritten = ::_vsnprintf (buffer, count, format, args) ;
			va_end (args) ;
			return cchWritten ;
		}

		static PCSZ StringStr (PCSZ szString, PCSZ szSearch) throw ()
		{
			return ::strstr (szString, szSearch) ;
		}

		static unsigned long StringToUL (PCSZ szString, PSZ * pEndPtr, int base) throw ()
		{
			return ::strtoul (szString, pEndPtr, base) ;
		}

		//// Conversion methods
		//static PSZ Unicode2Ansi (PCSZ szStr, unsigned int CodePage = CP_ACP) throw () ;
		//static PSZ Ansi2Unicode (PCSZ szStr, unsigned int CodePage = CP_ACP) throw () ;

		//// Methods for Ansi character set
		//static BOOL IsDBCSLeadByteEx (unsigned int CodePage, char TestChar) throw ()
		//{
		//	return ::IsDBCSLeadByteEx (CodePage, TestChar) ;
		//}

		bool operator () (PCSZ szKey1, PCSZ szKey2) const throw ()
		{
			// return true if the two strings are null
			if ( szKey1 == NULL && szKey2 == NULL )
				return true ;

			if ( szKey1 == NULL )
				return true ;

			if ( szKey2 == NULL )
				return false ;

			// case insensitive
			return ::_stricmp(szKey1, szKey2) < 0 ;
		}
		
	};

	class CloHttpStrW
	{
	public:
		typedef wchar_t	            CharType ;
		typedef wchar_t*	        PSZ ;
		typedef const wchar_t*   	PCSZ ;

		static PCSZ	szEmptyString ;

		static inline BOOL IsAnsi (void) throw ()
		{
			return FALSE ;
		}

		static inline size_t StringLen (PCSZ szStr) throw ()
		{
			return ::wcslen (szStr) ;
		}

		static inline PSZ StringCopy (PSZ szDest, PCSZ szSrc) throw ()
		{
			return ::wcscpy (szDest, szSrc) ;
		}

		static inline PSZ StringCat (PSZ szDest, PCSZ szSrc) throw ()
		{
			return ::wcscat (szDest, szSrc) ;
		}

		static inline PSZ StringNCopy (PSZ szDest, PCSZ szSrc, size_t count) throw ()
		{
			return ::wcsncpy (szDest, szSrc, count) ;
		}

		static inline int StringNICmp (PCSZ szStr1, PCSZ szStr2, size_t count) throw ()
		{
			return ::_wcsnicmp (szStr1, szStr2, count) ;
		}

		static inline int StringCmp (PCSZ szStr1, PCSZ szStr2) throw ()
		{
			return ::wcscmp (szStr1, szStr2) ;
		}

		static inline PCSZ StringChr (PCSZ szStr, CharType Chr) throw ()
		{
			return ::wcschr (szStr, Chr) ;
		}

		static inline PCSZ StringRChr (PCSZ szStr, CharType Chr) throw ()
		{
			return ::wcsrchr (szStr, Chr) ;
		}

		static int SNPrintf (PSZ buffer, size_t count, PCSZ format, ...) throw ()
		{
			int			cchWritten ;
			va_list		args ;

			va_start (args, format) ;
			cchWritten = ::_vsnwprintf (buffer, count, format, args) ;
			va_end (args) ;
			return cchWritten ;
		}

		static PCSZ StringStr (PCSZ szString, PCSZ szSearch) throw ()
		{
			return ::wcsstr (szString, szSearch) ;
		}

		static unsigned long StringToUL (PCSZ szString, PSZ * pEndPtr, int base) throw ()
		{
			return ::wcstoul (szString, pEndPtr, base) ;
		}

		//// Conversion methods
		//static PSZ Unicode2Ansi (PCSZ szStr, unsigned int CodePage = CP_ACP) throw () ;
		//static PSZ Ansi2Unicode (PCSZ szStr, unsigned int CodePage = CP_ACP) throw () ;

		//// Methods for Ansi character set
		//static BOOL IsDBCSLeadByteEx (unsigned int /* CodePage */, CharType /* TestChar */) throw ()
		//{
		//	return FALSE ;		// always returns FALSE
		//}

		// comparison function (used by STL multimap)
		bool operator () (PCSZ szKey1, PCSZ szKey2) const 	throw ()
		{
			// return true if the two strings are null
			if ( szKey1 == NULL && szKey2 == NULL )
				return true ;

			if ( szKey1 == NULL )
				return true ;

			if ( szKey2 == NULL )
				return false ;

			// case insensitive
			return ::wcsicmp (szKey1, szKey2) < 0 ;
		}
	};
}

#pragma warning (pop)	// restores the default
