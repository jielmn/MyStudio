#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <assert.h>

#include "MergeFolder.h"
#include "MyContainer.h"
#include "MyMisc.h"

using namespace std;

#define  NORMAL_SIZE               512
#define  FILE_BUFF_SIZE            0x800000
#define  DOUBLE_FILE_BUFF_SIZE     ( FILE_BUFF_SIZE * 2 )
#define  MAX_SUBFIX_SIZE           32


#ifdef UNICODE

#define STRLEN(s)                  wcslen(s)
#define STRCPY(d, s)               wcscpy(d, s)
#define STRCHR(s, c)               wcschr(s, c)
#define STRCMP(a,b)                wcscmp(a, b)
#define STRNCPY(a, b, n)           wcsncpy(a, b, n)
#define STRCAT(a, b)               wcscat(a, b)
#define STRICMP(a, b)              _wcsicmp(a, b)
#define SPRINTF                    wsprintf
#define SSCANF                     swscanf
#define SNPRINTF                   _snwprintf

#else

#define STRLEN(s)                  strlen(s)
#define STRCPY(d, s)               strcpy(d, s)
#define STRCHR(s, c)               strchr(s, c)
#define STRCMP(a,b)                strcmp(a, b)
#define STRNCPY(a, b, n)           strncpy(a, b, n)
#define STRCAT(a, b)               strcat(a, b)
#define STRICMP(a, b)              _stricmp(a, b)
#define SPRINTF                    sprintf
#define SSCANF                     sscanf
#define SNPRINTF                   _snprintf

#endif

#pragma warning( disable: 4996 )

// szFolder以'\\'结尾
int  _GetFolderSpaceInDisk ( const char * szFolder )
{
    TCHAR             szWildCast[NORMAL_SIZE];
    int nLen = strlen( szFolder );

    if ( '\\' == szFolder[nLen-1] || '/' == szFolder[nLen-1] )
    {
	    SNPRINTF( szWildCast, NORMAL_SIZE, _T("%s*.*"), szFolder );
    }
    else
    {
        SNPRINTF( szWildCast, NORMAL_SIZE, _T("%s\\*.*"), szFolder );
    }

    WIN32_FIND_DATA  FindData;

	HANDLE hFind  = ::FindFirstFile( szWildCast, &FindData );
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return -1;
	}
	
    int   nTotalSize = 0;
	while( ::FindNextFile(hFind, &FindData) )
	{
		if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
            if ( 0 == STRCMP( _T(".."), FindData.cFileName ) )
			{
				continue;
			}
			else if ( 0 == STRCMP( _T("."), FindData.cFileName ) )
			{
				continue;
			}

            TCHAR             szTmp[NORMAL_SIZE];
            SNPRINTF( szTmp, NORMAL_SIZE, "%s\\%s\\", szFolder, FindData.cFileName );
            
            nTotalSize += _GetFolderSpaceInDisk( szTmp );
		}
        else
        {
            nTotalSize += FindData.nFileSizeLow;
        }
	}
	FindClose( hFind );
    return nTotalSize;
}

// convert char * to TSTR
LPTSTR _MyA2T( LPTSTR pDest, size_t nSize, LPCSTR szSrc  )
{
	int nLen  = strlen(szSrc) + 1;

#ifdef UNICODE

	int nwLen = MultiByteToWideChar( CP_ACP, 0, szSrc, nLen, NULL, 0 );

	if ( (int)nSize < nwLen )
	{
		return NULL;
	}

	MultiByteToWideChar( CP_ACP, 0, szSrc, nLen, pDest, nSize ); 
	return pDest;

#else

	if ( (int)nSize < nLen )
	{
		return NULL;
	}

	strcpy( pDest, szSrc );

	return pDest;

#endif

}

// convert TSTR to char *
LPSTR  _MyT2A( LPSTR pDest,  size_t nSize, LPCTSTR szSrc )
{

#ifdef UNICODE
	int nLen = wcslen(szSrc) + 1;

	if ( (int)nSize < 2 * nLen )
	{
		return NULL;
	}

	WideCharToMultiByte( CP_ACP, 0, szSrc, nLen, pDest, nSize, NULL, NULL );

	return pDest;
#else

	int nLen = strlen( szSrc ) + 1;

	if ( (int)nSize < nLen )
	{
		return NULL;
	}
	strcpy( pDest, szSrc );

	return pDest;
#endif
}


