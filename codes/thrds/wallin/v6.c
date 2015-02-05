#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

// This program uses condition variables to synchronize the
// output order of the code.
// September 2007- J. Wallin

int counter = 0;
pthread_mutex_t  my_mutex[NUM_THREADS];
pthread_cond_t  cond[NUM_THREADS];


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

  // show the thread has started
  printf(" starting  %d \n",thread_id);
 
  // wait for clearance to go on 
  pthread_cond_wait( &cond[thread_id], &my_mutex[thread_id] );


  // update the counter and print out the results
  counter = counter + thread_id;
  printf("Hello World! It's me, thread #%d  counter = %d!\n", 
	 thread_id, counter);

  // unlock mutex
  pthread_mutex_unlock(&my_mutex[thread_id]);

  // clean up the space
  pthread_mutex_destroy(&my_mutex[thread_id]);
  pthread_exit(NULL);

}

 

int main(int argc, char *argv[])
{
  pthread_t thread1[NUM_THREADS];
  int t;
  int ec;
  int thread_ids[NUM_THREADS];

  // initialize timer
  srand(time(0));

  //  my_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  for(t=0;t<NUM_THREADS;t++){
    pthread_mutex_init(&my_mutex[t], NULL);

  // lock the thread
    pthread_mutex_lock(&my_mutex[t]);
  }

  // initialize the condition variables
  for(t=0;t<NUM_THREADS;t++){
    pthread_cond_init(&cond[t],NULL);
  }

  // create the threads
  for(t=0;t<NUM_THREADS;t++){
    
    thread_ids[t] = t;

    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, test_mutex, (void *)&thread_ids[t] );
  }




  // wait 7 seconds
  sleep(7);
  
  // signal each thread to move forward in 1 second intervals
  for(t=0;t<NUM_THREADS;t++){
    sleep(1);
    pthread_cond_signal(&cond[t]);
  }

  // join all the threads 
  for(t=0;t<NUM_THREADS;t++){
    pthread_join(thread1[t], NULL);
  }

  // print out the total summmation
  printf( "the results is %d \n",counter);

  // clean up conditional variables
  for(t=0;t<NUM_THREADS;t++){
    pthread_cond_destroy(  &cond[t]);
  }

  pthread_exit(NULL);

}


