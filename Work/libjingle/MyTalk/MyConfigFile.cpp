#include "stdafx.h"
#include "MyConfigFile.h"
#include <assert.h>
#include <fstream>
using namespace std;

MyConfigFile::file_not_found_exception::file_not_found_exception( const string & filename_ )
{
	filename = filename_;
}

MyConfigFile::key_not_found::key_not_found( const string & key_ )
{
	key = key_;
}

MyConfigFile::MyConfigFile( string delimiter /*= "=" */, string comment /*= "#" */ )
{
	m_delimiter = delimiter;
	m_comment   = comment; 
}

MyConfigFile::~MyConfigFile()
{
    Clear();
}

void MyConfigFile::LoadFile( const string & filename )
{
    WCHAR wszStr[512] = {0};
    int nCount = MultiByteToWideChar( CP_ACP, 0, filename.c_str(), strlen(filename.c_str()), wszStr, 512 - 1 );
    wszStr[nCount] = L'\0';

    FILE * fp = 0;
    fp = _wfopen( wszStr, L"rb" );
    if ( 0 == fp )
    {
        throw file_not_found_exception( filename );
    }

    fseek( fp, 0, SEEK_END );
    int nPos = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    char * pData = new char[nPos];
    fread( pData, 1, nPos, fp );

    fclose( fp );

	LoadData( pData, nPos );
	delete[] pData;
}

void MyConfigFile::LoadData( void * pData, unsigned int nDataLen )
{
	if ( 0 == pData )
	{
		throw invalid_argument();
	}

	Clear();

	m_pData = pData;
	m_nDataLen = nDataLen;

	string line;
	typedef string::size_type pos;

	while( GetLine(line) )
	{
        trim( line );
        
        if ( 0 == line.length() )
        {
            PTMyCfgItem pItem = new TMyCfgItem;
            pItem->eType = MY_CFG_TYPE_BLANK;
            m_context.push_back( pItem );
        }
        else if ( line.length() >= m_comment.length() 
                  && line.substr(0, m_comment.length()) == m_comment )
        {
            PTMyCfgItem pItem = new TMyCfgItem;
            pItem->eType = MY_CFG_TYPE_COMMENT;
            pItem->sItem = line;
            m_context.push_back( pItem );
        }
        else
        {
            size_t i      = 0;
            bool   bQuote = false;

            string key;
            string value;
            string comment;

            int    nFlag = 0;  // 0: indicate key; 1: value; 2: comment

            while( i < line.length() )
            {
                size_t nRemainLen = line.length() - i;

                if ( '"' == line.at(i) )
                {
                    if ( !bQuote )
                    {
                        bQuote = true;
                        i++;
                    }
                    else
                    {
                        if ( nRemainLen >= 2 && line.substr(i, 2) == "\"\"" )
                        {
                            if ( 0 == nFlag )
                            {
                                key += '"';
                            }
                            else
                            {
                                assert( 1 == nFlag );
                                value += '"';
                            }
                            i += 2;
                        }
                        else
                        {
                            bQuote = false;
                            i++;
                        }
                    }
                }
                else if ( nRemainLen >= m_comment.length()
                          && line.substr(i, m_comment.length() ) == m_comment )
                {
                    if ( !bQuote )
                    {
                        if ( 1 == nFlag )
                        {
                            nFlag = 2;
                        }
                        break;
                    }
                    else
                    {
                        if ( 0 == nFlag )
                        {
                            key += m_comment;
                        }
                        else
                        {
                            assert( 1 == nFlag );
                            value += m_comment;
                        }
                        i += m_comment.length();
                    }
                }
                else if ( nRemainLen >= m_delimiter.length()
                          && line.substr(i, m_delimiter.length() ) == m_delimiter )
                {
                    if ( !bQuote )
                    {
                        if ( 0 == nFlag )
                        {
                            nFlag = 1;
                        }
                        else
                        {
                            assert( 1 == nFlag );
                            value += m_delimiter;
                        }
                    }
                    else
                    {
                        if ( 0 == nFlag )
                        {
                            key += m_delimiter;
                        }
                        else if ( 1 == nFlag )
                        {
                            value += m_delimiter;
                        }
                    }
                    i += m_delimiter.length();
                }
                else
                {
                    if ( 0 == nFlag )
                    {
                        key += line.at(i);
                    }
                    else
                    {
                        assert( 1 == nFlag );
                        value += line.at( i );
                    }
                    i++;
                }
            }

            if ( nFlag > 0 )
            {
                trim( key );
                trim( value );
                
                PTMyCfgItem pItem = new TMyCfgItem;
                pItem->eType = MY_CFG_TYPE_DATA;
                pItem->sItem = key;
                m_context.push_back( pItem );

                PTMyCfgData pData = new TMyCfgData;
                pData->sData = value;

                if ( 2 == nFlag )
                {
                    pData->sComment = line.substr( i + 1 );
                }

                m_contents[key] = pData; 
            }
        }
	}
}

