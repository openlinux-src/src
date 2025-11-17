#!/bin/sh

BOOTFS_IMG="bootfs.img"
ROOTFS_IMG="root.ext4"
MNT_DIR="./mnt"

BOOT_SIZE=64
ROOT_SIZE=128
ROOT_IMG_SIZE=$((ROOT_SIZE - 2))
IMG_SIZE=$((BOOT_SIZE + ROOT_SIZE + 1))

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

if [ "$1" = "__docker" ]; then
	task "IMAGE" "Creating root filesystem image"
	dd if=/dev/zero of="$ROOTFS_IMG" bs=1M count=$ROOT_IMG_SIZE status=none

	task "FORMAT" "Formatting root filesystem image"
	mkfs.ext4 -q -F -L rootfs -m 0 "$ROOTFS_IMG"

	task "MOUNT" "Mounting root filesystem image"
	mkdir -p "$MNT_DIR"
	mount -o loop "$ROOTFS_IMG" "$MNT_DIR" >/dev/null

	task "COPY" "Copying sysroot to root filesystem image"
	cp -a "./build/x86_64/sysroot/." "$MNT_DIR/" >/dev/null

	task "UNMOUNT" "Unmounting root filesystem image"
	umount "$MNT_DIR" >/dev/null

	task "IMAGE" "Creating boot image"
	dd if=/dev/zero of="bootfs.img" bs=1M count=$BOOT_SIZE status=none
	mkfs.fat -F 32 -n EFI "bootfs.img" >/dev/null
	mmd -i "bootfs.img" "::/EFI" "::/EFI/BOOT"
	mcopy -i "bootfs.img" \
		"./build/x86_64/EFI/BOOT/BOOTX64.EFI" \
		"::/EFI/BOOT/BOOTX64.EFI"

	mcopy -i "bootfs.img" \
		"./build/x86_64/EFI/BOOT/initrd" \
		"::/initrd"

	IMG="$O"

	task "IMAGE" "Creating final image"
	dd if=/dev/zero of="$IMG" bs=1M count=$IMG_SIZE status=none

	task "PART" "Creating partition table"
	printf "label: gpt\n%s\n%s\n" \
		"size=${BOOT_SIZE}M,name=boot,type=C12A7328-F81F-11D2-BA4B-00A0C93EC93B" \
		"size=${ROOT_SIZE}M,name=root,type=0FC63DAF-8483-4772-8E79-3D69D8477DE4" |
		sfdisk "$IMG" >/dev/null

	boot_start=$(sfdisk -J "$IMG" | jq -r '.partitiontable.partitions[0].start')
	root_start=$(sfdisk -J "$IMG" | jq -r '.partitiontable.partitions[1].start')

	task "COPY" "Copying root filesystem image to final image"

	dd if="$BOOTFS_IMG" of="$IMG" bs=512 seek="$boot_start" conv=notrunc status=none
	dd if="$ROOTFS_IMG" of="$IMG" bs=512 seek="$root_start" conv=notrunc status=none

	task "CLEANUP" "Cleaning up"
	rm -f "$ROOTFS_IMG" "$BOOTFS_IMG"
	rm -rf "$MNT_DIR"
else
	{
		echo "FROM alpine:latest"
		echo "RUN apk add --no-cache e2fsprogs util-linux cpio" \
			"xz e2fsprogs-extra sfdisk losetup kpartx jq dosfstools mtools"
		echo "WORKDIR /openlinux"
	} >dockerfile

	task "DOCKER" "Building docker image"
	docker buildx build -q --platform=linux/amd64 -t openlinux-image:latest --load . >/dev/null
	task "DOCKER" "Creating OpenLinux image inside docker"
	docker run -q -it --rm --platform linux/amd64 \
		--privileged \
		-e O="$O" \
		-e ARCH="$ARCH" \
		-v "$(pwd)":/openlinux openlinux-image:latest \
		/bin/sh -c "sh tools/image.sh __docker"
	rm -f dockerfile
fi
