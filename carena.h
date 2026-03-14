// SPDX-License-Identifier: BSD-3-Clause
/*
------------------------------------------------------------------------------
BSD 3-Clause License

Copyright (c) 2026, Mir Saheb Ali
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------
*/

#ifndef CARENA_H
#define CARENA_H

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define IS_POWER_OF_TWO(x) (x != 0 && (x & (x - 1)))
#define _CARENA_ARRAY_DEFAULT_INIT_CAPACITY 16

#if __STDC_VERSION__ >= 201112L // Check for C11 or newer
#define _CARENA_ALIGNOF(T) _Alignof(T)
#elif defined(__GNUC__) || defined(__clang__)
#define _CARENA_ALIGNOF(T) __alignof__(T)
#else
#define _CARENA_ALIGNOF(T)                                                     \
  offsetof(                                                                    \
      struct {                                                                 \
        char c;                                                                \
        T member;                                                              \
      },                                                                       \
      member)
#endif

typedef struct Arena {
  void *data;
  uintptr_t offset;
  size_t size;
} Arena;

Arena arena_init(size_t size);

static inline uintptr_t align_forward(uintptr_t data_ptr, size_t alignment);
void *arena_alloc(Arena *self, size_t size, size_t alignment);
#define ARENA_ALLOC(arena, type, length)                                       \
  arena_alloc(arena, sizeof(type) * length, _CARENA_ALIGNOF(type))
void arena_reset(Arena *self);
void arena_free(Arena *self);
void arena_destroy(Arena *self);

typedef struct {
  size_t capacity;
  size_t length;
  size_t alignment;
  Arena *arena;
} _ArrayHeader;

void *array_init(Arena *arena, size_t item_size, size_t capacity,
                 size_t alignment);
void *array_reserve(const void *array, size_t item_size, size_t alignment,
                    size_t new_size);
size_t array_push(void *array, void *data);
void *array_get(void *array, size_t item_size, size_t alignment, size_t index);
void *array_get_ref(void *array, size_t item_size, size_t alignment,
                    size_t index);

// convenience macros
#define ARRAY_INIT(arena, type)                                                \
  (type *)array_init(arena, sizeof(type), _CARENA_ARRAY_DEFAULT_INIT_CAPACITY, \
                     _CARENA_ALIGNOF(type))

#define ARRAY_WITH_CAPACITY(arena, type, capacity)                             \
  (type *)array_init(arena, sizeof(type), capacity, _CARENA_ALIGNOF(type))

#define PADDED_HEADER_SIZE(alignment)                                          \
  ((sizeof(_ArrayHeader) + (alignment - 1)) & ~(alignment - 1))

#define ARRAY_HEADER(array, alignment)                                         \
  ((_ArrayHeader *)((char *)(array) - PADDED_HEADER_SIZE(alignment)))

#define PADDED_HEADER_SIZE_T(type)                                             \
  ((sizeof(_ArrayHeader) + (_CARENA_ALIGNOF(type) - 1)) &                      \
   ~(_CARENA_ALIGNOF(type) - 1))

#define ARRAY_HEADER_T(array, type)                                            \
  ((_ArrayHeader *)((char *)(array) - PADDED_HEADER_SIZE_T(type)))

#define ARRAY_LENGTH(array, type) (ARRAY_HEADER_T(array, type)->length)

#define ARRAY_CAPACITY(array, type) (ARRAY_HEADER_T(array, type)->capacity)

#define ARRAY_PUSH(array, type, data)                                          \
  do {                                                                         \
    _ArrayHeader *header = ARRAY_HEADER_T(array, type);                        \
    void *new_ptr = array_reserve(array, sizeof(type), _CARENA_ALIGNOF(type),  \
                                  header->length + 1);                         \
    assert(new_ptr != NULL);                                                   \
    header = ARRAY_HEADER_T(new_ptr, type);                                    \
    array = new_ptr;                                                           \
    array[header->length++] = data;                                            \
  } while (0)

