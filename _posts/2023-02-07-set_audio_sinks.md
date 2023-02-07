---
layout: post
title: "Set default source/sink in pulseaudio"
---

Useful when connecting behringer C1-U microphone to my linux systems

# Problem

- The source & sink are both set to the behringer microphone and monitor.
- Hence the output is not heard.

# Solution

- List all the sinks
	- `pactl list short sinks`
- Set the default sink
	- `pactl set-default-sink <device-name>`
- Restart pulseaudio daemon
	- `pulseaudio -k`
- Output should be fixed by now. Check.

## Alternate solution

- Edit the `/etc/pulse/default.pa` and set the default source/sink.
