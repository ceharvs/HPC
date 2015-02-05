program pif90
! variables
integer :: i, nx
double precision :: x, pi, dx, s
s =0.0d0 ; nx = 1000000000 
dx = 1.0d0 / dble(nx)
!$omp parallel do private(i,x) shared(nx,dx) reduction(+:s)
do i = 1, nx
  x = (dble(i) + 0.5d0) * dx
  s = s + 4.0d0 /(1.0d0 + x*x)
enddo
!$OMP END PARALLEL DO
pi = dx * s
print *, "estimate of Pi with ",nx, " is ",pi
end program pif90
