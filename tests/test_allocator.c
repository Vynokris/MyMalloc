#include <stdlib.h>
#include <stdio.h>
 
#include <my_allocator.h>

 
void test1()
{
   // MALLOC
   int* a = malloc(sizeof(int));
   *a = 10;
   printf("(malloc) a = %d\n", *a);
 
   // REALLOC
   a = realloc(a, 2 * sizeof(int));
   a[1] = 20;
   printf("(realloc) a[1] = %d\n", a[1]);

   // FREE
   free(a); 
 
   // CALLOC
   a = my_calloc(1, sizeof(int));
   printf("(calloc) a = %d\n", *a);
   free(a);
}


void testAlloc()
{
   // MALLOC
   int* a = malloc(sizeof(int));
   *a = 10;
   printf("(malloc) a = %d\n", *a);

   showDebugInfo();
}



void my_test()
{
    showDebugInfo();

    printf("Allocated 1024 bytes for the printf function.\n");

    showDebugInfo();

    // Callocate an int array.
    int* val0 = my_calloc(49, sizeof(int));
    printf("Callocated int[49] val0 of first 3 values: %d, %d, %d.\n", val0[0], val0[1], val0[2]);

    showDebugInfo();

    // Allocate a string.
    char* val1 = malloc(sizeof(char) * 14);
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

    // Allocate an int.
    int* val2 = malloc(sizeof(int));
    *val2 = 5;
    printf("Allocated int val2 of value: %d.\n", *val2);

    showDebugInfo();

    // Reallocate val0.
    int* val9 = realloc(val0, sizeof(int));
    printf("Reallocated int[50] val0 to int val9 of value: %d.\n", *val9);

    showDebugInfo();

    // Allocate val3.
    int* val3 = malloc(sizeof(int) * 5);
    val3[0] = 10;
    val3[1] = 5;
    printf("Allocated int[5] val3 of first 2 values: %d, %d.\n", val3[0], val3[1]);

    showDebugInfo();

    // Free val1.
    free(val1);
    printf("Freed char val1 of value: Hello, World!.\n");

    showDebugInfo();

    // Allocate another int.
    int* val4 = malloc(sizeof(int));
    *val4 = 97;
    printf("Allocated int val4 of value: %d.\n", *val4);

    showDebugInfo();

    // Reallocate val3.
    int* val5 = realloc(val3, sizeof(int));
    printf("Reallocated int val3 to variable val5 of value: %d.\n", *val5);

    showDebugInfo();

    // Reallocate val4.
    char* val6 = realloc(val4, sizeof(char));
    printf("Reallocated int val4 to variable char val6 of value: %c.\n", *val6);

    showDebugInfo();

    // Reallocate val5.
    long long int* val7 = realloc(val5, sizeof(long long int));
    *val7 = 1;
    printf("Reallocated int val5 to variable long long int val7 of value: %lld.\n", *val7);

    showDebugInfo();

    // Free val7 using realloc.
    int* val8 = realloc(val7, 0);
    (void)val8;
    printf("Using realloc, freed long long int val7 of value: 1.\n");

    showDebugInfo();

    // Free val6.
    free(val6);
    printf("Freed char val6 of value: a.\n");

    showDebugInfo();

    // Free val7.
    free(val2);
    printf("Freed char val2 of value: 5.\n");

    showDebugInfo();

    // Free val9.
    free(val9);
    printf("Freed int val9 of value: 0.\n");

    showDebugInfo();
}

 
int main()
{
    registerAllocatorHook();

    my_test();

    return 0;
}
