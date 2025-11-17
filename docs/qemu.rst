QEMU
====

Contents
--------

1. Overview
2. Prerequisites
3. Build artifacts
4. Running via make
5. Running manually
6. Architecture notes
7. Serial console and logging
8. Common workflows
9. Troubleshooting
10. See also

Overview
--------

This document explains how to run OpenLinux images under QEMU. The recommended entry point
is the `qemu` make target, which delegates to a helper script. You generally do not need to
call QEMU directly unless you want to experiment with different machine settings.

Prerequisites
-------------

Install a QEMU system emulator that matches your target architecture:

- For x86_64: qemu-system-x86_64
- For aarch64: qemu-system-aarch64
- For armv7-m (microcontroller-class, no MMU): usage varies and may require a specific board
  model; see “Architecture notes”.

You can verify availability with:

::

  qemu-system-x86_64 --version
  qemu-system-aarch64 --version

On macOS, QEMU can be installed via Homebrew:

::

  brew install qemu

Build artifacts
---------------

The QEMU run path expects that you have already built the disk image:

- Disk image: ``build/<ARCH>/openlinux-<VERSION>-<ARCH>.img``

Where:

- ``<ARCH>`` is selected by you (e.g., ``x86_64``, ``aarch64``)
- ``<VERSION>`` comes from the top-level makefile (e.g., ``1.0.0-alpha``)

To build the image:

::

  # Build everything and the bootable disk image
  make image
  # or explicitly:
  make all image

Running via make
----------------

The easiest way to run under QEMU is:

::

  make qemu

This invokes the helper script with the current architecture and image path:

::

  sh tools/qemu.sh $(ARCH) build/$(ARCH)/openlinux-$(VERSION)-$(ARCH).img

Notes:

- You can switch architectures by passing `ARCH` to make:

  ::

    make ARCH=x86_64 qemu
    make ARCH=aarch64 qemu

- Inspect `tools/qemu.sh` to see the exact QEMU options used (machine type, CPU, memory,
  acceleration, console, etc.). The script is the source of truth for runtime flags.

Running manually
----------------

If you need to run QEMU manually (for custom flags or debugging), use your system emulator
for the selected architecture and point it at the built disk image. For example, conceptually:

::

  # Example shape only — consult tools/qemu.sh and your QEMU docs for correct flags
  qemu-system-<arch> -hda build/<ARCH>/openlinux-<VERSION>-<ARCH>.img

Important:

- Exact options (machine type, CPU model, memory size, firmware/BIOS, serial console, acceleration)
  depend on your host and target architecture. Prefer the `qemu` make target unless you know
  what you need to change.
- If you change kernel command line or console settings, ensure they align with what the image
  expects (see “Serial console and logging”).

Architecture notes
------------------

x86_64:

- System emulator: ``qemu-system-x86_64``
- Typical PC-compatible machine and BIOS/UEFI environment
- Works well with serial console ``ttyS0``

aarch64:

- System emulator: ``qemu-system-aarch64``
- Requires selecting an appropriate machine/board model and firmware configuration
- Serial device naming differs by machine; ensure kernel console targets the device configured
  by the run script

armv7-m:

- Microcontroller class (no MMU), often run with a specific SoC/board in QEMU
- Disk images and filesystems may not apply in the same way as PC/server-class targets
- Use only if the rest of the build and arch support is configured for this profile

Serial console and logging
--------------------------

The default kernel command line (see `docs/boot.rst`) configures the console as:

- ``kernel.console=ttyS0``

Implications:

- You should see kernel and early userspace logs on the first serial port.
- The QEMU run configuration must attach the guest’s first serial device to something visible
  on your host (e.g., a terminal or QEMU’s stdio). When using `make qemu`, the helper script
  handles this for you.

Common workflows
----------------

Fast iteration loop:

::

  # 1) Build artifacts
  make -j8
  # 2) Rebuild disk image (if needed)
  make image
  # 3) Boot in QEMU
  make qemu

Switching architectures:

::

  make distclean
  make ARCH=aarch64 image
  make ARCH=aarch64 qemu

Capturing logs:

- Ensure the serial console is active (``ttyS0`` as configured).
- If the helper script routes serial to stdio, you can redirect output:

  ::

    make qemu | tee qemu.log

Troubleshooting
---------------

- QEMU binary not found:

  - Symptom: `make qemu` fails with “command not found” or similar.
  - Fix: Install the appropriate `qemu-system-*` for your target architecture and ensure it’s on PATH.

- Image not built:

  - Symptom: `tools/qemu.sh` or QEMU errors about missing image file.
  - Fix: Run `make image` first to produce `build/<ARCH>/openlinux-<VERSION>-<ARCH>.img`.

- No console output:

  - Symptom: QEMU window/terminal remains blank.
  - Fix: Ensure the kernel console is set to `ttyS0` (see `docs/boot.rst`) and that the QEMU
    run uses a serial device that maps to your host’s stdio or a visible window. Prefer `make qemu`.

- Acceleration not available:

  - Symptom: Slow emulation or warnings about missing acceleration backends.
  - Fix: Acceleration options are host-dependent (e.g., KVM on Linux, HVF on macOS). Use the
    helper script defaults; if you experiment manually, consult your QEMU documentation.

- UEFI/firmware issues:

  - Symptom: Boot failures related to firmware or machine type.
  - Fix: Use `make qemu` to run with the known-good configuration from `tools/qemu.sh`.

See also
--------

- Build system and artifacts: `docs/build.rst`
- Boot process and kernel command line: `docs/boot.rst`
