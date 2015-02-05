#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     2

pthread_mutex_t  my_mutex;


void *test_mutex(void *tid)
{ int *tt;
  int thread_id, rnd;
  tt = (int *) tid;
  thread_id = *tt;

  // print the initial message
  printf("Hello World! It's me, thread #%d \n",
         thread_id);

  // lock the thread if thread id is 0 and unlock
  // otherwise 
  //
  // the system will become deadlocked here because
  // the mutex has to be unlocked by thread 1.  thread 1
  // never starts up because it is waiting for thread 0
  // to exit at the join statement below.
  if (thread_id == 0) {
    pthread_mutex_lock(&my_mutex);
  } else {
    pthread_mutex_unlock(&my_mutex);
  }
 
  printf("Thread #%d has completed \n",
         thread_id);

  pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
  pthread_t thread1[NUM_THREADS];
  int t;
  int ec;
  srand(time(0));


  // set up the mutex locks
  pthread_mutex_init(&my_mutex, NULL);
  pthread_mutex_lock(&my_mutex);


  for(t=0;t<NUM_THREADS;t++){
    printf("In main: creating thread %d\n", t);

    // create a thread
    ec = pthread_create(&thread1[t], NULL, 
               test_mutex, (void *) &t );

    // join the thread - this statement waits
    // until the thread has exited
    pthread_join(thread1[t], NULL);
  }


}
