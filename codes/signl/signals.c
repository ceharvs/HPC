
#include <stdio.h>
#include <sys/signal.h>

int done=0; 

void quit()
{
  printf("interrupt signal received\n");
  done=1;
}

int main(int argc,char **argv)
{
   int i,s,total;
   int k=0;
   int n=100;

   /* install signal handler */ 
     signal(SIGINT,quit);

   /* do endless work */
   while( !done ) {
     s=0;
     for(i=k; i<k+n; i++) s+=i;
     printf("sum[%d -> %d] = %d\n",k,k+n,s);
     fflush(stdout);
     k+=10;
     total+=s;
     sleep(2);
   }

   printf("total=%d\n",total);
   return 0;
}

