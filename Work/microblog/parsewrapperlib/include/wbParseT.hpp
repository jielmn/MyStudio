#ifndef __WB_PARSE_T_H__
#define __WB_PARSE_T_H__

#include <string>
#include "wbParseMacro.h"

using namespace wbParserNS;


// 解析模块模板实现
template<typename T>
class CParseGenericT
{
public:
	typedef void*(*wbParseMallocT)(int size);
	typedef void (*wbParseFreeT)(void *pvoid,int size);
	typedef void (*wbParseDataT)(wbParserNS::REQOBJ *obj,T* p, unsigned long *pmemSize);

	CParseGenericT(const char* sc,const int len,wbParseMallocT malloT,wbParseFreeT freeT,wbParseDataT parseT)
	{
		source_  = sc;
		mallocT_ = malloT;
		freeT_   = freeT;
		parseT_  = parseT;
		object_  = NULL;
	}


	T* get()const {
		return object_;
	}

protected:
	T *object_;               ///< 解析的目标
	std::string    source_;   ///< 原始串
	wbParseMallocT mallocT_;  ///< 内存分配函数
	wbParseFreeT   freeT_;    ///< 内存释放函数
	wbParseDataT   parseT_;   ///< 解析函数
};


// 多个处理的模板
template<typename T>
class CParseGenericMultiplT : public CParseGenericT<T>
{
public:

	// 2012-02-06 add by sunfei linux编译下此处不添加如下定义则会出现“错误：‘wbParseMallocT’ 未声明”的问题
	typedef void*(*wbParseMallocT)(int size);
	typedef void (*wbParseFreeT)(void *pvoid,int size);
	typedef void (*wbParseDataT)(wbParserNS::REQOBJ *obj,T* p, unsigned long *pmemSize);
	// add end

	CParseGenericMultiplT(const char* sc,const int len,wbParseMallocT malloT,wbParseFreeT freeT,wbParseDataT parseT)
		:CParseGenericT<T>(sc,len,malloT,freeT,parseT)
	{
		counts_     = 0;
	}

	virtual ~CParseGenericMultiplT() 
	{
		if( CParseGenericT<T>::freeT_)
			CParseGenericT<T>::freeT_(CParseGenericT<T>::object_,counts_);
	}

	virtual void ParseBody(int *pmemSize)
	{
		if( !CParseGenericT<T>::parseT_)
			return ;

		int idx  = 0 ;
		wbParserNS::REQOBJ* pRoot = NULL;
		PARSE_DATA(XML,CParseGenericT<T>::source_.c_str(),pRoot);
		//
		GET_OBJECT_SIZE(XML,pRoot,counts_);
		if( counts_) 
		{
			CParseGenericT<T>::object_ = (T*)CParseGenericT<T>::mallocT_(counts_);
			if(CParseGenericT<T>::object_) {
				wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,pRoot);
				while( pChild ) {
					CParseGenericT<T>::parseT_(pChild,CParseGenericT<T>::object_ + idx, pmemSize);
					OnParseObject(CParseGenericT<T>::object_ + idx);
					++idx;
					pChild = wbParserNS::GetObject_Idx_XML(idx,pRoot);
				}
			}
		}
		wbParserNS::Parse_free_XML(pRoot);	
	}

	int GetCounts() const {
		return counts_;
	}

	// 事件回调
	virtual void OnParseObject(T* pObject){
	}

protected:
	int counts_; ///< 解析目标的个数
};


// 单条处理的模板

template<typename T>
class CParseGenericSingleT : public CParseGenericT<T>
{
public:

	// 2012-02-06 add by sunfei linux编译下此处不添加如下定义则会出现“错误：‘wbParseMallocT’ 未声明”的问题
	typedef void*(*wbParseMallocT)(int size);
	typedef void (*wbParseFreeT)(void *pvoid,int size);
	typedef void (*wbParseDataT)(wbParserNS::REQOBJ *obj,T* p, unsigned long *pmemSize);
	// add end

	CParseGenericSingleT(const char* sc,const int len, wbParseMallocT malloT,wbParseFreeT freeT,wbParseDataT parseT)
		:CParseGenericT<T>(sc,len,malloT,freeT,parseT)
	{
	}

	virtual ~CParseGenericSingleT() 
	{
		if( CParseGenericT<T>::freeT_)
			CParseGenericT<T>::freeT_(CParseGenericT<T>::object_,1);
	}

