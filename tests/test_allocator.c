#include <stdlib.h>
#include <stdio.h>
#include <my_allocator.h>
 
#define USE_MY_ALLOCATOR
 
#ifdef USE_MY_ALLOCATOR
#define ALLOC my_alloc
#define REALLOC my_realloc
#define CALLOC my_calloc
#define FREE my_free
#else
#define ALLOC malloc
#define REALLOC realloc
#define CALLOC calloc
#define FREE free
#endif
 
void test1()
{
   // MALLOC
   int* a = ALLOC(sizeof(int));
   *a = 10;
   printf("(malloc) a = %d\n", *a);
 
   // REALLOC
   a = REALLOC(a, 2 * sizeof(int));
   a[1] = 20;
   printf("(realloc) a[1] = %d\n", a[1]);

   // FREE
   FREE(a); 
 
   // CALLOC
   a = CALLOC(1, sizeof(int));
   printf("(calloc) a = %d\n", *a);
   FREE(a);
}

void my_test()
{
    printf("\n");

    showDebugInfo();

    // Allocate a string.
    char* val1 = my_alloc(sizeof(char) * 13);
    val1[0] = 'H';
    val1[1] = 'e';
    val1[2] = 'l';
    val1[3] = 'l';
    val1[4] = 'o';
    val1[5] = ',';
    val1[6] = ' ';
    val1[7] = 'W';
    val1[8] = 'o';
    val1[9] = 'r';
    val1[10] = 'l';
    val1[11] = 'd';
    val1[12] = '!';
    val1[13] = '\0';
    printf("Allocated char* val1 of value: %s.\n", val1);

    showDebugInfo();

    // Callocate an int array.
    int* val8 = my_calloc(50, sizeof(int));
    printf("Callocated int array val8 to values: %d, %d, %d.\n", val8[0], val8[1], val8[2]);

    showDebugInfo();

    // Allocate an int.
    int* val2 = my_alloc(sizeof(int));
    *val2 = 5;
    printf("Allocated int val2 of value: %d.\n", *val2);

    showDebugInfo();

    // Free val8.
    my_free(val8);
    printf("Freed int array val8 of values: 0, 0, 0.\n");

    showDebugInfo();

    // Free val2.
    my_free(val2);
    printf("Freed int val2 of value: 5.\n");

    showDebugInfo();

    // Reallocate val3.
    int* val3 = my_alloc(sizeof(int));
    *val3 = 10;
    printf("Allocated int val3 of value: %d.\n", *val3);

    showDebugInfo();

    // Free val1.
    my_free(val1);
    printf("Freed char val1 of value: Hello, World!.\n");

    showDebugInfo();

    // Allocate another int.
    int* val4 = my_alloc(sizeof(int));
    *val4 = 15;
    printf("Allocated int val4 of value: %d.\n", *val4);

    showDebugInfo();

    // Reallocate the val3.
    int* val5 = my_realloc(val3, sizeof(int));
    *val5 = 20;
    printf("Reallocated int val3 to value: %d.\n", *val5);

    showDebugInfo();

    // Reallocate val4.
    char* val6 = my_realloc(val4, sizeof(char));
    *val6 = 'a';
    printf("Reallocated int val4 to char value: %c.\n", *val6);

    showDebugInfo();

    // Reallocate val5.
    long long int* val7 = my_realloc(val5, sizeof(long long int));
    *val7 = 1;
    printf("Reallocated int val5 to value: %d.\n", *val7);

    showDebugInfo();

    // Free val7 using realloc.
    my_realloc(val7, 0);
    printf("Using realloc, freed long long int val7 of value: 1.\n");

    showDebugInfo();

    // Free val 6.
    my_free(val6);
    printf("Freed char val6 of value: a.\n");

    showDebugInfo();
}

 
int main()
{
    #ifdef USE_MY_ALLOCATOR
    my_test();
    #else
    test1();
    #endif

    return 0;
}
