/* posix.c: POSIX API Implementation */

#include "malloc/counters.h"
#include "malloc/freelist.h"

#include <assert.h>
#include <string.h>

/**
 * Allocate specified amount memory.
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
void *malloc(size_t size) {
    // Initialize counters
    init_counters();

    // Handle empty size
    if (!size) {
        return NULL;
    }

    // TODO: Search free list for any available block with matching size
    Block *block = block_allocate(size);

    // Could not find free block or allocate a block, so just return NULL
    if (!block) {
        return NULL;
    }

    // Check if allocated block makes sense
    assert(block->capacity >= block->size);
    assert(block->size     == size);
    assert(block->next     == block);
    assert(block->prev     == block);

    // Update counters
    Counters[MALLOCS]++;
    Counters[REQUESTED] += size;

    // Return data address associated with block
    return block->data;
}

/**
 * Release previously allocated memory.
 * @param   ptr     Pointer to previously allocated memory.
 **/
void free(void *ptr) {
    if (!ptr) {
        return;
    }

    // Update counters
    Counters[FREES]++;

    // TODO: Try to release block, otherwise insert it into the free list
    Block *block = BLOCK_FROM_POINTER(ptr);
    if (!block_release(block))
        free_list_insert(block);

    // Return pointer to previously allocated memory
}

/**
 * Allocate memory with specified number of elements and with each element set
 * to 0.
 * @param   nmemb   Number of elements.
 * @param   size    Size of each element.
 * @return  Pointer to requested amount of memory.
 **/
void *calloc(size_t nmemb, size_t size) {
    // TODO: Implement calloc
    // Counters[CALLOCS]++;
    char *data = malloc(nmemb * size);
    bzero(data, nmemb*size);
    Counters[CALLOCS]++;
    return data;
}

/**
 * Reallocate memory with specified size.
 * @param   ptr     Pointer to previously allocated memory.
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to requested amount of memory.
 **/
void *realloc(void *ptr, size_t size) {
    // TODO: Implement realloc
    void *newptr;
    size_t oldSiz = sizeof(ptr)/sizeof(void *);
    if (ptr && size == 0){
        free(ptr);
        return NULL;
    }

    else if (!ptr){
        return malloc(size);
    }

    else if (size <= oldSiz){
        Counters[REUSES]++;
        return ptr;
    }
    
    else{
        // This copies old size into newptr. There is still size-oldsize left as added memory.
        newptr = malloc(size);
        if (newptr){
            memcpy(newptr, ptr, oldSiz);
            Counters[REUSES]++;
            free(ptr);
        }
        else{
            return NULL;
        }
    }

        // Unless ptr == NULL, it must have been returned by an earlier call to malloc, calloc, or realloc.
    //
    // If the area pointed to was moved, free(ptr) is called
    Block *block = BLOCK_FROM_POINTER(newptr);
    Counters[REALLOCS]++;
    return block->data;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
