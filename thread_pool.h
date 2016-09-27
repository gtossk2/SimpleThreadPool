#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

// Forward declaration
typedef void *(*Job_Handler)(void* job_argv);
typedef struct job Job;
typedef struct jobqueue JobQueue;
typedef struct threadPool ThreadPool;
typedef struct _thread_handler ThreadHandler;

// Structure Definition
struct job{
  Job* next_job;
  Job_Handler job_handler;
  void *job_argv;
};

struct jobqueue{
  Job *front;
  Job *rear;
  int job_number;
  int has_job;
  pthread_mutex_t job_mutex;
  pthread_cond_t job_cond;
};

struct _thread_handler{
  ThreadPool *pool;
  pthread_t thread;
};

struct threadPool{
  ThreadHandler *thread_handler;
  int thread_number;
  JobQueue *job_pool;
};

//
// Thread Pool API
//

/** \brief Thread Pool Initialize function
 *
 *  This function only create the threads inside the threadpool structure.
 *  You have instanced the threadpool and pass the pointer to function.
 *
 * \param threadpool  the pointer to the threadpool structure
 * \param thread_num  The thread number will be created
 *
 * \return NULL
 */
void threadPool_init(ThreadPool *threadPool, int thread_num);

/** \brief Thread Pool Free function
 *
 *  This function will free the memory allocated by thread pool
 *
 * \param threadpool  the pointer to the threadpool structure
 *
 * \return NULL
 */
void threadPool_free(ThreadPool *threadpool);

/** \brief Add Jobs in the Thread Pool function
 *
 *  Add the job in the job queue of the threadpool
 *
 * \param threadpool  the pointer to the threadpool structure
 * \param job         the pointer to the Job structure
 *
 * \return NULL
 */
void threadPool_Add_job(ThreadPool *threadPool, Job *job);

/** \brief Thread Pool Join function
 *
 *  Wait for the threads in the threadpool has been existed.
 *
 * \param threadpool  the pointer to the threadpool structure
 *
 * \return NULL
 */
void threadPool_join(ThreadPool *threadPool);

//
// Thread Handle API (Internal)
//

/** \brief Initialize threads in the thread pool
 *
 *  Will create and initialize the thread_handler (pthread) in the threadpool
 *
 * \param threadpool        the pointer to the threadpool structure
 * \param thread_handler    the pointer to the thread_handler structure
 *
 * \return NULL
 */
void threadHandler_init(ThreadPool *pool, ThreadHandler *thread_handler);

/** \brief Start threads function
 *
 *  Start to enable each thread handler in the threadpool
 *
 * \param threadpool  the pointer to the threadpool structure
 *
 * \return NULL
 */
void *thread_init(void *thread_pool);

//
// Job Queue API
// 

/** \brief Push a job in the job_queue
 *
 * Will push a job into the job queue and signal the pthread_condition
 *
 * \param queue  the pointer to the JobQueu structure
 * \param newJob the pointer to the Job structure
 *
 * \return NULL
 */
void queue_push(JobQueue* queue, Job* newJob);

/** \brief Pull a job in the job_queue
 *
 *  Return a pointer to the first job in job_queue and signal the
 *  pthread_condition
 *
 * \param queue  the pointer to the JobQueue structure
 *
 * \return the pointer to the Job structure
 */
Job* queue_pull(JobQueue* queue);

#endif
