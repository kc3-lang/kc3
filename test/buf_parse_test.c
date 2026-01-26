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
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/kc3.h"
#include "test.h"
#include "buf_parse_test_su.h"

#define BUF_PARSE_TEST_ARRAY(test)                                     \
  do {                                                                 \
    s_buf buf;                                                         \
    s_array dest;                                                      \
    test_context("buf_parse_array(" # test ")");                       \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_array(&buf, &dest), strlen(test));               \
    TEST_EQ(buf.rpos, strlen(test));                                   \
    array_clean(&dest);                                                \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_BOOL(test, expected)                            \
  do {                                                                 \
    s_buf buf;                                                         \
    bool dest = -1;                                                    \
    test_context("buf_parse_bool(" # test ") -> " # expected);         \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_bool(&buf, &dest), strlen(test));                \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_CFN(test)                                       \
  do {                                                                 \
    s_buf buf;                                                         \
    s_cfn dest;                                                        \
    test_context("buf_parse_cfn(" # test ")");                         \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_cfn(&buf, &dest), strlen(test));                 \
    cfn_clean(&dest);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_CHARACTER(test, expected)                       \
  do {                                                                 \
    s_buf buf;                                                         \
    character dest = 0;                                               \
    test_context("buf_parse_character(" # test ") -> " # expected);    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_character(&buf, &dest), strlen(test));           \
    TEST_EQ(buf.wpos, strlen(test));                                   \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_CHARACTER_EOF(test)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    character dest = 0;                                                \
    test_context("buf_parse_character(" # test ") -> -1");             \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_character(&buf, &dest), -1);                     \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_DIGIT_BIN(test, expected)                       \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0x80;                                                    \
    test_context("buf_parse_digit_bin(" # test ") -> " # expected);    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_bin(&buf, &dest), 1);                      \
    TEST_EQ(buf.rpos, 1);                                              \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_DIGIT_DEC(test, expected)                       \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0x80;                                                    \
    test_context("buf_parse_digit_dec(" # test ") -> " # expected);    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_dec(&buf, &dest), 1);                      \
    TEST_EQ(buf.rpos, 1);                                              \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_DIGIT_HEX(test, expected)                       \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0x80;                                                    \
    test_context("buf_parse_digit_hex(" # test ") -> " # expected);    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_hex(&buf, &dest), 1);                      \
    TEST_EQ(buf.rpos, 1);                                              \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_DIGIT_OCT(test, expected)                       \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0x80;                                                    \
    test_context("buf_parse_digit_oct(" # test ") -> " # expected);    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_oct(&buf, &dest), 1);                      \
    TEST_EQ(buf.rpos, 1);                                              \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_F32(test, expected1, expected2)                 \
  do {                                                                 \
    s_buf buf;                                                         \
    f32 dest = -1;                                                     \
    test_context("buf_parse_f32(" # test ") -> " # expected1);         \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_f32(&buf, &dest), strlen(test));                 \
    TEST_FLOAT_EQ2(dest, (expected1), (expected2));                    \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_F64(test, expected)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    f64 f64_tmp;                                                       \
    test_context("buf_parse_f64(" # test ") -> " # expected);          \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_f64(&buf, &f64_tmp), strlen(test));              \
    TEST_DOUBLE_EQ(f64_tmp, (expected));                               \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_FN(test)                                        \
  do {                                                                 \
    s_buf buf;                                                         \
    s_fn dest;                                                         \
    test_context("buf_parse_fn(" # test ")");                          \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_fn(&buf, &dest), strlen(test));                  \
    fn_clean(&dest);                                                   \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_IDENT(test, expected)                           \
  do {                                                                 \
    s_buf buf;                                                         \
    s_ident dest;                                                      \
    test_context("buf_parse_ident(" # test ") -> " # expected);        \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_ident(&buf, &dest), strlen(test));               \
    if (g_test_last_ok)                                                \
      TEST_EQ(dest.sym->str.size, strlen(expected));                   \
    if (g_test_last_ok)                                                \
      TEST_STRNCMP(dest.sym->str.ptr.p, (expected),                    \
                   dest.sym->str.size);                                \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_INTEGER(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer(" # test ")");                     \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer(&buf, &i), strlen(test));                \
    if (g_test_last_ok)                                                \
      integer_clean(&i);                                               \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_INTEGER_UNSIGNED_BIN(test)                      \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_bin(" # test ")");        \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_bin(&buf, &i),                  \
            strlen(test));                                             \
    integer_clean(&i);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_INTEGER_UNSIGNED_DEC(test)                      \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_dec(" # test ")");        \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_dec(&buf, &i),                  \
            strlen(test));                                             \
    integer_clean(&i);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_INTEGER_UNSIGNED_HEX(test)                      \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_hex(" # test ")");        \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_hex(&buf, &i),                  \
            strlen(test));                                             \
    integer_clean(&i);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT(test)                      \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_oct(" # test ")");        \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_oct(&buf, &i),                  \
            strlen(test));                                             \
    integer_clean(&i);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_MODULE_NAME(test)                               \
  do {                                                                 \
    s_buf buf;                                                         \
    p_sym result;                                                      \
    test_context("buf_parse_module_name(" # test ")");                 \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_module_name(&buf, &result), strlen(test));       \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_BOOL(test)                                  \
  do {                                                                 \
    s_buf buf;                                                         \
    bool dest = 2;                                                     \
    test_context("buf_parse_bool(" # test ") -> 0");                   \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_bool(&buf, &dest), 0);                           \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 2);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_PCALL(test)                                 \
  do {                                                                 \
    s_buf buf;                                                         \
    p_call dest = {0};                                                 \
    test_context("buf_parse_pcall(" # test ") -> 0");                  \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_pcall(&buf, &dest), 0);                          \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_CFN(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    s_cfn dest = {0};                                                  \
    test_context("buf_parse_cfn(" # test ") -> 0");                    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_cfn(&buf, &dest), 0);                            \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_CHARACTER(test)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    character dest = 0;                                                \
    test_context("buf_parse_character(" # test ") -> 0");              \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_character(&buf, &dest), 0);                      \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_DIGIT_BIN(test)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0;                                                       \
    test_context("buf_parse_digit_bin(" # test ") -> 0");              \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_bin(&buf, &dest), 0);                      \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_DIGIT_DEC(test)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0;                                                       \
    test_context("buf_parse_digit_dec(" # test ") -> 0");              \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_dec(&buf, &dest), 0);                      \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_DIGIT_HEX(test)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0;                                                       \
    test_context("buf_parse_digit_hex(" # test ") -> 0");              \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_hex(&buf, &dest), 0);                      \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_DIGIT_OCT(test)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0;                                                       \
    test_context("buf_parse_digit_oct(" # test ") -> 0");              \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_digit_oct(&buf, &dest), 0);                      \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_IDENT(test)                                 \
  do {                                                                 \
    s_buf buf;                                                         \
    s_ident dest = {0};                                                \
    test_context("buf_parse_ident(" # test ") -> 0");                  \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_ident(&buf, &dest), 0);                          \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_INTEGER(test)                               \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer(" # test ") -> 0");                \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer(&buf, &i), 0);                           \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_BIN(test)                  \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_bin(" # test ") -> 0");   \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_bin(&buf, &i), 0);              \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_DEC(test)                  \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_dec(" # test ") -> 0");   \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_dec(&buf, &i), 0);              \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_HEX(test)                  \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_hex(" # test ") -> 0");   \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_hex(&buf, &i), 0);              \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_OCT(test)                  \
  do {                                                                 \
    s_buf buf;                                                         \
    s_integer i;                                                       \
    test_context("buf_parse_integer_unsigned_oct(" # test ") -> 0");   \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_integer_unsigned_oct(&buf, &i), 0);              \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
} while (0)

#define BUF_PARSE_TEST_NOT_STR(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    s_str dest = {0};                                                  \
    test_context("buf_parse_str(" # test ") -> 0");                    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str(&buf, &dest), 0);                            \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_STR_U8(test)                                \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0;                                                       \
    test_context("buf_parse_str_u8(" # test ") -> 0");                 \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str_u8(&buf, &dest), 0);                         \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_NOT_SYM(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    const s_sym *dest = NULL;                                          \
    test_context("buf_parse_sym(" # test ") -> 0");                    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_sym(&buf, &dest), 0);                            \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_PCALL(test)                                     \
  do {                                                                 \
    s_buf buf;                                                         \
    p_call dest;                                                       \
    test_context("buf_parse_pcall(" # test ")");                       \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_pcall(&buf, &dest), strlen(test));               \
    pcall_clean(&dest);                                                \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_PCALL_OP(test)                                  \
  do {                                                                 \
    s_buf buf;                                                         \
    p_call dest = {0};                                                 \
    test_context("buf_parse_pcall_op(" # test ")");                    \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_pcall_op(&buf, &dest), strlen(test));            \
    pcall_clean(&dest);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_PCALL_PAREN(test)                               \
  do {                                                                 \
    s_buf buf;                                                         \
    p_call dest = {0};                                                 \
    test_context("buf_parse_pcall_paren(" # test ")");                 \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_pcall_paren(&buf, &dest), strlen(test));         \
    pcall_clean(&dest);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_PLIST(test)                                     \
  do {                                                                 \
    s_buf buf;                                                         \
    s_list *dest = NULL;                                               \
    test_context("buf_parse_plist(" # test ")");                       \
    buf_init_1_const(&buf, (test));                                    \
    TEST_EQ(buf_parse_plist(&buf, &dest), strlen(test));               \
    list_delete_all(dest);                                             \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_PVAR(test, t)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    s_var *dest = NULL;                                                \
    test_context("buf_parse_pvar(" # test ")");                        \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_pvar(&buf, &dest), strlen(test));                \
    TEST_EQ(dest->type, (t));                                          \
    TEST_ASSERT(dest);                                                 \
    pvar_clean(&dest);                                                 \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STR(test, expected)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    s_str dest = {0};                                                  \
    test_context("buf_parse_str(" # test ") -> " # expected);          \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str(&buf, &dest), strlen(test));                 \
    TEST_EQ(dest.size, strlen(expected));                              \
    TEST_STRNCMP(dest.ptr.p, (expected), dest.size);                   \
    str_clean(&dest);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STR_EOF(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    s_str dest = {0};                                                  \
    test_context("buf_parse_str(" # test ") -> EOF");                  \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str(&buf, &dest), -1);                           \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STR_CHARACTER(test, expected)                   \
  do {                                                                 \
    s_buf buf;                                                         \
    character dest = -1;                                               \
    test_context("buf_parse_str_character(" # test ") -> " # expected);\
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str_character(&buf, &dest), strlen(test));       \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STR_N(test, n, expected)                        \
  do {                                                                 \
    s_buf buf;                                                         \
    s_str dest;                                                        \
    test_context("buf_parse_str(" # test ") -> " # expected);          \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str(&buf, &dest), strlen(test));                 \
    if (g_test_last_ok)                                                \
      TEST_EQ(dest.size, n);                                           \
    if (g_test_last_ok)                                                \
      TEST_EQ(memcmp(dest.ptr.p, expected, n), 0);                     \
    str_clean(&dest);						       \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STR_U8(test, size, expected)                    \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0x80;                                                    \
    test_context("buf_parse_str_u8(" # test ") -> " # expected);       \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str_u8(&buf, &dest), (size));                    \
    TEST_EQ(buf.rpos, (size));                                         \
    TEST_EQ(dest, (expected));                                         \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STR_U8_EOF(test)                                \
  do {                                                                 \
    s_buf buf;                                                         \
    u8 dest = 0x80;                                                    \
    test_context("buf_parse_str_u8(" # test ") -> -1");                \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_str_u8(&buf, &dest), -1);                        \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, 0x80);                                               \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_STRUCT(test)                                    \
  do {                                                                 \
    test_context("buf_parse_struct(" # test ")");       \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_struct(&buf, &s), strlen(test));                 \
    buf_clean(&buf);						       \
    test_context(NULL);						       \
  } while (0)

#define BUF_PARSE_TEST_SYM(test, expected)                             \
  do {                                                                 \
    s_buf buf;                                                         \
    const s_sym *dest = NULL;                                          \
    test_context("buf_parse_sym(" # test ") -> " # expected);          \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_sym(&buf, &dest), strlen(test));                 \
    if (g_test_last_ok)                                                \
      TEST_ASSERT(dest);                                               \
    if (g_test_last_ok)                                                \
      TEST_EQ(dest->str.size, strlen(expected));                       \
    if (g_test_last_ok)                                                \
      TEST_STRNCMP(dest->str.ptr.p, (expected), dest->str.size);       \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_SYM_EOF(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    const s_sym *dest = NULL;                                          \
    test_context("buf_parse_sym(" # test ") -> -1");                   \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_sym(&buf, &dest), -1);                           \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_TAG(test)                                       \
  do {                                                                 \
    s_buf buf;                                                         \
    s_tag dest = {0};                                                  \
    test_context("buf_parse_tag(" # test ")");                         \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_tag(&buf, &dest), strlen(test));                 \
    tag_clean(&dest);                                                  \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_TUPLE(test)                                     \
  do {                                                                 \
    s_buf buf;                                                         \
    s_tuple dest = {0};						       \
    test_context("buf_parse_tuple(" # test ")");                       \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_tuple(&buf, &dest), strlen(test));               \
    tuple_clean(&dest);                                                \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_UNQUOTE(test)                                   \
  do {                                                                 \
    s_buf buf;                                                         \
    s_unquote dest = {0};                                              \
    test_context("buf_parse_unquote(" # test ")");                     \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_parse_unquote(&buf, &dest), strlen(test));             \
    unquote_clean(&dest);                                              \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(buf_parse_array);
TEST_CASE_PROTOTYPE(buf_parse_bool);
TEST_CASE_PROTOTYPE(buf_parse_pcall);
TEST_CASE_PROTOTYPE(buf_parse_pcall_op);
TEST_CASE_PROTOTYPE(buf_parse_pcall_paren);
TEST_CASE_PROTOTYPE(buf_parse_cfn);
TEST_CASE_PROTOTYPE(buf_parse_character);
TEST_CASE_PROTOTYPE(buf_parse_digit_bin);
TEST_CASE_PROTOTYPE(buf_parse_digit_hex);
TEST_CASE_PROTOTYPE(buf_parse_digit_oct);
TEST_CASE_PROTOTYPE(buf_parse_digit_dec);
TEST_CASE_PROTOTYPE(buf_parse_f32);
TEST_CASE_PROTOTYPE(buf_parse_f64);
TEST_CASE_PROTOTYPE(buf_parse_fn);
TEST_CASE_PROTOTYPE(buf_parse_integer);
TEST_CASE_PROTOTYPE(buf_parse_integer_dec);
TEST_CASE_PROTOTYPE(buf_parse_integer_hex);
TEST_CASE_PROTOTYPE(buf_parse_integer_oct);
TEST_CASE_PROTOTYPE(buf_parse_integer_bin);
TEST_CASE_PROTOTYPE(buf_parse_ident);
TEST_CASE_PROTOTYPE(buf_parse_module_name);
TEST_CASE_PROTOTYPE(buf_parse_plist);
TEST_CASE_PROTOTYPE(buf_parse_pvar);
TEST_CASE_PROTOTYPE(buf_parse_str);
TEST_CASE_PROTOTYPE(buf_parse_str_character);
TEST_CASE_PROTOTYPE(buf_parse_str_u8);
TEST_CASE_PROTOTYPE(buf_parse_sym);
TEST_CASE_PROTOTYPE(buf_parse_tag);
TEST_CASE_PROTOTYPE(buf_parse_tuple);
TEST_CASE_PROTOTYPE(buf_parse_unquote);

void buf_parse_test (void)
{
  TEST_CASE_RUN(buf_parse_array);
  TEST_CASE_RUN(buf_parse_bool);
  TEST_CASE_RUN(buf_parse_pcall);
  TEST_CASE_RUN(buf_parse_pcall_op);
  TEST_CASE_RUN(buf_parse_pcall_paren);
  TEST_CASE_RUN(buf_parse_digit_bin);
  TEST_CASE_RUN(buf_parse_digit_hex);
  TEST_CASE_RUN(buf_parse_digit_oct);
  TEST_CASE_RUN(buf_parse_digit_dec);
  TEST_CASE_RUN(buf_parse_str_character);
  TEST_CASE_RUN(buf_parse_str_u8);
  TEST_CASE_RUN(buf_parse_cfn);
  TEST_CASE_RUN(buf_parse_character);
  TEST_CASE_RUN(buf_parse_f32);
  TEST_CASE_RUN(buf_parse_f64);
  TEST_CASE_RUN(buf_parse_fn);
  TEST_CASE_RUN(buf_parse_integer_bin);
  TEST_CASE_RUN(buf_parse_integer_dec);
  TEST_CASE_RUN(buf_parse_integer_hex);
  TEST_CASE_RUN(buf_parse_integer_oct);
  TEST_CASE_RUN(buf_parse_integer);
  TEST_CASE_RUN(buf_parse_str);
  TEST_CASE_RUN(buf_parse_module_name);
  TEST_CASE_RUN(buf_parse_sym);
  TEST_CASE_RUN(buf_parse_ident);
  TEST_CASE_RUN(buf_parse_plist);
  TEST_CASE_RUN(buf_parse_tag);
  TEST_CASE_RUN(buf_parse_tuple);
  TEST_CASE_RUN(buf_parse_unquote);
  TEST_CASE_RUN(buf_parse_pvar);
#ifdef KC3_TEST_BUF_PARSE_SU
  TEST_CASE_RUN(buf_parse_u8_binary);
  TEST_CASE_RUN(buf_parse_u8_octal);
  TEST_CASE_RUN(buf_parse_u8_hexadecimal);
  TEST_CASE_RUN(buf_parse_u8_decimal);
  TEST_CASE_RUN(buf_parse_u16_binary);
  TEST_CASE_RUN(buf_parse_u16_octal);
  TEST_CASE_RUN(buf_parse_u16_hexadecimal);
  TEST_CASE_RUN(buf_parse_u16_decimal);
  TEST_CASE_RUN(buf_parse_u32_binary);
  TEST_CASE_RUN(buf_parse_u32_octal);
  TEST_CASE_RUN(buf_parse_u32_hexadecimal);
  TEST_CASE_RUN(buf_parse_u32_decimal);
  TEST_CASE_RUN(buf_parse_u64_binary);
  TEST_CASE_RUN(buf_parse_u64_octal);
  TEST_CASE_RUN(buf_parse_u64_hexadecimal);
  TEST_CASE_RUN(buf_parse_u64_decimal);
  TEST_CASE_RUN(buf_parse_uw);
  TEST_CASE_RUN(buf_parse_s16_binary);
  TEST_CASE_RUN(buf_parse_s16_binary_negative);
  TEST_CASE_RUN(buf_parse_s16_octal);
  TEST_CASE_RUN(buf_parse_s16_octal_negative);
  TEST_CASE_RUN(buf_parse_s16_hexadecimal);
  TEST_CASE_RUN(buf_parse_s16_hexadecimal_negative);
  TEST_CASE_RUN(buf_parse_s16_decimal);
  TEST_CASE_RUN(buf_parse_s16_decimal_negative);
  TEST_CASE_RUN(buf_parse_s32_binary);
  TEST_CASE_RUN(buf_parse_s32_binary_negative);
  TEST_CASE_RUN(buf_parse_s32_octal);
  TEST_CASE_RUN(buf_parse_s32_octal_negative);
  TEST_CASE_RUN(buf_parse_s32_hexadecimal);
  TEST_CASE_RUN(buf_parse_s32_hexadecimal_negative);
  TEST_CASE_RUN(buf_parse_s32_decimal);
  TEST_CASE_RUN(buf_parse_s32_decimal_negative);
  TEST_CASE_RUN(buf_parse_s64_binary);
  TEST_CASE_RUN(buf_parse_s64_binary_negative);
  TEST_CASE_RUN(buf_parse_s64_octal);
  TEST_CASE_RUN(buf_parse_s64_octal_negative);
  TEST_CASE_RUN(buf_parse_s64_hexadecimal);
  TEST_CASE_RUN(buf_parse_s64_hexadecimal_negative);
  TEST_CASE_RUN(buf_parse_s64_decimal);
  TEST_CASE_RUN(buf_parse_s64_decimal_negative);
  TEST_CASE_RUN(buf_parse_s8_binary);
  TEST_CASE_RUN(buf_parse_s8_binary_negative);
  TEST_CASE_RUN(buf_parse_s8_octal);
  TEST_CASE_RUN(buf_parse_s8_octal_negative);
  TEST_CASE_RUN(buf_parse_s8_hexadecimal);
  TEST_CASE_RUN(buf_parse_s8_hexadecimal_negative);
  TEST_CASE_RUN(buf_parse_s8_decimal);
  TEST_CASE_RUN(buf_parse_s8_decimal_negative);
  TEST_CASE_RUN(buf_parse_sw);
#endif /* KC3_TEST_BUF_PARSE_SU */
}

TEST_CASE(buf_parse_array)
{
  BUF_PARSE_TEST_ARRAY("(U8[]){0}");
  BUF_PARSE_TEST_ARRAY("(U8[]) {0}");
  BUF_PARSE_TEST_ARRAY("(U8[]) {{0}, {0}}");
  BUF_PARSE_TEST_ARRAY("(U8[]) {{{0, 1}, {2, 3}}, {{4, 5}, {6, 7}}}");
  BUF_PARSE_TEST_ARRAY("(U8[]){ { 0 },{ 0 } }");
}
TEST_CASE_END(buf_parse_array)

TEST_CASE(buf_parse_bool)
{
  BUF_PARSE_TEST_NOT_BOOL("0");
  BUF_PARSE_TEST_NOT_BOOL("1");
  BUF_PARSE_TEST_NOT_BOOL("a");
  BUF_PARSE_TEST_NOT_BOOL("T");
  BUF_PARSE_TEST_NOT_BOOL("NIL");
  BUF_PARSE_TEST_NOT_BOOL("N");
  BUF_PARSE_TEST_NOT_BOOL("Y");
  BUF_PARSE_TEST_NOT_BOOL("t");
  BUF_PARSE_TEST_NOT_BOOL("nil");
  BUF_PARSE_TEST_NOT_BOOL("n");
  BUF_PARSE_TEST_NOT_BOOL("y");
  BUF_PARSE_TEST_NOT_BOOL("falser");
  BUF_PARSE_TEST_NOT_BOOL("truer");
  BUF_PARSE_TEST_BOOL("false", 0);
  BUF_PARSE_TEST_BOOL("true", 1);
}
TEST_CASE_END(buf_parse_bool)

TEST_CASE(buf_parse_pcall)
{
  BUF_PARSE_TEST_NOT_PCALL("0");
  BUF_PARSE_TEST_NOT_PCALL("9");
  BUF_PARSE_TEST_NOT_PCALL("A");
  BUF_PARSE_TEST_NOT_PCALL("Z");
  BUF_PARSE_TEST_NOT_PCALL("a");
  BUF_PARSE_TEST_NOT_PCALL("z");
  BUF_PARSE_TEST_NOT_PCALL("()");
  BUF_PARSE_TEST_PCALL("A.b()");
  BUF_PARSE_TEST_PCALL("A.b(c)");
  BUF_PARSE_TEST_PCALL("A.b(c, d)");
  BUF_PARSE_TEST_PCALL("A.b(c, d, e)");
  BUF_PARSE_TEST_PCALL("a()");
  BUF_PARSE_TEST_PCALL("a()");
  BUF_PARSE_TEST_PCALL("a(b)");
  BUF_PARSE_TEST_PCALL("a(b, c)");
  BUF_PARSE_TEST_PCALL("A.b(c(d), e)");
  BUF_PARSE_TEST_PCALL("A.b(C.d(e))");
  BUF_PARSE_TEST_PCALL("A.b(C.d(E.f(g, h), I.j(k, l)))");
  BUF_PARSE_TEST_PCALL("a(b, c, d)");
  BUF_PARSE_TEST_PCALL("a(b(c), d)");
  BUF_PARSE_TEST_PCALL("a(B.c(d))");
  BUF_PARSE_TEST_PCALL("a(B.c(D.e(f, g), H.i(j, k)))");
}
TEST_CASE_END(buf_parse_pcall)

TEST_CASE(buf_parse_pcall_op)
{
  BUF_PARSE_TEST_PCALL_OP("1 + 2");
  BUF_PARSE_TEST_PCALL_OP("1 + 2 + 3");
  BUF_PARSE_TEST_PCALL_OP("1 + 2 / 3");
  BUF_PARSE_TEST_PCALL_OP("1 + 2 / 3 * 4");
  BUF_PARSE_TEST_PCALL_OP("1 + 2 / 3 * 4 - 5");
}
TEST_CASE_END(buf_parse_pcall_op)

TEST_CASE(buf_parse_pcall_paren)
{
  BUF_PARSE_TEST_PCALL_PAREN("(1 + 2)");
  BUF_PARSE_TEST_PCALL_PAREN("(1 + 2 + 3)");
  BUF_PARSE_TEST_PCALL_PAREN("(1 + 2 / 3)");
  BUF_PARSE_TEST_PCALL_PAREN("(1 + 2 / 3 ; 4)");
  BUF_PARSE_TEST_PCALL_PAREN("(1 + 2 / 3 ; 4 - 5)");
}
TEST_CASE_END(buf_parse_pcall_paren)

TEST_CASE(buf_parse_cfn)
{
  BUF_PARSE_TEST_NOT_CFN("0");
  BUF_PARSE_TEST_NOT_CFN("9");
  BUF_PARSE_TEST_NOT_CFN("A");
  BUF_PARSE_TEST_NOT_CFN("Z");
  BUF_PARSE_TEST_NOT_CFN("a");
  BUF_PARSE_TEST_NOT_CFN("z");
  BUF_PARSE_TEST_NOT_CFN("cfn 0 1 (2)");
  BUF_PARSE_TEST_CFN("cfn Tag \"tag_add\" (Tag)");
  BUF_PARSE_TEST_CFN("cfn Tag \"tag_add\" (Tag, Tag)");
  BUF_PARSE_TEST_CFN("cfn Tag \"tag_add\" (Tag, Tag, Tag)");
}
TEST_CASE_END(buf_parse_cfn)

TEST_CASE(buf_parse_character)
{
  BUF_PARSE_TEST_NOT_CHARACTER("\x01");
  BUF_PARSE_TEST_NOT_CHARACTER("\xFF");
  BUF_PARSE_TEST_NOT_CHARACTER("0");
  BUF_PARSE_TEST_NOT_CHARACTER("9");
  BUF_PARSE_TEST_NOT_CHARACTER("A");
  BUF_PARSE_TEST_NOT_CHARACTER("Z");
  BUF_PARSE_TEST_NOT_CHARACTER("a");
  BUF_PARSE_TEST_NOT_CHARACTER("z");
  BUF_PARSE_TEST_CHARACTER_EOF("'");
  BUF_PARSE_TEST_CHARACTER_EOF("'\\");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\x");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\x'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\x0'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\x00'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\x61'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\00'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\0X'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\0x'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\nx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\rx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\tx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\vx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'\\sx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'0x'");
  BUF_PARSE_TEST_NOT_CHARACTER("'9x'");
  BUF_PARSE_TEST_NOT_CHARACTER("'Ax'");
  BUF_PARSE_TEST_NOT_CHARACTER("'Zx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'ax'");
  BUF_PARSE_TEST_NOT_CHARACTER("'zx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'Πx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'ÿx'");
  BUF_PARSE_TEST_NOT_CHARACTER("'꒴x'");
  BUF_PARSE_TEST_NOT_CHARACTER("'𐅀x'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x\\0'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x\\n'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x\\r'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x\\t'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x\\v'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x\\s'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x0'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x9'");
  BUF_PARSE_TEST_NOT_CHARACTER("'xA'");
  BUF_PARSE_TEST_NOT_CHARACTER("'xZ'");
  BUF_PARSE_TEST_NOT_CHARACTER("'xa'");
  BUF_PARSE_TEST_NOT_CHARACTER("'xz'");
  BUF_PARSE_TEST_NOT_CHARACTER("'xΠ'");
  BUF_PARSE_TEST_NOT_CHARACTER("'xÿ'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x꒴'");
  BUF_PARSE_TEST_NOT_CHARACTER("'x𐅀'");
  BUF_PARSE_TEST_CHARACTER("'\\0'", 0);
  BUF_PARSE_TEST_CHARACTER("'\\n'", '\n');
  BUF_PARSE_TEST_CHARACTER("'\\r'", '\r');
  BUF_PARSE_TEST_CHARACTER("'\\t'", '\t');
  BUF_PARSE_TEST_CHARACTER("'\\v'", '\v');
  BUF_PARSE_TEST_CHARACTER("'\\s'", ' ');
  BUF_PARSE_TEST_CHARACTER("'0'", '0');
  BUF_PARSE_TEST_CHARACTER("'9'", '9');
  BUF_PARSE_TEST_CHARACTER("'A'", 'A');
  BUF_PARSE_TEST_CHARACTER("'Z'", 'Z');
  BUF_PARSE_TEST_CHARACTER("'a'", 'a');
  BUF_PARSE_TEST_CHARACTER("'z'", 'z');
  BUF_PARSE_TEST_CHARACTER("'Π'", 928);
  BUF_PARSE_TEST_CHARACTER("'ÿ'", 0xFF);
  BUF_PARSE_TEST_CHARACTER("'꒴'", 42164);
  BUF_PARSE_TEST_CHARACTER("'𐅀'", 65856);
  BUF_PARSE_TEST_CHARACTER("'🎳'", 127923);
  BUF_PARSE_TEST_CHARACTER("'😄'", 128516);
  BUF_PARSE_TEST_CHARACTER("'🟣'", 128995);
  BUF_PARSE_TEST_CHARACTER("'🤩'", 129321);
}
TEST_CASE_END(buf_parse_character)

TEST_CASE(buf_parse_digit_bin)
{
  BUF_PARSE_TEST_NOT_DIGIT_BIN("\x01");
  BUF_PARSE_TEST_NOT_DIGIT_BIN("\x02");
  BUF_PARSE_TEST_NOT_DIGIT_BIN("\xFF");
  BUF_PARSE_TEST_NOT_DIGIT_BIN(".");
  BUF_PARSE_TEST_NOT_DIGIT_BIN(":");
  BUF_PARSE_TEST_NOT_DIGIT_BIN(",");
  BUF_PARSE_TEST_NOT_DIGIT_BIN(";");
  BUF_PARSE_TEST_NOT_DIGIT_BIN("G");
  BUF_PARSE_TEST_NOT_DIGIT_BIN("H");
  BUF_PARSE_TEST_NOT_DIGIT_BIN("g");
  BUF_PARSE_TEST_NOT_DIGIT_BIN("h");
  BUF_PARSE_TEST_DIGIT_BIN("0", 0);
  BUF_PARSE_TEST_DIGIT_BIN("1", 1);
}
TEST_CASE_END(buf_parse_digit_bin)

TEST_CASE(buf_parse_digit_hex)
{
  BUF_PARSE_TEST_NOT_DIGIT_HEX("\x01");
  BUF_PARSE_TEST_NOT_DIGIT_HEX("\x02");
  BUF_PARSE_TEST_NOT_DIGIT_HEX("\xFF");
  BUF_PARSE_TEST_NOT_DIGIT_HEX(".");
  BUF_PARSE_TEST_NOT_DIGIT_HEX(":");
  BUF_PARSE_TEST_NOT_DIGIT_HEX(",");
  BUF_PARSE_TEST_NOT_DIGIT_HEX(";");
  BUF_PARSE_TEST_NOT_DIGIT_HEX("G");
  BUF_PARSE_TEST_NOT_DIGIT_HEX("H");
  BUF_PARSE_TEST_NOT_DIGIT_HEX("g");
  BUF_PARSE_TEST_NOT_DIGIT_HEX("h");
  BUF_PARSE_TEST_DIGIT_HEX("0", 0);
  BUF_PARSE_TEST_DIGIT_HEX("9", 9);
  BUF_PARSE_TEST_DIGIT_HEX("A", 0x0A);
  BUF_PARSE_TEST_DIGIT_HEX("F", 0x0F);
  BUF_PARSE_TEST_DIGIT_HEX("a", 0x0A);
  BUF_PARSE_TEST_DIGIT_HEX("f", 0x0F);
}
TEST_CASE_END(buf_parse_digit_hex)

TEST_CASE(buf_parse_digit_oct)
{
  BUF_PARSE_TEST_NOT_DIGIT_OCT("\x01");
  BUF_PARSE_TEST_NOT_DIGIT_OCT("\x02");
  BUF_PARSE_TEST_NOT_DIGIT_OCT("\xFF");
  BUF_PARSE_TEST_NOT_DIGIT_OCT(".");
  BUF_PARSE_TEST_NOT_DIGIT_OCT(":");
  BUF_PARSE_TEST_NOT_DIGIT_OCT(",");
  BUF_PARSE_TEST_NOT_DIGIT_OCT(";");
  BUF_PARSE_TEST_NOT_DIGIT_OCT("G");
  BUF_PARSE_TEST_NOT_DIGIT_OCT("H");
  BUF_PARSE_TEST_NOT_DIGIT_OCT("g");
  BUF_PARSE_TEST_NOT_DIGIT_OCT("h");
  BUF_PARSE_TEST_DIGIT_OCT("0", 0);
  BUF_PARSE_TEST_DIGIT_OCT("7", 7);
}
TEST_CASE_END(buf_parse_digit_oct)

TEST_CASE(buf_parse_digit_dec)
{
  BUF_PARSE_TEST_NOT_DIGIT_DEC("\x01");
  BUF_PARSE_TEST_NOT_DIGIT_DEC("\x02");
  BUF_PARSE_TEST_NOT_DIGIT_DEC("\xFF");
  BUF_PARSE_TEST_NOT_DIGIT_DEC(".");
  BUF_PARSE_TEST_NOT_DIGIT_DEC(":");
  BUF_PARSE_TEST_NOT_DIGIT_DEC(",");
  BUF_PARSE_TEST_NOT_DIGIT_DEC(";");
  BUF_PARSE_TEST_NOT_DIGIT_DEC("G");
  BUF_PARSE_TEST_NOT_DIGIT_DEC("H");
  BUF_PARSE_TEST_NOT_DIGIT_DEC("g");
  BUF_PARSE_TEST_NOT_DIGIT_DEC("h");
  BUF_PARSE_TEST_DIGIT_DEC("0", 0);
  BUF_PARSE_TEST_DIGIT_DEC("9", 9);
}
TEST_CASE_END(buf_parse_digit_dec)

TEST_CASE(buf_parse_f32)
{
  BUF_PARSE_TEST_F32("0.0f", 0.0f, 0.0f);
  BUF_PARSE_TEST_F32("123.123f", 123.123, 123.1229934692383);
  BUF_PARSE_TEST_F32("3.14159f", 3.14159, 3.141589641571045);
  BUF_PARSE_TEST_F32("2.1e+2f", 210, 209.9999847412109);
}
TEST_CASE_END(buf_parse_digit_dec)

TEST_CASE(buf_parse_f64)
{
  BUF_PARSE_TEST_F64("0.0", 0.0);
  //BUF_PARSE_TEST_F64("123.123", 123.123);
  //BUF_PARSE_TEST_F64("3.14159", 3.14159);
  BUF_PARSE_TEST_F64("2.1e+2", 210);
}
TEST_CASE_END(buf_parse_digit_dec)

TEST_CASE(buf_parse_fn)
{
  BUF_PARSE_TEST_FN("fn () { [] }");
  BUF_PARSE_TEST_FN("fn (x) { x }");
  BUF_PARSE_TEST_FN("fn (x, y) { x }");
}
TEST_CASE_END(buf_parse_fn)

TEST_CASE(buf_parse_ident)
{
  BUF_PARSE_TEST_NOT_IDENT("0");
  BUF_PARSE_TEST_NOT_IDENT("9");
  BUF_PARSE_TEST_NOT_IDENT("A");
  BUF_PARSE_TEST_NOT_IDENT("Z");
  BUF_PARSE_TEST_NOT_IDENT("09AZ");
  BUF_PARSE_TEST_NOT_IDENT("Π");
  BUF_PARSE_TEST_IDENT("_\"0\"", "0");
  BUF_PARSE_TEST_IDENT("_\"9\"", "9");
  BUF_PARSE_TEST_IDENT("_\"A\"", "A");
  BUF_PARSE_TEST_IDENT("_\"Z\"", "Z");
  BUF_PARSE_TEST_IDENT("_\"a\"", "a");
  BUF_PARSE_TEST_IDENT("_\"z\"", "z");
  BUF_PARSE_TEST_IDENT("_\"09AZaz\"", "09AZaz");
  BUF_PARSE_TEST_IDENT("_", "_");
  BUF_PARSE_TEST_IDENT("__", "__");
  BUF_PARSE_TEST_IDENT("_0", "_0");
  BUF_PARSE_TEST_IDENT("_9", "_9");
  BUF_PARSE_TEST_IDENT("_A", "_A");
  BUF_PARSE_TEST_IDENT("_Z", "_Z");
  BUF_PARSE_TEST_IDENT("_a", "_a");
  BUF_PARSE_TEST_IDENT("_z", "_z");
  BUF_PARSE_TEST_IDENT("_Π", "_Π");
  BUF_PARSE_TEST_IDENT("_az09AZ", "_az09AZ");
  BUF_PARSE_TEST_IDENT("a", "a");
  BUF_PARSE_TEST_IDENT("z", "z");
  BUF_PARSE_TEST_IDENT("+", "+");
  BUF_PARSE_TEST_IDENT("-", "-");
  BUF_PARSE_TEST_IDENT("*", "*");
  BUF_PARSE_TEST_IDENT("/", "/");
  BUF_PARSE_TEST_IDENT("=", "=");
  BUF_PARSE_TEST_IDENT("az09AZ", "az09AZ");
  BUF_PARSE_TEST_IDENT("a+b", "a+b");
  BUF_PARSE_TEST_IDENT("+a", "+a");
  BUF_PARSE_TEST_IDENT("/a", "/a");
  BUF_PARSE_TEST_IDENT("=a", "=a");
  BUF_PARSE_TEST_IDENT("a+", "a+");
  BUF_PARSE_TEST_IDENT("a-", "a-");
  BUF_PARSE_TEST_IDENT("a*", "a*");
  BUF_PARSE_TEST_IDENT("a/", "a/");
  BUF_PARSE_TEST_IDENT("a=", "a=");
  BUF_PARSE_TEST_IDENT("é", "é");
  BUF_PARSE_TEST_IDENT("ÿ", "ÿ");
  BUF_PARSE_TEST_IDENT("π", "π");
  BUF_PARSE_TEST_IDENT("꒴", "꒴");
  BUF_PARSE_TEST_IDENT("𐅀", "𐅀");
  BUF_PARSE_TEST_IDENT("🎳", "🎳");
  BUF_PARSE_TEST_IDENT("😄", "😄");
  BUF_PARSE_TEST_IDENT("🟣", "🟣");
  BUF_PARSE_TEST_IDENT("🤩", "🤩");
  BUF_PARSE_TEST_IDENT("Module.i", "i");
  BUF_PARSE_TEST_IDENT("Module.ident_az09AZ", "ident_az09AZ");
}
TEST_CASE_END(buf_parse_ident)

TEST_CASE(buf_parse_integer)
{
  BUF_PARSE_TEST_NOT_INTEGER("-");
  BUF_PARSE_TEST_NOT_INTEGER("A");
  BUF_PARSE_TEST_NOT_INTEGER("F");
  BUF_PARSE_TEST_NOT_INTEGER("G");
  BUF_PARSE_TEST_NOT_INTEGER("STR");
  BUF_PARSE_TEST_NOT_INTEGER("a");
  BUF_PARSE_TEST_NOT_INTEGER("f");
  BUF_PARSE_TEST_NOT_INTEGER("g");
  BUF_PARSE_TEST_NOT_INTEGER("str");
  BUF_PARSE_TEST_INTEGER("0");
  BUF_PARSE_TEST_INTEGER("1");
  BUF_PARSE_TEST_INTEGER("00");
  BUF_PARSE_TEST_INTEGER("01");
  BUF_PARSE_TEST_INTEGER("10");
  BUF_PARSE_TEST_INTEGER("11");
  BUF_PARSE_TEST_INTEGER("100");
  BUF_PARSE_TEST_INTEGER("0b0");
  BUF_PARSE_TEST_INTEGER("0b1");
  BUF_PARSE_TEST_INTEGER("0b00");
  BUF_PARSE_TEST_INTEGER("0b01");
  BUF_PARSE_TEST_INTEGER("0b10");
  BUF_PARSE_TEST_INTEGER("0b11");
  BUF_PARSE_TEST_INTEGER("0b100");
  BUF_PARSE_TEST_INTEGER("0B0");
  BUF_PARSE_TEST_INTEGER("0B1");
  BUF_PARSE_TEST_INTEGER("0B00");
  BUF_PARSE_TEST_INTEGER("0B01");
  BUF_PARSE_TEST_INTEGER("0B10");
  BUF_PARSE_TEST_INTEGER("0B11");
  BUF_PARSE_TEST_INTEGER("0B100");
  BUF_PARSE_TEST_INTEGER("0o0");
  BUF_PARSE_TEST_INTEGER("0o1");
  BUF_PARSE_TEST_INTEGER("0o00");
  BUF_PARSE_TEST_INTEGER("0o01");
  BUF_PARSE_TEST_INTEGER("0o10");
  BUF_PARSE_TEST_INTEGER("0o11");
  BUF_PARSE_TEST_INTEGER("0o100");
  BUF_PARSE_TEST_INTEGER("0O0");
  BUF_PARSE_TEST_INTEGER("0O1");
  BUF_PARSE_TEST_INTEGER("0O00");
  BUF_PARSE_TEST_INTEGER("0O01");
  BUF_PARSE_TEST_INTEGER("0O10");
  BUF_PARSE_TEST_INTEGER("0O11");
  BUF_PARSE_TEST_INTEGER("0O100");
}
TEST_CASE_END(buf_parse_integer)

TEST_CASE(buf_parse_integer_bin)
{
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_BIN("A");
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_BIN("STR");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_BIN("0");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_BIN("1");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_BIN("10");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_BIN("11");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_BIN("100");
}
TEST_CASE_END(buf_parse_integer_bin)

TEST_CASE(buf_parse_integer_dec)
{
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_DEC("A");
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_DEC("STR");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_DEC("0");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_DEC("9");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_DEC("256");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_DEC("100_000_000_000_000_000");
}
TEST_CASE_END(buf_parse_integer_bin)

TEST_CASE(buf_parse_integer_hex)
{
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_HEX(" ");
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_HEX("H");
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_HEX("h");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_HEX("0");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_HEX("9");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_HEX("A");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_HEX("F");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_HEX("256");
}
TEST_CASE_END(buf_parse_integer_bin)

TEST_CASE(buf_parse_integer_oct)
{
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_OCT("8");
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_OCT("A");
  BUF_PARSE_TEST_NOT_INTEGER_UNSIGNED_OCT("a");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT("0");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT("1");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT("10");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT("11");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT("100");
  BUF_PARSE_TEST_INTEGER_UNSIGNED_OCT("777");
}
TEST_CASE_END(buf_parse_integer_bin)

TEST_CASE(buf_parse_plist)
{
  BUF_PARSE_TEST_PLIST("[]");
  BUF_PARSE_TEST_PLIST("[[], []]");
  BUF_PARSE_TEST_PLIST("[[] | []]");
  BUF_PARSE_TEST_PLIST("[[], [], []]");
  BUF_PARSE_TEST_PLIST("[[], [] | []]");
  BUF_PARSE_TEST_PLIST("[a | b]");
  BUF_PARSE_TEST_PLIST("[a, b | c]");
  BUF_PARSE_TEST_PLIST("[a, b, c | d]");
}
TEST_CASE_END(buf_parse_list)

TEST_CASE(buf_parse_sw)
{
}
TEST_CASE_END(buf_parse_sw)

TEST_CASE(buf_parse_str)
{
  BUF_PARSE_TEST_NOT_STR("");
  BUF_PARSE_TEST_NOT_STR("\x00");
  BUF_PARSE_TEST_NOT_STR("\x01");
  BUF_PARSE_TEST_NOT_STR("\xFF");
  BUF_PARSE_TEST_NOT_STR("\x00\x01\xFF");
  BUF_PARSE_TEST_NOT_STR("0");
  BUF_PARSE_TEST_NOT_STR("9");
  BUF_PARSE_TEST_NOT_STR("A");
  BUF_PARSE_TEST_NOT_STR("Z");
  BUF_PARSE_TEST_NOT_STR("a");
  BUF_PARSE_TEST_NOT_STR("z");
  BUF_PARSE_TEST_NOT_STR("09AZaz");
  BUF_PARSE_TEST_STR_EOF("\"");
  BUF_PARSE_TEST_STR_EOF("\"0");
  BUF_PARSE_TEST_STR_EOF("\"9");
  BUF_PARSE_TEST_STR_EOF("\"09AZaz");
  BUF_PARSE_TEST_STR_N("\"\\0\"", 1, "\0");
  BUF_PARSE_TEST_STR_N("\"\\0\\0\"", 2, "\0\0");
  BUF_PARSE_TEST_STR_N("\"\\0\\0\\0\"", 3, "\0\0\0");
  BUF_PARSE_TEST_STR("\"\"", "");
  BUF_PARSE_TEST_STR("\"\\n\"", "\n");
  BUF_PARSE_TEST_STR("\"\\r\"", "\r");
  BUF_PARSE_TEST_STR("\"\\t\"", "\t");
  BUF_PARSE_TEST_STR("\"\\v\"", "\v");
  BUF_PARSE_TEST_STR("\"\\s\"", " ");
  BUF_PARSE_TEST_STR("\"\\x01\"", "\x01");
  BUF_PARSE_TEST_STR("\"\\x02\"", "\x02");
  BUF_PARSE_TEST_STR("\"\\xff\"", "\xFF");
  BUF_PARSE_TEST_STR("\"\\xFF\"", "\xFF");
  BUF_PARSE_TEST_STR("\"0\"", "0");
  BUF_PARSE_TEST_STR("\"9\"", "9");
  BUF_PARSE_TEST_STR("\"A\"", "A");
  BUF_PARSE_TEST_STR("\"Z\"", "Z");
  BUF_PARSE_TEST_STR("\"a\"", "a");
  BUF_PARSE_TEST_STR("\"z\"", "z");
  BUF_PARSE_TEST_STR("\"Π\"", "Π");
  BUF_PARSE_TEST_STR("\"ÿ\"", "ÿ");
  BUF_PARSE_TEST_STR("\"꒴\"", "꒴");
  BUF_PARSE_TEST_STR("\"𐅀\"", "𐅀");
  BUF_PARSE_TEST_STR("\"●\"", "●");
  BUF_PARSE_TEST_STR("\"🎳\"", "🎳");
  BUF_PARSE_TEST_STR("\"😄\"", "😄");
  BUF_PARSE_TEST_STR("\"🟣\"", "🟣");
  BUF_PARSE_TEST_STR("\"🤩\"", "🤩");
  BUF_PARSE_TEST_STR("\"ꇤ\"", "ꇤ");
}
TEST_CASE_END(buf_parse_str)

TEST_CASE(buf_parse_str_character)
{
  BUF_PARSE_TEST_STR_CHARACTER("0", '0');
  BUF_PARSE_TEST_STR_CHARACTER("9", '9');
  BUF_PARSE_TEST_STR_CHARACTER("A", 'A');
  BUF_PARSE_TEST_STR_CHARACTER("Z", 'Z');
  BUF_PARSE_TEST_STR_CHARACTER("\\0", 0);
  BUF_PARSE_TEST_STR_CHARACTER("\\n", '\n');
  BUF_PARSE_TEST_STR_CHARACTER("\\r", '\r');
  BUF_PARSE_TEST_STR_CHARACTER("\\t", '\t');
  BUF_PARSE_TEST_STR_CHARACTER("\\v", '\v');
  BUF_PARSE_TEST_STR_CHARACTER("\\s", ' ');
  BUF_PARSE_TEST_STR_CHARACTER("\\U+0000", 0);
  BUF_PARSE_TEST_STR_CHARACTER("\\U+1F595", 128405);
  BUF_PARSE_TEST_STR_CHARACTER("\\U+FF", 0xFF);
  BUF_PARSE_TEST_STR_CHARACTER("a", 'a');
  BUF_PARSE_TEST_STR_CHARACTER("z", 'z');
  BUF_PARSE_TEST_STR_CHARACTER("ÿ", 0xFF);
  BUF_PARSE_TEST_STR_CHARACTER("Π", 928);
  BUF_PARSE_TEST_STR_CHARACTER("꒴", 42164);
  BUF_PARSE_TEST_STR_CHARACTER("𐅀", 65856);
  BUF_PARSE_TEST_STR_CHARACTER("🎳", 127923);
  BUF_PARSE_TEST_STR_CHARACTER("😄", 128516);
  BUF_PARSE_TEST_STR_CHARACTER("🟣", 128995);
  BUF_PARSE_TEST_STR_CHARACTER("🤩", 129321);
}
TEST_CASE_END(buf_parse_str_character)

TEST_CASE(buf_parse_str_u8)
{
  BUF_PARSE_TEST_STR_U8_EOF("");
  BUF_PARSE_TEST_STR_U8("\x01", 1, 0x01);
  BUF_PARSE_TEST_STR_U8("0", 1, '0');
  BUF_PARSE_TEST_STR_U8("00", 1, '0');
  BUF_PARSE_TEST_STR_U8("000", 1, '0');
  BUF_PARSE_TEST_STR_U8_EOF("\\");
  BUF_PARSE_TEST_STR_U8_EOF("\\x");
  BUF_PARSE_TEST_STR_U8_EOF("\\x0");
  BUF_PARSE_TEST_STR_U8("x", 1, 'x');
  BUF_PARSE_TEST_STR_U8("x0", 1, 'x');
  BUF_PARSE_TEST_STR_U8("x00", 1, 'x');
  BUF_PARSE_TEST_STR_U8_EOF("\\0");
  BUF_PARSE_TEST_STR_U8_EOF("\\00");
  BUF_PARSE_TEST_STR_U8("\\000", 4, 0x00);
  BUF_PARSE_TEST_STR_U8("\\001", 4, 0x01);
  BUF_PARSE_TEST_STR_U8("\\010", 4, 0x08);
  BUF_PARSE_TEST_STR_U8("\\100", 4, 0x40);
  BUF_PARSE_TEST_STR_U8("\\377", 4, 0xFF);
  BUF_PARSE_TEST_STR_U8("\\0008", 4, 0x00);
  BUF_PARSE_TEST_STR_U8("\\0018", 4, 0x01);
  BUF_PARSE_TEST_STR_U8("\\0108", 4, 0x08);
  BUF_PARSE_TEST_STR_U8("\\1008", 4, 0x40);
  BUF_PARSE_TEST_STR_U8("\\3778", 4, 0xFF);
  BUF_PARSE_TEST_STR_U8("\\x00", 4, 0x00);
  BUF_PARSE_TEST_STR_U8("\\x01", 4, 0x01);
  BUF_PARSE_TEST_STR_U8("\\x0F", 4, 0x0F);
  BUF_PARSE_TEST_STR_U8("\\x80", 4, 0x80);
  BUF_PARSE_TEST_STR_U8("\\xF0", 4, 0xF0);
  BUF_PARSE_TEST_STR_U8("\\xFF8", 4, 0xFF);
  BUF_PARSE_TEST_STR_U8("\\x008", 4, 0x00);
  BUF_PARSE_TEST_STR_U8("\\x018", 4, 0x01);
  BUF_PARSE_TEST_STR_U8("\\x0F8", 4, 0x0F);
  BUF_PARSE_TEST_STR_U8("\\x808", 4, 0x80);
  BUF_PARSE_TEST_STR_U8("\\xF08", 4, 0xF0);
  BUF_PARSE_TEST_STR_U8("\\xFF8", 4, 0xFF);
}
TEST_CASE_END(buf_parse_str_u8)

TEST_CASE(buf_parse_struct)
{
  s_buf buf;
  s_struct s = {0};
  const s_tag *symbol_value;
  BUF_PARSE_TEST_STRUCT("%KC3.Operator{symbol_value: 1}");
  TEST_ASSERT((symbol_value =
               struct_get_tag(&s, sym_1("symbol_value"))));
  TEST_EQ(symbol_value->type, TAG_U8);
  struct_clean(&s);
  BUF_PARSE_TEST_STRUCT("%KC3.Operator{symbol_value: void}");
  TEST_ASSERT((symbol_value =
               struct_get_tag(&s, sym_1("symbol_value"))));
  TEST_EQ(symbol_value->type, TAG_VOID);
  struct_clean(&s);
  test_context(NULL);
}
TEST_CASE_END(buf_parse_struct)

TEST_CASE(buf_parse_sym)
{
  BUF_PARSE_TEST_NOT_SYM("0");
  BUF_PARSE_TEST_NOT_SYM("9");
  BUF_PARSE_TEST_NOT_SYM("a");
  BUF_PARSE_TEST_NOT_SYM("z");
  BUF_PARSE_TEST_NOT_SYM("00");
  BUF_PARSE_TEST_NOT_SYM("0Abc");
  BUF_PARSE_TEST_NOT_SYM("0abc");
  BUF_PARSE_TEST_SYM_EOF(":");
  BUF_PARSE_TEST_SYM_EOF(":\"");
  BUF_PARSE_TEST_SYM_EOF(":\"abc");
  BUF_PARSE_TEST_NOT_SYM("_");
  BUF_PARSE_TEST_NOT_SYM("_abc");
  BUF_PARSE_TEST_SYM(":\"0\"", "0");
  BUF_PARSE_TEST_SYM(":\"9\"", "9");
  BUF_PARSE_TEST_SYM(":\"A\"", "A");
  BUF_PARSE_TEST_SYM(":\"Z\"", "Z");
  BUF_PARSE_TEST_SYM(":\"a\"", "a");
  BUF_PARSE_TEST_SYM(":\"z\"", "z");
  BUF_PARSE_TEST_SYM(":\"_az\"", "_az");
  BUF_PARSE_TEST_SYM("A", "A");
  BUF_PARSE_TEST_SYM("Z", "Z");
  BUF_PARSE_TEST_SYM("Az09az", "Az09az");
  BUF_PARSE_TEST_SYM(":az09AZ", "az09AZ");
  BUF_PARSE_TEST_SYM("A.B.C", "A.B.C");
}
TEST_CASE_END(buf_parse_sym)

TEST_CASE(buf_parse_tag)
{
  BUF_PARSE_TEST_TAG("x");
  BUF_PARSE_TEST_TAG("_x");
  BUF_PARSE_TEST_TAG("[x | _y]");
}
TEST_CASE_END(buf_parse_tag)

TEST_CASE(buf_parse_tuple)
{
  BUF_PARSE_TEST_TUPLE("{a, b}");
  BUF_PARSE_TEST_TUPLE("{a, b, c}");
  BUF_PARSE_TEST_TUPLE("{a, b, c, d}");
  BUF_PARSE_TEST_TUPLE("{a, b, c, d}");
  BUF_PARSE_TEST_TUPLE("{a, {b, c}}");
  BUF_PARSE_TEST_TUPLE("{{a, b}, c}");
  BUF_PARSE_TEST_TUPLE("{{a, b}, {c, d}}");
}
TEST_CASE_END(buf_parse_tuple)

TEST_CASE(buf_parse_u)
{
}
TEST_CASE_END(buf_parse_u)

TEST_CASE(buf_parse_unquote)
{
  BUF_PARSE_TEST_UNQUOTE("unquote 1");
  BUF_PARSE_TEST_UNQUOTE("unquote 1 + 1");
  BUF_PARSE_TEST_UNQUOTE("unquote ident");
  BUF_PARSE_TEST_UNQUOTE("unquote :sym");
  BUF_PARSE_TEST_UNQUOTE("unquote \"str\"");
}
TEST_CASE_END(buf_parse_unquote)

TEST_CASE(buf_parse_pvar)
{
  BUF_PARSE_TEST_PVAR("?", &g_sym_Tag);
  BUF_PARSE_TEST_PVAR("(U8) ?", &g_sym_U8);
}
TEST_CASE_END(buf_parse_var)

TEST_CASE(buf_parse_uw)
{
}
TEST_CASE_END(buf_parse_uw)

TEST_CASE(buf_parse_module_name)
{
  BUF_PARSE_TEST_MODULE_NAME("A");
  BUF_PARSE_TEST_MODULE_NAME("Ab");
  BUF_PARSE_TEST_MODULE_NAME("AbcD");
  BUF_PARSE_TEST_MODULE_NAME("AbcDEF");
  BUF_PARSE_TEST_MODULE_NAME("Abc.Def");
  BUF_PARSE_TEST_MODULE_NAME("Abc.Def.GhIJK");
  BUF_PARSE_TEST_MODULE_NAME("Abc0.D1ef.Gh2IJK3");
}
TEST_CASE_END(buf_parse_integer_bin)
