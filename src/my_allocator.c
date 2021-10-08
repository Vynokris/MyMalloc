#include <my_allocator.h>
#include "metadata.h"


// Definine console colors for the showDebugInfo function.
#define C_DEFAULT "\x1B[39m"
#define C_GRAY "\x1B[38;5;239m"
#define C_RED "\x1B[38;5;203m"
#define C_GREEN "\x1B[32m"
#define C_LIGHT_BLUE "\x1B[38;5;110m"


void showDebugInfo()
{
    // Metadata chained list.
    MetaData* metadata = *get_metadata();

    if (metadata == NULL) {
        printf("\n%sNo allocated data.%s\n", C_LIGHT_BLUE, C_DEFAULT);
    }
    else {
        printf("\n%sAllocated data:%s\n", C_LIGHT_BLUE, C_DEFAULT);

        // Loop through the elements and print them.
        MetaData* current = metadata;
        for (int i = 0; current != NULL; i++) 
        {
            // Print the metadata info in gray.
            printf("%s|-----------------------|%s\n", C_LIGHT_BLUE, C_DEFAULT);
            printf("%s", C_GRAY);
            printf(" metadata #%d\n", i);
            printf(" address: %p\n", current);
            printf(" size: %ld\n", get_metadata_size(current));
            printf("%s", C_DEFAULT);

            // Printf the data info in green if the data is free, and in red if it is used.
            printf("%s|-----------------------|%s\n", C_LIGHT_BLUE, C_DEFAULT);
            if (current->free)
                printf("%s", C_GREEN);
            else 
                printf("%s", C_RED);
            printf(" data #%d\n", i);
            printf(" address: %p\n", get_data(current));
            printf(" size: %ld\n", get_data_size(current));
            printf(" free: %s\n", (current->free ? "true":"false"));
            printf("%s", C_DEFAULT);
            
            // Move to the next metadata.
            current = current->next;
        }
        printf("%s|-----------------------|%s\n", C_LIGHT_BLUE, C_DEFAULT);
    }

    printf("%sBreak address: %p%s\n\n", C_LIGHT_BLUE, sbrk(0), C_DEFAULT);
}


static MetaData** get_metadata()
{
    static MetaData* metadata = NULL;
    return &metadata;
}


static void* get_data(MetaData* metadata_block)
{
    return (void*)metadata_block + sizeof(MetaData);
}


static size_t get_data_size(MetaData* metadata_block)
{
    if (metadata_block->next != NULL) {
        return (void*)metadata_block->next - get_data(metadata_block);
    }
    else {
        return (int)(sbrk(0) - (int)get_data(metadata_block));
    }
}


static size_t get_metadata_size(MetaData* metadata_block)
{
    return get_data(metadata_block) - (void*)metadata_block;
}


static size_t get_aligned_size(size)
{
    // Function found on https://www.geeksforgeeks.org/smallest-number-greater-than-or-equal-to-n-divisible-by-k/

    size_t remainer = (size + 8) % 8;

    if (remainer == 0) {
        return size;
    }
    else {
        return size + 8 - remainer;
    }
}


static void* alloc_metadata_and_data(MetaData** metadata_block, size_t size)
{
    // Make sure the break is at an address multiple of 8.
    while ((int)sbrk(0) % 8 != 0) {
        sbrk(1);
    }

    // Allocate room for the metadata and set its values.
    *metadata_block = sbrk(sizeof(MetaData));
    **metadata_block = (MetaData){ false, NULL };

    // Allocate room for the data, which size the closest nomber greater than the size argument that is divisible by 8. 
    // It will always be at an address multiple of 8 since sizeof(MetaData) = 16.
    sbrk(get_aligned_size(size));

    return get_data(*metadata_block);
}


static void* split_mem_block(MetaData* mem_block, size_t size)
{
    // Get the values for the meta-data of the new memory block.
    void* new_md_address = get_data(mem_block) + get_aligned_size(size);
    MetaData* next_metadata = mem_block->next;

    // Reallocate the found memory.
    mem_block->free = false;

    // Set the meta-data values for the new memory block.
    mem_block->next = new_md_address;
    *(mem_block->next) = (MetaData){ true, next_metadata };

    return get_data(mem_block);
}


void* my_malloc(size_t size)
{
    if (size == 14) {
        printf("A");
    }
    // MetaData chained list.
    MetaData** metadata = get_metadata();

    // If the metadata list is empty, allocate and return.
    if (*metadata == NULL) {
        return alloc_metadata_and_data(metadata, size);
    }

    else 
    {
        // Find a free memory block in the heap that is large enough to hold the data, or allocate a new memory block after the break.
        MetaData* mem_block = *metadata;
        while (!(mem_block->free && get_data_size(mem_block) >= (int)sizeof(MetaData) + get_aligned_size(size)) && mem_block->next != NULL) {
            mem_block = mem_block->next;
        }

        // If the found memory block is after the break, allocate and return.
        if (get_data(mem_block) + get_data_size(mem_block) >= sbrk(0)) {
            return alloc_metadata_and_data(&mem_block->next, size);
        }

        // If the found memory is freed memory...
        else {
            // If the found memory is just the right size, set it to used and return.
            if (get_data_size(mem_block) == size) {
                mem_block->free = false;
                return get_data(mem_block);
            }

            // If the found memory is too large, split it and return.
            else {
                return split_mem_block(mem_block, size);
            }
        }
    }
}


