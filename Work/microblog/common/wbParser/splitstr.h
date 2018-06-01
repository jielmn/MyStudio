#ifndef __SPLITSTR_H__
#define __SPLITSTR_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>


#ifdef  __cplusplus
extern "C" {
#endif


#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_PARSER_STATICLIB)

	#if defined(BUILDING_LIBWBPARSER)
		#define WBPARSER_API  __declspec(dllexport)
	#else
		#define WBPARSER_API  __declspec(dllimport)
	#endif

#else // 其他平台的，或者是静态链接

	#define WBPARSER_API

#endif

WBPARSER_API int  split_url_param(const char*& next , const char*& key , const char*& qe , const char*& split ,const char endflag );
WBPARSER_API int  split_url_copy_keyval(char* val , const char* start , const char* end);

WBPARSER_API int  wb_parse_oauth(const char *reply,char *token,char *secret, char* userid );
WBPARSER_API bool wb_readline( const char *reply , const char* &linestart, const char* &lineend );

WBPARSER_API int  wb_get_cookie(const char* reply , char *token, char *secret );
WBPARSER_API int  wb_parse_cookie( const char *reply , char *token, char *secret );

WBPARSER_API int  wb_get_cookie_keda(const char* reply , char *p_jsessionid );
WBPARSER_API int  wb_parse_cookie_keda( const char *reply , char p_jsessionid[][512], int num );

/** 错误分析 */
WBPARSER_API void wb_parse_error(const char *reply, char *request , char* error ,  char *error_code );

/** 短链接 */
WBPARSER_API int  wb_split_shortlink(const char* txt , const char* szEnd , const char*& start ,const char*& end , const char*& shortID);

#ifdef  __cplusplus
}
#endif

#endif // __SPLITSTR_H__

