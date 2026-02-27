/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compare_test.h"
#include "../libkc3/str.h"
#include "test.h"

#define STR_TEST_TO_HEX(test, expected)                                \
  do {                                                                 \
    s_str str;                                                         \
    s_str *test_;                                                      \
    test_context("str_to_hex(" # test ") -> " # expected);             \
    test_ = (test);                                                    \
    TEST_EQ(str_to_hex(test_, &str), &str);                            \
    TEST_EQ(str.size, strlen(expected));                               \
    TEST_STRNCMP(str.ptr.p_pvoid, (expected), str.size);                     \
    str_clean(&str);                                                   \
    str_delete(test_);                                                 \
    test_context(NULL);                                                \
  } while (0)

#define STR_TEST_TO_SYM(test)                                          \
  do {                                                                 \
    s_str str;                                                         \
    const s_sym *result = 0;                                           \
    test_context("str_to_sym(" # test ")");                            \
    str_init_1(&str, NULL, (test));                                    \
    TEST_ASSERT((result = str_to_sym(&str)));                          \
    TEST_EQ(str_to_sym(&str), result);                                 \
    COMPARE_TEST_STR(&result->str, &str, 0);                           \
  } while (0)

TEST_CASE_PROTOTYPE(str_character_is_reserved);
TEST_CASE_PROTOTYPE(str_init_clean);
TEST_CASE_PROTOTYPE(str_init_copy);
TEST_CASE_PROTOTYPE(str_init_copy_1);
TEST_CASE_PROTOTYPE(str_new_1);
TEST_CASE_PROTOTYPE(str_new_cpy);
TEST_CASE_PROTOTYPE(str_new_delete);
TEST_CASE_PROTOTYPE(str_new_copy);
TEST_CASE_PROTOTYPE(str_new_f);
TEST_CASE_PROTOTYPE(str_to_hex);
TEST_CASE_PROTOTYPE(str_to_ident);
TEST_CASE_PROTOTYPE(str_to_sym);

void str_test (void)
{
  TEST_CASE_RUN(str_init_clean);
  TEST_CASE_RUN(str_init_copy);
  TEST_CASE_RUN(str_init_copy_1);
  TEST_CASE_RUN(str_new_delete);
  TEST_CASE_RUN(str_new_1);
  TEST_CASE_RUN(str_new_copy);
  TEST_CASE_RUN(str_new_cpy);
  TEST_CASE_RUN(str_new_f);
  TEST_CASE_RUN(str_character_is_reserved);
  TEST_CASE_RUN(str_to_hex);
  TEST_CASE_RUN(str_to_sym);
}

TEST_CASE(str_character_is_reserved)
{
  TEST_ASSERT(str_character_is_reserved('\0'));
  TEST_ASSERT(str_character_is_reserved('\x01'));
  TEST_ASSERT(str_character_is_reserved('\x02'));
  TEST_ASSERT(str_character_is_reserved('\x03'));
  TEST_ASSERT(str_character_is_reserved('\x04'));
  TEST_ASSERT(str_character_is_reserved('\x05'));
  TEST_ASSERT(str_character_is_reserved('\x06'));
  TEST_ASSERT(! str_character_is_reserved(' '));
  TEST_ASSERT(str_character_is_reserved('\a'));
  TEST_ASSERT(str_character_is_reserved('\b'));
  TEST_ASSERT(str_character_is_reserved('\t'));
  TEST_ASSERT(str_character_is_reserved('\n'));
  TEST_ASSERT(str_character_is_reserved('\v'));
  TEST_ASSERT(str_character_is_reserved('\f'));
  TEST_ASSERT(str_character_is_reserved('\r'));
  TEST_ASSERT(! str_character_is_reserved('0'));
  TEST_ASSERT(! str_character_is_reserved('1'));
  TEST_ASSERT(! str_character_is_reserved('2'));
  TEST_ASSERT(! str_character_is_reserved('7'));
  TEST_ASSERT(! str_character_is_reserved('8'));
  TEST_ASSERT(! str_character_is_reserved('9'));
  TEST_ASSERT(! str_character_is_reserved('A'));
  TEST_ASSERT(! str_character_is_reserved('B'));
  TEST_ASSERT(! str_character_is_reserved('C'));
  TEST_ASSERT(! str_character_is_reserved('X'));
  TEST_ASSERT(! str_character_is_reserved('Y'));
  TEST_ASSERT(! str_character_is_reserved('Z'));
  TEST_ASSERT(! str_character_is_reserved('a'));
  TEST_ASSERT(! str_character_is_reserved('b'));
  TEST_ASSERT(! str_character_is_reserved('c'));
  TEST_ASSERT(! str_character_is_reserved('x'));
  TEST_ASSERT(! str_character_is_reserved('y'));
  TEST_ASSERT(! str_character_is_reserved('z'));
  TEST_ASSERT(! str_character_is_reserved('\''));
  TEST_ASSERT(str_character_is_reserved('"'));
  TEST_ASSERT(str_character_is_reserved('\\'));
}
TEST_CASE_END(str_character_is_reserved)

TEST_CASE(str_init_clean)
{
  size_t len;
  char *m;
  s_str stra;
  len = 4;
  str_init(&stra, NULL, len, "test");
  TEST_EQ(stra.size, len);
  TEST_STRNCMP(stra.ptr.p_pvoid, "test", len);
  str_clean(&stra);
  len = 4;
  m = malloc(len + 1);
  assert(m);
  memcpy(m, "test", len);
  str_init(&stra, m, len, m);
  TEST_EQ(stra.size, len);
  TEST_STRNCMP(stra.ptr.p_pvoid, "test", len);
  str_clean(&stra);
}
TEST_CASE_END(str_init_clean)

TEST_CASE(str_init_copy)
{
  size_t len;
  char *m;
  s_str str;
  s_str test;
  len = 4;
  str_init(&test, NULL, len, "test");
  str_init_copy(&str, &test);
  str_clean(&test);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p_pvoid, "test", len);
  str_clean(&str);
  len = 4;
  m = malloc(len + 1);
  assert(m);
  memcpy(m, "test", len);
  str_init(&test, m, len, m);
  str_init_copy(&str, &test);
  str_clean(&test);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p_pvoid, "test", len);
  str_clean(&str);
}
TEST_CASE_END(str_init_copy)

