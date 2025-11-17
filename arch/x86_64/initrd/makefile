include ../../../config

CFLAGS += -static
CFLAGS += -I../../../include
CFLAGS += -I../../../lib/libc/include
CFLAGS += -I../../../lib/libc/arch/$(ARCH)
CFLAGS += -I../../../lib/libc/arch/$(ARCH)/include

LIBC := ../../../build/x86_64/sysroot/lib/libc.a

init: init.c $(LIBC)
	$(TASK) CC $@
	$(HOSTCC) $(CFLAGS) $(LDFLAGS) -o $@ init.c $(LIBC)

initrd: bootconfig.txt init
	$(TASK) CPIO $@
	printf '.\ninit\n' | cpio -H newc -o > initrd
	python3 mkinitrd.py
	mv initrd.img $@

install: initrd
	$(TASK) INSTALL $^
	mkdir -p ../../../build/x86_64/EFI/BOOT
	cp -f initrd ../../../build/x86_64/EFI/BOOT/initrd

clean:
	rm -f initrd init

.PHONY: install clean
