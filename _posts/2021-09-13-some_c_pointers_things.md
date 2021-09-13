---
layout: post
title: "Some C pointer things"
---

Just some function, constant and double pointer things.

Here we shed some light on:-
- A constant pointer
- Pointer to a constant
- A constant pointer to a constant
- Pointer to a pointer
- Function pointers

# A constant pointer

- As the name suggests you can't make it to point to something else once declared. 
- If you try reassigning a new address then `gcc` will print a warning.

{% highlight c %}
int main(void)
{
	....
	int a = 1;
	int* const p1 = &a;
	....
}
{% endhighlight %}

# Pointer to a constant 

- As the name suggests you can't change the variable it is pointing to by dereferencing it.
- If you try modifying the variable it is pointing to then `gcc` will print a warning.

{% highlight c %}
int main(void)
{
	....
	int a = 1;
	const int* p1 = &a;
	....
}
{% endhighlight %}

# A constant pointer to a constant

- As the name suggests you can't change the variable it is pointing to by dereferencing it and also you can't change the address it is pointing to.
- If you try modifying any of the above then `gcc` will print a warning.

{% highlight c %}
int main(void)
{
	....
	int a = 1;
	const int* const p1 = &a;
	....
}
{% endhighlight %}

# A pointer to a pointer

- Nothing special, just a pointer pointing to another pointer.
- In the below code `*p1` will have value 1, `*p2` will have `p1`'s value(i.e. var a's address) and `**p2` will have value 1.
- Also obviously `p2` will have `p1`'s address.

{% highlight c %}
int main(void)
{
	....
	int a = 1;
	int* p1 = &a;
	int** p2 = &p1;
	....
}
{% endhighlight %}

# Function pointer

- Nothing special. Just a pointer to a function.
- Declaration of a function pointer returning an `int` and taking `int` arguments is shown.

{% highlight c %}
int (*fptr)(int, int);
{% endhighlight %}

{% highlight c %}
int foo(int x, int y)
{
	....
}

int main(void)
{
	....
	int (*fptr)(int, int);
	fptr = foo;
	foo(a,b);
	fptr(a,b); // Same as foo(a,b)
	....
}
{% endhighlight %}
