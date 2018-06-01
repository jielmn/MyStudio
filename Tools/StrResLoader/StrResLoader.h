#ifndef __STRING_RESOURCE_LOADER_2010_08_13__
#define __STRING_RESOURCE_LOADER_2010_08_13__

#ifdef WIN32
#pragma warning ( disable: 4786 )
#endif

// typical usage
//  load string resource from a file
//
// 001   "hello"
// 002   "world"
// 003   "!"
// 004   "say \"something\"! "
//
#include <string>
#include <map>

using namespace std;

class MyStrResLoader
{
private:
	typedef map<unsigned int, string> StrMap;
	StrMap  m_strings;

public:
	MyStrResLoader( );
	~MyStrResLoader( );

	bool LoadFile( const string & filename );
	void Clear();

	const char * operator[]( unsigned int nKey ) const;
};


#endif

