#include "heap.h"
#include "kernel.h"
#include "config.h"
#include "status.h"
#include "memory/memory.h"
#include <stdbool.h>

static int heap_validate_table(void* ptr, void* end, struct heap_table* table)
{
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / LEOS_HEAP_BLOCK_SIZE;
    if(table->total != total_blocks){
        res = -EINVARG;
        goto out;
    }

out:
    return res;
}

static _Bool heap_validate_alignment(void* ptr){
    return ((unsigned int) ptr % LEOS_HEAP_BLOCK_SIZE) == 0;
}

//The starter function wich will create the heap
int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table)
{   
    int res = 0;

    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))   
    {
        res = -EINVARG;
        goto out;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);
    if(res < 0){
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
    return res;
}

//Align our value to the next block
static uint32_t heap_align_value_to_upper(uint32_t value)
{
    if(value % LEOS_HEAP_BLOCK_SIZE == 0){
        return value;
    }

    value = (value - ( value % LEOS_HEAP_BLOCK_SIZE));
    value += LEOS_HEAP_BLOCK_SIZE;
    return value;
}

//This function will return the type of a given block
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f; //This will return the first 4 bits, we don't care about the other bits
}

int heap_get_start_block(struct heap* heap, uint32_t total_blocks)
{
    struct heap_table* table = heap->table;
    int bc = 0; // Store number of blocks free
    int bs = -1; // Store the block start, the first block free

    for(size_t i = 0; i < table->total; i++)
    {   
        //If the entry is taken there is no way for us to use that block so we reset the state
        if(heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            bc = 0;
            bs = -1;
            continue;
        }

        //If this is the first block
        if(bs == -1)
        {
            bs = i; //We set the start block to i
        }
        bc++;

        if(bc == total_blocks){
            break;
        }

    }

    if(bs == -1){
        return -EINOMEM;
    }

    return bs;
}

//This function will mark a number of blocks starting from an entry as full
void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks)
{
    int end_block = (start_block + total_blocks) -1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if(total_blocks > 1){
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for(size_t i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if(i != end_block -1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

//This function return an address based on the number of the block
void *heap_block_to_address(struct heap* heap, int block)
{
    return heap->saddr + (block * LEOS_HEAP_BLOCK_SIZE);
}

//Allocating the blocks of memory required
void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks)
{
    void* address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);

    //Checking for potential errors
    if(address < 0){
        goto out;
    }

    address = heap_block_to_address(heap, start_block);

    //Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);

out:
    return address;
}

//Convert an address to a block
int heap_address_to_block(struct heap* heap, void* address)
{
    return((int)(address - heap->saddr)) / LEOS_HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_free(struct heap* heap, int starting_block)
{
    struct heap_table* table = heap->table;
    for(size_t i = starting_block; i < table->total; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if(!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

//Function for calculating how many blocks we need and pass it to another function
void* heap_malloc(struct heap* heap, size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / LEOS_HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap, total_blocks);
}

//Function for clearing the heap blocks
void heap_free(struct heap* heap, void* ptr)
{
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}