TEST_CASE(str_init_copy_1)
{
  size_t len;
  char *m;
  s_str str;
  len = 4;
  str_init_copy_1(&str, "test");
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p_pvoid, "test", len);
  str_clean(&str);
  len = 4;
  m = malloc(len + 1);
  assert(m);
  memcpy(m, "test", len + 1);
  str_init_copy_1(&str, m);
  free(m);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p_pvoid, "test", len);
  str_clean(&str);
}
TEST_CASE_END(str_init_copy_1)

TEST_CASE(str_new_1)
{
  s_str *str;
  TEST_ASSERT((str = str_new_1(NULL, "test")));
  str_delete(str);
}
TEST_CASE_END(str_new_1)

TEST_CASE(str_new_cpy)
{
  s_str *str;
  TEST_ASSERT((str = str_new_cpy("test", 4)));
  str_delete(str);
}
TEST_CASE_END(str_new_cpy)

TEST_CASE(str_new_delete)
{
  size_t len;
  char *m;
  s_str *str;
  len = 4;
  TEST_ASSERT((str = str_new(NULL, len, "test")));
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p_pvoid, "test", len);
  str_delete(str);
  len = 4;
  m = malloc(len + 1);
  memcpy(m, "test", len);
  TEST_ASSERT((str = str_new(m, len, m)));
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p_pvoid, "test", len);
  str_delete(str);
}
TEST_CASE_END(str_new_delete)

TEST_CASE(str_new_copy)
{
  size_t len;
  char *m;
  s_str *str;
  s_str test;
  len = 4;
  str_init(&test, NULL, len, "test");
  TEST_ASSERT((str = str_new_copy(&test)));
  str_clean(&test);
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p_pvoid, "test", len);
  str_delete(str);
  len = 4;
  m = malloc(len + 1);
  memcpy(m, "test", len);
  str_init(&test, m, len, m);
  TEST_ASSERT((str = str_new_copy(&test)));
  str_clean(&test);
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p_pvoid, "test", len);
  str_delete(str);
}
TEST_CASE_END(str_new_copy)

