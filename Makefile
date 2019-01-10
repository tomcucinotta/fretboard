PROG=fretboard
CFLAGS=-Wall -pedantic

all: $(PROG)

clean:
	rm -f *~ *.o $(PROG)
