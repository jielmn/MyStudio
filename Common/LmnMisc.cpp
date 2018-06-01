#include "StdAfx.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#ifndef WIN32
#include <unistd.h>
#include <iconv.h>
#include <sys/times.h>
#include <semaphore.h>
#endif

#include "Inner.h"
#include "LmnCommon.h"


#define IGNORED_DIMENSION                  (0.000001f)
#define IS_FLOAT_ZEOR( n )                 ( (n) >= -IGNORED_DIMENSION && (n) <= IGNORED_DIMENSION )
#define IS_FLOAT_EQUAL( a, b )             IS_FLOAT_ZEOR( a - b )

#define IN_LINE_SECTION                    1
#define IN_EXTEND_LINE                     -1
#define NOT_IN_LINE                        0


typedef struct tagLine_
{
	Point p1;
	Point p2;
}Line_, * PLine_;



static BOOL _IsPointInPoint( const Point * p1, const Point * p2 )
{
	float diff_x = p1->x - p2->x;
	if ( !(diff_x >= -IGNORED_DIMENSION && diff_x <= IGNORED_DIMENSION) )
	{
		return FALSE;
	}

	float diff_y = p1->y - p2->y;
	if ( !(diff_y >= -IGNORED_DIMENSION && diff_y <= IGNORED_DIMENSION) )
	{
		return FALSE;
	}

	return TRUE;
}

/*********************************************************/
// return value:
// 1 : in the line section
// -1: int the line but not in the line section
// 0 : not in the line
/**********************************************************/
static int _IsPointInLine( const Point * pt, const Line_ * pLine )
{
	if ( _IsPointInPoint( pt, &pLine->p1 ) || _IsPointInPoint( pt, &pLine->p2 ) )
	{
		return IN_LINE_SECTION;
	}

	float diff_x = pLine->p1.x - pLine->p2.x;

	// if is vertical line
	if ( diff_x >= -IGNORED_DIMENSION && diff_x <= IGNORED_DIMENSION )
	{
		diff_x = pt->x - pLine->p1.x;
		// not in the line
		if ( !(diff_x >= -IGNORED_DIMENSION && diff_x <= IGNORED_DIMENSION) )
		{
			return NOT_IN_LINE;
		}

		// in the line
		float min_y = 0.0f;
		float max_y = 0.0f;

		if ( pLine->p1.y < pLine->p2.y )
		{
			min_y = pLine->p1.y;
			max_y = pLine->p2.y;
		}
		else
		{
			min_y = pLine->p2.y;
			max_y = pLine->p1.y;
		}

		if ( pt->y >= min_y && pt->y <= max_y )
		{
			return IN_LINE_SECTION;
		}
		else
		{
			return IN_EXTEND_LINE;
		}
	}
	else
	{
		float lineSlope = ( pLine->p2.y - pLine->p1.y ) / ( pLine->p2.x - pLine->p1.x );
		float cmpSlope  = ( pt->y       - pLine->p1.y ) / ( pt->x       - pLine->p1.x );

		float diff = lineSlope - cmpSlope;
		if ( !IS_FLOAT_ZEOR( diff ) )
		{
			return IN_EXTEND_LINE;
		}

		// in the line
		float min_y = 0.0f;
		float max_y = 0.0f;

		if ( pLine->p1.y < pLine->p2.y )
		{
			min_y = pLine->p1.y;
			max_y = pLine->p2.y;
		}
		else
		{
			min_y = pLine->p2.y;
			max_y = pLine->p1.y;
		}

		if ( pt->y >= min_y && pt->y <= max_y )
		{
			return IN_LINE_SECTION;
		}
		else
		{
			return IN_EXTEND_LINE;
		}
	}
}

COMMONLMN_API BOOL IsPointInPolygon( const Point * pt, 
									 const Point polygon[], DWORD dwNum )
{
	DWORD dwCount = 0;

	DWORD i = 0;
	for ( i = 0; i < dwNum; i++ )
	{
		Line_ line;
		line.p1 = polygon[i];
		line.p2 = polygon[(i+1)%dwNum];

		int nIsInLine = _IsPointInLine( pt, &line );
		// in the line section
		if ( IN_LINE_SECTION == nIsInLine )
		{
			return TRUE;
		}

		// if is horizontal line
		if ( IS_FLOAT_EQUAL( line.p1.y, line.p2.y ) )
		{
			continue;
		}

		if ( IS_FLOAT_EQUAL ( line.p1.y, pt->y ) && line.p1.x < pt->x )
		{
			if ( line.p1.y > line.p2.y )
			{
				dwCount++;
			}
		}
		else if ( IS_FLOAT_EQUAL ( line.p2.y, pt->y ) && line.p2.x < pt->x )
		{
			if ( line.p2.y > line.p1.y )
			{
				dwCount++;
			}
		}
		else
		{
			float min_y = 0.0f;
			float max_y = 0.0f;

			if ( line.p1.y < line.p2.y )
			{
				min_y = line.p1.y;
				max_y = line.p2.y;
			}
			else
			{
				min_y = line.p2.y;
				max_y = line.p1.y;
			}

			if ( !(pt->y >= min_y && pt->y <= max_y) )
			{
				continue;
			}

			Point intersectPoint;
			intersectPoint.x = 0.0f;
			intersectPoint.y = pt->y;

			// if is vertical line
			if ( IS_FLOAT_EQUAL( line.p1.x, line.p2.x) )
			{
				intersectPoint.x = line.p1.x;
			}
			else
			{
				float k = ( line.p2.y - line.p1.y ) / ( line.p2.x - line.p1.x );
				intersectPoint.x = line.p1.x + ( intersectPoint.y - line.p1.y ) / k;
			}

			if ( intersectPoint.x <= pt->x )
			{
				dwCount++;
			}
		}
	}

	return ( dwCount % 2 == 1 ? TRUE : FALSE );
}








/*************************************************************************/
/*  调试字节流用的                                                       */
/*  最终buff结果：左边为二进制代码，右边为字符                           */
/*  0001H 00 00 00 0a 0b 00 00 00 00 00 00 00 00 00 00 00 ...............*/
/*************************************************************************/
COMMONLMN_API int DebugStream(  char *       pchDebugBuf,  DWORD  dwDebugBufLen, 
				                const BYTE * pbyStream,    DWORD  dwStreamLen,
				                BOOL         bDosCarriageReturn /* = FALSE */,
                                BOOL         bShowAnsi /*= FALSE*/ )
{
    if (    0 == pchDebugBuf || 0 == dwDebugBufLen || 0 == pbyStream
		 || 0 == dwStreamLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }
	
    // i: iterator for stream
	DWORD i, k;

    // ascii string
	BYTE s[17];

	// keep a position for the terminate character(null)
	DWORD dwLeftSize = dwDebugBufLen - 1;

	const char * szReturn = 0;
	DWORD        dwReturnLen = 0;
	if ( bDosCarriageReturn )
	{
		szReturn = "\r\n";
	}
	else
	{
		szReturn = "\n";
	}
    dwReturnLen = strlen( szReturn );

    const char * szMargin = " ";
    DWORD dwMarginLen = strlen(szMargin);

    
    DWORD dwMod = 0;
	char  szBuf[32];
	
	for ( i = 0; i < dwStreamLen; i++ )
	{
        dwMod = i % 16;

        if ( 0 == dwMod )
        {
            // want carriage return
            if ( i > 0 )
            {
                // try to copy the ascii string

                if ( dwLeftSize < dwMarginLen )
                {
                    memcpy( pchDebugBuf, szMargin, dwLeftSize );
                    pchDebugBuf += dwLeftSize;
                    dwLeftSize  -= dwLeftSize;

		            *pchDebugBuf = '\0';
		            return LMN_ERR_NOT_ENOUGH_BUFF;
                }
    
                memcpy( pchDebugBuf, szMargin, dwMarginLen );
                pchDebugBuf += dwMarginLen;
                dwLeftSize  -= dwMarginLen;


                if ( dwLeftSize < 16 )
                {
                    memcpy( pchDebugBuf, s, dwLeftSize );
                    pchDebugBuf += dwLeftSize;
                    dwLeftSize  -= dwLeftSize;

		            *pchDebugBuf = '\0';
		            return LMN_ERR_NOT_ENOUGH_BUFF;
                }
    
                memcpy( pchDebugBuf, s, 16 );
                pchDebugBuf += 16;
                dwLeftSize  -= 16;

                if ( dwLeftSize < dwReturnLen )
			    {
				    *pchDebugBuf = '\0';
				    return LMN_ERR_NOT_ENOUGH_BUFF;
			    }
			    
			    memcpy( pchDebugBuf, szReturn, dwReturnLen );
                pchDebugBuf += dwReturnLen;
			    dwLeftSize  -= dwReturnLen;
            }
			

            // want the front 11 characters
            SNPRINTF( szBuf, sizeof(szBuf), "%08lxh: ", i );

			if ( dwLeftSize < 11 )
			{
                memcpy( pchDebugBuf, szBuf, dwLeftSize );
                pchDebugBuf += dwLeftSize;
                dwLeftSize  -= dwLeftSize;

				*pchDebugBuf = '\0';
				return LMN_ERR_NOT_ENOUGH_BUFF;
			}

			memcpy( pchDebugBuf, szBuf, 11 );
            pchDebugBuf += 11;
			dwLeftSize  -= 11;
        }

        SNPRINTF( szBuf, sizeof(szBuf), "%02x ",  pbyStream[i] );

		if ( dwLeftSize < 3 )
		{
			memcpy( pchDebugBuf, szBuf, dwLeftSize );
            pchDebugBuf += dwLeftSize;
            dwLeftSize  -= dwLeftSize;

			*pchDebugBuf = '\0';
			return LMN_ERR_NOT_ENOUGH_BUFF;
		}
        
		memcpy( pchDebugBuf, szBuf, 3 );
        pchDebugBuf += 3;
		dwLeftSize  -= 3;

		// ascii between 32 and 127 is visible
		if ( (char)pbyStream[i] >= 32 )
		{
			s[dwMod] = (char)pbyStream[i];
		}
		else
		{
            if ( bShowAnsi )
            {
                // 也许是汉字
                if ( LmnCommon::IfHasBit(pbyStream[i],7) )
                {
                    s[dwMod] = (char)pbyStream[i];
                }
                else
                {
                    s[dwMod] = '.';
                }
            }
            else
            {
                s[dwMod] = '.';
            }
		}
	}

    // give the ascii string the terminate char
    s[dwMod + 1] = '\0';

    for ( k = 0; k < 15 - dwMod; k++ )
	{
        if ( dwLeftSize < 3 )
        {
            memcpy( pchDebugBuf, "   ", dwLeftSize );
            pchDebugBuf += dwLeftSize;
            dwLeftSize  -= dwLeftSize;

			*pchDebugBuf = '\0';
			return LMN_ERR_NOT_ENOUGH_BUFF;
        }
		
		memcpy( pchDebugBuf, "   ", 3 );
        pchDebugBuf += 3;
		dwLeftSize  -= 3;
	}

    if ( dwLeftSize < dwMarginLen )
    {
        memcpy( pchDebugBuf, szMargin, dwLeftSize );
        pchDebugBuf += dwLeftSize;
        dwLeftSize  -= dwLeftSize;

		*pchDebugBuf = '\0';
		return LMN_ERR_NOT_ENOUGH_BUFF;
    }

    memcpy( pchDebugBuf, szMargin, dwMarginLen );
    pchDebugBuf += dwMarginLen;
    dwLeftSize  -= dwMarginLen;

    if ( dwLeftSize < dwMod + 1 )
    {
        memcpy( pchDebugBuf, s, dwLeftSize );
        pchDebugBuf += dwLeftSize;
        dwLeftSize  -= dwLeftSize;

		*pchDebugBuf = '\0';
		return LMN_ERR_NOT_ENOUGH_BUFF;
    }
    
    memcpy( pchDebugBuf, s, dwMod + 1 );
    pchDebugBuf += dwMod + 1;
    dwLeftSize  -= dwMod + 1;
    
    *pchDebugBuf = '\0';
    return LMN_ERR_OK;
}




