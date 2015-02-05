
EXE1=fmandel.x
OBJ1=fmandel.o fsub.o

EXE2=cmandel.x
OBJ2=cmandel.o fsub.o
LIB2=-lgfortran
#LIB2=-lifcore

EXE3=pmandel.x
OBJ3=pmandel.o fsub.o
LIB3=-lpthread -gfortran -lm

FOR=gfortran -O -c
FLN=gfortran -O 
#FOR=g77 -O -c
#FLN=g77 -O 
CMP=gcc -g -c #--pedantic
LNK=gcc #--pendantic

#FOR=ifort -O -c
#FLN=ifort -O 
#CMP=icc -g -c
#LNK=icc

all: $(EXE1) $(EXE2) $(EXE3)
	@echo "done"

clean:
	rm -f *.o *.x *.out *.png *.eps

$(EXE1): $(OBJ1) 
	$(FLN) -o $@ $(OBJ1)

$(EXE2): $(OBJ2)
	$(LNK) -o $@ $(OBJ2) $(LIB2)

$(EXE3): $(OBJ3)
	$(LNK) -o $@ $(OBJ3) $(LIB3)

fmandel.o: fmandel.f
	$(FOR) $?

fsub.o: fsub.f
	$(FOR) $?

cmandel.o: cmandel.c fsub.h
	$(CMP) $?

pmandel.o: pmandel.c fsub.h
	$(CMP) $?
