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


int main(int argc,char **argv)
{
  FILE *fp;
  int seed, k, i, j, err, nprc, rank, N, Lx, Ly, elements, my_Lx, my_Ly;
  double x, y, *xP, *yP, *xV, *yV, LxMAX, LyMAX, deltaX, deltaY;
  double xpt, ypt, min_X, max_X, min_Y, max_Y, deltaT, minT, dT;
  char buf[512];
  err=MPI_Init(&argc,&argv);
  err=MPI_Comm_size(MPI_COMM_WORLD,&nprc);
  err=MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  gethostname(buf,512);

  /* X and Y step sizes */
  deltaX = 1.0;
  deltaY = 1.0;
  minT = 500;
  seed = time(NULL);
  srand(seed*rank);

  /* X and Y maximum values */
  LxMAX = 10.0;
  LyMAX = 10.0;
 
  /* Total Number of Bins */
  Lx = ceil(LxMAX/deltaX);
  Ly = ceil(LyMAX/deltaY);

  /* Number of particles per bin */
  N = 3;

  /* Elements per processor */
  elements = (N*Lx*Ly)/nprc;

  /* Bin ranges on the processor */
  min_X = (LxMAX/nprc)*rank;
  max_X = (LxMAX/nprc)*(rank+1);
  min_Y = 0.0;  // We only split in the X direction for this
  max_Y = LyMAX;

  /* Local number of bins */
  my_Lx = Lx/nprc;
  my_Ly = Ly; //We are only splitting the X direction across processors

  xP = malloc(elements*sizeof(double)); 
  yP = malloc(elements*sizeof(double)); 
  xV = malloc(elements*sizeof(double));
  yV = malloc(elements*sizeof(double)); 

  if(rank==0){
    fp=fopen("output0.csv","w");
  } else if(rank==1){
    fp=fopen("output1.csv","w");
  } else if(rank==2) {
    fp=fopen("output2.csv","w");
  } else {
    fp=fopen("output3.csv","w");
  }    
  
  /* Create a 2D Domain of Lengths Lx and Ly */
  /* I am using a comain of Lx by Ly, split into rectangular blocks using deltaX and deltaY */
  /* Then generating N particles randomly in each sub-domain */
  printf("proc=%d\t minX=%f, maxX=%f, my_Lx=%d\n",rank, min_X, max_X, my_Lx);
  printf("proc=%d\t minY=%f, maxY=%f, my_Ly=%d\n",rank, min_Y, max_Y, my_Ly);
  for(i=0; i<my_Lx; i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<my_Ly; j++) {
      y = min_Y+j*deltaY;
      for (k=0; k<N; k++) { 
        xpt = randRange(x, x+deltaX);
        ypt = randRange(y, y+deltaY);
        xP[i*my_Lx+j+k] = xpt;
        yP[i*my_Lx+j+k] = ypt;
if(rank==0){
  printf("[%f,%f],[%f,%f] = (%f,%f)\n",x,x+deltaX,y,y+deltaY,xpt,ypt);
}
      
        /* Compute the X and Y velocities*/
        xV[i*my_Lx+j+k] = xGRAD(xpt, ypt);
        yV[i*my_Lx+j+k] = yGRAD(xpt, ypt);
        
        /* Compute the deltaT */
        deltaT = deltaX/magnitude(xV[i*my_Lx+j+k],yV[i*my_Lx+j+k]);
        if (deltaT < minT) {
            minT = deltaT;
        }
      }
    }
  }
  
  MPI_Reduce(&minT,&dT,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);

  /* Print the Minimum Delta T */
  if(rank==0){
    printf("minimum deltaT = %f\n",dT);
  }

  /* Print file to outputs */
  fprintf(fp, "x,y,xVel,yVel\n");
  for(i=0; i<elements; i++){
    fprintf(fp,"%f,%f,%f,%f\n",xP[i],yP[i],xV[i],yV[i]);
  }
  

  free(xP);
  free(yP);
  free(xV);
  free(yV);
  fclose(fp);
  err=MPI_Finalize();
  return 0;
}

