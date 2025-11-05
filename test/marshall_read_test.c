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
#include "../libkc3/endian.h"
#include "../libkc3/file.h"
#include "../libkc3/inspect.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "../libkc3/tag_init.h"
#include "test.h"
#include "tag_test.h"

#define MARSHALL_READ_TEST(type, test, expected)                        \
  do {                                                                  \
    type tmp = {0};                                                     \
    s_str tmp_str = {{0}, sizeof(expected) - 1, {expected}};            \
    s_marshall_read mr = {0};                                           \
    s_str test_str = {0};                                               \
    test_context("marshall_read_" # type "(" # test ") -> "             \
                 # expected);                                           \
    TEST_EQ(marshall_read_init_1(&mr, (test), sizeof(test) - 1), &mr);  \
    TEST_EQ(marshall_read_## type (&mr, false, &tmp), &mr);             \
    marshall_read_clean(&mr);                                           \
    inspect_ ## type(_Generic(tmp,                                      \
                              f32:  tmp,                                \
                              f64:  tmp,                                \
                              s8:   tmp,                                \
                              s16:  tmp,                                \
                              s32:  tmp,                                \
                              s64:  tmp,                                \
                              u8:   tmp,                                \
                              u16:  tmp,                                \
                              u32:  tmp,                                \
                              u64:  tmp,                                \
                              default: &tmp), &test_str);               \
    TEST_STR_EQ(test_str, tmp_str);                                     \
    str_clean(&test_str);                                               \
    str_clean(&tmp_str);                                                \
    test_context(NULL);                                                 \
  } while (0)

#define MARSHALL_READ_TEST_TAG(test, expected)                          \
  do {                                                                  \
    s_marshall_read mr = {0};                                           \
    s_tag tag = {0};                                                    \
    s_tag tag_expected = {0};                                           \
    test_context("marshall_read_tag(" # test ") -> " # expected);       \
    TEST_ASSERT(tag_init_1(&tag_expected, (expected)));                 \
    TEST_EQ(marshall_read_init_1(&mr, (test), sizeof(test) - 1), &mr);  \
    TEST_EQ(marshall_read_tag(&mr, false, &tag), &mr);                  \
    TAG_TEST_EQ(&tag, &tag_expected);                                   \
    marshall_read_clean(&mr);                                           \
    tag_clean(&tag);                                                    \
    tag_clean(&tag_expected);                                           \
    test_context(NULL);                                                 \
  } while (0)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_read_bool);
TEST_CASE_PROTOTYPE(marshall_read_character);
TEST_CASE_PROTOTYPE(marshall_read_init_buf);
TEST_CASE_PROTOTYPE(marshall_read_init_file);
TEST_CASE_PROTOTYPE(marshall_read_init_str);
TEST_CASE_PROTOTYPE(marshall_read_plist);
TEST_CASE_PROTOTYPE(marshall_read_s8);
TEST_CASE_PROTOTYPE(marshall_read_s16);
TEST_CASE_PROTOTYPE(marshall_read_s32);
TEST_CASE_PROTOTYPE(marshall_read_s64);
TEST_CASE_PROTOTYPE(marshall_read_sw);
TEST_CASE_PROTOTYPE(marshall_read_tag);
TEST_CASE_PROTOTYPE(marshall_read_u8);
TEST_CASE_PROTOTYPE(marshall_read_u16);
TEST_CASE_PROTOTYPE(marshall_read_u32);
TEST_CASE_PROTOTYPE(marshall_read_u64);
TEST_CASE_PROTOTYPE(marshall_read_uw);

void marshall_read_test (void)
{
  TEST_CASE_RUN(marshall_read_bool);
  TEST_CASE_RUN(marshall_read_tag);
}

