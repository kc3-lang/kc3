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
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/buf.h"
#include "../libkc3/str.h"
#include "test.h"

#define BUF_TEST_CLEAN(bufa)                                           \
  do {                                                                 \
    buf_clean(&bufa);                                                  \
  } while(0)

#define BUF_TEST_DELETE(buf)                                           \
  do {                                                                 \
    buf_delete(buf);                                                   \
  } while(0)

#define BUF_TEST_F(test, expected)                                     \
  do {                                                                 \
    u64 pos = buf.wpos;                                                \
    uw len = strlen(expected);                                         \
    test_context(# test " -> " # expected);                            \
    TEST_EQ(test, len);                                                \
    TEST_EQ(buf.wpos, pos + len);                                      \
    TEST_STRNCMP(buf.ptr.pchar + pos, expected, len);                  \
  } while (0)

#define BUF_TEST_IGNORE(test, count, expected)                         \
  do {                                                                 \
    s_buf buf;                                                         \
    test_context("buf_ignore(" # test ", " # count ")");               \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_ignore(&buf, (count)), expected);                      \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)  
  
#define BUF_TEST_PEEK_1(test)                                          \
  do {                                                                 \
    s_buf buf;                                                         \
    test_context("buf_peek_1(" # test ")");                            \
    buf_init_1_const(&buf, (test));                                    \
    TEST_EQ(buf_peek_1(&buf, (test)), strlen(test));                   \
    TEST_EQ(buf.rpos, 0);                                              \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define BUF_TEST_PEEK_CHARACTER_UTF8(test, expected, c)                \
  do {                                                                 \
    character dest;                                                    \
    s_buf buf;                                                         \
    test_context("buf_peek_character_utf8(" # test ")");               \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_peek_character_utf8(&buf, &dest), (expected));         \
    TEST_EQ(buf.rpos, 0);                                              \
    TEST_EQ(dest, (c));                                                \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_TEST_READ_1(test)                                          \
  do {                                                                 \
    s_buf buf;                                                         \
    test_context("buf_read_1(" # test ")");                            \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_read_1(&buf, (test)), strlen(test));                   \
    TEST_EQ(buf.rpos, strlen(test));                                   \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define BUF_TEST_READ_1_(data, test, result)                           \
  do {                                                                 \
    s_buf buf;                                                         \
    test_context("buf_read_1(" # data ", " # test ") -> " # result);   \
    buf_init_1_const(&buf, (data));                                   \
    TEST_EQ(buf_read_1(&buf, (test)), (result));                       \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define BUF_TEST_READ_CHARACTER_UTF8(test, result)                     \
  do {                                                                 \
    character c;                                                       \
    s_buf buf;                                                         \
    test_context("buf_read_character_utf8(" # test ") -> " # result);  \
    buf_init_1_const(&buf, (test));                                   \
    TEST_ASSERT(buf_read_character_utf8(&buf, &c) >= 0);               \
    TEST_EQ(c, result);                                                \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define BUF_TEST_READ_N(test, n, result)                               \
  do {                                                                 \
    character c;                                                       \
    s_buf buf;                                                         \
    test_context("buf_read_n(" # test ", " # n ") -> " # result);      \
    buf_init_1_const(&buf, (test));                                   \
    TEST_ASSERT(buf_read_n(&buf, &c) >= 0);                            \
    TEST_EQ(c, result);                                                \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define BUF_TEST_READ_TO_STR(test)                                     \
  do {                                                                 \
    s_buf buf;                                                         \
    s_str result;                                                      \
    test_context("buf_read_to_str(" # test ")");                       \
    buf_init_1_const(&buf, (test));                                   \
    TEST_EQ(buf_read_to_str(&buf, &result), sizeof(test) - 1);         \
    TEST_EQ(buf.rpos, strlen(test));                                   \
    str_clean(&result);                                                \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(buf_f);
TEST_CASE_PROTOTYPE(buf_init_clean);
TEST_CASE_PROTOTYPE(buf_ignore);
TEST_CASE_PROTOTYPE(buf_new_delete);
TEST_CASE_PROTOTYPE(buf_new_alloc_delete);
TEST_CASE_PROTOTYPE(buf_peek_1);
TEST_CASE_PROTOTYPE(buf_peek_character_utf8);
TEST_CASE_PROTOTYPE(buf_peek_f32);
TEST_CASE_PROTOTYPE(buf_peek_f64);
TEST_CASE_PROTOTYPE(buf_peek_s8);
TEST_CASE_PROTOTYPE(buf_peek_s16);
TEST_CASE_PROTOTYPE(buf_peek_s32);
TEST_CASE_PROTOTYPE(buf_peek_s64);
TEST_CASE_PROTOTYPE(buf_peek_u8);
TEST_CASE_PROTOTYPE(buf_peek_u16);
TEST_CASE_PROTOTYPE(buf_peek_u32);
TEST_CASE_PROTOTYPE(buf_peek_u64);
TEST_CASE_PROTOTYPE(buf_read_1);
TEST_CASE_PROTOTYPE(buf_read_character_utf8);
TEST_CASE_PROTOTYPE(buf_read_f32);
TEST_CASE_PROTOTYPE(buf_read_f64);
TEST_CASE_PROTOTYPE(buf_read_s8);
TEST_CASE_PROTOTYPE(buf_read_s16);
TEST_CASE_PROTOTYPE(buf_read_s32);
TEST_CASE_PROTOTYPE(buf_read_s64);
TEST_CASE_PROTOTYPE(buf_read_to_str);
TEST_CASE_PROTOTYPE(buf_read_u8);
TEST_CASE_PROTOTYPE(buf_read_u16);
TEST_CASE_PROTOTYPE(buf_read_u32);
TEST_CASE_PROTOTYPE(buf_read_u64);
TEST_CASE_PROTOTYPE(buf_write_f32);
TEST_CASE_PROTOTYPE(buf_write_f64);
TEST_CASE_PROTOTYPE(buf_write_s8);
TEST_CASE_PROTOTYPE(buf_write_s16);
TEST_CASE_PROTOTYPE(buf_write_s32);
TEST_CASE_PROTOTYPE(buf_write_s64);
TEST_CASE_PROTOTYPE(buf_write_str);
TEST_CASE_PROTOTYPE(buf_write_u8);
TEST_CASE_PROTOTYPE(buf_write_u16);
TEST_CASE_PROTOTYPE(buf_write_u32);
TEST_CASE_PROTOTYPE(buf_write_u64);
TEST_CASE_PROTOTYPE(buf_xfer);

void buf_test (void)
{
  TEST_CASE_RUN(buf_init_clean);
  TEST_CASE_RUN(buf_new_delete);
  TEST_CASE_RUN(buf_new_alloc_delete);
  TEST_CASE_RUN(buf_write_u8);
  TEST_CASE_RUN(buf_write_u16);
  TEST_CASE_RUN(buf_write_u32);
  TEST_CASE_RUN(buf_write_u64);
  TEST_CASE_RUN(buf_write_s8);
  TEST_CASE_RUN(buf_write_s16);
  TEST_CASE_RUN(buf_write_s32);
  TEST_CASE_RUN(buf_write_s64);
  TEST_CASE_RUN(buf_write_str);
  TEST_CASE_RUN(buf_peek_1);
  TEST_CASE_RUN(buf_peek_character_utf8);
  TEST_CASE_RUN(buf_peek_s8);
  TEST_CASE_RUN(buf_peek_s16);
  TEST_CASE_RUN(buf_peek_u8);
  TEST_CASE_RUN(buf_read_1);
  TEST_CASE_RUN(buf_read_f32);
  TEST_CASE_RUN(buf_read_f64);
  TEST_CASE_RUN(buf_read_s8);
  TEST_CASE_RUN(buf_read_s16);
  TEST_CASE_RUN(buf_read_s32);
  TEST_CASE_RUN(buf_read_s64);
  TEST_CASE_RUN(buf_read_u8);
  TEST_CASE_RUN(buf_read_u16);
  TEST_CASE_RUN(buf_read_u32);
  TEST_CASE_RUN(buf_read_u64);
  TEST_CASE_RUN(buf_read_character_utf8);
  TEST_CASE_RUN(buf_read_to_str);
  TEST_CASE_RUN(buf_xfer);
  TEST_CASE_RUN(buf_f);
  TEST_CASE_RUN(buf_ignore);
}

TEST_CASE(buf_f)
{
  char b[32];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);          \
  BUF_TEST_F(buf_f(&buf, "09AZaz"), "09AZaz");
  BUF_TEST_F(buf_f(&buf, "%d", 0), "0");
  BUF_TEST_F(buf_f(&buf, "%d", 42), "42");
  BUF_TEST_F(buf_f(&buf, "%d", -1), "-1");
  BUF_TEST_F(buf_f(&buf, "%s", ""), "");
  BUF_TEST_F(buf_f(&buf, "%s", " "), " ");
  TEST_EQ(buf_f(&buf, "%s", "ÉoàΠ꒴𐅀 \n\r\t\v\\\""), -1);
  buf.wpos = 0;
  BUF_TEST_F(buf_f(&buf, "%s", "ÉoàΠ꒴𐅀 \n\r\t\v\\\""),
             "ÉoàΠ꒴𐅀 \n\r\t\v\\\"");
  buf_clean(&buf);
}
TEST_CASE_END(buf_f)

TEST_CASE(buf_ignore)
{
  BUF_TEST_IGNORE("", 0, 0);
  BUF_TEST_IGNORE("", 1, -1);
  BUF_TEST_IGNORE("", 2, -1);
  BUF_TEST_IGNORE("", 10, -1);
  BUF_TEST_IGNORE("1", 0, 0);
  BUF_TEST_IGNORE("1", 1, 1);
  BUF_TEST_IGNORE("1", 2, -1);
  BUF_TEST_IGNORE("1", 10, -1);
  BUF_TEST_IGNORE("12", 0, 0);
  BUF_TEST_IGNORE("12", 1, 1);
  BUF_TEST_IGNORE("12", 2, 2);
  BUF_TEST_IGNORE("12", 10, -1);
  BUF_TEST_IGNORE("1234567890", 0, 0);
  BUF_TEST_IGNORE("1234567890", 1, 1);
  BUF_TEST_IGNORE("1234567890", 2, 2);
  BUF_TEST_IGNORE("1234567890", 10, 10);
}
TEST_CASE_END(buf_ignore)

TEST_CASE(buf_init_clean)
{
  char a[] = "test";
  size_t len;
  char *m;
  s_buf buf;
  len = 4;
  buf_init(&buf, false, len, a);
  TEST_EQ(buf.size, len);
  TEST_EQ(strncmp(buf.ptr.p, "test", len), 0);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 0);
  BUF_TEST_CLEAN(buf);
  len = 4;
  m = malloc(len);
  memcpy(m, "test", len);
  buf_init(&buf, true, len, m);
  TEST_EQ(buf.size, len);
  TEST_EQ(strncmp(buf.ptr.p, "test", len), 0);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 0);
  BUF_TEST_CLEAN(buf);
}
TEST_CASE_END(buf_init_clean)

TEST_CASE(buf_new_delete)
{
  char a[] = "test";
  size_t len;
  char *m;
  s_buf *buf;
  len = 4;
  TEST_ASSERT((buf = buf_new(false, len, a)));
  TEST_EQ(buf->size, len);
  TEST_EQ(strncmp(buf->ptr.p, "test", len), 0);
  TEST_EQ(buf->rpos, 0);
  TEST_EQ(buf->wpos, 0);
  BUF_TEST_DELETE(buf);
  len = 4;
  m = malloc(len);
  memcpy(m, "test", len);
  TEST_ASSERT((buf = buf_new(true, len, m)));
  TEST_EQ(buf->size, len);
  TEST_EQ(strncmp(buf->ptr.p, "test", len), 0);
  TEST_EQ(buf->rpos, 0);
  TEST_EQ(buf->wpos, 0);
  BUF_TEST_DELETE(buf);
}
TEST_CASE_END(buf_new_delete)

TEST_CASE(buf_new_alloc_delete)
{
  size_t len;
  s_buf *buf;
  len = 1;
  while (len < 16) {
    TEST_ASSERT((buf = buf_new_alloc(len)));
    TEST_EQ(buf->size, len);
    TEST_EQ(buf->rpos, 0);
    TEST_EQ(buf->wpos, 0);
    BUF_TEST_DELETE(buf);
    len++;
  }
}
TEST_CASE_END(buf_new_alloc_delete)

TEST_CASE(buf_peek_1)
{
  BUF_TEST_PEEK_1("");
  BUF_TEST_PEEK_1("\x01");
  BUF_TEST_PEEK_1("\x01\x02");
  BUF_TEST_PEEK_1("\x01\x02\x03");
  BUF_TEST_PEEK_1("A");
  BUF_TEST_PEEK_1("AB");
  BUF_TEST_PEEK_1("ABC");
  BUF_TEST_PEEK_1("a");
  BUF_TEST_PEEK_1("ab");
  BUF_TEST_PEEK_1("abc");
  BUF_TEST_PEEK_1("À");
  BUF_TEST_PEEK_1("É");
  BUF_TEST_PEEK_1("Π");
  BUF_TEST_PEEK_1("Ꝝ");
  BUF_TEST_PEEK_1("꒴");
  BUF_TEST_PEEK_1("𐅀");
  BUF_TEST_PEEK_1("à");
  BUF_TEST_PEEK_1("é");
  BUF_TEST_PEEK_1("π");
  BUF_TEST_PEEK_1("ꝝ");
  BUF_TEST_PEEK_1("\x01\x02\x03" "ABCabcÀÉΠꝜ꒴𐅀àéπꝝ");
}
TEST_CASE_END(buf_peek_1)

TEST_CASE(buf_peek_character_utf8)
{
  BUF_TEST_PEEK_CHARACTER_UTF8("0", 1, '0');
}
TEST_CASE_END(buf_peek_character_utf8)

TEST_CASE(buf_peek_s8)
{
  char a[5] = "ABCD";
  s_buf buf;
  s8 byte;
  buf_init_const(&buf, sizeof(a), a);
  buf.wpos = 0;
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf_peek_s8(&buf, &byte), -1);
  TEST_EQ(buf.rpos, 0);
  buf.wpos = 1;
  TEST_EQ(buf_peek_s8(&buf, &byte), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf.wpos, 1);
  buf.rpos = 1;
  TEST_EQ(buf_peek_s8(&buf, &byte), -1);
  TEST_EQ(buf.rpos, 1);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf.wpos, 1);
  buf.wpos = 3;
  buf.rpos = 1;
  TEST_EQ(buf_peek_s8(&buf, &byte), 1);
  TEST_EQ(buf.rpos, 1);
  TEST_EQ(byte, 'B');
  TEST_EQ(buf.wpos, 3);
  buf_clean(&buf);
}
TEST_CASE_END(buf_peek_s8)

TEST_CASE(buf_peek_s16)
{
  char b[8];
  s_buf buf;
  s16 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s16(&buf, 0), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(buf_peek_s16(&buf, &val), 2);
  TEST_EQ(val, 0x0000);
  TEST_EQ(buf.rpos, 0);
  buf_clean(&buf);
}
TEST_CASE_END(buf_peek_s16)

TEST_CASE(buf_peek_u8)
{
  char a[] = "ABCD";
  s_buf buf;
  u8 byte;
  buf_init(&buf, false, sizeof(a), a);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf_peek_u8(&buf, &byte), -1);
  TEST_EQ(buf.rpos, 0);
  buf.wpos = 1;
  TEST_EQ(buf_peek_u8(&buf, &byte), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf.wpos, 1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_peek_u8)

TEST_CASE(buf_read_1)
{
  BUF_TEST_READ_1("");
  BUF_TEST_READ_1("\x01");
  BUF_TEST_READ_1("\x01\x02");
  BUF_TEST_READ_1("\x01\x02\x03");
  BUF_TEST_READ_1("A");
  BUF_TEST_READ_1("AB");
  BUF_TEST_READ_1("ABC");
  BUF_TEST_READ_1("a");
  BUF_TEST_READ_1("ab");
  BUF_TEST_READ_1("abc");
  BUF_TEST_READ_1("À");
  BUF_TEST_READ_1("É");
  BUF_TEST_READ_1("Π");
  BUF_TEST_READ_1("Ꝝ");
  BUF_TEST_READ_1("꒴");
  BUF_TEST_READ_1("𐅀");
  BUF_TEST_READ_1("à");
  BUF_TEST_READ_1("é");
  BUF_TEST_READ_1("π");
  BUF_TEST_READ_1("ꝝ");
  BUF_TEST_READ_1("\x01\x02\x03" "ABCabcÀÉΠꝜ꒴𐅀àéπꝝ");
  BUF_TEST_READ_1_("", "", 0);
  BUF_TEST_READ_1_("", "a", 0);
  BUF_TEST_READ_1_("", "abc", 0);
  BUF_TEST_READ_1_("a", "", 0);
  BUF_TEST_READ_1_("abc", "", 0);
}
TEST_CASE_END(buf_read_1)

TEST_CASE(buf_read_character_utf8)
{
  BUF_TEST_READ_CHARACTER_UTF8("0", '0');
  BUF_TEST_READ_CHARACTER_UTF8("9", '9');
  BUF_TEST_READ_CHARACTER_UTF8("A", 'A');
  BUF_TEST_READ_CHARACTER_UTF8("Z", 'Z');
  BUF_TEST_READ_CHARACTER_UTF8("a", 'a');
  BUF_TEST_READ_CHARACTER_UTF8("z", 'z');
  BUF_TEST_READ_CHARACTER_UTF8("Π", 928);
  BUF_TEST_READ_CHARACTER_UTF8("꒴", 42164);
  BUF_TEST_READ_CHARACTER_UTF8("𐅀", 65856);
}
TEST_CASE_END(buf_read_character_utf8)

TEST_CASE(buf_read_f32)
{
  char b[16];
  s_buf buf;
  f32 f;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_f32(&buf, 1.0f), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf_read_f32(&buf, &f), 4);
  TEST_FLOAT_EQ(f, 1.0f);
  TEST_EQ(buf_write_f32(&buf, 2.0f), 4);
  TEST_EQ(buf.rpos, 4);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_f32(&buf, &f), 4);
  TEST_FLOAT_EQ(f, 2.0f);
  TEST_EQ(buf_write_f32(&buf, 3402823466.0f), 4);
  TEST_EQ(buf.rpos, 8);
  TEST_EQ(buf.wpos, 12);
  TEST_EQ(buf_read_f32(&buf, &f), 4);
  TEST_FLOAT_EQ(f, 3402823466.0f);
  TEST_EQ(buf_write_f32(&buf, FLT_MAX), 4);
  TEST_EQ(buf.rpos, 12);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(buf_read_f32(&buf, &f), 4);
  TEST_FLOAT_EQ(f, FLT_MAX);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_character_utf8)

TEST_CASE(buf_read_f64)
{
  char b[32];
  s_buf buf;
  f64 f;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_f64(&buf, 1.0), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_f64(&buf, &f), 8);
  TEST_EQ(f, 1.0);
  TEST_EQ(buf_write_f64(&buf, 2.0), 8);
  TEST_EQ(buf.rpos, 8);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(buf_read_f64(&buf, &f), 8);
  TEST_EQ(f, 2.0);
  TEST_DOUBLE_EQ(buf_write_f64(&buf, DBL_MIN), 8);
  TEST_EQ(buf.rpos, 16);
  TEST_EQ(buf.wpos, 24);
  TEST_EQ(buf_read_f64(&buf, &f), 8);
  TEST_DOUBLE_EQ(f, DBL_MIN);
  TEST_EQ(buf_write_f64(&buf, DBL_MAX), 8);
  TEST_EQ(buf.rpos, 24);
  TEST_EQ(buf.wpos, 32);
  TEST_EQ(buf_read_f64(&buf, &f), 8);
  TEST_DOUBLE_EQ(f, DBL_MAX);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_f64)

TEST_CASE(buf_read_s8)
{
  char a[8] = "ABCDEFG";
  s_buf buf;
  s8 byte;
  buf_init_const(&buf, sizeof(a), a);
  buf.wpos = 0;
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  buf.wpos = 1;
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf.rpos, 1);
  TEST_EQ(buf.wpos, 1);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf.rpos, 1);
  TEST_EQ(buf.wpos, 1);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf.rpos, 1);
  TEST_EQ(buf.wpos, 1);
  buf.wpos = 5;
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(buf.rpos, 2);
  TEST_EQ(buf.wpos, 5);
  TEST_EQ(byte, 'B');
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(buf.rpos, 3);
  TEST_EQ(byte, 'C');
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(byte, 'D');
  TEST_EQ(buf.rpos, 4);
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(byte, 'E');
  TEST_EQ(buf.rpos, 5);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  buf.wpos = 7;
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(byte, 'F');
  TEST_EQ(buf.rpos, 6);
  TEST_EQ(buf_read_s8(&buf, &byte), 1);
  TEST_EQ(byte, 'G');
  TEST_EQ(buf.rpos, 7);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  TEST_EQ(buf_read_s8(&buf, &byte), -1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_s8)

TEST_CASE(buf_read_s16)
{
  char b[8];
  s_buf buf;
  s16 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s16(&buf, 0), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(buf_read_s16(&buf, &val), 2);
  TEST_EQ(val, 0x0000);
  TEST_EQ(buf_write_s16(&buf, 1), 2);
  TEST_EQ(buf.rpos, 2);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf_read_s16(&buf, &val), 2);
  TEST_EQ(val, 0x0001);
  TEST_EQ(buf_write_s16(&buf, 0x0100), 2);
  TEST_EQ(buf.rpos, 4);
  TEST_EQ(buf.wpos, 6);
  TEST_EQ(buf_read_s16(&buf, &val), 2);
  TEST_EQ(val, 0x0100);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_s16)

TEST_CASE(buf_read_s32)
{
  char b[16];
  s_buf buf;
  s32 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s32(&buf, 0x00000000), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf_read_s32(&buf, &val), 4);
  TEST_EQ(val, 0x00000000);
  TEST_EQ(buf_write_s32(&buf, 0x00000001), 4);
  TEST_EQ(buf.rpos, 4);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_s32(&buf, &val), 4);
  TEST_EQ(val, 0x00000001);
  TEST_EQ(buf_write_s32(&buf, SHRT_MIN), 4);
  TEST_EQ(buf.rpos, 8);
  TEST_EQ(buf.wpos, 12);
  TEST_EQ(buf_read_s32(&buf, &val), 4);
  TEST_EQ(val, SHRT_MIN);
  TEST_EQ(buf_write_s32(&buf, SHRT_MAX), 4);
  TEST_EQ(buf.rpos, 12);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(buf_read_s32(&buf, &val), 4);
  TEST_EQ(val, SHRT_MAX);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_s16)

TEST_CASE(buf_read_s64)
{
  char b[32];
  s_buf buf;
  s64 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s64(&buf, 0x0000000000000000), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_s64(&buf, &val), 8);
  TEST_EQ(val, 0x0000000000000000);
  TEST_EQ(buf_write_s64(&buf, 0x0000000000000001), 8);
  TEST_EQ(buf.rpos, 8);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(buf_read_s64(&buf, &val), 8);
  TEST_EQ(val, 0x0000000000000001);
  TEST_EQ(buf_write_s64(&buf, -65536), 8);
  TEST_EQ(buf.rpos, 16);
  TEST_EQ(buf.wpos, 24);
  TEST_EQ(buf_read_s64(&buf, &val), 8);
  TEST_EQ(val, -65536);
  TEST_EQ(buf_write_s64(&buf, 65534), 8);
  TEST_EQ(buf.rpos, 24);
  TEST_EQ(buf.wpos, 32);
  TEST_EQ(buf_read_s64(&buf, &val), 8);
  TEST_EQ(val, 65534);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_s16)

TEST_CASE(buf_read_to_str)
{
  BUF_TEST_READ_TO_STR("");
  BUF_TEST_READ_TO_STR("0");
  BUF_TEST_READ_TO_STR("9");
  BUF_TEST_READ_TO_STR("A");
  BUF_TEST_READ_TO_STR("Z");
  BUF_TEST_READ_TO_STR("À");
  BUF_TEST_READ_TO_STR("É");
  BUF_TEST_READ_TO_STR("a");
  BUF_TEST_READ_TO_STR("z");
  BUF_TEST_READ_TO_STR("à");
  BUF_TEST_READ_TO_STR("é");
  BUF_TEST_READ_TO_STR("09AZÀÉazàé");
}
TEST_CASE_END(buf_read_to_str)

TEST_CASE(buf_read_u8)
{
  s_buf buf;
  u8 byte;
  buf_init_1_const(&buf, "ABCDEFGH");
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'A');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'B');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'C');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'D');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'E');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'F');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'G');
  TEST_EQ(buf_read_u8(&buf, &byte), 1);
  TEST_EQ(byte, 'H');
  TEST_EQ(buf_read_u8(&buf, &byte), -1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_u8)

