// #include <my_allocator.h> // For some reson my vscode flags this as an error... so I use the next line instead.
#include "../include/my_allocator.h"
#include "metadata.h"


// Definine console colors for the showDebugInfo function.
#define C_DEFAULT "\x1B[39m"
#define C_GRAY "\x1B[38;5;242m"
#define C_RED "\x1B[38;5;203m"
#define C_GREEN "\x1B[32m"
#define C_LIGHT_BLUE "\x1B[38;5;110m"


void showDebugInfo()
{
    // Metadata chained list.
    MetaData* metadata = *get_metadata();

    if (is_end_of_list(metadata)) {
        printf("\n%sNo allocated data.\n", C_LIGHT_BLUE);
    }
    else {
        printf("\n%sAllocated data: ", C_LIGHT_BLUE);
        printf("(%smetadata%s, %sused memory%s, %sfree memory%s)\n", C_GRAY, C_LIGHT_BLUE, C_RED, C_LIGHT_BLUE, C_GREEN, C_LIGHT_BLUE);

        // Loop through the elements and print them.
        MetaData* current = metadata;
        for (int i = 0; !is_end_of_list(current); i++) 
        {
            // Print the metadata info in gray (no need for the size as it is always 16).
            printf("%s|-----------------------|\n", C_LIGHT_BLUE);
            printf("%s", C_GRAY);
            printf(" address: %p\n", current);

            // Printf the data info in green if the data is free, and in red if it is used.
            printf("%s|-----------------------|\n", C_LIGHT_BLUE);
            if (current->free)
                printf("%s", C_GREEN);
            else 
                printf("%s", C_RED);
            printf(" address: %p\n", get_data(current));
            printf(" size: %ld\n", get_data_size(current));
            
            // Move to the next metadata.
            current = current->next;
        }
        printf("%s|-----------------------|\n", C_LIGHT_BLUE);
    }

    printf("Break address: %p%s\n\n", sbrk(0), C_DEFAULT);
}


static MetaData** get_metadata()
{
    // The metadata pointer is static so that it is not a global variable, but its value is still stored and modifiable.
    static MetaData* metadata = NULL;
    return &metadata;
}


static bool is_end_of_list(MetaData* metadata_block)
{
    return (metadata_block == NULL || (void*)metadata_block >= sbrk(0));
}


static void* get_data(MetaData* metadata_block)
{
    return (void*)metadata_block + sizeof(MetaData);
}


static size_t get_data_size(MetaData* metadata_block)
{
    if (!is_end_of_list(metadata_block->next))
        return (void*)metadata_block->next - get_data(metadata_block);
    else
        return (size_t)(sbrk(0) - (size_t)get_data(metadata_block));
}


static size_t get_aligned_size(size_t size)
{
    // Function found on https://www.geeksforgeeks.org/smallest-number-greater-than-or-equal-to-n-divisible-by-k/
    size_t remainer = (size + 8) % 8;
    if (remainer == 0)
        return size;
    else
        return size + 8 - remainer;
}


static void* alloc_metadata_and_data(MetaData** metadata_block, size_t size)
{
    // Make sure the break is at an address multiple of 8.
    while ((size_t)sbrk(0) % 8 != 0)
        sbrk(1);

    // Allocate room for the metadata and set its values.
    *metadata_block = sbrk(sizeof(MetaData));
    **metadata_block = (MetaData){ false, NULL };

    // Allocate room for the data. It will always be at an address multiple of 8 since sizeof(MetaData) = 16.
    sbrk(get_aligned_size(size));

    return get_data(*metadata_block);
}


static void* split_mem_block(MetaData* mem_block, size_t size)
{
    // Reallocate the memory block.
    mem_block->free = false;

    if (get_data_size(mem_block) >= get_aligned_size(size) + sizeof(MetaData) + 8) { // This checks if there is enough room to split.
        // Save the address to the next metadata.
        MetaData* next_metadata = mem_block->next;

        // Set the metadata values for the new memory block.
        mem_block->next = get_data(mem_block) + get_aligned_size(size);
        *(mem_block->next) = (MetaData){ true, next_metadata };
    }

    return get_data(mem_block);
}