int MyConfigFile::GetLine( string & line )
{
	if ( m_nDataLen > 0 )
	{
		line = "";
		char * pData = (char *)m_pData;
		assert( m_pData );

		unsigned int i;
		for ( i = 0; i < m_nDataLen; i++ )
		{
			if ( '\n' == pData[i] )
			{
				m_pData = pData + (i + 1);
				m_nDataLen -= i + 1;
				return 1;
			}
			else
			{
				line += pData[i];
			}
		}
		m_pData = NULL;
		m_nDataLen = 0;
		return 1;
	}
	else
	{
		line = "";
		return 0;
	}
}

// static
string & MyConfigFile::trim( string & s )
{
	// Remove leading and trailing whitespace
	static const char whitespace[] = " \n\t\r";
	s.erase( 0, s.find_first_not_of(whitespace) );
	s.erase( s.find_last_not_of(whitespace) + 1 );
    return s;
}

void MyConfigFile::Clear()
{
	vecContext::iterator q;
    for ( q = m_context.begin(); q != m_context.end(); q++ )
    {
        delete *q;
    }
    m_context.clear();

    mapContents::iterator p;
    for( p = m_contents.begin(); p != m_contents.end(); p++ )
    {
        delete p->second;
    }
    m_contents.clear();
}

// Check whether key exists in configuration
bool MyConfigFile::keyExists( const string & key ) const
{
    mapContents::const_iterator p = m_contents.find( key );
	return ( p != m_contents.end() );
}

// Check or change configuration syntax
string MyConfigFile::getDelimiter() const
{
	return m_delimiter;
}

string MyConfigFile::getComment() const
{
	return m_comment;
}


string MyConfigFile::setDelimiter( const string& s )
{
	string old = m_delimiter;
	m_delimiter = s;
	return old; 
}

string MyConfigFile::setComment( const string& s )
{
	string old = m_comment;
	m_comment = s;
	return old;
}

string MyConfigFile::read( const string & key ) const
{
    mapContents::const_iterator p = m_contents.find( key );
	if ( p == m_contents.end() )
	{
		return "";
	}
	else
	{
		return p->second->sData;
	}
}

string MyConfigFile::operator[] ( const string& key ) const
{
    return this->read( key );
}

void MyConfigFile::remove( const string & key )
{

    mapContents::iterator p = m_contents.find( key );
	if ( p != m_contents.end() )
	{
		m_contents.erase( p );
	}
	
	return;
}

#ifndef _SUPPORT_ADVANCED_TEMPLATE_

int MyConfigFile::readInt( const string & key ) const
{
    mapContents::const_iterator p = m_contents.find( key );
	if( p == m_contents.end() )
	{
		return 0;
	}
    
    PTMyCfgData pData = p->second;

    int t;
    std::istringstream ist(pData->sData);
	ist >> t;
	return t;
	return t;
}