TEST_CASE(buf_read_u16)
{
  char b[8];
  s_buf buf;
  u16 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u16(&buf, 0x0000), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(buf_read_u16(&buf, &val), 2);
  TEST_EQ(val, 0x0000);
  TEST_EQ(buf_write_u16(&buf, 0x0001), 2);
  TEST_EQ(buf.rpos, 2);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf_read_u16(&buf, &val), 2);
  TEST_EQ(val, 0x0001);
  TEST_EQ(buf_write_u16(&buf, 0xFF00), 2);
  TEST_EQ(buf.rpos, 4);
  TEST_EQ(buf.wpos, 6);
  TEST_EQ(buf_read_u16(&buf, &val), 2);
  TEST_EQ(val, 0xFF00);
  TEST_EQ(buf_write_u16(&buf, 0xFFFF), 2);
  TEST_EQ(buf.rpos, 6);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_u16(&buf, &val), 2);
  TEST_EQ(val, 0xFFFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_u8)

TEST_CASE(buf_read_u32)
{
  char b[16];
  s_buf buf;
  u32 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u32(&buf, 0x00000000), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf_read_u32(&buf, &val), 4);
  TEST_EQ(val, 0x00000000);
  TEST_EQ(buf_write_u32(&buf, 0x00000001), 4);
  TEST_EQ(buf.rpos, 4);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_u32(&buf, &val), 4);
  TEST_EQ(val, 0x00000001);
  TEST_EQ(buf_write_u32(&buf, 0xFF000000), 4);
  TEST_EQ(buf.rpos, 8);
  TEST_EQ(buf.wpos, 12);
  TEST_EQ(buf_read_u32(&buf, &val), 4);
  TEST_EQ(val, 0xFF000000);
  TEST_EQ(buf_write_u32(&buf, 0xFFFFFFFF), 4);
  TEST_EQ(buf.rpos, 12);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(buf_read_u32(&buf, &val), 4);
  TEST_EQ(val, 0xFFFFFFFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_u8)

