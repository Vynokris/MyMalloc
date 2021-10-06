#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct MetaData MetaData;

typedef struct MetaData {
    void* data;
    bool free;
    MetaData* next;
} MetaData;


MetaData** get_metadata()
{
    static MetaData* metadata = NULL;
    return &metadata;
}


void print_metadata()
{
    // Metadata chained list.
    MetaData* metadata = *get_metadata();

    printf("\nAllocated data:\n");

    // Loop through the elements and print them.
    MetaData* current = metadata;
    for (int i = 0; current != NULL; i++) {
        if (current->next) {
            printf("%d:\n address = %p\n free = %s\n size = %d\n", i, current->data, (current->free ? "true":"false"), current->next->data - current->data - sizeof(MetaData));
        }
        else {
            printf("%d:\n address = %p\n free = %s\n size = %d\n", i, current->data, (current->free ? "true":"false"), sbrk(0) - current->data);
        }
        current = current->next;
    }

    printf("\n");
}


void* my_malloc(int size)
{
    // MetaData chained list.
    MetaData** metadata = get_metadata();

    // If the metadata list is empty, move the break and return.
    if (*metadata == NULL) 
    {
        *metadata = sbrk(sizeof(MetaData));
        **metadata = (MetaData){ sbrk(size), false, NULL };
        printf("Moved break to %p\n", sbrk(0));
        return (*metadata)->data;
    }

    else 
    {
        // Find a free memory block in the heap.
        MetaData* mem_block = *metadata;
        while (mem_block->next != NULL || mem_block->free) {
            if (mem_block->next->data - mem_block->data - sizeof(MetaData) >= size) {
                break;
            }
            mem_block = mem_block->next;
        }

        // If the found memory block is after the break, move the break and return.
        if (mem_block->next == NULL) {
            mem_block->next = sbrk(sizeof(MetaData));
            *mem_block->next = (MetaData){ sbrk(size), false, NULL };
            printf("Moved break to %p\n", sbrk(0));
            return mem_block->next;
        }

        // If the found memory is freed memory...
        else {
            // If the found memory is just the right size, set it to used and return.
            if (mem_block->next->data - mem_block->data - sizeof(MetaData) == size) {
                mem_block->free = false;
                printf("Totally reallocated memory block %p\n", mem_block->data);
                return mem_block->data;
            }

            // If the found memory is too large, set the necessary size to used, leave the remaining free and return.
            else {
                mem_block->free = false;
                MetaData* temp = mem_block->next;
                mem_block->next = &(MetaData){ mem_block->data + size, true, temp };
                printf("Partially reallocated memory block %p\n", mem_block->data);
                return mem_block;
            }
        }
    }
}

void my_free(void* ptr)
{
    // MetaData chained list.
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
            printf("Freed memory block %p\n", ptr);
        }
    }
}

int main()
{
    // Get the position of the break.
    void* brkAdress = sbrk(0);
    printf("Break is currently at %p\n", brkAdress);

    // Allocate an int.
    int* val1 = my_malloc(sizeof(int));
    *val1 = 5;
    printf("%d\n", *val1);

    // Allocate a string.
    char* val2 = my_malloc(sizeof(char) * 15);
    val2 = "Hello, World!\0";
    printf("%s\n", val2);

    print_metadata();

    // Free the int.
    my_free(val1);

    print_metadata();

    // Reallocate the int.
    int* val3 = my_malloc(sizeof(int));
    *val1 = 10;
    printf("%d\n", *val1);

    print_metadata();
    return 0;
}