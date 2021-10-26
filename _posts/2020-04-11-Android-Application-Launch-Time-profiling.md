---
layout: post
title:  "Android App launch time profiling"
---

In this post we will discuss some basic and advanced ways of measuring
application launch times in Android. Plots to be added soon.

### Application creation

When your application launches, the blank starting window remains on the screen until the system finishes drawing the app for the first
time. At that point, the system process swaps out the starting window for your app, allowing the user to start interacting with the app.

If you've overloaded Application.onCreate() in your own app, the system invokes the onCreate() method on your app object. Afterwards, the
app spawns the main thread, also known as the UI thread, and tasks it with creating your main activity.

From this point, system- and app-level processes proceed in accordance with the app life cycle stages.

#### Activity creation

After the app process creates your activity, the activity performs the following operations:

- Initializes values.
- Calls constructors.
- Calls the callback method, such as Activity.onCreate(), appropriate to the current life cycle state of the activity.

Typically, the onCreate() method has the greatest impact on load time, because it performs the work with the highest overhead: loading and
inflating views, and initializing the objects needed for the activity to run.

# App Launch Profiling

The two most common metrics for the memory usage of a process are the virtual set size, Vss, and the resident set size, Rss: you will see
see these numbers in ps and top.

- Vss = virtual set size
- Rss = resident set size
- Pss = proportional set size
- Uss = unique set size

Vss, also called VIRT and VSZ is the total amount of virtual memory of the process has mapped, regardless of whether it has been committed
to physical memory

Rss, also called RES and RSS, is the amount of physical memory being mapped

The Vss is plainly an overestimate because applications commonly allocate memory they never use. Rss is a better measure, but still an
overestimate because it does not take into account pages of memory that are shared between processes. For example, there is only one copy of
the C library resident in memory because it is shared between all the processes that link with it yet Rss accounts for it multiple times.

Some years ago, Matt Mackall looked at the problem and added two new metrics called the Unique Set Size, Uss, and the Proportional Set Size,
Pss, and modified the kernel to expose the information needed to calculate them, which you will find in `/proc/[PID]/smaps`.

- Uss is unique set size, which is the amount of memory that is private to the process and is not shared with any other This is the amount
  of memory that would be freed if the application was terminated right now.

- Pss is the amount of memory shared with other processes, accounted in a way that the amount is divided evenly between the processes that
share it. This is memory that would not be released if the process was terminated, but is indicative of the amount that this process is
"contributing" to the overall memory load.

In general, the two numbers you want to watch are the Pss and Uss (Vss and Rss are generally worthless, because they don't accurately
reflect a process's usage of pages shared with other processes.)

We have adopted two methods to measure application launch times which are outlined below:-

## Logcat grepping

This method measures time by launching the application from the host through a script and then parses `logcat` to find out the launch time.
Procedure:-

- Launch an application through the script.
- Executing this `adb logcat | grep Displayed` command will output the below statement when the first screen of the application is drawn on
  the screen.

`ActivityManager: Displayed com.android.myexample/.StartupTiming: +3s534ms`

- The last value in the output(for example here `3secs and 534ms`) depicts the time to draw the first screen of the application.
- This method is a bit superficial and does not give accurate results but is found effective for initial testing phase.

### Typical values for application launch

| **Application** | **STD Build(in ms)** |
| :---------: | :--------------------: |
| Firefox | 271.9 |
| Spotify | 304.8 |
| Maps  | 390.5 |
| Youtube | 460.8 |
| Chrome | 226.5 |

## Anon Memory consumption saturation

This method is more detailed/involved and is supposed to provide an accurate number for the launch time. This involves a modified version of
the readily available utility named `procrank` and a python-based real-time plotter. `procrank` by default covers all the processes in `/proc`
and computes the memory consumption for each process and also takes in account the shared memory between the processes. For our purposes we
modify `procrank` to compute the memory consumption of a single process which we provide it as a pid in its argument chain. This version of
`procrank` prints the `Uss` consumption of the process and the real time plotter plots it on the host.

Here we launch a python script on the host which will launch the application and then plot the data collected from `procrank` in real-time.
A trend can be observed in the consumption of the anon memory as the real time curve peaks and saturates after complete loading of the
application. The point beyond which the variance drops below say `10%`(heuristic measure) in the anon memory consumption we call it the
complete launch point and measure the time stamp corresponding to it and call it the launch time of the application.

- Embed a picture for Youtube and Firefox plots here.

The Uss is useful because it shows the pages that are unique. You can think of it as the price you would pay in memory if you forked
that process to create a copy.

# References

- [Elinux article on Android Memory usage](https://elinux.org/Android_Memory_Usage)
- [procrank tutorial](http://www.2net.co.uk/tutorial/procrank)
- [Google App startup time](https://developer.android.com/topic/performance/vitals/launch-time)

