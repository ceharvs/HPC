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


double Rij(double deltaX, double deltaY, double phi, double phi_left, double phi_right, double phi_up, double phi_down, double rho) {
  double a, b, c;
  a = 1.0/(deltaX*deltaX);
  b = 1.0/(deltaY*deltaY);
  c = -2.0/(deltaX*deltaX)-2.0/(deltaY*deltaY);
  return a*(phi_left+phi_right)+b*(phi_up+phi_down) + c*phi - rho/c;
}

int main(int argc,char **argv)
{
  FILE *fp, *rhoFile;
  int i, j, k, c, size, rank, err, seed;
  int Nx, Ny, N, Gx, Gy; 
  double deltaX, deltaY, Lx, Ly, x, y, error, tMAX;
  double phi0, phi1, xi, t, deltaT, dPhi_old, dPhi_new;
  char buf[512];

  /* Define processor-specific variables */
  int my_Nx, my_Gx, elements, x_near, y_near;
  double min_X, min_Y, xpt, ypt;

  /* Define arrays */
  double *rho, *phi, *rho_share, *rho_rec;
  double *xPts, *yPts, *xVel, *yVel, *q, *Fx, *Fy, *Ex, *Ey;
  
  err=MPI_Init(&argc,&argv);
  err=MPI_Comm_size(MPI_COMM_WORLD,&size);
  err=MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  gethostname(buf,512);

  /* Set Random Seed */
  seed = time(NULL);
  srand(seed*rank);

  /* X and Y step sizes */
  deltaX = 0.05;
  deltaY = 0.05;
  deltaT = 0.1;
  t = 0.0;
  tMAX = 1.0;

  /* Maximum number of the grid limit */
  Lx = 1.0;
  Ly = 1.0;

  /* Maximum number of bins*/
  Nx = ceil(Lx/deltaX);
  Ny = ceil(Ly/deltaY);

  /* Number of grid points */
  Gx = Nx+1;
  Gy = Ny+1;

  /* Constant variables*/
  phi0 = 0.0;
  phi1 = 1.0;

  /* Number of particles per bin */
  N = 1;

  /* Local Number of Bins */
  my_Nx = Nx/size;
  
  /* Local Number of grid points */
  my_Gx = my_Nx+1;

  /* Elements per processor */
  elements = N*my_Nx*Ny;

  /* Set up processor-specific informtion for Bins */
  min_X = Lx/(double)size*rank;
  min_Y = 0.0;

  /* Set up the input files */
  if(rank==0){
    fp=fopen("output0.csv","w");
    rhoFile=fopen("rho0.csv","w");
  } else if(rank==1){
    fp=fopen("output1.csv","w");
    rhoFile=fopen("rho1.csv","w");
  } else if(rank==2) {
    fp=fopen("output2.csv","w");
    rhoFile=fopen("rho2.csv","w");
  } else {
    fp=fopen("output3.csv","w");
    rhoFile=fopen("rho3.csv","w");
  }   
 
  /* Grid Points */
  rho = malloc(my_Gx*Gy*sizeof(double));
  rho_rec = malloc(Gy*sizeof(double));
  rho_share = malloc(Gy*sizeof(double));
  phi = malloc(my_Gx*Gy*sizeof(double));
  Ex = malloc(my_Gx*Gy*sizeof(double));
  Ey = malloc(my_Gx*Gy*sizeof(double));

  /* Particle Points */
  Fx = malloc(elements*sizeof(double));
  Fy = malloc(elements*sizeof(double));
  xPts = malloc(elements*sizeof(double));
  yPts = malloc(elements*sizeof(double));
  xVel = malloc(elements*sizeof(double));
  yVel = malloc(elements*sizeof(double));
  q = malloc(elements*sizeof(double));

  /* Compute Charge Density on grid */
  printf("PROC:%d\t my_Nx:%d\t my_Gx:%d\t min_X:%f\n",rank,my_Nx,my_Gx,min_X);
  for(i=0; i<my_Gx; i++) {
    x = min_X+i*deltaX; 
    for(j=0; j<Gy; j++) {
      y = min_Y+j*deltaY;
      c = i*Gy+j;
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
 
  c = 0;
  /* Initalize Particles in each domain */
  for(i=0; i<my_Nx; i++) {
    x = min_X+i*deltaX;
    for(j=0; j<Ny; j++) {
      y = min_Y+ j*deltaY;
      for(k=0; k<N; k++) {
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
    
  /* Print Initalized Points to Output */ 
  fprintf(fp, "c,t,x,y,xVel,yVel\n");
  c = 0;
  for(i=0; i<my_Nx; i++) {
    for(j=0; j<Ny; j++) {
      for(k=0; k<N; k++) {
        fprintf(fp,"%d,%f,%f,%f,%f,%f\n",c,t,xPts[c],yPts[c],xVel[c],yVel[c]);
        c++;
      }
    }
  }

  for(t = deltaT; t <= tMAX; t+=deltaT) {
  /* Compute Charge Density on Grid */
  //Cycle through all of the grid points and assign their charge to a grid point
  c = 0;
  for(i=0; i<my_Nx; i++) {
    x = min_X+i*deltaX;
    for(j=0; j<Ny; j++) {
      y = min_Y+ j*deltaY;
      for(k=0; k<N; k++) {
        /* Find the nearest grid point */
        if(yPts[c] > (y+deltaY/2.0)) {
          //Particle is on the top half
          y += deltaY;
        }
        if(xPts[c] > (x+deltaX/2.0)) {
          //Particle is on the right half
          x += deltaX;
        }
        x_near = (int)((x-min_X)/deltaX);
        y_near = (int)(y/deltaY);

        //Add the charge density to the proper rho grid
        rho[x_near*Gy+y_near] += q[c]/(deltaX*deltaY);
        c++;
      }
    }
  }

  /* Sort out the Rho array that needs to be shared */
  //Send the lower Rho down one rank
  if (rank!=0) {
    // Put together rho_share
    for(i=0; i<Gy; i++) {
      rho_share[i] = rho[i];
    }
    MPI_Send(rho_share, Gy, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
  }
  /* The lower processor received the data and performs a summation */
  if (rank != (size-1)) {
    MPI_Recv(rho_rec, Gy, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Proc %d has recv data!\n", rank);
    for(i=0; i<Gy; i++) {
      rho_share[i] = rho_rec[i] + rho[my_Nx*Gy+i];
      rho[my_Nx*Gy+i] = rho_share[i];
    }
    
    /* Send total matrix up one rank */
    MPI_Send(rho_share, Gy, MPI_DOUBLE, rank+1, 1, MPI_COMM_WORLD);
  }
  /* Receive the new Rho Values */
  if (rank!=0) {
    /* MPI Receive */
    MPI_Recv(rho_rec, Gy, MPI_DOUBLE, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(i=0; i<Gy; i++) {
      rho[i] = rho_rec[i];
    }
  }
     
  /* Solve Possion Equation */
  while(0.001 < error) {
    error = 0;
    for(i=1; i<my_Gx-1; i++) {
      x = min_X+i*deltaX;
      for(j=1; j<Gy-1; j++) {
        y = min_Y+j*deltaY;
        c = i*Gy+j;
        dPhi_old = phi[c];
        dPhi_new = Rij(deltaX, deltaY, phi[c], phi[c-Gy], phi[c+Gy], phi[c+1], phi[c-1], rho[c]);
        phi[c] = dPhi_new + 0.1*(dPhi_new-dPhi_old);
        error += abs(dPhi_new-dPhi_old);
      }
    }
    error = error/((Gx-2)*(Gy-2));
  }
  
  /* Compute E-Field from phi */
  for(i=1; i<my_Gx-1; i++) {
    for(j=1; j<Gy-1; j++) {
      c = i*Gy+k;
      Ex[c] = -(phi[c+Gy]-phi[c-Gy])/(2*deltaX);
      Ey[c] = -(phi[c+1]-phi[c-1])/(2*deltaY);
    }
  }

  /* Find the Forces on each Particle */
  c = 0;
  for(i=0; i<my_Nx; i++) {
    x = min_X+i*deltaX;
    for(j=0; j<Ny; j++) {
      y = min_Y+ j*deltaY;
      for(k=0; k<N; k++) {
        /* Find the nearest grid point */
        if(yPts[c] > (y+deltaY/2.0)) {
          //Particle is on the top half
          y += deltaY;
        }
        if(xPts[c] > (x+deltaX/2.0)) {
          //Particle is on the right half
          x += deltaX;
        }
        x_near = (int)((x-min_X)/deltaX);
        y_near = (int)(y/deltaY);

        //Add the charge density to the proper rho grid
        Fx[c] = q[c]*Ex[x_near*Gy+y_near];
        Fy[c] = q[c]*Ey[x_near*Gy+y_near];
        c++;
      }
    }
  }

  /* Advance Particles and Recompute Velocity */
  c = 0;
  for(i=0; i<my_Nx; i++) {
    for(j=0; j<Ny; j++) {
      for(k=0; k<N; k++) {
        xPts[c] = xPts[c] + xVel[c]*deltaT+0.5*Fx[c]*deltaT*deltaT;
        yPts[c] = yPts[c] + yVel[c]*deltaT+0.5*Fy[c]*deltaT*deltaT;
        xVel[c] = xVel[c] + 0.5*Fx[c]*deltaT;
        yVel[c] = yVel[c] + 0.5*Fy[c]*deltaT;
        c++;
        
        /* Migrate Particle if they cross boundaries 
        //Migrate Right
        if(xPts[c] > x_Min+deltaX*my_Nx)
          MPI_Send(xPts[x],1,MPI_DOUBLE,xp,
          xPts[c] = NULL;
          yPts[c] = NULL;*/
          

      }
    }
  }
  
  
 
  // Print file to outputs 
  c = 0;
  for(i=0; i<my_Nx; i++) {
    for(j=0; j<Ny; j++) {
      for(k=0; k<N; k++) {
        fprintf(fp,"%d,%f,%f,%f,%f,%f\n",c,t,xPts[c],yPts[c],xVel[c],yVel[c]);
        c++;
      }
    }
  }
}

  // Print file to outputs 
  fprintf(rhoFile, "x,y,rho\n");
  for(i=0; i<my_Gx; i++) {
  x = min_X + i*deltaX;
    for(j=0; j<Gy; j++) {
      y = min_Y+ j*deltaY;
      c = i*Gy+j;
      fprintf(rhoFile,"%f,%f,%d,%f\n",x,y,c,rho[c]);
    }
  }

  free(rho);
  free(phi);
  free(rho_rec);
  free(rho_share);
  free(Fx);
  free(Fy);
  free(Ex);
  free(Ey);
  free(xPts);
  free(xVel);
  free(yVel);
  free(yPts);
  fclose(fp);
  fclose(rhoFile);
  free(q);
  err=MPI_Finalize();
  return 0;
}

