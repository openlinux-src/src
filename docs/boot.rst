BOOT
====

Contents
--------

1. Introduction
2. Boot Stages
3. Boot Process Overview

Introduction
------------

The boot process is a critical sequence of operations that occurs when a computer is powered on. 
It encompasses several stages, including hardware initialization, loading the operating system, 
and preparing the system for user interaction. Understanding the boot process is essential 
for troubleshooting startup issues and optimizing system performance.

Boot Stages
-----------

1. **Power-On Self Test (POST)**  
   Upon power-on, the BIOS or UEFI firmware performs the POST to verify that hardware components 
   are functioning correctly.

2. **Kernel Execution**  
   After POST, the bootloader loads the operating system kernel into memory and transfers 
   control to it.

3. **System Initialization**  
   The kernel initializes system components, mounts the root filesystem, and starts essential 
   kernel-level services.

4. **User Space Initialization**  
   The system transitions to user space, starting user-level services and the graphical 
   interface, if applicable.

Boot Process Overview
--------------------

A question often arises regarding the role of the bootloader. The bootloader is a small program 
residing in the Master Boot Record (MBR) or GUID Partition Table (GPT) of a storage device. Its 
primary function is to load the operating system kernel into memory and transfer control to it. 
Common bootloaders include GRUB for Linux systems and NTLDR for legacy Windows systems.

However, modern Linux kernels support the EFI stub feature, which allows the kernel to be 
executed directly without a traditional bootloader. In our case, we utilize this EFI stub feature 
and do not employ an external bootloader.

Kernel command-line parameters are configured via the `bootconfig.txt` file within the initramfs. 
This file is read by the kernel during the early boot stage. 

Here is an example of a `bootconfig.txt` file:

kernel.root=/dev/vda2
kernel.rw
kernel.loglevel=8
kernel.ignore_loglevel
kernel.console=ttyS0
kernel.init=/init

This configuration specifies the following:

- `kernel.root=/dev/vda2` : Sets the root filesystem to `/dev/vda2`.
- `kernel.rw` : Enables read-write mode.
- `kernel.loglevel=8` : Sets the kernel log verbosity level.
- `kernel.ignore_loglevel` : Forces log messages to be printed regardless of loglevel.
- `kernel.console=ttyS0` : Configures the primary console device.
- `kernel.init=/init` : Specifies the initial process to run from the initramfs.

After parsing this file, the kernel executes the `/init` process from the initramfs. This 
initial user-space program is responsible for:

- Further system initialization (e.g., mounting `sysfs`, `procfs`, `devtmpfs`).
- Preparing the environment for the real root filesystem.
- Transitioning to the real root filesystem using `pivot_root` and executing `/bin/init` 
  from the actual root filesystem.
