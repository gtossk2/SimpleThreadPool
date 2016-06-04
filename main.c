#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREAD  2

// Forward declaration
typedef void *(*Func)(void* argc);
typedef struct job Job;
typedef struct jobqueue JobQueue;
typedef struct threadPool ThreadPool;
typedef struct thread_handler ThreadHandler;

struct job{
  Job* next;
  Func handler;
  void *argc;
};

// TODO Put the jobqueue function inside structure
struct jobqueue{
  Job *front;
  Job *rear;
  int job_number;
};

struct thread_handler{
  ThreadPool *pool;
  pthread_t thread;
};

struct threadPool{
  ThreadHandler *thread;
  int thread_number;
  JobQueue *job_pool;
};

void threadPool_init(ThreadPool *threadPool, int thread_num);
void threadPool_Add_job(ThreadPool *threadPool, Job *job);
void threadPool_join(ThreadPool *threadPool);

void threadHandler_init(ThreadPool *pool, ThreadHandler *thread);

void *thread_handler(void *th_pool);
void *hello(void *argc);

void queue_push(JobQueue* queue, Job* newJob);
Job* queue_pull(JobQueue* queue);

//JobQueue jobPool = {.front = NULL, .rear = NULL, .job_number = 0};
volatile int job_flag = 0;


int main(){
//  pthread_t threads;
  int argc[5] = {1, 2, 3, 4, 5};
  Job job[5] = { {.handler = hello, .argc = &argc[0]},
                 {.handler = hello, .argc = &argc[1]},
                 {.handler = hello, .argc = &argc[2]},
                 {.handler = hello, .argc = &argc[3]},
                 {.handler = hello, .argc = &argc[4]}
               };  
/*
  int i = 0;
  for(i = 0; i < 5; ++i){
    queue_push(&jobPool, &job[i]);
  }

  pthread_create(&threads, NULL, &thread_handler, &job);

  pthread_join(threads, 0);

  printf("Job left : %d \n", jobPool.job_number);
*/  

  ThreadPool th_pool;
  threadPool_init(&th_pool, 1);
  int i = 0;
  for(i = 0; i < 5; i++){
    threadPool_Add_job(&th_pool, &job[i]);
  }

  printf("job : %d \n", th_pool.job_pool->job_number);
  
  threadPool_join(&th_pool);
  return 0;
}

void queue_push(JobQueue* queue, Job* newJob){
  newJob->next = NULL;
  
  switch(queue->job_number){
    case 0:
      queue->front = newJob;
      queue->rear = newJob;
      break;
    default:
      queue->rear->next = newJob;
      queue->rear = newJob;
      break;
  }

  queue->job_number++;
  if(queue->job_number)
    job_flag = 1;
};

Job* queue_pull(JobQueue* queue){
  Job* first_job;
  
  first_job = queue->front;
  queue->front = first_job->next;
  queue->job_number--;

  if(!queue->job_number)
    job_flag = 0;

  return first_job;
};

void threadPool_init(ThreadPool *threadPool, int thread_num){
 int idx = 0;

 // Check threadPool do exist
 if(threadPool == NULL)
  return;

 // Initial Thread_handler
  if(thread_num < 1)
    thread_num = 1;

  threadPool->thread = malloc(sizeof(thread_handler) * thread_num);
  for(idx = 0; idx < thread_num; idx++){
    threadHandler_init(threadPool, &threadPool->thread[idx]);
  }

  // Initial Job Queue
  threadPool->job_pool = malloc(sizeof(JobQueue));
};

void threadPool_Add_job(ThreadPool *threadPool, Job *job){
  queue_push(threadPool->job_pool, job);
};

void threadHandler_init(ThreadPool *pool, ThreadHandler *thread){
  printf("Hello");
  thread->pool = pool;

  printf("thread : %p \n", &thread->thread);
  pthread_create(&thread->thread, NULL, &thread_handler, (void *)pool);
};

void *thread_handler(void *th_pool){
  Job* t;
  ThreadPool *threadPool = (ThreadPool *)th_pool;
  printf("test");
  do{
    if(job_flag){    
      t = queue_pull(threadPool->job_pool);
      t->handler(t->argc);
    } else {
      printf("Thread idle ... \n");
    }
      
  }while(1);
}

void threadPool_join(ThreadPool *threadPool){
  int idx = 0;
  ThreadHandler *th_handler;

  for(idx = 0; idx < threadPool->thread_number; idx++){
    th_handler = &threadPool->thread[idx];
    pthread_join(th_handler->thread, 0);
  }
}

void *hello(void *argc){
  int *i = (int *)argc;
  printf("Hello ... %d \n", *i);
}
