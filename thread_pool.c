#include <stdlib.h>
#include <stdio.h>
#include "thread_pool.h"

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
  if(queue->job_number){
    queue->has_job = 1;
  }
};

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

  if(!queue->job_number){
    queue->has_job = 0;
  }
  return first_job;
};

void threadPool_init(ThreadPool *threadPool, int thread_num){
  int idx = 0;

  // Check threadPool do exist
  if(threadPool == NULL)
    return;

  // Initial Job Queue
  threadPool->job_pool = malloc(sizeof(JobQueue));
  memset(threadPool->job_pool, 0, sizeof(JobQueue));
  pthread_mutex_init(&threadPool->job_pool->job_mutex, NULL);

  // Set thread number
  if(thread_num < 1)
    thread_num = 1;

  threadPool->thread_number = thread_num;

  // Initial thread_handler
  threadPool->thread_handler = malloc(sizeof(ThreadHandler) * thread_num);
  for(idx = 0; idx < thread_num; idx++){
    threadHandler_init(threadPool, &threadPool->thread_handler[idx]);
  }
};

void threadPool_Add_job(ThreadPool *threadPool, Job *job){
  // Prevent race condition
  pthread_mutex_lock(&threadPool->job_pool->job_mutex);
  queue_push(threadPool->job_pool, job);
  pthread_mutex_unlock(&threadPool->job_pool->job_mutex);
};

void threadHandler_init(ThreadPool *pool, ThreadHandler *thread_handler){
  thread_handler->pool = pool;

  pthread_t *pth = &(thread_handler->thread);
  pthread_create(pth, NULL, &thread_init, (void *)pool);
};

void *thread_init(void *thread_pool){
  ThreadPool *threadPool = (ThreadPool *)thread_pool;
  Job *job = NULL;

  do{
    pthread_mutex_lock(&threadPool->job_pool->job_mutex);
    int flag = threadPool->job_pool->has_job;
    pthread_mutex_unlock(&threadPool->job_pool->job_mutex);

    if(flag){    
      // Prevent race condition
      pthread_mutex_lock(&threadPool->job_pool->job_mutex);
      job = queue_pull(threadPool->job_pool);
      pthread_mutex_unlock(&threadPool->job_pool->job_mutex);

      // Handle job task
      job->job_handler(job->job_argv);
    } else {
      /*Wait for task ... */
    }

  }while(1);
}

void threadPool_join(ThreadPool *threadPool){
  int idx = 0, th_num;
  ThreadHandler *th_handler;
  
  th_num = threadPool->thread_number;

  for(idx = 0; idx < th_num; idx++){
    th_handler = &threadPool->thread_handler[idx];
    pthread_join(th_handler->thread, 0);
  }
}
