/*
 * Molecular Dynamics Code using Particle-Particle Code
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))
#define HALFPI   1.570796325 

double random_number()
{
  /* generate a random number between 0 and 1 */
  double x=rand();
  double y=x / RAND_MAX;
  return y;
}

void random_seed(int seed)
{
  /* set seed for random number generator */
  srand(seed);
}

int init(int nprt,int ndim,double *box,
         double **pxyz,double **pvel,double **pacc)
{
  /* allocate & initialize particles */
  int     i,k;
  double  x;
  double *xyz=NULL;
  double *vel=NULL;
  double *acc=NULL;
  double boxk2;

  /* allocate */
  xyz=(double*)malloc(ndim*nprt*sizeof(double));
  vel=(double*)malloc(ndim*nprt*sizeof(double));
  acc=(double*)malloc(ndim*nprt*sizeof(double));
  if( !xyz || !vel || !acc ) return 1;

  /* init */
  random_seed(1);
  for(k=0; k<ndim; k++) {
    boxk2=box[k+k];
    #pragma omp parallel for private (i,x)
    for(i=0; i<nprt; i++) {
       x=random_number();
       xyz[i*ndim+k]=x * (box[k+k+1]-boxk2) + boxk2; 
    }
  }

  /* update pointers */
  *(pxyz)=xyz;
  *(pvel)=vel;
  *(pacc)=acc;
  return 0;
}

void dist(int ndim,double *r1,double *r2,double *dr)
{
  /* calc distance vector */
  int k;
  for(k=0; k<ndim; k++) {
    dr[k] = r1[k] - r2[k];
  }
}

double V(double x)
{
  /* potential: hamonic well that saturates to a max at PI/2 */
  double sine=sin(MIN(x,HALFPI));
  double vv=sine*sine;
  return vv;
}

double dV(double x)
{
  /* derivative of potential: hamonic well that saturates to a max at PI/2 */
  /*double pi=3.14159265;*/
  double min_value = MIN(x,HALFPI);
  double vv= 2.0*sin(min_value)*cos(min_value);
  return vv;
}

double magnitude(int ndim,double *v1)
{
  /* magnitude of a vector */
  int    k;
  double d=0.0;
  double v1k;
  for(k=0; k<ndim; k++) {
    v1k = v1[k];
    d+=v1k*v1k;
  }
  d=sqrt(d);
  return d;
}

void zeroforces(int nprt,int ndim,double *force)
{
  /* init forces to zero */
  int i,k;
  for(k=0; k<ndim; k++) {
    #pragma omp parallel for private (i)
    for(i=0; i<nprt; i++) {
      force[i*ndim+k]=0.0;
    }
  }
}

void calcforces(int nprt,int ndim,double *coord,double *force)
{
  /* calc forces */
  int    i,j,k,index;
  double d,rij[3];
  double *coord_i;

  for(k=0; k<ndim; k++) {
    for(i=0; i<nprt; i++) {
      coord_i = &coord[i*ndim];
      index = i*ndim+k;
      #pragma omp parallel for private (j,rij,d)
      for(j=0; j<nprt; j++) {
         if( j==i ) continue;
         dist(ndim,coord_i,&coord[j*ndim],rij);
         d=magnitude(ndim,rij);
         force[index] += -rij[k]*dV(d)/d;
      }
    }
  }
}

double calcpotent(int nprt,int ndim,double *coord)
{
  /* calc potential energy */
  int    i,j;
  double d,rij[3];
  double pot=0.0;
  double *coord_i,*coord_j;
  for(i=0; i<nprt; i++) {
    coord_i=&coord[i*ndim];
    #pragma omp parallel for private (j,coord_j,rij,d) reduction(+:pot)
    for(j=0; j<nprt; j++) {
      if( j==i ) continue;
      coord_j=&coord[j*ndim];
      dist(ndim,coord_i,coord_j,rij);
      d=magnitude(ndim,rij);
      pot+=0.5*V(d);
    }
  }
  return pot;
}

double calckinetic(int nprt,int ndim,double mass,double *veloc)
{
  /* calc kinetic energy */
  int    i;
  double kin=0.0;
  double mag_value;
  #pragma omp parallel for private (i,mag_value) reduction(+:kin)
  for(i=0; i<nprt; i++) {
    mag_value = magnitude(ndim,&veloc[i*ndim]);
    kin += 0.5*mass*mag_value*mag_value;
  }
  return kin;
}

void compute(int nprt,int ndim,
             double  mass ,double *coord,double *veloc,
             double *force,double *pkin ,double *ppot)
{
  zeroforces(nprt,ndim,force);
  calcforces(nprt,ndim,coord,force);
  *(ppot)=calcpotent(nprt,ndim,coord);
  *(pkin)=calckinetic(nprt,ndim,mass,veloc);
}

void verlet(int nprt,int ndim,double dt,double mass,
            double *coord,double *veloc,double *accel,double *force)

{
  /* advance one timestep */
  int i,k,index;
  for(k=0; k<ndim; k++) {
    for(i=0; i<nprt; i++) {
      index = i*ndim+k;
      coord[index] += veloc[index]*dt + 0.5*dt*dt*accel[index];
      veloc[index] += 0.5*dt*(force[index]/mass + accel[index]);
      accel[index]  = force[index]/mass;
    }
  }
}

void advance(int nprt,int ndim,double dt,double mass,
            double *coord,double *veloc,double *accel,
            double *pkin ,double *ppot)
{
  /* alloc aux space for forces */
  double *force=(double*)malloc(ndim*nprt*sizeof(double));

  /* compute forces & energies */
  compute(nprt,ndim,mass,coord,veloc,force,pkin,ppot);

  /* move particles using Verlet's algorithm */
  verlet(nprt,ndim,dt,mass,coord,veloc,accel,force);

  /* free aux space */
  free(force);
}

/* main driver */
int main(int argc,char **argv)
{
  int     i,err;
  int     ndim =3;                              /* spatial dimension */
  int     nprt =500;                            /* nr particles */
  int     nstp =200;                            /* nr steps */
  double  mass =1.0;                            /* mass of the particles */
  double  dt   =1.0e-4;                         /* timestep */
  double  box[]={0.0,10.0,0.0,10.0,0.0,10.0};   /* bounding box */
  double *coord=NULL;                           /* positions */
  double *veloc=NULL;                           /* velocities */
  double *accel=NULL;                           /* accelerations */
  double pot,kin,ene,e0;                        /* potential/kinetic/total energy */

  /* initialize positions, velocities & accelerations */
  err=init(nprt,ndim,box,&coord,&veloc,&accel);
  if( err ) {fprintf(stderr,"error init\n"); return 1;}

  /* time loop */
  for(i=0; i<nstp; i++) {
    advance(nprt,ndim,dt,mass,coord,veloc,accel,&kin,&pot);
    ene=pot+kin;
    if( i==0 ) e0=ene;
    printf("V=%e K=%e E=%e e(E)=%e\n",pot,kin,ene,(ene-e0)/e0);
  }
  return 0;
}

