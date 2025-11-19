# Build

## Contents

1. Overview
2. Prerequisites
3. Architecture Selection
4. Build Outputs and Layout
5. Common Build Tasks
6. Running with QEMU
7. Building a Docker Image
8. Tests and Formatting
9. Cleaning Up
10. Environment Variables Reference
11. Adding Binaries and Libraries
12. Troubleshooting
13. See Also

---

## 1. Overview

The build system is driven by a top-level `Makefile` and a shared `config` file that defines:

- Toolchain settings
- Enabled libraries
- Architecture-specific options

Typical workflows include:

- Building the sysroot (headers, libraries, userland binaries)
- Producing a kernel image
- Producing a bootable disk image
- Creating a compressed rootfs tarball
- Running the image in QEMU
- Packaging artifacts for Docker

---

## 2. Prerequisites

Ensure the following tools are available on your `PATH`:

- `clang`, `lld`, `llvm-ar` (or other `llvm-*` tools compatible with your platform)
- `make`, `rsync`, `find`, `grep`
- `tar`
  - On macOS, GNU tar is required (installed as `gtar`). The build auto-selects `gtar` on Darwin.
- `ccache` (optional; used automatically if present)
- `qemu-system-*` (for running under QEMU)
- `docker` (optional; for the `docker` target)
- `clang-format` (for the `format` target)

macOS setup (example via Homebrew):

```sh
brew install llvm gnu-tar qemu
```

The build auto-selects `gtar` on Darwin—no extra config required.

---

## 3. Architecture Selection

Supported architectures:

- `x86_64`
- `aarch64`
- `armv7-m`

Select via `ARCH` on the command line or by editing `config`:

```sh
make ARCH=x86_64
make ARCH=aarch64
make ARCH=armv7-m
```

---

## 4. Build Outputs and Layout

Artifacts are generated under:

```
build/<ARCH>/
```

Key paths:

- Sysroot:
  - `build/<ARCH>/sysroot/` (root)
  - `build/<ARCH>/sysroot/usr/include` (headers)
  - `build/<ARCH>/sysroot/lib` (static libraries)
  - `build/<ARCH>/sysroot/bin` (userland binaries)

- Images and archives:
  - Kernel image: `build/<ARCH>/openlinux-kernel.img`
  - Disk image: `build/<ARCH>/openlinux-<VERSION>-<ARCH>.img`
  - Rootfs tarball: `build/<ARCH>/openlinux-<VERSION>-<ARCH>-rootfs.tar.gz`

`<VERSION>` is defined in the top-level `Makefile` (e.g., `1.0.0-alpha`).

---

## 5. Common Build Tasks

Full build (headers, libraries, userland, arch install):

```sh
make
# or explicitly:
make all
```

Kernel image:

```sh
make kernel
# -> build/<ARCH>/openlinux-kernel.img
```

Disk image:

```sh
make image
# -> build/<ARCH>/openlinux-<VERSION>-<ARCH>.img
```

Rootfs tarball:

```sh
make rootfs
# -> build/<ARCH>/openlinux-<VERSION>-<ARCH>-rootfs.tar.gz
```

Packages into sysroot:

```sh
make packages
```

Docker artifacts:

```sh
make docker
```

Parallelism:

- Default set via `MAKEFLAGS` in `config` (e.g., `-j8`)
- Override manually:

```sh
make -j16
```

---

## 6. Running with QEMU

Run the built image:

```sh
make qemu
# Internally:
# sh tools/qemu.sh $(ARCH) build/<ARCH>/openlinux-<VERSION>-<ARCH>.img
```

Notes:

- Install correct `qemu-system-*` for your architecture (`qemu-system-x86_64`, `qemu-system-aarch64`, etc.)
- Serial console configuration is described in the Boot documentation.

---

## 7. Building a Docker Image

Create a Docker-ready root filesystem and import:

```sh
make docker
# Internally:
# sh tools/docker.sh $(ARCH) build/<ARCH>/openlinux-<VERSION>-<ARCH>-rootfs.tar.gz
```

Requirements:

- `docker` installed and running
- `rootfs` target builds the tarball used by `docker`

---

## 8. Tests and Formatting

Run tests:

```sh
make tests
# Internally: sh tools/test.sh
```

Format C sources (excluding build/sys directories):

```sh
make format
# Requires clang-format
```

---

## 9. Cleaning Up

Remove library/object/binary outputs:

```sh
make clean
```

Remove all build artifacts (including `build/`):

```sh
make distclean
```

---

## 10. Environment Variables Reference

Override defaults on the `make` command line.

`ARCH` (target architecture):

```sh
make ARCH=aarch64
```

`KERNEL` (path to kernel source/build directory):

```sh
make KERNEL=/absolute/path/to/kernel
```

`CCACHE` (auto-used if present; disable by overriding `HOSTCC`):

```sh
make HOSTCC=clang
```

Toolchain commands:

```sh
make HOSTCC=clang-18 HOSTLD=ld.lld HOSTAR=llvm-ar
```

`VERSION` (artifact tag; usually leave as defined in `Makefile`):

```sh
make VERSION=1.0.0-custom
```

Toolchain flags (from `config`):

- `CFLAGS`: Cross-target (`-target ${ARCH}-linux-eabi`), `-march=...`, `-nostdlib`, LTO, size/perf optimizations.
- `LDFLAGS`: `lld` with section GC, deterministic outputs (no build-id).

macOS:

- Uses `gtar` automatically.

---

## 11. Adding Binaries and Libraries

### Libraries

- Enabled static libraries listed in `LIBS` in `config` (e.g., `libc libm libjson libuefi`).
- To add a library:
  1. Create `lib/<name>/`
  2. Provide a Makefile including the shared `config`
  3. Add the name to `LIBS` in `config`
  4. Run `make` — library installed to `build/<ARCH>/sysroot/lib`

### Binaries

- Userland binaries live under `bin/<tool>/`
- Sub-Makefile typically sets:
  - `BIN` (absolute output path or name)
  - `SRC` (sources)

Including `config` auto-derives objects/deps, adds include paths, links statically, and exposes `install`:

Example workflow in `bin/<tool>/Makefile`:

```make
# include ../../config
# BIN := $(CURDIR)/tool
# SRC := main.c util.c
```

Install:

```sh
make install
# -> build/<ARCH>/sysroot/bin/<tool>
```

Architecture-specific installation:

- Top-level `__all` also runs: `make -C arch/$(ARCH) install`

---

## 12. Troubleshooting

GNU tar missing on macOS:

- Symptom: Unknown tar flags / rootfs creation failure
- Fix: `brew install gnu-tar`

QEMU not found:

- Symptom: `make qemu` fails
- Fix: Install correct `qemu-system-*` binary

LLVM tools missing:

- Symptom: Early “command not found”
- Fix: Install LLVM (`clang`, `lld`, `llvm-ar`) and ensure PATH correctness

Headers missing during userland builds:

- Symptom: Include errors
- Fix: Run base build to populate `build/<ARCH>/sysroot/usr/include`

Docker target fails:

- Symptom: Connection errors
- Fix: Ensure daemon is running and user has permissions

Slow rebuilds:

- Recommendation: Install `ccache` for faster iterations

---

## 13. See Also

- Boot Process: `boot.md`
- QEMU Usage: `qemu.md`
- Docker Workflow: `docker.md`

---
