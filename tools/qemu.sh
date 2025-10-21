#!/bin/sh

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

if [ ! -f "build/x86_64/vmlinuz" ]; then
	task "ERROR" "Kernel not found! Please build the kernel first."
	exit 1
fi

task "INFO" "Starting QEMU..."

ARCH="$1"
OSIMAGE="$2"
BZIMAGE="build/x86_64/vmlinuz"

qemu-system-"$ARCH" \
	-cpu max \
	-kernel "$BZIMAGE" \
	-drive file="$OSIMAGE",format=raw,if=none,id=mydrive \
	-device virtio-blk-pci,drive=mydrive \
	-append "root=/dev/vda1 rw rootwait earlyprintk=serial loglevel=8 ignore_loglevel console=ttyS0 init=/bin/init" \
	--nographic
