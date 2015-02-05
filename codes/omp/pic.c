#include <stdio.h>

int main(int argc,char **argv)
{
  int i;
  double x,pi;
  int    nx=1000000000;           /* nr intervals */
  double dx=1.0/((double)(nx));   /* interval */
  double s =0.0;                  /* sum */

#pragma omp parallel for private (i,x) shared(nx,dx) reduction(+:s) 
  for(i=0; i<nx; i++) {
    x = ((double)(i) + 0.5) * dx;
    s+= 4.0 / (1.0 + x*x);
  }

  pi=dx*s;
  printf("estimate of Pi with %d intervals is %1.15f\n",nx,pi);
  return 0;
}
