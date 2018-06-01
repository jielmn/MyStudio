#ifndef __lo_HTTP_THREAD_H__
#define __lo_HTTP_THREAD_H__


#if defined(WIN32) || defined(_WIN32)

	#define  STDCALLTYPE    __stdcall
	typedef HANDLE 		    THREAD_HANDLE;
	typedef unsigned long  	THREAD_RETURN_TYPE;

#else

	#define  STDCALLTYPE 
	typedef void*    THREAD_HANDLE;
	typedef void*    THREAD_RETURN_TYPE;

#endif

#endif //__lo_HTTP_THREAD_H__