/***********************************************************************/
// 功能: 获取一个随机数
/***********************************************************************/
DWORD GetRand( DWORD dwMin, DWORD dwMax )
{
    // 如果dwMin比dwMax大，交换dwMin和dwMax
    if ( dwMin > dwMax )
    {
        DWORD dwTemp = dwMax;
        dwMax = dwMin;
        dwMin = dwTemp;
    }
    
    DWORD dwLength = dwMax - dwMin + 1;
    DWORD dwRet = dwMin + (DWORD)( dwLength * ( rand() / ( RAND_MAX + 1.0f ) ) );
    
    return dwRet;
}

COMMONLMN_API void  InitRand( BOOL bCustomSrand /*= FALSE*/, DWORD dwSrand /*= 0*/ )
{
    if ( bCustomSrand )
    {
        srand( dwSrand );
    }
    else
    {
        srand( LmnGetTickCount() );
    }
}






#define MAX_CONSOLE_MENU_NAME_SIZE             80
#define CONSOLE_CLEAR()                        DO_ONCE( system("cls"); )
#define CONSOLE_LINE_WIDTH                     80
#define MAX_CONSOLE_CHOICE_BUF_SIZE            1024


typedef  struct  tagConsoleMenu_
{
    char                  szName[MAX_CONSOLE_MENU_NAME_SIZE];     // 菜单标题
    PList                 pItemsList;
}TConsoleMenu_, *PTConsoleMenu_;


static  BOOL             s_bConsoleMenuInited = FALSE;
static  HandleChoiceCb   s_pfnHandleChoice    = 0;
static  SelectChoiceCb   s_pfnSelect          = 0;
static  PList            s_pMenusList         = 0;


// 初始化菜单系统
COMMONLMN_API int InitConsole( SelectChoiceCb pfnSelect, HandleChoiceCb  pfnHandleChoice )
{
    if ( s_bConsoleMenuInited )
    {
        return LMN_ERR_CONSOLE_MENU_ALREADY_INITED;
    }

    if ( 0 == pfnHandleChoice || 0 == pfnSelect )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    s_pMenusList = InitList();
    if ( 0 == s_pMenusList )
    {
        return LMN_ERR_NO_MEMORY;
    }

    s_pfnSelect           = pfnSelect;
    s_pfnHandleChoice     = pfnHandleChoice;

    s_bConsoleMenuInited  = TRUE;

    return LMN_ERR_OK;
}


// 创建菜单
COMMONLMN_API ConsoleMenuHandle  CreateConsoleMenu( const char * szTitle )
{
    PTConsoleMenu_   pMenu_ = (PTConsoleMenu_)g_pfnAllocMem( sizeof(TConsoleMenu_) );
    if ( 0 == pMenu_ )
    {
        return 0;
    }
    memset( pMenu_, 0, sizeof(TConsoleMenu_) );


    if ( 0 != szTitle )
    {
        STRNCPY( pMenu_->szName,  szTitle,  sizeof(pMenu_->szName) );
    }

    pMenu_->pItemsList = InitList();
    if ( 0 == pMenu_->pItemsList )
    {
        SAFE_FREE( pMenu_ );
        return 0;
    }

    if ( 0 == Insert2ListTail( s_pMenusList, pMenu_ ) )
    {
        DeinitList( pMenu_->pItemsList );
        SAFE_FREE( pMenu_ );
        return 0;
    }

    return pMenu_;
}


// 添加菜单项
COMMONLMN_API int  AddConsoleMenuItem( ConsoleMenuHandle hConsoleMenu,  const TConsoleMenuItem * pMenuItem )
{
    if ( 0 == hConsoleMenu || 0 == pMenuItem )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    PListNode pFind = FindFirstListNodeByValue( s_pMenusList, hConsoleMenu );
    if ( 0 == pFind )
    {
        return LMN_ERR_NO_SUCH_CONSOLE_MENU;
    }
    
    PTConsoleMenu_  pMenu_ = (PTConsoleMenu_)pFind->pData;
    assert( (ConsoleMenuHandle)pMenu_ == hConsoleMenu );


    if ( 0 != pMenuItem->hMenu )
    {
        PListNode pFindNavigation = FindFirstListNodeByValue( s_pMenusList, pMenuItem->hMenu );
        if ( 0 == pFindNavigation )
        {
            return LMN_ERR_NO_SUCH_CONSOLE_MENU;
        }
    }


    PTConsoleMenuItem  pNewMenuItem = (PTConsoleMenuItem)g_pfnAllocMem( sizeof(TConsoleMenuItem) );
    if ( 0 == pNewMenuItem )
    {
        return LMN_ERR_NO_MEMORY;
    }
    
    memcpy( pNewMenuItem,  pMenuItem, sizeof(TConsoleMenuItem) );
    
    // 添加失败
    if ( 0 == Insert2ListTail( pMenu_->pItemsList, pNewMenuItem ) )
    {
        SAFE_FREE( pNewMenuItem );
        return LMN_ERR_NO_MEMORY;
    }

    return LMN_ERR_OK;
}


// 显示某个菜单
// 注意：在一次InitConsole和DeinitConsole之间只准调用一次
COMMONLMN_API int  DisplayConsoleMenu( ConsoleMenuHandle hConsoleMenu )
{
    if ( !s_bConsoleMenuInited ||  0 == hConsoleMenu )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    PListNode pFind = FindFirstListNodeByValue( s_pMenusList, hConsoleMenu );
    if ( 0 == pFind )
    {
        return LMN_ERR_NO_SUCH_CONSOLE_MENU;
    }

    PTConsoleMenu_  pMenu_ = (PTConsoleMenu_)pFind->pData;
    assert( (ConsoleMenuHandle)pMenu_ == hConsoleMenu );



    while( TRUE )
    {
        CONSOLE_CLEAR();

        
        DWORD i = 0;
        DWORD dwTitleLen = strlen( pMenu_->szName );
        
        // print  TITLE
        if ( dwTitleLen > 0  )
        {
            if ( CONSOLE_LINE_WIDTH > dwTitleLen )
            {
                DWORD dwStart = ( CONSOLE_LINE_WIDTH - dwTitleLen ) / 2;
                for ( i = 0; i < dwStart; i++ )	
                {
                    printf(" ");
                }
            }
            
            printf( "%s\n", pMenu_->szName );
        }
        
        // print ITEMS
        PListNode pItemNode = GetListHead( pMenu_->pItemsList ); 
        while( pItemNode )
        {
            PTConsoleMenuItem  pMenuItem = (PTConsoleMenuItem)pItemNode->pData;
            printf("%s\n", pMenuItem->szName);
            pItemNode = GetNextListNode( pItemNode );
        }
        
        
        char szChoice[MAX_CONSOLE_CHOICE_BUF_SIZE];
        printf("\nInput your choice: ");

        
        scanf( " %s", szChoice );
        StrTrim( szChoice );
        Str2Lower( szChoice );
        
        // 如果返回值是QUIT_CONSOLE_MENU，跳出系统
        DWORD dwChoice = s_pfnSelect( pMenu_, szChoice );
        if ( QUIT_CONSOLE_MENU == dwChoice )
        {
            break;
        }

        // index超出范围
        if ( dwChoice >= GetListSize( pMenu_->pItemsList ) )
        {
            printf("\nInvalid input! \n");

            fflush( stdin );
            printf("PREESS ANY KEY TO CONTINUE!");
            getchar();
            fflush( stdin );
		    printf("\n");
        }
        else
        {
            i = 0;
            pItemNode = GetListHead( pMenu_->pItemsList ); 
            while( pItemNode )
            {
                if ( dwChoice == i )
                {
                    break;
                }
                pItemNode = GetNextListNode( pItemNode );
                i++;
            }
            assert( pItemNode );

            PTConsoleMenuItem  pMenuItem = (PTConsoleMenuItem)pItemNode->pData;
            // 如果需要跳转到另一个子菜单
            if( 0 != pMenuItem->hMenu )
            {
                pMenu_ = (PTConsoleMenu_)pMenuItem->hMenu;
                continue;
            }
            else
            {
                s_pfnHandleChoice( hConsoleMenu, dwChoice );

                fflush( stdin );
                printf("PREESS ANY KEY TO CONTINUE!");
                getchar();
                fflush( stdin );
		        printf("\n");
            }
        }
    }
    

    return LMN_ERR_OK;
}

