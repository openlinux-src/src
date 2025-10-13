include ../../config

SRC_C := $(wildcard *.c)
SRC_S := $(wildcard *.s)
OBJ := $(addprefix ${CURDIR}/,$(SRC_C:.c=.o)) 
OBJ += $(addprefix ${CURDIR}/,$(SRC_S:.s=.o))
DEP := $(OBJ:.o=.d)

# Global headers
CFLAGS += -I$(CURDIR)/../../include 

# bits/asm headers
CFLAGS += -I$(CURDIR)/../libc/arch/$(ARCH)

libm.a: $(OBJ)
	$(TASK) AR $@
	${HOSTAR} cr $@ $^

%.o: %.c
	$(TASK) CC $(notdir $@)
	$(HOSTCC) -MMD -MP $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(TASK) AS $@
	$(CC) -MMD -MP $(CFLAGS) -c -o $@ $<

$(CURDIR)/../../build/$(ARCH)/sysroot/lib/libm.a: libm.a
	$(TASK) INSTALL lib/$^
	cp -f $^ $@

install: $(CURDIR)/../../build/$(ARCH)/sysroot/lib/libm.a

clean:
	rm -f libm.a $(OBJ) $(DEP)

.PHONY: clean install
