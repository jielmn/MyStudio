#include "loHttpStr.h"

namespace lohttp 
{
	CloHttpStrA::PCSZ  CloHttpStrA::szEmptyString = "" ;
	CloHttpStrW::PCSZ  CloHttpStrW::szEmptyString = L"" ;

	//// Conversion methods
	//// This method returns a converted ansi string from a unicode string.
	//// The returned string must be freed by using the ::free () function.
	//LPSTR CloHttpStrA::Unicode2Ansi (LPCWSTR szStr, unsigned int CodePage)
	//	throw ()
	//{
	//	// The unicode encodings are not allowed
	//	if ( !( (CodePage != CP_UTF8) && (CodePage != CP_UTF7) ) ){
	//		return NULL;
	//	}

	//	if ( szStr == NULL )
	//		return NULL ;

	//	int	cchNeeded = 0;

	//	if ( 0 == (cchNeeded = ::WideCharToMultiByte (CodePage, 0, szStr, -1, NULL, 0, NULL, NULL)) )
	//	{
	//		return NULL;
	//	}

	//	PSTR szAnsi = (PSTR) ::malloc (sizeof (char) * cchNeeded) ;
	//	if ( szAnsi == NULL ){
	//		return NULL;
	//	}

	//	if ( 0 == ::WideCharToMultiByte (CodePage, 0, szStr, -1, szAnsi, cchNeeded, NULL, NULL) ) {
	//		SAFEFREE (szAnsi) ;
	//		return NULL;
	//	}

	//	return szAnsi ;
	//}

	//// This method returns a converted unicode string from a ansi string.
	//// The returned string must be freed by using the ::free () function.
	//LPWSTR CloHttpStrA::Ansi2Unicode (LPCSTR szStr, unsigned int CodePage)
	//	throw ()
	//{
	//	// The unicode encodings are not allowed
	//	if( !( (CodePage != CP_UTF8) && (CodePage != CP_UTF7) ) ){
	//		return NULL;
	//	}

	//	if ( szStr == NULL )
	//		return NULL ;

	//	int	cchNeeded = 0;
	//	if ( 0 == (cchNeeded = ::MultiByteToWideChar (CodePage, 0, szStr, -1, NULL, 0)) ){
	//		return NULL;
	//	}

	//	PWSTR szUni = (PWSTR) ::malloc (sizeof (WCHAR) * cchNeeded) ;
	//	if ( szUni == NULL ){
	//		return NULL;
	//	}

	//	if ( 0 == ::MultiByteToWideChar (CodePage, 0, szStr, -1, szUni, cchNeeded) ) {
	//		SAFEFREE (szUni) ;
	//		return NULL;
	//	}
	//	return szUni ;
	//}



	// Conversion methods
	// This function returns a converted ansi string from a unicode string.
	// The returned string must be freed by using the ::free () function.
	//LPSTR CloHttpStrW::Unicode2Ansi (LPCWSTR szStr, unsigned int CodePage)
	//	throw ()
	//{
	//	// The unicode encodings are not allowed
	//	if( !((CodePage != CP_UTF8) && (CodePage != CP_UTF7) ) ){
	//		return NULL;
	//	}

	//	if ( szStr == NULL )
	//		return NULL ;

	//	int	cchNeeded = 0;

	//	if ( 0 == (cchNeeded = ::WideCharToMultiByte (CodePage, 0, szStr, -1, NULL, 0, NULL, NULL)) ){
	//		return NULL;
	//	}

	//	PSTR szAnsi = (PSTR) ::malloc (sizeof (char) * cchNeeded) ;
	//	if ( szAnsi == NULL ){
	//		return NULL;
	//	}

	//	if ( 0 == ::WideCharToMultiByte (CodePage, 0, szStr, -1, szAnsi, cchNeeded, NULL, NULL) ) {
	//		SAFEFREE (szAnsi) ;
	//		return NULL;
	//	}

	//	return szAnsi ;
	//}

	//// This method returns a converted unicode string from a ansi string.
	//// The returned string must be freed by using the ::free () function.
	//LPWSTR CloHttpStrW::Ansi2Unicode (LPCSTR szStr, unsigned int CodePage)
	//	throw ()
	//{
	//	// The unicode encodings are not allowed
	//	if( ! ( (CodePage != CP_UTF8) && (CodePage != CP_UTF7) ) ){
	//		return NULL;
	//	}

	//	if ( szStr == NULL )
	//		return NULL ;

	//	int	cchNeeded =0;
	//	if ( 0 == (cchNeeded = ::MultiByteToWideChar (CodePage, 0, szStr, -1, NULL, 0)) ){
	//		return NULL;
	//	}

	//	PWSTR szUni = (PWSTR) ::malloc (sizeof (WCHAR) * cchNeeded) ;
	//	if ( szUni == NULL ){
	//		return NULL;
	//	}

	//	if ( 0 == ::MultiByteToWideChar (CodePage, 0, szStr, -1, szUni, cchNeeded) ) {
	//		SAFEFREE (szUni) ;
	//		return NULL;
	//	}

	//	return szUni ;
	//}
}
