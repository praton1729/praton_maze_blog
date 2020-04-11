---
layout: post
title: "Emulation and cross compiling arm64 kernel"
---

# Cross compilation instructions

## For `x86_64`

### For a default build

- `make ARCH=x86_64 x86_64_defconfig`
- Need to create a working ramdisk/initramfs.

## For `arm64`

### For a default build

- `make ARCH=arm64 defconfig`
- `ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- make all -j16 2>&1 | tee
  build.log`
- Need to create a working ramdisk/initramfs.

# Emulation using QEMU

## For `arm64`

- `qemu-system-aarch64 -machine raspi2 -kernel arch/arm64/boot/Image -nographic
  --append "console=ttyS0" -m 512`
- Currently the above command gets stuck on my machine for some reason.

## For `x86_64`

- `qemu-system-x86_64 -kernel arch/x86/boot/bzImage -nographic --append
  "console=ttyS0" -initrd /boot/initrd.img-5.3.0-42-generic -m 512`
- Currently the above command boots the kernel but gets stuck when loading the
  initramfs as root partition.
- Need to create a working ramdisk/initramfs.

# Debugging with gdb

- Ref : http://nickdesaulniers.github.io/blog/2018/10/24/booting-a-custom-linux-kernel-in-qemu-and-debugging-it-with-gdb/

# Create Custom initramfs

- Ref : https://balau82.wordpress.com/2012/03/31/compile-linux-kernel-3-2-for-arm-and-emulate-with-qemu/
- Ref : https://gist.github.com/chrisdone/02e165a0004be33734ac2334f215380e
