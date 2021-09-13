---
layout: post
title: "A simple die function"
---
Just a simple die function using variadic arguments.

- Referenced from Linus Torvald's stupid TLB tester.

{% highlight c %}
#include<stdarg.h> // for va_* macros
#include<stdlib.h> // for exit() call

static void die(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
	fputc('\n', stderr);
	exit(1);
}

// sample call
void foo()
{
	if(found_some_error)
		die("Found error no: %d", error_code);
}
{% endhighlight %}
