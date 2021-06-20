---
layout: post
title : "ION Memory Allocator part-1"
---

A history of the Ion memory allocator in Android. We discuss its need and its
evolution over time and finally its dropping from the mainline linux kernel
because of introduction of dmabuf-heaps.

# What is ion memory allocator?

ION is a generalized memory manager that Google introduced in the Android 4.0
ICS (Ice Cream Sandwich) release to address the issue of fragmented memory
management interfaces across different Android devices. There are at least
three, probably more, PMEM-like interfaces. On Android devices using NVIDIA
Tegra, there is "NVMAP"; on Android devices using TI OMAP, there is "CMEM"; and
on Android devices using Qualcomm MSM, there is "PMEM" . All three SoC vendors
switched to ION.

The main goal of Android's ION subsystem is to allow for allocating and sharing
of buffers between hardware devices and user space in order to enable zero-copy
memory sharing between devices. This sounds simple enough, but in practice it's
a difficult problem. On system-on-chip (SoC) hardware, there are usually many
different devices that have direct memory access (DMA). These devices, however,
may have different capabilities and can view and access memory with different
constraints. For example, some devices may handle scatter-gather lists, while
others may be able to only access physically contiguous pages in memory. Some
devices may have access to all of memory, while others may only access a smaller
portion of memory. Finally, some devices might sit behind an I/O memory
management unit (IOMMU), which may require configuration to give the device
access to specific pages in memory.

If you have a buffer that you want to share with a device, and the buffer isn't
allocated in memory that the device can access, you have to use bounce buffers
to copy the contents of that memory over to a location where the other devices
can access it. This can be expensive and greatly hurt performance. So the
ability to allocate a buffer in a location accessible by all the devices using
the buffer is important.

Thus ION provides an interface that allows for centralized allocation of
different "types" of memory (or "heaps"). In current kernels without ION, if
you're trying to share memory between a DRM graphics device and a video4linux
(V4L) camera, you need to be sure to allocate the memory using the subsystem
that manages the most-constrained device. Thus, if the camera is the most
constrained device, you need to do your allocations via the V4L kernel
interfaces, while if the graphics is the most constrained device, you have to do
the allocations via the Graphics Execution Manager (GEM) interfaces. ION instead
provides one single centralized interface that allows 
applications to allocate memory that satisfies the required constraints.

A memory pool is a kernel object that allows memory blocks to be dynamically
allocated from a designated memory region. The memory blocks in a memory
pool can be of any size, thereby reducing the amount of wasted memory when an
application needs to allocate storage for data structures of different sizes.

# Why was it introduced?

Ion arose mostly out of necessity for Android. When Google first released
Android, most hardware blocks required large chunks of contiguous memory. Most
vendors also had their own custom driver for allocating and managing this memory
because features like CMA did not exist yet. Google eventually got tired of this
and decided to write a unified manager for vendors to use for their HALs.

# Hardware requirement

Any device which can only do DMA to a physically contiguous area requires (in
the absence of an I/O memory management unit) a physically contiguous buffer to
work with.
What we are seeing, though, are devices which have managed to gain capabilities
while retaining the contiguous DMA requirement. For example, there are video
capture engines which can grab full high-definition data, perform a number of
transformations on it, but still need a contiguous buffer for the result. The
advent of high definition video has aggravated the problem - those
physically-contiguous buffers are now quite a bit bigger and harder to allocate
than they were before.

# What is basic working methodology?

Like its PMEM-like predecessors, ION manages one or more memory pools, some of
which are set aside at boot time to combat fragmentation or to serve special
hardware needs. GPUs, display controllers, and cameras are some of the hardware
blocks that may have special memory requirements. ION presents its memory pools
as ION heaps. Each type of Android device can be provisioned with a different
set of ION heaps according to the memory requirements of the device.

