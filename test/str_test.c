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
#include <errno.h>
#include <float.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/assert.h"
#include "compare_test.h"
#include "../libkc3/alloc.h"
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
    TEST_STRNCMP(str.ptr.p_pvoid, (expected), str.size);               \
    str_clean(&str);                                                   \
    str_delete(test_);                                                 \
    test_context(NULL);                                                \
  } while (0)

#define STR_TEST_INIT_FLOAT(name, type, test, expected)                \
  do {                                                                 \
    s_str str = {0};                                                   \
    test_context("str_init_" # name "(" # test ") -> " # expected);   \
    TEST_EQ(str_init_ ## name(&str, (type) (test)), &str);             \
    TEST_EQ(str.size, strlen(expected));                               \
    TEST_STRNCMP(str.ptr.p_pchar, (expected), str.size);               \
    str_clean(&str);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define STR_TEST_INIT_FLOAT_ROUND_TRIP(name, type, parse, test)        \
  do {                                                                 \
    char *end;                                                         \
    type expected = (test);                                            \
    s_str str = {0};                                                   \
    test_context("str_init_" # name "(" # test ") round trip");       \
    TEST_EQ(str_init_ ## name(&str, expected), &str);                  \
    TEST_EQ(strchr(str.ptr.p_pchar, 'e'), NULL);                       \
    TEST_EQ(strchr(str.ptr.p_pchar, 'E'), NULL);                       \
    TEST_EQ(parse(str.ptr.p_pchar, &end), expected);                   \
    TEST_EQ(*end, 0);                                                  \
    str_clean(&str);                                                   \
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
TEST_CASE_PROTOTYPE(str_init_subst);
TEST_CASE_PROTOTYPE(str_init_subst_eof);
TEST_CASE_PROTOTYPE(str_init_subst_tail);
TEST_CASE_PROTOTYPE(str_init_subst_matched_bytes);
TEST_CASE_PROTOTYPE(str_init_subst_size_malformed);
TEST_CASE_PROTOTYPE(str_init_subst_malformed);
TEST_CASE_PROTOTYPE(str_init_subst_empty_search);
TEST_CASE_PROTOTYPE(str_init_f32);
TEST_CASE_PROTOTYPE(str_init_f64);
#if HAVE_F80
TEST_CASE_PROTOTYPE(str_init_f80);
#endif
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
  TEST_CASE_RUN(str_init_subst);
  TEST_CASE_RUN(str_init_subst_eof);
  TEST_CASE_RUN(str_init_subst_tail);
  TEST_CASE_RUN(str_init_subst_matched_bytes);
  TEST_CASE_RUN(str_init_subst_size_malformed);
  TEST_CASE_RUN(str_init_subst_malformed);
  TEST_CASE_RUN(str_init_subst_empty_search);
  TEST_CASE_RUN(str_init_f32);
  TEST_CASE_RUN(str_init_f64);
#if HAVE_F80
  TEST_CASE_RUN(str_init_f80);
#endif
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
  m = alloc(len + 1);
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
  m = alloc(len + 1);
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
  m = alloc(len + 1);
  assert(m);
  memcpy(m, "test", len + 1);
  str_init_copy_1(&str, m);
  alloc_free(m);
  TEST_EQ(str.size, len);
  TEST_STRNCMP(str.ptr.p_pvoid, "test", len);
  str_clean(&str);
}
TEST_CASE_END(str_init_copy_1)

static int str_subst_child (char (*test)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(3);
    _exit(test());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

static char str_subst_check (const s_str *src, const s_str *search,
                            const s_str *replace, const s_str *expected)
{
  s_str dest;
  TEST_EQ(str_init_subst_size(src, search, replace), expected->size);
  TEST_EQ(str_init_subst(&dest, src, search, replace), &dest);
  TEST_EQ(dest.size, expected->size);
  if (dest.size) {
    TEST_MEMCMP(dest.ptr.p_pchar, expected->ptr.p_pchar, dest.size);
    TEST_EQ(dest.ptr.p_pu8[dest.size], 0);
  }
  str_clean(&dest);
  return 0;
}

static char str_subst_reject (const s_str *src, const s_str *search,
                             const s_str *replace, bool size_only)
{
  const s_str unchanged = STR("unchanged");
  s_str dest = unchanged;
  s_str *result;
  if (size_only) {
    TEST_EQ(str_init_subst_size(src, search, replace), -1);
    return 0;
  }
  result = str_init_subst(&dest, src, search, replace);
  if (result)
    str_clean(&dest);
  TEST_EQ(result, NULL);
  TEST_EQ(dest.free.p_pvoid, unchanged.free.p_pvoid);
  TEST_EQ(dest.ptr.p_pvoid, unchanged.ptr.p_pvoid);
  TEST_EQ(dest.size, unchanged.size);
  TEST_MEMCMP(dest.ptr.p_pchar, unchanged.ptr.p_pchar, dest.size);
  return 0;
}

static char str_subst_valid (void)
{
  const struct {
    s_str src;
    s_str search;
    s_str replace;
    s_str expected;
  } cases[] = {
    {STR(""), STR("a"), STR("b"), STR("")},
    {STR("abc"), STR("z"), STR("!"), STR("abc")},
    {STR("aba"), STR("a"), STR("xyz"), STR("xyzbxyz")},
    {STR("aaaa"), STR("aa"), STR("b"), STR("bb")},
    {STR("abc"), STR("abc"), STR("x"), STR("x")},
    {STR("aaa"), STR("a"), STR(""), STR("")},
    {STR("aba"), STR("a"), STR(""), STR("b")},
    {STR("a\nb"), STR("\n"), STR("_"), STR("a_b")},
    {STR("A\0B"), STR("\0"), STR("_"), STR("A_B")},
    {STR("\0a\0"), STR("a"), STR("X"), STR("\0X\0")},
    {STR("\xC3\xA9\xCE\xBB\xE2\x82\xAC\xF0\x9F\x98\x80"),
     STR("\xCE\xBB"), STR("!"),
     STR("\xC3\xA9!\xE2\x82\xAC\xF0\x9F\x98\x80")},
    {STR("\xC3\xA9\xC3\xA9"), STR("\xC3\xA9"), STR("\xCE\xBB"),
     STR("\xCE\xBB\xCE\xBB")},
    {STR("a"), STR("a"), STR("\xF0\x9F\x98\x80"),
     STR("\xF0\x9F\x98\x80")},
    {STR("\xC3\xA9"), STR("\xA9"), STR("!"), STR("\xC3\xA9")},
    {STR("\xF0\x9F\x98\x80"), STR("\x9F"), STR("!"),
     STR("\xF0\x9F\x98\x80")}
  };
  uw i;
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (str_subst_check(&cases[i].src, &cases[i].search,
                        &cases[i].replace, &cases[i].expected))
      return 1;
  }
  return 0;
}

static char str_subst_eof (void)
{
  const struct {
    s_str src;
    s_str expected;
  } cases[] = {
    {STR(""), STR("")},
    {STR("a"), STR("b")},
    {STR("xyz"), STR("xyz")},
    {STR("\xC3\xA9"), STR("\xC3\xA9")},
    {STR("\xC2"), STR("\xC2")},
    {STR("\xE2"), STR("\xE2")},
    {STR("\xE2\x82"), STR("\xE2\x82")},
    {STR("\xF0"), STR("\xF0")},
    {STR("\xF0\x90"), STR("\xF0\x90")},
    {STR("\xF0\x90\x8C"), STR("\xF0\x90\x8C")},
    {STR("x\xC2"), STR("x\xC2")},
    {STR("a\xE2\x82"), STR("b\xE2\x82")},
    {STR("\xC3\xA9\xF0\x90\x8C"), STR("\xC3\xA9\xF0\x90\x8C")},
    {STR("\xE2x"), STR("\xE2x")},
    {STR("\xF0xy"), STR("\xF0xy")}
  };
  uw i;
  const s_str replace = STR("b");
  const s_str search = STR("a");
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (str_subst_check(&cases[i].src, &search, &replace,
                        &cases[i].expected))
      return 1;
  }
  return 0;
}

static char str_subst_tail (void)
{
  const struct {
    s_str src;
    s_str search;
    s_str replace;
    s_str expected;
  } cases[] = {
    {STR("abc"), STR("zz"), STR("x"), STR("abc")},
    {STR("abX"), STR("ab"), STR("c"), STR("cX")},
    {STR("x"), STR("longer"), STR("!"), STR("x")},
    {STR(""), STR("longer"), STR("!"), STR("")},
    {STR("abX"), STR("ab"), STR(""), STR("X")},
    {STR("abXY"), STR("ab"), STR("123"), STR("123XY")},
    {STR("aba"), STR("ab"), STR("x"), STR("xa")},
    {STR("abab"), STR("ab"), STR(""), STR("")},
    {STR("\xC3\xA9X"), STR("\xC3\xA9"), STR("!"), STR("!X")},
    {STR("ab\0"), STR("ab"), STR("c"), STR("c\0")},
    {STR("ab\xFF"), STR("ab"), STR("c"), STR("c\xFF")},
    {STR("\xE2\x82"), STR("abc"), STR("!"), STR("\xE2\x82")}
  };
  uw i;
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (str_subst_check(&cases[i].src, &cases[i].search,
                        &cases[i].replace, &cases[i].expected))
      return 1;
  }
  return 0;
}

static char str_subst_matched_bytes (void)
{
  const struct {
    s_str src;
    s_str search;
    s_str replace;
    s_str expected;
  } cases[] = {
    {STR("\xFF"), STR("\xFF"), STR("x"), STR("x")},
    {STR("\xFF\xFF"), STR("\xFF"), STR("xy"), STR("xyxy")},
    {STR("\x80"), STR("\x80"), STR(""), STR("")},
    {STR("\xC2"), STR("\xC2"), STR("y"), STR("y")},
    {STR("\xE2\x82"), STR("\xE2\x82"), STR("z"), STR("z")},
    {STR("a\xFF" "a"), STR("\xFF"), STR("_"), STR("a_a")},
    {STR("a"), STR("a"), STR("\xFF"), STR("\xFF")},
    {STR("a"), STR("a"), STR("\xC2"), STR("\xC2")}
  };
  uw i;
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (str_subst_check(&cases[i].src, &cases[i].search,
                        &cases[i].replace, &cases[i].expected))
      return 1;
  }
  return 0;
}

static char str_subst_invalid (bool size_only)
{
  const s_str cases[] = {
    STR("\xFF"), STR("\x80"), STR("\xBF"), STR("\xF8"), STR("\xFE"),
    STR("\xC2x"), STR("\xE2(\xA1"), STR("\xE2\x82x"),
    STR("\xF0(\x8C\xBC"), STR("\xF0\x90(\xBC"),
    STR("\xF0\x90\x8Cx"),
    STR("prefix\xFF"), STR("a\xFF"), STR("\xFF" "a"),
    STR("a\x80" "a"), STR("\xC3\xA9\xFF")
  };
  u16 byte;
  char data[4];
  uw i;
  const s_str replace = STR("b");
  const s_str search = STR("a");
  s_str src;
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (str_subst_reject(cases + i, &search, &replace, size_only))
      return 1;
  }
  for (byte = 0x80; byte <= 0xFF; byte++) {
    data[0] = (char) byte;
    data[1] = 'x';
    data[2] = 'y';
    data[3] = 'z';
    str_init(&src, NULL, sizeof(data), data);
    if (str_subst_reject(&src, &search, &replace, size_only))
      return 1;
  }
  return 0;
}

static char str_subst_size_malformed (void)
{
  return str_subst_invalid(true);
}

static char str_subst_malformed (void)
{
  return str_subst_invalid(false);
}

static char str_subst_empty_search (void)
{
  const s_str cases[] = {
    STR(""), STR("abc"), STR("\xFF"), STR("\xC2"), STR("\xC3\xA9")
  };
  uw i;
  const s_str replace = STR("x");
  const s_str search = STR("");
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (str_subst_reject(cases + i, &search, &replace, true) ||
        str_subst_reject(cases + i, &search, &replace, false))
      return 1;
  }
  return 0;
}

