echo "running mpic.x on 4 nodes:"
mpirun --mca btl tcp,self --hostfile hostfile -np 4 ./mpic.x

echo "running mpif.x on 20 nodes:"
mpirun --mca btl tcp,self --hostfile hostfile -np 20 ./mpic.x
