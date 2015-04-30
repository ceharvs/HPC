#include <stdio.h>
#include <stdlib.h>

double func( int x, int y, double a, double b) {
	return a*x + b*(x)/(x*x+y*y);
}

int main(int argc,char **argv)
{
	int ix, iy, nx, ny, maxX, maxY;
	double a, b, deltax, deltay, maxMAG, *arr, *gradx, *grady, *magnitude;
	FILE *fp;
  
	/* Declare Variables */
	a = 0.5;
	b = 3.25;
	deltax = 0.01;
	deltay = 0.02;
	nx = 1000;
	ny = 2000;
  
	/* Find the maximum magnitude */
	maxX = 0;
	maxY = 0;
	maxMAG = 0.0;
	
	/* 2D array declaration */
	arr = malloc(nx*ny*sizeof(double));
	gradx = malloc(nx*ny*sizeof(double));
	grady = malloc(nx*ny*sizeof(double));
	magnitude = malloc(nx*ny*sizeof(double));
  
	/* Set up initial array values */
	#pragma omp parallel for private (ix, iy) shared(nx, ny, a, b) 
	for(ix=0; ix<nx; ix++){
		for(iy=0; iy<ny; iy++){
			arr[nx*ix+iy] = func(ix, iy, a, b);
		}
	}
	
	/* Compute the X and Y gradients */
	/* 	grad[ix][iy] = (arr[ix+1][jx+1]-arr[ix+1][iy-1]
	-arr[ix-1][iy+1]+arr[ix-1][iy-1])/(4*deltax*deltay) */
	
	/* Print to file in format x,y,gradx,grady */
	fp = fopen("output.csv","w");
	fprintf(fp,"x,y,gradx,grady\n");
		 
	#pragma omp parallel for private (ix, iy) shared(nx, ny, deltax, deltay, a, b) 
	for(ix=0; ix<nx; ix++){
		for(iy=0; iy<ny; iy++){
			gradx[nx*ix+iy] = (func(ix + deltax, iy, a, b) - func(ix-deltax, iy, a, b))/(2*deltax);
			grady[nx*ix+iy] = (func(ix, iy+deltay, a, b) - func(ix, iy-deltay, a, b))/(2*deltay);
			magnitude[nx*ix+iy] = (func(ix+deltax,iy+deltay, a, b)-func(ix+deltax,iy-deltay, a, b) - func(ix-deltax, iy+deltay, a, b) + func(ix-deltax, iy-deltay, a, b))/(4*deltax*deltay);
		}
	}
	
	for(ix=0; ix<nx; ix++){
		for(iy=0; iy<ny; iy++){
			fprintf(fp, "%d,%d,%f,%f\n",ix,iy,gradx[nx*ix+iy],grady[nx*ix+iy]);
			if(magnitude[nx*ix+iy] > maxMAG){
				maxMAG = magnitude[nx*ix+iy];
				maxX = ix;
				maxY = iy;
			}
			if(ix==2 && iy==2){
				printf("Magnitude at (1,1) is %f.\n",magnitude[nx*ix+iy]);
			}
		}
	}
	
	printf("Maximum magnitude is at (%d, %d) and is %f.\n", maxX, maxY, maxMAG);

	free(arr);
	free(gradx);
	free(grady);
	free(magnitude);

	fclose(fp);

	return 0;

}
