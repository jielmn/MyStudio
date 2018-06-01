#ifndef _INNER_HEADER_2012_10_24_
#define _INNER_HEADER_2012_10_24_

#include <stdio.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <string.h>

//#include "libWeibo/weibo.h"

#define  WEIBO_VERSION                    "1.0.0.1"

#define  SHOW_HTTP_DATA            1






// #define DEBUG_FLAG

#ifdef DEBUG_FLAG

#ifdef WIN32
#define  PERROR(f,...)                    fprintf( stderr, f, __VA_ARGS__ )
#define  MYTRACE(f,...)                   printf( f, __VA_ARGS__ )
#else
#define  PERROR(f...)                     fprintf( stderr, f )
#define  MYTRACE(f...)                    printf( f )
#endif

#else

#ifdef WIN32
#define  PERROR(f,...)             
#define  MYTRACE(f,...)             
#else
#define  PERROR(f...)                     
#define  MYTRACE(f...)                     
#endif

#endif // DEBUG_FLAG


#ifdef WIN32
#define  SLEEP(ms)                         Sleep(ms)
#define  SNPRINTF                          _snprintf
#else
#define  SLEEP(ms)                         usleep((ms)*1000)
#define  SNPRINTF                          snprintf
#endif  // WIN32


#define  NORMAL_BUF_SIZE                   8192




/*************************       ������   **************************************/
#ifdef WIN32
typedef   CRITICAL_SECTION    LmnLockType;
#else
typedef   pthread_mutex_t     LmnLockType;
#endif

int  LmnInitLock  ( LmnLockType * pLock );
int  LmnDeinitLock( LmnLockType * pLock );
int  LmnLock( LmnLockType * pLock );
int  LmnUnlock( LmnLockType * pLock );
/*************************    END ������   **************************************/


#ifdef WIN32
typedef HANDLE          LmnThrdType;
#else
typedef pthread_t       LmnThrdType;
#endif

typedef  void * (*LmnThreadFunc)(void *);

// ����ֵ�� �ɹ�����0
LmnThrdType LmnCreateThread( LmnThreadFunc pfnThrd, void * pParam, DWORD dwStackSize );
void *      LmnWait4Thrd( LmnThrdType hThrd );



DWORD  LmnGetTickCount();                       //��λ������


typedef void * (*AllocMemCb)( size_t dwSize );
typedef void   (*FreeMemCb)( void *  pMemblock );

extern AllocMemCb  g_pfnAllocMem;
extern FreeMemCb   g_pfnFreeMem;




class  CDataBuf
{
public:
    CDataBuf( void * pData = 0, DWORD dwDataLen = 0 );
    ~CDataBuf();
    BOOL Append( const void * pData, DWORD dwDataLen );                        // д�����ݵ�ĩβ
    void Clear();

    BOOL Read( void * pData, DWORD dwDataSize = -1 );                    // �ӵ�ǰ��λ�ÿ�ʼ��ȡ����
    BOOL ReadAndShift( void * pData, DWORD dwDataSize = -1 );            // �ӵ�ǰ��λ�ÿ�ʼ��ȡ���ݲ��Ѷ�ȡ�Ķ�����
    void ResetReadPos();                                                 // �Ѷ�����Ϊ0

    DWORD         GetDataLength() const;
    const BYTE *  GetData() const;
    DWORD         GetReadPos() const;

private:
    // ���Զ���ķ���������Ҫ���ʵ�ʷ����ڴ��С
    static DWORD CalcReqBufLen( DWORD dwReqLen );

    BYTE *   m_pData;                                       // ����
    DWORD    m_dwDataBufLen;                                // ����buf����
    DWORD    m_dwDataLen;                                   // ��Ч���ݳ���
    DWORD    m_dwRPos;                                      // ��ǰ����λ��
};


class  CFuncLock
{
public:
    CFuncLock( LmnLockType * pLock );
    ~CFuncLock();

private:
    LmnLockType *  m_pLock;
};

char     TOLOWER( char ch );
int      STRICMP( const char * s1, const char * s2 );
int      STRNICMP( const char * s1, const char * s2, unsigned long dwCnt );
char *   StrTrim( char * szStr );
char *   Str2Lower( char * szStr );


int  EncodeBase64( void  *  pBase64, DWORD *  pdwSize, void  *  pSrc, DWORD    dwSrcLen );
int  DecodeBase64(    void  * pDest, DWORD * pdwSize, void  * pBase64, DWORD   dwBase64Len  );


