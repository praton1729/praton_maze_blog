---
layout: post
title: Adjusting source and sink using pulseaudio
---

This was used in my audacity setup with Behringer's C-1U microphone.

## Set default source
```bash
pactl set-default-source alsa_input.usb-Behringer_C-1U-00.analog-stereo
```
## Set default sink
```bash
pactl set-default-sink alsa_output.pci-0000_00_1f.3.analog-stereo
```
