
EXE1=ceh.x
OBJ1=ceh.o
LIB1=-lm


PRF=-pg
CMP=gcc -g -c --pedantic -Wall
LNK=gcc 
FOR=gfortran -g -c -p
FLN=gfortran -g 
#CMP=icc -g -c 
#LNK=icc
#FOR=ifort -g -c
#FLN=ifort -g 

all: $(EXE0) $(EXE1) $(EXE2)
	@echo "done"

clean:
	rm -f *.o *.x 

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1) $(LIB1)

pp.o: ceh.c
	$(CMP) $?


