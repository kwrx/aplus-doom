.PHONY: all clean

CC ?= cc
LD ?= cc

OUTPUT=doom
WAD=doom1.wad

SRCS=$(shell find src -name '*.c')
HDRS=$(shell find src -name '*.h')


all: $(OUTPUT)

$(OUTPUT): $(SRCS) $(HDRS)
	$(CC) -o $(OUTPUT) $(SRCS) -I aplus/include -Wl,--gc-sections -DNORMALUNIX -DLINUX -DSNDSERV -D_DEFAULT_SOURCE

install: $(OUTPUT)
	mkdir -p $(DESTDIR)/usr
	mkdir -p $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share
	mkdir -p $(DESTDIR)/usr/share/doom
	mkdir -p $(DESTDIR)/.pkg
	install -m 0755 $(OUTPUT) $(DESTDIR)/usr/bin/$(OUTPUT)
	install -m 0644 data/$(WAD) $(DESTDIR)/usr/share/doom/$(WAD)
