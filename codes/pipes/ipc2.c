#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ipcdef.h"

int main(int argc,char **argv)
{
   int   ok,fd,i,sum,my_sum;
   char  buf[BUFSIZE];
   
   /* do some work */
   printf("summing from 1 to %d\n",NMAX);
   printf("summing from -2 to -2*%d\n",NMAX);
   fflush(stdout);
   sum=0; 
   for(i=1; i<NMAX; i++) {
     sum+=i*i;
     my_sum+=-2*i;
   }
   printf("sum=%d\n",sum);
   printf("my_sum=%d\n",my_sum);
   printf("sleeping 10 secs...");
   fflush(stdout);
   sleep(10);
   printf("ok - continuing \n");
   fflush(stdout);

   /* open pipe for writting */
   printf("opening pipe for writting\n");
   fflush(stdout);
   fd=open(FIFONAME,O_WRONLY);
   if( !fd ) {fprintf(stderr,"could not open pipe for writting\n");}


   /* pack number into buffer */
   printf("writting to buffer\n");
   fflush(stdout);
   sprintf(buf,"%d",sum);
   
   /* write to the pipe */
   printf("writting buffer to pipe\n");
   fflush(stdout);
   write(fd,buf,strlen(buf));

   /* pack number into buffer */
   printf("writting second number to buffer\n");
   fflush(stdout);
   sprintf(buf,"%d",my_sum);
   
   /* write to the pipe */
   printf("writting second buffer to pipe\n");
   fflush(stdout);
   write(fd,buf,strlen(buf));

   /* print */
   printf("sleeping 10 secs...");
   fflush(stdout);
   sleep(10);
   printf("ok - done \n");
   fflush(stdout);

   /* done */
   close(fd);
   return 0;
}



