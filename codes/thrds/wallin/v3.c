#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5


// first attempt at using Mutex variables to prevent ambiguity in the output
//  September 2007 - J. Wallin

int counter = 0;
pthread_mutex_t  *my_mutex;


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

  // lock the thread
  // the addition of the mutex lock here prevents the counter value
  // from being updated in correctly... but it does not order the output
  pthread_mutex_lock(my_mutex);

  // update the counter and print out the results
  counter = counter + thread_id;
  printf("Hello World! It's me, thread #%d  counter = %d!\n", 
	 thread_id, counter);

  //unlock the thread
  pthread_mutex_unlock(my_mutex);


  // clean up memory
  pthread_mutex_destroy(my_mutex);
  pthread_exit(NULL);
}

 

int main(int argc, char *argv[])
{
  pthread_t thread1[NUM_THREADS];
  int t;
  int ec;
  int thread_ids[NUM_THREADS];

  // allocate space for a mutex variable
  my_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

  // initialize the value of the mutex
  pthread_mutex_init(my_mutex, NULL);

  // start the timer with a random value
  srand(time(0));

  // loop over thenumber of threads
  for(t=0;t<NUM_THREADS;t++){

    thread_ids[t] = t;

    // create the threads
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *) &thread_ids[t]);
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


