#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mpi.h>


int main(int argc,char **argv)
{
  int err,nprc,myid;
  char buf[512];
  err=MPI_Init(&argc,&argv);
  err=MPI_Comm_size(MPI_COMM_WORLD,&nprc);
  err=MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  gethostname(buf,512);

  printf("host=%s nproc=%d myid=%d\n",buf,nprc,myid);

  err=MPI_Finalize();
  return 0;
}