TEST_CASE(marshall_read_bool)
{
  MARSHALL_READ_TEST(bool,
                     "KC3MARSH"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x0a\x00\x00\x00\x00\x00\x00\x00"
                     "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                     "\x5f\x00",
                     "false");
  MARSHALL_READ_TEST(bool,
                     "KC3MARSH"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x0a\x00\x00\x00\x00\x00\x00\x00"
                     "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                     "\x5f\x01",
                     "true");
}
TEST_CASE_END(marshall_read_bool)

  TEST_CASE(marshall_read_plist)
{
  s_marshall_read mr = {0};
  s_tag expected = {0};
  s_tag test_tag = {0};
  const char test_str[] = "KC3MARSH"
    "\x02\x00\x00\x00\x00\x00\x00\x00"
    "\x15\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x10\x00\x10\x00\x00\x00\x00\x00"
    "\x00\x00\x00";
  TEST_EQ(marshall_read_init_1(&mr, test_str, sizeof(test_str) - 1),
          &mr);
  TEST_EQ(marshall_read_plist(&mr, false, &test_tag.data.plist), &mr);
  test_tag.type = TAG_PLIST;
  tag_init_plist_1(&expected, "[0, 1]");
  TAG_TEST_EQ(&test_tag, &expected);
  tag_clean(&test_tag);
  tag_clean(&expected);
}
TEST_CASE_END(marshall_read_plist)

  TEST_CASE(marshall_read_tag)
{
  // u8
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x00",
                         "0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x01",
                         "1");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x02",
                         "2");
  // u16
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1a\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x12"
                         "\x5f\x4b\x43\x33\x55\x31\x36\x5f"
                         "\x00\x01",
                         "256");
  // u32
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x13"
                         "\x5f\x4b\x43\x33\x55\x33\x32\x5f"
                         "\x00\x00\x01\x00",
                         "65536");
  // u64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x14"
                         "\x5f\x4b\x43\x33\x55\x36\x34\x5f"
                         "\xA3\x8D#\xD6\xE2S\x00\x00",
                         "92233720368547");
  // s8:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x10"
                         "\x5f\x4b\x43\x33\x53\x38\x5f\xff",
                         "-1");
  // s16:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1a\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0f"
                         "\x5f\x4b\x43\x33\x53\x31\x36\x5f"
                         "\x7f\xff",
                         "-129");
  // s32:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0e"
                         "\x5f\x4b\x43\x33\x53\x33\x32\x5f"
                         "\x00\x00\x00\x80",
                         "-2147483648");
  // s64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0d"
                         "\x5f\x4b\x43\x33\x53\x36\x34\x5f"
                         "\x5d\xf2\xa6\x63\x7e\xd0\xff\xff",
                         "-52233720368547");
  // f32
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\xdb\x0f\x49\x40",
                         "(F32) 3.1415927");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\x00\x00\x80\x3F",
                         "(F32) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\xFF\xFF\xFF\xFF",
                         "(F32) 4294967295.0");
  // f64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x18\x2d\x44\x54\xfb\x21\x09\x40",
                         "(F64) 3.141592653589793");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\xF0\x3F",
                         "(F64) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\xF0\x41",
                         "(F64) 4294967296.0");
#if HAVE_F80
  // f80
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 0.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x80\xff\x3f\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x5f\x32\xc2\x68\x21\xa2\xda\x0f"
                         "\xc9\x00\x40\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 3.1415926535897932384"
                         "28841971");
