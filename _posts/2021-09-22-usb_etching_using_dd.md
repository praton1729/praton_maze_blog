---
layout: post
title: "Simple USB etching using dd utility"
---
Just a simple usb etching command using dd to track progress.

{% highlight bash %}
sudo dd bs=1M status=progress oflag=sync if=~/Downloads/ubuntu-20.04-desktop-amd64.iso of=/dev/<block-dev-for-usb>
{% endhighlight %}

- `bs`: block size to transfer in one iteration. Can be experimented with.
- `status=progress`: Prints progress statistics.
- `oflag=sync`: Runs the `sync` command after dd to ensure the transfer is complete.
- `if`: input file.
- `of`: output file. Needs to point to the correct block device i.e. mostly the usb.
	- Can be found out using `lsblk` utility.
