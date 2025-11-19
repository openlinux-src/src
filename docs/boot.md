# BOOT

## Contents

1. [Introduction](#introduction)  
2. [Boot Stages](#boot-stages)  
3. [Boot Process Overview](#boot-process-overview)  
4. [Kernel Command Line via bootconfig.txt](#kernel-command-line-via-bootconfigtxt)  
5. [Early Init Responsibilities](#early-init-responsibilities)  

---

## Introduction

The boot process is the ordered sequence that begins when a machine powers on and ends when the operating system transitions into normal user-space operation. It covers:

- Firmware / hardware initialization
- Loading and starting the kernel
- Kernel internal setup
- Pivot into user space and launching the first process

A solid understanding of these steps helps with troubleshooting slow boots, kernel panics, or missing devices, and is essential when customizing low-level behavior.

---

## Boot Stages

1. **Power-On Self Test (POST)**  
   BIOS or UEFI firmware performs diagnostic checks to verify hardware integrity and initializes foundational chipset state.

2. **Kernel Execution**  
   The boot medium (firmware or a bootloader) locates the kernel image, places it into memory, and transfers control.

3. **System Initialization (Kernel Space)**  
   The kernel sets up memory management, interrupts, scheduler, device model scaffolding, mounts the initial (early) root (e.g., initramfs), and starts essential internal subsystems.

4. **User Space Initialization**  
   Control transitions to the user-space initial process (often `/init` from the initramfs). That process performs higher-level setup, prepares the real root filesystem, and eventually starts `/bin/init` (PID 1).

---

## Boot Process Overview

Traditionally a bootloader (e.g., GRUB) resides in the Master Boot Record (MBR) or is loaded via UEFI, and is responsible for:

- Presenting a menu (optional)
- Loading the kernel (and optionally an initramfs)
- Passing a command line and handing execution to the kernel

Modern Linux kernels can leverage an **EFI stub**—allowing firmware to load the kernel directly without an intermediate bootloader. In this project we intentionally omit a separate bootloader and rely on that EFI stub capability.

### Kernel Parameters Source

Kernel command-line parameters are provided by a `bootconfig.txt` file embedded in the initramfs. The kernel reads and parses this very early, before most subsystems are fully initialized. This mechanism centralizes tunables (root device, console, verbosity, etc.) in a single, reproducible artifact.

Example `bootconfig.txt`:

```/dev/null/bootconfig.txt#L1-7
kernel.root=/dev/vda2
kernel.rw
kernel.loglevel=8
kernel.ignore_loglevel
kernel.console=ttyS0
kernel.init=/init
```

Explanation:

- `kernel.root=/dev/vda2` — The device containing the real root filesystem.
- `kernel.rw` — Mount root read-write (as opposed to read-only).
- `kernel.loglevel=8` — Maximum verbosity for kernel messages (debug-level).
- `kernel.ignore_loglevel` — Forces printing of all messages regardless of dynamic adjustments.
- `kernel.console=ttyS0` — Directs primary console output to the first serial port.
- `kernel.init=/init` — Path of the initial user-space program in the initramfs.

---

## Kernel Command Line via bootconfig.txt

Using a file-based boot configuration (instead of editing firmware variables or a bootloader config) yields:

- Deterministic, version-controlled boot parameters
- Simplified rebuild workflows (regenerate initramfs → updated kernel arguments)
- Easier reproducibility across emulators (QEMU), physical devices, and containers (when applicable)

Best practices:

- Keep the file minimal—only include parameters you actively rely upon.
- Document any non-obvious flags near the top of the file or in this doc.
- When experimenting, add new parameters in a separate branch to avoid confusion in shared environments.

---

## Early Init Responsibilities

After parsing the configuration and mounting the initramfs, the kernel execs `/init` (the early user-space “stage 1” program). Typical responsibilities of this process:

1. Mount essential pseudo filesystems:
   - `proc` → `/proc`
   - `sysfs` → `/sys`
   - `devtmpfs` → `/dev`
2. Perform hardware or virtual environment discovery (if needed).
3. Set up device nodes or symlinks expected by later initialization.
4. Validate and, if necessary, repair the real root filesystem.
5. Mount the real root filesystem (the block device specified by `kernel.root`).
6. Switch from the initramfs to the real root using `pivot_root` (or `switch_root` depending on design).
7. Execute the true init (PID 1), typically `/bin/init`, which:
   - Starts service supervision
   - Launches gettys or a login manager
   - Applies runtime configuration (network, logging, etc.)

If any of these steps fail (e.g., root device missing), the early init should emit clear error messages to the configured console (`ttyS0` in our setup) and ideally drop into a rescue shell when possible.

---

## Summary

- We use EFI stub booting: no external bootloader.
- `bootconfig.txt` in the initramfs drives kernel parameters.
- Serial console is pinned to `ttyS0` for consistent logging (important for QEMU runs).
- The `/init` process performs foundational setup before handing off to the real init system.

For QEMU runtime specifics and console integration, see `qemu.md`. For build system details (including image assembly and embedding of the initramfs), see `build.md`.

---

## See Also

- Build process and artifacts: `build.md`
- Running under QEMU: `qemu.md`
- Docker usage (rootfs considerations): `docker.md`

---
