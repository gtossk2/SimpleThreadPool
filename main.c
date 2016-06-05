#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

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
  int has_job;
};

struct thread_handler{
  ThreadPool *pool;
  pthread_t thread;
};

struct threadPool{
  ThreadHandler *thread;
  int thread_number;
  JobQueue *job_pool;
  pthread_mutex_t thPool_mutex;
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
pthread_mutex_t mutex;

int main(){
//  pthread_t threads;
  int argc[5] = {1, 2, 3, 4, 5};
  Job job[5] = { {.handler = hello, .argc = &argc[0]},
                 {.handler = hello, .argc = &argc[1]},
                 {.handler = hello, .argc = &argc[2]},
                 {.handler = hello, .argc = &argc[3]},
                 {.handler = hello, .argc = &argc[4]}
               };   

  pthread_mutex_init(&mutex, NULL);

  ThreadPool th_pool;
  threadPool_init(&th_pool, 2);
  int i = 0;
  for(i = 0; i < 5; i++){
    threadPool_Add_job(&th_pool, &job[i]);
  }

  printf("job : %d \n", th_pool.job_pool->job_number);
  
  threadPool_join(&th_pool);

  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&th_pool.thPool_mutex);
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
  if(queue->job_number){
    queue->has_job = 1;
  }
};

Job* queue_pull(JobQueue* queue){
  Job* first_job;
  
  first_job = queue->front;
  queue->front = first_job->next;
  queue->job_number--;

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

  pthread_mutex_init(&(threadPool->thPool_mutex), NULL);

  // Initial Job Queue
  threadPool->job_pool = malloc(sizeof(JobQueue));
  memset(threadPool->job_pool, 0, sizeof(JobQueue));

  // Initial Thread_handler
  if(thread_num < 1)
    thread_num = 1;

  pthread_mutex_lock(&(threadPool->thPool_mutex));
  threadPool->thread_number = thread_num;
  pthread_mutex_unlock(&(threadPool->thPool_mutex));

  threadPool->thread = malloc(sizeof(thread_handler) * thread_num);
  for(idx = 0; idx < thread_num; idx++){
    threadHandler_init(threadPool, &threadPool->thread[idx]);
  }
};

void threadPool_Add_job(ThreadPool *threadPool, Job *job){
  pthread_mutex_lock(&mutex);
  queue_push(threadPool->job_pool, job);
  pthread_mutex_unlock(&mutex);
};

void threadHandler_init(ThreadPool *pool, ThreadHandler *thread){
  int s;
  thread->pool = pool;
  pthread_t *pth = &(thread->thread);
  s = pthread_create(pth, NULL, &thread_handler, (void *)pool);
  printf("S : %d\n", s);
};

void *thread_handler(void *th_pool){
  Job* t;
  ThreadPool *threadPool = (ThreadPool *)th_pool;

  do{
    pthread_mutex_lock(&mutex);
    int flag = threadPool->job_pool->has_job;
    pthread_mutex_unlock(&mutex);

    if(flag){    
      pthread_mutex_lock(&mutex);
      t = queue_pull(threadPool->job_pool);
      pthread_mutex_unlock(&mutex);
      t->handler(t->argc);
    } else {
      /*Wait for task ... */
    }

  }while(1);
}

void threadPool_join(ThreadPool *threadPool){
  int idx = 0, th_num;
  ThreadHandler *th_handler;
  
  pthread_mutex_lock(&(threadPool->thPool_mutex));
  th_num = threadPool->thread_number;
  pthread_mutex_unlock(&(threadPool->thPool_mutex));

  for(idx = 0; idx < th_num; idx++){
    th_handler = &threadPool->thread[idx];
    pthread_join(th_handler->thread, 0);
  }
}

void *hello(void *argc){
  int *i = (int *)argc;
  printf("Hello ... %d \n", *i);
}
