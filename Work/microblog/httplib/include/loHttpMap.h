#ifndef __lo_HTTP_MAP_H__
#define __lo_HTTP_MAP_H__

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#include <windows.h>			// for generic types, .. etc
#endif //defined(WIN32) || defined(_WIN32)

#include <utility>				// for the STL pair type
#include <map>					// for the STL multimap container
#include <vector>				// for the STL vector container
#include <utility>				// for the STL pair type

#include "loSafeInt.hpp"

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

	///////////////////////////////////////// CloHttpMapT /////////////////////////////////////////
	/*!
	* \internal
	* \brief	This is a private map for classes in this file.
	*
	* This is a private map for classes in this file. It uses STL's multimap.
	* If a method's return value is a string or contains a string, it guarantees that
	* it does not return NULL for string. (It returns "" instead of NULL)
	* It returns NULL only if an error occurred or the specified element is not found.
	*/
	template <typename HttpTool>
	class CloHttpMapT
	{
	public:
		typedef typename HttpTool::CharType			CharType ;
		typedef typename HttpTool::PSZ				PSZ ;
		typedef typename HttpTool::PCSZ				PCSZ ;

		CloHttpMapT (void) throw () ;
		virtual ~CloHttpMapT (void) throw () ;

		// MapValue
		typedef struct _MapValue
		{
			PCSZ		szValue ;
			unsigned long		dwFlag ;
			void Delete (void) throw ()
			{
				SAFEFREE(szValue) ;
			}
		} MapValue ;

		typedef typename std::multimap<PCSZ, MapValue, HttpTool>::const_iterator	ConstMapIter ;

		BOOL Clear (void) throw () ;
		BOOL Remove (unsigned long nIdx) throw () ;
		BOOL RemoveAll (PCSZ szName) throw () ;
		BOOL Remove (PCSZ szName, unsigned long nIdx = 0) throw () ;

		BOOL Exists (PCSZ szName, unsigned long nIdx = 0) throw () ;
		unsigned long Count (PCSZ szName = NULL) throw () ;
		BOOL Empty (void) const throw () ;

		PCSZ GetKey (unsigned long nIdx) throw () ;

		MapValue Get (unsigned long nIdx) throw () ;
		PCSZ GetValue (unsigned long nIdx) throw () ;
		unsigned long GetFlag (unsigned long nIdx) throw () ;

		MapValue Get (PCSZ szName, unsigned long nIdx = 0) throw () ;
		PCSZ GetValue (PCSZ szName, unsigned long nIdx = 0) throw () ;
		unsigned long GetFlag (PCSZ szName, unsigned long nIdx = 0) throw () ;

		void AddPointerDirectly (PSZ szName, PSZ szValue = NULL, BOOL dwFlag = 0) throw () ;
		void Add (PCSZ szName, PCSZ szValue = NULL, BOOL dwFlag = 0) throw () ;
		void Set (PCSZ szName, PCSZ szValue = NULL, BOOL dwFlag = 0, unsigned long nIdx = 0) throw () ;

		ConstMapIter Begin () const throw () ;
		ConstMapIter End () const throw () ;

	private:
		// Type definitions for map ===================================================
		typedef typename std::multimap<PCSZ, MapValue, HttpTool>			Map ;
		typedef typename std::multimap<PCSZ, MapValue, HttpTool>::iterator	MapIter ;
		typedef typename std::multimap<PCSZ, MapValue, HttpTool>::size_type	MapSizeType ;
		typedef std::pair<PCSZ, MapValue>									MapItem ;
		Map																	m_map ;
	} ;


	///////////////////////////////////////// CloHttpMapT /////////////////////////////////////////
	template <typename HttpTool>
	CloHttpMapT<HttpTool>::CloHttpMapT (void)
		throw ()
	{
		;	// do nothing yet
	}

	template <typename HttpTool>
	CloHttpMapT<HttpTool>::~CloHttpMapT (void)
		throw ()
	{
		Clear () ;
	}

	// If some data cleared, it will return TRUE, otherwise return FALSE.
	// If memory is exhausted, it can leak memory.
	template <typename HttpTool>
	BOOL CloHttpMapT<HttpTool>::Clear (void)
		throw ()
	{
		if ( m_map.empty () )
			return FALSE ;

		// Allocates memory to save pointers for key name.
		// If an overflow exception or a memory allocation failure is occurs,
		// The memory pointed by the key name pointer are leaked.
		::SafeInt<MapSizeType>	cKeys = m_map.size () ;
		::SafeInt<size_t>		cbRequired = 0 ;
		PCSZ *					arrKeys = NULL ;

		try {
			cbRequired = cKeys ;
			cbRequired *= sizeof (PCSZ) ;
			arrKeys = (PCSZ *) ::malloc (cbRequired.Value ()) ;
		} catch (::SafeIntException &) {
			arrKeys = NULL ;
		}

		MapSizeType		nIdx = 0 ;
		for (MapIter iter = m_map.begin (); iter != m_map.end (); ++iter) {
			(iter->second).Delete () ;

			// Saves the key name pointer
			if ( arrKeys )
				arrKeys[nIdx++] = iter->first ;
		}

		m_map.clear () ;

		if ( arrKeys == NULL )
			return TRUE ;

		for (nIdx = 0; nIdx < cKeys; nIdx++)
			SAFEFREE ( arrKeys[nIdx] ) ;
		SAFEFREE ( arrKeys ) ;

		return TRUE ;
	}

	// This function deletes an element in position nIdx from the map.
	// If the element deleted, it will return TRUE, otherwise return FALSE.
	template <typename HttpTool>
	BOOL CloHttpMapT<HttpTool>::Remove (unsigned long nIdx)
		throw ()
	{
		if ( m_map.empty () )
			return FALSE ;

		// Find the item to delete
		MapIter	 iter = m_map.begin () ;
		for (unsigned long i = 0; i < nIdx; i++) {
			++iter ;

			// If the nIdx is out of range.
			if ( iter == m_map.end () )
				return FALSE ;
		}

		(iter->second).Delete () ;
		PCSZ szName = iter->first ;
		m_map.erase (iter) ;
		SAFEFREE (szName) ;
		return TRUE ;
	}

	// This function erases all elements of which key name equals to szName.
	// If some data cleared, it will return TRUE, otherwise return FALSE.
	// If memory is exhausted, memory will be leaked.
	template <typename HttpTool>
	BOOL CloHttpMapT<HttpTool>::RemoveAll (PCSZ szName)
		throw ()
	{
		if(szName == NULL )
			return FALSE;

		::SafeInt<MapSizeType>	cKeys = m_map.count (szName) ;

		// If the specified key name does not exist in the map
		if ( cKeys.Value () == 0 )
			return FALSE ;

		// Allocates memory to save pointers to key name.
		// If the allocation failed, the memory pointed by the key name pointer are leaked.
		::SafeInt<size_t>	cbRequired ;
		PCSZ *				arrKeys = NULL ;
		try {
			cbRequired = cKeys ;
			cbRequired *= sizeof (PCSZ) ;
			arrKeys = (PCSZ *) ::malloc (cbRequired.Value ()) ;
		} catch (...) {
			arrKeys = NULL ;
		}

		std::pair<MapIter, MapIter>		pairIter = m_map.equal_range (szName) ;

		MapSizeType		nIdx = 0 ;
		for (MapIter iter = pairIter.first; iter != pairIter.second; ++iter) {
			(iter->second).Delete () ;

			// Saves the key name pointer
			if ( arrKeys )
				arrKeys[nIdx++] = iter->first ;
		}

		m_map.erase (pairIter.first, pairIter.second) ;

		if ( arrKeys == NULL )
			return TRUE ;

		for (nIdx = 0; nIdx < cKeys; nIdx++)
			SAFEFREE ( arrKeys[nIdx] ) ;
		SAFEFREE ( arrKeys ) ;

		return TRUE ;
	}

	// This function deletes an element of which the key name equals to szName and the index is nIdx.
	// If some data cleared, it will return TRUE, otherwise return FALSE.
	template <typename HttpTool>
	BOOL CloHttpMapT<HttpTool>::Remove (PCSZ szName, unsigned long nIdx)
		throw ()
	{
		HTTPCLIENT_ASSERT (szName != NULL, "CloHttpMapT::Remove: szName can not be NULL.") ;

		std::pair<MapIter, MapIter>		pairIter = m_map.equal_range (szName) ;

		// If the specified key name does not exist in the map
		if ( pairIter.first == pairIter.second )
			return FALSE ;

		MapIter iter = pairIter.first ;
		for (unsigned long i = 0; i < nIdx; i++) {
			++iter ;

			// If the nIdx is out of range.
			if ( iter == pairIter.second )
				return FALSE ;
		}

		// Deletes the element
		(iter->second).Delete () ;
		PCSZ szKeyName = iter->first ;
		m_map.erase (iter) ;
		SAFEFREE (szKeyName) ;

		return TRUE ;
	}

	template <typename HttpTool>
	BOOL CloHttpMapT<HttpTool>::Exists (PCSZ szName, unsigned long nIdx)
		throw ()
	{
		if( szName == NULL )
			return FALSE;

		::SafeInt<MapSizeType>		cKeys = m_map.count (szName) ;
		return (cKeys > nIdx) ;
	}

	// If the szName is NULL, it will return the count of elements.
	template <typename HttpTool>
	unsigned long CloHttpMapT<HttpTool>::Count (PCSZ szName)
		throw ()
	{
		::SafeInt<unsigned long> cKeys ;
		try 
		{
			cKeys = szName ? m_map.count (szName) : m_map.size () ;
		}
		catch (::SafeIntException &) 
		{
			cKeys = cKeys.MaxInt () ;	// This statement is never executed.
		}
		return cKeys.Value () ;
	}

	template <typename HttpTool>
	BOOL CloHttpMapT<HttpTool>::Empty (void) const
		throw ()
	{
		return m_map.empty () ? TRUE : FALSE ;
	}

	// If the nIdx is not valid, it will return NULL.
	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::PCSZ
		CloHttpMapT<HttpTool>::GetKey (unsigned long nIdx)
		throw ()
	{
		if ( m_map.empty () )
			return NULL ;

		MapIter	iter = m_map.begin () ;
		for (unsigned long i = 0; i < nIdx; i++) {
			++iter ;

			// If the nIdx is not valid.
			if ( iter == m_map.end () )
				return NULL ;
		}
		return iter->first ;
	}

	// If the specified element is not found in position nIdx,
	// returns MapValue of which szValue is NULL.
	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::MapValue
		CloHttpMapT<HttpTool>::Get (unsigned long nIdx)
		throw ()
	{
		MapValue mapValue = { NULL, 0 } ;

		if ( m_map.empty () )
			return mapValue ;

		MapIter	 iter = m_map.begin () ;
		for (unsigned long i = 0; i < nIdx; i++) {
			++iter ;

			// If the nIdx is not valid.
			if ( iter == m_map.end () )
				return mapValue ;
		}

		mapValue = iter->second ;
		if ( mapValue.szValue == NULL )
			mapValue.szValue = HttpTool::szEmptyString ;

		return mapValue ;
	}

	// If the specified element is not found, it will return NULL.
	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::PCSZ
		CloHttpMapT<HttpTool>::GetValue (unsigned long nIdx)
		throw ()
	{
		return Get (nIdx).szValue ;
	}

	// Returns 0 if the specified element is not found.
	// (If the dwFlag in MapValue is 0, it also returns 0)
	template <typename HttpTool>
	unsigned long CloHttpMapT<HttpTool>::GetFlag (unsigned long nIdx)
		throw ()
	{
		return Get (nIdx).dwFlag ;
	}

	// If the specified element is not found, returns MapValue of which szValue is NULL.
	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::MapValue
		CloHttpMapT<HttpTool>::Get (PCSZ szName, unsigned long nIdx)
		throw ()
	{
		MapValue mapValue = { NULL, 0 } ;
		if( szName == NULL )
			return mapValue;
		
		std::pair<MapIter, MapIter>	pairIter ;
		pairIter = m_map.equal_range (szName) ;

		// If the key is not found.
		if ( pairIter.first == pairIter.second )
			return mapValue ;

		MapIter iter = pairIter.first ;
		for (unsigned long i = 0; i < nIdx; i++) {
			++iter ;

			// If the nIdx is not valid
			if ( iter == pairIter.second )
				return mapValue ;
		}

		mapValue = iter->second ;
		if ( mapValue.szValue == NULL )
			mapValue.szValue = HttpTool::szEmptyString ;

		return mapValue ;
	}

	// Returns NULL if the specified element is not found.
	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::PCSZ
		CloHttpMapT<HttpTool>::GetValue (PCSZ szName, unsigned long nIdx)
		throw ()
	{
		return Get (szName, nIdx).szValue ;
	}

	// Returns 0 if the specified element is not found.
	// (If the dwFlag in MapValue is 0, it also returns 0)
	template <typename HttpTool>
	unsigned long CloHttpMapT<HttpTool>::GetFlag (PCSZ szName, unsigned long nIdx)
		throw ()
	{
		return Get (szName, nIdx).dwFlag ;
	}

	// Adds a new MapValue. It also receives the ownership of memory pointed by szName and szValue.
	// The szName and szValue must be allocated by using ::malloc.
	// The szValue could be NULL.
	template <typename HttpTool>
	void CloHttpMapT<HttpTool>::AddPointerDirectly (PSZ szName, PSZ szValue, BOOL dwFlag)
		throw ()
	{
		if( szName == NULL )
			return ;

		try 
		{
			// Checks the arithmetic overflow exception
			::SafeInt<MapSizeType>	cKeys = Count () ;
			::SafeInt<unsigned long>		cdwKeys = cKeys ;
			cKeys++ ;
			cdwKeys++ ;

			MapValue newValue = { (PCSZ) szValue, dwFlag } ;
			m_map.insert ( MapItem ( (PCSZ) szName, newValue) ) ;
		} 
		catch (...)
		{
			
		}
	}

	// The szValue could be NULL.
	template <typename HttpTool>
	void CloHttpMapT<HttpTool>::Add (PCSZ szName, PCSZ szValue, BOOL dwFlag)
		throw ()
	{
		if( szName == NULL )
			return ;

		PSZ					szNewName = NULL ;
		PSZ					szNewValue = NULL ;
		::SafeInt<size_t>	cbName, cbValue ;

		try 
		{
			cbName = HttpTool::StringLen (szName) ;
			cbName++ ;
			cbName *= sizeof (CharType) ;

			szNewName = (PSZ) ::malloc (cbName.Value ()) ;
			if ( szNewName == NULL )
			{
				return;
			}
			HttpTool::StringCopy (szNewName, szName) ;

			if ( szValue != NULL ) 
			{
				cbValue = HttpTool::StringLen (szValue) ;
				cbValue++ ;
				cbValue *= sizeof (CharType) ;

				szNewValue = (PSZ) ::malloc (cbValue.Value ()) ;
				if ( szNewValue == NULL )
				{
					SAFEFREE( szNewName );
					return ;
				}
				HttpTool::StringCopy (szNewValue, szValue) ;
			}
			AddPointerDirectly (szNewName, szNewValue, dwFlag) ;

		} 
		catch (...) 
		{
			SAFEFREE (szNewName) ;
			SAFEFREE (szNewValue) ;
		}
	}

	// If the specified element is not found, it will add a new value.
	// otherwise it will modifiy the existing value.
	// The szValue could be NULL.
	template <typename HttpTool>
	void CloHttpMapT<HttpTool>::Set (PCSZ szName, PCSZ szValue, BOOL dwFlag, unsigned long nIdx)
		throw ()
	{
		if( szName == NULL )
			return ;

		std::pair<MapIter, MapIter>	pairIter ;
		pairIter = m_map.equal_range (szName) ;

		// Adds a new value if the specified element is not found.
		if ( pairIter.first == pairIter.second ) {
			// The nIdx must be 0
#ifdef WIN32
			_ASSERTE ( nIdx == 0 ) ;
#endif //#ifdef WIN32
			Add (szName, szValue, dwFlag) ;
			return ;
		}

		MapIter iter = pairIter.first ;
		for (unsigned long i = 0; i < nIdx; i++) {
			++iter ;

			// The nIdx must be valid
#ifdef WIN32
			_ASSERTE ( iter != pairIter.second ) ;
#endif //#ifdef WIN32
		}

		MapValue newValue = { NULL, dwFlag } ;
		if ( szValue )
		{
			::SafeInt<size_t>		cbValue ;

			try 
			{
				cbValue = HttpTool::StringLen (szValue) ;
				cbValue++ ;
				cbValue *= sizeof (CharType) ;
			} catch (...)
			{
				return ;
			}

			newValue.szValue = (PCSZ) ::malloc (cbValue.Value ()) ;
			if ( newValue.szValue == NULL )
			{
				return ;
			}

			HttpTool::StringCopy ((PSZ) newValue.szValue, szValue) ;
		}

		MapValue oldValue = iter->second ;
		iter->second = newValue ;
		oldValue.Delete () ;
	}

	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::ConstMapIter
		CloHttpMapT<HttpTool>::Begin (void) const
		throw ()
	{
		return m_map.begin () ;
	}

	template <typename HttpTool>
	typename CloHttpMapT<HttpTool>::ConstMapIter
		CloHttpMapT<HttpTool>::End (void) const
		throw ()
	{
		return m_map.end () ;
	}
}

#pragma warning (pop)	// restores the default


#endif //__lo_HTTP_MAP_H__
