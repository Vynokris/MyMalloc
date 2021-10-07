#include <my_allocator.h>
#include "metadata.h"


// TODO: Figure out how to get calloc to work.
// TODO: Merge two free memory blocks if they are consecutive.


static MetaData** get_metadata()
{
    static MetaData* metadata = NULL;
    return &metadata;
}


void showDebugInfo()
{
    // Metadata chained list.
    MetaData* metadata = *get_metadata();

    if (metadata == NULL) {
        printf("\nNo data\n");
    }
    else {
        printf("\nAllocated data:\n");

        // Loop through the elements and print them.
        MetaData* current = metadata;
        for (int i = 0; current != NULL; i++) {
            if (current->next != NULL)
                printf("%d:\n metadata adress = %p\n metadata size = %d\n data adress = %p\n data size = %d\n free = %s\n", i, current, current->data - (void*)current, current->data, (void*)current->next - current->data, (current->free ? "true":"false"));
            else
                printf("%d:\n metadata adress = %p\n metadata size = %d\n data adress = %p\n data size = %d\n free = %s\n", i, current, current->data - (void*)current, current->data, sbrk(0) - (int)current->data, (current->free ? "true":"false"));

            current = current->next;
        }
    }

    printf("Break adress = %p\n\n", sbrk(0));
}


void* my_alloc(size_t size)
{
    // MetaData chained list.
    MetaData** metadata = get_metadata();

    // If the metadata list is empty, move the break and return.
    if (*metadata == NULL) 
    {
        *metadata = sbrk(sizeof(MetaData));
        **metadata = (MetaData){ sbrk(size), false, size, NULL };
        return (*metadata)->data;
    }

    else 
    {
        // Find a free memory block in the heap.
        MetaData* mem_block = *metadata;
        while (!(mem_block->free && mem_block->size >= sizeof(MetaData) + size) && mem_block->next != NULL) {
            mem_block = mem_block->next;
        }

        // If the found memory block is after the break, move the break and return.
        if (mem_block->next == NULL) {
            mem_block->next = sbrk(sizeof(MetaData));
            *(mem_block->next) = (MetaData){ sbrk(size), false, size, NULL };
            return mem_block->next->data;
        }

        // If the found memory is freed memory...
        else {
            // If the found memory is just the right size, set it to used and return.
            if (mem_block->size == size) {
                mem_block->free = false;
                return mem_block->data;
            }

            // If the found memory is too large, set the necessary size to used, leave the remaining free and return.
            else {
                // Get the values for the meta-data of the new memory block.
                void* new_md_adress = mem_block->data + size;
                size_t new_size = mem_block->size - sizeof(MetaData) - size;
                MetaData* next_metadata = mem_block->next;

                // Reallocate the found memory.
                mem_block->free = false;
                mem_block->size = size;

                // Set the meta-data values for the new memory block.
                mem_block->next = new_md_adress;
                *(mem_block->next) = (MetaData){ new_md_adress + sizeof(MetaData), new_size, true, next_metadata };
                mem_block->next->size = new_size; // For some reason this is necessary: even though the new size is already set on the previous line, it isn't set to the right value.

                return mem_block->data;
            }
        }
    }
}


void* my_realloc(void* ptr, size_t size)
{
    // Free the given pointer.
    my_free(ptr);

    // Allocate space of the given size.
    if (size > 0) {
        my_alloc(size);
    }
}


void* my_calloc(size_t nb, size_t size)
{
    // Allocate room for the pointer.
    void* ptr = my_alloc(nb * size);

    // Set all the pointer's values to 0.
    for (int i = 0; i < nb * size; i++) {
        *((char*)ptr + i) = 0;
    }

    return ptr;
}


void my_free(void* ptr)
{
    // Meta-data of all allocated memory.
    MetaData** metadata = get_metadata();

    // If the metadata list is empty, return NULL.
    if (*metadata != NULL) 
    {
        // Find the memory block to be freed.
        MetaData* mem_block = *metadata;
        while (mem_block && mem_block->data != ptr) {
            mem_block = mem_block->next;
        }

        // If the memory block to be freed was found, free it.
        if (mem_block != NULL) {
            mem_block->free = true;
        }
    }

    // Check for consecutive free memory blocks and merge them.
    MetaData* cur_mem_block = *metadata;
    MetaData* last_mem_block = *metadata;

    while (cur_mem_block->next != NULL) {
        cur_mem_block = cur_mem_block->next;

        // If the current and the precedent memory block are free...
        if (cur_mem_block->free && last_mem_block->free) {
            last_mem_block->size += sizeof(MetaData) + cur_mem_block->size;
            last_mem_block->next = cur_mem_block->next;
            cur_mem_block = last_mem_block;
        }
        else {
            last_mem_block = cur_mem_block;
        }
    }

    // Check if the last memory block is free and move the break back if so.
    if ((*metadata)->next == NULL) {
        brk(cur_mem_block);
        *metadata = NULL;
    }
    else {
        cur_mem_block = *metadata;
        while (cur_mem_block->next->next != NULL) {
            cur_mem_block = cur_mem_block->next;
        }
        if (cur_mem_block->next->free) {
            brk(cur_mem_block->next);
            cur_mem_block->next = NULL;
        }
    }
}