#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <mpi.h>

double randRange(double min, double max) {
    return min+((double)rand()/(double)RAND_MAX)*(max-min);
}

double xGRAD(double x, double y) {
    return -(0.3*x*x)/((x*x+y*y)*(x*x+y*y))+(0.15)/(x*x+y*y)+0.5;
}

double yGRAD(double x, double y) {
    return -(0.3*x*y)/((x*x+y*y)*(x*x+y*y));
}


int main(int argc,char **argv)
{
  int err,nprc,myidi, i, j, N, Lx, Ly;
  double *xPts, *yPts, *xVel, *yVel, LxMAX, LyMAX, deltaX, deltaY, xpt, ypt;
  char buf[512];
  err=MPI_Init(&argc,&argv);
  err=MPI_Comm_size(MPI_COMM_WORLD,&nprc);
  err=MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  gethostname(buf,512);
  
  deltaX = 1.0;
  deltaY = 1.0;
  LxMAX = 4.0;
  LyMAX = 2.0;
  Lx = ceil(Lx/deltaX);
  Ly = ceil(Ly/deltaY);
  N = 1;
  xPts = malloc(N*Lx*Ly*sizeof(double)); 
  yPts = malloc(N*Lx*Ly*sizeof(double)); 
  xVel = malloc(N*Lx*Ly*sizeof(double)); 
  yVel = malloc(N*Lx*Ly*sizeof(double)); 

  /* Create a 2D Domain of Lengths Lx and Ly */
  /* I am using a comain of Lx by Ly, split into rectangular blocks using deltaX and deltaY */
  /* Then generating N particles randomly in each sub-domain */
  for(i=0; i<LxMAX; i+=deltaX) {
    for(j=0; j<LyMAX; j+=deltaY) {
      xpt = randRange(i, i+deltaX);
      ypt = randRange(j, j+deltaY);
      xPts[i*Lx+j] = xpt;
      yPts[i*Lx+j] = ypt;
      
      /* Compute the X and Y velocities*/
      xVel[i*Lx+j] = xGRAD(xpt, ypt);
      yVel[i*Lx+j] = yGRAD(xpt, ypt);
      
      
    }
  }

  for(i=0; i<LxMAX; i+=deltaX) {
    for(j=0; j<LyMAX; j+=deltaY) {
      printf("Velocity: %f\n", xVel[i*Lx+j]);
      printf("Velocity: %f\n", yVel[i*Lx+j]);
    }
  }

     

  //printf("host=%s nproc=%d myid=%d\n",buf,nprc,myid);

  err=MPI_Finalize();
  return 0;
}

