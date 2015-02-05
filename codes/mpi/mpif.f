       program mpif
       implicit none
       include "mpif.h"
       integer ierr,nprc,myid
       call MPI_INIT(ierr)
       call MPI_COMM_SIZE(MPI_COMM_WORLD,nprc,ierr)
       call MPI_COMM_RANK(MPI_COMM_WORLD,myid,ierr)
       write(*,*)'nprc=',nprc,' myid=',myid
       call MPI_FINALIZE(ierr)
       end
