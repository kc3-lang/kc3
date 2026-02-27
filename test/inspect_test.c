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
#include <assert.h>
#include <string.h>
#include "../libkc3/array.h"
#include "../libkc3/buf_inspect.h"
#include "../libkc3/fact.h"
#include "../libkc3/ident.h"
#include "../libkc3/inspect.h"
#include "../libkc3/integer.h"
#include "../libkc3/list.h"
#include "../libkc3/pcall.h"
#include "../libkc3/pstruct.h"
#include "../libkc3/pvar.h"
#include "../libkc3/ratio.h"
#include "../libkc3/str.h"
#include "../libkc3/struct.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "../libkc3/tuple.h"
#include "../libkc3/var.h"
#include "test.h"

#define INSPECT_TEST_ARRAY(test, expected)                             \
  do {                                                                 \
    s_str str;                                                         \
    s_array tmp;                                                       \
    test_context("inspect_array(" # test ") -> " # expected);          \
    TEST_EQ(array_init_1(&tmp, (test)), &tmp);                         \
    TEST_EQ(inspect_array(&tmp, &str), &str);                          \
    TEST_STRNCMP(str.ptr.p_pvoid, (expected), str.size);                     \
    array_clean(&tmp);                                                 \
    str_clean(&str);						       \
  } while(0)

#define INSPECT_TEST_BOOL(test, expected)                              \
  do {                                                                 \
    s_str str;                                                         \
    bool tmp;                                                          \
    test_context("inspect_bool(" # test ") -> " # expected);           \
    tmp = (test);                                                      \
    TEST_EQ(inspect_bool(tmp, &str), &str);                           \
    TEST_EQ(str.size, strlen(expected));                               \
    TEST_STRNCMP(str.ptr.p_pvoid, (expected), str.size);                     \
    str_clean(&str);						       \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_CALL(test)                                       \
  do {                                                                 \
    p_call pcall;                                                      \
    s_str result;                                                      \
    TEST_EQ(pcall_init_1(&pcall, (test)), &pcall);                     \
    TEST_EQ(inspect_call(pcall, &result), &result);                    \
    TEST_STRNCMP(result.ptr.p_pvoid, (test), result.size);                   \
    TEST_EQ(result.size, strlen(test));                                \
    pcall_clean(&pcall);                                               \
    str_clean(&result);						       \
  } while (0)

#define INSPECT_TEST_FACT(test, expected)                              \
  do {                                                                 \
    s_str str;                                                         \
    TEST_EQ(inspect_fact(test, &str), &str);                           \
    TEST_EQ(str.size, strlen(expected));                               \
    if (g_test_last_ok)						       \
      TEST_STRNCMP(str.ptr.p_pvoid, (expected), str.size);		       \
    str_clean(&str);						       \
  } while (0)

#define INSPECT_TEST_IDENT(test, result)                               \
  do {                                                                 \
    s_ident ident;                                                     \
    s_str str;                                                         \
    assert(test);                                                      \
    assert(result);                                                    \
    test_context("inspect_ident(" #test ") -> " #result);              \
    ident_init_1(&ident, (test));                                      \
    TEST_EQ(inspect_ident(&ident, &str), &str);                        \
    TEST_STRNCMP(str.ptr.p_pvoid, (result), str.size);                       \
    str_clean(&str);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_LIST(test, expected)                              \
  do {                                                                 \
    s_list *list_test;                                                 \
    s_str str_result;                                                  \
    test_context("inspect_list(" # test ") -> " # expected);           \
    list_test = list_new_1(test);                                      \
    TEST_EQ(inspect_list(list_test, &str_result), &str_result);        \
    TEST_STRNCMP(str_result.ptr.p_pvoid, (expected), str_result.size);       \
    str_clean(&str_result);                                            \
    list_delete_all(list_test);                                        \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_PVAR(test, expected)                              \
  do {                                                                 \
    p_var pvar_test;                                                   \
    s_str str_result;                                                  \
    test_context("inspect_var(" # test ") -> " # expected);            \
    TEST_EQ(pvar_init_1(&pvar_test, (test)), &pvar_test);              \
    TEST_EQ(inspect_var(pvar_test, &str_result), &str_result);        \
    TEST_STRNCMP(str_result.ptr.p_pvoid, (expected), str_result.size);       \
    TEST_EQ(str_result.size, strlen(expected));                        \
    str_clean(&str_result);                                            \
    pvar_clean(&pvar_test);                                            \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_RATIO(test_num, test_den, expected)               \
  do {                                                                 \
  s_ratio ratio;                                                       \
  s_str result;                                                        \
  integer_init_u64(&ratio.numerator, test_num);                        \
  integer_init_u64(&ratio.denominator, test_den);                      \
  test_context("inspect_ratio(" # test_num "/" # test_den              \
               ") -> " # expected);                                    \
  TEST_EQ(inspect_ratio(&ratio, &result), &result);                    \
  TEST_STRNCMP(result.ptr.p_pvoid, expected, result.size);                   \
  ratio_clean(&ratio);                                                 \
  str_clean(&result);                                                  \
  test_context(NULL);                                                  \
} while (0)

#define INSPECT_TEST_STR(test, expected)                               \
  do {                                                                 \
    s_str result;                                                      \
    assert(test);                                                      \
    test_context("inspect_str(" # test ") -> " # expected);            \
    TEST_EQ(inspect_str((test), &result), &result);                    \
    TEST_STRNCMP(result.ptr.p_pvoid, (expected), result.size);               \
    str_clean(&result);                                                \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_STR_1(test, expected)                             \
  do {                                                                 \
    s_str result;                                                      \
    s_str str;                                                         \
    str_init_1(&str, NULL, (test));                                    \
    test_context("inspect_str(" # test ") -> " # expected);            \
    TEST_EQ(inspect_str(&str, &result), &result);                      \
    TEST_STRNCMP(result.ptr.p_pvoid, (expected), result.size);               \
    str_clean(&result);                                                \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_STRUCT(test, expected)                            \
  do {                                                                 \
    s_pretty pretty = {0};                                             \
    s_str result;                                                      \
    p_struct struct_test = NULL;                                       \
    assert(test);                                                      \
    test_context("inspect_struct(" # test ") -> " # expected);         \
    TEST_EQ(pstruct_init_1(&struct_test, (test)), &struct_test);       \
    TEST_EQ(inspect_struct(struct_test, &result), &result);            \
    TEST_STRNCMP(result.ptr.p_pvoid, (expected), result.size);               \
    TEST_EQ(buf_inspect_struct_size(&pretty, struct_test),             \
            strlen(expected));                                         \
    str_clean(&result);                                                \
    pstruct_clean(&struct_test);                                       \
    TEST_ASSERT(! struct_test);                                        \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_SYM(test, result)                                 \
  do {                                                                 \
    const s_sym *sym;                                                  \
    s_str str;                                                         \
    assert(test);                                                      \
    assert(result);                                                    \
    test_context("inspect_sym(" # test ") -> " # result);              \
    sym = sym_1(test);                                                 \
    TEST_EQ(inspect_sym(sym, &str), &str);                             \
    TEST_STRNCMP(str.ptr.p_pvoid, (result), str.size);                       \
    str_clean(&str);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_TAG(test, expected)                               \
  do {                                                                 \
    s_tag tag_test;                                                    \
    s_str str_result;                                                  \
    test_context("inspect_tag(" # test ") -> " # expected);            \
    tag_init_1(&tag_test, (test));                                     \
    TEST_EQ(inspect_tag(&tag_test, &str_result), &str_result);         \
    tag_clean(&tag_test);                                              \
    TEST_STRNCMP(str_result.ptr.p_pvoid, (expected), str_result.size);       \
    TEST_EQ(str_result.size, strlen(expected));                        \
    str_clean(&str_result);                                            \
    test_context(NULL);                                                \
  } while (0)

#define INSPECT_TEST_TUPLE(test, expected)                             \
  do {                                                                 \
    s_tuple tuple_test;                                                \
    s_str str_result;                                                  \
    test_context("inspect_tuple(" # test ") -> " # expected);          \
    tuple_init_1(&tuple_test, (test));                                 \
    TEST_EQ(inspect_tuple(&tuple_test, &str_result), &str_result);     \
    tuple_clean(&tuple_test);                                          \
    TEST_EQ(str_result.size, strlen(expected));                        \
    TEST_STRNCMP(str_result.ptr.p_pvoid, (expected), str_result.size);       \
    str_clean(&str_result);                                            \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(inspect_array);
TEST_CASE_PROTOTYPE(inspect_bool);
TEST_CASE_PROTOTYPE(inspect_call);
TEST_CASE_PROTOTYPE(inspect_fact);
TEST_CASE_PROTOTYPE(inspect_ident);
TEST_CASE_PROTOTYPE(inspect_list);
TEST_CASE_PROTOTYPE(inspect_pvar);
TEST_CASE_PROTOTYPE(inspect_ratio);
TEST_CASE_PROTOTYPE(inspect_str);
TEST_CASE_PROTOTYPE(inspect_struct);
TEST_CASE_PROTOTYPE(inspect_sym);
TEST_CASE_PROTOTYPE(inspect_tag);
TEST_CASE_PROTOTYPE(inspect_tuple);

void inspect_test (void)
{
  TEST_CASE_RUN(inspect_array);
  TEST_CASE_RUN(inspect_bool);
  TEST_CASE_RUN(inspect_call);
  TEST_CASE_RUN(inspect_fact);
  TEST_CASE_RUN(inspect_ident);
  TEST_CASE_RUN(inspect_list);
  TEST_CASE_RUN(inspect_ratio);
  TEST_CASE_RUN(inspect_str);
  TEST_CASE_RUN(inspect_struct);
  TEST_CASE_RUN(inspect_sym);
  TEST_CASE_RUN(inspect_tag);
  TEST_CASE_RUN(inspect_tuple);
  TEST_CASE_RUN(inspect_pvar);
}

TEST_CASE(inspect_array)
{
  INSPECT_TEST_ARRAY("(U8[]) {0}",
                     "(U8[]) {0}");
  INSPECT_TEST_ARRAY("(U8[]) {0, 0}",
                     "(U8[]) {0, 0}");
  INSPECT_TEST_ARRAY("(U8[]) {0, 0, 0}",
                     "(U8[]) {0, 0, 0}");
  INSPECT_TEST_ARRAY("(U8[]) {{0}, {0}}",
                     "(U8[]) {{0},\n"
                     "        {0}}");
  INSPECT_TEST_ARRAY("(U8[]) {{0, 0}, {0, 0}}",
                     "(U8[]) {{0, 0},\n"
                     "        {0, 0}}");
  INSPECT_TEST_ARRAY("(U8[]) {{0, 0}, {0, 0}, {0, 0}}",
                     "(U8[]) {{0, 0},\n"
                     "        {0, 0},\n"
                     "        {0, 0}}");
  INSPECT_TEST_ARRAY("(U8[]) {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}},"
                     " {{0, 0}, {0, 0}}}",
                     "(U8[]) {{{0, 0},\n"
                     "         {0, 0}},\n"
                     "        {{0, 0},\n"
                     "         {0, 0}},\n"
                     "        {{0, 0},\n"
                     "         {0, 0}}}");
  INSPECT_TEST_ARRAY("(U8[]) {1, 2, 3}",
                     "(U8[]) {1, 2, 3}");
  INSPECT_TEST_ARRAY("(U8[]) {1 + 1, 2 + 2, 3 + 3}",
                     "(U8[]) {2, 4, 6}");
  INSPECT_TEST_ARRAY("(U8[]) {255 + 1, 255 + 2, 255 + 3}",
                     "(U8[]) {0, 1, 2}");
  INSPECT_TEST_ARRAY("(U16[]) {255 + 1, 255 + 2, 255 + 3}",
                     "(U16[]) {256, 257, 258}");
  INSPECT_TEST_ARRAY("(U16[]) {65535 + 1, 65535 + 2, 65535 + 3}",
                     "(U16[]) {0, 1, 2}");
  INSPECT_TEST_ARRAY("(U32[]) {65535 + 1, 65535 + 2, 65535 + 3}",
                     "(U32[]) {65536, 65537, 65538}");
  INSPECT_TEST_ARRAY("(U32[]) {4294967295 + 1, 4294967295 + 2,"
                     " 4294967295 + 3}",
                     "(U32[]) {0, 1, 2}");
  INSPECT_TEST_ARRAY("(U64[]) {4294967295 + 1, 4294967295 + 2,"
                     " 4294967295 + 3}",
                     "(U64[]) {4294967296, 4294967297, 4294967298}");
  INSPECT_TEST_ARRAY("(U64[]) {18446744073709551615 + 1,"
                     " 18446744073709551615 + 2,"
                     " 18446744073709551615 + 3}",
                     "(U64[]) {0, 1, 2}");
  INSPECT_TEST_ARRAY("(Integer[]) {18446744073709551615 + 1,"
                     " 18446744073709551615 + 2,"
                     " 18446744073709551615 + 3}",
                     "(Integer[]) {18446744073709551616,"
                     " 18446744073709551617,"
                     " 18446744073709551618}");
}
TEST_CASE_END(inspect_array)

TEST_CASE(inspect_bool)
{
  INSPECT_TEST_BOOL(true, "true");
  INSPECT_TEST_BOOL(false, "false");
}
TEST_CASE_END(inspect_bool)

TEST_CASE(inspect_call)
{
  INSPECT_TEST_CALL("a()");
  INSPECT_TEST_CALL("a(b)");
  INSPECT_TEST_CALL("a(b, c)");
  INSPECT_TEST_CALL("a(b, c, d)");
  INSPECT_TEST_CALL("A.b()");
  INSPECT_TEST_CALL("A.b(c)");
  INSPECT_TEST_CALL("A.b(c, d)");
  INSPECT_TEST_CALL("A.b(c, d, e)");
}
TEST_CASE_END(inspect_call)

TEST_CASE(inspect_fact)
{
  s_tag tag[3];
  s_fact fact;
  tag_init_1(tag, "A");
  tag_init_1(tag + 1, "B");
  tag_init_1(tag + 2, "C");
  fact_init(&fact, tag, tag + 1, tag + 2);
  INSPECT_TEST_FACT(&fact, "{A, B, C}");
}
TEST_CASE_END(inspect_fact)

TEST_CASE(inspect_ident)
{
  INSPECT_TEST_IDENT("", "_\"\"");
  INSPECT_TEST_IDENT(" ", "_\" \"");
  INSPECT_TEST_IDENT("\n", "_\"\\n\"");
  INSPECT_TEST_IDENT("\r", "_\"\\r\"");
  INSPECT_TEST_IDENT("\t", "_\"\\t\"");
  INSPECT_TEST_IDENT("\v", "_\"\\v\"");
  INSPECT_TEST_IDENT("\"", "_\"\\\"\"");
  INSPECT_TEST_IDENT(".", ".");
  INSPECT_TEST_IDENT("..", "_\"..\"");
  INSPECT_TEST_IDENT("...", "_\"...\"");
  INSPECT_TEST_IDENT(".. .", "_\".. .\"");
  INSPECT_TEST_IDENT("t", "t");
  INSPECT_TEST_IDENT("T", "_\"T\"");
  INSPECT_TEST_IDENT("test", "test");
  INSPECT_TEST_IDENT("Test", "_\"Test\"");
  INSPECT_TEST_IDENT("123", "_\"123\"");
  INSPECT_TEST_IDENT("test123", "test123");
  INSPECT_TEST_IDENT("Test123", "_\"Test123\"");
  INSPECT_TEST_IDENT("test 123", "_\"test 123\"");
  INSPECT_TEST_IDENT("Test 123", "_\"Test 123\"");
  INSPECT_TEST_IDENT("test123.test456", "_\"test123.test456\"");
  INSPECT_TEST_IDENT("Test123.Test456", "_\"Test123.Test456\"");
  INSPECT_TEST_IDENT("test123(test456)", "_\"test123(test456)\"");
  INSPECT_TEST_IDENT("Test123(Test456)", "_\"Test123(Test456)\"");
  INSPECT_TEST_IDENT("test123{test456}", "_\"test123{test456}\"");
  INSPECT_TEST_IDENT("Test123{Test456}", "_\"Test123{Test456}\"");
  INSPECT_TEST_IDENT("É", "_\"É\"");
  INSPECT_TEST_IDENT("Éo", "_\"Éo\"");
  INSPECT_TEST_IDENT("Éoà \n\r\t\v\"",
                     "_\"Éoà \\n\\r\\t\\v\\\"\"");
  INSPECT_TEST_IDENT("éoà \n\r\t\v\"",
                     "_\"éoà \\n\\r\\t\\v\\\"\"");
  INSPECT_TEST_IDENT("é", "é");
  INSPECT_TEST_IDENT("éoπꝝ꒴", "éoπꝝ꒴");
}
TEST_CASE_END(inspect_ident)

TEST_CASE(inspect_list)
{
  INSPECT_TEST_LIST("[]", "[]");
  INSPECT_TEST_LIST("[[] | []]", "[[]]");
  INSPECT_TEST_LIST("[[], [] | []]", "[[],\n []]");
}
TEST_CASE_END(inspect_list)

TEST_CASE(inspect_pvar)
{
  INSPECT_TEST_PVAR("?", "?");
  INSPECT_TEST_PVAR("(U8) ?", "(U8) ?");
}
TEST_CASE_END(inspect_pvar)

TEST_CASE(inspect_ratio)
{
    INSPECT_TEST_RATIO(0, 1, "0/1");
    INSPECT_TEST_RATIO(1, 1, "1/1");
    INSPECT_TEST_RATIO(1, 10, "1/10");
    INSPECT_TEST_RATIO(1, 100, "1/100");
    INSPECT_TEST_RATIO(1, 1000, "1/1000");
    INSPECT_TEST_RATIO(10, 1, "10/1");
    INSPECT_TEST_RATIO(100, 1, "100/1");
    INSPECT_TEST_RATIO(1000, 1, "1000/1");
}
TEST_CASE_END(inspect_ratio)

TEST_CASE(inspect_str)
{
  s_str str;
  char zero[16] = {0};
  INSPECT_TEST_STR_1("", "\"\"");
  INSPECT_TEST_STR_1(" ", "\" \"");
  INSPECT_TEST_STR_1("\n", "\"\\n\"");
  INSPECT_TEST_STR_1("\r", "\"\\r\"");
  INSPECT_TEST_STR_1("\t", "\"\\t\"");
  INSPECT_TEST_STR_1("\v", "\"\\v\"");
  INSPECT_TEST_STR_1("\"", "\"\\\"\"");
  INSPECT_TEST_STR_1("\\", "\"\\\\\"");
  INSPECT_TEST_STR_1(".", "\".\"");
  INSPECT_TEST_STR_1("..", "\"..\"");
  INSPECT_TEST_STR_1("...", "\"...\"");
  INSPECT_TEST_STR_1(".. .", "\".. .\"");
  INSPECT_TEST_STR_1("t", "\"t\"");
  INSPECT_TEST_STR_1("T", "\"T\"");
  INSPECT_TEST_STR_1("test", "\"test\"");
  INSPECT_TEST_STR_1("Test", "\"Test\"");
  INSPECT_TEST_STR_1("123", "\"123\"");
  INSPECT_TEST_STR_1("test123", "\"test123\"");
  INSPECT_TEST_STR_1("Test123", "\"Test123\"");
  INSPECT_TEST_STR_1("test 123", "\"test 123\"");
  INSPECT_TEST_STR_1("Test 123", "\"Test 123\"");
  INSPECT_TEST_STR_1("test123.test456", "\"test123.test456\"");
  INSPECT_TEST_STR_1("Test123.Test456", "\"Test123.Test456\"");
  INSPECT_TEST_STR_1("É", "\"É\"");
  INSPECT_TEST_STR_1("Éo", "\"Éo\"");
  INSPECT_TEST_STR_1("Éoà \n\r\t\v\"",
                     "\"Éoà \\n\\r\\t\\v\\\"\"");
  INSPECT_TEST_STR_1("é", "\"é\"");
  INSPECT_TEST_STR_1("éo", "\"éo\"");
  INSPECT_TEST_STR_1("éoà \n\r\t\v\"",
                     "\"éoà \\n\\r\\t\\v\\\"\"");
  INSPECT_TEST_STR_1("Π", "\"Π\"");
  INSPECT_TEST_STR_1("꒴", "\"꒴\"");
  INSPECT_TEST_STR_1("𐅀", "\"𐅀\"");
  INSPECT_TEST_STR_1("ÉoàΠ꒴𐅀 \n\r\t\v\\\"",
                     "\"ÉoàΠ꒴𐅀 \\n\\r\\t\\v\\\\\\\"\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  1, zero), "\"\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  2, zero), "\"\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  3, zero), "\"\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  4, zero), "\"\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  5, zero),
                   "\"\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  6, zero),
                   "\"\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  7, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  8, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL,  9, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 10, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 11, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 12, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 13, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 14, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 15, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 16, zero),
                   "\"\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0"
                   "\\0\"");
  INSPECT_TEST_STR(str_init(&str, NULL, 1, "\xFF"), "\"\\xFF\"");
}
TEST_CASE_END(inspect_str)

TEST_CASE(inspect_struct)
{
  INSPECT_TEST_STRUCT("%KC3.Op{callable: fn (a, b) { a + b }}",
                      "%KC3.Op{sym: :+,\n"
                      "        arity: 2,\n"
                      "        special: false,\n"
                      "        precedence: 0,\n"
                      "        associativity: 1,\n"
                      "        callable: fn (a, b) { a + b }}");
  INSPECT_TEST_STRUCT("%KC3.Op{sym: :-, callable: fn (a, b) { a + b }}",
                      "%KC3.Op{sym: :-,\n"
                      "        arity: 2,\n"
                      "        special: false,\n"
                      "        precedence: 0,\n"
                      "        associativity: 1,\n"
                      "        callable: fn (a, b) { a + b }}");
}
TEST_CASE_END(inspect_struct)

TEST_CASE(inspect_sym)
{
  INSPECT_TEST_SYM("", ":\"\"");
  INSPECT_TEST_SYM(" ", ":\" \"");
  INSPECT_TEST_SYM("\n", ":\"\\n\"");
  INSPECT_TEST_SYM("\r", ":\"\\r\"");
  INSPECT_TEST_SYM("\t", ":\"\\t\"");
  INSPECT_TEST_SYM("\v", ":\"\\v\"");
  INSPECT_TEST_SYM("\"", ":\"\\\"\"");
  INSPECT_TEST_SYM(".", ":\".\"");
  INSPECT_TEST_SYM("..", ":\"..\"");
  INSPECT_TEST_SYM("...", ":\"...\"");
  INSPECT_TEST_SYM(".. .", ":\".. .\"");
  INSPECT_TEST_SYM("t", ":t");
  INSPECT_TEST_SYM("T", "T");
  INSPECT_TEST_SYM("test", ":test");
  INSPECT_TEST_SYM("Test", "Test");
  INSPECT_TEST_SYM("123", ":123");
  INSPECT_TEST_SYM("test123", ":test123");
  INSPECT_TEST_SYM("Test123", "Test123");
  INSPECT_TEST_SYM("test 123", ":\"test 123\"");
  INSPECT_TEST_SYM("Test 123", ":\"Test 123\"");
  INSPECT_TEST_SYM("test123.test456", ":\"test123.test456\"");
  INSPECT_TEST_SYM("Test123.Test456", "Test123.Test456");
  INSPECT_TEST_SYM("test123(test456)", ":\"test123(test456)\"");
  INSPECT_TEST_SYM("Test123(Test456)", ":\"Test123(Test456)\"");
  INSPECT_TEST_SYM("test123{test456}", ":\"test123{test456}\"");
  INSPECT_TEST_SYM("Test123{Test456}", ":\"Test123{Test456}\"");
  INSPECT_TEST_SYM("É", "É");
  INSPECT_TEST_SYM("Éo", "Éo");
  INSPECT_TEST_SYM("Éoà \n\r\t\v\"",
                   ":\"Éoà \\n\\r\\t\\v\\\"\"");
  INSPECT_TEST_SYM("é", ":é");
  INSPECT_TEST_SYM("éo", ":éo");
  INSPECT_TEST_SYM("éoà \n\r\t\v\"",
                   ":\"éoà \\n\\r\\t\\v\\\"\"");
}
TEST_CASE_END(inspect_sym)

TEST_CASE(inspect_tag)
{
  INSPECT_TEST_TAG("1 + 20",
                   "1 + 20");
  INSPECT_TEST_TAG("1 + 20 / 3",
                   "1 + 20 / 3");
  INSPECT_TEST_TAG("1 + 20 / 3 * 4",
                   "1 + 20 / 3 * 4");
  INSPECT_TEST_TAG("1 + 20 / 3 * 4 - 5",
                   "1 + 20 / 3 * 4 - 5");
  INSPECT_TEST_TAG("a = ? <- 1 ; 2",
                   "a = ? <- 1 ; 2");
}
TEST_CASE_END(inspect_tag)

TEST_CASE(inspect_tuple)
{
  INSPECT_TEST_TUPLE("{:a, :b}", "{:a, :b}");
  INSPECT_TEST_TUPLE("{{:a, :b}, {:c, :d}}", "{{:a, :b}, {:c, :d}}");
  INSPECT_TEST_TUPLE("{{:a, :b}, {:c, :d}, {:e, :f}}",
                     "{{:a, :b}, {:c, :d}, {:e, :f}}");
}
TEST_CASE_END(inspect_tuple)
