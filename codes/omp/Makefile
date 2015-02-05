
EXE1=pif.x
OBJ1=pif.o

EXE2=pif90.x
OBJ2=pif90.o 

EXE3=pic.x
OBJ3=pic.o 

CMP=gcc -g -c -fopenmp
LNK=gcc -fopenmp
FCM=gfortran -c -fopenmp
FLN=gfortran -g -fopenmp

# Intel compiler links
#CMP=icc -g -c -openmp
#LNK=icc -openmp
#FCM=ifort -c -openmp
#FLN=ifort -openmp

all: $(EXE1) $(EXE2) $(EXE3)
	@echo "done"

clean:
	rm -f *.o *.x 

$(EXE1): $(OBJ1) 
	$(FLN) -o $@ $(OBJ1)

$(EXE2): $(OBJ2) 
	$(FLN) -o $@ $(OBJ2)

$(EXE3): $(OBJ3) 
	$(LNK) -o $@ $(OBJ3)

pif.o: pif.f
	$(FCM) $?

pif90.o: pif90.f90
	$(FCM) $?

pic.o: pic.c
	$(CMP) $?

