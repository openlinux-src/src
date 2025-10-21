include config

VERSION := 1.0.0-alpha
ROOTFS_TAR := openlinux-$(VERSION)-$(ARCH)-rootfs.tar.gz

all: __all

rootfs: all
	$(TASK) TAR ${ROOTFS_TAR}
	$(TAR) --owner=0 --group=0 --numeric-owner -czf build/$(ARCH)/${ROOTFS_TAR} -C build/$(ARCH)/sysroot .

qemu: image
	sh tools/qemu.sh $(ARCH) build/$(ARCH)/openlinux-$(VERSION)-$(ARCH).img

docker: rootfs
	sh tools/docker.sh $(ARCH) build/$(ARCH)/${ROOTFS_TAR}

build/$(ARCH)/openlinux-kernel.img:
	mkdir -p build/$(ARCH)
	O="$@" ARCH="$(ARCH)" sh tools/kernel.sh

kernel: build/$(ARCH)/openlinux-kernel.img

packages: all
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

build/$(ARCH)/openlinux-$(VERSION)-$(ARCH).img: all
	mkdir -p build/$(ARCH)
	O="$@" sh tools/image.sh

image: build/$(ARCH)/openlinux-$(VERSION)-$(ARCH).img

tests:
	sh tools/test.sh


clean:
	for lib in $(LIBS); do \
		$(MAKE) -C lib/$$lib clean; \
	done
	$(MAKE) -C bin clean

distclean: clean
	rm -rf build

.PHONY: all packages kernel rootfs build tests pack image format distclean clean
