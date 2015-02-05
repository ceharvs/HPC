#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_THREADS     10

sem_t semaphore;
pthread_mutex_t  add_mutex;
pthread_mutex_t  cond_mutex;
pthread_cond_t cond;

void init_barrier(int nproc)
{
  sem_init(&semaphore, 0, 0);
}


void barrier(int thread_id)
{
  int semval;
  
  // increment the semaphore and print the value
  sem_post(&semaphore);
  sem_getvalue(&semaphore, &semval);
  printf("semaphore value = %d  thread= % d \n", semval, thread_id);

  // if the the semaphore values has reached the total number of 
  // threads, broadcast the condition variable
  if (semval == NUM_THREADS ) {
    pthread_cond_broadcast(&cond);
  } else{

    // if we have not reached end of the barrier, wait
    pthread_mutex_lock(&cond_mutex);
    pthread_cond_wait( &cond, &cond_mutex);
    pthread_mutex_unlock(&cond_mutex);
  }

}


void *test_mutex(void *tid)
{ int *tt;
  int thread_id, rnd, i;
  int semval;
  tt = tid;
  thread_id = (int) *tt; 
  rnd = rand() % 5;
  sleep(rnd);

  // print the semaphore before we hit the barrier
  sem_getvalue(&semaphore, &semval);
  printf("before barrier thread #%d  semi %d \n", 
         thread_id, semval);
 
  // execute the barrier code
  barrier(thread_id);

  // continue on
  printf("after barrier  thread #%d  semi %d\n", 
         thread_id, semval);

  pthread_exit(NULL);

}

int main(int argc, char *argv[])
{
  pthread_t thread1[NUM_THREADS];
  int t;
  int ec;

  // set up the mutex locks
  pthread_mutex_init(&add_mutex, NULL);
  pthread_mutex_init(&cond_mutex, NULL);

  // initialize the threads
  for(t=0;t<NUM_THREADS;t++){
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, 
               test_mutex, (void *) &t );
  }

  // join all the threads before exiting
  for(t=0;t<NUM_THREADS;t++){
    pthread_join( thread1[t], NULL);
  }
}