#endif
  // str
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0D\x00\x00\x00\x00\x00\x00\x00"
                         "\x23\x0B\x68\x65\x6C\x6C\x6F\x20"
                         "\x77\x6F\x72\x6C\x64",
                         "\"hello world\"");
  */

  // list
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x16\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x11\x02\x16\x2e\x00"
                         "\x00\x00\x00\x00\x00\x00\x16\x20"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "[1, 2]");
  */
  // Do block
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x10\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x03\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x11\x01\x11\x02\x11\x03",
                         "do\n"
                         "  1\n"
                         "  2\n"
                         "  3\n"
                         "end");
  */
  // Tuple
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x03\x16\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x11\x04\x16\x2e\x00"
                         "\x00\x00\x00\x00\x00\x00\x25\x03"
                         "\x00\x00\x00\x00\x00\x00\x00\x11"
                         "\x01\x11\x02\x16\x20\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "{1, 2, [3, 4]}");
  */
  // psym
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x1d\x01\x06\x00\x00\x00\x4d\x6f"
                         "\x64\x75\x6c\x65",
                         "Module");
  */
  // Map
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1B\x00\x00\x00\x00\x00\x00\x00"
                         "\x17\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x1D\x01\x01\x00\x00\x00\x61"
                         "\x11\x01\x1D\x01\x01\x00\x00\x00"
                         "\x62\x11\x02", "%{a: 1, b: 2}");
  */
  // Cfn
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x07\x00\x00\x00\x00\x00\x00\x00"
                         "\x6b\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x21\x00\x00\x00\x00\x00\x00"
                         "\x00\x29\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x33\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x03\x00\x00\x00"
                         "\x54\x61\x67\x43\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x07\x00\x00\x00"
                         "\x74\x61\x67\x5f\x61\x64\x64\x03"
                         "\x58\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x03\x00\x00\x00\x54\x61\x67"
                         "\x68\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x03\x00\x00\x00\x54\x61\x67"
                         "\x78\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x06\x00\x00\x00\x52\x65\x73"
                         "\x75\x6c\x74\x18\x20\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "cfn Tag \"tag_add\" (Tag, Tag, Result)");
  */
  // Call
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0e\x00\x00\x00\x00\x00\x00\x00"
                         "\x35\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x09\x00\x00\x00\x64\x65\x66"
                         "\x6d\x6f\x64\x75\x6c\x65\x04\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x02"
                         "\x00\x00\x00\x00\x00\x00\x00\x1d"
                         "\x01\x04\x00\x00\x00\x54\x65\x73"
                         "\x74\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x2e\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "defmodule Test do end");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x15\x00\x00\x00\x00\x00\x00\x00"
                         "\x2a\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x04\x00\x00\x00\x4c\x69\x73"
                         "\x74\x01\x07\x00\x00\x00\x72\x65"
                         "\x76\x65\x72\x73\x65\x04\x20\x00"
                         "\x00\x00\x00\x00\x00\x00\x29\x00"
                         "\x00\x00\x00\x00\x00\x00\x01\x00"
                         "\x00\x00\x00\x00\x00\x00\x16\x35"
                         "\x00\x00\x00\x00\x00\x00\x00\x35"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "List.reverse([])");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x34\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x3d\x01\x01"
                         "\x00\x00\x00\x61\x04\x20\x00\x00"
                         "\x00\x00\x00\x00\x00\x20\x00\x00"
                         "\x00\x00\x00\x00\x00\x02\x00\x00"
                         "\x00\x00\x00\x00\x00\x27\x26\x00"
                         "\x00\x00\x00\x00\x00\x00\x26\x00"
                         "\x00\x00\x00\x00\x00\x00\x11\x01"
                         "\x2c\x00\x00\x00\x00\x00\x00\x00",
                         "a = 1");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x0b\x00\x00\x00\x00\x00\x00\x00"
                         "\x84\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x2b\x00\x00\x00\x00"
                         "\x00\x00\x00\x33\x00\x00\x00\x00"
                         "\x00\x00\x00\x27\x3c\x00\x00\x00"
                         "\x00\x00\x00\x00\x44\x00\x00\x00"
                         "\x00\x00\x00\x00\x01\x01\x00\x00"
                         "\x00\x78\x16\x53\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x04\x65\x00\x00"
                         "\x00\x00\x00\x00\x00\x6d\x00\x00"
                         "\x00\x00\x00\x00\x00\x01\x01\x00"
                         "\x00\x00\x2a\x02\x00\x00\x00\x00"
                         "\x00\x00\x00\x27\x84\x00\x00\x00"
                         "\x00\x00\x00\x00\x8c\x00\x00\x00"
                         "\x00\x00\x00\x00\x01\x01\x00\x00"
                         "\x00\x78\x11\x02\x9c\x00\x00\x00"
                         "\x00\x00\x00\x00\x18\x20\x00\x00"
                         "\x00\x00\x00\x00\x00",
                         "fn (x) { x * 2 }");
  */
  // Ratio
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x15\x00\x00\x00\x00\x00\x00\x00"
                         "\x0b\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x02\x02\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x06",
                         "2/6");
  */
  // Quote
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x06\x00\x00\x00\x00\x00\x00\x00"
                         "\x26\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x2b\x22\x04"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x11\x01\x26\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "quote 1 + 1");
  */
  // Var
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x01\x02\x00\x00\x00\x55\x38"
                         "\x00\x21\x20\x00\x00\x00\x00\x00"
                         "\x00\x00",
                         "(U8) ?");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0a\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x01\x03\x00\x00\x00\x54\x61"
                         "\x67\x00\x21\x20\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "(Tag) ?");
  // Complex
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x07\x00\x00\x00\x00\x00\x00\x00"
                         "\x25\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x02\x00\x00\x00\x2b\x69\x04"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x11\x02\x27\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "1 +i 2");
  // Unquote
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x00"
                         "\x26\x11\x7b",
                         "unquote(123)");
  */
  // Time
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x48\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x27"
                         "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                         "\x5f\x00\x5f\x4b\x43\x33\x53\x57"
                         "\x5f\x5f\x4b\x43\x33\x53\x36\x34"
                         "\x5f\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x5f\x4b\x43\x33\x53\x57\x5f"
                         "\x5f\x4b\x43\x33\x53\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00",
                         "%Time{}");
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x12\x00\x00\x00\x00\x00\x00\x00"
                         "\x24\x00\xe8\x03\x00\x00\x00\x00"
                         "\x00\x00\xd0\x07\x00\x00\x00\x00"
                         "\x00\x00",
                         "%Time{tv_sec: 1000, tv_nsec: 2000}");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x4c\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x2b\x01\x01"
                         "\x00\x00\x00\x2b\x24\x01\x04\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x02"
                         "\x00\x00\x00\x00\x00\x00\x00\x11"
                         "\x01\x11\x01\x26\x00\x00\x00\x00"
                         "\x00\x00\x00\x04\x26\x00\x00\x00"
                         "\x00\x00\x00\x00\x26\x00\x00\x00"
                         "\x00\x00\x00\x00\x02\x00\x00\x00"
                         "\x00\x00\x00\x00\x11\x02\x11\x02"
                         "\x2c\x00\x00\x00\x00\x00\x00\x00",
                         "%Time{tv_sec: 1 + 1, tv_nsec: 2 + 2}");
  */
  // Ptr
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x2f\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x22"
                         "\x5f\x4b\x43\x33\x50\x54\x52\x5f"
                         "\x5f\x4b\x43\x33\x55\x57\x5f\x5f"
                         "\x4b\x43\x33\x55\x36\x34\x5f\x00"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "(Ptr) 0");
  // PtrFree
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x33\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x23"
                         "\x5f\x4b\x43\x33\x50\x54\x52\x46"
                         "\x52\x45\x45\x5f\x5f\x4b\x43\x33"
                         "\x55\x57\x5f\x5f\x4b\x43\x33\x55"
                         "\x36\x34\x5f\x00\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "(PtrFree) 0");

}
TEST_CASE_END(marshall_read_tag)
