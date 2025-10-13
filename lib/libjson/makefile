include ../../config

# Global headers
CFLAGS += -I$(CURDIR)/../../include 

# bits/asm headers
CFLAGS += -I$(CURDIR)/../libc/arch/$(ARCH)

libjson.a: json.o
	$(TASK) AR $@
	${HOSTAR} cr $@ $^

json.o: json.c
	$(TASK) CC $(notdir $@)
	$(HOSTCC) -MMD -MP $(CFLAGS) -c -o $@ $<

$(CURDIR)/../../build/$(ARCH)/sysroot/lib/libjson.a: libjson.a
	$(TASK) INSTALL lib/$^
	cp -f $^ $@


install: $(CURDIR)/../../build/$(ARCH)/sysroot/lib/libjson.a

clean:
	rm -f libjson.a $(OBJ) $(DEP)

.PHONY: clean install