// 去初始化菜单系统
COMMONLMN_API int  DeinitConsole()
{
    PListNode pMenuNode = GetListHead( s_pMenusList ); 
    while( pMenuNode )
    {
        PTConsoleMenu_ pMenu_ = (PTConsoleMenu_)pMenuNode->pData;

        ClearListWithPointerValue( pMenu_->pItemsList );
        DeinitList( pMenu_->pItemsList );
        SAFE_FREE( pMenu_ );

        pMenuNode = GetNextListNode( pMenuNode );
    }
    DeinitList( s_pMenusList );
    s_pMenusList = 0;

    s_pfnSelect           = 0;
    s_pfnHandleChoice     = 0;
    s_bConsoleMenuInited  = FALSE;

    return LMN_ERR_OK;
}








static  void  FileSlashNormalize( char * szFilePath )
{
    while ( '\0' != szFilePath[0] )
    {
#ifdef WIN32
        if ( '/' == szFilePath[0] )
        {
            szFilePath[0] = '\\';
        }
#else
        if ( '\\' == szFilePath[0] )
        {
            szFilePath[0] = '/';
        }
#endif  
        szFilePath++;
    }
}

/***********************************************************************/
// 功能: 根据文件路径名获取文件所在目录，文件名和扩展名  
/***********************************************************************/
COMMONLMN_API int  ParseFileName( const char * szFilePath, 
					char * szFileDirectory /* = 0 */,  DWORD  dwDirLen /* = 0 */,
                    char * szFileName /* = 0 */,       DWORD dwNameLen /* = 0 */,
                    char * szExtention /* = 0 */,      DWORD dwExtLen /* = 0 */ )
{
    if ( 0 == szFilePath )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    // 进行trim转换
	DWORD  dwLen   =  strlen( szFilePath );
    char * pchBuf  =  (char *)g_pfnAllocMem( sizeof(char) * (dwLen + 1 + 1) );

    // 如果分配内存失败
    if ( 0 == pchBuf )
    {
        return LMN_ERR_NO_MEMORY;
    }

	strcpy( pchBuf, szFilePath );
	StrTrim( pchBuf );

    DWORD dwBufLen = strlen( pchBuf );

    // 把所有'\\'转换为'/'( linux )
    // 或把所有'/'转换为'\'( windows )
    FileSlashNormalize( pchBuf );


#ifdef WIN32
    // 如果是'x:'形式，加上'\\'
    if ( 2 == dwBufLen && ':' == pchBuf[1] )
    {
        pchBuf[2] = '\\';
        pchBuf[3] = '\0';
        dwBufLen++;
    }
#else
    // 去警告
    dwBufLen = dwBufLen;
#endif

#ifdef WIN32
    char * pchLastSlash = strrchr( pchBuf, '\\' );
#else
    char * pchLastSlash = strrchr( pchBuf, '/' );
#endif

    if ( 0 == pchLastSlash )
    {
        if ( 0 != szFileDirectory && 0 != dwDirLen )
        {
            szFileDirectory[0] = '\0';
        }

        char * pchDot = strrchr ( pchBuf, '.' );
        if ( 0 == pchDot )
        {
            if ( 0 != szFileName && 0 != dwNameLen )
            {
                // name buffer不够
                if ( dwNameLen <= dwBufLen )
                {
                    return LMN_ERR_NOT_ENOUGH_BUFF;
                }

                strcpy( szFileName, pchBuf );
            }

            if ( 0 != szExtention && 0 != dwExtLen )
            {
                szExtention[0] = '\0';
            }
        }
        else
        {
            if ( 0 != szFileName && 0 != dwNameLen )
            {
                DWORD  dwRealLen  = pchDot - pchBuf;
                if ( dwNameLen <= dwRealLen )
                {
                    return LMN_ERR_NOT_ENOUGH_BUFF;
                }

                memcpy( szFileName, pchBuf, dwRealLen );
                szFileName[dwRealLen] = '\0';
            }

            if ( 0 != szExtention && 0 != dwExtLen )
            {
                DWORD dwRealLen = pchBuf + dwBufLen - ( pchDot + 1 );
                if ( dwExtLen <= dwRealLen )
                {
                    return LMN_ERR_NOT_ENOUGH_BUFF;
                }

                strcpy( szExtention, pchDot + 1 );
            }
        }
    }
    else
    {
        if ( 0 != szFileDirectory && 0 != dwDirLen )
        {
            DWORD  dwRealLen  = pchLastSlash - pchBuf;
            if ( dwDirLen <= dwRealLen )
            {
                return LMN_ERR_NOT_ENOUGH_BUFF;
            }

            memcpy( szFileDirectory, pchBuf, dwRealLen );
            szFileDirectory[dwRealLen] = '\0';
        }

        char * pchDot = strrchr ( pchBuf, '.' );
        if ( 0 == pchDot )
        {
            if ( 0 != szFileName && 0 != dwNameLen )
            {
                DWORD  dwRealLen  = pchBuf + dwBufLen - ( pchLastSlash + 1 );
                if ( dwNameLen <= dwRealLen )
                {
                    return LMN_ERR_NOT_ENOUGH_BUFF;
                }
                strcpy( szFileName, pchLastSlash + 1 );
            }

            if ( 0 != szExtention && 0 != dwExtLen )
            {
                szExtention[0] = '\0';
            }
        }
        else
        {
            // 无效的'.'
            if ( pchDot < pchLastSlash )
            {
                SAFE_FREE( pchBuf );
                return LMN_ERR_WRONG_FILE_PATH_FORMAT;
            }
            else
            {
                if ( 0 != szFileName && 0 != dwNameLen )
                {
                    DWORD  dwRealLen  = pchDot - pchLastSlash - 1 ;
                    if ( dwNameLen <= dwRealLen )
                    {
                        return LMN_ERR_NOT_ENOUGH_BUFF;
                    }
                
                    memcpy( szFileName, pchLastSlash + 1, dwRealLen );
                    szFileName[dwRealLen] = '\0';
                }

                if ( 0 != szExtention && 0 != dwExtLen )
                {
                    DWORD  dwRealLen  = pchBuf + dwBufLen - (pchDot + 1);
                    if ( dwExtLen <= dwRealLen )
                    {
                        return LMN_ERR_NOT_ENOUGH_BUFF;
                    }
                    strcpy( szExtention, pchDot + 1 );
                }
            }
        }
    }

    SAFE_FREE( pchBuf );
    return LMN_ERR_OK;
}






/*************************       互斥锁   **************************************/
COMMONLMN_API int  LmnInitLock  ( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32
    InitializeCriticalSection( pLock );
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( pLock, &attr );
    pthread_mutexattr_destroy( &attr );
#endif

    return LMN_ERR_OK;
}

COMMONLMN_API int  LmnDeinitLock( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32
    DeleteCriticalSection( pLock );
#else
    pthread_mutex_destroy( pLock );
#endif

    return LMN_ERR_OK;
}

COMMONLMN_API int  LmnLock( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif

    return LMN_ERR_OK;
}

COMMONLMN_API int  LmnUnlock( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32
    LeaveCriticalSection( pLock );
#else
    pthread_mutex_unlock( pLock );
#endif

    return LMN_ERR_OK;
}
/*************************    END 互斥锁   **************************************/





/*************************    信号量      ************************************/
#define MAX_SEMPHORE_COUNT    0x7FFFFFFF

#ifndef WIN32
typedef struct tagSemphore
{
    LmnLockType    m_lock;
    DWORD          m_dwCurCnt;
    DWORD          m_dwMaxCnt;
    sem_t          m_semphore;
}TSemphore_, *PTSemphore_;
#endif

COMMONLMN_API  int  LmnSemCreate( LmnSemphoreType * pSemphore, DWORD dwInitCnt,DWORD dwMaxCnt )
{
    if ( 0 == pSemphore  )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( 0 == dwMaxCnt || dwInitCnt > dwMaxCnt || dwMaxCnt > MAX_SEMPHORE_COUNT )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32

    HANDLE h = CreateSemaphore( 0,dwInitCnt,dwMaxCnt, 0 );
    if ( 0 == h )
    {
        return LMN_ERR_FAILED_TO_CREATE_SEMPHORE;
    }

    *pSemphore = h;

#else

    PTSemphore_  p = (PTSemphore_)g_pfnAllocMem( sizeof(TSemphore_) );
    if ( 0 == p )
    {
        return LMN_ERR_NO_MEMORY;
    }

    memset(p, 0, sizeof(TSemphore_));

    LmnInitLock( &p->m_lock );

    p->m_dwCurCnt = dwInitCnt;
    p->m_dwMaxCnt = dwMaxCnt;

    if ( 0 != sem_init( &p->m_semphore, 0, dwInitCnt ) )
    {
        LmnDeinitLock(&p->m_lock);
        SAFE_FREE( p );
        return LMN_ERR_FAILED_TO_CREATE_SEMPHORE;
    }

    *pSemphore = (LmnSemphoreType)p;

#endif

    return LMN_ERR_OK;
}