TEST_CASE(buf_read_u64)
{
  char b[32];
  s_buf buf;
  u64 val;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u64(&buf, 0x0000000000000000), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(buf_read_u64(&buf, &val), 8);
  TEST_EQ(val, 0x0000000000000000);
  TEST_EQ(buf_write_u64(&buf, 0x0000000000000001), 8);
  TEST_EQ(buf.rpos, 8);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(buf_read_u64(&buf, &val), 8);
  TEST_EQ(val, 0x0000000000000001);
  TEST_EQ(buf_write_u64(&buf, 0xFF00000000000000), 8);
  TEST_EQ(buf.rpos, 16);
  TEST_EQ(buf.wpos, 24);
  TEST_EQ(buf_read_u64(&buf, &val), 8);
  TEST_EQ(val, 0xFF00000000000000);
  TEST_EQ(buf_write_u64(&buf, 0xFFFFFFFFFFFFFFFF), 8);
  TEST_EQ(buf.rpos, 24);
  TEST_EQ(buf.wpos, 32);
  TEST_EQ(buf_read_u64(&buf, &val), 8);
  TEST_EQ(val, 0xFFFFFFFFFFFFFFFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_read_u8)

TEST_CASE(buf_write_s8)
{
  char b[4];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s8(&buf, 0x00), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 1);
  TEST_EQ(buf.ptr.ps8[0], 0x00);
  TEST_EQ(buf_write_s8(&buf, 0x01), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(buf.ptr.ps8[1], 0x01);
  TEST_EQ(buf_write_s8(&buf, -2), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 3);
  TEST_EQ(buf.ptr.ps8[2], -2);
  TEST_EQ(buf_write_s8(&buf, -1), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf.ptr.ps8[3], -1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_s8)

TEST_CASE(buf_write_s16)
{
  char b[8];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s16(&buf, 0x0000), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(*((s16 *) buf.ptr.ps8), 0x0000);
  TEST_EQ(buf_write_s16(&buf, 0x0001), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(*((s16 *) (buf.ptr.ps8 + 2)), 0x0001);
  TEST_EQ(buf_write_s16(&buf, -2), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 6);
  TEST_EQ(*((s16 *) (buf.ptr.ps8 + 4)), -2);
  TEST_EQ(buf_write_s16(&buf, -1), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(*((s16 *) (buf.ptr.ps8 + 6)), -1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_s16)

TEST_CASE(buf_write_s32)
{
  char b[16];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s32(&buf, 0x00000000), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(*((s32 *) buf.ptr.ps8), 0x00000000);
  TEST_EQ(buf_write_s32(&buf, 0x00000001), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(*((s32 *) (buf.ptr.ps8 + 4)), 0x00000001);
  TEST_EQ(buf_write_s32(&buf, -2), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 12);
  TEST_EQ(*((s32 *) (buf.ptr.ps8 + 8)), -2);
  TEST_EQ(buf_write_s32(&buf, -1), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(*((s32 *) (buf.ptr.ps8 + 12)), -1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_s32)

TEST_CASE(buf_write_s64)
{
  char b[32];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_s64(&buf, 0x0000000000000000), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(*((s64 *) buf.ptr.ps8), 0x0000000000000000);
  TEST_EQ(buf_write_s64(&buf, 0x0000000000000001), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(*((s64 *) (buf.ptr.ps8 + 8)), 0x0000000000000001);
  TEST_EQ(buf_write_s64(&buf, -2), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 24);
  TEST_EQ(*((s64 *) (buf.ptr.ps8 + 16)), -2);
  TEST_EQ(buf_write_s64(&buf, -1), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 32);
  TEST_EQ(*((s64 *) (buf.ptr.ps8 + 24)), -1);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_s64)

TEST_CASE(buf_write_u8)
{
  char b[4];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u8(&buf, 0x00), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 1);
  TEST_EQ(buf.ptr.pu8[0], 0x00);
  TEST_EQ(buf_write_u8(&buf, 0x01), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(buf.ptr.pu8[1], 0x01);
  TEST_EQ(buf_write_u8(&buf, 0xFE), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 3);
  TEST_EQ(buf.ptr.pu8[2], 0xFE);
  TEST_EQ(buf_write_u8(&buf, 0xFF), 1);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf.ptr.pu8[3], 0xFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_u8)

TEST_CASE(buf_write_u16)
{
  char b[8];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u16(&buf, 0x0000), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 2);
  TEST_EQ(*((u16 *) buf.ptr.pu8), 0x0000);
  TEST_EQ(buf_write_u16(&buf, 0x0001), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(*((u16 *) (buf.ptr.pu8 + 2)), 0x0001);
  TEST_EQ(buf_write_u16(&buf, 0xFF00), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 6);
  TEST_EQ(*((u16 *) (buf.ptr.pu8 + 4)), 0xFF00);
  TEST_EQ(buf_write_u16(&buf, 0xFFFF), 2);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(*((u16 *) (buf.ptr.pu8 + 6)), 0xFFFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_u16)

TEST_CASE(buf_write_u32)
{
  char b[16];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u32(&buf, 0x00000000), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(*((u32 *) buf.ptr.pu8), 0x00000000);
  TEST_EQ(buf_write_u32(&buf, 0x00000001), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(*((u32 *) (buf.ptr.pu8 + 4)), 0x00000001);
  TEST_EQ(buf_write_u32(&buf, 0xFFFF0000), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 12);
  TEST_EQ(*((u32 *) (buf.ptr.pu8 + 8)), 0xFFFF0000);
  TEST_EQ(buf_write_u32(&buf, 0xFFFFFFFF), 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(*((u32 *) (buf.ptr.pu8 + 12)), 0xFFFFFFFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_u32)

TEST_CASE(buf_write_u64)
{
  char b[32];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_write_u64(&buf, 0x0000000000000000), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 8);
  TEST_EQ(*((u64 *) buf.ptr.pu8), 0x0000000000000000);
  TEST_EQ(buf_write_u64(&buf, 0x0000000000000001), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 16);
  TEST_EQ(*((u64 *) (buf.ptr.pu8 + 8)), 0x0000000000000001);
  TEST_EQ(buf_write_u64(&buf, 0xFFFFFFFF00000000), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 24);
  TEST_EQ(*((u64 *) (buf.ptr.pu8 + 16)), 0xFFFFFFFF00000000);
  TEST_EQ(buf_write_u64(&buf, 0xFFFFFFFFFFFFFFFF), 8);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, 32);
  TEST_EQ(*((u64 *) (buf.ptr.pu8 + 24)), 0xFFFFFFFFFFFFFFFF);
  buf_clean(&buf);
}
TEST_CASE_END(buf_write_u64)

TEST_CASE(buf_write_str)
{
}
TEST_CASE_END(buf_write_str)

TEST_CASE(buf_xfer)
{
  char d[17] = {0};
  s_buf dest;
  char s[] = "0123456789ABCDEF";
  s_buf src;
  buf_init(&src, false, sizeof(s), s);
  src.wpos = 16;
  buf_init(&dest, false, sizeof(d), d);
  TEST_EQ(buf_xfer(&dest, &src, 0), 0);
  TEST_EQ(buf_xfer(&dest, &src, 1), 1);
  TEST_EQ(buf_xfer(&dest, &src, 2), 2);
  TEST_EQ(buf_xfer(&dest, &src, 3), 3);
  TEST_EQ(buf_xfer(&dest, &src, 4), 4);
  TEST_EQ(buf_xfer(&dest, &src, 5), 5);
  TEST_EQ(buf_xfer(&dest, &src, 6), 0);
  TEST_EQ(buf_xfer(&dest, &src, 7), 0);
  TEST_EQ(buf_xfer(&dest, &src, 8), 0);
  buf_clean(&dest);
  buf_clean(&src);
}
TEST_CASE_END(buf_xfer)
