#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "thread_pool.h"

#define TRUE           1
#define FALSE          0

#define DEFAULT_THREAD 1

/** \brief Push a job in the job_queue
 *
 * Will push a job into the job queue and signal the pthread_condition
 *
 * \param queue  the pointer to the JobQueu structure
 * \param newJob the pointer to the Job structure
 *
 * \return NULL
 */
void queue_push(JobQueue* queue, Job* newJob){
  newJob->next_job = NULL;

  switch(queue->job_number){
    case 0:
      queue->front = newJob;
      queue->rear = newJob;
      break;
    default:
      queue->rear->next_job = newJob;
      queue->rear = newJob;
      break;
  }

  queue->job_number++;
  queue->has_job = TRUE;

  // Job is already to be processed
  pthread_cond_signal(&queue->job_cond);
};

/** \brief Pull a job in the job_queue
 *
 *  Return a pointer to the first job in job_queue and signal the
 *  pthread_condition
 *
 * \param queue  the pointer to the JobQueue structure
 *
 * \return the pointer to the Job structure
 */
Job* queue_pull(JobQueue* queue){
  Job* first_job;
 
  switch(queue->job_number){
    case 0:
      first_job = NULL;
      queue->front = NULL;
      queue->rear = NULL;
      queue->job_number = 0;
      break;
    case 1:
      first_job = queue->front;
      queue->front = NULL;
      queue->rear = NULL;
      queue->job_number = 0;
      break;
    default:
      first_job = queue->front;
      queue->front = queue->front->next_job;
      queue->job_number--;
      break;
  }

  // Make sure it has any job in job_queue
  if(!queue->job_number){
    queue->has_job = FALSE;
  } else {
    pthread_cond_signal(&queue->job_cond);
  }

  return first_job;
};

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
void threadPool_init(ThreadPool *threadPool, int thread_num){
  int idx = 0;

  // Check threadPool do exist
  if(threadPool == NULL)
    return;

  // Initial Job Queue
  threadPool->job_pool = malloc(sizeof(JobQueue));
  memset(threadPool->job_pool, 0, sizeof(JobQueue));
  pthread_mutex_init(&threadPool->job_pool->job_mutex, NULL);
  pthread_cond_init(&threadPool->job_pool->job_cond, NULL);

  // Set thread number (Default is 1)
  if(thread_num < 1){
    thread_num = DEFAULT_THREAD;
    printf("Create Default thread number ... \n");
  }
  threadPool->thread_number = thread_num;

  // Initial thread_handler
  threadPool->thread_handler = malloc(sizeof(ThreadHandler) * thread_num);
  for(idx = 0; idx < thread_num; idx++){
    threadHandler_init(threadPool, &threadPool->thread_handler[idx]);
  }
};

/** \brief Add Jobs in the Thread Pool function
 *
 *  Add the job in the job queue of the threadpool
 *
 * \param threadpool  the pointer to the threadpool structure
 * \param job         the pointer to the Job structure
 *
 * \return NULL
 */
void threadPool_Add_job(ThreadPool *threadPool, Job *job){
  // Prevent race condition
  pthread_mutex_lock(&threadPool->job_pool->job_mutex);
  queue_push(threadPool->job_pool, job);
  pthread_mutex_unlock(&threadPool->job_pool->job_mutex);
};

/** \brief Initialize threads in the thread pool
 *
 *  Will create and initialize the thread_handler (pthread) in the threadpool
 *
 * \param threadpool        the pointer to the threadpool structure
 * \param thread_handler    the pointer to the thread_handler structure
 *
 * \return NULL
 */
void threadHandler_init(ThreadPool *pool, ThreadHandler *thread_handler){
  thread_handler->pool = pool;

  pthread_t *pth = &(thread_handler->thread);
  pthread_create(pth, NULL, &thread_init, (void *)pool);
};

/** \brief Start threads function
 *
 *  Start to enable each thread handler in the threadpool
 *
 * \param threadpool  the pointer to the threadpool structure
 *
 * \return NULL
 */
void *thread_init(void *thread_pool){
  ThreadPool *threadPool = (ThreadPool *)thread_pool;
  Job *job = NULL;
  int flag = 0;

  do{
    pthread_mutex_lock(&threadPool->job_pool->job_mutex);
    // Avoiding CPU Busy Waiting
    while(!threadPool->job_pool->has_job){
      pthread_cond_wait(&threadPool->job_pool->job_cond,
                        &threadPool->job_pool->job_mutex);
    }
    flag = 1;
    pthread_mutex_unlock(&threadPool->job_pool->job_mutex);

    if(flag){    
      // Prevent race condition
      pthread_mutex_lock(&threadPool->job_pool->job_mutex);
      job = queue_pull(threadPool->job_pool);
      pthread_mutex_unlock(&threadPool->job_pool->job_mutex);

      // Handle job task
      if(job){
        job->job_handler(job->job_argv);
      }
      flag = 0;
    }
  }while(1);
}

/** \brief Thread Pool Join function
 *
 *  Wait for the threads in the threadpool has been existed.
 *
 * \param threadpool  the pointer to the threadpool structure
 *
 * \return NULL
 */
void threadPool_join(ThreadPool *threadPool){
  int idx = 0, th_num = 0;
  ThreadHandler *th_handler;
  
  th_num = threadPool->thread_number;

  // Join All pthreads 
  for(idx = 0; idx < th_num; idx++){
    th_handler = &threadPool->thread_handler[idx];
    pthread_join(th_handler->thread, 0);
  }
}