void* my_realloc(void* ptr, size_t size)
{
    // If the size is 0, just free the memory block.
    if (size == 0) {
        my_free(ptr);
    }

    // Else, reallocate it.
    else
    {
        // Metadata of all allocated memory.
        MetaData* metadata = *get_metadata();

        // If the metadata list isn't empty...
        if (metadata != NULL)
        {
            // Find the memory block to be reallocated.
            MetaData* mem_block = metadata;
            while (mem_block != NULL && get_data(mem_block) != ptr) {
                mem_block = mem_block->next;
            }

            // If the memory block to be reallocated was found.
            if (mem_block != NULL) 
            {
                size_t temp1 = get_data_size(mem_block);
                size_t temp2 = sizeof(MetaData) + get_aligned_size(size);
                // If the block is the right size, return.
                if (get_data_size(mem_block) == get_aligned_size(size)) {
                    return ptr;
                }

                // If the block is too big and can be split, split it and return.
                else if (get_data_size(mem_block) < sizeof(MetaData) + get_aligned_size(size)) {
                    return split_mem_block(mem_block, size);
                }

                else {
                    // If the block is too small and the next block is free, check if merging the two makes enough room for the new size.
                    if (mem_block->next != NULL && mem_block->next->free && get_data_size(mem_block) + get_metadata_size(mem_block->next) + get_data_size(mem_block->next) >= get_aligned_size(size)) {
                        mem_block->next = mem_block->next->next;

                        // If the new larger block is too big and can be split, split it.
                        if (get_data_size(mem_block) <  sizeof(MetaData) + get_aligned_size(size)) {
                            split_mem_block(mem_block, size);
                        }

                        return mem_block;
                    }

                    // If merging the two memory blocks isn't enough, call free on it, then malloc a new memory block for it.
                    else {
                        MetaData* new_mem_block = my_malloc(size) - sizeof(MetaData);
                        for (size_t i = 0; i < get_data_size(mem_block) && i < size; i++) {
                            *((char*)get_data(new_mem_block) + i) = *((char*)get_data(mem_block) + i);
                        }
                        my_free(get_data(mem_block));
                        return get_data(new_mem_block);
                    }
                }
            }
        }
    }
}


void* my_calloc(size_t nb, size_t size)
{
    // Allocate room for the pointer.
    void* ptr = my_malloc(nb * size);

    // Set all the pointer's values to 0.
    for (size_t i = 0; i < nb * size; i++) {
        *((char*)ptr + i) = 0;
    }

    return ptr;
}


void my_free(void* ptr)
{
    // Meta-data of all allocated memory.
    MetaData** metadata = get_metadata();

    // If the metadata list is not empty...
    if (*metadata != NULL) 
    {
        // Find the memory block to be freed.
        MetaData* mem_block = *metadata;
        while (mem_block != NULL && get_data(mem_block) != ptr) {
            mem_block = mem_block->next;
        }

        // If the memory block to be freed was found, free it.
        if (mem_block != NULL) {
            mem_block->free = true;
        }


        MetaData* cur_mem_block = *metadata;
        MetaData* last_mem_block = *metadata;

        // Check for consecutive free memory blocks and merge them.
        while (cur_mem_block->next != NULL) 
        {
            cur_mem_block = cur_mem_block->next;

            // If the current and the precedent memory block are free, merge them
            if (cur_mem_block->free && last_mem_block->free) {
                last_mem_block->next = cur_mem_block->next;
                cur_mem_block = last_mem_block;
            }

            else {
                last_mem_block = cur_mem_block;
            }
        }

        // If there is only 1 memory block in the list and it is free, move the break back and empty the metadata array.
        if ((*metadata)->next == NULL) {
            brk(cur_mem_block);
            *metadata = NULL;
        }

        // Else check if the last memory block is free and move the break back if so.
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
}



/*
void* malloc_hook(size_t size, const void* caller)            { return my_malloc(size); }
void* realloc_hook(void* ptr, size_t size, const void* caller){ return my_realloc(ptr, size); }
void  free_hook(void* ptr, const void* caller)                { return my_free(ptr); }
*/

void registerAllocatorHook(void)
{
    __malloc_hook = my_malloc;
    __realloc_hook = my_realloc;
    __free_hook = my_free;
}