/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3.h"
#include "test.h"

#define BUF_INSPECT_TEST_ARRAY(test, expected)                         \
  do {                                                                 \
    char b[1024];                                                      \
    s_buf buf_result;                                                  \
    s_pretty pretty = {0};                                             \
    s_array tmp;                                                       \
    test_context("buf_inspect_array(" # test ") -> " # expected);      \
    array_init_1(&tmp, (test));                                        \
    buf_init(&buf_result, false, sizeof(b), b);                        \
    TEST_EQ(buf_inspect_array_size(&pretty, &tmp), strlen(expected));  \
    TEST_EQ(buf_inspect_array(&buf_result, &tmp), strlen(expected));   \
    TEST_EQ(buf_result.wpos, strlen(expected));                        \
    TEST_STRNCMP(buf_result.ptr.pchar, (expected), buf_result.wpos);   \
    array_clean(&tmp);                                                 \
    buf_clean(&buf_result);                                            \
  } while (0)

#define BUF_INSPECT_TEST_BOOL(test, expected)                          \
  do {                                                                 \
    char b[16];                                                        \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    bool tmp;                                                          \
    test_context("buf_inspect_bool(" # test ") -> " # expected);       \
    buf_init(&buf, false, sizeof(b), b);                               \
    tmp = (test);                                                      \
    TEST_EQ(buf_inspect_bool_size(&pretty, tmp), strlen(expected));    \
    TEST_EQ(buf_inspect_bool(&buf, tmp), strlen(expected));            \
    TEST_STRNCMP(buf.ptr.p, (expected), buf.wpos);                     \
    test_context(NULL);                                                \
  } while (0)
  
#define BUF_INSPECT_TEST_CHARACTER(test, expected)                     \
  do {                                                                 \
    char b[32];                                                        \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    character tmp;                                                     \
    test_context("buf_inspect_character(" # test ") -> " # expected);  \
    buf_init(&buf, false, sizeof(b), b);                               \
    tmp = (test);                                                      \
    TEST_EQ(buf_inspect_character_size(&pretty, tmp),                  \
            strlen(expected));                                         \
    TEST_EQ(buf_inspect_character(&buf, tmp), strlen(expected));       \
    TEST_EQ(buf.wpos, strlen(expected));                               \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    test_context(NULL);                                                \
  } while (0)

#define BUF_INSPECT_TEST_F32(test, expected)                           \
  do {                                                                 \
    char b[32];                                                        \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    f32 tmp;                                                           \
    test_context("buf_inspect_f32(" # test ") -> " # expected);        \
    tmp = (test);                                                      \
    buf_init(&buf, false, sizeof(b), b);                               \
    buf_inspect_f32(&buf, tmp);                                        \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    TEST_EQ(buf.wpos, strlen(expected));                               \
    TEST_EQ(buf_inspect_f32_size(&pretty, tmp), strlen(expected));     \
    buf_init(&buf, false, sizeof(b), b);                               \
    TEST_EQ(buf_inspect_f32(&buf, tmp), strlen(expected));             \
    test_context(NULL);                                                \
  } while (0)

#define BUF_INSPECT_TEST_F64(test, expected)                           \
  do {                                                                 \
    char b[64];                                                        \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    f64 tmp;                                                           \
    test_context("buf_inspect_f64(" # test ") -> " # expected);        \
    tmp = (test);                                                      \
    buf_init(&buf, false, sizeof(b), b);                               \
    buf_inspect_f64(&buf, tmp);                                        \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    TEST_EQ(buf.wpos, strlen(expected));                               \
    TEST_EQ(buf_inspect_f64_size(&pretty, tmp), strlen(expected));     \
    buf_init(&buf, false, sizeof(b), b);                               \
    TEST_EQ(buf_inspect_f64(&buf, tmp), strlen(expected));             \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    test_context(NULL);                                                \
  } while (0)

#if HAVE_F80

#define BUF_INSPECT_TEST_F80(test, expected)                           \
  do {                                                                 \
    char b[80];                                                        \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    f80 tmp;                                                           \
    test_context("buf_inspect_f80(" # test ") -> " # expected);        \
    tmp = (test);                                                      \
    buf_init(&buf, false, sizeof(b), b);                               \
    buf_inspect_f80(&buf, tmp);                                        \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    TEST_EQ(buf.wpos, strlen(expected));                               \
    TEST_EQ(buf_inspect_f80_size(&pretty, tmp), strlen(expected));     \
    buf_init(&buf, false, sizeof(b), b);                               \
    TEST_EQ(buf_inspect_f80(&buf, tmp), strlen(expected));             \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    test_context(NULL);                                                \
  } while (0)

#endif

#define BUF_INSPECT_TEST_INTEGER(test, expected)                       \
  do {                                                                 \
    char b[1024];                                                      \
    s_buf buf_result;                                                  \
    s_pretty pretty = {0};                                             \
    s_integer i;                                                       \
    test_context("buf_inspect_integer(" # test ") -> " # expected);    \
    integer_init_1(&i, (test));                                        \
    buf_init(&buf_result, false, sizeof(b), b);                        \
    TEST_EQ(buf_inspect_integer_size(&pretty, &i), strlen(expected));  \
    TEST_EQ(buf_inspect_integer(&buf_result, &i), strlen(expected));   \
    integer_clean(&i);                                                 \
    TEST_EQ(buf_result.wpos, strlen(expected));                        \
    TEST_STRNCMP(buf_result.ptr.pchar, (expected), buf_result.wpos);   \
    buf_clean(&buf_result);                                            \
  } while (0)

#define BUF_INSPECT_TEST_LIST(test, expected)                          \
  do {                                                                 \
    s_buf buf;                                                         \
    s_list *list_test;                                                 \
    s_pretty pretty = {0};                                             \
    test_context("buf_inspect_list(" # test ") -> " # expected);       \
    list_test = list_new_1(test);                                      \
    buf_init_alloc(&buf, 1024 * 1024);                                 \
    TEST_EQ(buf_inspect_list_size(&pretty, list_test),                 \
            strlen(expected));                                         \
    TEST_EQ(buf_inspect_list(&buf, list_test), strlen(expected));      \
    TEST_EQ(buf.wpos, strlen(expected));                               \
    TEST_STRNCMP(buf.ptr.p, (expected), buf.wpos);                     \
    list_delete_all(list_test);                                        \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define BUF_INSPECT_TEST_STR(test, expected)                           \
  do {                                                                 \
    char b[1024];                                                      \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    s_str str;                                                         \
    test_context("buf_inspect_str(" # test ") -> " # expected);        \
    str_init(&str, NULL, sizeof(test) - 1, (test));                    \
    buf_init(&buf, false, sizeof(b), b);                               \
    TEST_MEMCMP(buf.ptr.p, (expected), buf.wpos);                      \
    TEST_EQ(buf_inspect_str_size(&pretty, &str), sizeof(expected) - 1);\
    TEST_EQ(buf_inspect_str(&buf, &str), sizeof(expected) - 1);        \
    test_context(NULL);                                                \
  } while (0)

#define BUF_INSPECT_TEST_STR_CHARACTER(test, expected)                 \
  do {                                                                 \
    char b[32];                                                        \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    character tmp;                                                     \
    test_context("buf_inspect_str_character(" # test ") -> "           \
                 # expected);                                          \
    buf_init(&buf, false, sizeof(b), b);                               \
    tmp = (test);                                                      \
    TEST_EQ(buf_inspect_str_character_size(&pretty, tmp),              \
            strlen(expected));                                         \
    TEST_EQ(buf_inspect_str_character(&buf, tmp), strlen(expected));   \
    TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);                 \
    test_context(NULL);                                                \
  } while (0)

#define BUF_INSPECT_TEST_TAG(test, expected)			       \
  do {                                                                 \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    s_tag *test_tag;                                                   \
    test_context("buf_inspect_tag(" # test ") -> " # expected);        \
    test_tag = (test);                                                 \
    buf_init_alloc(&buf, strlen(expected));                            \
    TEST_EQ(buf_inspect_tag_size(&pretty, test_tag),                   \
            strlen(expected));                                         \
    TEST_EQ(buf_inspect_tag(&buf, test_tag), strlen(expected));        \
    TEST_EQ(buf.wpos, strlen(expected));                               \
    if (g_test_last_ok)                                                \
      TEST_STRNCMP(buf.ptr.pchar, (expected), buf.wpos);               \
    buf_clean(&buf);                                                   \
  } while (0)

void buf_inspect_test (void);
TEST_CASE_PROTOTYPE(buf_inspect_array);
TEST_CASE_PROTOTYPE(buf_inspect_bool);
TEST_CASE_PROTOTYPE(buf_inspect_character);
TEST_CASE_PROTOTYPE(buf_inspect_f32);
TEST_CASE_PROTOTYPE(buf_inspect_f64);
#if HAVE_F80
TEST_CASE_PROTOTYPE(buf_inspect_f80);
#endif
#if HAVE_FLOAT128
TEST_CASE_PROTOTYPE(buf_inspect_f128);
#endif
TEST_CASE_PROTOTYPE(buf_inspect_integer);
TEST_CASE_PROTOTYPE(buf_inspect_list);
TEST_CASE_PROTOTYPE(buf_inspect_str);
TEST_CASE_PROTOTYPE(buf_inspect_str_character);
TEST_CASE_PROTOTYPE(buf_inspect_tag);

void buf_inspect_test (void)
{
  TEST_CASE_RUN(buf_inspect_array);
  TEST_CASE_RUN(buf_inspect_bool);
  TEST_CASE_RUN(buf_inspect_str_character);
  TEST_CASE_RUN(buf_inspect_character);
  TEST_CASE_RUN(buf_inspect_f32);
  TEST_CASE_RUN(buf_inspect_f64);
#if HAVE_F80
  TEST_CASE_RUN(buf_inspect_f80);
#endif
#if HAVE_FLOAT128
  TEST_CASE_RUN(buf_inspect_f128);
#endif
  TEST_CASE_RUN(buf_inspect_integer);
  TEST_CASE_RUN(buf_inspect_list);
  TEST_CASE_RUN(buf_inspect_str);
  TEST_CASE_RUN(buf_inspect_tag);
}

TEST_CASE(buf_inspect_array)
{
  BUF_INSPECT_TEST_ARRAY("(U8[]){0}",
                         "(U8[]) {0}");
  BUF_INSPECT_TEST_ARRAY("(U8[]){{0, 1}, {2, 3}}",
                         "(U8[]) {{0, 1},\n"
                         "        {2, 3}}");
  BUF_INSPECT_TEST_ARRAY("(U8[]){ { { 0 , 1 } , {2 , 3 } } , { { 4 , 5 } , { 6 , 7 } } }",
                         "(U8[]) {{{0, 1},\n"
                         "         {2, 3}},\n"
                         "        {{4, 5},\n"
                         "         {6, 7}}}");
}
TEST_CASE_END(buf_inspect_array)

TEST_CASE(buf_inspect_bool)
{
  BUF_INSPECT_TEST_BOOL(true, "true");
  BUF_INSPECT_TEST_BOOL(false, "false");
}
TEST_CASE_END(buf_inspect_bool)

TEST_CASE(buf_inspect_character)
{
  BUF_INSPECT_TEST_CHARACTER(0, "'\\0'");
  BUF_INSPECT_TEST_CHARACTER(1, "'\\x01'");
  BUF_INSPECT_TEST_CHARACTER(2, "'\\x02'");
  BUF_INSPECT_TEST_CHARACTER('0',    "'0'");
  BUF_INSPECT_TEST_CHARACTER('9',    "'9'");
  BUF_INSPECT_TEST_CHARACTER('A',    "'A'");
  BUF_INSPECT_TEST_CHARACTER('Z',    "'Z'");
  BUF_INSPECT_TEST_CHARACTER('a',    "'a'");
  BUF_INSPECT_TEST_CHARACTER('z',    "'z'");
  BUF_INSPECT_TEST_CHARACTER(928,    "'Π'");
  BUF_INSPECT_TEST_CHARACTER(0xFF,   "'ÿ'");
  BUF_INSPECT_TEST_CHARACTER(42164,  "'꒴'");
  BUF_INSPECT_TEST_CHARACTER(65856,  "'𐅀'");
  BUF_INSPECT_TEST_CHARACTER(127923, "'🎳'");
  BUF_INSPECT_TEST_CHARACTER(128516, "'😄'");
  BUF_INSPECT_TEST_CHARACTER(128995, "'🟣'");
  BUF_INSPECT_TEST_CHARACTER(129321, "'🤩'");
}
TEST_CASE_END(buf_inspect_character)

TEST_CASE(buf_inspect_f32)
{
  BUF_INSPECT_TEST_F32(0.0f, "(F32) 0.0");
  BUF_INSPECT_TEST_F32(0.1f, "(F32) 1.0e-1");
  BUF_INSPECT_TEST_F32(0.1234567f, "(F32) 1.234567e-1");
  BUF_INSPECT_TEST_F32(1.234567f, "(F32) 1.234567");
  BUF_INSPECT_TEST_F32(1234567.0f, "(F32) 1.234567e+6");
  BUF_INSPECT_TEST_F32(-0.1f, "(F32) -1.0e-1");
  BUF_INSPECT_TEST_F32(-0.1234567f, "(F32) -1.234567e-1");
  BUF_INSPECT_TEST_F32(-1.234567f, "(F32) -1.234567");
  BUF_INSPECT_TEST_F32(-1234567.0f, "(F32) -1.234567e+6");
}
TEST_CASE_END(buf_inspect_f32)

TEST_CASE(buf_inspect_f64)
{
  BUF_INSPECT_TEST_F64(0.0, "(F64) 0.0");
  BUF_INSPECT_TEST_F64(0.1, "(F64) 1.0e-1");
  BUF_INSPECT_TEST_F64(0.123456789, "(F64) 1.23456788999999e-1");
  BUF_INSPECT_TEST_F64(1.23456789, "(F64) 1.23456788999999");
  BUF_INSPECT_TEST_F64(123456789.0, "(F64) 1.23456789e+8");
  BUF_INSPECT_TEST_F64(-0.1, "(F64) -1.0e-1");
  BUF_INSPECT_TEST_F64(-0.123456789, "(F64) -1.23456788999999e-1");
  BUF_INSPECT_TEST_F64(-1.23456789, "(F64) -1.23456788999999");
  BUF_INSPECT_TEST_F64(-123456789.0, "(F64) -1.23456789e+8");
}
TEST_CASE_END(buf_inspect_f64)

#if HAVE_F80

TEST_CASE(buf_inspect_f80)
{
  BUF_INSPECT_TEST_F80(0.0, "(F80) 0.0");
  BUF_INSPECT_TEST_F80(0.1, "(F80) 1.0e-1");
  BUF_INSPECT_TEST_F80(0.123456789, "(F80) 1.23456788999999e-1");
  BUF_INSPECT_TEST_F80(1.23456789, "(F80) 1.23456788999999");
  BUF_INSPECT_TEST_F80(123456789.0, "(F80) 1.23456789e+8");
  BUF_INSPECT_TEST_F80(-0.1, "(F80) -1.0e-1");
  BUF_INSPECT_TEST_F80(-0.123456789, "(F80) -1.23456788999999e-1");
  BUF_INSPECT_TEST_F80(-1.23456789, "(F80) -1.23456788999999");
  BUF_INSPECT_TEST_F80(-123456789.0, "(F80) -1.23456789e+8");
}
TEST_CASE_END(buf_inspect_f80)

#endif

TEST_CASE(buf_inspect_integer)
{
  BUF_INSPECT_TEST_INTEGER("0", "(Integer) 0");
  BUF_INSPECT_TEST_INTEGER("1234", "(Integer) 1234");
  BUF_INSPECT_TEST_INTEGER("-1234", "(Integer) -1234");
  BUF_INSPECT_TEST_INTEGER("12345678901234567890123456789013",
			   "12345678901234567890123456789013");
  BUF_INSPECT_TEST_INTEGER("-1234567890123456789012345678901",
			   "-1234567890123456789012345678901");
  BUF_INSPECT_TEST_INTEGER("12345678901234567890123456789012332132131",
			   "12345678901234567890123456789012332132131");
  BUF_INSPECT_TEST_INTEGER("12345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131",
			   "12345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131");
  BUF_INSPECT_TEST_INTEGER("-12345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131",
			   "-12345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131");
  BUF_INSPECT_TEST_INTEGER("123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131",
			   "123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131123456789012345678901234567890123321321311234567890123456789012345678901233213213112345678901234567890123456789012332132131");
}
TEST_CASE_END(buf_inspect_integer)

TEST_CASE(buf_inspect_list)
{
  BUF_INSPECT_TEST_LIST("[]", "[]");
  BUF_INSPECT_TEST_LIST("[[], []]", "[[], []]");
  BUF_INSPECT_TEST_LIST("[[] | []]", "[[]]");
}
TEST_CASE_END(buf_inspect_list)
 
TEST_CASE(buf_inspect_str)
{
  BUF_INSPECT_TEST_STR("", "\"\"");
  BUF_INSPECT_TEST_STR(" ", "\" \"");
  BUF_INSPECT_TEST_STR("\n", "\"\\n\"");
  BUF_INSPECT_TEST_STR("\r", "\"\\r\"");
  BUF_INSPECT_TEST_STR("\t", "\"\\t\"");
  BUF_INSPECT_TEST_STR("\v", "\"\\v\"");
  BUF_INSPECT_TEST_STR("\"", "\"\\\"\"");
  BUF_INSPECT_TEST_STR("\x00", "\"\\0\"");
  // {
  //   char b[1024];
  //   s_buf buf;
  //   s_pretty pretty = {0};
  //   s_str str;
  //   test_context("buf_inspect_str(\"\\0\") -> \"\\0\"");
  //   str_init(&str, NULL, 1, "\0");
  //   buf_init(&buf, false, sizeof(b), b);
  //   TEST_EQ(buf_inspect_str_size(&pretty, &str), strlen("\"\\0\""));
  //   TEST_EQ(buf_inspect_str(&buf, &str), strlen("\"\\0\""));
  //   TEST_STRNCMP(buf.ptr.p, "\"\\0\"", buf.wpos);
  //   test_context(NULL);
  // }
  BUF_INSPECT_TEST_STR("\x01", "\"\\x01\"");
  BUF_INSPECT_TEST_STR("\x02", "\"\\x02\"");
  BUF_INSPECT_TEST_STR("\x03", "\"\\x03\"");
  BUF_INSPECT_TEST_STR("\x04", "\"\\x04\"");
  BUF_INSPECT_TEST_STR("\x05", "\"\\x05\"");
  BUF_INSPECT_TEST_STR("\x06", "\"\\x06\"");
  BUF_INSPECT_TEST_STR(".", "\".\"");
  BUF_INSPECT_TEST_STR("..", "\"..\"");
  BUF_INSPECT_TEST_STR("...", "\"...\"");
  BUF_INSPECT_TEST_STR(".. .", "\".. .\"");
  BUF_INSPECT_TEST_STR("t", "\"t\"");
  BUF_INSPECT_TEST_STR("T", "\"T\"");
  BUF_INSPECT_TEST_STR("test", "\"test\"");
  BUF_INSPECT_TEST_STR("Test", "\"Test\"");
  BUF_INSPECT_TEST_STR("123", "\"123\"");
  BUF_INSPECT_TEST_STR("test123", "\"test123\"");
  BUF_INSPECT_TEST_STR("Test123", "\"Test123\"");
  BUF_INSPECT_TEST_STR("test 123", "\"test 123\"");
  BUF_INSPECT_TEST_STR("Test 123", "\"Test 123\"");
  BUF_INSPECT_TEST_STR("test123.test456", "\"test123.test456\"");
  BUF_INSPECT_TEST_STR("Test123.Test456", "\"Test123.Test456\"");
  BUF_INSPECT_TEST_STR("test123(test456)", "\"test123(test456)\"");
  BUF_INSPECT_TEST_STR("Test123(Test456)", "\"Test123(Test456)\"");
  BUF_INSPECT_TEST_STR("test123{test456}", "\"test123{test456}\"");
  BUF_INSPECT_TEST_STR("Test123{Test456}", "\"Test123{Test456}\"");
  BUF_INSPECT_TEST_STR("É", "\"É\"");
  BUF_INSPECT_TEST_STR("Éo", "\"Éo\"");
  BUF_INSPECT_TEST_STR("Éoà \n\r\t\v\"",
                       "\"Éoà \\n\\r\\t\\v\\\"\"");
  BUF_INSPECT_TEST_STR("éoà \n\r\t\v\"",
                       "\"éoà \\n\\r\\t\\v\\\"\"");
  BUF_INSPECT_TEST_STR("é", "\"é\"");
  BUF_INSPECT_TEST_STR("éoπꝝ꒴𐅀🎳😄🟣🤩", "\"éoπꝝ꒴𐅀🎳😄🟣🤩\"");
}
TEST_CASE_END(buf_inspect_str)

TEST_CASE(buf_inspect_str_character)
{
  BUF_INSPECT_TEST_STR_CHARACTER(0, "\\0");
  BUF_INSPECT_TEST_STR_CHARACTER(1, "\\x01");
  BUF_INSPECT_TEST_STR_CHARACTER(2, "\\x02");
  BUF_INSPECT_TEST_STR_CHARACTER(10, "\\n");
  BUF_INSPECT_TEST_STR_CHARACTER('0', "0");
  BUF_INSPECT_TEST_STR_CHARACTER('9', "9");
  BUF_INSPECT_TEST_STR_CHARACTER('A', "A");
  BUF_INSPECT_TEST_STR_CHARACTER('Z', "Z");
  BUF_INSPECT_TEST_STR_CHARACTER('a', "a");
  BUF_INSPECT_TEST_STR_CHARACTER('z', "z");
  BUF_INSPECT_TEST_STR_CHARACTER(928, "Π");
  BUF_INSPECT_TEST_STR_CHARACTER(0xFF, "ÿ");
  BUF_INSPECT_TEST_STR_CHARACTER(42164, "꒴");
  BUF_INSPECT_TEST_STR_CHARACTER(65856, "𐅀");
  BUF_INSPECT_TEST_STR_CHARACTER(127923, "🎳");
  BUF_INSPECT_TEST_STR_CHARACTER(128516, "😄");
  BUF_INSPECT_TEST_STR_CHARACTER(128995, "🟣");
  BUF_INSPECT_TEST_STR_CHARACTER(129321, "🤩");
}
TEST_CASE_END(buf_inspect_str_character)

TEST_CASE(buf_inspect_tag)
{
  s_tag tag;
  tag_init(&tag);
  BUF_INSPECT_TEST_TAG(tag_bool(&tag, false), "false");
  BUF_INSPECT_TEST_TAG(tag_bool(&tag, true), "true");
  BUF_INSPECT_TEST_TAG(tag_character(&tag, '\n'), "'\\n'");
  BUF_INSPECT_TEST_TAG(tag_f32(&tag, 1.0f), "(F32) 1.0");
  BUF_INSPECT_TEST_TAG(tag_f64(&tag, 1.0), "(F64) 1.0");
#if HAVE_F80
  BUF_INSPECT_TEST_TAG(tag_f80(&tag, 1.0), "(F80) 1.0");
#endif
  BUF_INSPECT_TEST_TAG(tag_ident_1(&tag, "ident"), "ident");
  BUF_INSPECT_TEST_TAG(tag_integer_1(&tag, "-0x10000000000000000"), "-18446744073709551616");
  BUF_INSPECT_TEST_TAG(tag_integer_1(&tag, "0x10000000000000000"), "18446744073709551616");
  BUF_INSPECT_TEST_TAG(tag_plist(&tag, NULL), "[]");
  BUF_INSPECT_TEST_TAG(tag_plist_1(&tag, "[[] | []]"), "[[]]");
  BUF_INSPECT_TEST_TAG(tag_plist_1(&tag, "[]"), "[]");
  BUF_INSPECT_TEST_TAG(tag_psym(&tag, sym_1("Module")), "Module");
  BUF_INSPECT_TEST_TAG(tag_s16(&tag, -0x100), "-256");
  BUF_INSPECT_TEST_TAG(tag_s32(&tag, -0x10000), "-65536");
  BUF_INSPECT_TEST_TAG(tag_s64(&tag, -0x100000000), "-4294967296");
  BUF_INSPECT_TEST_TAG(tag_s8(&tag, -1), "-1");
  BUF_INSPECT_TEST_TAG(tag_s8(&tag, -10), "-10");
  BUF_INSPECT_TEST_TAG(tag_str(&tag, NULL, 0, ""), "\"\"");
  BUF_INSPECT_TEST_TAG(tag_str_1(&tag, NULL, ""), "\"\"");
  BUF_INSPECT_TEST_TAG(tag_str_1(&tag, NULL, "09AZaz"), "\"09AZaz\"");
  BUF_INSPECT_TEST_TAG(tag_u16(&tag, 0x100), "256");
  BUF_INSPECT_TEST_TAG(tag_u32(&tag, 0x10000), "65536");
  BUF_INSPECT_TEST_TAG(tag_u64(&tag, 0x100000000), "4294967296");
  BUF_INSPECT_TEST_TAG(tag_u8(&tag, 0), "0");
  BUF_INSPECT_TEST_TAG(tag_u8(&tag, 1), "1");
  BUF_INSPECT_TEST_TAG(tag_u8(&tag, 10), "10");
  BUF_INSPECT_TEST_TAG(tag_void(&tag), "void");
  tag_clean(&tag);
}
TEST_CASE_END(buf_inspect_tag)