#define ARRAY_GET_REF(array, type, index)                                      \
  ((type *)array_get(array, sizeof(type), _CARENA_ALIGNOF(type), index))

#define ARRAY_GET(array, type, index) (*ARRAY_GET_REF(array, type, index))

#endif // !CARENA_H

#ifdef CARENA_IMPLEMENTATION

Arena arena_init(size_t size) {
  Arena arena = (Arena){
      .data = NULL,
      .offset = 0,
      .size = size,
  };
  arena.data = malloc(size);
  assert(arena.data != NULL);
  return arena;
}

static inline uintptr_t align_forward(uintptr_t data_ptr, size_t alignment) {
  if (alignment == 0)
    return 1;
  if (IS_POWER_OF_TWO(alignment)) {
    return data_ptr;
  };

  uintptr_t new_ptr = data_ptr;

  uintptr_t modulo = new_ptr & (alignment - 1);
  if (modulo != 0)
    new_ptr += alignment - modulo;
  return new_ptr;
}
void *arena_alloc(Arena *self, size_t size, size_t alignment) {
  assert(self != NULL);
  uintptr_t aligned =
      align_forward((uintptr_t)self->data + self->offset, alignment);
  uintptr_t new_offset = aligned - (uintptr_t)self->data;

  assert(new_offset < self->size);
  assert(new_offset + size <= self->size);
  void *result = (void *)aligned;
  self->offset = new_offset + size;
  return result;
}

void arena_reset(Arena *self) { self->offset = 0; }
void arena_destroy(Arena *self) {
  free(self->data);
  self->data = NULL;
  self->offset = 0;
  self->size = 0;
}

void *array_init(Arena *arena, size_t item_size, size_t capacity,
                 size_t alignment) {
  size_t header_size = sizeof(_ArrayHeader);
  size_t padded_header_size =
      (header_size + (alignment - 1)) & ~(alignment - 1);

  capacity = capacity ? capacity : 16;
  size_t total_size = padded_header_size + (item_size * capacity);

  _ArrayHeader *header =
      (_ArrayHeader *)arena_alloc(arena, total_size, alignment);

  assert(header != NULL);

  void *data = (char *)header + padded_header_size;

  header->capacity = capacity;
  header->length = 0;
  header->alignment = alignment;
  header->arena = arena;

  return data;
}

void *array_reserve(const void *array, size_t item_size, size_t alignment,
                    size_t new_size) {
  _ArrayHeader *header = ARRAY_HEADER(array, alignment);
  if (header->capacity >= new_size)
    return (void *)array;

  assert(header->alignment == alignment);
  size_t new_capacity = header->capacity ? header->capacity * 2 : 8;
  size_t padded_header_size = PADDED_HEADER_SIZE(alignment);
  size_t total_new_size = padded_header_size + (new_capacity * item_size);
  _ArrayHeader *new_header = (_ArrayHeader *)arena_alloc(
      header->arena, total_new_size, header->alignment);
  if (!new_header)
    return NULL;
  new_header->capacity = new_capacity;
  new_header->length = header->length;
  new_header->alignment = header->alignment;
  new_header->arena = header->arena;

  void *new_data_ptr = ((char *)new_header + padded_header_size);
  memcpy(new_data_ptr, array, header->length * (item_size));

  return new_data_ptr;
}

void *array_get(void *array, size_t item_size, size_t alignment, size_t index) {
  assert(array != NULL);
  _ArrayHeader *header = ARRAY_HEADER(array, alignment);
  assert(header != NULL);
  assert(header->alignment == alignment);
  assert(index < header->length && index >= 0);

  return (char *)array + (index * item_size);
}

void *array_get_ref(void *array, size_t item_size, size_t alignment,
                    size_t index) {
  assert(array != NULL);
  _ArrayHeader *header = ARRAY_HEADER(array, alignment);
  assert(header != NULL);
  assert(header->alignment == alignment);
  assert(index >= 0);
  assert(index < header->length);

  return (char *)array + (index * item_size);
}

#endif // CARENA_IMPLEMENTATION
