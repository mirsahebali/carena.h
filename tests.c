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

    char *buf = ARENA_ALLOC(&arena, char, 30);
    snprintf(buf, 30, "hello %zu", i);
    my_struct_array =
        ARRAY_PUSH(my_struct_array, TestStruct1, (&(TestStruct1){i * 10, buf}));
  }

  for (size_t i = 0; i < ARRAY_LENGTH(my_struct_array, TestStruct1); i++) {
    printf("ARRAY_CAPACITY(my_struct_array, TestStruct1) = %zu\n",
           ARRAY_CAPACITY(my_struct_array, TestStruct1));
    printf("my_struct_array[%zu].a = %d\n", i, my_struct_array[i].a);
    printf("my_struct_array[%zu].x = %s\n", i, my_struct_array[i].x);
  }

  arena_destroy(&arena);
})

void test_array_push(void) {
  TEST_STARTED;
  Arena arena = arena_init(10 << 10);
  TestStruct1 *my_struct_array = ARRAY_INIT(&arena, TestStruct1);

  for (size_t i = 0; i < 78; i++) {

    char *buf = ARENA_ALLOC(&arena, char, 30);
    snprintf(buf, 30, "hello %zu", i);
    TestStruct1 *t = (TestStruct1 *)malloc(sizeof(TestStruct1));
    t->a = i * 10;
    t->x = buf;
    my_struct_array = (TestStruct1 *)array_push((void *)my_struct_array,
                                                (void *)t, sizeof(TestStruct1),
                                                _CARENA_ALIGNOF(TestStruct1));
    free(t);
  }

  for (size_t i = 0; i < ARRAY_LENGTH(my_struct_array, TestStruct1); i++) {
    printf("ARRAY_CAPACITY(my_struct_array, TestStruct1) = %zu\n",
           ARRAY_CAPACITY(my_struct_array, TestStruct1));
    printf("my_struct_array[%zu].a = %d\n", i, my_struct_array[i].a);
    printf("my_struct_array[%zu].x = %s\n", i, my_struct_array[i].x);
  }

  arena_destroy(&arena);
  TEST_PASSED;
}

int main() {
  test_array_gen();
  test_array_push();
}
