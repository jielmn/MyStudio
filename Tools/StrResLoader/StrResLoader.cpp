#ifdef WIN32
#pragma warning ( disable: 4786 )
#endif

#include <fstream>
#ifdef WIN32
#include <windows.h>
#endif

#include "StrResLoader.h"



using namespace std;

#define BUFF_MAX_SIZE          4096

#ifdef WIN32
#pragma warning(disable: 4996)
#endif

static char * _strtrim( char * str )
{
	int nLen = strlen( str );

	int i, j;

	for ( i = 0; i < nLen; i++ )
	{
		if ( ' ' == str[i] || '\t' == str[i] || '\r' == str[i] || '\n' == str[i] )
		{
			continue;
		}
		else
		{
			break;
		}
	}

	for ( j = nLen - 1; j >= i; j-- )
	{
		if ( ' ' == str[j] || '\t' == str[j] || '\r' == str[j] || '\n' == str[j] )
		{
			continue;
		}
		else
		{
			break;
		}
	}

	int nLen2 = j - i + 1;

	char * pTmp = new char[nLen2 + 1];
	strncpy( pTmp, str + i, nLen2 );
	pTmp[nLen2] = '\0';

	strcpy( str, pTmp );

	delete[] pTmp;

	return str;
}

static char * _streplace( char * szDest, unsigned int nSize, const char * szSource, 
				  const char * szReplaced, const char * szReplaceWith )
{
	const char * p = NULL;
	const char * q = NULL;

	char * m           = szDest;
	unsigned int nLeft = nSize - 1;

	unsigned int a = strlen( szReplaced );
	unsigned int b = strlen( szReplaceWith );

	p = szSource;
	q = strstr( p, szReplaced );
	while( q )
	{
		unsigned int n = q - p;
		if ( n > nLeft )
		{
			strncpy( m, p, nLeft );
			m[nLeft] = '\0';
			nLeft = 0;
			break;
		}
		else
		{
			strncpy( m, p, n );
			m += n;
			nLeft -= n;

			if ( b > nLeft )
			{
				strncpy( m, szReplaceWith, nLeft );
				m[nLeft] = '\0';
				nLeft = 0;
				break;
			}
			else
			{
				strncpy( m, szReplaceWith, b );
				m += b;
				nLeft -= b;

				p = q + a;
				q = strstr( p, szReplaced );
			}
		}
	}

	if ( 0 == q )
	{
		unsigned int c = strlen( szSource );
		if ( szSource + c - p > (int)nLeft )
		{
			strncpy( m, p, nLeft );
			m[nLeft] = '\0';
		}
		else
		{
			strcpy( m, p );
		}
	}

	return szDest;
}

static  FILE * LmnOpenFile ( const char * szFile, const char * szMode )
{
    FILE * fp = 0;

#ifdef WIN32
    WCHAR   wszFile[260] = { 0 };
    WCHAR   wszMode[64]  = { 0 };

    MultiByteToWideChar( CP_ACP,  0, szFile, strlen(szFile),  wszFile, 260 - 1 );
    MultiByteToWideChar( CP_ACP,  0, szMode, strlen(szMode),  wszMode, 64 - 1 );

    fp = _wfopen( wszFile, wszMode );
#else
    fp = fopen( szFile, szMode );
#endif

    return fp;
}


MyStrResLoader::MyStrResLoader( )
{

}

MyStrResLoader::~MyStrResLoader( )
{
	Clear();
}

bool MyStrResLoader::LoadFile( const string & filename )
{
    FILE * fp = LmnOpenFile( filename.c_str(), "rb" );
    if ( fp == 0 )
    {
        return false;
    }


	char  buff[BUFF_MAX_SIZE];
	char  str_value[BUFF_MAX_SIZE];
	char  rep_value1[BUFF_MAX_SIZE];
    char  rep_value2[BUFF_MAX_SIZE];


    while( !feof(fp) )
	{
        fgets( buff, BUFF_MAX_SIZE, fp );
		_strtrim( buff );

		unsigned int nLen = strlen( buff );
        // 如果长度为0
		if ( 0 == nLen )
		{
			continue;
		}

		unsigned int nId;
        // 如不不是数字
		if ( 0 == sscanf( buff, " %d", &nId ) )
		{
			continue;
		}

		char * p = strchr( buff, '"' );
        // 如果没有前引号
        if ( 0 == p )
        {
            continue;
        }

        char * q = strrchr( buff, '"' );
        // 如果没有后引号
        if ( 0 == q )
        {
            continue;
        }

        // 如果其实前引号和后引号是一个引号
        if ( p == q )
        {
            continue;
        }

        // 计算长度
        int n = q - (p + 1);
        strncpy( str_value, p + 1, n );
        str_value[n] = '\0';

        // 解析转义引号(\")
        _streplace( rep_value1, BUFF_MAX_SIZE, str_value, "\\\"", "\"" );
        // 解析转义TAB(\t)
        _streplace( rep_value2, BUFF_MAX_SIZE, rep_value1, "\\t",  "\t" );
        n = strlen( rep_value2 );

        char * y = new char[n + 1];
        if ( 0 == y )
        {
            fgets( buff, BUFF_MAX_SIZE, fp );
            continue;
        }

        strcpy( y, rep_value2 );
        m_strings[nId] = y;
        delete[] y;
	}

	// fs.close();
    fclose( fp );

	return true;
}

void MyStrResLoader::Clear()
{
	m_strings.clear();
}

const char * MyStrResLoader::operator[]( unsigned int nKey ) const
{
	StrMap::const_iterator it;
	it = m_strings.find( nKey );

	if ( it == m_strings.end() )
	{
		return "";
	}
	else
	{
		return (it->second).c_str();
	}
}

