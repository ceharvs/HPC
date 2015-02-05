       program mandel
       implicit none
!      ---variables
       integer nx,ny,itmx,i,j,i0,j0,i1,j1
       integer img(512*512)
       real*8  x0,y0,x1,y1,dx,dy
!      ---init values
       nx  =256             ! image x-dimension
       ny  =256             ! image y-dimension
       itmx=5000          ! max iterations
       x0=-2.0              ! image x-origin
       y0=-2.0              ! image y-origin
       x1= 2.0              ! image x-max
       y1= 2.0              ! image y-max
       dx=(x1-x0)/(nx-1)    ! image x-size
       dy=(y1-y0)/(ny-1)    ! image y-size
!      ---build image
       i0=1
       j0=1
       i1=256
       j1=256
       call fmandel(i0,i1,j0,j1,nx,ny,itmx,x0,y0,dx,dy,img)
!      ---save image to file
       open(1,file='mandel.out',status='unknown')
       do i=1,nx
         do j=1,ny
           write(1,*) img( (j-1)*nx+i )
         enddo
       enddo
!      ---done
       end