void MfeInit()
{
	InitThrdDataSys();
}

void MfeDeinit()
{
	DeinitThrdDataSys();
}

int _WriteName( LPCTSTR szName, ofstream & fs )
{
	char sName[NORMAL_SIZE];
	_MyT2A( sName, NORMAL_SIZE, szName );

	int n = strlen( sName );
	fs.write( (char *)&n, sizeof(int) );

	fs.write( sName, n );

	return n + sizeof( int );
}

LPCTSTR _GetFileSubfix( LPCTSTR szFilePath )
{
	int n = STRLEN( szFilePath );
	int i;
	for ( i = n - 1; i >=0; i-- )
	{
		if ( _T('.') == szFilePath[i] )
		{
			break;
		}
	}

	if ( i < 0 )
	{
		return _T("");
	}
	else
	{
		return szFilePath + i + 1;
	}
};

int _WriteFile( LPCTSTR szPath, ofstream & fs, PFunBuffCompOrUncomp pfn, int & nFinishedSize )
{
	int nSize = 0;
	int nOriginalSize = 0;
	
	ifstream ifs;
	ifs.open( szPath, ios::in | ios::binary );

	// write file size temporarily
	int nCurPos = fs.tellp();
	fs.write( (char *)&nSize, sizeof(int) );
	// fill the original file size temporarily
	fs.write( (char *)&nOriginalSize, sizeof(int) );
	nSize += sizeof(int);
	
	BYTE * buff = new BYTE[FILE_BUFF_SIZE];
	
	BYTE * comp_buff = NULL;
	if ( NULL != pfn )
	{
		comp_buff = new BYTE[DOUBLE_FILE_BUFF_SIZE];
	}

	do 
	{
		ifs.read( (char *)buff, FILE_BUFF_SIZE );
		int cnt = ifs.gcount();
		nOriginalSize += cnt;
		
		if ( cnt > 0 )
		{
			if ( NULL != pfn )
			{
				int nCmpCnt = pfn( buff, cnt, comp_buff, DOUBLE_FILE_BUFF_SIZE );

				fs.write( (char *)&nCmpCnt, sizeof(int) );
				fs.write( (char *)comp_buff, nCmpCnt );

				nSize += sizeof(int) + nCmpCnt;
			}
			else
			{
				fs.write( (char *)&cnt, sizeof(int) );
				fs.write( (char *)buff, cnt );

				nSize += sizeof(int) + cnt;
			}

            // 总共完成多少
            nFinishedSize += cnt;
            SetThrdData( 2, (void *)nFinishedSize );
		}
		
	} while ( !ifs.fail() );
	
	delete[] buff;
	if ( NULL != comp_buff )
	{
		delete[] comp_buff;
	}

	ifs.close();
	
	fs.seekp( nCurPos, ios::beg );
	fs.write( (char *)&nSize, sizeof(int) );
	fs.write( (char *)&nOriginalSize, sizeof(int) );
	fs.seekp( 0, ios::end );

	return sizeof(int) + nSize;
}

