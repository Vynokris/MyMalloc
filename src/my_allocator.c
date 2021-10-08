#include <my_allocator.h>
#include "metadata.h"


// Definine console colors for the showDebugInfo function.
#define C_DEFAULT "\x1B[39m"
#define C_GRAY "\x1B[38;5;239m"
#define C_RED "\x1B[38;5;203m"
#define C_GREEN "\x1B[32m"
#define C_LIGHT_BLUE "\x1B[38;5;110m"


static MetaData** get_metadata()
{
    static MetaData* metadata = NULL;
    return &metadata;
}


static void* get_data(MetaData* metadata_block)
{
    return (void*)metadata_block + sizeof(MetaData);
}


static int get_data_size(MetaData* metadata_block)
{
    if (metadata_block->next != NULL) {
        return (void*)metadata_block->next - get_data(metadata_block);
    }
    else {
        return sbrk(0) - (int)get_data(metadata_block);
    }
}


static int get_metadata_size(MetaData* metadata_block)
{
    return get_data(metadata_block) - (void*)metadata_block;
}


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
            printf("%s|----------------------|%s\n", C_LIGHT_BLUE, C_DEFAULT);
            printf("%s", C_GRAY);
            printf(" metadata #%d\n", i);
            printf(" adress: %p\n", current);
            printf(" size: %d\n", get_metadata_size(current));
            printf(" %s\n", ((int)current % 8 == 0 ? "adress divisible by 8" : "ADRESS NOT DIVISIBLE BY 8"));
            printf("%s", C_DEFAULT);

            // Printf the data info in green if the data is free, and in red if it is used.
            printf("%s|----------------------|%s\n", C_LIGHT_BLUE, C_DEFAULT);
            if (current->free)
                printf("%s", C_GREEN);
            else 
                printf("%s", C_RED);
            printf(" data #%d\n", i);
            printf(" adress: %p\n", get_data(current));
            printf(" size: %d\n", get_data_size(current));
            printf(" free: %s\n", (current->free ? "true":"false"));
            printf(" %s\n", ((int)get_data(current) % 8 == 0 ? "adress divisible by 8" : "ADRESS NOT DIVISIBLE BY 8"));
            printf("%s", C_DEFAULT);
            
            // Move to the next metadata.
            current = current->next;
        }
        printf("%s|----------------------|%s\n", C_LIGHT_BLUE, C_DEFAULT);
    }

    printf("%sBreak adress: %p%s\n\n", C_LIGHT_BLUE, sbrk(0), C_DEFAULT);
}


void* my_alloc(size_t size)
{
    // MetaData chained list.
    MetaData** metadata = get_metadata();

    // If the metadata list is empty, move the break and return.
    if (*metadata == NULL) 
    {
        // Allocate space for the metadata.
        *metadata = sbrk(sizeof(MetaData));

        // Make sure the metadata is at an adress multiple of 8
        while ((int)(*metadata) % 8 != 0) {
            *metadata = sbrk(1);
        }

        // The stored data will also be at an adress multiple of 8 since sizeof(Metadata) = 16.
        sbrk(size);
        **metadata = (MetaData){ false, NULL };

        // Make sure the break is at an adress multiple of 8.
        while ((int)sbrk(0) % 8 != 0) {
            sbrk(1);
        }

        return get_data(*metadata);
    }

    else 
    {
        // Find a free memory block in the heap that is large enough to hold the data, or use a new memory block after the break..
        MetaData* mem_block = *metadata;
        while (mem_block->next != NULL && !(mem_block->free && get_data_size(mem_block) >= sizeof(MetaData) + size + size % 8)) {
            mem_block = mem_block->next;
        }

        // If the found memory block is after the break, move the break and return.
        if (get_data(mem_block) + get_data_size(mem_block) >= sbrk(0)) 
        {
            // Allocate room for the metadata.
            mem_block->next = sbrk(sizeof(MetaData));

            // Make sure the metadata is at an adress multiple of 8.
            while ((int)(mem_block->next) % 8 != 0) {
                mem_block->next = sbrk(1);
            }

            // The stored data will also be at an adress multiple of 8 since sizeof(Metadata) = 16.
            sbrk(size);
            *(mem_block->next) = (MetaData){ false, NULL };

            // Make sure the break is at an adress multiple of 8.
            while ((int)sbrk(0) % 8 != 0) {
                sbrk(1);
            }

            return get_data(mem_block->next);
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
                // Get the values for the meta-data of the new memory block.
                void* new_md_adress = get_data(mem_block) + size + size % 8;
                MetaData* next_metadata = mem_block->next;

                // Reallocate the found memory.
                mem_block->free = false;

                // Set the meta-data values for the new memory block.
                mem_block->next = new_md_adress;
                *(mem_block->next) = (MetaData){ true, next_metadata };

                return get_data(mem_block);
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

    // If the metadata list is not empty...
    if (*metadata != NULL) 
    {
        // Find the memory block to be freed.
        MetaData* mem_block = *metadata;
        while (mem_block && get_data(mem_block) != ptr) {
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

        // If the only memory block is free, move the break back and empty the metadata array.
        if ((*metadata)->next == NULL) {
            brk(cur_mem_block);
            *metadata = NULL;
        }

        // Check if the last memory block is free and move the break back if so.
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