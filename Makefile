.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: imgcrop

imgcrop: imgcrop.o image.o geometry.o
	$(CC) $(LDFLAGS) -o imgcrop imgcrop.o image.o geometry.o $(LDLIBS)

clean:
	rm -f imgcrop *.o imgcrop-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f imgcrop $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/imgcrop
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f imgcrop.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/imgcrop.1

dist: clean
	mkdir -p imgcrop-$(VERSION)
	cp -R COPYING config.mk Makefile README imgcrop.1 imgcrop.c \
		image.c image.h geometry.c geometry.h stb_image.h stb_image_write.h \
		imgcrop-$(VERSION)
	tar -cf imgcrop-$(VERSION).tar imgcrop-$(VERSION)
	gzip imgcrop-$(VERSION).tar
	rm -rf imgcrop-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/imgcrop
	rm -f $(DESTDIR)$(MANPREFIX)/man1/imgcrop.1