// nFinishedSize: 完成大小
int _MfeMerge( const TCHAR * szFolder, DWORD dwDepth, ofstream & fs, PFunBuffCompOrUncomp pfn, int & nFinishedSize )
{
	TCHAR             szDir[NORMAL_SIZE], szWildCast[NORMAL_SIZE];
	TCHAR *           pName = NULL;
	HANDLE            hFind;   
	WIN32_FIND_DATA   FindData;

	vector<TCHAR *>   vSubFolders;
	vector<TCHAR *>   vFiles;

	DWORD nLen = STRLEN( szFolder );
	if ( _T('\\') == szFolder[nLen-1] || _T('/') == szFolder[nLen-1] )
	{
		STRCPY( szDir, szFolder );
	}
	else
	{
		SNPRINTF( szDir, NORMAL_SIZE, _T("%s\\"), szFolder );
	}

	SNPRINTF( szWildCast, NORMAL_SIZE, _T("%s*.*"), szDir );
	
	// write the total size including the sub-directories and files
	int nCurPos = fs.tellp();
	int nFolderSize = 0;
	fs.write( (char *)&nFolderSize, sizeof(int) );


	hFind  = ::FindFirstFile( szWildCast, &FindData );
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return 0;
	}
	
	while( ::FindNextFile(hFind, &FindData) )
	{
		if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if ( 0 == STRCMP( _T(".."), FindData.cFileName ) )
			{
				continue;
			}
			else if ( 0 == STRCMP( _T("."), FindData.cFileName ) )
			{
				continue;
			}

			pName = new TCHAR[MAX_PATH];
			STRCPY( pName, FindData.cFileName );
			vSubFolders.push_back( pName );
		}
		else
		{
			pName = new TCHAR[MAX_PATH];
			STRCPY( pName, FindData.cFileName );
			vFiles.push_back( pName );
		}
	}
	FindClose( hFind );

	// write files count
	int nFileCnt = vFiles.size();
	fs.write( (char *)&nFileCnt, sizeof(int) );

	nFolderSize += sizeof( int );

	vector<TCHAR *>::iterator it;

	for ( it = vFiles.begin(); it != vFiles.end(); it++ )
	{
		pName = *it;

		//OutputDebugStringA("== file: ");
		//OutputDebugString( pName );
		//OutputDebugStringA( "\n" );
		
		// write file name
		nFolderSize += _WriteName( pName, fs );
		
		// get full path of file
		TCHAR szFilePath[NORMAL_SIZE];
		SNPRINTF( szFilePath, NORMAL_SIZE, _T("%s%s"), szDir, pName );

		int nFileSize = _WriteFile( szFilePath, fs, pfn, nFinishedSize );
		nFolderSize += nFileSize;

		delete[] pName;
	}
	vFiles.clear();


	// write sub-folders count
	int nSubFoldersCnt = vSubFolders.size();
	fs.write( (char *)&nSubFoldersCnt, sizeof(int) );

	nFolderSize += sizeof( int );

	for ( it = vSubFolders.begin(); it != vSubFolders.end(); it++ )
	{
		pName = *it;

		//OutputDebugStringA("== directory: ");
		//OutputDebugString( pName );
		//OutputDebugStringA( "\n" );

		// write sub-folder name
		nFolderSize += _WriteName( pName, fs );
		
		// get full path of sub-directory
		TCHAR szSubFolder[NORMAL_SIZE];
		SNPRINTF( szSubFolder, NORMAL_SIZE, _T("%s%s"), szDir, pName );

		int nSubFolderSize = _MfeMerge( szSubFolder, dwDepth + 1, fs, pfn, nFinishedSize );
		nFolderSize += nSubFolderSize;

		delete[] pName;
	}
	vSubFolders.clear();

	fs.seekp( nCurPos, ios::beg );
	fs.write( (char *)&nFolderSize, sizeof(int) );
	fs.seekp( 0, ios::end );

	return sizeof(int) + nFolderSize;
}

BOOL _IsDriver( LPCTSTR pPath )
{
	DWORD dwLen = STRLEN( pPath );
	DWORD i;
	for ( i = 0; i < dwLen; i++ )
	{
		if ( _T(':') == pPath[i] )
		{
			return TRUE;
		}
	}

	return FALSE;
}

LPTSTR _GetOutputFileName( LPCTSTR pPath, LPTSTR pName, DWORD dwSize )
{
	DWORD dwLen = STRLEN( pPath );
	DWORD dwLimit = 0;

	if ( _T('\\') == pPath[dwLen - 1] || _T('/') == pPath[dwLen - 1] )
	{
		dwLimit = dwLen - 1;
	}
	else
	{
		dwLimit = dwLen;
	}

	STRNCPY( pName, pPath, dwLimit );
	pName[ dwLimit ] = _T('\0');
	STRCAT( pName, _T(".mgf") );

	return pName;
}

