/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <string.h>
#include "../libc3/array.h"
#include "../libc3/str.h"
#include "../libc3/sym.h"
#include "test.h"

#define ARRAY_TEST_INIT_CLEAN(type, dimension, dimensions)             \
  do {                                                                 \
    s_array a;                                                         \
    TEST_EQ(array_init(&a, (type), (dimension), (dimensions)), &a);    \
    array_clean(&a);                                                   \
  } while(0)

#define ARRAY_TEST_INIT_1_CLEAN(test)                                  \
  do {                                                                 \
    s_array a;                                                         \
    TEST_EQ(array_init_1(&a, (test)), &a);                             \
    array_clean(&a);                                                   \
  } while(0)

#define ARRAY_TEST_INSPECT(test, expected)                             \
  do {                                                                 \
    s_str str;                                                         \
    s_array tmp;                                                       \
    test_context("array_inspect(" # test ") -> " # expected);          \
    TEST_EQ(array_init_1(&tmp, (test)), &tmp);                         \
    TEST_EQ(array_inspect(&tmp, &str), &str);                          \
    TEST_STRNCMP(str.ptr.p, (expected), str.size);                     \
    array_clean(&tmp);                                                 \
    str_clean(&str);						       \
  } while(0)

void array_test (void);
TEST_CASE_PROTOTYPE(array_data);
TEST_CASE_PROTOTYPE(array_init_clean);
TEST_CASE_PROTOTYPE(array_init_1_clean);
TEST_CASE_PROTOTYPE(array_inspect);

void array_test (void)
{
  TEST_CASE_RUN(array_init_clean);
  TEST_CASE_RUN(array_init_1_clean);
  TEST_CASE_RUN(array_data);
  TEST_CASE_RUN(array_inspect);
}

TEST_CASE(array_data)
{
  s_array a;
  TEST_EQ(array_init_1(&a, "(U8[]) {1, 2, 3}"), &a);
  TEST_EQ(* (u8 *) array_data(&a, (uw []) {0}), 1);
  TEST_EQ(* (u8 *) array_data(&a, (uw []) {1}), 2);
  TEST_EQ(* (u8 *) array_data(&a, (uw []) {2}), 3);
  array_clean(&a);
  TEST_EQ(array_init_1(&a, "(U8[]) {1 + 1, 2 + 2, 3 + 3}"), &a);
  TEST_EQ(* (u8 *) array_data(&a, (uw []) {0}), 2);
  TEST_EQ(* (u8 *) array_data(&a, (uw []) {1}), 4);
  TEST_EQ(* (u8 *) array_data(&a, (uw []) {2}), 6);
  array_clean(&a);
}
TEST_CASE_END(array_data)

TEST_CASE(array_init_clean)
{
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 1, (uw []) {1});
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 1, (uw []) {10});
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 1, (uw []) {100});
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 2, ((uw []) {1, 1}));
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 2, ((uw []) {10, 10}));
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 2, ((uw []) {100, 100}));
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 3, ((uw []) {1, 1, 1}));
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 3, ((uw []) {10, 10, 10}));
  ARRAY_TEST_INIT_CLEAN(sym_1("U8[]"), 3, ((uw []) {100, 100, 100}));
}
TEST_CASE_END(array_init_clean)

TEST_CASE(array_init_1_clean)
{
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {0}");
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {0, 0}");
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {0, 0, 0}");
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {{0}, {0}}");
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {{0, 0}, {0, 0}}");
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {{0, 0}, {0, 0}, {0, 0}}");
  ARRAY_TEST_INIT_1_CLEAN("(U8[]) {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}}");
}
TEST_CASE_END(array_init_1_clean)

TEST_CASE(array_inspect)
{
  ARRAY_TEST_INSPECT("(U8[]) {0}",
                     "(U8[]) {0}");
  ARRAY_TEST_INSPECT("(U8[]) {0, 0}",
                     "(U8[]) {0, 0}");
  ARRAY_TEST_INSPECT("(U8[]) {0, 0, 0}",
                     "(U8[]) {0, 0, 0}");
  ARRAY_TEST_INSPECT("(U8[]) {{0}, {0}}",
                     "(U8[]) {{0}, {0}}");
  ARRAY_TEST_INSPECT("(U8[]) {{0, 0}, {0, 0}}",
                     "(U8[]) {{0, 0}, {0, 0}}");
  ARRAY_TEST_INSPECT("(U8[]) {{0, 0}, {0, 0}, {0, 0}}",
                     "(U8[]) {{0, 0}, {0, 0}, {0, 0}}");
  ARRAY_TEST_INSPECT("(U8[]) {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}}",
                     "(U8[]) {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}}");
  ARRAY_TEST_INSPECT("(U8[]) {1, 2, 3}",
                     "(U8[]) {1, 2, 3}");
  ARRAY_TEST_INSPECT("(U8[]) {1 + 1, 2 + 2, 3 + 3}",
                     "(U8[]) {2, 4, 6}");
  ARRAY_TEST_INSPECT("(U8[]) {255 + 1, 255 + 2, 255 + 3}",
                     "(U8[]) {0, 1, 2}");
  ARRAY_TEST_INSPECT("(U16[]) {255 + 1, 255 + 2, 255 + 3}",
                     "(U16[]) {256, 257, 258}");
  ARRAY_TEST_INSPECT("(U16[]) {65535 + 1, 65535 + 2, 65535 + 3}",
                     "(U16[]) {0, 1, 2}");
  ARRAY_TEST_INSPECT("(U32[]) {65535 + 1, 65535 + 2, 65535 + 3}",
                     "(U32[]) {65536, 65537, 65538}");
  ARRAY_TEST_INSPECT("(U32[]) {4294967295 + 1, 4294967295 + 2, 4294967295 + 3}",
                     "(U32[]) {0, 1, 2}");
  ARRAY_TEST_INSPECT("(U64[]) {4294967295 + 1, 4294967295 + 2, 4294967295 + 3}",
                     "(U64[]) {4294967296, 4294967297, 4294967298}");
  ARRAY_TEST_INSPECT("(U64[]) {18446744073709551615 + 1, 18446744073709551615 + 2, 18446744073709551615 + 3}",
                     "(U64[]) {0, 1, 2}");
  ARRAY_TEST_INSPECT("(Integer[]) {18446744073709551615 + 1, 18446744073709551615 + 2, 18446744073709551615 + 3}",
                     "(Integer[]) {18446744073709551616, 18446744073709551617, 18446744073709551618}");
}
TEST_CASE_END(array_inspect)
