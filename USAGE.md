# Allocate individual blocks and structs
```c

Arena arena = arena_init(10 << 10); // 10KB initial allocation

// allocating a single object
FooType *foo_object1 = ARENA_ALLOC(&arena, FooType, 1);
FooType *foo_object2 = ARENA_ALLOC(&arena, FooType, 1);


// Free then all at once by scoping the objects' lifetimes with this arena
arena_destroy(&arena);

```

# Arena allocated dynamic Arrays
```c
Arena arena = arena_init(10 << 10); // 10KB initial allocation
// Dynamic arrays are just same as pointers to the element, i.e.,
// int*, char*, FooType*

// Recommended to alias the type to avoid confusion from original pointers to it
typedef int* IntArray;
typedef char* CharArray;
typedef FooType* FooTypeArray;

// inside main or some other function
IntArray int_arr = ARRAY_INIT(&arena, int);
FooTypeArray foo_arr = ARRAY_INIT(&arena, FooType);

// Now we can simply push
ARRAY_PUSH(int_arr, 10); // 10
ARRAY_PUSH(int_arr, 20); // 10, 20
ARRAY_PUSH(int_arr, 30); // 10, 20, 30

ARRAY_PUSH(foo_arr, foo_data1); // foo_data1 
ARRAY_PUSH(foo_arr, foo_data2); // foo_data1, foo_data2 
ARRAY_PUSH(foo_arr, foo_data3); // foo_data1, foo_data2, foo_data3

// We can access elements safely using the macro 'ARRAY_GET'
FooType foo_data = ARRAY_GET(foo_arr, index);

// if we want the pointer to that 
FooType *foo_data_ref = ARRAY_GET_REF(foo_arr, index);
// Now we can modify the data at the pointer(but be very *CAREFUL* using this)
// As they are actually just pointers to them, we can also access elements by this:
int data1 = int_arr[4]; 
//^ But this may not be safe and could give currupted data if there were'nt that many elements pushed in it previously

// Get length
size_t footype_len = ARRAY_LENGTH(foo_arr, FooType);

// Get capacity
size_t footype_cap = ARRAY_CAPACITY(foo_arr, FooType);

// Destroy all of then together :)
arena_destroy(&arena);

```
