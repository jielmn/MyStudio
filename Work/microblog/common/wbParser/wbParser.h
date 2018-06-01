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

#else 	// ����ƽ̨�ģ������Ǿ�̬����

	#define WBPARSER_API

#endif

#ifndef NULL
#define NULL  0
#endif

namespace wbParserNS
{
	typedef void REQOBJ;

	////////////////////////////////////////////////////////////////////////////////
	/** ����XML,data format : UTF8 */
	WBPARSER_API REQOBJ* Parse_data_XML(const char* data, const char *filename  = NULL);

	/** �ͷ� */
	WBPARSER_API void Parse_free_XML(REQOBJ *obj);

	// ������Ԫ����Ŀ
	WBPARSER_API int GetObject_XML_SIZE(REQOBJ *obj);

	// ����ָ��keyֵ��Ӧ����Ԫ����Ŀ
	WBPARSER_API int GetObject_XML_Key_SIZE(const char *key, REQOBJ *obj);

	// ��ȡobj�µ�Idx������Ϊkey��ֵ����Ԫ�ؽṹ
	WBPARSER_API REQOBJ *GetObject_Key_Idx_XML(const char *key, const int idx, REQOBJ *obj);

	// ��ȡobj�µ�һ������Ϊkey��ֵ����Ԫ�ؽṹ
	WBPARSER_API REQOBJ *GetObject_Key_XML(const char *key, REQOBJ *obj);

	// ��ȡ��������idx����Ԫ�ؽṹ
	WBPARSER_API REQOBJ *GetObject_Idx_XML(const int idx, REQOBJ *&obj );

	// ��ȡobj�µ�һ������Ϊkey��ֵ����Ԫ�ص�text���ݲ�ת����long����
	WBPARSER_API long GetLONG_Key_XML(const char *key, REQOBJ *obj);

	// ��ȡ��������idx����Ԫ�ص�text���ݲ�ת����long����
	WBPARSER_API long GetLONG_Idx_XML(const int idx, REQOBJ *obj);

	// ��ȡobj�µ�һ������Ϊkey��ֵ����Ԫ�ص�text���ݲ�ͨ������buf���ַ�����ʽ���� 
	WBPARSER_API void GetCHAR_Key_XML(const char *key, REQOBJ *obj, char *out, const int len);

	// ��ȡ��������idx����Ԫ�ؽṹ��text���ݲ�ͨ������buf���ַ�����ʽ����
	WBPARSER_API void GetCHAR_Idx_XML(const int idx, REQOBJ *obj, char *out, const int len);

	// ���ָ��keyֵ�µ�text���ַ������ݺ��ַ����ĳ�����ָ����ʽ����
	WBPARSER_API const char* GetCHAR_Key_XML_EX(const char *key, REQOBJ *obj, int& outlen);

    WBPARSER_API const char* Get_XML_Name( REQOBJ *obj );
    WBPARSER_API long        GetInt_XML(REQOBJ *obj);

	// ��ȡԪ�ص�text���ݲ�ת��64bit�����ͷ���
#ifdef _MSC_VER
	WBPARSER_API __int64 GetLong_XML(REQOBJ *obj);
#else
	WBPARSER_API long long GetLong_XML(REQOBJ *obj);
#endif

	// ��ȡԪ�ص�text���ݲ�����
	WBPARSER_API const char* GetChar_XML(REQOBJ *obj);

	// ��ȡkeyֵָ����Ԫ�ص���һ��Ԫ�أ���keyΪNULL��Ϊ��ǰԪ�ص���һ��Ԫ��
	WBPARSER_API REQOBJ *GetSibling_XML(const char* key, REQOBJ *obj);

}

#ifdef  __cplusplus
}
#endif


#endif // __WBPARSER_H__