double MyConfigFile::readDouble( const string & key ) const
{
    mapContents::const_iterator p = m_contents.find( key );
	if( p == m_contents.end() )
	{
		return 0.0;
	}
    
    PTMyCfgData pData = p->second;

    double t;
    std::istringstream ist(pData->sData);
	ist >> t;
	return t;
	return t;
}

void MyConfigFile::set( const string & key, const string & value )
{
    string sKey = key;
	trim(sKey);

	if ( 0 == sKey.length() )
	{
		return;
	}

    mapContents::const_iterator p = m_contents.find( sKey );
    if ( p == m_contents.end() )
    {
        PTMyCfgData pData = new TMyCfgData;
        pData->sData = value;
        m_contents[sKey] = pData;

        PTMyCfgItem pItem = new TMyCfgItem;
        pItem->eType = MY_CFG_TYPE_DATA;
        pItem->sItem = sKey;
        m_context.push_back( pItem );
    }
    else
    {
        PTMyCfgData pData = p->second;
        pData->sData = value;
    }
	return;
}

void MyConfigFile::setInt( const string & key, int value )
{
    string sKey = key;
	trim(sKey);

	if ( 0 == sKey.length() )
	{
		return;
	}

    std::ostringstream ost;
	ost << value;

    mapContents::const_iterator p = m_contents.find( sKey );
    if ( p == m_contents.end() )
    {
        PTMyCfgData pData = new TMyCfgData;
        pData->sData = ost.str();
        m_contents[sKey] = pData;

        PTMyCfgItem pItem = new TMyCfgItem;
        pItem->eType = MY_CFG_TYPE_DATA;
        pItem->sItem = sKey;
        m_context.push_back( pItem );
    }
    else
    {
        PTMyCfgData pData = p->second;
        pData->sData = ost.str();
    }
	return;
}

void MyConfigFile::setDouble( const string & key, double value )
{
    string sKey = key;
	trim(sKey);

	if ( 0 == sKey.length() )
	{
		return;
	}

    std::ostringstream ost;
	ost << value;

    mapContents::const_iterator p = m_contents.find( sKey );
    if ( p == m_contents.end() )
    {
        PTMyCfgData pData = new TMyCfgData;
        pData->sData = ost.str();
        m_contents[sKey] = pData;

        PTMyCfgItem pItem = new TMyCfgItem;
        pItem->eType = MY_CFG_TYPE_DATA;
        pItem->sItem = sKey;
        m_context.push_back( pItem );
    }
    else
    {
        PTMyCfgData pData = p->second;
        pData->sData = ost.str();
    }
	return;
}
#endif

void MyConfigFile::setDataComment( const string & key, const string & comment )
{
    string sKey = key;
	trim(sKey);

	if ( 0 == sKey.length() )
	{
		return;
	}

    mapContents::const_iterator p = m_contents.find( sKey );
    if ( p == m_contents.end() )
    {
        PTMyCfgData pData = new TMyCfgData;
        pData->sComment = comment;
        m_contents[sKey] = pData;

        PTMyCfgItem pItem = new TMyCfgItem;
        pItem->eType = MY_CFG_TYPE_DATA;
        pItem->sItem = sKey;
        m_context.push_back( pItem );
    }
    else
    {
        PTMyCfgData pData = p->second;
        pData->sComment = comment;
    }
	return;
}

string & MyConfigFile::replace( string & s, const string & m, const string & r )
{
    string::size_type i = 0;
    string::size_type n = 0;

    n = s.find( m, i );
    while( string::npos != n )
    {
        s.replace( n, n + m.length() , r );

        i = n + r.length();
        n = s.find( m, i );
    }

    return s;
}


string MyConfigFile::GetFileFormatStr( const string & s )
{
    string sNew = s;

    replace( sNew, "\"", "\"\"" );

    if ( string::npos != sNew.find( m_delimiter ) )
    {
        return "\"" + sNew + "\"";
    }
    else if ( string::npos != sNew.find( m_comment) )
    {
        return "\"" + sNew + "\"";
    }
    else
    {
        return sNew;    
    }
}

