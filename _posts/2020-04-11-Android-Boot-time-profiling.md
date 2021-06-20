---
layout: post
title: "Boot time profiling for Android"
---

In this post we will cover an overview of Android Boot flow and a simple method
of breaking it down into different milestones and some typical timings from
Android devices for each milestone.

# Android Boot Flow

## Bootloader

- Low level hardware initialization before loading the kernel.
- The bootloader executes in two stages. In the first stage it detects external RAM and loads a program which helps in the second stage. In
  the second stage, the bootloader setups the network, memory, etc, which requires to run kernel. The bootloader is able to provide
  configuration parameters or inputs to the kernel for specific purposes.
- First stage bootloader will also detect certain key presses to enter the recovery mode, fastboot mode etc.

## Kernel

- Core kernel initialization.
        - Memory and I/O areas are initialized.
        - Interrupts are started, process tables are initialized.
- Driver initialization.
- Kernel daemons(threads) are started.
- Root file system is mounted.
- The first user space process(`init`) is started.

## Init

- init is a root process. It has two responsibilities, mount directories like /sys, /dev, /proc and parses the init.rc script that starts,
  among other things, native daemons like Service Manager, Media Server, radio and volume daemons etc..

- The Android runtime is started by init root process with the app_process command, which tells it to start Art or the Dalvik process
  virtual machine and to call Zygote_s main() function.

### Zygote startup

- `init` calls the program `/system/bin/app_process64` and gives the resulting process the name zygote.
```
service zygote /system/bin/app_process64 -Xzygote /system/bin --zygote\
--start-system-server --socket-name=zygote
```
*from the file : `android/system/core/rootdir/init.zygote64_32.rc`*

- Efficient and fast app launch is achieved thanks to the fact that Zygote starts by preloading all classes and resources which an app may
  potentially need at runtime into the system_s memory. It then listens for connections on its socket for requests to start new apps. When
  it gets a request to start an app, it forks itself and launches the new app. It serves as a parent to all Android apps.
- Zygote starts the `SystemServer`

### System Server

- System Server is the first process started by Zygote. After it starts, it lives on as an entirely separate process from its parent. It
starts initializing each system service it houses e.g *Battery Service, Alarm Manager etc.* and registering them with the previously started
Service Manager. It also starts the
Activity Manager.

### Activity Manager

- Activity Manager is responsible for a new Activity thread process creation, maintenance of the Activity life cycle and managing the
  Activity stack. By the end of its startup it also executes an Intent to start a Home Launcher over which it receives onClick events to
  start new apps from icons on the home screen.

## Summary

![Android boot flow](./pictures/bootprocess.png)

# Boot Profiling

- The command `adb logcat -d -b events | grep "boot"` will give you almost all the milestones in the boot process. The log here will have
  `boot_*` tags for e.g. `boot_progress_start` signifies start of Android runtime. From the second reference link you can see:-

```bash
# Device boot timings.  We include monotonic clock values because the
# intrinsic event log times are wall-clock.
#
# Runtime starts:
3000 boot_progress_start (time|2|3)
# ZygoteInit class preloading starts:
3020 boot_progress_preload_start (time|2|3)
# ZygoteInit class preloading ends:
3030 boot_progress_preload_end (time|2|3)
```

## References

- [Events boot log
  tags](https://android.googlesource.com/platform/frameworks/base/+/kitkat-release/services/java/com/android/server/EventLogTags.logtags)
- [Event Log Tags](https://chromium.googlesource.com/aosp/platform/system/core/+/master/logcat/event.logtags)
- [Blog on Boot process with focus on Zygote](https://blog.codecentric.de/en/2018/04/android-zygote-boot-process/)
- [Android Zygote Startup](https://elinux.org/Android_Zygote_Startup)
- [Android Booting blog from NXP](https://community.nxp.com/docs/DOC-102546)
- [Boot flow overview article on elinux](https://elinux.org/Android_Booting)
