/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../libc3/str.h"
#include "test.h"

#define STR_TEST_CMP(a, b, expected)                                   \
  do {                                                                 \
    s_str *a_ = (a);                                                   \
    s_str *b_ = (b);                                                   \
    sw tmp = str_compare(a_, b_);                                          \
    str_delete(a_);                                                    \
    str_delete(b_);                                                    \
    if (tmp == expected) {                                             \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "str_compare(%s, %s) == %s\n"                                 \
             "Expected %s got %ld.%s\n",                               \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # a, # b, # expected, # expected, tmp,                    \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#define STR_TEST_INSPECT(test, expected)                               \
  do {                                                                 \
    s_str *str_test;                                                   \
    s_str result;                                                      \
    str_test = (test);						       \
    assert(str_test);                                                  \
    test_context("str_inspect(" # test ") -> " # expected);            \
    TEST_EQ(str_inspect(str_test, &result), &result);                  \
    TEST_STRNCMP(result.ptr.p, (expected), result.size);               \
    str_clean(&result);                                                \
    str_delete(str_test);                                              \
    test_context(NULL);                                                \
  } while (0)

#define STR_TEST_INSPECT_1(test, result)                               \
  do {                                                                 \
    STR_TEST_INSPECT(str_new_1(NULL, (test)), (result));               \
  } while (0)

#define STR_TEST_TO_HEX(test, result)                                  \
  do {                                                                 \
    s_str str;                                                         \
    s_str *test_;                                                      \
    test_context("str_to_hex(" # test ") -> " # result);               \
    test_ = (test);                                                    \
    TEST_EQ(str_to_hex(test_, &str), &str);                            \
    TEST_STRNCMP(str.ptr.p, (result), str.size);                       \
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
    TEST_STR_CMP(&result->str, &str, 0);                               \
  } while (0)

void str_test_character_is_reserved ();
void str_test_compare ();
void str_test_init_clean ();
void str_test_init_dup ();
void str_test_init_dup_1 ();
void str_test_inspect ();
void str_test_new_1 ();
void str_test_new_cpy ();
void str_test_new_delete ();
void str_test_new_dup ();
void str_test_new_f ();
void str_test_to_hex ();
void str_test_to_ident ();
void str_test_to_sym ();


void str_test ()
{
  str_test_init_clean();
  str_test_init_dup();
  str_test_init_dup_1();
  str_test_new_delete();
  str_test_new_1();
  str_test_new_dup();
  str_test_compare();
  str_test_new_cpy();
  str_test_new_f();
  str_test_character_is_reserved();
  str_test_inspect();
  str_test_to_hex();
  str_test_to_sym();
}

void str_test_character_is_reserved ()
{
  TEST_ASSERT(str_character_is_reserved('\0'));
  TEST_ASSERT(str_character_is_reserved('\x01'));
  TEST_ASSERT(str_character_is_reserved('\x02'));
  TEST_ASSERT(str_character_is_reserved('\x03'));
  TEST_ASSERT(! str_character_is_reserved(' '));
  TEST_ASSERT(str_character_is_reserved('\n'));
  TEST_ASSERT(str_character_is_reserved('\r'));
  TEST_ASSERT(str_character_is_reserved('\t'));
  TEST_ASSERT(str_character_is_reserved('\v'));
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

void str_test_compare ()
{
  s_str *a;
  TEST_EQ((a = str_new_empty(), str_compare(a, a)), 0);
  str_delete(a);
  TEST_EQ((a = str_new_1(NULL, "abc"), str_compare(a, a)), 0);
  str_delete(a);
  STR_TEST_CMP(str_new_empty(), str_new_empty(), 0);
  STR_TEST_CMP(str_new_empty(), str_new_1(NULL, "0"), -1);
  STR_TEST_CMP(str_new_1(NULL, "0"), str_new_empty(), 1);
  STR_TEST_CMP(str_new_1(NULL, "0"), str_new_1(NULL, "0"), 0);
  STR_TEST_CMP(str_new_1(NULL, "0"), str_new_1(NULL, "A"), -1);
  STR_TEST_CMP(str_new_1(NULL, "01"), str_new_1(NULL, "0"), 1);
  STR_TEST_CMP(str_new_1(NULL, "01"), str_new_1(NULL, "01"), 0);
  STR_TEST_CMP(str_new_1(NULL, "01"), str_new_1(NULL, "012"), -1);
  STR_TEST_CMP(str_new_1(NULL, "01"), str_new_1(NULL, "02"), -1);
  STR_TEST_CMP(str_new_1(NULL, "01"), str_new_1(NULL, "023"), -1);
  STR_TEST_CMP(str_new_1(NULL, "01"), str_new_1(NULL, "ABC"), -1);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "0"), 1);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "01"), 1);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "012"), 0);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "0123"), -1);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "013"), -1);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "0134"), -1);
  STR_TEST_CMP(str_new_1(NULL, "012"), str_new_1(NULL, "ABC"), -1);
}

void str_test_init_clean ()
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

void str_test_init_dup ()
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

void str_test_init_dup_1 ()
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

void str_test_inspect ()
{
  s8 zero[16] = {0};
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
  STR_TEST_INSPECT_1("\xff", "\"\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff", "\"\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff", "\"\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff\xff", "\"\\xFF\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff\xff\xff", "\"\\xFF\\xFF\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff\xff\xff\xff",
                     "\"\\xFF\\xFF\\xFF\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff\xff\xff\xff\xff",
                     "\"\\xFF\\xFF\\xFF\\xFF\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff\xff\xff\xff\xff\xff",
                     "\"\\xFF\\xFF\\xFF\\xFF\\xFF\\xFF\\xFF\"");
  STR_TEST_INSPECT_1("\xff\xff\xff\xff\xff\xff\xff\xff",
                     "\"\\xFF\\xFF\\xFF\\xFF\\xFF\\xFF\\xFF\\xFF\"");
  STR_TEST_INSPECT(str_new(NULL,  1, zero), "\"\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  2, zero), "\"\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  3, zero), "\"\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  4, zero), "\"\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  5, zero), "\"\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  6, zero), "\"\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  7, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  8, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL,  9, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 10, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 11, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 12, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 13, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 14, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 15, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  STR_TEST_INSPECT(str_new(NULL, 16, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0"
                   "\\0\"");
}

void str_test_new_1 ()
{
  s_str *str;
  TEST_ASSERT((str = str_new_1(NULL, "test")));
  str_delete(str);
}

void str_test_new_cpy ()
{
  s_str *str;
  TEST_ASSERT((str = str_new_cpy(4, "test")));
  str_delete(str);
}

void str_test_new_delete ()
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

void str_test_new_dup ()
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

void str_test_new_f ()
{
  s_str *str;
  TEST_ASSERT((str = str_new_f("test%d", 42)));
  TEST_STRNCMP(str->ptr.p, "test42", str->size);
  str_delete(str);
  TEST_ASSERT((str = str_new_f("test%lld", (long long) 42)));
  TEST_STRNCMP(str->ptr.p, "test42", str->size);
  str_delete(str);
}

void str_test_to_hex ()
{
  s8 zero[32] = {0};
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

void str_test_to_sym ()
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
