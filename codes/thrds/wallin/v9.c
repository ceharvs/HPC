#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     5

int counter = 0;


void *test_mutex(void *tid)
{ int *tt;
  int thread_id, rnd;
  tt = (int *) tid;
  thread_id = *tt;

  rnd = rand() % 5;
  sleep(rnd);
  counter = counter + thread_id;
  printf(
  "Hello World! It's me, thread #%d  counter = %d!\n",
         thread_id, counter);
  pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
  pthread_t thread1[NUM_THREADS];
  int t;
  int i;
  int ec;
  srand(time(0));
  for(t=0;t<NUM_THREADS;t++){
    printf("In main: creating thread %d\n", t);
    ec = pthread_create(&thread1[t], NULL, 
               test_mutex, (void *) &t );
    }
  

  // join all the threads before moving to the next line
  for(i=0;i<NUM_THREADS;i++){
    pthread_join( thread1[i], NULL);
  }


  printf( "the results is %d \n",counter);
}