Ion was written to have three main purposes: allocation of memory, mapping of
memory, and sharing of memory.
Most applications call malloc or kmalloc to allocate heap memory and will never
think twice about that. Drivers have to be a bit more thoughtful about where
their memory is coming from. Most modern CPUs have an MMU to make memory appear
contiguous even if isn’t in reality. Some hardware blocks may have their own MMU
(sometimes called an IOMMU or a System MMU) to do something simlar to the CPU
MMU. Certain memory may be optimized for hardware blocks so it’s beneficial to
be able to get memory from a particular area. Ion attempts to manage this
through an abstraction called ‘heaps’. A heap represents a particular type of
memory. Common heap types are system memory, carveout and DMA. Users of Ion can
pass in the ID of a particular heap to allocate corresponding memory. There are
APIs to do this both in the kernel and userspace via an ioctl interface.
Sharing of buffers is done by passing the file descriptor of the allocated
buffer between processes using mechanisms like binder IPC.
The default ION driver (which can be cloned from here) offers three heaps as
listed below:
```C
   ION_HEAP_TYPE_SYSTEM:        memory allocated via vmalloc_user().
   ION_HEAP_TYPE_SYSTEM_CONTIG: memory allocated via kzalloc.
   ION_HEAP_TYPE_CARVEOUT:	carveout memory is physically contiguous and set
aside at boot.
```
Developers may choose to add more ION heaps. For example, this NVIDIA patch was
submitted to add ION_HEAP_TYPE_IOMMU for hardware blocks equipped with an IOMMU.
Constraint solving. One of the things Ion does a reasonable job of is
abstracting away hardware requirements. A heap on one system can be contiguous
memory and discontiguous memory on another. 
‘what’s not right with ION allocation’ - Userspace is required to understand
device buffer constraints, and available heaps.
Ultimately though, the users may still have to make an informed choice about
which heap to allocate from to ensure the right type of memory is allocated.
Sumit Semwal has been doing some work on cenalloc to develop a constraint based
allocator. This would determine what type of memory to allocate based on what
devices are present in the system.  This is a big hurdle in moving ION to the
main kernel tree.

# Userspace usage

Typically, user space device access libraries will use ION to allocate large
contiguous media buffers. For example, the still camera library may allocate a
capture buffer to be used by the camera device. Once the buffer is fully
populated with video data, the library can pass the buffer to the kernel to be
processed by a JPEG encoder hardware block. The buffer passed to kernel driver
is managed in terms of a struct which is extracted from the file descriptor of
the buffer.
User-space clients interact with ION using the ioctl() system call interface. To
allocate a buffer, the client makes this call:
```C
   int ioctl(int client_fd, ION_IOC_ALLOC, struct ion_allocation_data
*allocation_data)
```
For smart phone use cases involving multimedia middleware, the user process
typically allocates the buffer from ION, obtains a file descriptor using
the ION_IOC_SHARE command, then passes the file desciptor to a kernel driver.
The kernel driver calls ion_import_fd() which converts the file descriptor to
an ion_handle object, as shown below:
```C
    struct ion_handle *ion_import_fd(struct ion_client *client, int
fd_from_user);
```
# More roles of ION

Another major role that ION plays as a central buffer allocator and manager is
handling cache maintenance for DMA.
ION allows for buffer users to set a flag describing the needed cache behavior
on allocations.

# Similarities with dma_buf framework:-

One caveat is that while ION can export dma-bufs it won't import dma-bufs
exported from other drivers.

# Issues with ion:

One of the complex issues when handling a buffer that is shared between devices
and CPUs is deciding who can access it at any given time. This is because of the
caches: a processor's accesses typically involve the cache, while device
accesses may not. Concurrent access may cause a mismatch between the cache and
memory, leading to data corruption. To handle this issue, the drivers and
applications must declare when they need to access shared memory for reading or
writing; this allows the kernel to manage the caches correctly.

# Destaging of ION:

The ION allocator has been in use since around 2012, but it remains stuck in the
kernel's staging tree. The work to add ION to the mainline started in 2013; at
that time, the allocator had multiple issues that made inclusion impossible.
Recently, John Stultz posted a patch set introducing DMA-BUF heaps, an evolution
of ION, that is designed to do exactly that — get the Android DMA-buffer
allocator to the mainline Linux kernel.
ION was developed, out of tree, in parallel with in-tree kernel APIs like DMA
buffer sharing (DMA-BUF) and the contiguous memory allocator (CMA). It naturally
duplicates parts of their functionality. In addition, as ION's first platform
was Android on 32-bit ARM processors, it used ARM-specific kernel APIs when
there were no generic ones available. This obviously did not help the
upstreaming process. The new DMA-BUF heaps patch set is a complete rework of the
ION internals: it uses CMA to implement a physically contiguous heap from a
special memory zone and it does not make use of any architecture-specific
functions. A self-test included with the patch set presents the API. This
interface has been merged in 5.6 kernel.

# References:-

- [Great overview with a lot of good references](https://lwn.net/Articles/565469/)
- [ION API discussioni](https://lwn.net/Articles/480055/)
- [Great ion article with history](https://www.labbott.name/blog/2015/10/22/ion-past-and-future/)
- [Issues with ion by Sumit Sehwal](https://www.linuxplumbersconf.net/2015/ocw//system/presentations/3273/original/Upstreaming%20ION-%20issues%20that%20remain.pdf)
- [Ion and coherency issues](http://www.linuxplumbersconf.net/2014/ocw//system/presentations/2409/original/04%20-%20iondma_foreview_v2.pdf)
- [Destaging of ION](https://lwn.net/Articles/792733/)
- [Basic quora answer](https://www.quora.com/What-is-the-importance-of-ion-memory-in-Android)
