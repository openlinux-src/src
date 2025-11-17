BUILD
=====

Contents
--------

1. Overview
2. Prerequisites
3. Architecture selection
4. Build outputs and layout
5. Common build tasks
6. Running with QEMU
7. Building a Docker image
8. Tests and formatting
9. Cleaning up
10. Environment variables reference
11. Adding binaries and libraries
12. Troubleshooting

Overview
--------

The build system is driven by a top-level `makefile` and a shared `config` that defines
toolchain settings, enabled libraries, and architecture-specific options.

Typical workflows include:

- Building the sysroot with headers, libraries, and userland binaries
- Producing a bootable disk image
- Producing a kernel image (via helper script)
- Creating a compressed rootfs tarball
- Running the image in QEMU
- Packaging artifacts for Docker


Prerequisites
-------------

You will need the following tools available on your PATH:

- clang, lld, llvm-ar (or `llvm-*` tools compatible with your platform)
- make, rsync, find, grep
- tar
  - On macOS, GNU tar is required (installed as `gtar`). The build automatically uses `gtar` on Darwin.
- ccache (optional; used automatically if present)
- qemu-system-* (for running under QEMU)
- docker (optional; for the `docker` target)
- clang-format (for the `format` target)

macOS notes:

- Install LLVM and GNU tar (e.g., via Homebrew: `brew install llvm gnu-tar`).
- The build auto-selects `gtar` on Darwin; no manual setup is required beyond installation.


Architecture selection
----------------------

Supported values:

- ``x86_64``
- ``aarch64``
- ``armv7-m``

Select the architecture by passing `ARCH` to make, or by editing it in `config`:

::

  make ARCH=x86_64
  make ARCH=aarch64
  make ARCH=armv7-m


Build outputs and layout
------------------------

Artifacts are generated under ``build/<ARCH>/``. Key paths:

- Sysroot:
  - ``build/<ARCH>/sysroot/`` (root)
  - ``build/<ARCH>/sysroot/usr/include`` (headers)
  - ``build/<ARCH>/sysroot/lib`` (static libraries)
  - ``build/<ARCH>/sysroot/bin`` (userland binaries)

- Images and archives:
  - Kernel image: ``build/<ARCH>/openlinux-kernel.img``
  - Disk image: ``build/<ARCH>/openlinux-<VERSION>-<ARCH>.img``
  - Rootfs tarball: ``build/<ARCH>/openlinux-<VERSION>-<ARCH>-rootfs.tar.gz``

Where ``<VERSION>`` is defined in the top-level `makefile` (e.g., `1.0.0-alpha`).


Common build tasks
------------------

- Full build (headers, libraries, userland, arch install):

  ::

    make
    # or explicitly:
    make all

- Build kernel image:

  ::

    make kernel
    # Produces: build/<ARCH>/openlinux-kernel.img

- Build full disk image:

  ::

    make image
    # Produces: build/<ARCH>/openlinux-<VERSION>-<ARCH>.img

- Build rootfs tarball:

  ::

    make rootfs
    # Produces: build/<ARCH>/openlinux-<VERSION>-<ARCH>-rootfs.tar.gz

- Build and install packages into the sysroot:

  ::

    make packages

- Build Docker artifacts from the rootfs (see “Building a Docker image”):

  ::

    make docker

Parallelism:

- The default parallelism is configured via `MAKEFLAGS` in `config` (e.g., `-j8`).
- You can override it at invocation time:

  ::

    make -j16


Running with QEMU
-----------------

To run the built image under QEMU:

::

  make qemu
  # Internally runs: sh tools/qemu.sh $(ARCH) build/<ARCH>/openlinux-<VERSION>-<ARCH>.img

Notes:

- Ensure the appropriate QEMU system binary is installed for your target architecture (e.g.,
  `qemu-system-x86_64`, `qemu-system-aarch64`).
- Serial console is configured; see `docs/boot.rst` for kernel command line details and console settings.


Building a Docker image
-----------------------

You can produce a Docker-ready root filesystem tarball and run a helper to import it:

::

  make docker
  # Internally runs: sh tools/docker.sh $(ARCH) build/<ARCH>/openlinux-<VERSION>-<ARCH>-rootfs.tar.gz

Requirements:

- `docker` must be installed and running.
- The tarball is created by the `rootfs` target (which is a prerequisite of `docker`).


Tests and formatting
--------------------

- Run tests:

  ::

    make tests
    # Internally: sh tools/test.sh

- Format C sources in-place (excluding build and sys directories):

  ::

    make format
    # Requires clang-format


Cleaning up
-----------

