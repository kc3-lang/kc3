/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "../libc3/str.h"
#include "test.h"

#define STR_TEST_INSPECT(test, expected)                               \
  do {                                                                 \
    s_str result;                                                      \
    assert(test);                                                      \
    test_context("str_inspect(" # test ") -> " # expected);            \
    TEST_EQ(str_inspect((test), &result), &result);                    \
    TEST_STRNCMP(result.ptr.p, (expected), result.size);               \
    str_clean(&result);                                                \
    test_context(NULL);                                                \
  } while (0)

#define STR_TEST_INSPECT_1(test, expected)                               \
  do {                                                                 \
    s_str result;                                                      \
    s_str str;                                                         \
    str_init_1(&str, NULL, (test));                                    \
    test_context("str_inspect(" # test ") -> " # expected);            \
    TEST_EQ(str_inspect(&str, &result), &result);                      \
    TEST_STRNCMP(result.ptr.p, (expected), result.size);               \
    str_clean(&result);                                                \
    test_context(NULL);                                                \
  } while (0)

#define STR_TEST_TO_HEX(test, expected)                                  \
  do {                                                                 \
    s_str str;                                                         \
    s_str *test_;                                                      \
    test_context("str_to_hex(" # test ") -> " # expected);               \
    test_ = (test);                                                    \
    TEST_EQ(str_to_hex(test_, &str), &str);                            \
    TEST_STRNCMP(str.ptr.p, (expected), str.size);                       \
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
TEST_CASE_PROTOTYPE(str_init_dup);
TEST_CASE_PROTOTYPE(str_init_dup_1);
TEST_CASE_PROTOTYPE(str_inspect);
TEST_CASE_PROTOTYPE(str_new_1);
TEST_CASE_PROTOTYPE(str_new_cpy);
TEST_CASE_PROTOTYPE(str_new_delete);
TEST_CASE_PROTOTYPE(str_new_dup);
TEST_CASE_PROTOTYPE(str_new_f);
TEST_CASE_PROTOTYPE(str_to_hex);
TEST_CASE_PROTOTYPE(str_to_ident);
TEST_CASE_PROTOTYPE(str_to_sym);


void str_test (void)
{
  TEST_CASE_RUN(str_init_clean);
  TEST_CASE_RUN(str_init_dup);
  TEST_CASE_RUN(str_init_dup_1);
  TEST_CASE_RUN(str_new_delete);
  TEST_CASE_RUN(str_new_1);
  TEST_CASE_RUN(str_new_dup);
  TEST_CASE_RUN(str_new_cpy);
  TEST_CASE_RUN(str_new_f);
  TEST_CASE_RUN(str_character_is_reserved);
  TEST_CASE_RUN(str_inspect);
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
  TEST_STRNCMP(stra.ptr.p, "test", len);
  str_clean(&stra);
  test_ok();
  len = 4;
  m = malloc(len);
  assert(m);
  memcpy(m, "test", len);
  str_init(&stra, m, len, m);
  TEST_EQ(stra.size, len);
  TEST_STRNCMP(stra.ptr.p, "test", len);
  str_clean(&stra);
  test_ok();
}
TEST_CASE_END(str_init_clean)

TEST_CASE(str_init_dup)
{
  size_t len;
  char *m;
  s_str str;
  s_str test;
  len = 4;
  str_init(&test, NULL, len, "test");
  str_init_dup(&str, &test);
  str_clean(&test);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p, "test", len);
  str_clean(&str);
  test_ok();
  len = 4;
  m = malloc(len);
  assert(m);
  memcpy(m, "test", len);
  str_init(&test, m, len, m);
  str_init_dup(&str, &test);
  str_clean(&test);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p, "test", len);
  str_clean(&str);
  test_ok();
}
TEST_CASE_END(str_init_dup)

TEST_CASE(str_init_dup_1)
{
  size_t len;
  char *m;
  s_str str;
  len = 4;
  str_init_dup_1(&str, "test");
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p, "test", len);
  str_clean(&str);
  test_ok();
  len = 4;
  m = malloc(len + 1);
  assert(m);
  memcpy(m, "test", len + 1);
  str_init_dup_1(&str, m);
  free(m);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p, "test", len);
  str_clean(&str);
  test_ok();
}
TEST_CASE_END(str_init_dup_1)

