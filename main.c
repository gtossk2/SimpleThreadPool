#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread_pool.h"

// Default Job Task
void *job_task(void *argv);

int main(){
//  pthread_t threads;
  int argv[5] = {1, 2, 3, 4, 5};
  Job job[5] = { {.job_handler = job_task, .job_argv = &argv[0]},
                 {.job_handler = job_task, .job_argv = &argv[1]},
                 {.job_handler = job_task, .job_argv = &argv[2]},
                 {.job_handler = job_task, .job_argv = &argv[3]},
                 {.job_handler = job_task, .job_argv = &argv[4]}
               };   


  ThreadPool th_pool;
  threadPool_init(&th_pool, 1);
  int i = 0;

  while(1)
  {
    // Here we test with the existing jobs
    // Make sure that do not repeat the job.
    // Please finish all jobs in your timeout, or segmentation fault
    for(i = 0; i < 5; i++){
      threadPool_Add_job(&th_pool, &job[i]);
    }
    usleep(3000000);
  }

  threadPool_join(&th_pool);

  pthread_mutex_destroy(&th_pool.job_pool->job_mutex);
  pthread_cond_destroy(&th_pool.job_pool->job_cond);
  return 0;
}

void *job_task(void *argc){
  int *i = (int *)argc;
  printf("Hello ... %d \n", *i);
  usleep(1000);
  return 0;
}

