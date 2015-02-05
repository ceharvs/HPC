
INCL=ipcdef.h

EXE1=ipc1.x
OBJ1=ipc1.o
SRC1=ipc1.c

EXE2=ipc2.x
OBJ2=ipc2.o
SRC2=ipc2.c

#CMP=icc -g -c
#LNK=icc

CMP=gcc -g -c --pedantic
LNK=gcc --pedantic

all: $(EXE1) $(EXE2)
	@echo "done"

clean:
	rm -f *.o *.x myfifo

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1)

$(EXE2): $(OBJ2) 
	$(LNK) -o $@ $(OBJ2)

ipc1.o: $(SRC1)
	$(CMP) $?

ipc2.o: $(SRC2)
	$(CMP) $?

ipc1.c: $(INCL)

ipc2.c: $(INCL)
