#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

// this code reorders the thread completion times so that it
// happens in sequential order.   
// September 2007 - J. Wallin

  int counter = 0;
pthread_mutex_t  my_mutex[NUM_THREADS];


void *test_mutex(void *tid)
{
  int thread_id;
  int rnd;
  int *tt;

  // get the thread id  
  tt = tid;
  thread_id = (int) *tt;

  // sleep the thread for a random time interval
  rnd = rand() % 5;
  sleep(rnd);

  if (thread_id > 0) {
    //unlock the thread
    pthread_mutex_lock(&my_mutex[thread_id]);
  }

  // update the counter and print out the results
  counter = counter + thread_id;
  printf("Hello World! It's me, thread #%d  counter = %d!\n", 
	 thread_id, counter);


  thread_id = thread_id + 1;
  if (thread_id < NUM_THREADS) {
    pthread_mutex_unlock(&my_mutex[thread_id]);
  }


    pthread_mutex_destroy(&my_mutex[thread_id]);
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


  for(t=0;t<NUM_THREADS;t++){
    // initialize the mutex
    pthread_mutex_init(&my_mutex[t], NULL);

    // lock the mutex
    pthread_mutex_lock(&my_mutex[t]);
  }

  // loop over thenumber of threads
  for(t=0;t<NUM_THREADS;t++){ 

    thread_ids[t] = t;

    // create the threads
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *)&thread_ids[t]);
  }
  

  // join all the threads before moving to the next line
  for(t=0;t<NUM_THREADS;t++){
    pthread_join(thread1[t], NULL);
  }

  printf( "the results is %d \n",counter);
  pthread_exit(NULL);

}


