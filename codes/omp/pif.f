       program pif
!      ---variables
       integer i,nx
       real*8  x,pi,dx,s
!      ---init
       nx = 1000000000         ! nr of intervals
       dx = 1.0d0 / dble(nx)   ! interval length
       s  =0.0d0               ! sum
!      ---calc sum (reduction operation)
!$omp parallel do private(i,x) shared(nx,dx) reduction(+:s)
       do i = 1, nx
         x = (dble(i) + 0.5d0) * dx
         s = s + 4.0d0 /(1.0d0 + x*x)
       enddo
!$omp end parallel do
!      ---calc pi & print
       pi = dx * s
       write(*,*) 'estimate of Pi with ',nx, ' is ',pi
       end program pif
