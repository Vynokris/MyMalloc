# Malloc project



## Presentation

Memory allocation project for ISART digital (2021). <br>
The following functions were remade entirely with custom code, and replace the functions of `<malloc.h>` with the same name.


<br>

## Functions

<ul>
<li> malloc </li>
<li> realloc </li>
<li> calloc </li>
<li> free </li>
</ul>


<br>

## Function documentation

### Malloc

<ul>
<li> Definition: </li>
    <ul>
    <li> <code> void* my_malloc(size_t size) </code> </li>
    </ul>
<li> Description: </li>
    <ul>
    <li> Allocates memory of a given size and returns a pointer to it.
    </ul>
<li> Parameters: </li>
    <ul>
    <li> <code> size_t size </code> is the size of the memory to be allocated. </li>
    </ul>
<li> Return value: </li>
    <ul>
    <li> Returns a void pointer to the adress of the allocated memory. </li>
    </ul>
<li> Notes: </li>
    <ul>
    <li> This function should be called with <code> malloc()</code>. <br> 
         This is because it replaces the malloc function of <code> malloc.h</code>. </li>
    </ul>
</ul>


### Realloc

<ul>
<li> Definition: </li>
    <ul>
    <li> <code> void* my_realloc(void* ptr, size_t size) </code> </li>
    </ul>
<li> Description: </li>
    <ul>
    <li> Returns a pointer to a memory block of the given size, that contains the same data as the given pointer. </li>
    </ul>
<li> Parameters: </li>
    <ul>
    <li> <code> void* ptr </code> is the pointer which data should be copied to the new memory block. </li>
    <li> <code> size_t size </code> is the size of the memory block to be allocated. </li>
    </ul>
<li> Return value: </li>
    <ul>
    <li> Returns a void pointer to the adress of the allocated memory. </li>
    </ul>
<li> Notes: </li>
    <ul>
    <li> This function should be called with <code> realloc()</code>. <br> 
         This is because it replaces the realloc function of <code> malloc.h</code>. </li>
    </ul>
</ul>


### Calloc

<ul>
<li> Definition: </li>
    <ul>
    <li> <code> void* my_calloc(size_t nb, size_t size) </code> </li>
    </ul>
<li> Description: </li>
    <ul>
    <li> Allocates memory of a given size, initializes all its values to 0 and returns a pointer to it. </li>
    </ul>
<li> Parameters: </li>
    <ul>
    <li> <code> size_t nb </code> is the number of items to be allocated in the memory block. </li>
    <li> <code> size_t size </code> is the size of the items to be allocated. </li>
    </ul>
<li> Return value: </li>
    <ul>
    <li> Returns a void pointer to the adress of the allocated memory. </li>
    </ul>
<li> Notes: </li>
    <ul>
    <li> This function cannot be called with <code> my_calloc()</code>. <br> 
         This is because <code> malloc.h </code> doesn't provide a way to replace its calloc function. </li>
    </ul>
</ul>


### Free

<ul>
<li> Definition: </li>
    <ul>
    <li> <code> void my_free(void* ptr) </code> </li>
    </ul>
<li> Description: </li>
    <ul>
    <li> Frees the memory block that is pointed to by the given pointer </li>
    </ul>
<li> Parameters: </li>
    <ul>
    <li> <code> void* ptr </code> is the pointer to the memory that should be freed. </li>
    </ul>
<li> Return value: </li>
    <ul>
    <li> Returns nothing. </li>
    </ul>
<li> Notes: </li>
    <ul>
    <li> This function should be called with <code> free()</code>. <br> 
         This is because it replaces the free function of <code> malloc.h</code>. </li>
    </ul>
</ul>



<br>

## Building the project

Simply call `make` in the root of the project and run test_allocator to test the project. 

To include the library in another project, copy the src/ and include/ folders to the root of your project and include `<my_allocator.h>`. <br>
Don't forget to build `src/my_allocator.c` when you build your project!