	// 2012-02-27 revised by sunfei 用于计算内存分配
	virtual void ParseBody(unsigned long *pmemSize) 
	{
		if( !CParseGenericT<T>::parseT_)
			return ;

		wbParserNS::REQOBJ* pRoot = wbParserNS::Parse_data_XML(CParseGenericT<T>::source_.c_str());
		if( !pRoot ){
			return ;
		}
		//
		CParseGenericT<T>::object_ = (T*)CParseGenericT<T>::mallocT_(1);
		*pmemSize = sizeof(T);
		CParseGenericT<T>::parseT_(pRoot,CParseGenericT<T>::object_, pmemSize);

		OnParseObject(CParseGenericT<T>::object_);
		wbParserNS::Parse_free_XML(pRoot);
	}

	// 事件回调
	virtual void OnParseObject(T* pObject){
	}
};


// LIST列表处理模板

template<typename T>
class CParseGenericByCursorT : public CParseGenericT<T>
{
public:

	// 2012-02-06 add by sunfei linux编译下此处不添加如下定义则会出现“错误：‘wbParseMallocT’ 未声明”的问题
	typedef void*(*wbParseMallocT)(int size);
	typedef void (*wbParseFreeT)(void *pvoid,int size);
	typedef void (*wbParseDataT)(wbParserNS::REQOBJ *obj,T* p, unsigned long *pmemSize);
	// add end

	CParseGenericByCursorT(const char* sc,const int len,wbParseMallocT malloT,wbParseFreeT freeT,wbParseDataT parseT)
		:CParseGenericT<T>(sc,len,malloT,freeT,parseT)
	{
		counts_ = 0;
		pcur_   = NULL;
	}

	virtual ~CParseGenericByCursorT()
	{
		if( pcur_ )
			wbParserNS::wbParse_Free_Cursor(pcur_,1);

		if( CParseGenericT<T>::freeT_)
			CParseGenericT<T>::freeT_(CParseGenericT<T>::object_,counts_);
	}

	virtual void ParseBody(const char *key, unsigned long *pmemSize) 
	{
		wbParserNS::REQOBJ* pRQRoot  = wbParserNS::Parse_data_XML(CParseGenericT<T>::source_.c_str());
		wbParserNS::REQOBJ* pRQChild = wbParserNS::GetObject_Key_XML(key,pRQRoot);
		//
		if( !pRQChild){
			pRQChild = pRQRoot;
		}
		wbParserNS::REQOBJ* pCursorNext = wbParserNS::GetObject_Key_XML("next_cursor",pRQRoot);
		wbParserNS::REQOBJ* pCursorPrev = wbParserNS::GetObject_Key_XML("previous_cursor",pRQRoot);

		counts_ = wbParserNS::GetObject_XML_SIZE(pRQChild);
		if(counts_)
		{
			CParseGenericT<T>::object_ = (T*)CParseGenericT<T>::mallocT_(counts_);
			if(CParseGenericT<T>::object_)
			{
				OnParseREQ(pRQChild, pmemSize);
				if( pCursorPrev || pCursorNext )
				{
					pcur_ = (struct t_wbParse_Cursor*)wbParserNS::wbParse_Malloc_Cursor(1);
					if( pcur_ )
					{
						pcur_->prevcur = wbParserNS::GetLong_XML(pCursorPrev);
						pcur_->nextcur = wbParserNS::GetLong_XML(pCursorNext);
					}
				}
			}
		}

		if( pCursorPrev || pCursorNext ) 
		{
			pcur_ = (struct t_wbParse_Cursor*)wbParserNS::wbParse_Malloc_Cursor(1);
			if( pcur_ )
			{
				pcur_->prevcur = wbParserNS::GetLong_XML(pCursorPrev);
				pcur_->nextcur = wbParserNS::GetLong_XML(pCursorNext);
			}
		}
		wbParserNS::Parse_free_XML(pRQRoot);
	}

	t_wbParse_Cursor* GetCursor(){
		return pcur_;
	}

	int GetCounts() const {
		return counts_;
	}

	virtual void OnParseREQ(wbParserNS::REQOBJ *reqObj, int *pmemSize)
	{
		int  idx = 0;
		wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,reqObj);
		while( pChild ) 
		{
			CParseGenericT<T>::parseT_(reqObj,CParseGenericT<T>::object_ + idx, pmemSize);
			OnParseObject(CParseGenericT<T>::object_ + idx);
			++ idx;
			pChild = wbParserNS::GetObject_Idx_XML(idx,reqObj);
		}
	}

	// 事件回调
	virtual void OnParseObject(T* pObject){
	}

protected:
	int               counts_;
	t_wbParse_Cursor* pcur_;
};

#endif // __WB_PARSE_T_H__
