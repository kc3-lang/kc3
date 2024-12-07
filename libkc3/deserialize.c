/* kc3_main
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "buf.h"
#include "deserialize.h"
#include "map.h"

bool *      deserialize_bool (s_deserialize *ds, bool *dest);
character * deserialize_character (s_deserialize *ds, character *c);

s_deserialize * deserialize_init (s_deserialize *ds)
{
  s_deserialize tmp = {0};
  map_init(&tmp.map, 0);
  *ds = tmp;
  return ds;
}

s_list **   deserialize_list (s_deserialize *ds, s_list **list);
s_tag *     deserialize_tag (s_deserialize *ds,  s_tag *tag);
s_tuple *   deserialize_tuple (s_deserialize *ds, s_tuple *tuple);
s8 *        deserialize_s8 (s_deserialize *ds, s8 *x);
s16 *       deserialize_s16 (s_deserialize *ds, s16 *x);
s32 *       deserialize_s32 (s_deserialize *ds, s32 *x);
s64 *       deserialize_s64 (s_deserialize *ds, s64 *x);
s_str *     deserialize_str (s_deserialize *ds, s_str *str);
sw *        deserialize_sw (s_deserialize *ds, sw *x);

u8 * deserialize_u8 (s_deserialize *ds, u8 *x)
{
  if (buf_read_u8(&ds->buf, x) <= 0)
    return NULL;
  return x;
}

u16 * deserialize_u16 (s_deserialize *ds, u16 *x);
u32 * deserialize_u32 (s_deserialize *ds, u32 *x);
u64 * deserialize_u64 (s_deserialize *ds, u64 *x);
uw * deserialize_uw (s_deserialize *ds, uw *x);
