#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

/* this program shows how threads are created and how the
   order of execution does not have a simple or predictable behavior 
   Written Sept 2007 - J. Wallin */

int counter = 0;

void *test_mutex(void *tid)
{
  int *tt;
  int thread_id;
  int rnd;
  int tmp_counter;

  // set an integer pointer equal to the void pointer address
  tt = tid;
  thread_id = (int) *tt;
  

  // we are going to store value from main memory into a local variable
  // before we sleep the code -- this could happen if you had a longer
  // calculation or 
  tmp_counter = counter;

  // sleep the thread for a random time interval
  rnd = rand() % 5;
  sleep(rnd);

  // update the counter and print out the results
  // NOTE - with out putting a mutex on the value of counter,
  // we get the wrong results
  counter = tmp_counter + thread_id;
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

    // set element t of the array thread_ids to the value of t--
    // if you just pass "&t" to thread_create, the value stored at 
    // the variable t's address will change before the program is completed.
    // you need to pass a variable that is not changing to the routine 
    // to make sure this doesn't happen.
    thread_ids[t] = t;

    // create the threads
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *) &thread_ids[t]);

    // error check on the thread creation
    if (ec == 1) {
      printf("error from thread %d\n",ec);
      exit(-1);
    }
  }

  // print out the total summmation
  printf( "the results is %d \n",counter);

  // thread exit
  pthread_exit(NULL);

}


