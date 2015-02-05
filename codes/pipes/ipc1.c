#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ipcdef.h"

int main(int argc,char **argv)
{
   int   err,fd,i,sum,sum2,nread;
   char  buf[BUFSIZE];
   
   /* create named pipe */
   printf("creating named pipe=%s\n",FIFONAME);
   fflush(stdout);
   err=mkfifo(FIFONAME,FIFOMODE);
   /*if( err==-1 ) {fprintf(stderr,"could not mkfifo\n"); exit(1);}*/

   /* do some work */
   printf("summing numbers from 0 to %d\n",NMAX);
   fflush(stdout);
   sum=0;
   for(i=0; i<NMAX; i++) sum+=i;

   /* open pipe for reading */
   printf("opening pipe for reading\n");
   fflush(stdout);
   fd=open(FIFONAME,O_RDONLY);
   if( !fd ) {fprintf(stderr,"could not open pipe for reading\n");}

   /* read from pipe, store into buffer */
   printf("reading buffer from pipe\n");
   fflush(stdout);
   nread=read(fd,buf,BUFSIZE);
   buf[nread]='\0';
      
   /* scan buffer for number */
   sscanf(buf,"%d",&sum2);

   /* print */
   printf("my sum=%d, got sum2=%d\n",sum,sum2);
   fflush(stdout);

   /* done */
   printf("done\n");
   fflush(stdout);
   close(fd);
   return 0;
}


