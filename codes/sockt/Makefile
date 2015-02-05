
EXE1=server.x
OBJ1=server.o

EXE2=client.x
OBJ2=client.o 

#CMP=icc -g -c
#LNK=icc

CMP=gcc -g -c --pedantic
LNK=gcc --pedantic

all: $(EXE1) $(EXE2) 
	@echo "done"

clean:
	rm -f *.o *.x 

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1)

$(EXE2): $(OBJ2) 
	$(LNK) -o $@ $(OBJ2) $(LIB2)

server.o: server.c
	$(CMP) $?

client.o: client.c
	$(CMP) $?

