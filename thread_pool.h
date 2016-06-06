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

// Thread Pool API
void threadPool_init(ThreadPool *threadPool, int thread_num);
void threadPool_Add_job(ThreadPool *threadPool, Job *job);
void threadPool_join(ThreadPool *threadPool);

// Thread Handle API
void threadHandler_init(ThreadPool *pool, ThreadHandler *thread_handler);
void *thread_init(void *thread_pool);

// Job Queue API
void queue_push(JobQueue* queue, Job* newJob);
Job* queue_pull(JobQueue* queue);

#endif
