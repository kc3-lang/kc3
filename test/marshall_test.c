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

#include <endian.h>
#include "../libkc3/file.h"
#include "../libkc3/marshall.h"
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "test.h"

#define MARSHALL_TEST(type, on_heap, test, expected)                  \
  do {                                                                \
    s_marshall m = {0};                                               \
    const s_str expected_str = STR_1(expected);                       \
    s_str test_str = {0};                                             \
    test_context("marshall_" # type "(&m, " # on_heap ", " # test ")" \
                 " -> " # expected);                                  \
    TEST_ASSERT(marshall_init(&m));                                   \
    TEST_ASSERT(marshall_ ## type (&m, on_heap, (type) (test)));      \
    TEST_ASSERT(marshall_to_str(&m, &test_str));                      \
    TEST_STR_EQ(test_str, expected_str);                              \
    marshall_clean(&m);                                               \
    test_context(NULL);                                               \
  } while (0)

#define MARSHALL_TEST_STR(on_heap, test, expected)                    \
  do {                                                                \
    s_str str_expected = STR_1(expected);                             \
    s_str str_test = {0};                                             \
    s_str str_result = {0};                                           \
    test_context("marshall_str(&m, " # on_heap ", " # test ")"        \
                 " -> " # expected);                                  \
    TEST_EQ(marshall_init(&m), &m);                                   \
    TEST_EQ(str_init(&str_test, NULL, sizeof(test) - 1, (test)),      \
            &str_test);                                               \
    TEST_EQ(marshall_str(&m, (on_heap), &str_test), &m);              \
    TEST_EQ(marshall_to_str(&m, &str_result), &str_result);           \
    TEST_STR_EQ(str_result, str_expected);                            \
    str_clean(&str_result);                                           \
    str_clean(&str_test);                                             \
    marshall_clean(&m);                                               \
    test_context(NULL);                                               \
  } while (0)

#define MARSHALL_TEST_TAG(on_heap, test, expected)                    \
  do {                                                                \
    s_marshall m = {0};                                               \
    const s_str expected_str = STR_1(expected);                       \
    s_tag tag = {0};                                                  \
    s_str test_str = {0};                                             \
    test_context("marshall_tag(" # on_heap ", " # test ") -> "        \
                 # expected);                                         \
    TEST_ASSERT(tag_init_1(&tag, test));                              \
    TEST_EQ(marshall_init(&m), &m);                                   \
    TEST_EQ(marshall_tag(&m, (on_heap), &tag), &m);                   \
    TEST_ASSERT(marshall_to_str(&m, &test_str));                      \
    TEST_STR_EQ(test_str, expected_str);                              \
    marshall_clean(&m);                                               \
    tag_clean(&tag);                                                  \
    test_context(NULL);                                               \
  } while (0)

#define MARSHALL_TEST_BOOL_BUF(test, expected)  \
  MARSHALL_TEST(bool, false, test, expected)

#define MARSHALL_TEST_BOOL_HEAP(test, expected) \
  MARSHALL_TEST(bool, true, test, expected)

#define MARSHALL_TEST_CHARACTER_BUF(test, expected)     \
  MARSHALL_TEST(character, false, test, expected)

#define MARSHALL_TEST_CHARACTER_HEAP(test, expected)    \
  MARSHALL_TEST(character, true, test, expected)

#define MARSHALL_TEST_S8_BUF(test, expected)    \
  MARSHALL_TEST(s8, false, test, expected)

#define MARSHALL_TEST_S8_HEAP(test, expected)   \
  MARSHALL_TEST(s8, true, test, expected)

#define MARSHALL_TEST_S16_BUF(test, expected)   \
  MARSHALL_TEST(s16, false, test, expected)

#define MARSHALL_TEST_S16_HEAP(test, expected)  \
  MARSHALL_TEST(s16, true, test, expected)

#define MARSHALL_TEST_S32_BUF(test, expected)   \
  MARSHALL_TEST(s32, false, test, expected)

#define MARSHALL_TEST_S32_HEAP(test, expected)  \
  MARSHALL_TEST(s32, true, test, expected)

#define MARSHALL_TEST_S64_BUF(test, expected)   \
  MARSHALL_TEST(s64, false, test, expected)

#define MARSHALL_TEST_S64_HEAP(test, expected)  \
  MARSHALL_TEST(s64, true, test, expected)

#define MARSHALL_TEST_STR_BUF(test, expected)   \
  MARSHALL_TEST_STR(false, test, expected)

#define MARSHALL_TEST_STR_HEAP(test, expected)  \
  MARSHALL_TEST_STR(true, test, expected)

#define MARSHALL_TEST_SW_BUF(test, expected)    \
  MARSHALL_TEST(sw, false, test, expected)

#define MARSHALL_TEST_SW_HEAP(test, expected)   \
  MARSHALL_TEST(sw, true, test, expected)

#define MARSHALL_TEST_TAG_BUF(test, expected)   \
  MARSHALL_TEST_TAG(false, test, expected)

#define MARSHALL_TEST_TAG_HEAP(test, expected)   \
  MARSHALL_TEST_TAG(true, test, expected)

#define MARSHALL_TEST_U8_BUF(test, expected)    \
  MARSHALL_TEST(u8, false, test, expected)

#define MARSHALL_TEST_U8_HEAP(test, expected)   \
  MARSHALL_TEST(u8, true, test, expected)

#define MARSHALL_TEST_U16_BUF(test, expected)   \
  MARSHALL_TEST(u16, false, test, expected)

#define MARSHALL_TEST_U16_HEAP(test, expected)  \
  MARSHALL_TEST(u16, true, test, expected)

#define MARSHALL_TEST_U32_BUF(test, expected)   \
  MARSHALL_TEST(u32, false, test, expected)

#define MARSHALL_TEST_U32_HEAP(test, expected)  \
  MARSHALL_TEST(u32, true, test, expected)

#define MARSHALL_TEST_U64_BUF(test, expected)   \
  MARSHALL_TEST(u64, false, test, expected)

#define MARSHALL_TEST_U64_HEAP(test, expected)  \
  MARSHALL_TEST(u64, true, test, expected)

#define MARSHALL_TEST_UW_BUF(test, expected)    \
  MARSHALL_TEST(uw, false, test, expected)

#define MARSHALL_TEST_UW_HEAP(test, expected)   \
  MARSHALL_TEST(uw, true, test, expected)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_bool);
TEST_CASE_PROTOTYPE(marshall_character);
TEST_CASE_PROTOTYPE(marshall_s8);
TEST_CASE_PROTOTYPE(marshall_s16);
TEST_CASE_PROTOTYPE(marshall_s32);
TEST_CASE_PROTOTYPE(marshall_s64);
TEST_CASE_PROTOTYPE(marshall_sw);
TEST_CASE_PROTOTYPE(marshall_to_buf);
TEST_CASE_PROTOTYPE(marshall_to_file);
TEST_CASE_PROTOTYPE(marshall_to_str);
TEST_CASE_PROTOTYPE(marshall_u8);
TEST_CASE_PROTOTYPE(marshall_u16);
TEST_CASE_PROTOTYPE(marshall_u32);
TEST_CASE_PROTOTYPE(marshall_u64);
TEST_CASE_PROTOTYPE(marshall_uw);
TEST_CASE_PROTOTYPE(marshall_plist);
TEST_CASE_PROTOTYPE(marshall_plist_twice);
TEST_CASE_PROTOTYPE(marshall_str);
TEST_CASE_PROTOTYPE(marshall_tag);

void marshall_test (void)
{
  TEST_CASE_RUN(marshall_bool);
  TEST_CASE_RUN(marshall_character);
  TEST_CASE_RUN(marshall_s8);
  TEST_CASE_RUN(marshall_s16);
  TEST_CASE_RUN(marshall_s32);
  TEST_CASE_RUN(marshall_s64);
  TEST_CASE_RUN(marshall_sw);
  TEST_CASE_RUN(marshall_to_buf);
  TEST_CASE_RUN(marshall_to_str);
  TEST_CASE_RUN(marshall_to_file);
  TEST_CASE_RUN(marshall_u8);
  TEST_CASE_RUN(marshall_u16);
  TEST_CASE_RUN(marshall_u32);
  TEST_CASE_RUN(marshall_u64);
  TEST_CASE_RUN(marshall_uw);
  TEST_CASE_RUN(marshall_plist);
  TEST_CASE_RUN(marshall_plist_twice);
  TEST_CASE_RUN(marshall_str);
  TEST_CASE_RUN(marshall_tag);
}

TEST_CASE(marshall_bool)
{
  MARSHALL_TEST_BOOL_BUF(false,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x00\x00\x00\x00\x00\x00\x00"
                         "\x00");
  MARSHALL_TEST_BOOL_BUF(true,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x00\x00\x00\x00\x00\x00\x00"
                         "\x01");
  MARSHALL_TEST_BOOL_HEAP(false,
                          "KC3MARSH"
                          "\x00\x00\x00\x00\x00\x00\x00\x00"
                          "\x01\x00\x00\x00\x00\x00\x00\x00"
                          "\x00\x00\x00\x00\x00\x00\x00\x00"
                          "\x00");
  MARSHALL_TEST_BOOL_HEAP(true,
                          "KC3MARSH"
                          "\x00\x00\x00\x00\x00\x00\x00\x00"
                          "\x01\x00\x00\x00\x00\x00\x00\x00"
                          "\x00\x00\x00\x00\x00\x00\x00\x00"
                          "\x01");
}
TEST_CASE_END(marshall_bool)

TEST_CASE(marshall_character)
{
  MARSHALL_TEST_CHARACTER_BUF('A',
                              "KC3MARSH"
                              "\x00\x00\x00\x00\x00\x00\x00\x00"
                              "\x00\x00\x00\x00\x00\x00\x00\x00"
                              "\x01\x00\x00\x00\x00\x00\x00\x00"
                              "A");
  MARSHALL_TEST_CHARACTER_BUF('\0',
                              "KC3MARSH"
                              "\x00\x00\x00\x00\x00\x00\x00\x00"
                              "\x00\x00\x00\x00\x00\x00\x00\x00"
                              "\x01\x00\x00\x00\x00\x00\x00\x00"
                              "\x00");
  MARSHALL_TEST_CHARACTER_BUF('Z',
                              "KC3MARSH"
                              "\x00\x00\x00\x00\x00\x00\x00\x00"
                              "\x00\x00\x00\x00\x00\x00\x00\x00"
                              "\x01\x00\x00\x00\x00\x00\x00\x00"
                              "Z");
  MARSHALL_TEST_CHARACTER_HEAP('A',
                               "KC3MARSH"
                               "\x00\x00\x00\x00\x00\x00\x00\x00"
                               "\x01\x00\x00\x00\x00\x00\x00\x00"
                               "\x00\x00\x00\x00\x00\x00\x00\x00"
                               "A");
  MARSHALL_TEST_CHARACTER_HEAP('\0',
                               "KC3MARSH"
                               "\x00\x00\x00\x00\x00\x00\x00\x00"
                               "\x01\x00\x00\x00\x00\x00\x00\x00"
                               "\x00\x00\x00\x00\x00\x00\x00\x00"
                               "\x00");
  MARSHALL_TEST_CHARACTER_HEAP('Z',
                               "KC3MARSH"
                               "\x00\x00\x00\x00\x00\x00\x00\x00"
                               "\x01\x00\x00\x00\x00\x00\x00\x00"
                               "\x00\x00\x00\x00\x00\x00\x00\x00"
                               "Z");
}
TEST_CASE_END(marshall_character)

TEST_CASE(marshall_plist)
{
  s_marshall m = {0};
  s_list *list_test;
  s_str str = {0};
  const s_str expected = STR_1("KC3MARSH"
                               "\x02\x00\x00\x00\x00\x00\x00\x00"
                               "\x16\x00\x00\x00\x00\x00\x00\x00"
                               "\x08\x00\x00\x00\x00\x00\x00\x00"
                               "\x11\x00\x16#\x00\x00\x00\x00"
                               "\x00\x00\x00\x11\x01\x16.\x00"
                               "\x00\x00\x00\x00\x00\x00 \x00"
                               "\x00\x00\x00\x00\x00\x00");

  TEST_ASSERT(marshall_init(&m));
  list_test = list_new_1("[0, 1]");
  TEST_ASSERT(list_test);
  TEST_ASSERT(marshall_plist(&m, false, list_test));
  marshall_to_str(&m, &str);
  TEST_STR_EQ(str, expected);
  str_clean(&str);
  list_delete_all(list_test);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_plist)

TEST_CASE(marshall_plist_twice)
{
  s_marshall m = {0};
  s_list *list_test;
  s_str str = {0};
  const s_str expected = STR_1("KC3MARSH"
                               "\x04\x00\x00\x00\x00\x00\x00\x00"
                               ",\x00\x00\x00\x00\x00\x00\x00"
                               "\x10\x00\x00\x00\x00\x00\x00\x00"
                               "\x11\x00\x16#\x00\x00\x00\x00"
                               "\x00\x00\x00\x11\x01\x16.\x00"
                               "\x00\x00\x00\x00\x00\x00\x11\x00"
                               "\x16\x39\x00\x00\x00\x00\x00\x00"
                               "\x00\x11\x01\x16\x44\x00\x00\x00"
                               "\x00\x00\x00\x00 \x00\x00\x00"
                               "\x00\x00\x00\x00\x36\x00\x00\x00"
                               "\x00\x00\x00\x00");

  TEST_ASSERT(marshall_init(&m));
  list_test = list_new_1("[0, 1]");
  TEST_ASSERT(list_test);
  TEST_ASSERT(marshall_plist(&m, false, list_test));
  TEST_ASSERT(marshall_plist(&m, false, list_test));
  marshall_to_str(&m, &str);
  TEST_STR_EQ(str, expected);
  str_clean(&str);
  list_delete_all(list_test);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_plist_twice)

TEST_CASE(marshall_s8)
{
  MARSHALL_TEST_S8_BUF(0,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x01\x00\x00\x00\x00\x00\x00\x00"
                       "\x00");
  MARSHALL_TEST_S8_BUF(0xAB,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x01\x00\x00\x00\x00\x00\x00\x00"
                       "\xAB");
  MARSHALL_TEST_S8_HEAP(0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x01\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00");
  MARSHALL_TEST_S8_HEAP(0xAB,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x01\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\xAB");
}
TEST_CASE_END(marshall_s8)

TEST_CASE(marshall_s16)
{
  MARSHALL_TEST_S16_BUF(0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00");
  MARSHALL_TEST_S16_BUF(-1,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF");
  MARSHALL_TEST_S16_HEAP(0,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00");
  MARSHALL_TEST_S16_HEAP(-1,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFF\xFF");
}
TEST_CASE_END(marshall_s16)

TEST_CASE(marshall_s32)
{
  MARSHALL_TEST_S32_BUF(0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00");
  MARSHALL_TEST_S32_BUF(~0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_S32_BUF(0xDEADBEEF,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\xEF\xBE\xAD\xDE");
  MARSHALL_TEST_S32_BUF(0xDEADCAFE,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\xFE\xCA\xAD\xDE");
  MARSHALL_TEST_S32_BUF(0xFEEDBABE,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\xBE\xBA\xED\xFE");
  MARSHALL_TEST_S32_HEAP(0,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x04\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00");
  MARSHALL_TEST_S32_HEAP(~0,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x04\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_S32_HEAP(0xDEADBEEF,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x04\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xEF\xBE\xAD\xDE");
  MARSHALL_TEST_S32_HEAP(0xDEADCAFE,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x04\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFE\xCA\xAD\xDE");
  MARSHALL_TEST_S32_HEAP(0xFEEDBABE,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x04\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xBE\xBA\xED\xFE");
}
TEST_CASE_END(marshall_s32)

TEST_CASE(marshall_s64)
{
  MARSHALL_TEST_S64_BUF(0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_S64_BUF(~0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_S64_HEAP(0,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x08\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_S64_HEAP(~0,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x08\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_s64)

TEST_CASE(marshall_str)
{
  s_marshall m = {0};
  MARSHALL_TEST_STR_BUF("",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00");
  MARSHALL_TEST_STR_BUF("hello",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\t\x00\x00\x00\x00\x00\x00\x00"
                        "\x05\x00\x00\x00hell"
                        "o");
  MARSHALL_TEST_STR_BUF("hello\xFF\x00world",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x10\x00\x00\x00\x00\x00\x00\x00"
                        "\x0C\x00\x00\x00hell"
                        "o\xFF\x00world");
  MARSHALL_TEST_STR_BUF("héllo wörld",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x11\x00\x00\x00\x00\x00\x00\x00"
                        "\r\x00\x00\x00héllo wörld");
}
TEST_CASE_END(marshall_str)

TEST_CASE(marshall_sw)
{
  MARSHALL_TEST_SW_BUF(0,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x08\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_SW_BUF(~0,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x08\x00\x00\x00\x00\x00\x00\x00"
                       "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_SW_HEAP(0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_SW_HEAP(~0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_sw)

TEST_CASE(marshall_tag)
{
  // U8
  MARSHALL_TEST_TAG_BUF("0",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\x11\x00");
  MARSHALL_TEST_TAG_BUF("1",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\x11\x01");
  MARSHALL_TEST_TAG_BUF("2",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\x11\x02");
  // U16
  MARSHALL_TEST_TAG_BUF("256",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x03\x00\x00\x00\x00\x00\x00\x00"
                        "\x12\x00\x01");
  // S32
  MARSHALL_TEST_TAG_BUF("-2147483648",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x05\x00\x00\x00\x00\x00\x00\x00"
                        "\x0E\x00\x00\x00\x80");
  // U64
  MARSHALL_TEST_TAG_BUF("92233720368547",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\t\x00\x00\x00\x00\x00\x00\x00"
                        "\x14\xA3\x8D#\xD6\xE2S\x00"
                        "\x00");
  // Integer
  MARSHALL_TEST_TAG_BUF("(Integer) 1024",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x0C\x00\x00\x00\x00\x00\x00\x00"
                        "\n\x03\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x04\x00");
  MARSHALL_TEST_TAG_BUF("10000000000000000000000000000000000",
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x19\x00\x00\x00\x00\x00\x00\x00"
                        "\n\x10\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x01\xED\t\xBE\xAD\x87"
                        "\xC0\x37\x8D\x8E\x64\x00\x00\x00"
                        "\x00");
  // List
  MARSHALL_TEST_TAG_BUF("[1, 2]",
                        "KC3MARSH"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\x16\x00\x00\x00\x00\x00\x00\x00"
                        "\t\x00\x00\x00\x00\x00\x00\x00"
                        "\x11\x01\x16#\x00\x00\x00\x00"
                        "\x00\x00\x00\x11\x02\x16.\x00"
                        "\x00\x00\x00\x00\x00\x00\x16 "
                        "\x00\x00\x00\x00\x00\x00\x00");
  // Map
  MARSHALL_TEST_TAG_BUF("%{a: 1, b: 2}",
                        "KC3MARSH\0");
  // Call
  MARSHALL_TEST_TAG_BUF("defmodule Test do end",
                        "KC3MARSH\0");
  MARSHALL_TEST_TAG_BUF("List.reverse([])",
                        "KC3MARSH\0");
  MARSHALL_TEST_TAG_BUF("a = 1",
                        "KC3MARSH\0");
}
TEST_CASE_END(marshall_tag)

TEST_CASE(marshall_to_buf)
{
  char b[1024024] = {0};
  s_buf buf = {0};
  s_marshall m = {0};
  buf_init(&buf, false, sizeof(b), b);
  TEST_ASSERT(marshall_init(&m) != NULL);
  TEST_EQ(marshall_to_buf(&m, &buf), sizeof(s_marshall_header));
  marshall_clean(&m);
}
TEST_CASE_END(marshall_to_buf)

TEST_CASE(marshall_to_file)
{
  u32 value = 42;
  s_marshall m = {0};
  s_str path = {0};
  TEST_ASSERT(marshall_init(&m));
  TEST_ASSERT(marshall_u32(&m, false, value));
  TEST_EQ(marshall_to_file(&m, ".marshall_test_to_file.1.kc3m"),
          sizeof(s_marshall_header) +
          sizeof(u32));
  TEST_EQ(test_file_compare(".marshall_test_to_file.1.kc3m",
                            "marshall_test_to_file.1.expected.kc3m"),
          0);
  TEST_EQ(str_init_1(&path, NULL, ".marshall_test_to_file.1.kc3m"),
          &path);
  TEST_ASSERT(file_unlink(&path));
  str_clean(&path);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_to_file)

TEST_CASE(marshall_to_str)
{
  u32 value = 424242;
  s_marshall m = {0};
  s_str str = {0};
  s_str expected = {0};
  TEST_ASSERT(marshall_init(&m));
  TEST_ASSERT(marshall_u32(&m, false, value));
  TEST_EQ(marshall_to_str(&m, &str), &str);
  marshall_clean(&m);
  expected = STR_1("KC3MARSH"
                   "\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x04\x00\x00\x00\x00\x00\x00\x00"
                   "\x32\x79\x06\x00");
  TEST_STR_EQ(str, expected);
  str_clean(&str);
}
TEST_CASE_END(marshall_to_str)

TEST_CASE(marshall_u8)
{
  MARSHALL_TEST_U8_BUF(0x00,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x01\x00\x00\x00\x00\x00\x00\x00"
                       "\x00");
  MARSHALL_TEST_U8_BUF(0xFF,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x01\x00\x00\x00\x00\x00\x00\x00"
                       "\xFF");
  MARSHALL_TEST_U8_HEAP(0x00,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x01\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00");
  MARSHALL_TEST_U8_HEAP(0xFF,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x01\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF");
}
TEST_CASE_END(marshall_u8)

TEST_CASE(marshall_u16)
{
  MARSHALL_TEST_U16_BUF(0xFFFF,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x02\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF");
  MARSHALL_TEST_U16_HEAP(0xFFFF,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFF\xFF");
}

TEST_CASE_END(marshall_u16)

TEST_CASE(marshall_u32)
{
  MARSHALL_TEST_U32_BUF(0xFFFFFFFF,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x04\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_U32_HEAP(0xFFFFFFFF,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x04\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u32)

TEST_CASE(marshall_u64)
{
  MARSHALL_TEST_U64_BUF(0xFFFFFFFFFFFFFFFF,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_U64_HEAP(0xFFFFFFFFFFFFFFFF,
                         "KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x08\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u64)

TEST_CASE(marshall_uw)
{
  MARSHALL_TEST_UW_BUF(0,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x08\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_UW_BUF(~0,
                       "KC3MARSH"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x00\x00\x00\x00\x00\x00\x00\x00"
                       "\x08\x00\x00\x00\x00\x00\x00\x00"
                       "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_UW_HEAP(0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_UW_HEAP(~0,
                        "KC3MARSH"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\x08\x00\x00\x00\x00\x00\x00\x00"
                        "\x00\x00\x00\x00\x00\x00\x00\x00"
                        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_uw)

