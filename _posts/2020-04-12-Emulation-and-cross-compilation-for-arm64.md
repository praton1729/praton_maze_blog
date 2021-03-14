---
layout: post
title: "Emulation and cross compiling arm64 kernel"
---

# Flow of the article

- Setup qemu for different architectures.
- Setup buildroot to create a root file system.
- Compile the kernel for arm64(just an example)
- Use qemu's arm64 virtual machine to load the kernel image.

# Setup tools

## Setup QEMU virtual machines

- Clone the qemu repo and build the executables from the latest stable code.
- Follow the instructions given [here](https://www.qemu.org/download/#source).

## Setup buildroot

- Download the buildroot stable source from [here](https://buildroot.org/download.html). 
- Run `make menuconfig` to configure the buildroot build.
- Follow below steps:-
	- Select target architecture as `AARCH64 little endian`.
	- Inside toolchain select `External Toolchain`.
	- I personally use Linaro Toolchain.
	- Inside `System Configuration` select `Enable root login with
	  password` and set any password.
	- Also select `Run a getty login prompt after boot` option. change the port to "ttyAMA0" inside it.
	- Inside `Target Packages` select `Show packages that are also provided
	  by busybox`
	- Inside `Debugging Tools` select `strace` and whatever you need.
	- Inside `Text Editors` select `vim` for ease.
	- Inside `Filesystem images` select `cpio root filesystem`
	- Exit from the config menu.
- Run `make -j16`.
- This will download toolchain and some files the first time so it might take
  some time.
- The consequent builds would be very fast.

# Cross compilation instructions

## For `x86_64`

### For a default build

- `make ARCH=x86_64 x86_64_defconfig`

## For `arm64`

### For a default build

- `make ARCH=arm64 defconfig`
- `ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- make all -j16 2>&1 | tee
  build.log`
- Tweak the config file as shown below to use the kernel image with qemu.

# Emulation using QEMU

## For `arm64`

### Kernel building tips

- Before building the kernel you would need to tweak a few options in the
  generated config in order to boot it with qemu.
- Set `CONFIG_CMDLINE="console=ttyAMA0"`.
- Set `CONFIG_INITRAMFS_SOURCE=<path to rootfs>`.
- For example for me it was `CONFIG_INITRAMFS_SOURCE=/home/prathubaronia/Downloads/Tools/Buildroot/buildroot-2020.02.6/output/images/rootfs.cpio`.

### Emualting with qemu

- Provide the executable that we built in the first section of the article.
- Use the below bash script for booting the VM.

```
#!/bin/bash
qemu-system-aarch64 \
	-machine virt \
	-cpu cortex-a57 \
	-machine type=virt \
	-nographic \
	-smp 1 -m 2048 \
	-kernel ./arch/arm64/boot/Image \
	--append "console=ttyAMA0" \
	$1 $2
```

**Yay!!!**

# Debugging with gdb

- Ref : https://www.youtube.com/watch?v=swniLhXg-3U&list=PLbv5B7I2l-j4Gj21_h_oh7sSFCYEvPU_m&index=2
- Ref : http://nickdesaulniers.github.io/blog/2018/10/24/booting-a-custom-linux-kernel-in-qemu-and-debugging-it-with-gdb/

# Create Custom initramfs

- Ref : https://balau82.wordpress.com/2012/03/31/compile-linux-kernel-3-2-for-arm-and-emulate-with-qemu/
- Ref : https://gist.github.com/chrisdone/02e165a0004be33734ac2334f215380e
