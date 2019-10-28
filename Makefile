VERSION=1.0
PROG=fretboard
CFLAGS=-Wall -pedantic -Wno-variadic-macros
PREFIX ?=/usr/local

BINDIR:=$(PREFIX)/bin
MANDIR:=$(PREFIX)/man/man1
PKGDIR:=$(PROG)-$(VERSION)

all: $(PROG)

clean:
	rm -f *~ *.o $(PROG)

pkg:
	mkdir -p /tmp/$(PKGDIR) && cp -a * /tmp/$(PKGDIR)/ && cd /tmp/$(PKGDIR) && make clean && cd .. && tar -czf $(PKGDIR).tar.gz $(PKGDIR) && echo "Source package built into /tmp/$(PKGDIR).tar.gz"

install:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(PROG) $(DESTDIR)$(BINDIR)/
	chmod 0755 $(DESTDIR)$(BINDIR)/$(PROG)
	mkdir -p $(DESTDIR)$(MANDIR)
	cp $(PROG).1 $(DESTDIR)$(MANDIR)/

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(PROG)
	rm -f $(DESTDIR)$(MANDIR)/$(PROG).1
