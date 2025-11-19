# QEMU

> Run OpenLinux disk images under QEMU for fast, reproducible testing.

## Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Build Artifacts](#build-artifacts)
4. [Running via `make`](#running-via-make)
5. [Running Manually](#running-manually)
6. [Architecture Notes](#architecture-notes)
7. [Serial Console & Logging](#serial-console--logging)
8. [Common Workflows](#common-workflows)
9. [Troubleshooting](#troubleshooting)
10. [See Also](#see-also)

---

## Overview

This document explains how to boot OpenLinux images under QEMU. The recommended entry point is the `qemu` make target, which invokes a helper script (`tools/qemu.sh`). Direct invocation of QEMU is optional unless you want to experiment with machine, CPU, or acceleration settings.

---

## Prerequisites

Install a QEMU _system_ emulator matching your target architecture:

| Target  | Binary                                                   |
| ------- | -------------------------------------------------------- |
| x86_64  | `qemu-system-x86_64`                                     |
| aarch64 | `qemu-system-aarch64`                                    |
| armv7-m | Depends on specific board model (see Architecture Notes) |

Verify installation:

```sh
qemu-system-x86_64 --version
qemu-system-aarch64 --version
```

macOS (Homebrew):

```sh
brew install qemu
```

Ensure the binary is on your PATH.

---

## Build Artifacts

QEMU expects a built disk image:

- Disk image: `build/<ARCH>/openlinux-<VERSION>-<ARCH>.img`

Where:

- `<ARCH>`: `x86_64`, `aarch64`, etc.
- `<VERSION>`: Defined in the top-level `makefile` (e.g., `1.0.0-alpha`)

Build the image (after full build):

```sh
make image
# or:
make all image
```

---

## Running via `make`

Fast path:

```sh
make qemu
```

Internally equivalent to:

```sh
sh tools/qemu.sh $(ARCH) build/$(ARCH)/openlinux-$(VERSION)-$(ARCH).img
```

Switching architectures:

```sh
make ARCH=x86_64 qemu
make ARCH=aarch64 qemu
```

Inspect the script (`tools/qemu.sh`) for exact flags (machine type, CPU, memory, console, acceleration).

---

## Running Manually

Manual invocation is useful for experimentation. Conceptually:

```sh
# Shape only — consult tools/qemu.sh for correct flags
qemu-system-<arch> -hda build/<ARCH>/openlinux-<VERSION>-<ARCH>.img
```

Considerations:

- Machine type (`-machine`), CPU model (`-cpu`), memory (`-m`), acceleration (`-accel kvm` / `-accel hvf`), serial devices (`-serial stdio`), and firmware options vary per architecture and host.
- Stick to `make qemu` unless you have a specific change in mind.

---

## Architecture Notes

### x86_64

- Emulator: `qemu-system-x86_64`
- Standard PC-compatible environment (BIOS/UEFI)
- Serial console: `ttyS0` (matches default kernel command line)

### aarch64

- Emulator: `qemu-system-aarch64`
- Requires explicit machine/board model (e.g., `-machine virt`)
- Serial device naming differs; ensure kernel console matches what the script configures.

### armv7-m

- Microcontroller-class (often no MMU)
- May need a board model (e.g., `-machine some_board`)
- Filesystem + disk image workflows might differ; only use if the broader build system supports this profile.

---

## Serial Console & Logging

Kernel command line (see `boot.md`) configures:

```
kernel.console=ttyS0
```

Implications:

- Early kernel and init output appear on the first serial port.
- The helper script should attach `ttyS0` to your terminal (often via `-serial stdio`).
- If customizing flags manually, ensure you keep a working serial configuration.

Capturing output:

```sh
make qemu | tee qemu.log
```

---

## Common Workflows

### Fast Iteration

```sh
make -j8           # Rebuild userland/libraries
make image         # Recreate disk image (if content changed)
make qemu          # Boot in QEMU
```

### Switching Architecture

```sh
make distclean
make ARCH=aarch64 image
make ARCH=aarch64 qemu
```

### Log Capture for Debugging

```sh
make qemu | tee boot-session.txt
```

---

## Troubleshooting

| Symptom                             | Cause                                             | Fix                                                                               |
| ----------------------------------- | ------------------------------------------------- | --------------------------------------------------------------------------------- |
| `make qemu` fails: binary not found | QEMU not installed or wrong arch                  | Install appropriate `qemu-system-*`                                               |
| Image missing error                 | Disk image not built                              | Run `make image`                                                                  |
| Blank / no console output           | Serial device mismatch or wrong console parameter | Verify kernel console is `ttyS0`; use `make qemu` defaults                        |
| Slow execution                      | No acceleration (KVM/HVF) available               | Ensure host supports KVM (Linux) or HVF (macOS); otherwise performance is limited |
| Boot firmware errors                | Wrong machine type or missing firmware            | Use helper script; avoid manual experimental flags unless necessary               |
| Unexpected hang                     | Incorrect flags during manual invocation          | Revert to `make qemu`; compare with `tools/qemu.sh`                               |

Detailed Cases:

- Missing acceleration:
  - Warning text like “Could not initialize KVM”.
  - Possible on macOS if Hypervisor.framework not available or on Linux inside container.
- No output for >10s:
  - Check serial mapping, ensure not using a graphical interface with hidden serial.
- Kernel panics early:
  - Confirm disk image rebuild; mismatch between kernel and userspace can cause failures.

---

## See Also

- Build system & artifacts: `build.md`
- Boot process & kernel command line: `boot.md`
- Docker usage: `docker.md`

---

## Reference Checklist

Before running:

- [ ] Architecture selected (`ARCH` matches built artifacts)
- [ ] Disk image exists in `build/<ARCH>/`
- [ ] QEMU system binary installed and on PATH
- [ ] Kernel console matches run script (`ttyS0`)
- [ ] (Optional) Acceleration available (KVM/HVF)

After boot:

- [ ] Serial output visible
- [ ] `/init` (or specified init) executed
- [ ] Able to interact with shell or specified userland tools

---

## Appendix: Example Expanded Command (Illustrative)

Below is an annotated _example shape_ (do not copy verbatim; consult `tools/qemu.sh`):

```sh
qemu-system-x86_64 \
  -machine type=q35,accel=kvm \
  -cpu host \
  -m 512M \
  -drive file=build/x86_64/openlinux-1.0.0-alpha-x86_64.img,format=raw,if=virtio \
  -serial stdio \
  -display none
```

Key flags:

- `-machine`: Selects chipset + acceleration backend.
- `-cpu`: Guest CPU model (may use `host` for compatibility).
- `-drive`: Attaches the disk image (virtio recommended for performance).
- `-serial stdio`: Routes guest serial to your terminal.
- `-display none`: Headless mode (all interaction via serial).

Match these with what the helper script uses for consistency.

---
