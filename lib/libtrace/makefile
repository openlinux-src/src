include ../../config

CFLAGS += -I$(CURDIR)/../../include 

libtrace.a: libtrace.o
	$(TASK) AR $@
	$(HOSTAR) rcs $@ $^

libtrace.o: libtrace.c libtrace.h
	$(TASK) CC $(notdir $@)
	$(HOSTCC) -MMD -MP $(CFLAGS) -c libtrace.c -o $@


install: $(CURDIR)/../../build/$(ARCH)/sysroot/lib/libtrace.a \
	$(CURDIR)/../../build/$(ARCH)/sysroot/usr/include/trace.h


clean:
	rm -f libtrace.a libtrace.o

.PHONY: install clean

