#ifndef __WB_PARSE_MACRO_H__
#define __WB_PARSE_MACRO_H__


#define PARSE_DATA(type,source,outobj)		outobj  = wbParserNS::Parse_data_##type(source)
#define PARSE_DATA_FREE(type,obj)			wbParserNS::Parse_free_##type(obj)

#define	GET_OBJECT_SIZE(type,obj,outsize)	outsize = wbParserNS::GetObject_##type##_SIZE(obj)

#define GET_LONG(type,obj,key,out)			out = wbParserNS::GetLONG_Key_##type(key,obj);
#define GET_LONG_IDX(type,obj,idx,out)		out = wbParserNS::GetLONG_Idx_##type(idx,obj);

#define GET_BOOL(type,obj,key,out)			out = (wbParserNS::GetLONG_Key_##type( key,obj) == 1 )?(WTRUE):(WFALSE);

#define GET_STR(type,obj,key,out,outlen)	 wbParserNS::GetCHAR_Key_##type(key,obj,out,outlen)
#define GET_STR_EX(type,obj,key,out,outlen)  out = wbParserNS::GetCHAR_Key_##type##_EX(key,obj,outlen)
#define GET_STR_IDX(type,obj,idx,out,outlen) wbParserNS::GetCHAR_Idx_##type(idx,obj,out,outlen)

#define GET_STR_(type,obj,key,out)	 wbParserNS::GetCHAR_Key_##type(key,obj,out,sizeof(out) )


#if defined(WIN32) || defined(WINCE)
	#define GET_LONG_TO_STR(type,obj,key,out,outlen) _ltoa( wbParserNS::GetLONG_Key_##type( key,obj ),out,10 );
#else
	#define GET_LONG_TO_STR(type,obj,key,out,outlen) sprintf( out,"%ld",GetLONG_Key_##type( key,obj ) );
#endif


#define USE_WEIBOPARSE_OBJ_PTR wbParserNS::REQOBJ *pObj = NULL;
#define GET_OBJECT(type,obj,key,out,parse)\
	{\
		pObj = wbParserNS::GetObject_Key_##type(key,obj) ;\
		if( pObj ){ out = parse(pObj);}\
	}

#define GET_OBJECT_SUNFEI(type,obj,key,out,memSize,parse)\
	{\
	pObj = wbParserNS::GetObject_Key_##type(key,obj) ;\
	if( pObj ){ out = parse(pObj,memSize);}\
	}

// 获取key值指定的元素的下一个元素，若key为NULL则为当前元素的下一个元素，并进行解析
#define GET_OBJECT_NEXT(type,obj,key,out,parse)\
	{\
		pObj = wbParserNS::GetSibling_##type(key,obj) ;\
		if( pObj ){ out = parse(pObj);}\
	}

// 获取等于索引idx的子元素结构，并进行解析
#define GET_OBJECT_IDX_PARSE(type,obj,idx,out,parse)\
	{\
		pObj =  wbParserNS::GetObject_Idx_##type(idx,obj);\
		if(pObj){out = parse(pObj);}\
	}

#define GET_OBJECT_IDX(type,obj,idx,outobj) outobj = GetObject_Idx_##type(idx,obj)

#define PARSE_T(func) CWBXmlParser::func

#endif // __WB_PARSE_MACRO_H__