DWORD GetMergeProgress( DWORD dwThrdId )
{
    void * pTotal;
    void * pFinished;

    int a = GetThrdData( dwThrdId, 1, &pTotal );
    int b = GetThrdData( dwThrdId, 2, &pFinished );
    if ( GetThrdData( dwThrdId, 1, &pTotal ) && GetThrdData( dwThrdId, 2, &pFinished ) )
    {
        int nTotal = (int)pTotal;
        int nFinished = (int)pFinished;

        // TRACE("######### total = %d, finished = %d\n", nTotal, nFinished );

        assert( nFinished <= nTotal );

        if ( 0 == nTotal )
        {
            return 100;
        }
        else
        {
            return (DWORD)( nFinished * 100.0 / nTotal );
        }
    }
    return 0;
}

DWORD MfeMerge( const TCHAR * szFolder, PFunBuffCompOrUncomp pfn /* = NULL */ )
{
	if ( NULL == szFolder )
	{
		return MFE_NULL_PARAM;
	}

	DWORD dwAttributes = GetFileAttributes( szFolder );
	if ( -1 == dwAttributes )
	{
		return MFE_NOT_DIRECTORY;
	}

	if ( !(FILE_ATTRIBUTE_DIRECTORY & dwAttributes) )
	{
		return MFE_NOT_DIRECTORY;
	}

	if ( _IsDriver(szFolder) )
	{
		MFE_DRIVER;
	}
	
	TCHAR szOutputName[NORMAL_SIZE];
	_GetOutputFileName( szFolder, szOutputName, NORMAL_SIZE );

    // 目录总大小
    int nTotalSize = _GetFolderSpaceInDisk( szFolder );
    // 太大了
    if ( nTotalSize < 0 )
    {   
        return MFE_TOO_MUCH;
    }
    SetThrdData( 1,  (void *)nTotalSize );
   
	ofstream fs;
	fs.open( szOutputName, ios::out | ios::binary );

    int nFinishedSize = 0;
    SetThrdData( 2, (void *)nFinishedSize );

	_MfeMerge( szFolder, 0, fs, pfn, nFinishedSize );

	fs.close();
	return MFE_SUCCESS;
}

typedef struct tagMfe_
{
	ifstream  fs;
	PTree     pFolderTree;
}Mfe_, *PMfe_;

#define MFE_NODE_TYPE_FOLDER   0
#define MFE_NODE_TYPE_FILE     1

typedef struct tagMfeNode_
{
	int          nType;   // 0: folder, 1: file
	LPTSTR       pName;
	ifstream *   pfs;
}MfeNode_, *PMfeNode_;

typedef struct tagMfeFolder_ : MfeNode_
{
	int   nFileCnt;
	int   nSubFolderCnt;
}MfeFolder_, *PMfeFolder_;

typedef struct tagMfeFile_ : MfeNode_
{
	int   nPos;
	int   nLen;
	int   nSkipLen;
}MfeFile_, *PMfeFile_;

LPTSTR _ReadName( ifstream & fs )
{
	int nLen = 0;
	fs.read( (char *)&nLen, sizeof(int) );

	char * pName = new char[nLen + 1];
	fs.read( pName, nLen );

	pName[nLen] = '\0';

#ifdef UNICODE
	TCHAR sName = new TCHAR[nLen + 1];
	_MyA2T( sName, nLen + 1, pName );
	delete[] pName;
	return sName;
#else
	return pName;
#endif

}

void _MfeOpenFolder( ifstream & fs, PTreeNode pTreeNode )
{
	PMfeFolder_ pFolder_ = (PMfeFolder_)pTreeNode->pData;

	int nSize = 0;
	fs.read( (char *)&nSize, sizeof(int) );

	int nFilesCnt = 0;
	fs.read( (char *)&nFilesCnt, sizeof(int) );

	pFolder_->nFileCnt = nFilesCnt;

	int i;
	for ( i = 0; i < nFilesCnt; i++ )
	{
		PMfeFile_ pFile_ = new MfeFile_;
		
		pFile_->nType = MFE_NODE_TYPE_FILE;
		pFile_->pName = _ReadName( fs );
		pFile_->pfs   = &fs;
		
		int nSkipLen = 0;
		int nFileLen = 0;

		fs.read( (char *)&nSkipLen, sizeof(int) );
		fs.read( (char *)&nFileLen, sizeof(int) );
		
		pFile_->nSkipLen = nSkipLen - sizeof( int );
		pFile_->nLen = nFileLen;
		pFile_->nPos = fs.tellg();

		Insert2Tree( pTreeNode, LAST_TREE_NODE, pFile_ );

		// skip file content
		assert ( nSkipLen - sizeof(int) >= 0 ) ;
		fs.seekg( (nSkipLen - sizeof(int)), ios::cur );
	}

	int nSubFoldersCnt = 0;
	fs.read( (char *)&nSubFoldersCnt, sizeof(int) );

	pFolder_->nSubFolderCnt = nSubFoldersCnt;

	for ( i = 0; i < nSubFoldersCnt; i++ )
	{
		PMfeFolder_ pFolder_ = new MfeFolder_;
		
		pFolder_->nType = MFE_NODE_TYPE_FOLDER;
		pFolder_->pName = _ReadName( fs );
		pFolder_->pfs   = &fs;
		
		PTreeNode pSubNode = Insert2Tree( pTreeNode, LAST_TREE_NODE, pFolder_ );
		_MfeOpenFolder( fs, pSubNode );
	}
}

