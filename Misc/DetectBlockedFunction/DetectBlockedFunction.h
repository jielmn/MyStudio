#ifndef __DETECT_BLOCKED_FUNCTION__
#define __DETECT_BLOCKED_FUNCTION__

// ���ü�麯�������Ŀ���(1����0��)
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




#define  MIN_BLOCKED_ELAPSED_TIME               2000        // �೤ʱ�����Ϊ��������λ�� ����
#define  DETECT_INTERVAL                        3          // ÿ������ʱ�����������飬��λ����




// ���������麯������
#if ENABLE_BLOCKED_FUNCTION_DETECT


#if defined(WIN32) && WINVER <= 0x400   // vc6, vc6��֧��__FUNCTION__
// ÿ�������ĺ�����ֻ�ܷ���һ��
#define    BLOCKED_FUNCTION_DETECT()              cDetectBlockedFunction detect_obj_(__FILE__, "", __LINE__ )
#else // ��vc6
// ÿ�������ĺ�����ֻ�ܷ���һ��
#define    BLOCKED_FUNCTION_DETECT()              cDetectBlockedFunction detect_obj_(__FILE__, __FUNCTION__, __LINE__ )
#endif  // defined(WIN32) && WINVER <= 0x400

// ÿ�������ĺ�������Է���0����������������BLOCKED_FUNCTION_DETECT()֮��
#define    BLOCKED_LINE_DETECT()                  detect_obj_.m_dwLine = __LINE__;



#else


// ÿ�������ĺ�����ֻ�ܷ���һ��
#define    BLOCKED_FUNCTION_DETECT()              
// ÿ�������ĺ�������Է���0����������������BLOCKED_FUNCTION_DETECT()֮��
#define    BLOCKED_LINE_DETECT()                  


#endif // NABLE_BLOCKED_FUNCTION_DETECT










#endif //__DETECT_BLOCKED_FUNCTION__


