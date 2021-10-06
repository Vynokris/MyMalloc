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

    printf("\nAllocated data:\n");

    // Loop through the elements and print them.
    MetaData* current = metadata;
    for (int i = 0; current != NULL; i++) {
        printf("%d:\n free = %s\n size = %d\n", i, (current->free ? "true":"false"), current->size);
        current = current->next;
    }

    printf("\n");
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
        while (!(mem_block->free && mem_block->size >= size) && mem_block->next != NULL) {
            mem_block = mem_block->next;
        }

        // If the found memory block is after the break, move the break and return.
        if (!mem_block->free) {
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
                void* new_adress = mem_block->data + size;
                size_t new_size = mem_block->size - size;
                MetaData* next_metadata = mem_block->next;

                // Reallocate the found memory.
                mem_block->free = false;
                mem_block->size = size;

                // Set the meta-data values for the new memory block.
                mem_block->next = sbrk(sizeof(MetaData));
                *(mem_block->next) = (MetaData){ new_adress, new_size, true, next_metadata };
                mem_block->next->size = new_size; // For some reason this is necessary: even though the new size is already set on the previous line, it isn't set to the right value.

                return mem_block->data;
            }
        }
    }
}


void* my_realloc(void* ptr, size_t size)
{
    // Meta-data of all allocated memory.
    MetaData* metadata = *get_metadata();

    // If the metadata list is empty, return NULL.
    if (metadata != NULL) 
    {
        // Find the memory block to be reallocated.
        MetaData* mem_block = metadata;
        while (mem_block && mem_block->data != ptr) {
            mem_block = mem_block->next;
        }

        // If the memory block to be reallocated was found...
        if (mem_block != NULL) 
        {
            // If the size is 0, free the block and return.
            if (size == 0) {
                mem_block->free = true;
                return NULL;
            }
            // If the memory block to be reallocated is too small for the new value, free it and allocate a new memory block.
            else if (size > mem_block->size) {
                my_free(mem_block->data);
                my_alloc(size);
            }
            // If the memory block to be reallocated the right size, allocate it and return.
            else if (size == mem_block->size) {
                mem_block->free = false;
                return mem_block->data;
            }
            // If the memory block to be reallocated is too large, set the necessary size to used, leave the remaining free and return.
            else {
                // Get the values for the meta-data of the new memory block.
                void* new_adress = mem_block->data + size;
                size_t new_size = mem_block->size - size;
                MetaData* next_metadata = mem_block->next;

                // Reallocate the found memory.
                mem_block->free = false;
                mem_block->size = size;

                // Set the meta-data values for the new memory block.
                mem_block->next = sbrk(sizeof(MetaData));
                *(mem_block->next) = (MetaData){ new_adress, new_size, true, next_metadata };
                mem_block->next->size = new_size; // For some reason this is necessary: even though the new size is already set on the previous line, it isn't set to the right value.

                return mem_block->data;
            }
        }
    }
}


void* my_calloc(size_t nb, size_t size)
{
    void* ptr = my_alloc(nb * size);

    for (int i = 0; i < size; i++) {
        // ptr[i] = 0; // I guess some kind of type cast should be used here but idk which.
    }
}


void my_free(void* ptr)
{
    // Meta-data of all allocated memory.
    MetaData* metadata = *get_metadata();

    // If the metadata list is empty, return NULL.
    if (metadata != NULL) 
    {
        // Find the memory block to be freed.
        MetaData* mem_block = metadata;
        while (mem_block && mem_block->data != ptr) {
            mem_block = mem_block->next;
        }

        // If the memory block to be freed wasn't found, return NULL.
        if (mem_block != NULL) {
            mem_block->free = true;
        }
    }
}