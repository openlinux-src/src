include config

VERSION := 1.0.0-alpha

ROOTFS_TAR := openlinux-$(VERSION)-$(ARCH)-rootfs.tar.gz

all: __all
	$(TASK) TAR ${ROOTFS_TAR}
	gtar -czf build/$(ARCH)/${ROOTFS_TAR} -C build/$(ARCH)/sysroot .
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

clean:
	for lib in $(LIBS); do \
		$(MAKE) -C lib/$$lib clean; \
	done
	$(MAKE) -C bin clean
	rm -rf build

.PHONY: all build clean
