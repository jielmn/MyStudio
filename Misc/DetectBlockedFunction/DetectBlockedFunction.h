#ifndef __DETECT_BLOCKED_FUNCTION__
#define __DETECT_BLOCKED_FUNCTION__

// 启用检查函数阻塞的开关(1开，0关)
#define   ENABLE_BLOCKED_FUNCTION_DETECT                1




#define MAX_SOURCE_FILE_NAME_SIZE                     128
#define MAX_FUNCTION_NAME_SIZE                        64

class  cDetectBlockedFunction
{
public:
    cDetectBlockedFunction( const char * szFile,  const char * szFunction, unsigned long dwLine );
    ~cDetectBlockedFunction();


    char                  m_szFile[MAX_SOURCE_FILE_NAME_SIZE];
    char                  m_szFunction[MAX_FUNCTION_NAME_SIZE];
    unsigned long         m_dwLine;
    unsigned long         m_dwTick;
};




#define  MIN_BLOCKED_ELAPSED_TIME               2000        // 多长时间才认为阻塞，单位： 毫秒
#define  DETECT_INTERVAL                        3          // 每隔多少时间进行阻塞检查，单位：秒




// 如果开启检查函数阻塞
#if ENABLE_BLOCKED_FUNCTION_DETECT


#if defined(WIN32) && WINVER <= 0x400   // vc6, vc6不支持__FUNCTION__
// 每个待检查的函数里只能放置一个
#define    BLOCKED_FUNCTION_DETECT()              cDetectBlockedFunction detect_obj_(__FILE__, "", __LINE__ )
#else // 非vc6
// 每个待检查的函数里只能放置一个
#define    BLOCKED_FUNCTION_DETECT()              cDetectBlockedFunction detect_obj_(__FILE__, __FUNCTION__, __LINE__ )
#endif  // defined(WIN32) && WINVER <= 0x400

// 每个待检查的函数里可以放置0个或多个，但必须在BLOCKED_FUNCTION_DETECT()之后
#define    BLOCKED_LINE_DETECT()                  detect_obj_.m_dwLine = __LINE__;



#else


// 每个待检查的函数里只能放置一个
#define    BLOCKED_FUNCTION_DETECT()              
// 每个待检查的函数里可以放置0个或多个，但必须在BLOCKED_FUNCTION_DETECT()之后
#define    BLOCKED_LINE_DETECT()                  


#endif // NABLE_BLOCKED_FUNCTION_DETECT










#endif //__DETECT_BLOCKED_FUNCTION__


