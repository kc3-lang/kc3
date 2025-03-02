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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "deserialize.h"
#include "map.h"
#include "tag_type.h"

bool * deserialize_bool (s_deserialize *ds, bool *dest)
{
  u8 x;
  deserialize_u8(ds, &x);
  *dest = x ? 1 : 0;
  return dest;
}

character * deserialize_character (s_deserialize *ds, character *c);

void deserialize_delete (s_deserialize *ds)
{
  //deserialize_clean(ds);
  free(ds);
}

bool deserialize_from_str (s_deserialize *ds, s_str *str)
{
  if (! buf_init_str(&ds->buf, false, str))
    return false;
  return true;
}

s_deserialize * deserialize_init (s_deserialize *ds)
{
  s_deserialize tmp = {0};
  map_init(&tmp.map, 0);
  *ds = tmp;
  return ds;
}

s_list **   deserialize_list (s_deserialize *ds, s_list **list);

s_deserialize * deserialize_new (void)
{
  s_deserialize *ds;
  ds = alloc(sizeof(s_deserialize));
  return ds;
}

s_tag * deserialize_tag (s_deserialize *ds, s_tag *tag)
{
  u8 u8;
  if (! deserialize_u8(ds, &u8))
    return NULL;
  tag->type = u8;
  switch (tag->type) {
  case TAG_U8:
    deserialize_u8(ds, &tag->data.u8);
    break;
  default:
    err_write_1("deserialize_tag: not implemented: ");
    err_write_1(tag_type_to_string(tag->type));
    err_write_1("\n");
    assert(! "deserialize_tag: not implemented");
    return NULL;
  }
  return tag;
}

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
