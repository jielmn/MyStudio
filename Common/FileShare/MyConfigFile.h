#ifndef __MY_CONFIG_FILE_2010_08_13__
#define __MY_CONFIG_FILE_2010_08_13__

#ifdef WIN32
#pragma warning(disable: 4786)
#endif

// Typical usage
// -------------
// 
// Given a configuration file "GameSetting.config":
//   num  = 25
//   length = 1.23  # double
//   name = Cristiano Ronaldo
//  #configuration must be in a line
// 
// Named values are read in various ways, with or without default values:
//   MyConfigFile config( "GameSetting.config" );
//   int num = config.read<int>( "num" );
//   double length = config.read( "length");
//   string s = config.read("name");

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>


//#define _SUPPORT_ADVANCED_TEMPLATE_


using namespace std;

class MyConfigFile
{
public:
    enum eMyCfgType
    {
        MY_CFG_TYPE_DATA,
        MY_CFG_TYPE_BLANK,
        MY_CFG_TYPE_COMMENT
    };

    typedef struct tagTMyCfgItem
    {
        eMyCfgType    eType;
        string        sItem;             // 写在上方的注释或关键字(据此在m_commets内查找数据)
                                         // 由类型决定
        tagTMyCfgItem()
        {
            eType = MY_CFG_TYPE_DATA;
        }
    }TMyCfgItem, *PTMyCfgItem;

    typedef vector< PTMyCfgItem > vecContext;
    
private:
	string m_delimiter;  // separator between key and value
	string m_comment;    // separator between value and comments

    vecContext m_context;     // 保留原文的上下文

    typedef struct tagTMyCfgData
    {
        string       sData;
        string       sComment;            // 写在右方的注释
    }TMyCfgData, *PTMyCfgData;

    typedef map<string, PTMyCfgData> mapContents;

	mapContents m_contents;  // extracted keys and values

	void * m_pData;     // used when parse data
	unsigned int m_nDataLen; // used when parse data

private:
	int GetLine( string & line );
	static string & trim( string & s );

#ifdef _SUPPORT_ADVANCED_TEMPLATE_
	template<class T> static T string_as_T( const string & s );
	template<class T> static string T_as_string( const T & t );
#endif

    static string & replace( string & s, const string & m, const string & r );
    string GetFileFormatStr( const string & s );
    void WriteItem2File( ofstream & fs, PTMyCfgItem pItem, 
                         unsigned int nMinKeyWidth,
                         unsigned int nMinKeyDataWidth );

public:
	MyConfigFile( string delimiter = "=", string comment = "#" );
	~MyConfigFile();

	void LoadFile( const string & filename );
	void LoadData( void * pData, unsigned int nDataLen );

	void Clear();

	// Check whether key exists in configuration
	bool keyExists( const string & key ) const;

	// Check or change configuration syntax
	string getDelimiter() const;
	string getComment() const;
	string setDelimiter( const string & s );
	string setComment( const string & s );

	string read( const string& key ) const;
    string operator[] ( const string& key ) const;
    string operator[] ( const char * key ) const;
   
#ifdef _SUPPORT_ADVANCED_TEMPLATE_
    // no supported by vc6
	template<class T> T read( const string& key ) const;  // call as read<T>
	// Modify keys and values
	template<class T> void set( const string & key, const T & value );
#else
    int readInt( const string & key ) const;
    double readDouble( const string & key ) const;
    void set( const string & key, const string & value );
    void setInt( const string & key, int value );
    void setDouble( const string & key, double value );
#endif

    void setDataComment( const string & key, const string & comment );

	void remove( const string& key );

	// parameter pvKey, for example, "# this is a comment", "abc", "xyz", ""(blank) ...
	bool SaveFile( const string & filename, vector<string> * pvItems = 0,
		           unsigned int nMinKeyWidth = 12, unsigned int nMinKeyDataWidth = 30 );

public:
	struct file_not_found_exception
	{
		string filename;
		file_not_found_exception( const string & filename_ );
	};

	struct key_not_found
	{
		string key;
		key_not_found( const string & key_ );
	};

	struct invalid_argument
	{

	};
};

#ifdef _SUPPORT_ADVANCED_TEMPLATE_
/* static */
template<class T>
string MyConfigFile::T_as_string( const T & t )
{
	// Convert from a T to a string
	// Type T must support << operator
	std::ostringstream ost;
	ost << t;
	return ost.str();
}

/* static */
template<class T>
T MyConfigFile::string_as_T( const string & s )
{
	// Convert from a string to a T
	// Type T must support >> operator
	T t;
	std::istringstream ist(s);
	ist >> t;
	return t;
}

template<class T>
T MyConfigFile::read( const string& key ) const
{
	// Read the value corresponding to key
	
    mapContents::const_iterator p = m_contents.find( key );
	if( p == m_contents.end() )
	{
		throw key_not_found(key);
	}
    
    PTMyCfgData pData = p->second;
	return string_as_T<T>( pData->sData );
}


template <class T>
void MyConfigFile::set( const string & key, const T & value )
{
    string sKey = key;
	trim(sKey);

	if ( 0 == sKey.length() )
	{
		return;
	}

	// Add a key with given value
	string v = T_as_string( value );

    mapContents::const_iterator p = m_contents.find( sKey );
    if ( p == m_contents.end() )
    {
        PTMyCfgData pData = new TMyCfgData;
        pData->sData = v;
        m_contents[key] = pData;
    }
    else
    {
        PTMyCfgData pData = p->second;
        pData->sData = v;
    }
	return;
}

#endif // end _SUPPORT_ADVANCED_TEMPLATE_

#endif

