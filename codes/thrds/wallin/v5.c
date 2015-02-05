#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

// using Condtion varables - a basic introduction
// this code shows the use of conditional variables and signals 
// instead of using Mutex's.  Synchronization is not 
// guaranteed with this code.
// September 2007 - J. Wallin

int counter = 0;
pthread_mutex_t  my_mutex[NUM_THREADS];
pthread_mutex_t  my_cond_mutex;
pthread_cond_t cond;


void *test_mutex(void *tid)
{
  int thread_id;
  int rnd;
  int *tt;

  // get the thread id  
  tt = tid;
  thread_id = (int) *tt;

  // sleep the thread for a random time interval
  rnd = rand() % 3;
  sleep(rnd);


  // wait for clearance to go on 
  pthread_mutex_lock(&my_cond_mutex);
  pthread_cond_wait( &cond, &my_cond_mutex);
  pthread_mutex_unlock(&my_cond_mutex);


  // update the counter and print out the results
  counter = counter + thread_id;
  printf("Hello World! It's me, thread #%d  counter = %d!\n", 
	 thread_id, counter);

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


  // lock the conditional mutex, initialize the condition variable
  pthread_mutex_init(&my_cond_mutex, NULL);
  pthread_cond_init(&cond,NULL);
  


  // loop over thenumber of threads
  for(t=0;t<NUM_THREADS;t++){ 
    
    thread_ids[t] = t;

    // create the threads
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *)&thread_ids[t]);
  }
  

  // wait 7 seconds
  sleep(4);
  
  // signal each thread to move forward in 1 second intervals
  pthread_cond_broadcast(&cond);





  printf( "the results is %d \n",counter);
  pthread_exit(NULL);

}