COMMONLMN_API  int  LmnSemTake(LmnSemphoreType semphore)
{
    if ( 0 == semphore )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32
    
    if ( WAIT_FAILED == WaitForSingleObject( semphore, INFINITE ) )
    {
        return LMN_ERR_FAILED_TO_WAIT_SEMPHORE;
    }

#else

    PTSemphore_  p = (PTSemphore_)semphore;

    if ( 0 != sem_wait( &p->m_semphore ) )
    {
        return LMN_ERR_FAILED_TO_WAIT_SEMPHORE;;
    }
    
    LmnLock( &p->m_lock );
    p->m_dwCurCnt--;
    assert( p->m_dwCurCnt <= p->m_dwMaxCnt );
    LmnUnlock( &p->m_lock );

#endif

    return LMN_ERR_OK;
}

COMMONLMN_API  int  LmnSemTakeByTime(LmnSemphoreType semphore, DWORD dwWaitMiniSeconds)
{
    if ( 0 == semphore )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32

    DWORD dwRet = WaitForSingleObject( semphore, dwWaitMiniSeconds );
    if ( WAIT_OBJECT_0 == dwRet )
    {
        return LMN_ERR_OK;
    }
    else
    {
        return LMN_ERR_FAILED_TO_WAIT_SEMPHORE;
    }

#else

    PTSemphore_  p = (PTSemphore_)semphore;

    timespec ts;                         
    clock_gettime(CLOCK_REALTIME, &ts );       //获取当前时间
    ts.tv_sec  += ( dwWaitMiniSeconds / 1000 );           //加上等待时间的秒数
    ts.tv_nsec += ( dwWaitMiniSeconds % 1000 ) * 1000;    //加上等待时间纳秒数

    if ( 0 != sem_timedwait( &p->m_semphore, &ts ) )
    {
        return LMN_ERR_FAILED_TO_WAIT_SEMPHORE;
    }
    
    LmnLock( &p->m_lock );
    p->m_dwCurCnt--;
    assert( p->m_dwCurCnt <= p->m_dwMaxCnt );
    LmnUnlock( &p->m_lock );

    return LMN_ERR_OK;

#endif
}

COMMONLMN_API  int  LmnSemGive(LmnSemphoreType semphore)
{
    if ( 0 == semphore )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32

    if ( ReleaseSemaphore( semphore, 1, 0 ) )
    {
        return LMN_ERR_OK;
    }
    else
    {
        return LMN_ERR_FAILED_TO_RELEASE_SEMPHORE;
    }

#else

    PTSemphore_  p = (PTSemphore_)semphore;

    LmnLock( &p->m_lock );

    // 达到最大值
    if ( p->m_dwCurCnt == p->m_dwMaxCnt )
    {
        LmnUnlock( &p->m_lock );
        return LMN_ERR_FAILED_TO_RELEASE_SEMPHORE;
    }
    
    if ( 0 != sem_post( &p->m_semphore ) )
    {
        LmnUnlock( &p->m_lock );
        return LMN_ERR_FAILED_TO_RELEASE_SEMPHORE;
    }
    
    p->m_dwCurCnt++;
    LmnUnlock( &p->m_lock );

    return LMN_ERR_OK;

#endif
}

COMMONLMN_API  int  LmnSemDestroy(LmnSemphoreType semphore)
{
    if ( 0 == semphore )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

#ifdef WIN32

    if ( !CloseHandle( semphore ) )
    {
        return LMN_ERR_FAILED_TO_DESTROY_SEMPHORE;
    }

#else

    PTSemphore_  p = (PTSemphore_)semphore;

    if ( 0 != sem_destroy( &p->m_semphore ) )
    {
        return LMN_ERR_FAILED_TO_DESTROY_SEMPHORE;
    }

    LmnDeinitLock( &p->m_lock );

    SAFE_FREE( p );

#endif

    return LMN_ERR_OK;
}

/*************************   END 信号量    ************************************/







/*************************    线程      *************************************/
COMMONLMN_API  DWORD  LmnGetCurrentThrdId()
{
#ifdef WIN32
    return GetCurrentThreadId();
#else
    return (DWORD)pthread_self();
#endif
}


// 2层Hashtable
// 
// key:   thread id
// value: hashtable
//       key:   用户定义id
//       value: 用户自定义值

static  PHashtable    s_phsThrdData = 0;
static  LmnLockType   s_ThrdDataLock;
static  BOOL          s_bThrdDataInited = FALSE;

static  HashFunc           s_pfnThrdKeyHash  = 0;
static  CompFunc           s_pfnThrdKeyComp  = 0;
static  OnThrdDataEraseCb  s_pfnOnThrdErase  = 0;
static  OnThrdDataChangeCb s_pfnOnThrdChange = 0;

// init thread local storage module
COMMONLMN_API  int   LmnInitThrdTls( HashFunc           pfnHash /*=0*/,      // hash(key)
                                     CompFunc           pfnComp /*=0*/,      // compare(key)
                                     OnThrdDataEraseCb  pfnOnErase /*=0*/,   // hash item 被删除
                                     OnThrdDataChangeCb pfnOnChange /*=0*/ ) // hash item 被修改
{
    if ( s_bThrdDataInited )
    {
        return LMN_ERR_ALREADY_INITED;
    }

    LmnInitLock  ( &s_ThrdDataLock );
    s_phsThrdData = InitHashtable( 0, 0, 0 );
    if ( 0 == s_phsThrdData )
    {
        LmnDeinitLock( &s_ThrdDataLock );
        return LMN_ERR_NO_MEMORY;
    }
    
    s_pfnThrdKeyHash  = pfnHash;
    s_pfnThrdKeyComp  = pfnComp;
    s_pfnOnThrdErase  = pfnOnErase;
    s_pfnOnThrdChange = pfnOnChange;
    
    s_bThrdDataInited = TRUE;

    return LMN_ERR_OK;
}

COMMONLMN_API  int   LmnSetThrdData( const void * pKey,  const void * pValue )
{
    if ( !s_bThrdDataInited )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    LmnLock( &s_ThrdDataLock );

    DWORD dwThrdId = LmnGetCurrentThrdId();
    PHashtable pHt = 0;

    // 如果没有这个线程对应的tls hashtable
    if ( !GetHashtableValue( s_phsThrdData, (void *)dwThrdId, &pHt ) )
    {
        pHt =  InitHashtable( 0, s_pfnThrdKeyHash, s_pfnThrdKeyComp );
        if ( 0 == pHt )
        {
            LmnUnlock( &s_ThrdDataLock );
            return LMN_ERR_NO_MEMORY;
        }

        PHashNode pNode = Add2Hashtable( s_phsThrdData, (void *)dwThrdId, (void *)pHt );
        if ( 0 == pNode )
        {
            LmnUnlock( &s_ThrdDataLock );
            return LMN_ERR_NO_MEMORY;
        }
    }

    assert( pHt );

    void * pOldValue = 0;
    // 如果已经有数据
    if ( GetHashtableValue( pHt, pKey, &pOldValue ) )
    {
        // 通知回调有数据改动
        if ( s_pfnOnThrdChange )
        {
            s_pfnOnThrdChange( (void *)pKey, pOldValue );
        }
    }

    SetHashtable( pHt, pKey, pValue );

    LmnUnlock( &s_ThrdDataLock );
    return LMN_ERR_OK;
}

COMMONLMN_API  int   LmnGetThrdData( const void * pKey,  void ** ppValue )
{
    if ( !s_bThrdDataInited )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( 0 == ppValue )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    LmnLock( &s_ThrdDataLock );

    unsigned long dwThrdId = LmnGetCurrentThrdId();
    PHashtable pHt = 0;

    // 如果该线程没有对应的hashtable数据
    if ( !GetHashtableValue( s_phsThrdData, (void *)dwThrdId, &pHt ) )
    {
        *ppValue = 0;
        LmnUnlock( &s_ThrdDataLock );
        return LMN_ERR_OK;
    }

    assert( pHt );

    if ( !GetHashtableValue( pHt, pKey, ppValue ) )
    {
        *ppValue = 0;
        LmnUnlock( &s_ThrdDataLock );
        return LMN_ERR_OK;
    }

    LmnUnlock( &s_ThrdDataLock );
    return LMN_ERR_OK;
}

COMMONLMN_API  int   LmnEraseThrdData( const void * pKey )
{
    if ( !s_bThrdDataInited )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    LmnLock( &s_ThrdDataLock );

    unsigned long dwThrdId = LmnGetCurrentThrdId();
    PHashtable pHt = 0;

    // 如果该线程没有对应的hashtable数据
    if ( !GetHashtableValue( s_phsThrdData, (void *)dwThrdId, &pHt ) )
    {
        LmnUnlock( &s_ThrdDataLock );
        return LMN_ERR_OK;
    }

    assert( pHt );

    void * pOldValue = 0;
    // 如果已经有数据
    if ( GetHashtableValue( pHt, pKey, &pOldValue ) )
    {
        // 通知回调有数据改动
        if ( s_pfnOnThrdErase )
        {
            s_pfnOnThrdErase( (void *)pKey, pOldValue );
        }
    }

    EraseHashtableByKey( pHt, pKey );

    LmnUnlock( &s_ThrdDataLock );
    return LMN_ERR_OK;
}

