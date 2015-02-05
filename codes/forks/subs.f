       subroutine work(n,s)
       implicit none
       integer i,n,s
c
c      ---open output file
c
       open(1,file='subs.out',status='unknown')
       write(1,*)'--> subs-----------'
       flush(1)
c
c      ---do some work
c
       write(1,*)'--> summing from 1 to ',n
       flush(1)
c
       s=0
       do i=1,n
         s=s+i
       enddo
c
c      ---print results
c
       write(1,*)'--> sum=',s
       flush(1)
c
c      ---done
c
       write(1,*)'--> done'
       flush(1)
       close(1)

       return 
       end
