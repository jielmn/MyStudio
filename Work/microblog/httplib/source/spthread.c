#include "spthread.h"


#if defined(WIN32) || defined(_WIN32)

	int sp_thread_mutex_init( sp_thread_mutex_t * mutex, void * attr )
	{
		*mutex = CreateMutex( NULL, FALSE, NULL );
		return NULL == * mutex ? GetLastError() : 0;
	}

	int sp_thread_mutex_destroy( sp_thread_mutex_t * mutex )
	{
		int ret = CloseHandle( *mutex );
		return 0 == ret ? GetLastError() : 0;
	}

	int sp_thread_mutex_lock( sp_thread_mutex_t * mutex )
	{
		int ret = WaitForSingleObject( *mutex, INFINITE );
		return WAIT_OBJECT_0 == ret ? 0 : GetLastError();
	}

	int sp_thread_mutex_unlock( sp_thread_mutex_t * mutex )
	{
		int ret = ReleaseMutex( *mutex );
		return 0 != ret ? 0 : GetLastError();
	}

	int sp_thread_cond_init( sp_thread_cond_t * cond, void * attr )
	{
		*cond = CreateEvent( NULL, FALSE, FALSE, NULL );
		return NULL == *cond ? GetLastError() : 0;
	}

	int sp_thread_cond_destroy( sp_thread_cond_t * cond )
	{
		int ret = CloseHandle( *cond );
		return 0 == ret ? GetLastError() : 0;
	}

	/*
	Caller MUST be holding the mutex lock; the
	lock is released and the caller is blocked waiting
	on 'cond'. When 'cond' is signaled, the mutex
	is re-acquired before returning to the caller.
	*/
	int sp_thread_cond_wait( sp_thread_cond_t * cond, sp_thread_mutex_t * mutex )
	{
		int ret = 0;

		sp_thread_mutex_unlock( mutex );

		ret = WaitForSingleObject( *cond, INFINITE );

		sp_thread_mutex_lock( mutex );

		return WAIT_OBJECT_0 == ret ? 0 : GetLastError();
	}

	int sp_thread_cond_signal( sp_thread_cond_t * cond )
	{
		int ret = SetEvent( *cond );
		return 0 == ret ? GetLastError() : 0;
	}

	sp_thread_t sp_thread_self()
	{
		return GetCurrentThreadId();
	}

	int sp_thread_sem_init( sp_thread_sem_t * sem )
	{
		*sem = CreateSemaphore( NULL, 0, 200, NULL );
		return NULL == *sem ? GetLastError() : 0;
	}
	
	int sp_thread_sem_destroy( sp_thread_sem_t * sem )
	{
		int ret = CloseHandle( *sem );
		return 0 == ret ? GetLastError() : 0;
	}
	
	int sp_thread_sem_wait( sp_thread_sem_t * sem )
	{
		int ret = WaitForSingleObject( *sem, INFINITE );
		return WAIT_OBJECT_0 == ret ? 0 : GetLastError();
	}
	
	int sp_thread_sem_post( sp_thread_sem_t * sem )
	{
		int ret = ReleaseSemaphore( *sem, 1, NULL );
		return 0 == ret ? GetLastError() : 0;
	}

	int sp_thread_sem_unlink( const char *name )
	{
		return 0;
	}

	int sp_thread_attr_init( sp_thread_attr_t * attr )
	{
		*attr = 0;
		return 0;
	}

	int sp_thread_attr_setdetachstate( sp_thread_attr_t * attr, int detachstate )
	{
		*attr |= detachstate;
		return 0;
	}

	int sp_thread_create( sp_thread_t * thread, sp_thread_attr_t * attr, sp_thread_func_t myfunc, void * args )
	{
		// _beginthreadex returns 0 on an error
		HANDLE h = (HANDLE)_beginthreadex( NULL, 0, myfunc, args, 0, thread );
		CloseHandle(h);
		return h > 0 ? 0 : GetLastError();
	}

#else ///////////////////////////////////////////////////////////////////////

	#ifdef _IOS_

		int sp_thread_sem_init( sp_thread_sem_t * sem )
		{
			sem = sem_open( "mysem", O_CREAT, 0644, 200 );  
			return SEM_FAILED == sem ? -1 : 0;
		}

	#else

	#endif

#endif