COMMONLMN_API  int   LmnDeinitThrdTls()
{
    if ( !s_bThrdDataInited )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    LmnLock( &s_ThrdDataLock );
      
    PHashNode pNode = GetHashtableFirstNode( s_phsThrdData );
    while( pNode )
    {
        PHashtable pHt = (PHashtable)pNode->pData;

        // 通知回调有数据改动
        if ( s_pfnOnThrdErase )
        {
            PHashNode pSubNode = GetHashtableFirstNode( pHt );
            while ( pSubNode )
            {
                void * pKey = GetHashNodeKey( pSubNode );

                s_pfnOnThrdErase ( pKey, pSubNode->pData);

                pSubNode = GetNextHashtableNode( pSubNode );
            }
        }
        
        DeinitHashtable( pHt );

        pNode = GetNextHashtableNode( pNode );
    }

    DeinitHashtable( s_phsThrdData );
    s_phsThrdData = 0;

    LmnUnlock( &s_ThrdDataLock );
    LmnDeinitLock( &s_ThrdDataLock );

    s_pfnThrdKeyHash  = 0;
    s_pfnThrdKeyComp  = 0;
    s_pfnOnThrdErase  = 0;
    s_pfnOnThrdChange = 0;

    s_bThrdDataInited = FALSE;

    return LMN_ERR_OK;
}


#ifdef WIN32
static DWORD WINAPI _WinThrdRuntine( LPVOID lpThreadParameter )
{
    void ** ppArgs    = (void **)lpThreadParameter;

    LmnThreadFunc pFunc = (LmnThreadFunc)ppArgs[0];
    void * pParam     = ppArgs[1];

    void * pRet = pFunc( pParam );

    SAFE_FREE( ppArgs );

    return (DWORD)pRet;
}
#endif


COMMONLMN_API  LmnThrdType LmnCreateThread( LmnThreadFunc pfnThrd, void * pParam, DWORD dwStackSize, BOOL bDetached /*= FALSE*/ )
{
    if ( 0 == pfnThrd )
    {
        return 0;
    }

#ifdef WIN32
    
    void ** ppArgs = (void **)g_pfnAllocMem( sizeof(void *) * 2 );
    if ( 0 == ppArgs )
    {
        return 0;
    }

    ppArgs[0] = pfnThrd;
    ppArgs[1] = pParam;

    LmnThrdType hThrd = ::CreateThread( 0, dwStackSize, _WinThrdRuntine, ppArgs, 0, 0 );
    if ( 0 == hThrd )
    {
        SAFE_FREE( ppArgs );
        return 0;
    }

    if ( bDetached && hThrd )
    {
        CloseHandle( hThrd );
    }

    return hThrd;

#else

    LmnThrdType  hThrd;
    int nRet = 0;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if ( dwStackSize > 0 )
    {
        pthread_attr_setstacksize( &attr, dwStackSize );
    }

    if ( bDetached )
    {
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    }

    nRet = pthread_create( &hThrd, &attr, pfnThrd, pParam );
    pthread_attr_destroy(&attr);

    if ( 0 == nRet )
    {
        return hThrd;
    }
    else
    {
        return 0;
    }
#endif
}


COMMONLMN_API  void *   LmnWait4Thrd( LmnThrdType hThrd )
{
    if ( 0 == hThrd )
    {
        return 0;
    }

#ifdef WIN32

    WaitForSingleObject( hThrd, INFINITE );

    DWORD dwExit;
    if ( GetExitCodeThread( hThrd, &dwExit ) )
    {
        CloseHandle( hThrd );
        return (void *)dwExit;
    }
    else
    {
        CloseHandle( hThrd );
        return 0;
    }
#else

    void * pRet;

    if ( 0 == pthread_join( hThrd, &pRet ) )
    {
        return pRet;
    }
    else
    {
        return 0;
    }

#endif
}

/*************************   END 线程    ************************************/

































static  BOOL         s_bCharSetInited = FALSE;
static  LmnLockType  s_CharsetLock;


#ifndef WIN32
static iconv_t       s_utf8_to_ansi;
static iconv_t       s_ansi_to_utf8;
#endif


#ifdef WIN32
#define MAX_CHARSET_CONVERT_BUFFER_SIZE         8192
#endif



/****************************************************************************
 * 函数名：  Utf8ToAnsi                                                     *
 * 功  能：  utf8编码变为Ansi编码                                           *
 * 返回值：  非0        成功                                                *
 *           0          失败                                                *
 ****************************************************************************/
COMMONLMN_API char * Utf8ToAnsi( char * szAnsi, unsigned int dwAnsiSize, const char * szUtf8 )
{
    if ( !s_bCharSetInited )
    {
        LmnInitLock( &s_CharsetLock );
#ifndef WIN32
        s_ansi_to_utf8 = iconv_open( "utf-8", "gbk"   );
        s_utf8_to_ansi = iconv_open( "gbk",   "utf-8" );
#endif
        s_bCharSetInited = TRUE;
    }

    if ( 0 == szAnsi || 0 == dwAnsiSize || 0 == szUtf8 )
    {
        return 0;
    }

    size_t  dwSrcLen = strlen(szUtf8);

    // 不需要转换
    if ( 0 == dwSrcLen )
    {
        szAnsi[0] = '\0';
        return szAnsi;
    }

    LmnLock( &s_CharsetLock );

#ifdef WIN32	
    WCHAR wszStr[MAX_CHARSET_CONVERT_BUFFER_SIZE] = {0};
    WCHAR * pwszStr = 0;

    int nCount = MultiByteToWideChar( CP_UTF8, 0, szUtf8, dwSrcLen, 0, 0 );
    // 错误
    if ( 0 == nCount )
    {
        return 0;
    }

    if ( nCount > (int)MAX_CHARSET_CONVERT_BUFFER_SIZE )
    {
        pwszStr = (WCHAR *)g_pfnAllocMem( sizeof(WCHAR) * nCount );
        if ( 0 == pwszStr )
        {
            return 0;
        }
        memset( pwszStr, 0, sizeof(WCHAR) * nCount );
    }
    else
    {
        pwszStr = wszStr;
    }


    int nRet = MultiByteToWideChar( CP_UTF8, 0, szUtf8, dwSrcLen, pwszStr, nCount );
    // 错误
    if ( 0 == nRet )
    {
        if ( pwszStr != wszStr )
        {
            SAFE_FREE( pwszStr );
        }
        return 0;
    }

    int nLen = WideCharToMultiByte( CP_ACP, 0, pwszStr, nRet, szAnsi, dwAnsiSize - 1, 0, 0 );
    if ( 0 == nLen )
    {
        if ( pwszStr != wszStr )
        {
            SAFE_FREE( pwszStr );
        }
        return 0;
    }

    szAnsi[nLen] = '\0';

    if ( pwszStr != wszStr )
    {
        SAFE_FREE( pwszStr );
    }

    return szAnsi;

#else
    size_t dwTmpSize = dwAnsiSize - 1;
    char * pTmp      = szAnsi;

    int nRet = iconv( s_utf8_to_ansi, (char **)&szUtf8, &dwSrcLen, &pTmp, &dwTmpSize );
    if ( -1 == nRet )
    {
        return 0;
    }
    else
    {
        *pTmp = '\0';
        return szAnsi;
    }
#endif
}







/****************************************************************************
 * 函数名：  AnsiToUtf8                                                     *
 * 功  能：  Ansi编码转为utf8编码                                           *
 * 返回值：  非0        成功                                                *
 *           0          失败                                                *
 ****************************************************************************/
COMMONLMN_API char * AnsiToUtf8( char * szUtf8, unsigned int dwUtf8Size, const char * szAnsi )
{
    if ( !s_bCharSetInited )
    {
        LmnInitLock( &s_CharsetLock );
#ifndef WIN32
        s_ansi_to_utf8 = iconv_open( "utf-8", "gbk"   );
        s_utf8_to_ansi = iconv_open( "gbk",   "utf-8" );
#endif
        s_bCharSetInited = TRUE;
    }

    if ( 0 == szUtf8 || 0 == dwUtf8Size || 0 == szAnsi )
    {
        return 0;
    }

    size_t  dwSrcLen = strlen(szAnsi);

    // 不需要转换
    if ( 0 == dwSrcLen )
    {
        szUtf8[0] = '\0';
        return szUtf8;
    }

    LmnLock( &s_CharsetLock );

#ifdef WIN32	
    WCHAR wszStr[MAX_CHARSET_CONVERT_BUFFER_SIZE] = {0};
    WCHAR * pwszStr = 0;

    int nCount = MultiByteToWideChar( CP_ACP, 0, szAnsi, dwSrcLen, 0, 0 );
    // 错误
    if ( 0 == nCount )
    {
        return 0;
    }

    if ( nCount > (int)MAX_CHARSET_CONVERT_BUFFER_SIZE )
    {
        pwszStr = (WCHAR *)g_pfnAllocMem( sizeof(WCHAR) * nCount );
        if ( 0 == pwszStr )
        {
            return 0;
        }
        memset( pwszStr, 0, sizeof(WCHAR) * nCount );
    }
    else
    {
        pwszStr = wszStr;
    }


    int nRet = MultiByteToWideChar( CP_ACP, 0, szAnsi, dwSrcLen, pwszStr, nCount );
    // 错误
    if ( 0 == nRet )
    {
        if ( pwszStr != wszStr )
        {
            SAFE_FREE( pwszStr );
        }
        return 0;
    }

    int nLen = WideCharToMultiByte( CP_UTF8, 0, pwszStr, nRet, szUtf8, dwUtf8Size - 1, 0, 0 );
    // 错误
    if ( 0 == nLen )
    {
        if ( pwszStr != wszStr )
        {
            SAFE_FREE( pwszStr );
        }
        return 0;
    }

    szUtf8[nLen] = '\0';

    if ( pwszStr != wszStr )
    {
        SAFE_FREE( pwszStr );
    }

    return szUtf8;

#else
    size_t dwTmpSize = dwUtf8Size - 1;
    char * pTmp      = szUtf8;

    int nRet = iconv( s_ansi_to_utf8, (char **)&szAnsi, &dwSrcLen, &pTmp, &dwTmpSize );
    // 错误
    if ( -1 == nRet )
    {
        return 0;
    }
    else
    {
        *pTmp = '\0';
        return szUtf8;
    }
#endif
}











