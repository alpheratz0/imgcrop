.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: pngcrop

pngcrop: pngcrop.o
	$(CC) $(LDFLAGS) -o pngcrop pngcrop.o $(LDLIBS)

clean:
	rm -f pngcrop pngcrop.o pngcrop-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f pngcrop $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/pngcrop
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f pngcrop.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/pngcrop.1

dist: clean
	mkdir -p pngcrop-$(VERSION)
	cp -R COPYING config.mk Makefile README pngcrop.1 pngcrop.c pngcrop-$(VERSION)
	tar -cf pngcrop-$(VERSION).tar pngcrop-$(VERSION)
	gzip pngcrop-$(VERSION).tar
	rm -rf pngcrop-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/pngcrop
	rm -f $(DESTDIR)$(MANPREFIX)/man1/pngcrop.1
