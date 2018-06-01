#ifndef __WBPARSER_H__
#define __WBPARSER_H__

#ifdef  __cplusplus
extern "C" {
#endif

#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_PARSER_STATICLIB)

	#if defined(BUILDING_LIBWBPARSER)
		#define WBPARSER_API  __declspec(dllexport)
	#else
		#define WBPARSER_API  __declspec(dllimport)
	#endif

#else 	// 其他平台的，或者是静态链接

	#define WBPARSER_API

#endif

#ifndef NULL
#define NULL  0
#endif

namespace wbParserNS
{
	typedef void REQOBJ;

	////////////////////////////////////////////////////////////////////////////////
	/** 解析XML,data format : UTF8 */
	WBPARSER_API REQOBJ* Parse_data_XML(const char* data, const char *filename  = NULL);

	/** 释放 */
	WBPARSER_API void Parse_free_XML(REQOBJ *obj);

	// 计算子元素数目
	WBPARSER_API int GetObject_XML_SIZE(REQOBJ *obj);

	// 计算指定key值对应的子元素数目
	WBPARSER_API int GetObject_XML_Key_SIZE(const char *key, REQOBJ *obj);

	// 获取obj下第Idx个名字为key的值的子元素结构
	WBPARSER_API REQOBJ *GetObject_Key_Idx_XML(const char *key, const int idx, REQOBJ *obj);

	// 获取obj下第一个名字为key的值的子元素结构
	WBPARSER_API REQOBJ *GetObject_Key_XML(const char *key, REQOBJ *obj);

	// 获取等于索引idx的子元素结构
	WBPARSER_API REQOBJ *GetObject_Idx_XML(const int idx, REQOBJ *&obj );

	// 获取obj下第一个名字为key的值的子元素的text内容并转换成long返回
	WBPARSER_API long GetLONG_Key_XML(const char *key, REQOBJ *obj);

	// 获取等于索引idx的子元素的text内容并转换成long返回
	WBPARSER_API long GetLONG_Idx_XML(const int idx, REQOBJ *obj);

	// 获取obj下第一个名字为key的值的子元素的text内容并通过参数buf以字符串形式返回 
	WBPARSER_API void GetCHAR_Key_XML(const char *key, REQOBJ *obj, char *out, const int len);

	// 获取等于索引idx的子元素结构的text内容并通过参数buf以字符串形式返回
	WBPARSER_API void GetCHAR_Idx_XML(const int idx, REQOBJ *obj, char *out, const int len);

	// 获得指定key值下的text中字符串内容和字符串的长度以指针形式返回
	WBPARSER_API const char* GetCHAR_Key_XML_EX(const char *key, REQOBJ *obj, int& outlen);

    WBPARSER_API const char* Get_XML_Name( REQOBJ *obj );
    WBPARSER_API long        GetInt_XML(REQOBJ *obj);

	// 获取元素的text内容并转成64bit长整型返回
#ifdef _MSC_VER
	WBPARSER_API __int64 GetLong_XML(REQOBJ *obj);
#else
	WBPARSER_API long long GetLong_XML(REQOBJ *obj);
#endif

	// 获取元素的text内容并返回
	WBPARSER_API const char* GetChar_XML(REQOBJ *obj);

	// 获取key值指定的元素的下一个元素，若key为NULL则为当前元素的下一个元素
	WBPARSER_API REQOBJ *GetSibling_XML(const char* key, REQOBJ *obj);

}

#ifdef  __cplusplus
}
#endif


#endif // __WBPARSER_H__
