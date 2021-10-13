#include <stdlib.h>
#include <stdio.h>
 
#include <my_allocator.h>



void test_malloc()
{
    printf("\n-------------------- TESTING MALLOC --------------------\n\n");
    
    // Test malloc.
    char* character = malloc(sizeof(char));
    *character = 'b';
    int* integer = malloc(sizeof(int));
    *integer = 15;
    printf("Allocated a character of value: %c\n", *character);
    printf("Allocated an integer of value: %d\n", *integer);

    showDebugInfo();

    // Free what was allocated so that the list is clean.
    free(character);
    free(integer);
}

void test_calloc()
{
    printf("\n-------------------- TESTING CALLOC --------------------\n\n");

    // Test calloc.
    int* int_array = my_calloc(15, sizeof(int));
    printf("Callocated an int array of length 15 of values:\n{ ");
    for (int i = 0; i < 15; i++) {
        printf("%d, ", int_array[i]);
    }
    printf("\b\b }\n");

    showDebugInfo();

    // Free what was allocated so that the list is clean.
    free(int_array);
}

void test_realloc()
{
    printf("\n-------------------- TESTING REALLOC --------------------\n\n");

    // Allocate memory to test realloc.
    int* integer = malloc(sizeof(int));
    *integer = 15;
    char* string = malloc(sizeof(char) * 30);
    string[0] = 'H'; string[1] = 'o'; string[2] = 'h'; string[3] = 'o'; string[4] = '\0';
    short int* boolean = malloc(sizeof(short int));
    printf("Allocated an int = 15, a string = \"Hoho\", and a boolean.\n");

    showDebugInfo();

    // Test reallocate for a block of the same size.
    int* new_integer = realloc(integer, sizeof(int));
    printf("Reallocated the integer to an integer of same size and of value: %d\n", *new_integer);

    showDebugInfo();

    // Test reallocate for a block of inferior size.
    char* character = realloc(string, sizeof(char));
    printf("Reallocated the string to a char of inferior size and of value: %c\n", *character);

    showDebugInfo();

    // Test reallocate for a block of superior size.
    unsigned long long int* ull_int_array = realloc(new_integer, sizeof(unsigned long long int) * 10);
    printf("Reallocated the new int to a ull int array of higher size and of first value: %lld\n", ull_int_array[0]);

    showDebugInfo();

    // Test reallocate for a block of superior size but with a free block before and after it.
    char* new_string = realloc(character, sizeof(char) * 15);
    printf("Rellocated the character to a new string to test realloc with a free block before and after it.\n");

    showDebugInfo();

    // Test realloc to free a memory block.
    char* temp = realloc(new_string, 0);
    (void)temp;
    printf("Used realloc to free the new string.\n");
    
    showDebugInfo();

    // Free what was allocated so that the list is clean.
    free(boolean);
    free(ull_int_array);
}

void test_free()
{
    printf("\n-------------------- TESTING FREE --------------------\n\n");

    // Allocate memory to test free.
    int* integer = malloc(sizeof(int));
    char* string = malloc(sizeof(char) * 5);
    printf("Allocated an int and a string.\n");

    showDebugInfo();

    // Test free when there are no free blocks around and not at the end of the list.
    free(integer);
    printf("Freed the string to test free when there are no free blocks around and not at the end of the list.\n");

    showDebugInfo();

    // Test free when the memory block is at the end of the list.
    free(string);
    printf("Freed the integer to test free when the block is at the end of the list.\n");

    showDebugInfo();
}

void test_merge()
{
    printf("\n-------------------- TESTING MERGE --------------------\n\n");

    // Allocate memory to test merge.
    int* integer = malloc(sizeof(int));
    char* string = malloc(sizeof(char) * 5);
    short int* boolean = malloc(sizeof(short int));
    unsigned long long int* ull_integer = malloc(sizeof(unsigned long long int));
    printf("Allocated an int, a string, a boolean, a ull int.\n");

    showDebugInfo();

    // Test merge with the previous memory block.
    free(string);
    printf("Freed the string.\n");
    showDebugInfo();
    free(boolean);
    printf("Freed the boolean to merge it with the previous memory block.\n");
    showDebugInfo();

    // Test merge with the next memory block.
    free(integer);
    printf("Freed the int to merge it with the next memory block.\n");
    showDebugInfo();

    // Free what was allocated so that the list is clean.
    free(ull_integer);
}

void test_split()
{
    printf("\n-------------------- TESTING SPLIT --------------------\n\n");

    // Allocate and free memory to test split.
    unsigned long long int* ull_int_array = malloc(sizeof(unsigned long long int) * 10);
    char* character = malloc(sizeof(char));
    free(ull_int_array);
    printf("Allocated a ull int array and a char, then freed the ull int array.\n");

    showDebugInfo();

    // Test split by allocating a new int.
    int* integer = malloc(sizeof(int));
    printf("Allocated an int to test split.\n");

    showDebugInfo();

    // Free what was allocated so that the list is clean.
    free(integer);
    free(character);
}



void test_allocator()
{
    showDebugInfo();

    // Let printf allocate its memory.
    printf("Printf allocated the necessary memory for it to function.\n");

    showDebugInfo();

    test_malloc();
    test_calloc();
    test_realloc();
    test_free();
    test_merge();
    test_split();
}

 
int main()
{
    registerAllocatorHook();

    test_allocator();

    return 0;
}