TEST_CASE(str_init_subst)
{
  TEST_EQ(str_subst_child(str_subst_valid), 0);
}
TEST_CASE_END(str_init_subst)

TEST_CASE(str_init_subst_eof)
{
  TEST_EQ(str_subst_child(str_subst_eof), 0);
}
TEST_CASE_END(str_init_subst_eof)

TEST_CASE(str_init_subst_tail)
{
  TEST_EQ(str_subst_child(str_subst_tail), 0);
}
TEST_CASE_END(str_init_subst_tail)

TEST_CASE(str_init_subst_matched_bytes)
{
  TEST_EQ(str_subst_child(str_subst_matched_bytes), 0);
}
TEST_CASE_END(str_init_subst_matched_bytes)

TEST_CASE(str_init_subst_size_malformed)
{
  TEST_EQ(str_subst_child(str_subst_size_malformed), 0);
}
TEST_CASE_END(str_init_subst_size_malformed)

TEST_CASE(str_init_subst_malformed)
{
  TEST_EQ(str_subst_child(str_subst_malformed), 0);
}
TEST_CASE_END(str_init_subst_malformed)

TEST_CASE(str_init_subst_empty_search)
{
  TEST_EQ(str_subst_child(str_subst_empty_search), 0);
}
TEST_CASE_END(str_init_subst_empty_search)

