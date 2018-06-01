
#include <assert.h>

#include "wbParser/wbParser.h"
#include "strconv.h"

#include "tinyxml/tinyxml.h"


#if (defined(WIN32) || defined(_WIN32_WCE) )
	#include <windows.h>
#else
	#define IsBadReadPtr(p,cb) (!(p))
	#define ZeroMemory(out,len) memset((void*)out,0,len)
	#define strncpy_s( out,len,txt,txtlen ) strncpy(out, txt, txtlen)
	#define _i64toa_s(l, out, len, radix) snprintf(out, len, "%lld", l)
#endif //WIN32 || WINCE


#ifdef  __cplusplus
extern "C" {
#endif

	namespace wbParserNS
	{
#ifdef USE_REQOBJ_XML
		struct REQOBJ_XML
		{
			TiXmlElement*  curxml; 
			TiXmlDocument  doc;
		};

#define GET_XML_OBJECT(obj) \
		struct REQOBJ_XML* xmlobj = (struct REQOBJ_XML*)obj;\
		TiXmlElement *xmlElement = xmlobj->curxml;

#define RETURN_XML_OBJECT(xml) \
	xmlobj->curxml = xml; \
	return xmlobj; 

#else

#define GET_XML_OBJECT(obj) \
	TiXmlElement *xmlElement = (TiXmlElement*)(obj);

#define RETURN_XML_OBJECT(xml) \
	return (REQOBJ*)xml;

#endif
		/** 解析XML,data format : UTF8 */
		WBPARSER_API REQOBJ* Parse_data_XML(const char *data, const char *filename)
		{
#ifdef USE_REQOBJ_XML
			struct REQOBJ_XML* xml = new REQOBJ_XML();
			if( !xml )
				return NULL;

			xml->doc.Parse(data,0,TIXML_ENCODING_UTF8);
			if( xml->doc.Error() )
			{
				delete xml;
				return NULL;
			}
			xml->curxml = xml->doc.RootElement();
			return xml;
#else

			TiXmlDocument  doc;

	#if defined(ANDROID) || defined(__ANDROID__)

			doc.Parse( data, 0, TIXML_ENCODING_UTF8 );

	#else 

			CUTF82C	c_data( data );
			doc.Parse( c_data.c_str(), 0, TIXML_ENCODING_UNKNOWN );

			// printf("Parse_data_XML ==>  c_data.c_str() = \n%s\n", c_data.c_str());

	#endif

			if( doc.Error() )
			{
				printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
				return NULL;
			}
			TiXmlElement *pXml = new TiXmlElement( *doc.RootElement() );
			return pXml;
#endif
		}


		/** 释放 */
		WBPARSER_API void Parse_free_XML(REQOBJ *obj)
		{
#ifdef USE_REQOBJ_XML
			if( obj && !IsBadReadPtr(obj,sizeof(REQOBJ_XML)) )
			{
				delete (REQOBJ_XML*)obj;
				obj = 0;
			}
#else
			TiXmlElement *pElement = reinterpret_cast<TiXmlElement*>(obj);
			if( !pElement || IsBadReadPtr(pElement,sizeof(TiXmlElement)) )
				return ;
			delete pElement;
#endif
		}

#ifdef USE_REQOBJ_XML
		// object
		WBPARSER_API REQOBJ *GetChild_XML(const char *key ,REQOBJ *obj)
		{
			TiXmlElement *pElement = reinterpret_cast<TiXmlElement*>(obj);
			if( !pElement || IsBadReadPtr(pElement,sizeof(TiXmlElement)) )
				return NULL;
			return reinterpret_cast<TiXmlElement*>(pElement->FirstChildElement(key));
		}

		WBPARSER_API REQOBJ *GetSibling_XML(const char* key ,REQOBJ *obj)
		{
			TiXmlElement *pElement = reinterpret_cast<TiXmlElement*>(obj);
			if( !pElement || IsBadReadPtr(pElement,sizeof(TiXmlElement)) )
				return NULL;

			return ( key && *key != 0 ) ? (reinterpret_cast<TiXmlElement*>(pElement->NextSiblingElement(key))) :
				(reinterpret_cast<TiXmlElement*>(pElement->NextSiblingElement())) ;
		}

		WBPARSER_API char   *GetText_XML(const char  *key ,REQOBJ *obj)
		{
			TiXmlElement *pElement = reinterpret_cast<TiXmlElement*>(obj);
			if( !pElement || IsBadReadPtr(pElement,sizeof(TiXmlElement)) )
				return NULL;

			return const_cast<char*>(pElement->GetText());
		}

		WBPARSER_API char   *GetAttribute_XML(const char  *key ,REQOBJ *obj)
		{
			TiXmlElement *pElement = reinterpret_cast<TiXmlElement*>(obj);
			if( !pElement || IsBadReadPtr(pElement,sizeof(TiXmlElement)) )
				return NULL;

			TiXmlAttribute* pAttr = pElement->FirstAttribute();
			while( pAttr)
			{
				if( strcmpi(key,pAttr->Name() ) == 0 )		// Name
				{
					return const_cast<char*>(pAttr->Value());
				}
				pAttr = pAttr->Next();
			}
			return NULL;
		}
#else 

		// 计算子元素数目
		WBPARSER_API int     GetObject_XML_SIZE(REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}
			//
			TiXmlElement *pChild = xmlElement->FirstChildElement();
			int i = 0;
			while(pChild)
			{
				++ i;
				pChild = pChild->NextSiblingElement();
			}
			return i;
		}

		// 2012-02-08 add by sunfei

		// 计算指定key值对应的子元素数目
		WBPARSER_API int     GetObject_XML_Key_SIZE(const char *key ,REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}
			//
			TiXmlElement *pChild = xmlElement->FirstChildElement(key);
			int i = 0;
			while(pChild)
			{
				++ i;
				pChild = pChild->NextSiblingElement(key);
			}

			return i;
		}


		// 获取obj下第Idx个名字为key的值的子元素结构
		WBPARSER_API REQOBJ *GetObject_Key_Idx_XML(const char *key,const int idx, REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}

			TiXmlElement *pChild = xmlElement->FirstChildElement(key);
			int i = 0;
			while(pChild && i < idx)
			{
				++i;
				pChild = pChild->NextSiblingElement(key);
			}

			RETURN_XML_OBJECT(pChild);
		}

		// add end

		// 获取obj下第一个名字为key的值的子元素结构
		WBPARSER_API REQOBJ *GetObject_Key_XML(const char *key ,REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}
			TiXmlElement *xChild = xmlElement->FirstChildElement(key);
			RETURN_XML_OBJECT(xChild);
		}

		// 获取等于索引idx的子元素结构
		WBPARSER_API REQOBJ *GetObject_Idx_XML(const int idx ,REQOBJ *&obj )
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}
			TiXmlElement *pChild = xmlElement->FirstChildElement();
			int ii = 0;
			while(pChild && ii < idx )
			{
				pChild = pChild->NextSiblingElement();
				++ ii;
			}
			return pChild;
		}

		// 获取obj下第一个名字为key的值的子元素的text内容并转换成long返回
		WBPARSER_API long GetLONG_Key_XML(const char *key,REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}
			TiXmlElement *pChild = xmlElement->FirstChildElement(key);
			if( pChild ){
				// 2012-02-07 revised by sunfei 
				if (pChild->GetText() == NULL)
				{
					return 0;
				}
				else
				{
					return atol( pChild->GetText() );
				}
// 				return atol( pChild->GetText() );
				// revised end
			}
			return 0;
		}

		// 获取等于索引idx的子元素的text内容并转换成long返回
		WBPARSER_API long GetLONG_Idx_XML(const int idx ,REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return 0;
			}
			TiXmlElement *pChild = xmlElement->FirstChildElement();
			int ii = 0;
			while(pChild && ii < idx )
			{
				pChild = pChild->NextSiblingElement();
				++ ii;
			}
			if( pChild ){
				return atol( pChild->GetText() );
			}
			return 0;
		}

		// 获取obj下第一个名字为key的值的子元素的text内容并通过参数buf以字符串形式返回
		WBPARSER_API void GetCHAR_Key_XML(const char *key,REQOBJ *obj,char *out,const int len)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return ;
			}

			if( out && *out != 0 )
				ZeroMemory(out,sizeof(char) * len);

			TiXmlElement *pChild = xmlElement->FirstChildElement(key);
			if( pChild && pChild->GetText() )
			{
// 				char* outstr = 0;
//
// #if defined(WIN32) || defined(_WIN32_WCE)
// 				if( 0 == lo_Utf82C(&outstr,pChild->GetText()) )
// 					return ;
// #endif //WIN32||_WIN32_WCE
// 
// 				strncpy(out,outstr,len);
// 				free(outstr);

				strncpy(out,pChild->GetText(),len);
			}
		}

		// 获得指定key值下的text中字符串内容和字符串的长度以指针形式返回
		WBPARSER_API const char* GetCHAR_Key_XML_EX(const char *key,REQOBJ *obj, int& outlen)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  ){
				return NULL;
			}
			TiXmlElement *pChild = xmlElement->FirstChildElement(key);
			if( pChild && pChild->GetText() )  {
				outlen = strlen(pChild->GetText());
				return pChild->GetText();
			}
			return NULL;
		}

        WBPARSER_API const char* Get_XML_Name( REQOBJ *obj )
        {
            GET_XML_OBJECT(obj);
            return xmlElement->Value();
        }

		// 获取等于索引idx的子元素结构的text内容并通过参数buf以字符串形式返回
		WBPARSER_API void GetCHAR_Idx_XML(const int idx,REQOBJ *obj,char *out,const int len)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))  )
			{
				return ;
			}

			if( out && *out != 0 )
				ZeroMemory(out,sizeof(char) * len);

			//
			TiXmlElement *pChild = xmlElement->FirstChildElement();
			int ii = 0;
			while(pChild && ii < idx )
			{
				pChild = pChild->NextSiblingElement();
				++ ii;
			}

			//
			if( pChild && pChild->GetText() )
			{
				char* outstr = 0;
#if defined(WIN32) || defined(_WIN32_WCE)
				if( 0 == lo_Utf82C(&outstr,pChild->GetText()) )
					return ;
#endif //WIN32||WINCE
				strncpy(out,outstr,len);
				free(outstr);
			}
		}

		// 获取元素的text内容并转成64bit长整型返回

