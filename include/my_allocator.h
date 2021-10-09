#pragma once
#include <malloc.h>

void registerAllocatorHook(void);
void showDebugInfo(void); // Display information on metadata
void* my_calloc(size_t nb, size_t size); // I have to keep this function here since there is no calloc hook.
