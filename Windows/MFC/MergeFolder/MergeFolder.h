#ifndef __MERGE_FOLDER_2010_6_12__
#define __MERGE_FOLDER_2010_6_12__

enum emMergeFolderError
{
	MFE_SUCCESS,
	MFE_NULL_PARAM,
	MFE_NOT_DIRECTORY,
	MFE_DRIVER,
	MFE_UNKNOWN,
    MFE_TOO_MUCH,
	MFE_BUFF
};

// return value is the size after compressing or uncompressing
typedef unsigned int (*PFunBuffCompOrUncomp)( const BYTE * pIn, unsigned int nInSize, BYTE * pOut, unsigned int nOutSize );

typedef void *   PMfe;
typedef void *   PMfeFolder;
typedef void *   PMfeFile;
typedef void *   PMfeRead;

//////////////////////////////////////////////////////////////////////
// merge to a file ( under the same parent folder )
void MfeInit();
void MfeDeinit();

DWORD MfeMerge( const TCHAR * szFolder, PFunBuffCompOrUncomp pfn = NULL );
// 进度百分比
DWORD GetMergeProgress( DWORD dwThrdId );




PMfe MfeOpen( LPCTSTR szMfeFile );
void MfeClose( PMfe pHandle );

PMfeFolder MfeGetRootFolder( PMfe p );
PMfeFolder MfeGetSpecifiedFolder( PMfeFolder pd, LPCTSTR szPath );
PMfeFile   MfeGetSpecifiedFile  ( PMfeFolder pd, LPCTSTR szPath );

PMfeFolder MfeGetParentFolder     ( void * pNode );

PMfeFolder MfeGetNextSiblingFolder( PMfeFolder pd );
PMfeFolder MfeGetPrevSiblingFolder( PMfeFolder pd );
PMfeFolder MfeGetFirstSubFolder   ( PMfeFolder pd );

PMfeFile   MfeGetFirstSubFile( PMfeFolder pd );
PMfeFile   MfeGetNextSiblingFile( PMfeFile pf );
PMfeFile   MfeGetPrevSiblingFile( PMfeFile pf );

LPCTSTR    MfeGetFolderName( PMfeFolder pd );
LPCTSTR    MfeGetFileName( PMfeFile pf );

BOOL       IsMfeFolder( void * pNode );

unsigned int MfeGetFileSize( PMfeFile pf );

PMfeRead   MfeStartReadFile( PMfeFile pf, PFunBuffCompOrUncomp pfn = NULL );
void       MfeCloseReadFile( PMfeRead pr );
unsigned int MfeRead( PMfeRead pr, void * pBuff, unsigned int nSize );
void         MfeRewindRead( PMfeRead pr );

int    MfeReadFullFile( PMfeFile pf, void ** ppData, PFunBuffCompOrUncomp pfn = NULL );
void   MfeFree( void * pData );

//////////////////////////////////////////////////////////////////////


#endif