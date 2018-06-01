// stdafx.cpp : 只包括标准包含文件的源文件
// MyTalk.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#pragma warning( disable: 4996 )

LibJingleEx::TLoginSettings     g_tAccounts[MAX_ACCOUNT_NUMBER];
LibJingleEx::HANDLE             g_hXmppApp[MAX_ACCOUNT_NUMBER] = {0};

MyConfigFile                    g_Config;

static                          HWND s_hXmppWnd = 0;

FILE *  g_fp = 0;                  
CRITICAL_SECTION g_tLock;

vChatRoom g_vChatRooms[MAX_ACCOUNT_NUMBER];
BOOL      g_bConferenceAvailable[MAX_ACCOUNT_NUMBER];


BOOL GetBitsFromIPicture( BYTE * pbyData, DWORD & dwSize, IPicture *pPic );
BOOL GetBitsFromIPicture( BYTE * pbyData, DWORD & dwSize, IPicture *pPic, int nWidth, int nHeight);






std::string GetFullJid( const std::string & sNode, 
                                                  const std::string & sDomain,
                                                  const std::string & sResource )
{
    if ( sResource.length() > 0 )
    {
        return sNode + "@" + sDomain + "/" + sResource;
    }
    else
    {
        return sNode + "@" + sDomain;
    }
    
}   

std::string GetBareJid( const std::string & sNode, const std::string & sDomain )
{
    return sNode + "@" + sDomain;
}

void SetXmppConsole( HWND hWnd )
{
    s_hXmppWnd = hWnd;
}

void OnXmppMessage( const char * pbyData, unsigned long dwSize, BOOL bInput )
{
    EnterCriticalSection( &g_tLock );
    if ( bInput )
    {
        fwrite( "<<<<<<<<<<\r\n", 1, strlen("<<<<<<<<<<\r\n"), g_fp );
    }
    else
    {
        fwrite( ">>>>>>>>>>\r\n", 1, strlen(">>>>>>>>>>\r\n"), g_fp );
    }
    fwrite( pbyData, 1, dwSize, g_fp );
    fwrite( "\r\n\r\n", 1, 4, g_fp );
    fflush( g_fp );

    LeaveCriticalSection( &g_tLock );
    
    if ( s_hXmppWnd )
    {
        char * p = new char[dwSize+1];
        memcpy( p, pbyData, dwSize );
        p[dwSize] = '\0';

        // dwSize不会超过65535
        DWORD dwTmp = (bInput<<16) | dwSize;

        ::PostMessage( s_hXmppWnd, UM_XMPP_MSG, (WPARAM)p, (LPARAM)dwTmp );
    }
}

void OnLogInfo( const char * szLogMessage )
{
    EnterCriticalSection( &g_tLock );

    fwrite( szLogMessage, 1, strlen(szLogMessage), g_fp );
    // fwrite( "\r\n\r\n", 1, 4, g_fp );
    fflush( g_fp );

    LeaveCriticalSection( &g_tLock );

    if ( s_hXmppWnd )
    {
        int nLen = strlen( szLogMessage);

        char * p = new char[nLen+1];
        strcpy( p, szLogMessage );

        ::PostMessage( s_hXmppWnd, UM_DEBUG_MSG, (WPARAM)p, (LPARAM)nLen );
    }
}

