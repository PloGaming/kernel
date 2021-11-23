#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

//Heap initializer
void kheap_init()
{
    int total_table_entries = LEOS_HEAP_SIZE_BYTES / LEOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) (LEOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void* end = (void *)(LEOS_HEAP_ADDRESS + LEOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void *)(LEOS_HEAP_ADDRESS), end, &kernel_heap_table);

    if(res < 0){
        print("Failed to create heap \n");
    }

}
