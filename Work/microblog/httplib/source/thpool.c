
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "libhttp/thpool.h"      /* here you can also find the interface to each function */

#include "spthread.h"


static int thpool_keepalive=1;


/* ================================= STRUCTURES ================================================ */

/* Individual job */
typedef struct thpool_job_t{
	sp_thread_func_t     function;
	void*                     arg;                     /**< function's argument      */
	struct thpool_job_t*     next;                     /**< pointer to next job      */
	struct thpool_job_t*     prev;                     /**< pointer to previous job  */
}thpool_job_t;


/* Job queue as doubly linked list */
typedef struct thpool_jobqueue{
	thpool_job_t      *head;                        /**< pointer to head of queue */
	thpool_job_t      *tail;                        /**< pointer to tail of queue */
	int               jobsN;                        /**< amount of jobs in queue  */
	sp_thread_sem_t   *queueSem;                    /**< semaphore(this is probably just holding the same as jobsN) */
	sp_thread_mutex_t *queueMutex;                  /**< used to serialize queue access */
}thpool_jobqueue;


/* The threadpool */
typedef struct thpool_t{
	sp_thread_t*     threads;                          /**< pointer to threads' ID   */
	int              threadsN;                         /**< amount of threads        */
	thpool_jobqueue* jobqueue;                         /**< pointer to the job queue */
}thpool_t;


/* Container for all things that each thread is going to need */
typedef struct thread_data{                            
	sp_thread_mutex_t *mutex_p;
	thpool_t          *tp_p;
}thread_data;


/* ===========================  INNER FUNCTIONS ================================================ */

/**
 * @brief What each thread is doing
 * 
 * In principle this is an endless loop. The only time this loop gets interuppted is once
 * thpool_destroy() is invoked.
 * 
 * @param threadpool to use
 * @return nothing
 */
void thpool_thread_do(thpool_t* tp_p);

/**
 * @brief Initialize queue
 * @param  pointer to threadpool
 * @return 0 on success,
 *        -1 on memory allocation error
 */
int thpool_jobqueue_init(thpool_t* tp_p);


/**
 * @brief Add job to queue
 * 
 * A new job will be added to the queue. The new job MUST be allocated
 * before passed to this function or else other functions like thpool_jobqueue_empty()
 * will be broken.
 * 
 * @param pointer to threadpool
 * @param pointer to the new job(MUST BE ALLOCATED)
 * @return nothing 
 */
void thpool_jobqueue_add(thpool_t* tp_p, thpool_job_t* newjob_p);


/**
 * @brief Remove last job from queue. 
 * 
 * This does not free allocated memory so be sure to have peeked() \n
 * before invoking this as else there will result lost memory pointers.
 * 
 * @param  pointer to threadpool
 * @return 0 on success,
 *         -1 if queue is empty
 */
int thpool_jobqueue_removelast(thpool_t* tp_p);


/** 
 * @brief Get last job in queue (tail)
 * 
 * Gets the last job that is inside the queue. This will work even if the queue
 * is empty.
 * 
 * @param  pointer to threadpool structure
 * @return job a pointer to the last job in queue,
 *         a pointer to NULL if the queue is empty
 */
thpool_job_t* thpool_jobqueue_peek(thpool_t* tp_p);


/**
 * @brief Remove and deallocate all jobs in queue
 * 
 * This function will deallocate all jobs in the queue and set the
 * jobqueue to its initialization values, thus tail and head pointing
 * to NULL and amount of jobs equal to 0.
 * 
 * @param pointer to threadpool structure
 * */
void thpool_jobqueue_empty(thpool_t* tp_p);





