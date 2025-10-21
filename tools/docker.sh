#!/bin/sh

ARCH="$1"
ROOTFS_TAR="$2"
DOCKER_ARCH=""

if [ "$ARCH" = "x86_64" ]; then
	DOCKER_ARCH="amd64"
elif [ "$ARCH" = "aarch64" ]; then
	DOCKER_ARCH="arm64"
else
	echo "Unsupported architecture: $ARCH"
	exit 1
fi

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

{
	echo "FROM scratch"
	echo "ADD ${ROOTFS_TAR} /"
	echo "CMD [\"/bin/sh\"]"
} > dockerfile

task "BUILD" "docker image openlinux:latest for ${ARCH}"
docker buildx build -q --platform=linux/${DOCKER_ARCH} -t openlinux:latest --load . > /dev/null

task  "RUN" "OpenLinux docker image for ${ARCH}"
docker run --rm --platform=linux/${DOCKER_ARCH} -it openlinux:latest /bin/sh

rm -f dockerfile
