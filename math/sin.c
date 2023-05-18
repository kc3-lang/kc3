/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "sin.h"

s8 sin_u8 (u8 x)
{
  s16 i;
  u8 n;
  u8 side_bits;
  s8 y;
  side_bits = (sizeof(u8) * 8 - 3);
  n = (1 << side_bits) - 1;
  switch (x >> side_bits) {
  case 0:
    i = x & n;
    y = (S16_MAX) i * S8_MAX / n * S8_MAX / sqrt(n * n + i * i / n / n) / S8_MAX;
    return y;
  case 1:
    return 1;
  case 2:
    return 2;
  case 3:
    return 3;
  case 4:
    return 4;
  case 5:
    return 5;
  case 6:
    return 6;
  case 7:
    return 7;
  }
  assert(! "sin_u8: error");
  errx(1, "sin_u8");
  return 0;
}