BOOL _IfJidEqual( const LibJingleEx::TJid & t1, const LibJingleEx::TJid & t2 )
{
    if (     0 == strcmp( t1.sDomain, t2.sDomain ) && 0 == strcmp( t1.sNode, t2.sNode )
          && 0 == strcmp( t1.sResource, t2.sResource )
        )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

const char * GetPresence( EPresence e )
{
    switch ( e )
    {
    case PRESENCE_NONE:
        return "离线";
        break;

    case PRESENCE_OFFLINE:
        return "离线";
        break;

    case PRESENCE_XA:
        return "远远离开";
        break;

    case PRESENCE_AWAY:
        return "离开";
        break;

    case PRESENCE_DND:
        return "忙碌";
        break;

    case PRESENCE_ONLINE:
        return "在线";
        break;

    case PRESENCE_CHAT:
        return "可以聊天";
        break;

    default:
        return "离线";
        break;
    }
}

const char * GetAffiliation( EAffiliation e )
{
    switch( e )
    {
    case AFFILIATION_OWNER:
        return "所有者";
        break;
    case AFFILIATION_NONE:
        return "一般";
        break;
    case AFFILIATION_MEMBER:
        return "成员";
        break;
    case AFFILIATION_ADMIN:
        return "ADMIN";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}


HRESULT    LoadFromBuffer( const BYTE * pBuf, DWORD dwSize, IPicture * & pPicture )
{

    HGLOBAL   hGlobal  =  GlobalAlloc( GMEM_MOVEABLE,  dwSize );
    void *    pData    = GlobalLock(hGlobal);
    memcpy( pData, pBuf, dwSize );
    GlobalUnlock(hGlobal);

    IStream *  pStream  =  0;
    
    HRESULT  hRet   =  CreateStreamOnHGlobal( hGlobal, TRUE, &pStream );
    if ( S_OK != hRet )
    {
        return hRet;
    }

    hRet  =  OleLoadPicture( pStream, dwSize, FALSE, IID_IPicture, (LPVOID *)&pPicture );
    if ( S_OK != hRet )
    {
        return hRet;
    }

    pStream->Release();

    return hRet;
}


void   Draw2Dc(  IPicture * & pPicture,  CDC * pDc, CRect * pRect )
{
    long     hmWidth,  hmHeight;
    pPicture->get_Width( &hmWidth );
    pPicture->get_Height( &hmHeight );

    int      nWidth,  nHeight;
    nWidth   =  MulDiv( hmWidth,  pDc->GetDeviceCaps(LOGPIXELSX), 2540 );
    nHeight  =  MulDiv( hmHeight, pDc->GetDeviceCaps(LOGPIXELSY), 2540 );

    //CDC     memDc;
    //memDc.CreateCompatibleDC(  0 );

    //CBitmap  bitmap;
    //bitmap.CreateCompatibleBitmap( pDc, nWidth, nHeight );
    //memDc.SelectObject( &bitmap );

    // pPicture->Render( memDc.m_hDC, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, 0 );

    // pDc->StretchBlt( 0, 0, nWidth, nHeight,  &memDc, 0, 0, nWidth, nHeight, SRCCOPY );

    CRect r(0, 0, nWidth, nHeight);
    if ( 0 == pRect )
    {
        pRect = &r;
    }
    CRect & rect = *pRect;
    pPicture->Render( pDc->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(), 
                      0, hmHeight, hmWidth, -hmHeight, 0 );
}

int  GetDataFromPicture( BYTE * pbyData, DWORD & dwSize,  IPicture * & pPicture, CRect * pRect )
{
    CRect r;

    if ( 0 == pRect )
    {
        long     hmWidth,  hmHeight;
        pPicture->get_Width( &hmWidth );
        pPicture->get_Height( &hmHeight );

        CDC     memDc;
        memDc.CreateCompatibleDC(  0 );

        int      nWidth,  nHeight;
        nWidth   =  MulDiv( hmWidth,  memDc.GetDeviceCaps(LOGPIXELSX), 2540 );
        nHeight  =  MulDiv( hmHeight, memDc.GetDeviceCaps(LOGPIXELSY), 2540 );

        r.left = 0;
        r.right = nWidth;
        r.top = 0;
        r.bottom = nHeight;

        pRect = &r;
    }

    CRect & rect = *pRect;

    if ( !GetBitsFromIPicture( pbyData, dwSize, pPicture, pRect->Width(), pRect->Height() ) )
    {
        return -1;
    }

   return 0;
}


BOOL GetBitsFromIPicture( BYTE * pbyData, DWORD & dwSize, IPicture *pPic )
{
    // Create ILockBytes Buffer
    ILockBytes *plkbyt = NULL;
    CreateILockBytesOnHGlobal(NULL, TRUE, &plkbyt);

    // Create IStorage
    IStorage   *pStorage = NULL;
    HRESULT hr = ::StgCreateDocfileOnILockBytes(plkbyt,
        STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);
    if (FAILED(hr))
    {
        plkbyt->Release();
        plkbyt = NULL;
        return FALSE;
    }

    // Create IStream
    IStream    *pStream = NULL;
    hr = pStorage->CreateStream(L"PICTURE",
        STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, 0, &pStream);
    if (FAILED(hr))
    {
        pStorage->Release();
        pStorage = NULL;
        plkbyt->Release();
        plkbyt = NULL;
        return FALSE;
    }

    // Copy Data Stream
    long  lSize;
    hr = pPic->SaveAsFile(pStream, TRUE, &lSize);
    if (FAILED(hr))
    {
        pStream->Release();
        pStream = NULL;
        pStorage->Release();
        pStorage = NULL;
        plkbyt->Release();
        plkbyt = NULL;
        return FALSE;
    }
    // Get Statistics For Final Size Of Byte Array
    STATSTG  statStg;
    hr = plkbyt->Stat(&statStg, STATFLAG_NONAME);
    if (FAILED(hr))
    {
        pStream->Release();
        pStream = NULL;
        pStorage->Release();
        pStorage = NULL;
        plkbyt->Release();
        plkbyt = NULL;
        return FALSE;
    }

    // calculate "Pure" Picture Data, Must Be In a 512 Blocks...
    double  dbSkipFloat = (double(lSize) / 512);   
    DWORD  dwPicDataSize = 0;
    if(dbSkipFloat > DWORD(dbSkipFloat))
    {
        dwPicDataSize = (DWORD)dbSkipFloat + 1;
    }
    else
    {
        dwPicDataSize = (DWORD)dbSkipFloat;
    }
    dwPicDataSize = dwPicDataSize * 512;

    // 如果缓冲不足
    if ( dwSize < dwPicDataSize )
    {
        pStream->Release();
        pStream = NULL;
        plkbyt->Release();
        pStorage->Release();
        pStorage = NULL;
        plkbyt = NULL;
        return FALSE;
    }

    // Allocate Only The "Pure" Picture Data
    // BYTE  *pPicDataBuffer = (BYTE*)malloc(dwPicDataSize);
    BYTE  *pPicDataBuffer = pbyData;
    if (pPicDataBuffer == NULL)
    {
        pStream->Release();
        pStream = NULL;
        plkbyt->Release();
        pStorage->Release();
        pStorage = NULL;
        plkbyt = NULL;
        return FALSE;
    }

    // Read "Pure" Picture Data to Buffer
    _ULARGE_INTEGER ulOffset;
    ulOffset.LowPart = 0;
    ulOffset.HighPart = 0;
    ulOffset.QuadPart = (DWORD)(statStg.cbSize.QuadPart - dwPicDataSize);
    DWORD dwRealDataSize;
    hr = plkbyt->ReadAt(ulOffset, pPicDataBuffer, dwPicDataSize, &dwRealDataSize);
    if (FAILED(hr))
    {
        free(pPicDataBuffer);
        pPicDataBuffer = NULL;
        pStream->Release();
        pStream = NULL;
        pStorage->Release();
        pStorage = NULL;
        plkbyt->Release();
        plkbyt = NULL;
        return FALSE;
    }

    // Save "Pure" Picture Data to file
    //CFile  fBmp;
    //CFileException e;
    //if (!fBmp.Open(szFileName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite, &e))
    //{
    //    free(pPicDataBuffer);
    //    pPicDataBuffer = NULL;
    //    pStream->Release();
    //    pStream = NULL;
    //    pStorage->Release();
    //    pStorage = NULL;
    //    plkbyt->Release();
    //    plkbyt = NULL;
    //    return FALSE;
    //}
    //fBmp.Write(pPicDataBuffer, dwRealDataSize);
    //fBmp.Close();

    // free(pPicDataBuffer);
    dwSize = dwPicDataSize;

    pPicDataBuffer = NULL;
    pStream->Release();
    pStream = NULL;
    pStorage->Release();
    pStorage = NULL;
    plkbyt->Release();
    plkbyt = NULL;
    return TRUE;
}


BOOL GetBitsFromIPicture( BYTE * pbyData, DWORD & dwSize, IPicture *pPic, int nWidth, int nHeight)
{
    // create a new IPicture
    OLE_HANDLE hPic = NULL;
    if (FAILED(pPic->get_Handle(&hPic)))
    {
        return FALSE;
    }
    HBITMAP hBmp = (HBITMAP)CopyImage((::HANDLE)hPic,
        IMAGE_BITMAP,
        nWidth,
        nHeight,
        LR_CREATEDIBSECTION);
    if (hBmp == NULL)
    {
        return FALSE;
    }

    PICTDESC picDesc;
    picDesc.cbSizeofstruct = sizeof(PICTDESC);
    picDesc.picType = PICTYPE_BITMAP;
    picDesc.bmp.hbitmap = hBmp;

    IPicture *pNewPic = NULL;
    if (SUCCEEDED(OleCreatePictureIndirect(&picDesc, IID_IPicture, FALSE, (LPVOID *)&pNewPic)))
    {
        // Save to file
        // Save2File(szFileName, pNewPic);
        GetBitsFromIPicture( pbyData, dwSize, pNewPic );

        pNewPic->Release();
        pNewPic = NULL;
        DeleteObject(hBmp);
        hBmp = NULL;
        return TRUE;
    }
    DeleteObject(hBmp);
    hBmp = NULL;
    return FALSE;
}