static void* merge_mem_block(MetaData* mem_block_before)
{
    if (!is_end_of_list(mem_block_before->next)) // This handles the case where there is only 1 node in the metadata list.
    {
        MetaData* mem_block = mem_block_before->next;
        mem_block->free = true;

        // If the memory block before the freed one is free, merge the two.
        if (mem_block_before->free) {
            mem_block_before->next = mem_block->next;
            mem_block = mem_block_before;
        }

        // If the memory block after the freed one is free, merge the two.
        if (!is_end_of_list(mem_block->next) && mem_block->next->free) {
            mem_block->next = mem_block->next->next;
        }

        return mem_block;
    }

    return mem_block_before;
}


static void copy_data_to_ptr(MetaData* dest, MetaData* src, size_t size)
{
    // Loop over the data of src and copy it to dest.
    if (src != dest) {
        for (size_t i = 0; i < get_data_size(src) && i < size; i++) {
            *((char*)get_data(dest) + i) = *((char*)get_data(src) + i);
        }
    }
}


static void my_free(void* ptr)
{
    // Meta-data of all allocated memory.
    MetaData** metadata = get_metadata();

    // If the metadata list is not empty...
    if (!is_end_of_list(*metadata)) 
    {
        // Find the memory block before the one to be to be freed.
        MetaData* mem_block = *metadata;
        MetaData* mem_block_before = *metadata;
        while (!is_end_of_list(mem_block) && get_data(mem_block) != ptr) {
            mem_block_before = mem_block;
            mem_block = mem_block->next;
        }

        // If the memory block to be freed was found, free it.
        if (!is_end_of_list(mem_block)) 
        {
            // Try to merge the freed memory block with the one before it and the one after it.
            mem_block = merge_mem_block(mem_block_before);

            // If the freed memory block is the last block of the list, move the break back.
            if (is_end_of_list(mem_block->next)) {
                brk(mem_block);
            }
        }
    }
}


static void* my_malloc(size_t size)
{
    // MetaData chained list.
    MetaData** metadata = get_metadata();

    // If the metadata list is empty, allocate and return.
    if (is_end_of_list(*metadata)) {
        return alloc_metadata_and_data(metadata, size);
    }

    // If the metadata list isn't empty find an adress to allocate memory.
    else 
    {
        // Try to find a free memory block in the heap that is large enough to hold the data.
        MetaData* mem_block = *metadata;
        while (!(mem_block->free && get_data_size(mem_block) >= get_aligned_size(size)) && !is_end_of_list(mem_block->next)) {
            mem_block = mem_block->next;
        }

        // If the found memory block is after the break, allocate and return.
        if (is_end_of_list(mem_block->next)) {
            return alloc_metadata_and_data(&mem_block->next, size);
        }

        // If the found memory is freed memory...
        else {
            // Try to split the memory block.
            mem_block = split_mem_block(mem_block, size) - sizeof(MetaData);

            return get_data(mem_block);
        }
    }
}


static void* my_realloc(void* ptr, size_t size)
{
    if (size > 0)
    {
        MetaData* metadata = *get_metadata();

        // Find the pointer in the metadata list, as well as the pointer before it.
        MetaData* mem_block_before = metadata;
        MetaData* mem_block = metadata;
        while (!is_end_of_list(mem_block) && get_data(mem_block) != ptr) {
            mem_block_before = mem_block;
            mem_block = mem_block->next;
        }

        // If the pointer was found.
        if (!is_end_of_list(mem_block)) 
        {
            // Merge the memory block with its neighbors and copy the data to it.
            MetaData* merged_block = merge_mem_block(mem_block_before);
            copy_data_to_ptr(merged_block, mem_block, size);

            // Allocate a new memory block of the right size and copy the data to it.
            MetaData* new_mem_block = my_malloc(size) - sizeof(MetaData);
            copy_data_to_ptr(new_mem_block, merged_block, size);

            return get_data(new_mem_block);
        }

        return NULL;
    }

    // Free the pointer if the given size is 0 or negative.
    else {
        my_free(ptr);
        return NULL;
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


void* malloc_hook(size_t size, const void* caller)            { return my_malloc(size); }
void* realloc_hook(void* ptr, size_t size, const void* caller){ return my_realloc(ptr, size); }
void  free_hook(void* ptr, const void* caller)                { return my_free(ptr); }

void registerAllocatorHook(void)
{
    __malloc_hook = malloc_hook;
    __realloc_hook = realloc_hook;
    __free_hook = free_hook;
}