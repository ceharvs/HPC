
EXE1=signals.x
OBJ1=signals.o

#CMP=icc -g -c
#LNK=icc

CMP=gcc -g -c --pedantic
LNK=gcc --pedantic

all: $(EXE1) 
	@echo "done"

clean:
	rm -f *.o *.x 

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1)

signals.o: signals.c
	$(CMP) $?


