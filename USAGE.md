# Allocate individual blocks and structs
```c

Arena arena = arena_init(10 << 10); // 10KB initial allocation

// use function with explicit cast
MyType *my_new_object1 = (MyType*)arena_alloc(&arena, sizeof(MyType), alignof(MyType);
MyType *my_new_object2 = (MyType*)arena_alloc(&arena, sizeof(MyType), alignof(MyType);

// OR use convenience macro

// if allocating a single object
MyType *my_new_object1 = ARENA_ALLOC(&arena, MyType, 1);
MyType *my_new_object2 = ARENA_ALLOC(&arena, MyType, 1);


// Free then all at once by scoping the objects' lifetimes with this arena
arena_destroy(&arena);

```

# Arena allocated dynamic Arrays


