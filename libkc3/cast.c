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
#include "assert.h"
#include "cast.h"

bool cast_needed_s8 (s8 i)
{
  return i >= 0;
}

bool cast_needed_s16 (s16 i)
{
  return i >= S8_MIN;
}

bool cast_needed_s32 (s32 i)
{
  return i >= S16_MIN;
}

bool cast_needed_s64 (s64 i)
{
  return i >= S32_MIN;
}

bool cast_needed_sw (sw i)
{
  (void) i;
  return true;
}

bool cast_needed_u8 (u8 i)
{
  (void) i;
  return false;
}

bool cast_needed_u16 (u16 i)
{
  return i <= U8_MAX;
}

bool cast_needed_u32 (u32 i)
{
  return i <= U16_MAX;
}

bool cast_needed_u64 (u64 i)
{
  return i <= U32_MAX;
}

bool cast_needed_uw (uw i)
{
  (void) i;
  return true;
}
