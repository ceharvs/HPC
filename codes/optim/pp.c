/*
 * Molecular Dynamics Code using Particle-Particle Code
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))

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

  /* allocate */
  xyz=(double*)malloc(ndim*nprt*sizeof(double));
  vel=(double*)malloc(ndim*nprt*sizeof(double));
  acc=(double*)malloc(ndim*nprt*sizeof(double));
  if( !xyz || !vel || !acc ) return 1;

  /* init */
  random_seed(1);
  for(i=0; i<nprt; i++) {
    for(k=0; k<ndim; k++) {
       x=random_number();
       xyz[i*ndim+k]=x * (box[2*k+1]-box[2*k]) + box[2*k]; 
    }
  }

  /* update pointers */
  *(pxyz)=xyz;
  *(pvel)=vel;
  *(pacc)=acc;
  return 0;
}

double square(double x)
{
   double x2=pow(x,2.0);
   return x2;
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
  double pi=3.14159265;
  double vv=square(sin(MIN(x,pi/2.0)));
  return vv;
}

double dV(double x)
{
  /* derivative of potential: hamonic well that saturates to a max at PI/2 */
  double pi=3.14159265;
  double vv= 2.0*sin(MIN(x,pi/2.0))*cos(MIN(x,pi/2.0));
  return vv;
}

double magnitude(int ndim,double *v1)
{
  /* magnitude of a vector */
  int    k;
  double d=0.0;
  for(k=0; k<ndim; k++) d+=square(v1[k]);
  d=sqrt(d);
  return d;
}

void zeroforces(int nprt,int ndim,double *force)
{
  /* init forces to zero */
  int i,k;
  for(i=0; i<nprt; i++) {
    for(k=0; k<ndim; k++) {
      force[i*ndim+k]=0.0;
    }
  }
}

void calcforces(int nprt,int ndim,double *coord,double *force)
{
  /* calc forces */
  int    i,j,k;
  double d,rij[3];
  for(i=0; i<nprt; i++) {
    for(j=0; j<nprt; j++) {
      if( j==i ) continue;
      for(k=0; k<ndim; k++) {
         dist(ndim,&coord[i*ndim],&coord[j*ndim],rij);
         d=magnitude(ndim,rij);
         force[i*ndim+k] += -rij[k]*dV(d)/d;
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
  for(i=0; i<nprt; i++) {
    for(j=0; j<nprt; j++) {
      if( j==i ) continue;
      dist(ndim,&coord[i*ndim],&coord[j*ndim],rij);
      d=magnitude(ndim,rij);
      pot+=1.0/2.0*V(d);
    }
  }
  return pot;
}

double calckinetic(int nprt,int ndim,double mass,double *veloc)
{
  /* calc kinetic energy */
  int    i;
  double kin=0.0;
  for(i=0; i<nprt; i++) {
    kin += 1.0/2.0*mass*square(magnitude(ndim,&veloc[i*ndim]));
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
  int i,k;
  for(i=0; i<nprt; i++) {
    for(k=0; k<ndim; k++) {
      coord[i*ndim+k] += veloc[i*ndim+k]*dt + 1.0/2.0*square(dt)*accel[i*ndim+k];
      veloc[i*ndim+k] += 1.0/2.0*dt*(force[i*ndim+k]/mass + accel[i*ndim+k]);
      accel[i*ndim+k]  = force[i*ndim+k]/mass;
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