TEST_CASE(str_new_f)
{
  s_str *str;
  TEST_ASSERT((str = str_new_f("test%d", 42)));
  TEST_STRNCMP(str->ptr.p_pvoid, "test42", str->size);
  str_delete(str);
  TEST_ASSERT((str = str_new_f("test%lld", (long long) 42)));
  TEST_STRNCMP(str->ptr.p_pvoid, "test42", str->size);
  str_delete(str);
}
TEST_CASE_END(str_new_f)

TEST_CASE(str_to_hex)
{
  char zero[32] = {0};
  STR_TEST_TO_HEX(str_new_1(NULL, ""), "");
  STR_TEST_TO_HEX(str_new(NULL,  1, zero), "00");
  STR_TEST_TO_HEX(str_new(NULL,  2, zero), "0000");
  STR_TEST_TO_HEX(str_new(NULL,  3, zero), "000000");
  STR_TEST_TO_HEX(str_new(NULL,  4, zero), "00000000");
  STR_TEST_TO_HEX(str_new(NULL,  5, zero), "0000000000");
  STR_TEST_TO_HEX(str_new(NULL,  6, zero), "000000000000");
  STR_TEST_TO_HEX(str_new(NULL,  7, zero), "00000000000000");
  STR_TEST_TO_HEX(str_new(NULL,  8, zero), "0000000000000000");
  STR_TEST_TO_HEX(str_new(NULL,  9, zero), "000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 10, zero), "00000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 11, zero), "0000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 12, zero),
                  "000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 13, zero),
                  "00000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 14, zero),
                  "0000000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 15, zero),
                  "000000000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 16, zero),
                  "00000000000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 17, zero),
                  "00000000000000000000000000000000"
                  "00");
  STR_TEST_TO_HEX(str_new(NULL, 18, zero),
                  "00000000000000000000000000000000"
                  "0000");
  STR_TEST_TO_HEX(str_new(NULL, 19, zero),
                  "00000000000000000000000000000000"
                  "000000");
  STR_TEST_TO_HEX(str_new(NULL, 20, zero),
                  "00000000000000000000000000000000"
                  "00000000");
  STR_TEST_TO_HEX(str_new(NULL, 21, zero),
                  "00000000000000000000000000000000"
                  "0000000000");
  STR_TEST_TO_HEX(str_new(NULL, 22, zero),
                  "00000000000000000000000000000000"
                  "000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 23, zero),
                  "00000000000000000000000000000000"
                  "00000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 24, zero),
                  "00000000000000000000000000000000"
                  "0000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 25, zero),
                  "00000000000000000000000000000000"
                  "000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 26, zero),
                  "00000000000000000000000000000000"
                  "00000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 27, zero),
                  "00000000000000000000000000000000"
                  "0000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 28, zero),
                  "00000000000000000000000000000000"
                  "000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 29, zero),
                  "00000000000000000000000000000000"
                  "00000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 30, zero),
                  "00000000000000000000000000000000"
                  "0000000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 31, zero),
                  "00000000000000000000000000000000"
                  "000000000000000000000000000000");
  STR_TEST_TO_HEX(str_new(NULL, 32, zero),
                  "00000000000000000000000000000000"
                  "00000000000000000000000000000000");
  STR_TEST_TO_HEX(str_new_1(NULL, "abc"), "616263");
  STR_TEST_TO_HEX(str_new_1(NULL, "abcdefghijklmnopqrstuvwxyz"),
                  "6162636465666768696A6B6C6D6E6F70"
                  "7172737475767778797A");
}
TEST_CASE_END(str_to_hex)

TEST_CASE(str_to_sym)
{
  STR_TEST_TO_SYM("");
  STR_TEST_TO_SYM("0");
  STR_TEST_TO_SYM("9");
  STR_TEST_TO_SYM("A");
  STR_TEST_TO_SYM("Z");
  STR_TEST_TO_SYM("À");
  STR_TEST_TO_SYM("É");
  STR_TEST_TO_SYM("a");
  STR_TEST_TO_SYM("z");
  STR_TEST_TO_SYM("à");
  STR_TEST_TO_SYM("é");
}
TEST_CASE_END(str_to_sym)
