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
#include "deserialize.h"

sw deserialize_bool (s_buf *buf, bool *dest);
sw  deserialize_character (s_buf *buf, character *c);
sw deserialize_list (s_buf *buf, s_list **list);
sw deserialize_tag (s_buf *buf,  s_tag *tag);
sw deserialize_tuple (s_buf *buf, s_tuple *tuple);
sw deserialize_s8 (s_buf *buf, s8 *x);
sw deserialize_s16 (s_buf *buf, s16 *x);
sw deserialize_s32 (s_buf *buf, s32 *x);
sw deserialize_s64 (s_buf *buf, s64 *x);
sw deserialize_str (s_buf *buf, s_str *str);
sw deserialize_sw (s_buf *buf, sw *x);

u8 * deserialize_u8 (s_buf *buf, u8 *x)
{
  if (buf_read_u8(&buf, x) <= 0)
    return NULL;
  return x;
}

sw deserialize_u16 (s_buf *buf, u16 *x);
sw deserialize_u32 (s_buf *buf, u32 *x);
sw deserialize_u64 (s_buf *buf, u64 *x);
sw deserialize_uw (s_buf *buf, uw *x);

#endif /* LIBKC3_DESERIALIZE_H */