PMfe MfeOpen( LPCTSTR szMfeFile )
{
	if ( NULL == szMfeFile )
	{
		return NULL;
	}

	PMfe_ pMfe_ = new Mfe_;
	
	pMfe_->fs.open( szMfeFile, ios::in | ios::binary );
	// open file failed;
	if ( !pMfe_->fs.good() )
	{
		delete pMfe_;
		return NULL;
	}
	
	pMfe_->pFolderTree = InitTree();
	PTree pTree = pMfe_->pFolderTree;

	// root folder
	PMfeFolder_ pFolder_ = new MfeFolder_;
	pFolder_->nType = MFE_NODE_TYPE_FOLDER;
	pFolder_->pName = NULL;
	pFolder_->pfs   = &pMfe_->fs;

	// root node
	PTreeNode pRoot = Insert2TreeRoot( pTree, pFolder_ );
	
	_MfeOpenFolder( pMfe_->fs, pRoot );

#if 0
	PTreeNode p = pRoot;
	while( p )
	{
		PMfeNode_ q = (PMfeNode_)p->pData;
		TCHAR buff[256];
		SNPRINTF( buff, 256, _T("==[%d:%s]\n"), q->nType, q->pName );
		OutputDebugString( buff );
		p = GetTreeNextNode( p );
	}
#endif


	return (PMfe)pMfe_;
}

void MfeClose( PMfe pMfe )
{
	if ( NULL == pMfe )
	{
		return;
	}

	PMfe_ pMfe_ = (PMfe_)pMfe;

	pMfe_->fs.close();

	PTree pTree = pMfe_->pFolderTree;

	PTreeNode p = GetTreeRoot( pTree );
	while ( p )
	{
		PMfeNode_ pNode_ = (PMfeNode_)p->pData;
		if ( pNode_->pName )
		{
			delete[] pNode_->pName;
		}
		delete p->pData;
		p = GetTreeNextNode( p );
	}

	DeinitTree( pTree );

	delete pMfe_;
}

PMfeFolder MfeGetRootFolder( PMfe pMfe )
{
	if ( NULL == pMfe )
	{
		return NULL;
	}

	PMfe_ pMfe_ = (PMfe_)pMfe;

	PTree pTree = pMfe_->pFolderTree;

	PTreeNode pNode = GetTreeRoot( pTree );
	assert( pNode );

	return (PMfeFolder)pNode;
}

