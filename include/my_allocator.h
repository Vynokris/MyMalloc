#pragma once
#include <malloc.h>

void registerAllocatorHook(void);
void showDebugInfo(void); // Display information on metadata

/*
#include <stddef.h>

void* my_malloc(size_t size);
void* my_realloc(void* ptr, size_t size);
void* my_calloc(size_t nb, size_t size);
void my_free(void* ptr);
void showDebugInfo(); // Display information on metadata.
*/

