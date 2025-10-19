include config

VERSION := 1.0.0-alpha

ROOTFS_TAR := openlinux-$(VERSION)-$(ARCH)-rootfs.tar.gz

all: __all

pack:
	$(TASK) TAR ${ROOTFS_TAR}
	gtar --owner=0 --group=0 --numeric-owner -czf build/$(ARCH)/${ROOTFS_TAR} -C build/$(ARCH)/sysroot .
	ARCH=${ARCH} sh tools/package.sh

__all: | build
	for lib in $(LIBS); do \
		$(MAKE) -C lib/$$lib install; \
	done
	$(MAKE) -C bin install

build/$(ARCH)/sysroot/usr/include:
	mkdir -p $@

build: build/$(ARCH)/sysroot/usr/include
	mkdir -p $@/$(ARCH)/sysroot/lib
	mkdir -p $@/$(ARCH)/sysroot/bin
	rsync -aq --no-perms include/ $@/$(ARCH)/sysroot/usr/include

format:
	find . -name "*.c" -o -name "*.h" | grep -v build | grep -v sys | xargs -r clang-format -i


clean:
	for lib in $(LIBS); do \
		$(MAKE) -C lib/$$lib clean; \
	done
	$(MAKE) -C bin clean
	rm -rf build

.PHONY: all build pack format clean