PTreeNode _MfeGetSpecifiedNode( PTreeNode pNode, LPCTSTR szPath, int nType )
{
	TCHAR szName[NORMAL_SIZE];
	PTreeNode pChild = NULL;
	
	unsigned int i = 0;
	unsigned int j = 0;
	while( szPath[i] )
	{
		if ( _T('\\') == szPath[i] || _T('/') == szPath[i] )
		{
			szName[j] = _T('\0');
			j = 0;

			pChild = GetTreeChild( pNode );
			while( pChild )
			{
				PMfeNode_ pData_ = (PMfeNode_)pChild->pData;
				if ( pData_->nType == MFE_NODE_TYPE_FOLDER 
					 && 0 == STRICMP( pData_->pName, szName ) )
				{
					break;
				}
				pChild = GetTreeNextSibling( pChild );
			}
			
			// if not find the node
			if ( NULL == pChild )
			{
				return NULL;
			}
			else
			{
				pNode = pChild;
			}
		}
		else
		{
			szName[j] = szPath[i];
			j++;
		}

		i++;
	}

	if ( j > 0 )
	{
		szName[j] = _T('\0');
		j = 0;

		pChild = GetTreeChild( pNode );
		while( pChild )
		{
			PMfeNode_ pData_ = (PMfeNode_)pChild->pData;
			if ( pData_->nType == nType 
				 && 0 == STRICMP( pData_->pName, szName ) )
			{
				break;
			}
			pChild = GetTreeNextSibling( pChild );
		}
	}

	if ( MFE_NODE_TYPE_FILE == nType && pChild )
	{
		PMfeNode_ pData_ = (PMfeNode_)pChild->pData;
		if ( MFE_NODE_TYPE_FILE != pData_->nType )
		{
			return NULL;
		}
	}

	return pChild;
}

PMfeFolder MfeGetSpecifiedFolder( PMfeFolder pd, LPCTSTR szPath )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	if ( NULL == szPath )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;

	PMfeNode_ pMfeNode_ = (PMfeNode_)pNode->pData;

	if ( MFE_NODE_TYPE_FOLDER != pMfeNode_->nType )
	{
		return NULL;
	}

	return (PMfeFolder)_MfeGetSpecifiedNode( pNode, szPath, MFE_NODE_TYPE_FOLDER );
}

PMfeFile MfeGetSpecifiedFile ( PMfeFolder pd, LPCTSTR szPath )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	if ( NULL == szPath )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;

	PMfeNode_ pMfeNode_ = (PMfeNode_)pNode->pData;

	if ( MFE_NODE_TYPE_FOLDER != pMfeNode_->nType )
	{
		return NULL;
	}

	int n = STRLEN( szPath );
	if ( 0 == n )
	{
		return NULL;
	}

	// incorrect format
	if ( _T('\\') == szPath[n-1] || _T('/') == szPath[n-1] )
	{
		return NULL;
	}

	return (PMfeFile)_MfeGetSpecifiedNode( pNode, szPath, MFE_NODE_TYPE_FILE );
}

PMfeFolder MfeGetParentFolder ( void * pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	PTreeNode p = (PTreeNode)pNode;
	
	return (PMfeFolder)GetTreeParent( p );
}

// the next sibling must be folder or NULL
PMfeFolder MfeGetNextSiblingFolder( PMfeFolder pd )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;
	
	return (PMfeFolder)GetTreeNextSibling( pNode );
}

// the previous sibling can be folder, file or NULL
PMfeFolder MfeGetPrevSiblingFolder( PMfeFolder pd )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;

	PTreeNode pPrev = GetTreePrevSibling( pNode );

	if ( NULL == pPrev )
	{
		return NULL;
	}
	else
	{
		PMfeNode_ pMfeNode_ = (PMfeNode_)pPrev->pData;

		if ( MFE_NODE_TYPE_FOLDER == pMfeNode_->nType )
		{
			return (PMfeFolder)pPrev;
		}
		else
		{
			return NULL;
		}
	}
}

PMfeFolder MfeGetFirstSubFolder( PMfeFolder pd )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;
	
	PTreeNode pChild = GetTreeChild( pNode );

	while( pChild )
	{
		PMfeNode_ pMfeNode_ = (PMfeNode_)pChild->pData;

		if ( MFE_NODE_TYPE_FOLDER == pMfeNode_->nType )
		{
			return (PMfeFolder)pChild;
		}
		
		pChild = GetTreeNextSibling( pChild );
	}

	return NULL;
}

PMfeFile MfeGetFirstSubFile( PMfeFolder pd )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;
	
	PTreeNode pChild = GetTreeChild( pNode );

	while( pChild )
	{
		PMfeNode_ pMfeNode_ = (PMfeNode_)pChild->pData;

		if ( MFE_NODE_TYPE_FILE == pMfeNode_->nType )
		{
			return (PMfeFolder)pChild;
		}
		
		pChild = GetTreeNextSibling( pChild );
	}

	return NULL;
}

