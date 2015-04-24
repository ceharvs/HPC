#include <stdio.h>

double func( int x, int y, double a, double b) {
	return a*ix + b*(x)/(x*x+y*y)
}

int main(int argc,char **argv)
{
	int ix, iy, nx, ny;
	double a, b, deltax, deltay;
  
	/* Declare Variables */
	a = 2.5;
	b = 1.25;
	deltax = 0.1;
	deltay = 0.2;
	nx = 20;
	ny = 10;
  
	/* 2D array declaration */
	double arr[nx][ny];
	double gradx[nx][ny];
	double grady[nx][ny];
	double magnitude[nx][ny];
  
	/* Set up initial array values */
	for(ix=0; ix<nx; ix++){
		for(iy=0; iy<ny; iy++){
			arr[ix][iy] = func(ix, iy, a, b);
		}
	}
	
	/* Compute the X and Y gradients */
	/* 	grad[ix][iy] = (arr[ix+1][jx+1]-arr[ix+1][iy-1]
	-arr[ix-1][iy+1]+arr[ix-1][iy-1])/(4*deltax*deltay) */
	
	for(ix=0; ix<nx; ix++){
		for(iy=0; iy<ny; iy++){
			gradx[ix][iy] = (func(ix + deltax, iy, a, b) - func(ix-deltax, iy, a, b))/2*deltax;
			grady[ix][iy] = (func(ix, iy+deltay, a, b) - func(ix, iy-deltay, a, b))/2*deltay;
			magnitude[ix][iy] = (func(ix+deltax,iy+deltay, a, b)-func(ix+deltax,iy-deltay, a, b) - func(ix-deltax, iy+deltay, a, b) + func(ix-deltax, iy-deltay, a, b))/(4*deltax*deltay);
		}
	}
	
	/* Find the maximum magnitude */
	int maxX = 0;
	int maxY = 0;
	double maxMAG = 0.0;
	 
	for(ix=0; ix<nx; ix++){
		for(iy=0; iy<ny; iy++){
			if(magnitude[ix][iy] > maxMAG){
				maximum = magnitude[ix][iy];
				maxX = ix;
				maxY = iy;
			}
		}
	}
	
	printf("Maximum magnitude is at (%d, %d) and is %f.\n", maxX, maxY, maximum)
}
