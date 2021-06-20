---
layout: post
title: Basic Kernel Debugging Techniques
---

Gist of an introductory session that I took on how to analyze different kinds of
kernel issues. I have tried to keep it generic buyt somethings are none the less
Qualcomm chipset specific.

# Scope of talk
- dmesg analysis, dump analysis, tools
- General kernel debugging
- procfs and sysfs nodes for userspace level information and tweaking
- sysrq functions
- We will do some demos if demo gods are with us.

## dmesg:
- Data comes from /proc/kmsg kernel ring buffer.
- /proc/kmsg --> klogd --> syslogd (priority is decided here)
- Need root permission to extract usually so basically you would need sdebug
  builds.
- Can be extracted using the bugreport in production builds since we don't have
  root excess.

- We can one add logs from userspace to kernel logs using klogd APIs.
- Use of ratelimiting APIs.
- CONFIG_LOG_BUF_SHIFT : The minimal kernel log buffer size as a power of 2 to
  this config's value. 4KB to 1MB.
- Busybox's implementation of dmsg in embedded devices.

### Main sections of dmesg:
- Kernel loading
- init first stage
- init second stage
- Driver init for peripherals.

## sysrq keys:
It is a 'magical' key combo you can hit which the kernel will respond to
regardless of whatever else it is doing, unless it is completely locked up.

## dump_analysis:
- Types of dumps
- Dumps parsers

### Dumps:
- Minidump
- Full dump

### Parsing flows

#### Minidump

Mini Dump.gz --> Extract Minidump --> Split Minidump --> Decrypt Minidump -->
Generate ap_minidump.elf file --> parse decrypted dump through LRDPv2 --> launch
T32 with the generated script or launch crash

#### Full dump

Full Dump --> parse through LRDPv2 --> launch T32 with the generated script or
launch crash

## Pulling config:
- `adb pull /proc/config.gz`
- CONFIG_IKCONFIG_PROC

## faddr2line

`faddr2line vmlinux function_name+offset`

## Decoding stack trace

- decode_stacktrace.sh
- Show input and output traces.

## Simple setup with qemu
- Booting with a qemu based arm64 vm
- Really fast way to test general kernel changes

## FTRACE

- CONFIG_FUNCTION_TRACER
- CONFIG_FUNCTION_GRAPH_TRACER
- CONFIG_STACK_TRACER
- CONFIG_DYNAMIC_FTRACE

### Frameworks and tools
- trace-cmd
- Kernelshark
- Systemtap
- Perf

## Techniques
- Logs and dump analysis
- Tracing/profiling
- Interactive debugging 

## References
- https://elinux.org/images/1/14/Linuxkerneldebugging.pdf
