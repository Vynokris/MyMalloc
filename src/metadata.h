#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct MetaData MetaData;

typedef struct MetaData {
    void* data;
    bool free;
    size_t size;
    MetaData* next;
} MetaData;


// Returns the meta-data of all allocated memory.
static MetaData** get_metadata();

// Prints all the metadata.
void print_metadata();