#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char **argv)
{
  char *slave="./slave.x";  /* slave code executable */
  char *args[]={"1000"};    /* default argument for slave */
  int   i,err,sum,opt=0;
  pid_t pid;

  
  /* overwrite default with 1st command line argument if given */
  if( argc>1 ) args[0]=argv[1];

  /* fork */
  pid=fork();
  if( pid ) {
    printf("pid=%05d => parent => waiting for children\n",pid);
    fflush(stdout);
  }
  else {
    printf("pid=%05d => child  => executing %s (%s)\n",pid,slave,args[0]);
    fflush(stdout);
    err=execv(slave,args);
    if( err ) {fprintf(stderr,"err=%d\n",err);}
  }

  /* Compute sum fromm i=1,n */
  printf("summing from 1 to %d\n",1000);
  for(i=1;i<1000;i++){
    sum+=2*i;
  }
  printf("sum=%d\n",sum);

  /* wait */
  waitpid(-1,&err,opt);
  printf("children done\nmaster sleeping 10 sec...\n");
  fflush(stdout);
  sleep(10);

  /* done */
  printf("master done\n"); fflush(stdout);
  return 0;
}