#ifdef _MSC_VER
		WBPARSER_API __int64 GetLong_XML(REQOBJ *obj)
#else
		WBPARSER_API long long GetLong_XML(REQOBJ *obj)
#endif
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))) {
				return 0;
			}

#ifdef _MSC_VER
			return _atoi64(xmlElement->GetText());
#else
			return atoll(xmlElement->GetText());
#endif

		}

        WBPARSER_API long       GetInt_XML(REQOBJ *obj)
        {
            GET_XML_OBJECT(obj);
            if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))) {
                return 0;
            }
            return atol( xmlElement->GetText() );
        }


		// 2012-02-07 add by sunfei


		// 获取元素的text内容并返回
		WBPARSER_API const char* GetChar_XML(REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr((xmlElement),sizeof(TiXmlElement))) {
				return 0;
			}
			return (xmlElement->GetText());
		}

		// add end

		// 获取key值指定的元素的下一个元素，若key为NULL则为当前元素的下一个元素
		WBPARSER_API REQOBJ *GetSibling_XML(const char* key ,REQOBJ *obj)
		{
			GET_XML_OBJECT(obj);
			if( !xmlElement || IsBadReadPtr(xmlElement,sizeof(TiXmlElement)) )
				return NULL;

			TiXmlElement* next = NULL;
			if( key && *key != 0 ) 
			{
				next = xmlElement->NextSiblingElement(key);
			}
			else
			{
				next = xmlElement->NextSiblingElement();
			}
			RETURN_XML_OBJECT(next);
		}
#endif //

	}
#ifdef  __cplusplus
}
#endif