#define  PROXYTYPE_NONE          0
#define  PROXYTYPE_SOCK5         1
#define  PROXYTYPE_HTTP          2


//class   CGlobalDataWeibo
//{
//public:
//    static CGlobalDataWeibo * GetInstance()
//    {
//        if ( 0 == s_pInstance )
//        {
//            s_pInstance = new CGlobalDataWeibo();
//        }
//
//        return s_pInstance;
//    }
//
//    ~CGlobalDataWeibo()
//    {
//        LmnDeinitLock( &m_lock );
//    }
//
//    void GetCommonProxyParam( PTWeiboProxyParam  pParam )
//    {
//        CFuncLock theLock ( &m_lock );
//        memcpy( pParam, &m_proxy_data,  sizeof(TWeiboProxyParam) );
//    }
//
//    void SetCommonProxyParam( const TWeiboProxyParam * pParam )
//    {
//        CFuncLock theLock ( &m_lock );
//        memcpy( &m_proxy_data, pParam, sizeof(TWeiboProxyParam) );
//    }
//
//    const char * GetCompileTime()
//    {
//        return m_szCompileTime;
//    }
//
//private:
//
//    CGlobalDataWeibo()
//    {
//        LmnInitLock( &m_lock );
//
//        memset( &m_proxy_data, 0, sizeof(m_proxy_data) );
//
//        SNPRINTF( m_szCompileTime, 128, "%s %s", __DATE__, __TIME__ );
//    }
//    
// 
//    LmnLockType               m_lock;
//    TWeiboProxyParam          m_proxy_data;
//    char                      m_szCompileTime[128];
//
//    static    CGlobalDataWeibo *   s_pInstance;
//};




// 2012-05-03 revised by sunfei ���ӶԶ��ļ��Ĵ�����ʱ����Ϊ10���ļ���
struct t_wb_upload_form
{
    char   file_formname[64];                     // ������
    int    fileCount;                             // �ļ�������ͼƬ���߸�����
    char   file[10][256];                         // ����·���ļ���

    char   postcontent[4096];                     // ���е�post����
    char*  data;
    long   fSize;
};
// revised end


FILE * LmnOpenFile   ( const char * szFile, const char * szMode );
long   LmnGetFileSize( const char * szFile );

int     GetDigitalFromChar( char ch );
char    GetCharFromUrl( char ch1, char ch2 );
char *  GetStringFromUrl( char * szDest, DWORD dwDestSize, const char * szUrl );



//#if defined(WIN32)
//
//#ifdef MICROBLOG_EXPORTS
//#define MICROBLOG_API __declspec(dllexport)
//#else
//#define MICROBLOG_API __declspec(dllimport)
//#endif
//
//#else
//
//#define MICROBLOG_API
//
//#endif //defined(WIN32)


typedef void (*POnMicroblogCallback) ( const char * szLog );

void  SetLogCallback( POnMicroblogCallback pfnLog );

void  MicroblgLog( const char * szFormat, ... );

#ifdef WIN32
#define MICROBLOG_LOG(f,...)           MicroblgLog( f, __VA_ARGS__ )
#else
#define MICROBLOG_LOG(f...)            MicroblgLog( f )
#endif




extern  LmnLockType    g_Lock;
void    InitGLock();


char * GetIpAddr( char * szBuf, unsigned int dwSize, unsigned int dwIp );




/*******************************************************************************/
// ����: �鿴����(char|unsigned char|short|unsigned short|int|unsigned int��
//       �Ƿ�������ĳλ��λ��������0��ʼ��
// ����ֵ: 
//          TRUE,  ������
//          FALSE, û������(���������)
/*******************************************************************************/
template <class T>
bool IfHasBit(T num, BYTE index)
{
    if ( index >= sizeof(T) * 8 )
    {
        return false;
    }

    if ( 0 == ( num & ((T)1<<index) ) )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*******************************************************************************/
// ����ĳλ��ֵ                                                        
/*******************************************************************************/
template <class T>
void SetBit(T& num, BYTE index, bool bSet = true)
{
    if ( index >= sizeof(T) * 8 )
    {
        return;
    }

    if ( bSet )
    {
        num |= (((T)0x01)<<(index));
    }
    else
    {
        num &= (~(((T)0x01)<<(index)));
    }
}


// extern fun_wb_header_callback  g_pfnHeaderCallback;

#define GET_MIN(a, b)                      ((a)<(b)?(a):(b))


int StrReplace(  char * szDest, DWORD dwSize, const char * szSource,  const char * szToReplace, const char * szReplaceWith  );




#endif




