#include <stdio.h>
#include <stdlib.h>

#include "fsub.h"

/* global variables */
#define MAXTHREAD 10
int    nthrd=1;                 /* default nr of threads */
int    nx   =256;               /* image x-dimension */
int    ny   =256;               /* image y-dimension */
int    itmx =50000;              /* max iterations */
double xmin =-2.0;              /* image x-origin */
double ymin =-2.0;              /* image y-origin */
double xmax = 2.0;              /* image x-max */
double ymax = 2.0;              /* image y-max */
int   *img  =NULL;              /* image pixels */

/* fortran API */
void callfmandel(int i0,int i1,int j0,int j1,int nx,int ny,int itmx,
                 double xmin,double ymin,double dx,double dy,int *img) 
{
   fmandel_(&i0,&i1,&j0,&j1,&nx,&ny,&itmx,&xmin,&ymin,&dx,&dy,img);
}

/* thread-specific function */
void calcmandel(void *vptr)
{
   /* set indices for thread sub-image */
   int myid=*((int*)vptr);          /* my thread id */
   int ni = nx/nthrd;               /* nr of pixels in x dir of sub-image */
   int nj = ny/nthrd;               /* nr of pixels in y dir of sub-image */
   int j0 = 1 + myid*nj;            /* i-start of sub-image */
   int j1 = j0+nj-1;                /* i-end of sub-image */
   int i0 = 1;                      /* j-start of sub-image */
   int i1 = 256;                    /* j-end of sub-image */
   double dx  =(xmax-xmin)/(nx-1);  /* image x-size */
   double dy  =(ymax-ymin)/(ny-1);  /* image y-size */

   /* print */
   printf("thread[%d]: i0=%d i1=%d j0=%d j1=%d\n",myid,i0,i1,j0,j1);
   fflush(stdout);

   /* calc sub-image */
   callfmandel(i0,i1,j0,j1,nx,ny,itmx,xmin,ymin,dx,dy,img);

   /* done */
   pthread_exit(NULL);
}

/* main */
int main(int argc,char **argv)
{
   int       i,j,err;
   int       tids[MAXTHREAD];  /* thread ids */
   pthread_t tlst[MAXTHREAD];  /* thread info */
   FILE     *fp=NULL;

   /* get nr threads from command line args */
   if( argc>1 ) nthrd=atoi(argv[1]);
   printf("num threads=%d\n",nthrd);
   fflush(stdout);

   /* allocate image */
   img=(int*)calloc(nx*ny,sizeof(int));

   /* launch threads */
   for(i=0; i<nthrd; i++) {
     tids[i]=i;
     err=pthread_create(&tlst[i], NULL, calcmandel, (void *) &tids[i]);
     if( err==1 ) {fprintf(stderr,"error creating thread %d (%d)\n",i,err); exit(1);}
   }

   /* join threads */
   for(i=0; i<nthrd; i++) pthread_join( tlst[i], NULL);

   /* save image */
   fp=fopen("mandel.out","w");
   for(i=0; i<nx; i++) {
      for(j=0; j<ny; j++) {
         fprintf(fp,"%d\n",img[j*nx+i]);
      }
   }
   fclose(fp);

   /* done */
   free(img);
   return 0;
}

