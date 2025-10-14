#!/bin/sh

ROOT=$(pwd)
ROOTFS=$(realpath "$ROOT/build/$ARCH/sysroot")

task() {
	printf '  \033[34m%+8s\033[m %s\n' "$1" "$2"
}

mkdir -p build/"$ARCH"/packages

for package in tools/package.d/*; do
	package_name=$(basename "$package")
	task PKG "$package_name"
	mkdir -p "build/$ARCH/packages/$package_name"
	_path=$(realpath "$package")
	cd "build/$ARCH/packages/$package_name" || exit 1
	ROOT=$ROOT ROOTFS=$ROOTFS ARCH=$ARCH sh "$_path" || exit 1
	find root | sed 's/^root//' >"$package_name.list"
	cd - >/dev/null || exit 1

	gtar czf "build/$ARCH/packages/$package_name.pkg" -C "build/$ARCH/packages/$package_name" .
	rm -rf "build/$ARCH/packages/$package_name"
done
