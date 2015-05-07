#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double func( double x, double y, double a, double b) {
	return a*x + b*(x)/(x*x+y*y);
}



int main(int argc,char **argv)
{
	int ix, iy, nx, ny;
	double a, b, deltax, deltay, maxMAG, *arr, *gradx, *grady, *magnitude, x0, y0, maxX, maxY;
        double x,y;
	FILE *fp;
  
	/* Declare Variables */
	a = 0.5;
	b = 0.15;
	deltax = 0.00002;
	deltay = 0.00002;
	x0 = -0.1;
	y0 = -0.1;
	nx = 10001;
	ny = 10001;
  
	/* Find the maximum magnitude */
	maxX = 0.0;
	maxY = 0.0;
	maxMAG = 0.0;
	
	/* 2D array declaration */
	arr = malloc(nx*ny*sizeof(double));
	gradx = malloc(nx*ny*sizeof(double));
	grady = malloc(nx*ny*sizeof(double));
	magnitude = malloc(nx*ny*sizeof(double));
  
	/* Set up initial array values */
	/*#pragma omp parallel for private (ix, iy, x, y) shared(nx, ny, a, b)*/
	for(ix=0; ix<nx; ix++){
            x=x0+ix*deltax;
		for(iy=0; iy<ny; iy++){
                    y=y0+iy*deltay;
		    arr[nx*ix+iy] = func(x, y, a, b);
		    /*printf("%f \t%f\t%f\n", x,y,arr[nx*ix+iy]);*/
		}
	}
	
	/* Compute the X and Y gradients */
	
	/* Print to file in format x,y,z,gradx,grady */
	fp = fopen("output.csv","w");
	fprintf(fp,"x,y,z,gradx,grady\n");
		 
	/*#pragma omp parallel for private (ix, iy) shared(nx, ny, deltax, deltay) */
	for(ix=1; ix<nx-1; ix++){
	        x=x0+ix*deltax;
		for(iy=1; iy<ny-1; iy++){
                    	y=y0+iy*deltay;
			gradx[nx*ix+iy] = (arr[nx*(ix+1)+iy]-arr[nx*(ix-1)+iy])/(2*deltax);
		        /*printf("Grad X at (%f,%f) = %f\n", x,y,gradx[nx*ix+iy]);*/
			grady[nx*ix+iy] = (arr[nx*ix+iy+1]-arr[nx*ix+iy-1])/(2*deltay);
			magnitude[nx*ix+iy] = sqrt(pow(gradx[nx*ix+iy],2) +pow(grady[nx*ix+iy],2) );
		        /*printf("Grad Y at (%f,%f) = %f\n", x,y,grady[nx*ix+iy]);*/
		}
	}
	
	for(ix=1; ix<nx-1; ix++){
	        x=x0+ix*deltax;
		for(iy=1; iy<ny-1; iy++){
                    	y=y0+iy*deltay;
			/*fprintf(fp, "%f,%f,0,%f,%f\n",x,y,gradx[nx*ix+iy],grady[nx*ix+iy]);*/
			if(magnitude[nx*ix+iy] > maxMAG){
				maxMAG = magnitude[nx*ix+iy];
				maxX = x;
				maxY = y;
				/*printf("max is at (%f, %f) = %f\n", maxX, maxY, maxMAG);*/
			}
		}
	}
	
	printf("Maximum magnitude is at (%f, %f) and is %f.\n", maxX, maxY, maxMAG);

	free(arr);
	free(gradx);
	free(grady);
	free(magnitude);

	fclose(fp);

	return 0;

}
