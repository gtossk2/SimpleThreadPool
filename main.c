#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREAD  2



typedef void *(*Func)(void* argc);
typedef struct job Job;
typedef struct jobqueue JobQueue;

struct job{
  Job* next;
  Func handler;
  void *argc;
};

struct jobqueue{
  Job *front;
  Job *rear;
  int job_number;
};

void *thread_handler(void *job);
void *hello(void *argc);
void queue_push(JobQueue* queue, Job* newJob);
Job* queue_pull(JobQueue* queue);

JobQueue jobPool = {.front = NULL, .rear = NULL, .job_number = 0};

int main(){

  pthread_t threads;
  int argc[5] = {1, 2, 3, 4, 5};
  Job job[5] = { {.handler = hello, .argc = &argc[0]},
                 {.handler = hello, .argc = &argc[1]},
                 {.handler = hello, .argc = &argc[2]},
                 {.handler = hello, .argc = &argc[3]},
                 {.handler = hello, .argc = &argc[4]}
               };  

  int i = 0;
  for(i = 0; i < 5; ++i){
    queue_push(&jobPool, &job[i]);
  }

  pthread_create(&threads, NULL, &thread_handler, &job);

  pthread_join(threads, 0);

  printf("Jon left : %d \n", jobPool.job_number);
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
};

Job* queue_pull(JobQueue* queue){
  Job* first_job;
  
  first_job = queue->front;
  queue->front = first_job->next;
  queue->job_number--;

  return first_job;
};

void *thread_handler(void *job){
  Job* t;

  do{
    t = queue_pull(&jobPool);
    t->handler(t->argc);
  }while(jobPool.job_number);

}

void *hello(void *argc){
  int *i = (int *)argc;
  printf("Hello ... %d \n", *i);
}