TEST_CASE(str_inspect)
{
  s_str str;
  s8 zero[16];
  bzero(zero, sizeof(zero));
  STR_TEST_INSPECT_1("", "\"\"");
  STR_TEST_INSPECT_1(" ", "\" \"");
  STR_TEST_INSPECT_1("\n", "\"\\n\"");
  STR_TEST_INSPECT_1("\r", "\"\\r\"");
  STR_TEST_INSPECT_1("\t", "\"\\t\"");
  STR_TEST_INSPECT_1("\v", "\"\\v\"");
  STR_TEST_INSPECT_1("\"", "\"\\\"\"");
  STR_TEST_INSPECT_1("\\", "\"\\\\\"");
  STR_TEST_INSPECT_1(".", "\".\"");
  STR_TEST_INSPECT_1("..", "\"..\"");
  STR_TEST_INSPECT_1("...", "\"...\"");
  STR_TEST_INSPECT_1(".. .", "\".. .\"");
  STR_TEST_INSPECT_1("t", "\"t\"");
  STR_TEST_INSPECT_1("T", "\"T\"");
  STR_TEST_INSPECT_1("test", "\"test\"");
  STR_TEST_INSPECT_1("Test", "\"Test\"");
  STR_TEST_INSPECT_1("123", "\"123\"");
  STR_TEST_INSPECT_1("test123", "\"test123\"");
  STR_TEST_INSPECT_1("Test123", "\"Test123\"");
  STR_TEST_INSPECT_1("test 123", "\"test 123\"");
  STR_TEST_INSPECT_1("Test 123", "\"Test 123\"");
  STR_TEST_INSPECT_1("test123.test456", "\"test123.test456\"");
  STR_TEST_INSPECT_1("Test123.Test456", "\"Test123.Test456\"");
  STR_TEST_INSPECT_1("É", "\"É\"");
  STR_TEST_INSPECT_1("Éo", "\"Éo\"");
  STR_TEST_INSPECT_1("Éoà \n\r\t\v\"",
                     "\"Éoà \\n\\r\\t\\v\\\"\"");
  STR_TEST_INSPECT_1("é", "\"é\"");
  STR_TEST_INSPECT_1("éo", "\"éo\"");
  STR_TEST_INSPECT_1("éoà \n\r\t\v\"",
                     "\"éoà \\n\\r\\t\\v\\\"\"");
  STR_TEST_INSPECT_1("Π", "\"Π\"");
  STR_TEST_INSPECT_1("꒴", "\"꒴\"");
  STR_TEST_INSPECT_1("𐅀", "\"𐅀\"");
  STR_TEST_INSPECT_1("ÉoàΠ꒴𐅀 \n\r\t\v\\\"",
                     "\"ÉoàΠ꒴𐅀 \\n\\r\\t\\v\\\\\\\"\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  1, zero), "\"\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  2, zero), "\"\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  3, zero), "\"\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  4, zero), "\"\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  5, zero), "\"\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  6, zero), "\"\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  7, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  8, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL,  9, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 10, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 11, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 12, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 13, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 14, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 15, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_init(&str, NULL, 16, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0"
                   "\\0\"");
}
TEST_CASE_END(str_inspect)

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
  TEST_ASSERT((str = str_new_cpy(4, "test")));
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
  TEST_STRNCMP(str->ptr.p, "test", len);
  str_delete(str);
  test_ok();
  len = 4;
  m = malloc(len);
  memcpy(m, "test", len);
  TEST_ASSERT((str = str_new(m, len, m)));
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p, "test", len);
  str_delete(str);
  test_ok();
}
TEST_CASE_END(str_new_delete)

TEST_CASE(str_new_dup)
{
  size_t len;
  char *m;
  s_str *str;
  s_str test;
  len = 4;
  str_init(&test, NULL, len, "test");
  TEST_ASSERT((str = str_new_dup(&test)));
  str_clean(&test);
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p, "test", len);
  str_delete(str);
  test_ok();
  len = 4;
  m = malloc(len);
  memcpy(m, "test", len);
  str_init(&test, m, len, m);
  TEST_ASSERT((str = str_new_dup(&test)));
  str_clean(&test);
  TEST_EQ(str->size, len);
  TEST_STRNCMP(str->ptr.p, "test", len);
  str_delete(str);
  test_ok();
}
TEST_CASE_END(str_new_dup)

TEST_CASE(str_new_f)
{
  s_str *str;
  TEST_ASSERT((str = str_new_f("test%d", 42)));
  TEST_STRNCMP(str->ptr.p, "test42", str->size);
  str_delete(str);
  TEST_ASSERT((str = str_new_f("test%lld", (long long) 42)));
  TEST_STRNCMP(str->ptr.p, "test42", str->size);
  str_delete(str);
}
TEST_CASE_END(str_new_f)

TEST_CASE(str_to_hex)
{
  s8 zero[32];
  bzero(zero, sizeof(zero));
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