/*************************    时间      ********************************/
#ifndef WIN32
static DWORD  _CLK_TCK = sysconf( _SC_CLK_TCK );
#endif

COMMONLMN_API DWORD  LmnGetTickCount()                       //单位：毫秒
{
#ifdef WIN32
    return GetTickCount();
#else
    struct tms t;
    memset( &t, 0, sizeof(t) );

    DWORD ticks = (DWORD) times( &t );
    return ticks * 1000 / _CLK_TCK;
#endif
}


COMMONLMN_API void   LmnSleep( DWORD dwMiliSeconds )         //单位：毫秒
{
#ifdef WIN32
    Sleep( dwMiliSeconds );
#else
    usleep( dwMiliSeconds * 1000 );
#endif
}

/*************************  END 时间   **********************************/









/************************ 线程(带post) *************************/

// 线程对象
typedef struct tagThread
{
    POnThreadMessage          pfnOnMessage;
    PList                     pLstMsg;
    PList                     pLstDelayedMsg;
    LmnLockType               lock;
    BOOL                      bRun;
    LmnThrdType               thrd;
    BOOL                      bCreatedThrd;
    void *                    pContext;
    BOOL                      bDetached;
}Thread, *PThread;

//// 普通消息
//typedef struct tagThrdMsg
//{
//    void *                   pMsg;
//    DWORD                    dwMsgLen;
//}ThrdMsg, *PThrdMsg;

// 延时类消息
typedef struct tagDelayedThrdMsg
{
    DWORD                    dwMsgId;
    void *                   pMsg;
    DWORD                    dwMsgLen;
    DWORD                    dwTriggerTick;
}DelayedThrdMsg, *PDelayedThrdMsg;


#define  THRD_SLEEP_TIME                     20
// 注意： MAX_CHECK_DELAY_TIME MUST <= MAX_POST_DELAY_TIME
#define  MAX_CHECK_DELAY_TIME                0x5265C00      // 一天时间(单位:微秒)，最多必须在该时间里检查有无消息
#define  MAX_POST_DELAY_TIME                 0x5265C00      // 一天时间(单位:微秒)，最大的post delay message时间

// 有消息时，继续试图GetMessage
// 没有消息时， sleep一段时间
// 延时消息按照延时时间先后循序排列
static void * _LmxThread (void * pArg)
{
    Thread * pThread = (Thread *)pArg;

    PList pLstMsg = InitList();
    if ( 0 == pLstMsg )
    {
        PERROR("failed to InitList()! \n");
        return 0;
    }

    while( TRUE )
    {
        BOOL  bQuit = !pThread->bRun;


        DWORD  dwCurTick = LmnGetTickCount();

        LmnLock( &pThread->lock );

        // 检查有无延时消息到期
        PListNode pNode = GetListHead( pThread->pLstDelayedMsg );
        while( pNode )
        {
            PDelayedThrdMsg  pDelayMsg = (PDelayedThrdMsg)pNode->pData;
            // 到期的延时时间
            if (  dwCurTick - pDelayMsg->dwTriggerTick <= MAX_CHECK_DELAY_TIME )
            {
                PListNode pNewNode = Insert2ListTail( pLstMsg, pNode->pData );
                if ( 0 == pNewNode )
                {
                    PERROR("failed to Insert2ListTail()! \n");
                }

                pNode = EraseList( pThread->pLstDelayedMsg, pNode );
            }
            // 没有到期，按照list队列从小到大，认为后续也灭有到期
            else
            {
                break;                
            }
        }
        
        // 检查有无普通消息
        if ( 0 == GetListSize( pLstMsg ) )
        {
            pNode = GetListHead( pThread->pLstMsg );
            if ( pNode )
            {
                PListNode pNewNode = Insert2ListTail( pLstMsg, pNode->pData );
                if ( 0 == pNewNode )
                {
                    PERROR("failed to Insert2ListTail()! \n");
                }

                pNode = EraseList( pThread->pLstMsg, pNode );
            }
        }

        LmnUnlock( &pThread->lock );



        // sleep
        if ( 0 == GetListSize( pLstMsg ) )
        {
            if ( bQuit )
            {
                break;
            }
            LmnSleep( THRD_SLEEP_TIME );
        }
        else
        {
            BOOL bContinue = TRUE;
            pNode = GetListHead( pLstMsg );                

            while( pNode )
            {
                PDelayedThrdMsg  pDelayMsg = (PDelayedThrdMsg)pNode->pData;

                if ( bContinue )
                {
                    // 通知上层应用
                    LmxMsgData tMsg;
                    tMsg.dwMsgId  = pDelayMsg->dwMsgId;
                    tMsg.pMsg     = pDelayMsg->pMsg;
                    tMsg.dwMsgLen = pDelayMsg->dwMsgLen;
                    bContinue = pThread->pfnOnMessage( pThread, &tMsg, pThread->pContext );
                    //
                }

                // 回收内存
                if ( pDelayMsg->pMsg )
                {
                    g_pfnFreeMem( pDelayMsg->pMsg );
                }
                g_pfnFreeMem( pDelayMsg );
                
                pNode = EraseList( pLstMsg, pNode );
            }

            ClearList( pLstMsg );

            // 如果回调函数返回FALSE，跳出消息循环
            if ( !bContinue )
            {
                break;
            }

            if ( bQuit )
            {
                break;
            }
        }
    }

    DeinitList( pLstMsg );


    // 释放
    LmnLock( &pThread->lock );
    PListNode pNode = GetListHead( pThread->pLstDelayedMsg );
    while( pNode )
    {
        PDelayedThrdMsg  pDelayMsg = (PDelayedThrdMsg)pNode->pData;
        if ( pDelayMsg->pMsg )
        {
            g_pfnFreeMem( pDelayMsg->pMsg );
        }
        g_pfnFreeMem( pDelayMsg );

        pNode = GetNextListNode( pNode );
    }

    pNode = GetListHead( pThread->pLstMsg );
    while( pNode )
    {
        PDelayedThrdMsg  pDelayMsg = (PDelayedThrdMsg)pNode->pData;
        if ( pDelayMsg->pMsg )
        {
            g_pfnFreeMem( pDelayMsg->pMsg );
        }
        g_pfnFreeMem( pDelayMsg );

        pNode = GetNextListNode( pNode );
    }

    DeinitList( pThread->pLstDelayedMsg );
    DeinitList( pThread->pLstMsg );

    LmnUnlock( &pThread->lock );
    LmnDeinitLock( &pThread->lock );
    g_pfnFreeMem( pThread );
    // END (释放)


    return 0;
}


COMMONLMN_API   LmnThrdHandle   LmxCreateThread( POnThreadMessage pfnOnMessage, BOOL bCreateThrd /*= TRUE*/, void * pContext /*= 0*/, BOOL bDetach /*= FALSE*/  )
{
    // 参数不对
    if ( 0 == pfnOnMessage )
    {
        return 0;
    }

    Thread * pThread = (Thread *)g_pfnAllocMem( sizeof(Thread) );
    if ( 0 == pThread )
    {
        return 0;
    }
    
    // 初始化为0
    memset( pThread, 0, sizeof(Thread) );

    // 结构体赋值
    pThread->pfnOnMessage        = pfnOnMessage;

    pThread->pContext            = pContext;

    // 创建消息队列和延时消息队列
    pThread->pLstMsg             = InitList();
    if ( 0 == pThread->pLstMsg )
    {
        g_pfnFreeMem( pThread );
        return 0;
    }

    pThread->pLstDelayedMsg      = InitList();
    if ( 0 == pThread->pLstDelayedMsg )
    {
        DeinitList( pThread->pLstMsg );
        g_pfnFreeMem( pThread );
        return 0;
    }

    LmnInitLock( &pThread->lock );

    pThread->bRun = TRUE;

    pThread->bDetached = bDetach;


    if ( bCreateThrd )
    {
        // 创建线程
        LmnThrdType thrd = LmnCreateThread( _LmxThread, (void *)pThread, 0, bDetach );
        if ( 0 == thrd )
        {
            DeinitList( pThread->pLstDelayedMsg );
            DeinitList( pThread->pLstMsg );
            LmnDeinitLock( &pThread->lock );
            g_pfnFreeMem( pThread );
            return 0;
        }
        pThread->thrd = thrd;
        pThread->bCreatedThrd = TRUE;
    }
    
    return (LmnThrdHandle)pThread;
}

COMMONLMN_API   int LmxDestroyThread( LmnThrdHandle h )
{
    if ( 0 == h )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    Thread * pThread = (Thread *)h;

    BOOL bCreate     = pThread->bCreatedThrd;
    BOOL bDetached   = pThread->bDetached;
    LmnThrdType thrd = pThread->thrd;

    pThread->bRun = FALSE;

    // 如果创建了线程则等待该线程退出
    if ( bCreate && !bDetached )
    {
        LmnWait4Thrd( thrd );
    }

    return  LMN_ERR_OK;
}

