#!/bin/sh

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

ARCH="$1"
OSIMAGE="$2"

task "INFO" "Starting QEMU..."

if [ ! -f "$OSIMAGE" ]; then
	task "ERROR" "System image not found! Please build the image first."
	exit 1
fi

qemu-system-x86_64 \
	-drive if=pflash,format=raw,readonly=on,file=/opt/homebrew/share/qemu/edk2-x86_64-code.fd \
	-drive if=pflash,format=raw,file=/opt/homebrew/share/qemu/edk2-i386-vars.fd \
	-drive file="$OSIMAGE",format=raw,if=none,id=drive \
	-device virtio-blk-pci,drive=drive,bootindex=0 \
	-cpu max \
	--nographic

