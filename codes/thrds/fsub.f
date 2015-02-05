       subroutine fmandel(i0,i1,j0,j1,nx,ny,itmx,x0,y0,dx,dy,img)
       implicit none
!      ---arguments
       integer i0,i1       ! i-loop limits
       integer j0,j1       ! j-loop limits
       integer nx,ny       ! image dimensions
       integer itmx        ! max iterations
       real*8  x0,y0       ! image origin
       real*8  dx,dy       ! image sizes
       integer img(nx*ny)  ! image pixels
!      ---local variables
       integer i,j,k
       real*8  x,y
       complex c,z
!      ---loop over pixels
       do i=i0,i1
         do j=j0,j1
           x = (i-1)*dx + x0
           y = (j-1)*dy + y0
           c = cmplx(x,y)
           z = c
!          ---compute nr iterations needed           
           k=0
           do while( k<itmx .and. abs(z)<2 ) 
             k = k + 1
             z = z*z + c
           enddo
!          ---store in image
           img( (j-1)*nx+i )=k
         enddo
       enddo
!      ---done
       return
       end
