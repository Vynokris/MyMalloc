#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct MetaData MetaData;

typedef struct MetaData {
    bool free;
    MetaData* next;
} MetaData;


// Returns the meta-data of all allocated memory.
static MetaData** get_metadata();

// Returns the data of a metadata block.
static void* get_data(MetaData* metadata_block);

// Returns the size of the data storded after a metadata block.
static size_t get_data_size(MetaData* metadata_block);

// Returns the size of a metadata block.
static size_t get_metadata_size(MetaData* metadata_block);

// Allocates room for a metadata and a data at the given address, and make sure they and the brake are at an address multiple of 8.
static void* alloc_metadata_and_data(MetaData** metadata_block, size_t size);