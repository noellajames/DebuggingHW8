CC=gcc
CFLAGS=-I.
DEPS = myutils.h mypattern.h myfilter.h 
OBJ = mygrep.o myutils.o mypattern.o myfilter.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mygrep: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o mygrep