/* Initialise thread pool */
loHTTP_API pthreadpool thpool_init(int threadsN){
	thpool_t* tp_p;
	sp_thread_attr_t attr;
	int t;
	
	if (!threadsN || threadsN<1) threadsN=1;

	/* Make new thread pool */
	tp_p=(thpool_t*)malloc(sizeof(thpool_t));                              /* MALLOC thread pool */
	if (tp_p==NULL){
		fprintf(stderr, "thpool_init(): Could not allocate memory for thread pool\n");
		return NULL;
	}
	tp_p->threads=(sp_thread_t*)malloc(threadsN*sizeof(sp_thread_t));          /* MALLOC thread IDs */
	if (tp_p->threads==NULL){
		fprintf(stderr, "thpool_init(): Could not allocate memory for thread IDs\n");
		return NULL;
	}
	tp_p->threadsN=threadsN;
	
	/* Initialise the job queue */
	if (thpool_jobqueue_init(tp_p)==-1){
		fprintf(stderr, "thpool_init(): Could not allocate memory for job queue\n");
		return NULL;
	}
	
	/* Initialise semaphore*/
	tp_p->jobqueue->queueSem=(sp_thread_sem_t*)malloc(sizeof(sp_thread_sem_t)); /* MALLOC job queue semaphore */
	sp_thread_sem_init(tp_p->jobqueue->queueSem);

	/* Initialise mutex*/
	tp_p->jobqueue->queueMutex=(sp_thread_mutex_t*)malloc(sizeof(sp_thread_mutex_t)); /* MALLOC job queue mutex */
	sp_thread_mutex_init(tp_p->jobqueue->queueMutex, NULL);

	sp_thread_attr_init( &attr );
	sp_thread_attr_setdetachstate( &attr, SP_THREAD_CREATE_DETACHED );
	
	/* Make threads in pool */
	for (t=0; t<threadsN; t++){
		printf("Created thread %d in pool \n", t);
		sp_thread_create(&(tp_p->threads[t]), &attr, (sp_thread_func_t)thpool_thread_do, (void *)tp_p);
	}
	
	return (pthreadpool)tp_p;
}


/* What each individual thread is doing 
 * */
/* There are two scenarios here. One is everything works as it should and second if
 * the thpool is to be killed. In that manner we try to BYPASS sem_wait and end each thread. */
void thpool_thread_do(thpool_t* tp_p){

	while(thpool_keepalive){
		
		if (sp_thread_sem_wait(tp_p->jobqueue->queueSem)) {/* WAITING until there is work in the queue */
			perror("thpool_thread_do(): Waiting for semaphore");
			exit(1);
		}

		if (thpool_keepalive){
			
			/* Read job from queue and execute it */
			sp_thread_func_t func_buff;
			void*  arg_buff;
			thpool_job_t* job_p;
	
			sp_thread_mutex_lock(tp_p->jobqueue->queueMutex);     /* LOCK */
			
			job_p = thpool_jobqueue_peek(tp_p);
			func_buff=job_p->function;
			arg_buff =job_p->arg;
			thpool_jobqueue_removelast(tp_p);
			
			sp_thread_mutex_unlock(tp_p->jobqueue->queueMutex);   /* UNLOCK */
			
			func_buff(arg_buff);               			 /* run function */
			free(job_p);                                 /* DEALLOC job */
		}
		else
		{
			return; /* EXIT thread*/
		}
	}
	return;
}


/* Add work to the thread pool */
loHTTP_API int thpool_add_work(pthreadpool tp_ptr, thread_func function_p, void* arg_p){
	thpool_job_t* newJob;
	thpool_t* tp_p = (thpool_t *)tp_ptr;
	
	newJob=(thpool_job_t*)malloc(sizeof(thpool_job_t));                        /* MALLOC job */
	if (newJob==NULL){
		fprintf(stderr, "thpool_add_work(): Could not allocate memory for new job\n");
		exit(1);
	}
	
	/* add function and argument */
	newJob->function=function_p;
	newJob->arg=arg_p;
	
	/* add job to queue */
	sp_thread_mutex_lock(tp_p->jobqueue->queueMutex);     /* LOCK */
	thpool_jobqueue_add(tp_p, newJob);
	sp_thread_mutex_unlock(tp_p->jobqueue->queueMutex);     /* LOCK */
	
	return 0;
}


/* Destroy the threadpool */
loHTTP_API void thpool_destroy(pthreadpool tp_ptr){
	int t;
	thpool_t* tp_p = (thpool_t *)tp_ptr;
	
	/* End each thread's infinite loop */
	thpool_keepalive=0; 

	printf("tp_p->threadsN = %d\n", tp_p->threadsN);
	/* Awake idle threads waiting at semaphore */
	for (t=0; t<(tp_p->threadsN); t++){
		sp_millisleep(10);
		if (sp_thread_sem_post(tp_p->jobqueue->queueSem)){
			fprintf(stderr, "thpool_destroy(): Could not bypass sem_wait()  t=%d\n", t);
		}
	}

	/* Kill semaphore */
	if (sp_thread_sem_destroy(tp_p->jobqueue->queueSem)!=0){
		fprintf(stderr, "thpool_destroy(): Could not destroy semaphore\n");
	}

	if (sp_thread_sem_unlink("mysem")!=0){
		fprintf(stderr, "thpool_destroy(): Could not unlink semaphore\n");
	}
	
	/* Kill mutex */
	if (sp_thread_mutex_destroy(tp_p->jobqueue->queueMutex)!=0){
		fprintf(stderr, "thpool_destroy(): Could not destroy mutex\n");
	}
	
	/* Wait for threads to finish */
// 	for (t=0; t<(tp_p->threadsN); t++){
// 		pthread_join(tp_p->threads[t], NULL);
// 	}
	
	thpool_jobqueue_empty(tp_p);

	printf("thpool_destroy done!\n");
	
	/* Dealloc */
	free(tp_p->threads);                                                   /* DEALLOC threads             */
	free(tp_p->jobqueue->queueSem);                                        /* DEALLOC job queue semaphore */
	free(tp_p->jobqueue->queueMutex);                                      /* DEALLOC job queue mutex     */
	free(tp_p->jobqueue);                                                  /* DEALLOC job queue           */
	free(tp_p);                                                            /* DEALLOC thread pool         */
}



