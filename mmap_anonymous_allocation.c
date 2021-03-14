/*
 * =====================================================================================
 *
 *       Filename:  mmap_anonymous_allocation.c
 *
 *    Description:  Allocating anonymous memory through malloc and mmap
 *                  Since mmap allocates the zeroed out memory and for malloc
 *		    we need to explicitly zero it out.
 *                  So basically comparing the timings of both.
 *
 *        Version:  1.0
 *        Created:  Tuesday 10 November 2020 07:04:42  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  PRATHU BARONIA (praton), prathu.baronia@oneplus.com
 *   Organization:  OnePlus RnD
 *
 * =====================================================================================
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>

#define MB_SIZE (1 << 20)
#define NSECS_PER_SEC 1000000000ULL

#define CLOCK_IT(timespec_struct) clock_gettime(CLOCK_MONOTONIC_RAW,&timespec_struct)

typedef struct timespec* my_timer;

int print_time_taken(my_timer start, my_timer end){
	unsigned long long delta;
	delta = NSECS_PER_SEC * (end->tv_sec - start->tv_sec);
	delta += (end->tv_nsec - start->tv_nsec);
	printf("Experiment took %f secs\n", (double)(delta / (double)NSECS_PER_SEC));
	return 0;
}

int do_malloc_experiment(int buf_size_bytes){
	void *malloc_buf;
	struct timespec start, end;

	CLOCK_IT(start); 
	malloc_buf = malloc(buf_size_bytes); 
	memset(malloc_buf, 0, buf_size_bytes);
	CLOCK_IT(end);
	printf("Malloc allocation and zeroing:\n");
	print_time_taken(&start, &end);
	free(malloc_buf);
	return 0;
}

int do_mmap_experiment(int buf_size_bytes){
	void *mmap_buf;
	struct timespec start, end;

	CLOCK_IT(start); 
	mmap_buf = mmap(NULL, buf_size_bytes, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	memset(mmap_buf, 1, buf_size_bytes);
	CLOCK_IT(end);
	printf("Mmaping and writing 1:\n");
	print_time_taken(&start, &end);
	munmap(mmap_buf, buf_size_bytes);
	return 0;
}
int main(int argc, char* argv[])
{
	int ret = 0;
	int buf_size = atoi(argv[1]);
	int buf_size_bytes = buf_size * MB_SIZE;

	printf("Doing experiment for %d MB anonymous allocation\n\n", buf_size);

	ret = do_malloc_experiment(buf_size_bytes);
	assert(ret==0);

	printf("------------Break-----------\n");

	ret = do_mmap_experiment(buf_size_bytes);
	assert(ret==0);

	return 0;
}
