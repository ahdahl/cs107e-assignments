/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by tacking on the requested
 * space to the end of the heap thus far. 
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include <stddef.h> // for NULL
#include "strings.h"

extern int __bss_end__;

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define TOTAL_HEAP_SIZE 0x1000000 // 16 MB


/* Global variables for the bump allocator
 *
 * `heap_start` tracks where heap segment begins in memory.
 * It is initialized to point to the address at end of data segment.
 * It uses symbol __bss_end__ from memmap to locate end.
 * `heap_used` tracks the number of bytes allocated thus far.
 * The next available memory begins at heap_start + heap_used.
 * `heap_max` is total number of bytes set aside for heap segment.
 */
static void *heap_start = NULL;
static int heap_used = 0, heap_max = TOTAL_HEAP_SIZE;

void *malloc(size_t nbytes) 
{
    // TODO: replace with your code.
    if (!heap_start) {
        heap_start = &__bss_end__;
    }
    nbytes = roundup(nbytes, 8);
    if (heap_used + nbytes > heap_max)
        return NULL;
    void *alloc = (char *)heap_start + heap_used;
    heap_used += nbytes;

    return alloc;
}

void free(void *ptr) 
{
    // TODO: fill in your own code here.
}

void *realloc (void *old_ptr, size_t new_size)
{
    // TODO: adapt to your allocator.
    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;
    // ideally would copy the min of new_size and old_size, but this allocator
    // doesn't know the old_size. Why not? 
    // Why is it "safe" (but not efficient) to always copy new_size?
    memcpy(new_ptr, old_ptr, new_size);
    free(old_ptr);
    return new_ptr;
}

void heap_dump () {
    // TODO: fill in your own code here.
}