// 对于调用LmxCreateThread时第二个参数为FALSE，主动阻塞，不停的获取消息
COMMONLMN_API int LmxThreadRun( LmnThrdHandle h )
{
    if ( 0 == h )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    Thread * pThread = (Thread *)h;

    // 如果创建了线程则认为错误
    if ( pThread->bCreatedThrd )
    {
        return LMN_ERR_INVALID_THREAD_HANDLE;
    }

    // 注意： 会阻塞在此
    // 回调函数的返回值可以控制退出
    _LmxThread( (void *)pThread );

    return 0;
}


COMMONLMN_API   int   LmxPostMessage( LmnThrdHandle h, const LmxMsgData * pMsgData )
{
    if ( 0 == h || 0 == pMsgData )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    Thread * pThread = (Thread *)h;

    LmnCommon::CFuncLock  lock( &pThread->lock );

    if ( !pThread->bRun )
    {
        return LMN_ERR_UNKNOWN;
    }

    PDelayedThrdMsg  pMsg = (PDelayedThrdMsg)g_pfnAllocMem( sizeof(DelayedThrdMsg) );
    if ( 0 == pMsg )
    {
        return LMN_ERR_NO_MEMORY;
    }
    memset( pMsg, 0, sizeof(DelayedThrdMsg) );

    if ( pMsgData->pMsg && pMsgData->dwMsgLen > 0 )
    {
        pMsg->pMsg = g_pfnAllocMem( pMsgData->dwMsgLen );
        if ( 0 == pMsg->pMsg )
        {
            g_pfnFreeMem( pMsg );
            return LMN_ERR_NO_MEMORY;
        }
        memcpy( pMsg->pMsg, pMsgData->pMsg, pMsgData->dwMsgLen );
        pMsg->dwMsgLen = pMsgData->dwMsgLen;
    }
    
    pMsg->dwMsgId = pMsgData->dwMsgId;

    PListNode pNode = Insert2ListTail( pThread->pLstMsg, pMsg );
    if ( 0 == pNode )
    {
        if ( pMsg->pMsg && pMsg->dwMsgLen )
        {
            g_pfnFreeMem( pMsg->pMsg );
        }
        g_pfnFreeMem( pMsg );
        return LMN_ERR_UNKNOWN;
    }

    return  LMN_ERR_OK;
}

COMMONLMN_API   int   LmxPostDelayMessage( LmnThrdHandle h, const LmxMsgData * pMsgData, DWORD dwDelayedMs )
{
    if ( 0 == h || 0 == pMsgData )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( dwDelayedMs > MAX_POST_DELAY_TIME )
    {
        return LMN_ERR_TOO_MUCH_DELAY_TIME;
    }

    Thread * pThread = (Thread *)h;

    PDelayedThrdMsg  pMsg = (PDelayedThrdMsg)g_pfnAllocMem( sizeof(DelayedThrdMsg) );
    if ( 0 == pMsg )
    {
        return LMN_ERR_NO_MEMORY;
    }
    memset( pMsg, 0, sizeof(DelayedThrdMsg) );

    if ( pMsgData->pMsg && pMsgData->dwMsgLen > 0 )
    {
        pMsg->pMsg = g_pfnAllocMem( pMsgData->dwMsgLen );
        if ( 0 == pMsg->pMsg )
        {
            g_pfnFreeMem( pMsg );
            return LMN_ERR_NO_MEMORY;
        }
        memcpy( pMsg->pMsg, pMsgData->pMsg, pMsgData->dwMsgLen );
        pMsg->dwMsgLen = pMsgData->dwMsgLen;
    }

    pMsg->dwMsgId = pMsgData->dwMsgId;
    pMsg->dwTriggerTick = LmnGetTickCount() + dwDelayedMs;



    LmnLock( &pThread->lock );
    // Insert2ListTail( pThread->pLstDelayedMsg, pMsg );

    PListNode pNode = GetListTail( pThread->pLstDelayedMsg );
    while ( pNode )
    {
        PDelayedThrdMsg  pDelayMsg  = (PDelayedThrdMsg)pNode->pData;
        if ( pMsg->dwTriggerTick - pDelayMsg->dwTriggerTick <= MAX_POST_DELAY_TIME + MAX_CHECK_DELAY_TIME )
        {
            break;
        }

        pNode = GetPrevListNode( pNode );
    }

    // 插入到pNode之后(如果pNode为0，则插入到头节点)
    Insert2List( pThread->pLstDelayedMsg, pMsg, pNode );

    LmnUnlock( &pThread->lock );

    return  LMN_ERR_OK;
}





/************************ END 线程(带post) *************************/



// 2 <= dwcnt <= 4
COMMONLMN_API  DWORD GenerateNumericVersion( DWORD dwCnt, ... )
{
    va_list ap;
    va_start( ap, dwCnt );

    if ( dwCnt < 2 || dwCnt > 4 )
    {
        va_end(ap);
        return 0;
    }

    DWORD dwVersion = 0;
    DWORD i;

    for ( i = 0; i < dwCnt; i++ )
    {
        DWORD dwItem = va_arg( ap, DWORD );

        if ( dwItem > 255 )
        {
            va_end(ap);
            return 0;
        }
        
        dwVersion <<= 8;
        dwVersion |= dwItem;
    }

    va_end( ap );
    return dwVersion;
}


COMMONLMN_API  char *  LmnGetVersion( char * szVersion, DWORD dwVersionSize, DWORD dwVersion, EVersionType eVer /*= VERSION_TYPE4*/ )
{
    if ( 0 == szVersion )
    {
        return 0;
    }

    if ( 0 == dwVersionSize )
    {
        szVersion[0] = '\0';
        return szVersion;
    }

    int a, b, c, d;

    a = dwVersion & 0xFF000000;
    a >>= 24;

    b = dwVersion & 0x00FF0000;
    b >>= 16;

    c = dwVersion & 0x0000FF00;
    c >>= 8;

    d = dwVersion & 0x000000FF;

    switch ( eVer )
    {
    case VERSION_TYPE3:
        SNPRINTF( szVersion, dwVersionSize - 1, "%d.%d.%d", b, c, d );
        break;
    case VERSION_TYPE2:
        SNPRINTF( szVersion, dwVersionSize - 1, "%d.%d", c, d );
        break;
    // 缺省按4个数字形式的version处理
    default:
        SNPRINTF( szVersion, dwVersionSize - 1, "%d.%d.%d.%d", a, b, c, d );
    }

    szVersion[dwVersionSize-1] = '\0';
    return szVersion;
}


