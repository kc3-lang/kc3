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
#include "integer.h"
#include "tag.h"

s_tag * tag_band (s_tag *a, s_tag *b, s_tag *dest)
{
  s_tag tag = {0};
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  assert(a);
  assert(b);
  assert(dest);
  switch (a->type) {
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_INTEGER:
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &b->data.integer,
                   &tag.data.integer);
      goto integer_reduce;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &tag.data.integer);
      goto integer_clean_tmp;
    default:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_sw(&tag, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      tag_init_sw(&tag, a->data.sw & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_sw(&tag, a->data.sw & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_sw(&tag, a->data.sw & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_sw(&tag, a->data.sw & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_sw(&tag, a->data.sw & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_sw(&tag, a->data.sw & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_sw(&tag, a->data.sw & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_sw(&tag, a->data.sw & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_sw(&tag, a->data.sw & b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_sw(&tag, a->data.sw & b->data.uw);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &b->data.integer, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_SW:
      tag_init_s64(&tag, a->data.s64 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s64(&tag, a->data.s64 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, a->data.s64 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s64(&tag, a->data.s64 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s64(&tag, a->data.s64 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_s64(&tag, a->data.s64 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s64(&tag, a->data.s64 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, a->data.s64 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &b->data.integer, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_SW:
      tag_init_sw(&tag, a->data.s32 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s64(&tag, a->data.s32 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s32(&tag, a->data.s32 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s32(&tag, a->data.s32 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s32(&tag, a->data.s32 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_s32(&tag, a->data.s32 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s32(&tag, a->data.s32 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, a->data.s32 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &b->data.integer, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_SW:
      tag_init_sw(&tag, a->data.s16 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s64(&tag, a->data.s16 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s32(&tag, a->data.s16 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s16(&tag, a->data.s16 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s16(&tag, a->data.s16 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_s16(&tag, a->data.s16 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s32(&tag, a->data.s16 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, a->data.s16 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &b->data.integer, &tag.data.integer);
      goto integer_clean_tmp;
    case TAG_SW:
      tag_init_sw(&tag, a->data.s8 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s64(&tag, a->data.s8 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s32(&tag, a->data.s8 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s16(&tag, a->data.s8 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s8(&tag, a->data.s8 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_s16(&tag, a->data.s8 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s32(&tag, a->data.s8 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, a->data.s8 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_band(&tmp, &tmp2, &tag.data.integer); 
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u8(&tag, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      tag_init_u8(&tag, a->data.u8 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_u8(&tag, a->data.u8 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_u8(&tag, a->data.u8 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_u8(&tag, a->data.u8 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_u8(&tag, a->data.u8 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u8(&tag, a->data.u8 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u8(&tag, a->data.u8 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u8(&tag, a->data.u8 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u8(&tag, a->data.u8 & b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u8(&tag, a->data.u8 & b->data.uw);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u16(&tag, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      tag_init_u16(&tag, a->data.u16 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_u16(&tag, a->data.u16 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_u16(&tag, a->data.u16 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_u16(&tag, a->data.u16 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_u16(&tag, a->data.u16 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u16(&tag, a->data.u16 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u16(&tag, a->data.u16 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u16(&tag, a->data.u16 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u16(&tag, a->data.u16 & b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u16(&tag, a->data.u16 & b->data.uw);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u32(&tag, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      tag_init_u32(&tag, a->data.u32 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_u32(&tag, a->data.u32 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_u32(&tag, a->data.u32 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_u32(&tag, a->data.u32 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_u32(&tag, a->data.u32 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u32(&tag, a->data.u32 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u32(&tag, a->data.u32 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u32(&tag, a->data.u32 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u32(&tag, a->data.u32 & b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u32(&tag, a->data.u32 & b->data.uw);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u64(&tag, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      tag_init_u64(&tag, a->data.u64 & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_u64(&tag, a->data.u64 & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_u64(&tag, a->data.u64 & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_u64(&tag, a->data.u64 & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_u64(&tag, a->data.u64 & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u64(&tag, a->data.u64 & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u64(&tag, a->data.u64 & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u64(&tag, a->data.u64 & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u64(&tag, a->data.u64 & b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u64(&tag, a->data.u64 & b->data.uw);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_uw(&tag, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      tag_init_uw(&tag, a->data.uw & b->data.sw);
      goto integer_reduce;
    case TAG_S64:
      tag_init_uw(&tag, a->data.uw & b->data.s64);
      goto integer_reduce;
    case TAG_S32:
      tag_init_uw(&tag, a->data.uw & b->data.s32);
      goto integer_reduce;
    case TAG_S16:
      tag_init_uw(&tag, a->data.uw & b->data.s16);
      goto integer_reduce;
    case TAG_S8:
      tag_init_uw(&tag, a->data.uw & b->data.s8);
      goto integer_reduce;
    case TAG_U8:
      tag_init_uw(&tag, a->data.uw & b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_uw(&tag, a->data.uw & b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_uw(&tag, a->data.uw & b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_uw(&tag, a->data.uw & b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_uw(&tag, a->data.uw & b->data.uw);
      goto integer_reduce;
    default:
      goto error;
    }
    goto error;
  default:
    goto error;
  }
 error:
  err_write_1("tag_band: invalid tag type: ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" & ");
  err_puts(tag_type_to_string(b->type));
  assert(! "tag_band: invalid tag type");
  return NULL;
 integer_clean_tmp:
  integer_clean(&tmp);
 integer_reduce:
  if (! tag_integer_reduce(&tag, dest)) {
    tag_clean(&tag);
    return NULL;
  }
  tag_clean(&tag);
  return dest;
}
