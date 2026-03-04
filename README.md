# Memory Arena + Arena allocated dynamic array implementation in C11


## Usage
To use it in your project just get it by:
- curl
```bash
curl -O https://raw.githubusercontent.com/mirsahebali/carena.h/refs/heads/main/carena.h

```
- wget

```bash
wget https://raw.githubusercontent.com/mirsahebali/carena.h/refs/heads/main/carena.h
```
- git 
```bash
git clone --depth 1 https://github.com/mirsahebali/carena.h

```


## Using in your project
```c
#define CARENA_IMPLEMENTATION
#include "path/of/deps/carena.h"

```
## Reference

### Arena API
```c

Arena arena_init(size_t size);

void *arena_alloc(Arena *self, size_t size, size_t alignment);

// OR use convenience macro(recommended)
ARENA_ALLOC(arena, type, length);

void arena_destroy(Arena *self);

```

> convenience macro is recommended as it makes the type display explicit 

### Dynamic Array API(allocated in the arena)

```c

void *array_init(Arena *arena, size_t item_size, size_t capacity,
                 size_t alignment);
size_t array_push(void *array, void *data);
void *array_get(void *array, size_t item_size, size_t alignment, size_t index);
// Get the pointer with points to that memory location in the array(NOT recommended) but use if needed
void *array_get_ref(void *array, size_t item_size, size_t alignment,
                    size_t index);

// Use convenience macros for strong type display
ARRAY_PUSH(array, type, data);

ARRAY_GET(array, type, index);

// get reference to that element
ARRAY_GET_REF(array, type, index);


// Other helper functions to use

ARRAY_CAPACITY(array, type);
ARRAY_LENGTH(array, type);
// the _ArrayHeader which contains meta data for the array
ARRAY_HEADER_T(array, type);

```

> [NOTE]
> No need to free the array as it allocated in the arena, it will automatically rellocated when elements are pushed
> It will all be freed at once


