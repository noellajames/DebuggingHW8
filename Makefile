CC=gcc
CFLAGS=-I.
DEPS = myutils.h mypattern.h myfilter.h handleexistingstates.h cantransition.h
OBJ = mygrep.o myutils.o mypattern.o myfilter.o handleexistingstates.o cantransition.o
TARGET = mygrep

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGET)

mygrep: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o mygrep