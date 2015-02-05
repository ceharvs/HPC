
EXE1=mpic.x
OBJ1=mpic.o

EXE2=mpif.x
OBJ2=mpif.o

FOR=mpif77 -g -c 
FLN=mpif77 -g 
CMP=mpicc -g -c --pedantic
LNK=mpicc --pendantic

all: $(EXE1) $(EXE2) 
	@echo "done"

clean:
	rm -f *.o *.x 

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1) 

$(EXE2): $(OBJ2) 
	$(FLN) -o $@ $(OBJ2) 

mpic.o: mpic.c
	$(CMP) $?

mpif.o: mpif.f
	$(FOR) $?

