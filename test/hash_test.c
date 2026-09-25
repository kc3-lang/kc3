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
#include <string.h>
#include "../libkc3/primehash.h"
#include "test.h"

void hash_test (void);
TEST_CASE_PROTOTYPE(primehash_u32_simd);
TEST_CASE_PROTOTYPE(primehash_u32_stream);

void hash_test (void)
{
  TEST_CASE_RUN(primehash_u32_simd);
  TEST_CASE_RUN(primehash_u32_stream);
}

TEST_CASE(primehash_u32_simd)
{
  u8 data[513];
  u32 expected;
  uw i;
  s_str key = {0};

  i = 0;
  while (i < sizeof(data)) {
    data[i] = (u8) (i * 37U + i / 7U);
    i++;
  }
  key.ptr.p_pu8 = data;
  i = 0;
  while (i <= sizeof(data)) {
    key.size = i;
    expected = primehash_u32_scalar_inline(&key, 0x12345678U);
    TEST_EQ(primehash_u32_inline(&key, 0x12345678U), expected);
    TEST_EQ(primehash_u32(&key, 0x12345678U), expected);
    i++;
  }
}
TEST_CASE_END(primehash_u32_simd)

TEST_CASE(primehash_u32_stream)
{
  u8 data[513];
  u32 expected;
  t_hash hash;
  uw i;
  uw size;
  s_str key = {0};

  i = 0;
  while (i < sizeof(data)) {
    data[i] = (u8) (i * 37U + i / 7U);
    i++;
  }
  key.ptr.p_pu8 = data;
  key.size = sizeof(data);
  expected = primehash_u32_inline(&key, 0x89abcdefU);
  primehash_u32_init_inline(&hash, 0x89abcdefU);
  i = 0;
  while (i < sizeof(data)) {
    size = i % 23U + 1U;
    if (size > sizeof(data) - i)
      size = sizeof(data) - i;
    primehash_u32_update_inline(&hash, data + i, size);
    i += size;
  }
  TEST_EQ(primehash_u32_finalize_inline(&hash, 0x89abcdefU), expected);
}
TEST_CASE_END(primehash_u32_stream)