static  BOOL  _IsDecimalChar( char ch )
{
    if ( ch >='0' && ch <= '9')
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


COMMONLMN_API  DWORD   LmnGetVersion( IN const char * szVersion )
{
    if ( 0 == szVersion )
    {
        return 0;
    }

    char  buf[NORMAL_BUF_SIZE];
    DWORD        dwCnt        = 0;
    const char * pLastComma   = szVersion;
    DWORD        dwVersion    = 0;

    while ( 1 )
    {
        if ( '.' == *szVersion || '\0' == *szVersion )
        {
            // 第4个逗号认为格式不对
            if ( '.' == *szVersion && 3 == dwCnt )
            {
                return 0;
            }

            DWORD  dwLen = szVersion - pLastComma;
            if ( 0 == dwLen || dwLen >= NORMAL_BUF_SIZE )
            {
                return 0;
            }

            memcpy( buf, pLastComma, dwLen );
            buf[dwLen] = '\0';

            StrTrim( buf );

            DWORD dwBufLen = strlen(buf);
            DWORD i;
            for ( i = 0; i < dwBufLen; i++ )
            {
                if ( !_IsDecimalChar(buf[i]) )
                {
                    return 0;
                }
            }

            int  nNumber = 0;
            if ( 0 != Str2Int( buf, &nNumber ) )
            {
                return 0;
            }

            if ( nNumber > 255 )
            {
                return 0;
            }
    
            dwVersion <<= 8;
            dwVersion |= nNumber;

            if ( '\0' == *szVersion )
            {
                break;
            }

            pLastComma = szVersion + 1;
            dwCnt++;
        }

        szVersion++;
    }
    
    return dwVersion;
}
















/******************************************************************************************
|  Unicode符号范围      |  UTF-8编码方式  
n |  (十六进制)           | (二进制)  
---+-----------------------+------------------------------------------------------  
1 | 0000 0000 - 0000 007F |                                              0xxxxxxx  
2 | 0000 0080 - 0000 07FF |                                     110xxxxx 10xxxxxx  
3 | 0000 0800 - 0000 FFFF |                            1110xxxx 10xxxxxx 10xxxxxx  
4 | 0001 0000 - 0010 FFFF |                   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
5 | 0020 0000 - 03FF FFFF |          111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
6 | 0400 0000 - 7FFF FFFF | 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  

                              UTF-8的编码规则
******************************************************************************************/




COMMONLMN_API  int  Unicode2Utf8( IN DWORD dwUnicode, INOUT BYTE * pOutput, INOUT DWORD * pdwOutputSize, BOOL  bLittleEndian /*= TRUE*/ )
{
    if ( 0 == pOutput || 0 == pdwOutputSize || 0 == *pdwOutputSize )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    BYTE * pSrc  = (BYTE *)(&dwUnicode);
    BYTE b1, b2, b3, b4;
    if ( bLittleEndian )
    {
        b1 = pSrc[3];
        b2 = pSrc[2];
        b3 = pSrc[1];
        b4 = pSrc[0];
    }
    else
    {
        b1 = pSrc[0];
        b2 = pSrc[1];
        b3 = pSrc[2];
        b4 = pSrc[3];
    }


    if ( dwUnicode <= 0x7F )
    {
        *pOutput = (BYTE)dwUnicode;
        *pdwOutputSize = 1;
    }
    else if ( dwUnicode <= 0x7FF )
    {
        if ( *pdwOutputSize < 2 )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        *pOutput     = (BYTE)( 0xC0 | (b3<<2) | GET_FRONT_BITS(b4, 2) );
        *(pOutput+1) = (BYTE)( 0x80 | GET_BACK_BITS(b4, 6) );

        *pdwOutputSize = 2;
    }
    else if ( dwUnicode <= 0xFFFF )
    {
        if ( *pdwOutputSize < 3 )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        *pOutput     = (BYTE)( 0xE0 | GET_FRONT_BITS(b3, 4) );
        *(pOutput+1) = (BYTE)( 0x80 | (GET_BACK_BITS(b3, 4)<<2) | GET_FRONT_BITS(b4, 2) );
        *(pOutput+2) = (BYTE)( 0x80 | GET_BACK_BITS(b4, 6) );

        *pdwOutputSize = 3;
    }
    else if ( dwUnicode <= 0x10FFFF )
    {
        if ( *pdwOutputSize < 4 )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        *pOutput     = (BYTE)( 0xF0 | GET_FRONT_BITS(b2, 6) );
        *(pOutput+1) = (BYTE)( 0x80 | (GET_BACK_BITS(b2, 2)<<4) | GET_FRONT_BITS(b3, 4) );
        *(pOutput+2) = (BYTE)( 0x80 | (GET_BACK_BITS(b3, 4)<<2) | GET_FRONT_BITS(b4, 2) );
        *(pOutput+3) = (BYTE)( 0x80 | GET_BACK_BITS(b4, 6) );

        *pdwOutputSize = 4;
    }
    else if ( dwUnicode <= 0x3FFFFFF )
    {
        if ( *pdwOutputSize < 5 )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        *pOutput     = (BYTE)( 0xF8 | b1 );
        *(pOutput+1) = (BYTE)( 0x80 | GET_FRONT_BITS(b2, 6) );
        *(pOutput+2) = (BYTE)( 0x80 | (GET_BACK_BITS(b2, 2)<<4) | GET_FRONT_BITS(b3, 4) );
        *(pOutput+3) = (BYTE)( 0x80 | (GET_BACK_BITS(b3, 4)<<2) | GET_FRONT_BITS(b4, 2) );
        *(pOutput+4) = (BYTE)( 0x80 | GET_BACK_BITS(b4, 6) );

        *pdwOutputSize = 5;
    }
    else if ( dwUnicode <= 0x7FFFFFFF )
    {
        if ( *pdwOutputSize < 6 )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        *pOutput     = (BYTE)( 0xFC | GET_FRONT_BITS(b1, 2) );
        *(pOutput+1) = (BYTE)( 0x80 | GET_BACK_BITS(b1, 6) );
        *(pOutput+2) = (BYTE)( 0x80 | GET_FRONT_BITS(b2, 6) );
        *(pOutput+3) = (BYTE)( 0x80 | (GET_BACK_BITS(b2, 2)<<4) | GET_FRONT_BITS(b3, 4) );
        *(pOutput+4) = (BYTE)( 0x80 | (GET_BACK_BITS(b3, 4)<<2) | GET_FRONT_BITS(b4, 2) );
        *(pOutput+5) = (BYTE)( 0x80 | GET_BACK_BITS(b4, 6) );

        *pdwOutputSize = 6;
    }
    else
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    return LMN_ERR_OK;
}


COMMONLMN_API  int  UnicodeStream2Utf8Stream( IN const WORD * pUnicode, IN DWORD dwUnicodeSize, 
                                             INOUT BYTE * pOutput, INOUT DWORD * pdwOutputSize, BOOL  bLittleEndian /*= TRUE*/ )
{
    if ( 0 == pUnicode || 0 == dwUnicodeSize || 0 == pOutput || 0 == pdwOutputSize || *pdwOutputSize == 0 )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    DWORD  i;
    DWORD  dwLeft = *pdwOutputSize;
    int    ret = 0;

    for ( i = 0; i < dwUnicodeSize; i++ )
    {
        if ( 0 == dwLeft )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        DWORD  dwUnicode = (DWORD)(*(pUnicode+i));
        DWORD  dwConvertSize = dwLeft;

        ret = Unicode2Utf8( dwUnicode, pOutput, &dwConvertSize, bLittleEndian );
        if ( LMN_ERR_OK != ret )
        {
            return ret;
        }

        dwLeft  -= dwConvertSize;
        pOutput += dwConvertSize;
    }

    *pdwOutputSize -= dwLeft;
    return LMN_ERR_OK;
}




COMMONLMN_API  int  Utf8ToUnicode( IN const BYTE * pUtf8, INOUT DWORD * pdwUtf8Size, OUT DWORD * pdwUnicode,  BOOL bLittleEndian /*= TRUE*/ )
{
    if ( 0 == pUtf8 || 0 == pdwUtf8Size || 0 == *pdwUtf8Size || 0 == pdwUnicode )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    DWORD  dwExpectCount = 1;
    BYTE   byIndex = 7;
    BYTE   bSrc0 = *pUtf8;
    BYTE   bDst[4] = { 0 };

    if ( !LmnCommon::IfHasBit(bSrc0, byIndex) )
    {
        *pdwUnicode = (DWORD)bSrc0;
        *pdwUtf8Size = dwExpectCount;
    }
    else
    {
        byIndex--;
        while( LmnCommon::IfHasBit(bSrc0,byIndex)  )
        {
            dwExpectCount++;

            if ( byIndex > 0 )
            {
                byIndex--;
            }
            else
            {
                break;
            }
        }

        if ( dwExpectCount > 6 )
        {
            return LMN_ERR_INVALID_FORMAT;
        }

        if ( *pdwUtf8Size < dwExpectCount )
        {
            return LMN_ERR_INVALID_FORMAT;
        }

        switch( dwExpectCount )
        {
        case 2:
            {
                bDst[2] = GET_MID_BITS(pUtf8[0], 2, 3);
                bDst[3] = (GET_BACK_BITS(pUtf8[0], 2)<<6) | GET_BACK_BITS(pUtf8[1], 6) ;
            }
            break;

        case 3:
            {
                bDst[2] = (GET_BACK_BITS(pUtf8[0],4)<<4) | GET_MID_BITS(pUtf8[1],2,4);
                bDst[3] = (GET_BACK_BITS(pUtf8[1],2)<<6) | GET_BACK_BITS(pUtf8[2], 6) ;
            }
            break;

        case 4:
            {
                bDst[1] = (GET_BACK_BITS(pUtf8[0],3)<<2) | GET_MID_BITS(pUtf8[1],4,2);
                bDst[2] = (GET_BACK_BITS(pUtf8[1],4)<<4) | GET_MID_BITS(pUtf8[2],2,4);
                bDst[3] = (GET_BACK_BITS(pUtf8[2],2)<<6) | GET_BACK_BITS(pUtf8[3], 6) ;
            }
            break;

        case 5:
            {
                bDst[0] = GET_BACK_BITS(pUtf8[0],2);
                bDst[1] = (GET_BACK_BITS(pUtf8[1],6)<<2) | GET_MID_BITS(pUtf8[2],4,2);
                bDst[2] = (GET_BACK_BITS(pUtf8[2],4)<<4) | GET_MID_BITS(pUtf8[3],2,4);
                bDst[3] = (GET_BACK_BITS(pUtf8[3],2)<<6) | GET_BACK_BITS(pUtf8[4], 6) ;
            }
            break;

        case 6:
            {
                bDst[0] = (GET_BACK_BITS(pUtf8[0],1)<<6) | GET_BACK_BITS(pUtf8[1], 6);
                bDst[1] = (GET_BACK_BITS(pUtf8[2],6)<<2) | GET_MID_BITS(pUtf8[3],4,2);
                bDst[2] = (GET_BACK_BITS(pUtf8[3],4)<<4) | GET_MID_BITS(pUtf8[4],2,4);
                bDst[3] = (GET_BACK_BITS(pUtf8[4],2)<<6) | GET_BACK_BITS(pUtf8[5], 6) ;
            }
            break;

        default:
            break;
        }

        if ( bLittleEndian )
        {
            *pdwUnicode = bDst[3] | (bDst[2]<<8) | (bDst[1]<<16) | (bDst[0]<<24);
        }
        else
        {
            *pdwUnicode = bDst[0] | (bDst[1]<<8) | (bDst[2]<<16) | (bDst[3]<<24);
        }

        *pdwUtf8Size = dwExpectCount;
    }


    return LMN_ERR_OK;
}


COMMONLMN_API  int  Utf8Stream2UnicodeStream( IN const BYTE * pUtf8, IN DWORD dwUtf8Size, 
                                             OUT WORD * pwUnicode, INOUT DWORD * pdwUnicodeSize, BOOL bLittleEndian /*= TRUE*/ )
{
    if ( 0 == pUtf8 || 0 == dwUtf8Size || 0 == pwUnicode || 0 == pdwUnicodeSize || *pdwUnicodeSize == 0 )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    DWORD  i = 0;
    int    ret = 0;
    const  BYTE * pSrc = pUtf8;
    DWORD  dwSrcSize   = dwUtf8Size;
    DWORD  dwUnicode = 0;
    DWORD  dwTmp;

    while( dwSrcSize > 0 )
    {
        if ( i >= *pdwUnicodeSize )
        {
            return LMN_ERR_NOT_ENOUGH_BUFF;
        }

        dwTmp = dwSrcSize;
        ret = Utf8ToUnicode( pSrc, &dwTmp, &dwUnicode );
        if ( LMN_ERR_OK != ret )
        {
            return ret;
        }

        pwUnicode[i] = (WORD)dwUnicode;
        i++;

        pSrc      += dwTmp;
        dwSrcSize -= dwTmp;
    }

    *pdwUnicodeSize = i;
    return LMN_ERR_OK;
}


