// stdafx.cpp : 只包括标准包含文件的源文件
// CreateFileBySize.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


// win32下使用wchar
FILE * LmnOpenFile( const char * szFile, const char * szMode )
{
    FILE * fp = 0;

#ifdef WIN32
    WCHAR   wszFile[256] = { 0 };
    WCHAR   wszMode[256] = { 0 };

    MultiByteToWideChar( CP_ACP,  0, szFile, strlen(szFile),  wszFile, 256 - 1 );
    MultiByteToWideChar( CP_ACP,  0, szMode, strlen(szMode),  wszMode, 256 - 1 );

    fp = _wfopen( wszFile, wszMode );
#else
    fp = fopen( szFile, szMode );
#endif

    return fp;
}