/* =================== JOB QUEUE OPERATIONS ===================== */



/* Initialise queue */
int thpool_jobqueue_init(thpool_t* tp_p){
	tp_p->jobqueue=(thpool_jobqueue*)malloc(sizeof(thpool_jobqueue));      /* MALLOC job queue */
	if (tp_p->jobqueue==NULL) return -1;
	tp_p->jobqueue->tail=NULL;
	tp_p->jobqueue->head=NULL;
	tp_p->jobqueue->jobsN=0;
	return 0;
}


/* Add job to queue */
void thpool_jobqueue_add(thpool_t* tp_p, thpool_job_t* newjob_p){ /* remember that job prev and next point to NULL */

	thpool_job_t *oldFirstJob;

	newjob_p->next=NULL;
	newjob_p->prev=NULL;
	
	oldFirstJob = tp_p->jobqueue->head;
	
	/* fix jobs' pointers */
	switch(tp_p->jobqueue->jobsN){
		
		case 0:     /* if there are no jobs in queue */
					tp_p->jobqueue->tail=newjob_p;
					tp_p->jobqueue->head=newjob_p;
					break;
		
		default: 	/* if there are already jobs in queue */
					oldFirstJob->prev=newjob_p;
					newjob_p->next=oldFirstJob;
					tp_p->jobqueue->head=newjob_p;

	}

	(tp_p->jobqueue->jobsN)++;     /* increment amount of jobs in queue */
	printf("########## add ==>  jobqueue->jobsN = %d\n", tp_p->jobqueue->jobsN);

	sp_thread_sem_post(tp_p->jobqueue->queueSem);
	
// 	int sval;
// 	sem_getvalue(tp_p->jobqueue->queueSem, &sval);
}


/* Remove job from queue */
int thpool_jobqueue_removelast(thpool_t* tp_p){
	thpool_job_t *oldLastJob;
	oldLastJob = tp_p->jobqueue->tail;
	
	/* fix jobs' pointers */
	switch(tp_p->jobqueue->jobsN){
		
		case 0:     /* if there are no jobs in queue */
					return -1;
					break;
		
		case 1:     /* if there is only one job in queue */
					tp_p->jobqueue->tail=NULL;
					tp_p->jobqueue->head=NULL;
					break;
					
		default: 	/* if there are more than one jobs in queue */
					oldLastJob->prev->next=NULL;               /* the almost last item */
					tp_p->jobqueue->tail=oldLastJob->prev;
					
	}
	
	(tp_p->jobqueue->jobsN)--;
	printf("########## remove ==>  jobqueue->jobsN = %d\n", tp_p->jobqueue->jobsN);
	
// 	int sval;
// 	sem_getvalue(tp_p->jobqueue->queueSem, &sval);
	return 0;
}


/* Get first element from queue */
thpool_job_t* thpool_jobqueue_peek(thpool_t* tp_p){
	return tp_p->jobqueue->tail;
}

/* Remove and deallocate all jobs in queue */
void thpool_jobqueue_empty(thpool_t* tp_p){
	
	thpool_job_t* curjob;
	curjob=tp_p->jobqueue->tail;
	
	while(tp_p->jobqueue->jobsN){
		tp_p->jobqueue->tail=curjob->prev;
		free(curjob);
		curjob=tp_p->jobqueue->tail;
		tp_p->jobqueue->jobsN--;
	}
	
	/* Fix head and tail */
	tp_p->jobqueue->tail=NULL;
	tp_p->jobqueue->head=NULL;
}
