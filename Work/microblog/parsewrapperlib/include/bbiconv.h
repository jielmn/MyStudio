#ifndef __BB_ICONV_H__
#define __BB_ICONV_H__

#include <string>
#include <stdio.h>
#ifdef _ANDROID_
namespace std {         typedef basic_string<wchar_t> wstring; }
#endif
#ifdef __cplusplus
extern "C" {
#endif

	/** 估算[不精确] utf8 to gbk 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_utf8_2_gbk(const char* in, size_t in_len);
	extern int bbiconv_utf8_2_gbk(const char* in, size_t in_len, char* outbuffer, size_t& out_len);
	extern int bbiconv_utf8_2_gbk_s(const char* in, size_t in_len, std::string& outbuffer);


	/** 估算[不精确] gbk to utf8 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_gbk_2_utf8(const char* in, size_t in_len);
	extern int bbiconv_gbk_2_utf8(const char* in, size_t in_len, char* outbuffer, size_t& out_len);
	extern int bbiconv_gbk_2_utf8_s(const char* in, size_t in_len, std::string& outbuffer);


	/** 估算[不精确] gb2312 to utf8 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_gb2312_2_utf8(const char* in, size_t in_len);
	extern int bbiconv_gb2312_2_utf8(const char* in, size_t in_len, char* outbuffer, size_t& out_len);
	extern int bbiconv_gb2312_2_utf8_s(const char* in, size_t in_len, std::string& outbuffer);


	/** 估算[不精确] utf8 to UCS-2(UTF-16) 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_utf8_2_wchar(const char* in, size_t in_len);
	extern int bbiconv_utf8_2_wchar(const char* in, size_t in_len, wchar_t* outbuffer, size_t& out_len);
	extern int bbiconv_utf8_2_wchar_s(const char* in, size_t in_len, std::wstring& outbuffer);


	/**  估算[不精确] UCS-2(UTF-16) to utf8 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_wchar_2_utf8(const wchar_t* in ,size_t in_len);
	extern int bbiconv_wchar_2_utf8(const wchar_t* in , size_t in_len, char* outbuffer, size_t& out_len);
	extern int bbiconv_wchar_2_utf8_s(const wchar_t* in , size_t in_len, std::string& outbuffer);


	/**  估算[不精确] UCS-2(UTF-16) to gbk 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_wchar_2_gbk(const wchar_t* in, size_t in_len);
	extern int bbiconv_wchar_2_gbk(const wchar_t* in, size_t in_len, char* outbuffer, size_t& out_len);
	extern int bbiconv_wchar_2_gbk_s(const wchar_t* in, size_t in_len, std::string& outbuffer);


	/**  估算[不精确] gbk to UCS-2(UTF-16) 字符串需要的长度是多少，以便分配合理的内存 */
	extern size_t bbiconv_counts_gbk_2_wchar(const char* in, size_t in_len);
	extern int bbiconv_gbk_2_wchar(const char* in, size_t in_len, wchar_t* outbuffer, size_t& out_len);
	extern int bbiconv_gbk_2_wchar_s(const char* in, size_t in_len, std::wstring& outbuffer);

#ifdef __cplusplus
}
#endif

#endif /* __BB_ICONV_H__ */

