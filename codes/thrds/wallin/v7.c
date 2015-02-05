#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

/* this is a disfunctional program that shows the problems
associated with passing the same variable to each thread within
a loop.   The program passes the reference to the value of the
variable t to teach thread.   The thread then equates an integer
pointer to the void pointer, and casts it as an integer.

In the time it takes for the transfer and casting to take place,
the value of t has changed in the main program.  Thus, the values
that are in the routines are random.

September 2007 - J. Wallin
*/

int counter = 0;

void *test_mutex(void *tid)
{
  int *tt;
  int thread_id;
  int rnd;

  // get the thread id  
  tt = (int *) tid;
  thread_id = *tt;
  
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

  // start the timer with a random value
  srand(time(0));

  // loop over thenumber of threads
  for(t=0;t<NUM_THREADS;t++){
   
    // create the threads
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *) &t );

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


