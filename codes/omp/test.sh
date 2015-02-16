OMP_NUMP_THREADS=1
echo "Results with 1 thread"
time ./pic.x

OMP_NUMP_THREADS=2
echo "Results with 2 threads"
time ./pic.x

OMP_NUMP_THREADS=4
echo "Results with 4 threads"
time ./pic.x

OMP_NUMP_THREADS=8
echo "Results with 8 threads"
time ./pic.x

OMP_NUMP_THREADS=16
echo "Results with 16 threads"
time ./pic.x


