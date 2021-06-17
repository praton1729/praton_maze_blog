---
layout: post
title: "kmem caches in Linux Kernel"
---

# Engineering problem at hand

- Repeated allocating and deallocating frequently used structures.
	- Find out why is it a problem really.

# How does kmem cache solve this?

# Common usecases of kmem cache

- task_struct : Representation of a process.
- mm_struct : Process VA.
- inode structs in fs code: Presentation of file metadata in RAM.

# kmem cache interface

# Implementation of kmem caches
## For linux kernel version: v5.13-rc1

# References