PMfeFile MfeGetNextSiblingFile( PMfeFile pf )
{
	if ( NULL == pf )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pf;

	PTreeNode pNext = GetTreeNextSibling( pNode );

	if ( NULL == pNext )
	{
		return NULL;
	}
	else
	{
		PMfeNode_ pMfeNode_ = (PMfeNode_)pNext->pData;

		if ( MFE_NODE_TYPE_FILE == pMfeNode_->nType )
		{
			return (PMfeFile)pNext;
		}
		else
		{
			return NULL;
		}
	}
}

PMfeFile MfeGetPrevSiblingFile( PMfeFile pf )
{
	if ( NULL == pf )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pf;
	
	return (PMfeFile)GetTreePrevSibling( pNode );
}

LPCTSTR MfeGetFolderName( PMfeFolder pd )
{
	if ( NULL == pd )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pd;

	PMfeNode_ pMfeNode_ = (PMfeNode_)pNode->pData;

	if ( MFE_NODE_TYPE_FOLDER == pMfeNode_->nType )
	{
		return pMfeNode_->pName;
	}

	return NULL;
}

LPCTSTR MfeGetFileName( PMfeFile pf )
{
	if ( NULL == pf )
	{
		return NULL;
	}

	PTreeNode pNode = (PTreeNode)pf;

	PMfeNode_ pMfeNode_ = (PMfeNode_)pNode->pData;

	if ( MFE_NODE_TYPE_FILE == pMfeNode_->nType )
	{
		return pMfeNode_->pName;
	}

	return NULL;
}

BOOL IsMfeFolder( void * pNode )
{
	if ( NULL == pNode )
	{
		return FALSE;
	}

	PTreeNode p = (PTreeNode)pNode;

	PMfeNode_ q = (PMfeNode_)p->pData;

	return ( MFE_NODE_TYPE_FOLDER == q->nType ? TRUE : FALSE );
}

unsigned int MfeGetFileSize( PMfeFile pf )
{
	if ( NULL == pf )
	{
		return 0;
	}

	PTreeNode p = (PTreeNode)pf;

	PMfeNode_ q = (PMfeNode_)p->pData;

	if ( MFE_NODE_TYPE_FILE != q->nType )
	{
		return 0;
	}

	PMfeFile_ x = (PMfeFile_)q;

	return x->nLen;
}

typedef struct tagMfeReadFile_
{
	int    nStartPos;
	int    nLen;
	int    nSkipLen;
	int    nCurPos;
	BYTE * buff;     // size: FILE_BUFF_SIZE
	int    nBuffLen;
	int    nBuffPos;
	PFunBuffCompOrUncomp pfn;
	ifstream *   pfs;
}MfeReadFile_, *PMfeReadFile_;

PMfeRead MfeStartReadFile( PMfeFile pf, PFunBuffCompOrUncomp pfn /* = NULL */ )
{
	if ( NULL == pf )
	{
		return NULL;
	}

	PTreeNode p = (PTreeNode)pf;

	PMfeNode_ q = (PMfeNode_)p->pData;

	if ( MFE_NODE_TYPE_FILE != q->nType )
	{
		return NULL;
	}

	PMfeFile_ x = (PMfeFile_)q;

	PMfeReadFile_ y = new MfeReadFile_;
	y->nStartPos = x->nPos;
	y->nLen      = x->nLen;
	y->nSkipLen  = x->nSkipLen;
	
	y->buff      = new BYTE[FILE_BUFF_SIZE];
	y->nBuffLen  = 0;
	y->nBuffPos  = 0;
	y->pfn       = pfn;
	y->pfs       = x->pfs;

	if ( 0 >= y->nSkipLen )
	{
		y->nCurPos = -1;
	}
	else
	{
		y->nCurPos = y->nStartPos;
	}

	return (PMfeRead)y;
}

void MfeCloseReadFile( PMfeRead pr )
{
	if ( pr )
	{
		PMfeReadFile_ y = (PMfeReadFile_)pr;

		delete[] y->buff;
		delete y;
	}
}

