#define CARENA_IMPLEMENTATION
#include "carena.h"

#include "stdio.h"
#include <stdalign.h>

#define TEST_STARTED printf("`%s` started\n", __FUNCTION__)
#define TEST_PASSED printf("`%s` passed\n", __FUNCTION__)

#define FN_TEST(fn_name, body)                                                 \
  void fn_name(void) {                                                         \
    TEST_STARTED;                                                              \
    do                                                                         \
      body while (0);                                                          \
    TEST_PASSED;                                                               \
  }

typedef struct {
  int a;
  char *x;
} TestStruct1;

FN_TEST(test_array_gen, {
  Arena arena = arena_init(10 << 10);

  TestStruct1 *my_struct_array = ARRAY_INIT(&arena, TestStruct1);

  for (size_t i = 0; i < 78; i++) {

    char *buf = ARENA_ALLOC(&arena, char, 1);
    snprintf(buf, 30, "hello %zu", i);
    ARRAY_PUSH(my_struct_array, TestStruct1, ((TestStruct1){i * 10, buf}));
  }

  for (size_t i = 0; i < ARRAY_LENGTH(my_struct_array, TestStruct1); i++) {
    printf("ARRAY_CAPACITY(my_struct_array, TestStruct1) = %zu\n",
           ARRAY_CAPACITY(my_struct_array, TestStruct1));
    printf("my_struct_array[%zu].a = %d\n", i, my_struct_array[i].a);
    printf("my_struct_array[%zu].x = %s\n", i, my_struct_array[i].x);
  }

  arena_destroy(&arena);
});

int main() { test_array_gen(); }
