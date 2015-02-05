
EXE1=master.x
OBJ1=master.o
SRC1=master.c

EXE2=slave.x
OBJ2=slave.o subs.o
SRC2=slave.c 
SRC3=subs.f
LIB2=-lgfortran
#LIB2=-lifcore 

#FOR=ifort -g -c
#FLN=ifort -g 
#CMP=icc -g -c
#LNK=icc

FOR=gfortran -g -c
FLN=gfortran -g 
CMP=gcc -g -c
LNK=gcc

all: $(EXE1) $(EXE2) 
	@echo "done"

clean:
	rm -f *.o *.x *.out

$(EXE1): $(OBJ1) 
	$(LNK) -o $@ $(OBJ1)

$(EXE2): $(OBJ2) 
	$(LNK) -o $@ $(OBJ2) $(LIB2)

master.o: $(SRC1)
	$(CMP) $?

slave.o: $(SRC2)
	$(CMP) $?

subs.o: $(SRC3)
	$(FOR) $?

