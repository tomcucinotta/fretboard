PROG=fretboard
CFLAGS=-Wall -pedantic -Wno-variadic-macros

all: $(PROG)

clean:
	rm -f *~ *.o $(PROG)
