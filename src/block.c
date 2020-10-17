/* block.c: Block Structure */

#include "malloc/block.h"
#include "malloc/counters.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/**
 * Allocate a new block on the heap using sbrk:
 *
 *  1. Determined aligned amount of memory to allocate.
 *  2. Allocate memory on the heap.
 *  3. Set allocage block properties.
 *
 * @param   size    Number of bytes to allocate.
 * @return  Pointer to data portion of newly allocate block.
 **/
Block *	block_allocate(size_t size) {
    // Allocate block
    intptr_t allocated = sizeof(Block) + ALIGN(size);
    Block *  block     = sbrk(allocated);
    if (block == SBRK_FAILURE) {
    	return NULL;
    }

    // Record block informations
    block->capacity = ALIGN(size);
    block->size     = size;
    block->prev     = block;
    block->next     = block;

    // Update counters
    Counters[HEAP_SIZE] += allocated;
    Counters[BLOCKS]++;
    Counters[GROWS]++;
    return block;
}

/**
 * Attempt to release memory used by block to heap:
 *
 *  1. If the block is at the end of the heap.
 *  2. The block capacity meets the trim threshold.
 *
 * @param   block   Pointer to block to release.
 * @return  Whether or not the release completed successfully.
 **/
bool	block_release(Block *block) {
    // TODO: Implement block release
    size_t allocated = 0;
    // Counters[BLOCKS]--;
    // Counters[SHRINKS]++;
    // Counters[HEAP_SIZE] -= allocated;

    intptr_t endHeap  = (intptr_t)sbrk(0);
    if (endHeap == (intptr_t)SBRK_FAILURE)
        return false;

    intptr_t blockPos = (intptr_t)block->data + block->capacity;
   
    if (blockPos == endHeap && block->capacity >= TRIM_THRESHOLD){
        Block *detach = block_detach(block);
        if (!detach)
            return false;
    
        allocated = block->capacity + sizeof(Block);
       
        Counters[BLOCKS]--;
        Counters[SHRINKS]++;
        Counters[HEAP_SIZE] -= allocated;
        sbrk(allocated * -1);
        return true;
    }
    return false;
}

/**
 * Detach specified block from its neighbors.
 *
 * @param   block   Pointer to block to detach.
 * @return  Pointer to detached block.
 **/
Block * block_detach(Block *block) {
    // TODO: Detach block from neighbors by updating previous and next block
    if (block){
        Block *before = block->prev;
        Block *after  = block->next;

        before->next = after;
        after->prev = before;

        block->prev = block;
        block->next = block;
    }
    return block;
}

/**
 * Attempt to merge source block into destination.
 *
 *  1. Compute end of destination and start of source.
 *
 *  2. If they both match, then merge source into destination by giving the
 *  destination all of the memory allocated to source.
 *
 * @param   dst     Destination block we are merging into.
 * @param   src     Source block we are merging from.
 * @return  Whether or not the merge completed successfully.
 **/
bool	block_merge(Block *dst, Block *src) {
    // TODO: Implement block merge
    // Counters[MERGES]++;
    // Counters[BLOCKS]--;

    if ((intptr_t)src == sizeof(Block) + (intptr_t)dst + dst->capacity){
        dst->capacity += src->capacity + sizeof(Block);
        //dst->size += src->size;
        //dst->next = src->next;

        Counters[MERGES]++;
        Counters[BLOCKS]--;
        return true;
    }

    return false;
}

/**
 * Attempt to split block with the specified size:
 *
 *  1. Check if block capacity is sufficient for requested aligned size and
 *  header Block.
 *
 *  2. Split specified block into two blocks.
 *
 * @param   block   Pointer to block to split into two separate blocks.
 * @param   size    Desired size of the first block after split.
 * @return  Pointer to original block (regardless if it was split or not).
 **/
Block * block_split(Block *block, size_t size) {
    // TODO: Implement block split
    // Counters[SPLITS]++;
    // Counters[BLOCKS]++;

    //size_t aSize = ALIGN(size);
    if (block->capacity > (ALIGN(size) + sizeof(Block))){
        Block *new = (Block *)(block->data + ALIGN(size));
        new->size = block->capacity - sizeof(Block) - ALIGN(size);
        new->capacity = ALIGN(new->size);
        new->next = block->next;
        new->prev = block;
        block->next->prev = new;

        block->capacity = ALIGN(size);
        block->size = size;
        block->next = new;
       
        Counters[SPLITS]++;
        Counters[BLOCKS]++;
    }
    else{
        block->size = size;
    }
    return block;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
