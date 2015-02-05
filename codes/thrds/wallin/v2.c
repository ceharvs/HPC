#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

/* simple thread program with a join to synchronize the
   final results... contains some of the problematic issues 
   from v1.c... so you use with care! 
   -September 2007 - J. Wallin */

int counter = 0;

void *test_mutex(void *tid)
{
  int *tt;
  int thread_id;
  int rnd;

  // get the thread id  
  tt = tid;
  thread_id = (int) *tt;

  // sleep the thread for a random time interval
  rnd = rand() % 5;
  sleep(rnd);

  // update the counter and print out the results
  counter = counter + thread_id;
  printf("Hello World! It's me, thread #%d  counter = %d!\n", 
	 thread_id, counter);

  // clean up memory
  pthread_exit(NULL);

}

 

int main(int argc, char *argv[])
{
  pthread_t thread1[NUM_THREADS];
  int t;
  int ec;
  int thread_ids[NUM_THREADS];

  // start the timer with a random value
  srand(time(0));

  // loop over thenumber of threads
  for(t=0;t<NUM_THREADS;t++){

    thread_ids[t] = t;

    // create the threads
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *)&thread_ids[t] );
  }

  // join all the threads before moving to the next line
  for(t=0;t<NUM_THREADS;t++){
    pthread_join(thread1[t], NULL);
  }


  // print out the total summmation
  printf( "the results is %d \n",counter);

  // thread exit
  pthread_exit(NULL);

}


