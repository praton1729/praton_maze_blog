---
layout: post
title: "Spinlock history in Linux Kernel"
---

# Engineering problem at hand

- Implement a simple and fast easy to use lock.
	- That can be used from any context.

# History of spinlock

## Normal spinlocks

### Implementation of normal spinlocks

#### Atomic compare and exchange operation

# Common usecases of spinlock

### Problems with normal spinlocks

- There is a lot of cache line bouncing because all contented cpus spin on the same
  lock variable.
- In high contention scenario it is not predictabel who will get the lock once
  its freed. Its possible that the same CPU acquires it again due to presence of
lock in the cacheline in its personal cache. So it is unfair.

## Ticket spinlocks

- Developed by Nick Piggins.
- Solves the problem by providing the lock to the CPU who came first.
- Makes it more fair.

### Early problem with this

- Only 8-bit space reserved for cpu indexing so only 256 CPUs could be
  addressed.
- Later on this space was increased to 16-bit to make the max nr_of_cpus to
  65536.

## Queued Spinlocks

- Solves the problem of cache line bouncing.
- Introduces per-cpu variables on which CPUs can spin instead of the main lock
  and hence avoid cache line bouncing.

# Spinlock interface
## For linux kernel version: v5.13-rc1

# References

- Ticket spinlocks: https://lwn.net/Articles/267968/
- MCS spinlocks: https://lwn.net/Articles/590243/

- Queued spinlocks discussion: https://0xax.gitbooks.io/linux-insides/content/SyncPrim/linux-sync-2.html
