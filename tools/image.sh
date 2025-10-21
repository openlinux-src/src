#!/bin/sh

ROOTFS_IMG="root.ext4"
ROOTFS_SIZE=32M
MNT_DIR="./mnt"

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

if [ "$1" = "__docker" ]; then
	task "IMAGE" "Creating root filesystem image"
	dd if=/dev/zero of="$ROOTFS_IMG" bs=1M count=32 status=none

	task "FORMAT" "Formatting root filesystem image"
	mkfs.ext4 -q -F -L rootfs "$ROOTFS_IMG"

	task "MOUNT" "Mounting root filesystem image"
	mkdir -p "$MNT_DIR"
	mount -o loop "$ROOTFS_IMG" "$MNT_DIR" > /dev/null

	task "COPY" "Copying sysroot to root filesystem image"
	cp -a "./build/x86_64/sysroot/." "$MNT_DIR/" > /dev/null

	task "UNMOUNT" "Unmounting root filesystem image"
	umount "$MNT_DIR" > /dev/null

	IMG="$O"
	IMG_SIZE=64M

	task "IMAGE" "Creating final image"
	dd if=/dev/zero of="$IMG" bs=1M count=64 status=none

	task "PART" "Creating partition table"
	printf "label: gpt\nsize=32M,name=root,type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n" | 
		sfdisk "$IMG" > /dev/null

	part_start=$(sfdisk -J "$IMG" | jq -r '.partitiontable.partitions[0].start')
	offset=$((part_start * 512))

	task "COPY" "Copying root filesystem image to final image"
	dd if="$ROOTFS_IMG" of="$IMG" bs=512 seek="$part_start" conv=notrunc status=none

	task "CLEANUP" "Cleaning up"
	rm -f "$ROOTFS_IMG"
	rm -rf "$MNT_DIR"
else
	{
		echo "FROM alpine:latest"
		echo "RUN apk add --no-cache e2fsprogs util-linux cpio xz e2fsprogs-extra sfdisk losetup kpartx jq"
		echo "WORKDIR /openlinux"
	} >dockerfile

	task "DOCKER" "Building docker image"
	docker buildx build -q --platform=linux/amd64 -t openlinux-image:latest --load . > /dev/null
	task "DOCKER" "Creating OpenLinux image inside docker"
	docker run -q -it --rm  --platform linux/amd64 \
		--privileged \
		-e O="$O" \
		-v "$(pwd)":/openlinux openlinux-image:latest \
		/bin/sh -c "sh tools/image.sh __docker"
	rm -f dockerfile
fi
