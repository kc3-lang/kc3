/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include "compare.h"
#include "tag.h"

#define COMPARE_DEF(type)                       \
  s8 compare_##type (type a, type b)            \
  {                                             \
    if (a < b)                                  \
      return -1;                                \
    if (a > b)                                  \
      return 1;                                 \
    return 0;                                   \
  }                                             \

s8 compare_call (const s_call *a, const s_call *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_ident(&a->ident, &b->ident)))
    return r;
  return compare_list(a->arguments, b->arguments);
}


COMPARE_DEF(f32)

COMPARE_DEF(f64)

s8 compare_fact (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag(a->subject, b->subject)))
    return r;
  if ((r = compare_tag(a->predicate, b->predicate)))
    return r;
  r = compare_tag(a->object, b->object);
  return r;
}

s8 fact_compare_unbound_var_count (const s_fact *a,
                                   const s_fact *b)
{
  u8 a_count;
  u8 b_count;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  a_count = 0;
  b_count = 0;
  if (tag_is_unbound_var(a->subject))
    a_count++;
  if (tag_is_unbound_var(a->predicate))
    a_count++;
  if (tag_is_unbound_var(a->object))
    a_count++;
  if (tag_is_unbound_var(b->subject))
    b_count++;
  if (tag_is_unbound_var(b->predicate))
    b_count++;
  if (tag_is_unbound_var(b->object))
    b_count++;
  return compare_u8(a_count, b_count);
}

s8 compare_fact_pos (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag(a->predicate, b->predicate)))
    return r;
  if ((r = compare_tag(a->object, b->object)))
    return r;
  r = compare_tag(a->subject, b->subject);
  return r;
}

s8 compare_fact_osp (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag(a->object, b->object)))
    return r;
  if ((r = compare_tag(a->subject, b->subject)))
    return r;
  r = compare_tag(a->predicate, b->predicate);
  return r;
}

s8 compare_ptr (const void *a, const void *b)
{
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  return 1;
}

COMPARE_DEF(s8)

COMPARE_DEF(s16)

COMPARE_DEF(s32)

COMPARE_DEF(s64)

s8 compare_tag (const s_tag *a, const s_tag *b) {
  if (tag_is_bound_var(a))
    a = a->data.var;
  if (tag_is_bound_var(b))
    b = b->data.var;
  if (a == b)
    return 0;
  if (!a ||
      a == TAG_FIRST ||
      b == TAG_LAST)
    return -1;
  if (!b ||
      a == TAG_LAST ||
      b == TAG_FIRST)
    return 1;
  if (a->type.type < b->type.type)
    return -1;
  if (a->type.type > b->type.type)
    return 1;
  switch (a->type.type) {
  case TAG_VOID: return 0;
  case TAG_BOOL: return compare_bool(a->data.bool, b->data.bool);
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    return compare_call(&a->data.call, &b->data.call);
  case TAG_CHARACTER: return compare_character(a->data.character,
                                               b->data.character);
  case TAG_F32: return compare_f32(a->data.f32, b->data.f32);
  case TAG_F64: return compare_f64(a->data.f64, b->data.f64);
  case TAG_FN: return compare_ptr(a, b);
  case TAG_IDENT: return compare_ident(&a->data.ident, &b->data.ident);
  case TAG_INTEGER: return compare_integer(&a->data.integer,
                                           &b->data.integer);
  case TAG_LIST: return compare_list(a->data.list, b->data.list);
  case TAG_PTAG: return compare_ptag(a->data.ptag, b->data.ptag);
  case TAG_QUOTE: return compare_quote(a->data.quote, b->data.quote);
  case TAG_S8: return compare_s8(a->data.s8, b->data.s8);
  case TAG_S16: return compare_s16(a->data.s16, b->data.s16);
  case TAG_S32: return compare_s32(a->data.s32, b->data.s32);
  case TAG_S64: return compare_s64(a->data.s64, b->data.s64);
  case TAG_STR: return compare_str(&a->data.str, &b->data.str);
  case TAG_SYM: return compare_str(&a->data.sym->str,
                                   &b->data.sym->str);
  case TAG_TUPLE: return compare_tuple(&a->data.tuple, &b->data.tuple);
  case TAG_U8: return  compare_u8(a->data.u8, b->data.u8);
  case TAG_U16: return compare_u16(a->data.u16, b->data.u16);
  case TAG_U32: return compare_u32(a->data.u32, b->data.u32);
  case TAG_U64: return compare_u64(a->data.u64, b->data.u64);
  case TAG_VAR: return compare_ptr(a->data.var, b->data.var);
  }
  assert(! "compare_tag: error");
  errx(1, "compare_tag");
  return 0;
}

COMPARE_DEF(u8)

COMPARE_DEF(u16)

COMPARE_DEF(u32)

COMPARE_DEF(u64)