TEST_CASE(str_init_f32)
{
  STR_TEST_INIT_FLOAT(f32, f32, 0.0f, "0.0");
  STR_TEST_INIT_FLOAT(f32, f32, -0.0f, "0.0");
  STR_TEST_INIT_FLOAT(f32, f32, 0.1f, "0.1");
  STR_TEST_INIT_FLOAT(f32, f32, -0.1f, "-0.1");
  STR_TEST_INIT_FLOAT(f32, f32, 1.234567f, "1.234567");
  STR_TEST_INIT_FLOAT(f32, f32, 123456.0f, "123456.0");
  STR_TEST_INIT_FLOAT(f32, f32, 1.0e-31f,
                      "0.0000000000000000000000000000001");
  STR_TEST_INIT_FLOAT(f32, f32, 1.0e+33f,
                      "1000000000000000000000000000000000.0");
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f32, f32, strtof,
                                 0.000123456789f);
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f32, f32, strtof, FLT_MIN);
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f32, f32, strtof, FLT_MAX);
}
TEST_CASE_END(str_init_f32)

TEST_CASE(str_init_f64)
{
  STR_TEST_INIT_FLOAT(f64, f64, 0.0, "0.0");
  STR_TEST_INIT_FLOAT(f64, f64, -0.0, "0.0");
  STR_TEST_INIT_FLOAT(f64, f64, 0.1, "0.1");
  STR_TEST_INIT_FLOAT(f64, f64, -0.1, "-0.1");
  STR_TEST_INIT_FLOAT(f64, f64, 1.23456789, "1.23456789");
  STR_TEST_INIT_FLOAT(f64, f64, 123456789.0, "123456789.0");
  STR_TEST_INIT_FLOAT(f64, f64, 1.0e-31,
                      "0.0000000000000000000000000000001");
  STR_TEST_INIT_FLOAT(f64, f64, 1.0e+33,
                      "1000000000000000000000000000000000.0");
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f64, f64, strtod,
                                 0.00012345678901234567);
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f64, f64, strtod, DBL_MIN);
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f64, f64, strtod, DBL_MAX);
}
TEST_CASE_END(str_init_f64)

