
#ifndef _THPOOL_
#define _THPOOL_

#ifdef  __cplusplus
extern "C" {
#endif

#if (defined(WIN32) || defined(_WIN32) ) && !defined(HTTP_STATICLIB)

	#if defined(LIBHTTP_DLL)
		#define loHTTP_API  __declspec(dllexport)
	#else
		#define loHTTP_API  __declspec(dllimport)
	#endif

#else  // 其他平台的，或者是静态链接

	#define loHTTP_API 

#endif


typedef void * pthreadpool;

#if defined(WIN32) || defined(_WIN32)
	typedef unsigned long ( __stdcall * thread_func )(void *);
#else
	typedef void* (*thread_func)(void *);
#endif

/**
 * @brief  Initialize threadpool
 * 
 * Allocates memory for the threadpool, jobqueue, semaphore and fixes 
 * pointers in jobqueue.
 * 
 * @param  number of threads to be used
 * @return threadpool struct on success,
 *         NULL on error
 */
loHTTP_API pthreadpool thpool_init(int threadsN);


/**
 * @brief Add work to the job queue
 * 
 * Takes an action and its argument and adds it to the threadpool's job queue.
 * If you want to add to work a function with more than one arguments then
 * a way to implement this is by passing a pointer to a structure.
 * 
 * ATTENTION: You have to cast both the function and argument to not get warnings.
 * 
 * @param  threadpool to where the work will be added to
 * @param  function to add as work
 * @param  argument to the above function
 * @return int
 */
 loHTTP_API int thpool_add_work(pthreadpool tp_p, thread_func function_p, void* arg_p);


/**
 * @brief Destroy the threadpool
 * 
 * This will 'kill' the threadpool and free up memory. If threads are active when this
 * is called, they will finish what they are doing and then they will get destroyied.
 * 
 * @param threadpool a pointer to the threadpool structure you want to destroy
 */
loHTTP_API void thpool_destroy(pthreadpool tp_p);


#ifdef  __cplusplus
}
#endif


#endif
