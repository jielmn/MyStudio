
#ifndef __wb_LOG_H__
#define __wb_LOG_H__

#include <stdio.h>

#if defined(DEBUG) || defined(_DEBUG)

	#include <assert.h>

	#define wbPRINTF printf
	#define wbASSERT(s) assert(s)

#else

	#define wbPRINTF(...)
	#define wbASSERT(s)

#endif

#endif // __wb_LOG_H__
