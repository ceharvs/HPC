#include <stdio.h>
#include <stdlib.h>

#include "fsub.h"

/* fortran API */
void callfmandel(int i0,int i1,int j0,int j1,int nx,int ny,int itmx,
                 double x0,double y0,double dx,double dy,int *img) 
{
   fmandel_(&i0,&i1,&j0,&j1,&nx,&ny,&itmx,&x0,&y0,&dx,&dy,img);
}
                  
/* main */
int main(int argc,char **argv)
{
   int i,j,i0,i1,j0,j1;
   /* image variables */
   int    nx  =256;               /* image x-dimension */
   int    ny  =256;               /* image y-dimension */
   int    itmx=5000;              /* max iterations */
   double x0  =-2.0;              /* image x-origin */
   double y0  =-2.0;              /* image y-origin */
   double x1  = 2.0;              /* image x-max */
   double y1  = 2.0;              /* image y-max */
   double dx  =(x1-x0)/(nx-1);    /* image x-size */
   double dy  =(y1-y0)/(ny-1);    /* image y-size */
   int   *img =NULL;              /* image pixels */
   FILE  *fp  =NULL;

   /* allocate image */
   img=(int*)calloc(nx*ny,sizeof(int));

   /* calc image */
   i0=1;
   j0=1;
   i1=256;
   j1=256;
   callfmandel(i0,i1,j0,j1,nx,ny,itmx,x0,y0,dx,dy,img);

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

