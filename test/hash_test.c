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
#include "../libkc3/rapidhash.h"
#include "test.h"

void hash_test (void);
TEST_CASE_PROTOTYPE(hash_rapid_stream);

void hash_test (void)
{
  TEST_CASE_RUN(hash_rapid_stream);
}

TEST_CASE(hash_rapid_stream)
{
  u8 data[600];
  s_hash_rapid hash;
  uw chunk;
  uw i;
  uw len;
  uw offset;
  uw size;
  u64 expected;

  i = 0;
  while (i < sizeof(data)) {
    data[i] = (u8) (i * 37 + i / 7);
    i++;
  }
  len = 0;
  while (len <= sizeof(data)) {
    expected = rapidhash(data, len);
    chunk = 1;
    while (chunk <= 128) {
      hash_rapid_init(&hash);
      offset = 0;
      while (offset < len) {
        size = len - offset;
        if (size > chunk)
          size = chunk;
        TEST_ASSERT(hash_rapid_update(&hash, data + offset, size));
        offset += size;
      }
      TEST_EQ(hash_rapid_final(&hash), expected);
      hash_rapid_clean(&hash);
      chunk++;
    }
    len++;
  }
}
TEST_CASE_END(hash_rapid_stream)
