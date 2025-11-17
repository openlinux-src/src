#!/bin/sh

KERNEL_URL="https://github.com/openlinux-src/kernel.git"

PACKAGES="bash git make gcc g++ musl-dev cpio xz git"
PACKAGES="$PACKAGES ncurses-dev flex bison elfutils-dev"
PACKAGES="$PACKAGES openssl-dev perl bc linux-headers"
PACKAGES="$PACKAGES llvm clang lld diffutils findutils"

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

{
	echo "FROM alpine:latest"
	echo "RUN apk add --no-cache $PACKAGES"
	echo "RUN git clone --depth=1 $KERNEL_URL /kernel"
	echo "COPY arch/$ARCH/config/config /kernel/.config"
	echo "WORKDIR /kernel"
	echo "RUN make olddefconfig"
} > dockerfile

task "BUILD" "Building kernel build environment..."
docker buildx build -q --platform=linux/amd64 -t openlinux-image:latest --load . > /dev/null

task "RUN" "Starting kernel build environment..."
docker run -q -it --rm --platform linux/amd64 \
    -v "$PWD/build/$ARCH:/build" \
    openlinux-image:latest \
    /bin/sh

    #/bin/sh -c "yes '' | make -j\$(nproc) LLVM=1 ARCH=$ARCH bzImage; cp vmlinux /build/EFI/BOOT/KERNEL.ELF"

rm -f dockerfile
