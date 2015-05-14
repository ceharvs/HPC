#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>

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

double magnitude(double vx, double vy) {
    return sqrt(vx*vx+vy*vy);
}

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
  int k, elements, N, c, seed, i, j, err, size, rank, Lx, Ly, ci, Nx, Ny, my_Nx, iMax, iMin;
  double x, y, *rho, *deltaPhi, *phi, deltaX, deltaY, xi, phi0, phi1, deltaTi, t, maxT, deltaT;
  double min_X, max_X, min_Y, max_Yi, minT;
  double dT, xpt, ypt, *xPts, *yPts, *xVel, *yVel, *q;
  int x_near, y_near, my_Nx_extended;
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
  minT = 500.0;
  seed = time(NULL);
  srand(seed*rank);
  

  /* Maximum number of the grid limit */
  Lx = 1.0;
  Ly = 1.0;

  /* Maximum number of bins*/
  Nx = ceil(Lx/deltaX);
  Ny = ceil(Ly/deltaY);
printf("Nx=%d\tNy=%d\n",Nx,Ny);

  /* Constant variables*/
  phi0 = 0.0;
  phi1 = 1.0;

  /* Number of particles per bin */
  N = 1;

  /* Elements per processor */
  elements = (N*Nx*Ny)/size;

  /* Set up processor-specific information */
  min_X = Lx/(double)size*rank;
  max_X = Lx/(double)size*(rank+1);
  min_Y = 0.0;  //We only split in the X direction

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

  /* Number of bins with Boundaries*/
  my_Nx_extended = iMax-iMin;

  if(rank==0){
    fp=fopen("output0.csv","w");
  } else if(rank==1){
    fp=fopen("output1.csv","w");
  } else if(rank==2) {
    fp=fopen("output2.csv","w");
  } else {
    fp=fopen("output3.csv","w");
  }      
 
  rho = malloc(my_Nx_extended*Ny*sizeof(double));
  phi = malloc(my_Nx_extended*Ny*sizeof(double));
  deltaPhi = malloc(my_Nx_extended*Ny*sizeof(double));
  xPts = malloc(elements*sizeof(double));
  yPts = malloc(elements*sizeof(double));
  xVel = malloc(elements*sizeof(double));
  yVel = malloc(elements*sizeof(double));
  q = malloc(elements*sizeof(double));

  /* Compute Charge Density on grid */
  /* This has an extended boundary when needed */
  printf("minx=%f\t maxX=%f\t rank=%d\n",min_X, max_X, rank);
  for(i=iMin; i<iMax; i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Ny; j++) {
      y = min_Y+j*deltaY;
      c = i*my_Nx_extended+j;
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
      rho[c] = 0.0;
    }
  }

  /* Initalize Particles in each domain */
  printf("proc=%d\t minX=%f, maxX=%f, my_Lx=%d\n",rank, min_X, max_X, my_Nx);
  c = 0;
  for(i=0; i<my_Nx; i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Ny; j++) {
      y = min_Y+j*deltaY;
      for (k=0; k<N; k++) { 
        xpt = randRange(x, x+deltaX);
        ypt = randRange(y, y+deltaY);
        xPts[c] = xpt;
        yPts[c] = ypt;
      
        /* Compute the X and Y velocities*/
        xVel[c] = 0;
        yVel[c] = 0;

	/* Assign a random value to the particle charge */
        q[c] = randRange(-1.0, 1.0);        
        c++;
      }
    }
  }

  /* Compute Charge Density on grid */
  c = 0;
  for(i=0; i<my_Nx; i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Ny; j++) {
      y = min_Y+j*deltaY;
      for (k=0; k<N; k++) { 
        /* Find the nearest grid point */
        // Top or Bottom half
        if(yPts[c] > y+deltaY/2.0) {
          //Item is on the top half
          y = y + deltaY;
        }
        //Left or right side
        if(xPts[c] > x+deltaX/2.0) {
          x = x + deltaX;
        }
        //Nearest Grid Point
        x_near = (int)((x-min_X)/deltaX);
        y_near = (int)((y-min_Y)/deltaY);

        //Add the charge density to the proper rho[c]
        
        rho[c] += 
      



        c++;
      }
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

  //MPI_Reduce(&minT,&dT,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);

  // Print the Minimum Delta T 
  if(rank==0){
    printf("minimum deltaT = %f\n",dT);
  }
  // Print file to outputs 
  c = 0;
  fprintf(fp, "x,y,phi,deltaPhi\n");
  /*for(i=0; i<=(Nx/size); i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Ny; j++) {
      y = min_Y+j*deltaY;
      c = i*Nx+j;
      fprintf(fp,"%f,%f,%f,%f\n",x,y,phi[c],deltaPhi[c]);
    }
  }*/

  free(rho);
  free(phi);
  free(deltaPhi);
  free(xPts);
  free(xVel);
  free(yVel);
  free(yPts);
  fclose(fp);
  free(q)
  err=MPI_Finalize();
  return 0;
}

