#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <mpi.h>

double Rij(double deltaX, double deltaY, double phi, double phi_left, double phi_right, double phi_up, double phi_down, double rho) {
  double a, b, c;
  a = 1.0/(deltaX*deltaX);
  b = 1.0/(deltaY*deltaY);
  c = -2.0/(deltaX*deltaX)-2.0/(deltaY*deltaY);
  return a*(phi_left+phi_right)+b*(phi_up+phi_down) + c*phi - rho/c;
}

int main(int argc,char **argv)
{
  FILE *fp;
  int c, seed, i, j, err, size, rank, Lx, Ly, ci, Nx, Ny, my_Nx, iMax, iMin;
  double x, y, *rho, *deltaPhi, *phi, deltaX, deltaY, xi, phi0, phi1, deltaTi, t, maxT, deltaT;
  double min_X, max_X, min_Y, max_Y;
  char buf[512];
  err=MPI_Init(&argc,&argv);
  err=MPI_Comm_size(MPI_COMM_WORLD,&size);
  err=MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  gethostname(buf,512);

  /* X and Y step sizes */
  deltaX = 0.05;
  deltaY = 0.05;
  deltaT = 1.0;
  maxT = 1.0;

  /* Maximum number of the grid limit */
  Lx = 1.0;
  Ly = 1.0;

  /* Maximum number of bins*/
  Nx = ceil(Lx/deltaX);
  Ny = ceil(Ly/deltaY);

  /* Constant variables*/
  phi0 = 0.0;
  phi1 = 1.0;

  /* Set up processor-specific information */
  min_X = Lx/size*rank;
  max_X = Lx/size*(rank+1);
  min_Y = 0.0;  //We only split in the X direction
  max_Y = Ly;

  /* Local Number of Bins */
  my_Nx = Nx/size;

  /* Set up the input files */
  if(rank==0){
    fp=fopen("output0.csv","w");
  } else if(rank==1){
    fp=fopen("output1.csv","w");
  } else if(rank==2) {
    fp=fopen("output2.csv","w");
  } else {
    fp=fopen("output3.csv","w");
  }   
 
  c = 0;

  /* Check to make sure boundaries aren't violated */
  if (rank==0) {
    iMax = my_Nx+1;
    iMin = 0;
  } else if (rank==(size-1)) {
    iMax = my_Nx;
    iMin = -1;
  } else {
    iMax = my_Nx+1;
    iMin = -1;  
  }

  if(rank==0){
    fp=fopen("output0.csv","w");
  } else if(rank==1){
    fp=fopen("output1.csv","w");
  } else if(rank==2) {
    fp=fopen("output2.csv","w");
  } else {
    fp=fopen("output3.csv","w");
  }      
 
  rho = malloc(Nx*Ny*sizeof(double));
  phi = malloc(Nx*Ny*sizeof(double));
  deltaPhi = malloc(Nx*Ny*sizeof(double));

  for(i=iMin; i<iMax; i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Ny; j++) {
      y = min_Y+j*deltaY;
      c = i*Nx+j;
      if(x == 0) {
        phi[c] = phi0;
      } else if(x==Lx) { 
        phi[c] = phi1;
      } else if (y==0 || y==Ly) {
        xi = (x-0)/(Lx-0);
        phi[c] = (1-xi)*phi0+xi*phi1;
      } else { 
        phi[c] = 0.0;
      }
      rho[c] = 1.0;
    }
  }
  //Compute the gradient   
  for(t=0; t<maxT; t+=deltaT) {
    for(i=iMin+1; i<iMax-1; i++) {
    x = min_X+i*deltaX; 
      for(j=1; j<Ny-1; j++) {
        y = min_Y+j*deltaY;
        c = i*Nx+j;
        deltaPhi[c] = Rij(deltaX, deltaY, phi[c], phi[c-Nx], phi[c+Nx], phi[c+1], phi[c-1], 0);
        //printf("deltaphi[%d]=%f\n",c,deltaPhi[c]);
      }
    }
  }

  printf("minx=%f\t maxX=%f\t rank=%d\n",min_X, max_X, rank);
  //MPI_Reduce(&minT,&dT,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);

  // Print the Minimum Delta T 
  //if(rank==0){
  //  printf("minimum deltaT = %f\n",dT);
  //}
  // Print file to outputs 
  c = 0;
  fprintf(fp, "x,y,deltaPhi\n");
  for(i=0; i<(Nx/size); i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Ny; j++) {
      y = min_Y+j*deltaY;
      c = i*Nx+j;
//printf("%f,%f,%d\n",x,y,c);
      fprintf(fp,"%f,%f,%f\n",x,y,deltaPhi[c]);
    }
  }

  free(rho);
  free(phi);
  free(deltaPhi);
  fclose(fp);
  err=MPI_Finalize();
  return 0;
}

