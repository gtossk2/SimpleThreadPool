#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread_pool.h"

#define THREAD_NUM 4

// Default Job Task
void *job_task(void *argv);

int main(){
  // Initialize the Jobs
  int argv[5] = {1, 2, 3, 4, 5};
  Job job[5] = { {.job_handler = job_task, .job_argv = &argv[0]},
                 {.job_handler = job_task, .job_argv = &argv[1]},
                 {.job_handler = job_task, .job_argv = &argv[2]},
                 {.job_handler = job_task, .job_argv = &argv[3]},
                 {.job_handler = job_task, .job_argv = &argv[4]}
               };   


  ThreadPool th_pool;
  threadPool_init(&th_pool, THREAD_NUM);

  int i = 0;
  while(1)
  {
    // Add Jobs into Job Queue
    for(i = 0; i < 5; i++){
      threadPool_Add_job(&th_pool, &job[i]);
    }

    // halt adding jobs
    usleep(3000000);
  }

  // Wait for all jobs been processed
  threadPool_join(&th_pool);

  // Destroy mutex and condition variable
  pthread_mutex_destroy(&th_pool.job_pool->job_mutex);
  pthread_cond_destroy(&th_pool.job_pool->job_cond);
  return 0;
}

// Default Job Task
void *job_task(void *argc){
  int *i = (int *)argc;
  printf("Job %d ... Processed\n", *i);
  usleep(1000);
  return 0;
}