- Remove built objects from libraries/binaries and arch outputs:

  ::

    make clean

- Remove all build artifacts (including `build/`):

  ::

    make distclean


Environment variables reference
-------------------------------

You can set these variables on the `make` command line to override defaults.

- ``ARCH``: Target architecture (``x86_64``, ``aarch64``, ``armv7-m``).

  ::

    make ARCH=aarch64

- ``KERNEL``: Path to the kernel source/build directory (default comes from `config`):

  ::

    make KERNEL=/absolute/path/to/kernel

- ``CCACHE``: If present on your PATH, it’s used automatically. You can disable use by
  unsetting or overriding `HOSTCC`:

  ::

    make HOSTCC=clang

- ``HOSTCC``, ``HOSTLD``, ``HOSTAR``: Toolchain commands (default to `clang`, `lld`, and `llvm-ar`).

  ::

    make HOSTCC=clang-18 HOSTLD=ld.lld HOSTAR=llvm-ar

- ``VERSION``: Artifact version tag (set in the top-level `makefile`). Overriding is generally
  not necessary but supported:

  ::

    make VERSION=1.0.0-custom

Toolchain and flags (defined in `config`):

- `CFLAGS` are set for cross-compilation with `-target ${ARCH}-linux-eabi`, `-march=...`,
  `-nostdlib`, LTO, and size/perf-oriented flags.
- `LDFLAGS` uses `lld` with section garbage collection and no build-id for deterministic outputs.

macOS-specific:

- TAR implementation is auto-selected: `gtar` on Darwin, `tar` elsewhere.


Adding binaries and libraries
-----------------------------

Libraries:

- Enabled static libraries are listed in `LIBS` in `config` (e.g., `libc libm libjson libuefi`).
- The top-level build installs each library into the sysroot. To add a new library:
  1. Place it under `lib/<name>`.
  2. Ensure it has a build that conforms to the shared `config` (usually by including it).
  3. Add the library name to `LIBS` in `config`.
  4. Run `make` to build and install it into `build/<ARCH>/sysroot/lib`.

Binaries:

- Userland binaries live under `bin/` subdirectories and typically include the shared `config`.
- The shared `config` supports per-binary builds when the sub-makefile defines:

  - `BIN`: Absolute output path to the binary (the sub-makefile often assembles this using `$(CURDIR)`).
  - `SRC`: Source files that make up the binary.

- When included, the shared `config`:
  - Derives `OBJ` and `DEP` from `SRC`.
  - Adds appropriate include paths (e.g., sysroot headers and arch-specific libc headers).
  - Links statically against the sysroot libc by default.
  - Exposes an `install` target that copies the resulting binary into
    `build/<ARCH>/sysroot/bin`.

Typical subdir workflow:

::

  # In bin/<tool>/Makefile (example sketch)
  # include ../../config
  # BIN := tool               # or $(CURDIR)/tool
  # SRC := main.c util.c

  # Then:
  make install
  # Installs to: build/<ARCH>/sysroot/bin/<tool>

Arch-specific installation:

- The top-level `__all` target also runs `make -C arch/$(ARCH) install` so that arch-specific
  components are installed into the sysroot and/or included in images.


Troubleshooting
---------------

- GNU tar missing on macOS:

  - Symptom: Errors mentioning unknown tar flags or failure to create the rootfs tar.
  - Fix: `brew install gnu-tar` and re-run the build. The build auto-selects `gtar` on Darwin.

- QEMU not found or wrong system binary:

  - Symptom: `make qemu` fails or QEMU cannot boot the image.
  - Fix: Install the correct `qemu-system-*` binary for your `ARCH` (e.g., `qemu-system-x86_64`).

- clang/lld not found:

  - Symptom: Compilation or linking fails early with “command not found.”
  - Fix: Install LLVM toolchain and ensure `clang`, `lld`, and `llvm-ar` are on PATH.
    You can also override `HOSTCC`, `HOSTLD`, `HOSTAR`.

- Headers not available in sysroot:

  - Symptom: Includes cannot be found when compiling userland tools.
  - Fix: Ensure the `build` target has run, which populates `build/<ARCH>/sysroot/usr/include`
    from the top-level `include/` directory.

- Docker target fails:

  - Symptom: `make docker` errors out with Docker connection issues.
  - Fix: Ensure Docker is installed, running, and your user has permission to access the Docker daemon.

- Large rebuilds are slow:

  - Recommendation: Ensure `ccache` is installed to speed up iterative builds. It will be used automatically if present.


See also
--------

- Boot process and kernel command-line details: `docs/boot.rst`
