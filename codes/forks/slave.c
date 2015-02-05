#include <stdio.h>
#include <stdlib.h>

#include "subs.h"

int main(int argc,char **argv)
{
  FILE *fp=NULL;
  int   arg=-1;
  int   sum=-1;

  /* open output file */
  fp=fopen("slave.out","w");
  fprintf(fp,"slave-----------\n");
  fflush(fp);

  /* get argument */
  if( argc>0 ) arg=atoi(argv[0]);
  fprintf(fp,"got arg=%d\n",arg);
  fflush(fp);

  /* work */
  fprintf(fp,"calling work sub(%d)\n",arg);
  fflush(fp);
  work_(&arg,&sum);

  /* done */
  fprintf(fp,"done (sum=%d)\n",sum);
  fflush(fp);
  fclose(fp);
  exit(EXIT_SUCCESS);
}

