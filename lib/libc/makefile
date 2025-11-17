include ../../config

SRC := $(wildcard */*.c) $(wildcard arch/${ARCH}/*.c)
SRC_ASM := $(wildcard arch/${ARCH}/*.s)
OBJ := $(addprefix ${CURDIR}/,$(SRC:.c=.o))
OBJ += $(addprefix ${CURDIR}/,$(SRC_ASM:.s=.o))
DEP := $(OBJ:.o=.d)

libc.a: $(OBJ)
	$(TASK) AR $@
	${HOSTAR} cr $@ $^

$(OBJ): $(CURDIR)/arch/$(ARCH)/include/linux/version.h

# Global headers
CFLAGS += -I$(CURDIR)/../../include

# Libc private headers
CFLAGS += -I$(CURDIR)/include

# Arch specific headers
CFLAGS += -I$(CURDIR)/arch/$(ARCH)

# Kernel user api
CFLAGS += -I$(CURDIR)/arch/$(ARCH)/include

$(CURDIR)/arch/$(ARCH)/include/linux/version.h:
	$(MAKE) -C $(KERNEL) KBUILD_HOSTLDFLAGS= ARCH=$(ARCH) headers_install INSTALL_HDR_PATH="$(CURDIR)/arch/$(ARCH)"

%.o: %.c
	$(TASK) CC $(notdir $@)
	$(HOSTCC) -MMD -MP $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(TASK) AS $(notdir $@)
	$(HOSTCC) -Wno-unused-command-line-argument -MMD -MP $(CFLAGS) -c -o $@ $<

$(CURDIR)/../../build/$(ARCH)/sysroot/lib/libc.a: libc.a
	$(TASK) INSTALL libc.a
	cp -f libc.a $(CURDIR)/../../build/$(ARCH)/sysroot/lib

BITS := $(wildcard $(CURDIR)/arch/$(ARCH)/bits/*.h)
DEST := $(CURDIR)/../../build/$(ARCH)/sysroot/usr/include/bits

BITS_DEST := $(patsubst $(CURDIR)/arch/$(ARCH)/bits/%,$(DEST)/%,$(BITS))

$(DEST)/%.h: $(CURDIR)/arch/$(ARCH)/bits/%.h
	@mkdir -p $(DEST)
	$(TASK) INSTALL $<
	cp -f $< $@

install: $(BITS_DEST) $(CURDIR)/../../build/$(ARCH)/sysroot/lib/libc.a

clean:
	rm -rf libc.a ${OBJ} ${DEP} $(CURDIR)/arch/${ARCH}/include

.PHONY: install clean

ifdef DEP
-include $(DEP)
endif
