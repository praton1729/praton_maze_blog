---
layout: post
title: "Removing a compiled kernel from the system"
---

# Delete all the relevant files from `/boot`

- **Currently only applicable to Ubuntu. Will update it as I work on other
  systems**

- Delete all the files of the below format from the correpsonding path.
```bash
/boot/vmlinuz*KERNEL-VERSION*
/boot/initrd*KERNEL-VERSION*
/boot/System-map*KERNEL-VERSION*
/boot/config-*KERNEL-VERSION*
/lib/modules/*KERNEL-VERSION*/
/var/lib/initramfs/*KERNEL-VERSION*/
```

- Run `sudo update-grub2`.
- You are done!!
