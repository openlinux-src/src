include ../../config

CFLAGS += -DDISABLE_WCHAR

CFLAGS += -I$(CURDIR)/../../include -I. -I$(CURDIR)/../libterminfo
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

libcurses.a: $(OBJS)
	$(HOSTAR) rcs $@ $(OBJS)

%.o: %.c
	$(TASK) CC $(notdir $@)
	$(HOSTCC) -MMD -MP $(CFLAGS) -c -o $@ $<

$(CURDIR)/../../build/$(ARCH)/sysroot/lib/libcurses.a: libcurses.a
	$(TASK) INSTALL lib/$^
	cp -f $^ $@

$(CURDIR)/../../build/$(ARCH)/sysroot/usr/include/curses.h: curses.h
	$(TASK) INSTALL include/$^
	mkdir -p $(CURDIR)/../../build/$(ARCH)/sysroot/usr/include
	cp -f $^ $(CURDIR)/../../build/$(ARCH)/sysroot/usr/include/


install: $(CURDIR)/../../build/$(ARCH)/sysroot/lib/libcurses.a \
	         $(CURDIR)/../../build/$(ARCH)/sysroot/usr/include/curses.h

clean:
	rm -f $(OBJS) *.d libcurses.a

.PHONY: install clean