#if HAVE_F80
TEST_CASE(str_init_f80)
{
  STR_TEST_INIT_FLOAT(f80, f80, 0.0L, "0.0");
  STR_TEST_INIT_FLOAT(f80, f80, -0.0L, "0.0");
  STR_TEST_INIT_FLOAT(f80, f80, 0.1L, "0.1");
  STR_TEST_INIT_FLOAT(f80, f80, -0.1L, "-0.1");
  STR_TEST_INIT_FLOAT(f80, f80, 1.234567890123456789L,
                      "1.234567890123456789");
  STR_TEST_INIT_FLOAT(f80, f80, 123456789.0L, "123456789.0");
  STR_TEST_INIT_FLOAT(f80, f80, 1.0e-31L,
                      "0.0000000000000000000000000000001");
  STR_TEST_INIT_FLOAT(f80, f80, 1.0e+33L,
                      "1000000000000000000000000000000000.0");
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f80, f80, strtold,
                                 0.0001234567890123456789L);
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f80, f80, strtold, LDBL_MIN);
  STR_TEST_INIT_FLOAT_ROUND_TRIP(f80, f80, strtold, LDBL_MAX);
}
TEST_CASE_END(str_init_f80)
#endif

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
  m = alloc(len + 1);
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
  m = alloc(len + 1);
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
