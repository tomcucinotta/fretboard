VERSION=1.0
PROG=fretboard
CFLAGS=-Wall -pedantic -Wno-variadic-macros
prefix ?=/usr/local

bindir:=$(prefix)/bin
mandir:=$(prefix)/man/man1
pkgdir:=$(PROG)-$(VERSION)

all: $(PROG)

clean:
	rm -f *~ *.o $(PROG)

pkg:
	mkdir -p /tmp/$(pkgdir) && cp -a * /tmp/$(pkgdir)/ && cd /tmp/$(pkgdir) && make clean && cd .. && tar -czf $(pkgdir).tar.gz $(pkgdir) && echo "Source package built into /tmp/$(pkgdir).tar.gz"

install:
	mkdir -p $(DESTDIR)$(bindir)
	cp $(PROG) $(DESTDIR)$(bindir)/
	chmod 0755 $(DESTDIR)$(bindir)/$(PROG)
	mkdir -p $(DESTDIR)$(mandir)
	cp $(PROG).1 $(DESTDIR)$(mandir)/

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(PROG)
	rm -f $(DESTDIR)$(mandir)/$(PROG).1