void MyConfigFile::WriteItem2File( FILE * fp, PTMyCfgItem pItem, 
                                  unsigned int nMinKeyWidth,
                                  unsigned int nMinKeyDataWidth )
{
    string sLine;

    if ( pItem->eType == MY_CFG_TYPE_BLANK )
    {
        // fs.write( "\r\n", 2 );
        fwrite( "\r\n", 1, 2, fp );
    }
    else if ( pItem->eType == MY_CFG_TYPE_COMMENT )
    {
        sLine = pItem->sItem;
        sLine += "\r\n";
        // fs.write( sLine.c_str(), sLine.length() );
        fwrite( sLine.c_str(), 1, sLine.length(), fp );
    }
    else
    {
		mapContents::const_iterator p = m_contents.find( pItem->sItem );
		if ( p != m_contents.end() )
		{
			PTMyCfgData pData = p->second;

            sLine = GetFileFormatStr(pItem->sItem);

			int nBlankNum = (int)nMinKeyWidth - (int)sLine.length();

			if ( nBlankNum > 0 )
			{
				for ( int i = 0; i < nBlankNum; i++ )
				{
					sLine += ' ';
				}
			}

            sLine += ' ';

            sLine += m_delimiter;

            sLine += ' ';

			sLine += GetFileFormatStr(pData->sData);

            if ( pData->sComment.length() > 0 )
            {
                nBlankNum = (int)nMinKeyDataWidth - (int)sLine.length();
                if ( nBlankNum > 0 )
			    {
				    for ( int i = 0; i < nBlankNum; i++ )
				    {
					    sLine += ' ';
				    }
			    }

                sLine += ' ';
                sLine += m_comment;
                sLine += pData->sComment;
            }
            

            sLine += "\r\n";

			// fs.write( sLine.c_str(), sLine.length() );
            fwrite( sLine.c_str(), 1, sLine.length(), fp );
		}
    }
}

bool MyConfigFile::SaveFile( const string & filename, vector<string> * pvItems /* = 0 */,
							 unsigned int nMinKeyWidth /* = 16 */,
                             unsigned int nMinKeyDataWidth /* = 40 */  )
{
	//ofstream fs;
	// fs.open( filename.c_str(), ios_base::out | ios_base::binary );

	//if ( !fs )
	//{
	//	return false;
	//}
    
    WCHAR wszStr[512] = {0};
    int nCount = MultiByteToWideChar( CP_ACP, 0, filename.c_str(), strlen(filename.c_str()), wszStr, 512 - 1 );
    wszStr[nCount] = L'\0';

    FILE * fp = _wfopen( wszStr, L"wb" );
    if ( 0 == fp )
    {
        return false;
    }

	if ( 0 == pvItems )
	{
        vecContext::const_iterator y;
        for ( y = m_context.begin(); y != m_context.end(); y++ )
        {
            WriteItem2File( fp, *y, nMinKeyWidth, nMinKeyDataWidth );
        }
	}
	else
	{
        vector<string>::const_iterator q;
		for ( q = pvItems->begin(); q != pvItems->end(); q++ )
		{
			const string & sItem = *q;

            string sLine = sItem;
            trim( sLine );

            TMyCfgItem item;

            if ( 0 == sLine.length() )
            {
                item.eType = MY_CFG_TYPE_BLANK;
            }
            else if ( m_comment == sLine.substr(0, m_comment.length()) )
			{
                item.eType = MY_CFG_TYPE_COMMENT;
                item.sItem = sLine;
			}
			else
			{
                item.eType = MY_CFG_TYPE_DATA;
                item.sItem = sLine;
			}

            WriteItem2File( fp, &item, nMinKeyWidth, nMinKeyDataWidth );
		}
	}

	// fs.close();
    fclose( fp );

	return true;
}


