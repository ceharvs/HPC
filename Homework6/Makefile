
EXE1=hw6.x
OBJ1=hw6.o
LIB1=-lm

FOR=mpif77 -g -c 
FLN=mpif77 -g 
CMP=mpicc -g -c #--pedantic
LNK=mpicc #--pendantic

all: $(EXE1) 
	@echo "done"

clean:
	rm -f *.o *.x 

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1) $(LIB1)


hw6.o: hw6.c
	$(CMP) $?