unsigned int _MfeRead( PMfeReadFile_ y, void * pBuff, unsigned int nSize )
{
	assert( nSize > 0 );
	
	int nBuffLeftLen = y->nBuffLen - y->nBuffPos;
	assert( nBuffLeftLen >= 0 );

	// need to read data to buff
	if ( 0 == nBuffLeftLen )
	{
		// reach end of file
		if ( -1 == y->nCurPos )
		{
			return 0;
		}
		else
		{
			// seek to the position
			ifstream & fs = *y->pfs;
			fs.seekg( y->nCurPos );

			int nBlockSize = 0;
			fs.read( (char *)&nBlockSize, sizeof(int) );
			assert( nBlockSize > 0 );
			y->nCurPos += sizeof(int);
			
			BYTE * pCompressed = new BYTE[nBlockSize];
			fs.read( (char *)pCompressed, nBlockSize );
			y->nCurPos += nBlockSize;

			PFunBuffCompOrUncomp pfn = y->pfn;

			if ( NULL == pfn )
			{
				memcpy( y->buff, pCompressed, nBlockSize );
				y->nBuffLen = nBlockSize;
			}
			else
			{
				y->nBuffLen = pfn( pCompressed, nBlockSize, y->buff, FILE_BUFF_SIZE );
			}
			y->nBuffPos = 0;

			delete[] pCompressed;

			nBuffLeftLen = y->nBuffLen - y->nBuffPos;

			// check if reaching the end of file
			if ( y->nCurPos - y->nStartPos >= y->nSkipLen )
			{
				y->nCurPos = -1;
			}
		}
	}

	if ( nBuffLeftLen >= nSize )
	{
		memcpy( pBuff, y->buff + y->nBuffPos, nSize );
		y->nBuffPos += nSize;

		return nSize;
	}
	else
	{
		memcpy( pBuff, y->buff + y->nBuffPos, nBuffLeftLen );
		y->nBuffPos = y->nBuffLen;

		nSize -= nBuffLeftLen;
		BYTE * z = (BYTE *)pBuff;
		int nReadSize = nBuffLeftLen;
		int nReadCnt = nBuffLeftLen;
		
		while( nSize > 0 )
		{
			z += nReadSize;
			nReadSize = _MfeRead( y, z, nSize );

			if ( 0 == nReadSize )
			{
				break;
			}
			else
			{
				nReadCnt += nReadSize;
				nSize -= nReadSize;
			}
		}

		return nReadCnt;
	}
}

unsigned int MfeRead( PMfeRead pr, void * pBuff, unsigned int nSize )
{
	if ( NULL == pr )
	{
		return 0;
	}

	if ( NULL == pBuff )
	{
		return 0;
	}

	if ( 0 == nSize )
	{
		return 0;
	}

	PMfeReadFile_ y = (PMfeReadFile_)pr;

	return _MfeRead( y, pBuff, nSize );
}

void  MfeRewindRead( PMfeRead pr )
{
	if ( NULL == pr )
	{
		return;
	}

	PMfeReadFile_ y = (PMfeReadFile_)pr;

	// keep value
	if ( 0 >= y->nSkipLen )
	{
		assert( -1 == y->nCurPos );
		return;
	}
	else
	{
		y->nCurPos   = y->nStartPos;
		y->nBuffLen  = 0;
		y->nBuffPos  = 0;
	}
}

int MfeReadFullFile( PMfeFile pf, void ** ppData, PFunBuffCompOrUncomp pfn /* = NULL */ )
{
	if ( NULL == pf )
	{
		return 0;
	}

	if ( NULL == ppData )
	{
		return 0;
	}

	PTreeNode p = (PTreeNode)pf;

	PMfeNode_ q = (PMfeNode_)p->pData;

	if ( MFE_NODE_TYPE_FILE != q->nType )
	{
		return 0;
	}

	PMfeFile_ x = (PMfeFile_)q;

	if ( 0 >= x->nLen )
	{
		return 0;
	}

	*ppData = new BYTE[x->nLen];
	
	PMfeRead pr = MfeStartReadFile( pf, pfn );
	int n = MfeRead( pr, *ppData, x->nLen );

	assert( n == x->nLen );
	MfeCloseReadFile( pr );

	return n;
}

void MfeFree( void * pData )
{
	if ( NULL == pData )
	{
		return;
	}

	delete[] (BYTE *)pData;
}