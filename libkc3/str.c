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
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_inspect_s8_decimal.h"
#include "buf_inspect_s16_decimal.h"
#include "buf_inspect_s32_decimal.h"
#include "buf_inspect_s64_decimal.h"
#include "buf_inspect_sw_decimal.h"
#include "buf_inspect_u8_decimal.h"
#include "buf_inspect_u16_decimal.h"
#include "buf_inspect_u32_decimal.h"
#include "buf_inspect_u64_decimal.h"
#include "buf_inspect_uw_decimal.h"
#include "buf_parse.h"
#include "call.h"
#include "character.h"
#include "ident.h"
#include "io.h"
#include "kc3_main.h"
#include "list.h"
#include "str.h"
#include "sw.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"
#include "uw.h"

#define DEF_STR_INIT(name, type)                                       \
  s_str * str_init_ ## name (s_str *str, type x)                       \
  {                                                                    \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    sw r;                                                              \
    sw size;                                                           \
    size = buf_inspect_ ## name ## _size(&pretty, &x);                 \
    if (! size)                                                        \
      return str_init_empty(str);                                      \
    if (size < 0) {                                                    \
      err_puts("str_init_" # name ": buf_inspect_" # name              \
               "_size < 0");                                           \
      return NULL;                                                     \
    }                                                                  \
    if (! buf_init_alloc(&buf, size)) {                                \
      err_puts("str_init_" # name ": buf_init_alloc");                 \
      return NULL;                                                     \
    }                                                                  \
    if ((r = buf_inspect_ ## name(&buf, &x)) < 0) {                    \
      err_puts("str_init_" # name ": buf_inspect_" # name " < 0");     \
      buf_clean(&buf);                                                 \
      return NULL;                                                     \
    }                                                                  \
    if (r != size) {                                                   \
      err_write_1("str_init_" # name ": buf_inspect_" # name ": ");    \
      err_inspect_sw_decimal(&r);                                      \
      err_write_1(" != ");                                             \
      err_inspect_sw_decimal(&size);                                   \
      err_write_1("\n");                                               \
      buf_clean(&buf);                                                 \
      return NULL;                                                     \
    }                                                                  \
    assert(buf.wpos == (uw) size);                                     \
    return buf_to_str(&buf, str);                                      \
  }

#define DEF_STR_INIT_INT(type)                                         \
  s_str * str_init_ ## type (s_str *str, type x)                       \
  {                                                                    \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    sw size;                                                           \
    size = buf_inspect_ ## type ## _decimal_size(&pretty, &x);         \
    if (! size)                                                        \
      return str_init_empty(str);                                      \
    if (size < 0) {                                                    \
      err_puts("str_init_" # type ": buf_inspect_" # type              \
               "_size < 0");                                           \
      return NULL;                                                     \
    }                                                                  \
    if (! buf_init_alloc(&buf, size)) {                                \
      err_puts("str_init_" # type ": buf_init_alloc");                 \
      return NULL;                                                     \
    }                                                                  \
    buf_inspect_ ## type ## _decimal(&buf, &x);                        \
    assert(buf.wpos == (uw) size);                                     \
    return buf_to_str(&buf, str);                                      \
  }

#define DEF_STR_INIT_PTR(name, type)                                       \
  s_str * str_init_ ## name (s_str *str, type x)                       \
  {                                                                    \
    s_buf buf;                                                         \
    s_pretty pretty = {0};                                             \
    sw r;                                                              \
    sw size;                                                           \
    size = buf_inspect_ ## name ## _size(&pretty, x);                 \
    if (! size)                                                        \
      return str_init_empty(str);                                      \
    if (size < 0) {                                                    \
      err_puts("str_init_" # name ": buf_inspect_" # name              \
               "_size < 0");                                           \
      return NULL;                                                     \
    }                                                                  \
    if (! buf_init_alloc(&buf, size)) {                                \
      err_puts("str_init_" # name ": buf_init_alloc");                 \
      return NULL;                                                     \
    }                                                                  \
    if ((r = buf_inspect_ ## name(&buf, x)) < 0) {                    \
      err_puts("str_init_" # name ": buf_inspect_" # name " < 0");     \
      buf_clean(&buf);                                                 \
      return NULL;                                                     \
    }                                                                  \
    if (r != size) {                                                   \
      err_write_1("str_init_" # name ": buf_inspect_" # name ": ");    \
      err_inspect_sw_decimal(&r);                                      \
      err_write_1(" != ");                                             \
      err_inspect_sw_decimal(&size);                                   \
      err_write_1("\n");                                               \
      buf_clean(&buf);                                                 \
      return NULL;                                                     \
    }                                                                  \
    assert(buf.wpos == (uw) size);                                     \
    return buf_to_str(&buf, str);                                      \
  }

#define DEF_STR_INIT_STRUCT(name)                                      \
  s_str * str_init_ ## name (s_str *str, const s_ ## name *x)          \
  {                                                                    \
    s_buf buf = {0};                                                   \
    s_pretty pretty = {0};                                             \
    sw r;                                                              \
    sw size;                                                           \
    size = buf_inspect_ ## name ## _size(&pretty, x);                  \
    if (! size)                                                        \
      return str_init_empty(str);                                      \
    if (size < 0) {                                                    \
      err_puts("str_init_" # name ": buf_inspect_" # name              \
               "_size < 0");                                           \
      return NULL;                                                     \
    }                                                                  \
    if (! buf_init_alloc(&buf, size)) {                                \
      err_puts("str_init_" # name ": buf_init_alloc");                 \
      return NULL;                                                     \
    }                                                                  \
    if ((r = buf_inspect_ ## name(&buf, x)) < 0) {                     \
      err_puts("str_init_" # name ": buf_inspect_" # name " < 0");     \
      err_inspect_ ## name(x);                                         \
      buf_clean(&buf);                                                 \
      return NULL;                                                     \
    }                                                                  \
    if (r != size) {                                                   \
      err_write_1("str_init_" # name ": buf_inspect_" # name ": ");    \
      err_inspect_sw_decimal(&r);                                      \
      err_write_1(" != ");                                             \
      err_inspect_sw_decimal(&size);                                   \
      err_write_1("\n");                                               \
      buf_clean(&buf);                                                 \
      return NULL;                                                     \
    }                                                                  \
    assert(buf.wpos == (uw) size);                                     \
    return buf_to_str(&buf, str);                                      \
  }

s_tag * str_access (const s_str *str, s_list *key, s_tag *dest)
{
  uw pos;
  const s_sym *sym_Uw = &g_sym_Uw;
  character tmp;
  assert(str);
  if (! key ||
      list_next(key) ||
      ! uw_init_cast(&pos, &sym_Uw, &key->tag) ||
      str_character(str, pos, &tmp) <= 0) {
    err_puts("str_access: invalid key");
    err_stacktrace();
    return tag_init_void(dest);
  }
  dest->type = TAG_CHARACTER;
  dest->data.character = tmp;
  return dest;
}

sw str_character (const s_str *str, uw position, character *dest)
{
  character c;
  uw i = 0;
  sw r = 0;
  s_str s;
  s = *str;
  while (s.size > 0 && i <= position) {
    if ((r = str_read_character_utf8(&s, &c)) <= 0)
      return -1;
    i++;
  }
  *dest = c;
  return r;
}

bool str_character_is_reserved (character c)
{
  return ! character_is_printable(c) ||
    c == '"' ||
    c == '\\';
}

sw str_character_position (const s_str *str, character c)
{
  uw i = 0;
  sw r = 0;
  s_str tmp = {0};
  character tmp_c = 0;
  assert(str);
  tmp = *str;
  while (tmp.size && (r = str_read_character_utf8(&tmp, &tmp_c)) > 0) {
    if (c == tmp_c)
      return i;
    i++;
  }
  if (r < 0) {
    err_puts("str_character_position: invalid str character utf8");
    assert(! "str_character_position: invalid str character utf8");
  }
  return -1;
}

sw str_character_position_last (const s_str *str, character c)
{
  sw i = 0;
  sw j = -1;
  sw r = 0;
  s_str tmp = {0};
  character tmp_c = 0;
  assert(str);
  tmp = *str;
  while (tmp.size &&
         (r = str_read_character_utf8(&tmp, &tmp_c)) > 0) {
    if (c == tmp_c)
      j = i;
    i++;
  }
  if (r < 0) {
    err_puts("str_character_position: invalid str character utf8");
    assert(! "str_character_position: invalid str character utf8");
  }
  return j;
}

void str_clean (s_str *str)
{
  assert(str);
  if (str->free.p)
    free(str->free.p);
}

void str_delete (s_str *str)
{
  str_clean(str);
  free(str);
}

bool * str_ends_with (const s_str *str, const s_str *end, bool *dest)
{
  uw i;
  assert(str);
  assert(end);
  if (! end->size) {
    *dest = true;
    return dest;
  }
  if (str->size < end->size) {
    *dest = false;
    return dest;
  }
  i = str->size - end->size;
  *dest = ! memcmp(str->ptr.pchar + i, end->ptr.p, end->size);
  return dest;
}

bool * str_ends_with_1 (const s_str *str, const char *end, bool *dest)
{
  s_str end_str = {{NULL}, strlen(end), {end}};
  return str_ends_with(str, &end_str, dest);
}

bool * str_has_characters (const s_str *src, const s_str *chars,
                           bool *dest)
{
  character c;
  sw r;
  s_str str;
  str_init(&str, NULL, src->size, src->ptr.p);
  while (str.size > 0) {
    if ((r = str_read_character_utf8(&str, &c)) < 0)
      return NULL;
    if (! r)
      break;
    if (str_character_position(chars, c) >= 0) {
      *dest = true;
      return dest;
    }
  }
  *dest = false;
  return dest;
}

bool * str_has_reserved_characters (const s_str *src, bool *dest)
{
  character c;
  sw r;
  s_str str;
  str_init(&str, NULL, src->size, src->ptr.p);
  while (str.size > 0) {
    if ((r = str_read_character_utf8(&str, &c)) < 0) {
      *dest = true;
      return dest;
    }
    if (! r)
      break;
    if (str_character_is_reserved(c)) {
      *dest = true;
      return dest;
    }
  }
  *dest = false;
  return dest;
}

bool * str_has_str (const s_str *src, const s_str *search, bool *dest)
{
  uw offset;
  offset = 0;
  while (1) {
    if (src->size - offset < search->size) {
      *dest = false;
      return dest;
    }
    if (! memcmp(src->ptr.pchar + offset, search->ptr.pchar,
                 search->size)) {
      *dest = true;
      return dest;
    }
    offset++;
  }
  return NULL;
}

s_str * str_init (s_str *str, char *free, uw size, const char *p)
{
  s_str tmp = {0};
  assert(str);
  tmp.free.p = free;
  tmp.size = size;
  tmp.ptr.p = p;
  *str = tmp;
  return str;
}

s_str * str_init_1 (s_str *str, char *free, const char *p)
{
  s_str tmp = {0};
  assert(str);
  tmp.free.p = free;
  tmp.size = strlen(p);
  tmp.ptr.p = p;
  *str = tmp;
  return str;
}

s_str * str_init_1_alloc (s_str *str, const char *p)
{
  uw size;
  s_str tmp = {0};
  assert(str);
  size = strlen(p);
  tmp.free.p = alloc(size + 1);
  if (! tmp.free.p)
    return NULL;
  tmp.size = size;
  tmp.ptr.p = tmp.free.p;
  memcpy(tmp.free.p, p, size);
  *str = tmp;
  return str;
}

s_str * str_init_alloc (s_str *str, uw size)
{
  s_str tmp = {0};
  assert(str);
  tmp.free.p = alloc(size + 1);
  if (! tmp.free.p)
    return NULL;
  tmp.size = size;
  tmp.ptr.p = tmp.free.p;
  *str = tmp;
  return str;
}

s_str * str_init_alloc_copy (s_str *str, uw size, const char *p)
{
  s_str tmp = {0};
  assert(str);
  tmp.free.p = alloc(size + 1);
  if (! tmp.free.p)
    return NULL;
  tmp.size = size;
  tmp.ptr.p = tmp.free.p;
  memcpy(tmp.free.p, p, size);
  *str = tmp;
  return str;
}

DEF_STR_INIT_STRUCT(array)
DEF_STR_INIT(bool, bool)
DEF_STR_INIT_STRUCT(callable)

s_str * str_init_cast (s_str *str, const s_sym * const *type,
                       const s_tag *tag)
{
  assert(str);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:
    return str_init_array(str, &tag->data.array);
  case TAG_BOOL:
    return str_init_bool(str, tag->data.bool_);
  case TAG_CHARACTER:
    return str_init_character(str, tag->data.character);
  case TAG_F32:
    return str_init_f32(str, tag->data.f32);
  case TAG_F64:
    return str_init_f64(str, tag->data.f64);
  case TAG_F128:
    return str_init_f128(str, tag->data.f128);
  case TAG_IDENT:
    return str_init_ident(str, &tag->data.ident);
  case TAG_INTEGER:
    return str_init_integer(str, &tag->data.integer);
  case TAG_LIST:
    return str_init_list(str, (const s_list * const *) &tag->data.list);
  case TAG_MAP:
    return str_init_map(str, &tag->data.map);
  case TAG_PCALLABLE:
    return str_init_callable(str, tag->data.pcallable);
  case TAG_PSTRUCT:
    return str_init_struct(str, tag->data.pstruct);
  case TAG_PTR:
    return str_init_ptr(str, &tag->data.ptr);
  case TAG_PTR_FREE:
    return str_init_ptr_free(str, &tag->data.ptr_free);
  case TAG_S8:
    return str_init_s8(str, tag->data.s8);
  case TAG_S16:
    return str_init_s16(str, tag->data.s16);
  case TAG_S32:
    return str_init_s32(str, tag->data.s32);
  case TAG_S64:
    return str_init_s64(str, tag->data.s64);
  case TAG_STR:
    return str_init_copy(str, &tag->data.str);
  case TAG_SYM:
    return str_init_copy(str, &tag->data.sym->str);
  case TAG_SW:
    return str_init_sw(str, tag->data.sw);
  case TAG_TUPLE:
    return str_init_tuple(str, &tag->data.tuple);
  case TAG_U8:
    return str_init_u8(str, tag->data.u8);
  case TAG_U16:
    return str_init_u16(str, tag->data.u16);
  case TAG_U32:
    return str_init_u32(str, tag->data.u32);
  case TAG_U64:
    return str_init_u64(str, tag->data.u64);
  case TAG_UW:
    return str_init_uw(str, tag->data.uw);
  case TAG_VAR:
    return str_init_var(str, &tag->data.var);
  case TAG_VOID:
    return str_init_empty(str);
  default:
    break;
  }
  err_write_1("str_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Str)
    err_puts(" to Str");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka Str");
  }
  assert(! "str_init_cast: cannot cast to Str");
  return NULL;
}

DEF_STR_INIT_STRUCT(cfn)

s_str * str_init_character (s_str *str, const character src)
{
  char b[4];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  if (buf_write_character_utf8(&buf, src) < 0)
    return NULL;
  if (! buf_read_to_str(&buf, str))
    return NULL;
  return str;
}

s_str * str_init_concatenate (s_str *str, const s_str *a,
                              const s_str *b)
{
  s_str tmp = {0};
  assert(str);
  assert(a);
  assert(b);
  tmp.size = a->size + b->size;
  tmp.free.p = alloc(tmp.size + 1);
  if (! tmp.free.p)
    return NULL;
  tmp.ptr.p = tmp.free.p;
  if (a->size)
    memcpy(tmp.free.ps8, a->ptr.p, a->size);
  if (b->size)
    memcpy(tmp.free.ps8 + a->size, b->ptr.p, b->size);
  *str = tmp;
  return str;
}

s_str * str_init_concatenate_list (s_str *str,
                                   const s_list * const *list)
{
  const s_list *l;
  char *p;
  s_str tmp = {0};
  l = *list;
  while (l) {
    if (l->tag.type != TAG_STR) {
      err_write_1("str_init_concatenate_list: not a Str: ");
      err_inspect_tag(&l->tag);
      err_write_1("\n");
      return NULL;
    }
    tmp.size += l->tag.data.str.size;
    l = list_next(l);
  }
  if (! str_init_alloc(&tmp, tmp.size))
    return NULL;
  p = tmp.free.pchar;
  l = *list;
  while (l) {
    if (p + l->tag.data.str.size > tmp.free.pchar + tmp.size) {
      err_puts("str_init_concatenate_list: buffer overflow");
      assert(! "str_init_concatenate_list: buffer overflow");
      str_clean(&tmp);
      return NULL;
    }
    memcpy(p, l->tag.data.str.ptr.p, l->tag.data.str.size);
    p += l->tag.data.str.size;
    l = list_next(l);
  }
  *str = tmp;
  return str;
}

s_str * str_init_copy (s_str *str, const s_str *src)
{
  s_str tmp = {0};
  assert(str);
  assert(src);
  tmp.free.p = alloc(src->size + 1);
  if (! tmp.free.p)
    return NULL;
  tmp.size = src->size;
  tmp.ptr.p = tmp.free.p;
  memcpy(tmp.free.p, src->ptr.p, tmp.size);
  *str = tmp;
  return str;
}

s_str * str_init_copy_1 (s_str *str, const char *src)
{
  uw len;
  s_str tmp = {0};
  assert(str);
  assert(src);
  len = strlen(src);
  tmp.free.p = alloc(len + 1);
  if (! tmp.free.p)
    return NULL;
  tmp.size = len;
  tmp.ptr.p = tmp.free.p;
  memcpy(tmp.free.p, src, len + 1);
  *str = tmp;
  return str;
}

s_str * str_init_empty (s_str *str)
{
  s_str tmp = {0};
  assert(str);
  *str = tmp;
  return str;
}

s_str * str_init_f (s_str *str, const char *fmt, ...)
{
  va_list ap;
  s_str tmp = {0};
  va_start(ap, fmt);
  if (! str_init_vf(&tmp, fmt, ap)) {
    va_end(ap);
    return NULL;
  }
  va_end(ap);
  *str = tmp;
  return str;
}

s_str * str_init_f32 (s_str *str, f32 x)
{
  char a[32];
  s_buf buf;
  s64 exp;
  u8 i;
  u8 j;
  sw r;
  buf_init(&buf, false, sizeof(a), a);
  exp = 0.0;
  if (x == 0.0) {
    if ((r = buf_write_1(&buf, "0.0")) < 0)
      return NULL;
    goto ok;
  }
  if (x < 0) {
    if ((r = buf_write_1(&buf, "-")) <= 0)
      return NULL;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(&buf, i)) <= 0)
    return NULL;
  if ((r = buf_write_1(&buf, ".")) <= 0)
    return NULL;
  j = 6;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(&buf, i)) <= 0)
      return NULL;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(&buf, "e")) <= 0)
      return NULL;
    if (exp > 0) {
      if ((r = buf_write_1(&buf, "+")) <= 0)
        return NULL;
    }
    if ((r = buf_inspect_s64_decimal(&buf, &exp)) <= 0)
      return NULL;
  }
 ok:
  return buf_read_to_str(&buf, str);
}

s_str * str_init_f64 (s_str *str, f64 x)
{
  char a[32];
  s_buf buf;
  s64 exp;
  u8 i;
  u8 j;
  sw r;
  buf_init(&buf, false, sizeof(a), a);
  exp = 0.0;
  if (x == 0.0) {
    if ((r = buf_write_1(&buf, "0.0")) < 0)
      return NULL;
    goto ok;
  }
  if (x < 0) {
    if ((r = buf_write_1(&buf, "-")) <= 0)
      return NULL;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(&buf, i)) <= 0)
    return NULL;
  if ((r = buf_write_1(&buf, ".")) <= 0)
    return NULL;
  j = 14;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(&buf, i)) <= 0)
      return NULL;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(&buf, "e")) <= 0)
      return NULL;
    if (exp > 0) {
      if ((r = buf_write_1(&buf, "+")) <= 0)
        return NULL;
    }
    if ((r = buf_inspect_s64_decimal(&buf, &exp)) <= 0)
      return NULL;
  }
 ok:
  return buf_read_to_str(&buf, str);
}

s_str * str_init_f128 (s_str *str, f128 x)
{
  char a[128];
  s_buf buf;
  s64 exp;
  u8 i;
  u8 j;
  sw r;
  buf_init(&buf, false, sizeof(a), a);
  exp = 0.0;
  if (x == 0.0) {
    if ((r = buf_write_1(&buf, "0.0")) < 0)
      return NULL;
    goto ok;
  }
  if (x < 0) {
    if ((r = buf_write_1(&buf, "-")) <= 0)
      return NULL;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(&buf, i)) <= 0)
    return NULL;
  if ((r = buf_write_1(&buf, ".")) <= 0)
    return NULL;
  j = 33;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(&buf, i)) <= 0)
      return NULL;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(&buf, "e")) <= 0)
      return NULL;
    if (exp > 0) {
      if ((r = buf_write_1(&buf, "+")) <= 0)
        return NULL;
    }
    if ((r = buf_inspect_s64_decimal(&buf, &exp)) <= 0)
      return NULL;
  }
 ok:
  return buf_read_to_str(&buf, str);
}

DEF_STR_INIT_STRUCT(fn)

s_str * str_init_ftime (s_str *str, s_time *time, const s_str *format)
{
  char *buf;
  sw r;
  uw size;
  time_t t;
  s_str tmp;
  const struct tm *utc = NULL;
  t = time->tv_sec;
  if (! (utc = gmtime(&t))) {
    err_puts("str_init_ftime: gmtime");
    assert(! "str_init_ftime: gmtime");
    return NULL;
  }
  size = format->size * 32;
  if (! (buf = alloc(size)))
    return NULL;
  r = strftime(buf, size, format->ptr.pchar, utc);
  if ((! r) || (r == (sw) size)) {
    err_puts("str_init_ftime: strftime");
    assert(! "str_init_ftime: strftime");
    goto clean;
  }
  if (! str_init_1_alloc(&tmp, buf))
    goto clean;
  free(buf);
  *str = tmp;
  return str;
 clean:
  free(buf);
  return NULL;
}

DEF_STR_INIT_STRUCT(ident)
DEF_STR_INIT_STRUCT(integer)
DEF_STR_INIT_PTR(list, const s_list * const *)
DEF_STR_INIT_STRUCT(map)
DEF_STR_INIT_PTR(ptr, const u_ptr_w *)
DEF_STR_INIT_PTR(ptr_free, const u_ptr_w *)

s_str * str_init_random_base64 (s_str *str, const s_tag *len)
{
  const s_sym *type;
  char *random_bytes;
  sw    random_bytes_len;
  char *result;
  sw    result_len;
  uw i;
  uw j;
  uw k;
  uw u;
  type = &g_sym_Uw;
  if (! sw_init_cast(&result_len, &type, len)) {
    err_write_1("str_init_random_base64: cannot cast to Uw: ");
    err_inspect_tag(len);
    err_write_1("\n");
    return NULL;
  }
  random_bytes_len = (result_len + 1) * 3 / 4;
  if (false) {
    err_write_1("str_init_random_base64: random_bytes_len: ");
    err_inspect_sw_decimal(&random_bytes_len);
    err_write_1("\n");
  }
  if (! (random_bytes = alloc(random_bytes_len)))
    return NULL;
  if (! (result = alloc(result_len + 1))) {
    free(random_bytes);
    return NULL;
  }
  arc4random_buf(random_bytes, random_bytes_len);
  i = 0;
  j = 0;
  while (i + 3 <= (uw) random_bytes_len &&
         j < (uw) result_len) {
    u = ((random_bytes[i] << 16) +
         (random_bytes[i + 1] << 8) +
         (random_bytes[i + 2]));
    k = 0;
    while (k < 4 &&
           j < (uw) result_len) {
      result[j] = g_kc3_base64url.ptr.pchar[u % 64];
      u /= 64;
      j++;
      k++;
    }
    i += 3;
  }
  if (random_bytes_len - i == 2) {
    u = ((random_bytes[i] << 8) +
         random_bytes[i + 1]);
    k = 0;
    while (j < (uw) result_len) {
      result[j] = g_kc3_base64url.ptr.pchar[u % 64];
      u /= 64;
      j++;
      k++;
    }
  }
  else if (random_bytes_len - i == 1) {
    u = random_bytes[i];
    k = 0;
    while (j < (uw) result_len) {
      result[j] = g_kc3_base64url.ptr.pchar[u % 64];
      u /= 64;
      j++;
      k++;
    }
  }
  free(random_bytes);
  return str_init(str, result, result_len, result);
}

DEF_STR_INIT_STRUCT(ratio)
DEF_STR_INIT_INT(s8)
DEF_STR_INIT_INT(s16)
DEF_STR_INIT_INT(s32)
DEF_STR_INIT_INT(s64)
DEF_STR_INIT_STRUCT(struct)
DEF_STR_INIT_INT(sw)

s_str * str_init_slice (s_str *str, const s_str *src, sw start, sw end)
{
  s_buf buf;
  s_str tmp = {0};
  assert(str);
  assert(src);
  buf_init_const(&buf, src->size, src->ptr.pchar);
  if (! str_sw_pos_to_uw(start, src->size, &buf.rpos)) {
    err_puts("str_init_slice: str_sw_pos_to_uw: start");
    assert(! "str_init_slice: str_sw_pos_to_uw: start");
    return NULL;
  }
  if (! str_sw_pos_to_uw(end, src->size, &buf.wpos)) {
    err_puts("str_init_slice: str_sw_pos_to_uw: end");
    assert(! "str_init_slice: str_sw_pos_to_uw: end");
    return NULL;
  }
  if (buf.rpos > buf.wpos) {
    err_write_1("str_init_slice: invalid positions: ");
    err_inspect_uw(&buf.rpos);
    err_write_1(" > ");
    err_inspect_uw(&buf.wpos);
    err_write_1("\n");
    assert(! "str_init_slice: invalid positions");
    return NULL;
  }
  if (! buf_read_to_str(&buf, &tmp)) {
    err_puts("str_init_slice: buf_read_to_str");
    assert(! "str_init_slice: buf_read_to_str");
    return NULL;
  }
  *str = tmp;
  return str;
}

s_str * str_init_subst (s_str *str, const s_str *src,
                        const s_str *search, const s_str *replace)
{
  character c;
  s_buf in;
  s_buf out;
  sw r;
  sw size;
  assert(str);
  assert(src);
  assert(search);
  assert(replace);
  if ((size = str_init_subst_size(src, search, replace)) < 0)
    return NULL;
  if (! buf_init_alloc(&out, size))
    return NULL;
  buf_init_str_const(&in, src);
  while (in.wpos - in.rpos >= search->size) {
    if (buf_read_str(&in, search) > 0) {
      if ((r = buf_write_str(&out, replace)) < 0)
        goto clean;
    }
    else {
      if ((r = buf_read_character_utf8(&in, &c)) < 0)
        break;
      if ((r = buf_write_character_utf8(&out, c)) < 0)
        goto clean;
    }
  }
  if ((r = buf_xfer(&out, &in, in.wpos - in.rpos)) < 0)
    goto clean;
  buf_to_str(&out, str);
  return str;
 clean:
  buf_clean(&out);
  return NULL;
}

sw str_init_subst_size (const s_str *src, const s_str *search,
                        const s_str *replace)
{
  character c;
  s_buf in;
  sw r;
  sw result = 0;
  assert(src);
  assert(search);
  assert(replace);
  buf_init_str_const(&in, src);
  while (in.wpos - in.rpos >= search->size) {
    if (buf_read_str(&in, search) > 0)
      result += replace->size;
    else {
      if ((r = buf_read_character_utf8(&in, &c)) < 0)
        return -1;
      result += r;
    }
  }
  return result;
}

s_str * str_init_to_lower (s_str *str, const s_str *src)
{
  s_buf buf;
  character c;
  s_str i_src;
  assert(str);
  assert(src);
  if (! buf_init_alloc(&buf, src->size))
    return NULL;
  i_src = *src;
  while (str_read_character_utf8(&i_src, &c) > 0) {
    c = character_to_lower(c);
    buf_write_character_utf8(&buf, c);
  }
  return buf_to_str(&buf, str);
}

s_str * str_init_to_upper (s_str *str, const s_str *src)
{
  s_buf buf;
  character c;
  s_str i_src;
  assert(str);
  assert(src);
  if (! buf_init_alloc(&buf, src->size))
    return NULL;
  i_src = *src;
  while (str_read_character_utf8(&i_src, &c) > 0) {
    c = character_to_upper(c);
    buf_write_character_utf8(&buf, c);
  }
  return buf_to_str(&buf, str);
}

DEF_STR_INIT_STRUCT(tuple)
DEF_STR_INIT_INT(u8)
DEF_STR_INIT_INT(u16)
DEF_STR_INIT_INT(u32)
DEF_STR_INIT_INT(u64)
DEF_STR_INIT_INT(uw)
DEF_STR_INIT_STRUCT(var)

s_str * str_init_vf (s_str *str, const char *fmt, va_list ap)
{
  int len;
  char *s;
  len = vasprintf(&s, fmt, ap);
  if (len < 0) {
    err_puts("str_init_vf: vasprintf");
    assert(! "str_init_vf: vasprintf");
    return NULL;
  }
  return str_init(str, s, len, s);
}

sw str_length_utf8 (const s_str *str)
{
  character c;
  sw r;
  sw result = 0;
  s_str tmp;
  tmp = *str;
  while ((r = str_read_character_utf8(&tmp, &c)) > 0)
    result++;
  if (r < 0)
    return r;
  return result;
}

s_str * str_new (char *free, uw size, const char *p)
{
  s_str *str;
  str = alloc(sizeof(s_str));
  if (! str)
    return NULL;
  str_init(str, free, size, p);
  return str;
}

s_str * str_new_1 (char *free, const char *s)
{
  uw len = strlen(s);
  s_str *str = str_new(free, len, s);
  return str;
}

s_str * str_new_cpy (const char *p, uw size)
{
  char *a;
  s_str *str;
  a = alloc(size);
  if (! a)
    return NULL;
  memcpy(a, p, size);
  str = str_new(a, size, a);
  if (! str) {
    free(a);
    return NULL;
  }
  return str;
}

s_str * str_new_copy (const s_str *src)
{
  char *a;
  s_str *dest;
  assert(src);
  a = alloc(src->size);
  if (! a)
    return NULL;
  memcpy(a, src->ptr.p, src->size);
  dest = str_new(a, src->size, a);
  if (! dest) {
    free(a);
    return NULL;
  }
  return dest;
}

s_str * str_new_empty (void)
{
  s_str *dest;
  dest = str_new(NULL, 0, "");
  return dest;
}

s_str * str_new_f (const char *fmt, ...)
{
  va_list ap;
  s_str *dest;
  va_start(ap, fmt);
  dest = str_new_vf(fmt, ap);
  va_end(ap);
  return dest;
}

s_str * str_new_vf (const char *fmt, va_list ap)
{
  int len;
  char *s;
  s_str *dest;
  len = vasprintf(&s, fmt, ap);
  if (len < 0) {
    err_puts("str_new_vf: vasprintf");
    assert(! "str_new_vf: vasprintf");
    return NULL;
  }
  dest = str_new(s, len, s);
  return dest;
}

bool str_parse_eval (const s_str *str, s_tag *dest)
{
  s_tag *arg;
  character c;
  s_buf in_buf;
  s_list  *l;
  s_list **tail;
  s_list  *list = NULL;
  s_buf out_buf = {0};
  sw r;
  s_tag tag;
  s_tag tmp = {0};
  buf_init_str_const(&in_buf, str);
  if (! buf_init_alloc(&out_buf, str->size))
    goto restore;
  tail = &list;
  while (1) {
    r = buf_read_1(&in_buf, "#{");
    if (r < 0)
      break;
    if (r > 0) {
      if (out_buf.wpos > 0) {
        *tail = list_new(NULL);
        (*tail)->tag.type = TAG_STR;
        buf_read_to_str(&out_buf, &(*tail)->tag.data.str);
        tail = &(*tail)->next.data.list;
        out_buf.rpos = out_buf.wpos = 0;
      }
      *tail = list_new(NULL);
      r = buf_parse_tag(&in_buf, &(*tail)->tag);
      if (r <= 0)
        goto restore;
      tail = &(*tail)->next.data.list;
      r = buf_ignore_spaces(&in_buf);
      if (r < 0)
        goto restore;
      r = buf_parse_comments(&in_buf);
      if (r < 0)
        goto restore;
      r = buf_read_1(&in_buf, "}");
      if (r <= 0)
        goto restore;
      continue;
    }
    r = buf_peek_character_utf8(&in_buf, &c);
    if (r <= 0)
      break;
    r = buf_xfer(&out_buf, &in_buf, r);
    if (r <= 0)
      goto restore;
  }
  if (out_buf.wpos > 0) {
    *tail = list_new(NULL);
    (*tail)->tag.type = TAG_STR;
    buf_read_to_str(&out_buf, &(*tail)->tag.data.str);
  }
  if (! list)
    tag_init_str_empty(&tmp);
  else if (list->tag.type == TAG_STR &&
           ! list_next(list)) {
    tmp = list->tag;
    free(list);
  }
  else {
    tag_init_call(&tmp);
    ident_init(&tmp.data.call.ident, &g_sym_KC3, &g_sym_str);
    tmp.data.call.arguments = list_new(NULL);
    l = list;
    while (l) {
      if (l->tag.type == TAG_STR ||
          (l->tag.type == TAG_CALL &&
           l->tag.data.call.ident.module == &g_sym_Str &&
           l->tag.data.call.ident.sym == &g_sym_cast))
        goto next;
      tag_init_call_cast(&tag, &g_sym_Str);
      arg = &list_next(tag.data.call.arguments)->tag;
      *arg = l->tag;
      l->tag = tag;
    next:
      l = list_next(l);
    }
    arg = &tmp.data.call.arguments->tag;
    arg->type = TAG_LIST;
    arg->data.list = list;
  }
  *dest = tmp;
  buf_clean(&out_buf);
  return true;
 restore:
  list_delete_all(list);
  buf_clean(&out_buf);
  return false;
}

sw str_peek_character_utf8 (const s_str *str, character *c)
{
  //assert(str);
  //assert(c);
  const u8 *b;
  u8 x[4];
  const u8 _00000111 = 0x07;
  const u8 _00001111 = 0x0F;
  const u8 _00011111 = 0x1F;
  const u8 _00111111 = 0x3F;
  const u8 _10000000 = 0x80;
  const u8 _11000000 = 0xC0;
  const u8 _11100000 = 0xE0;
  const u8 _11110000 = 0xF0;
  const u8 _11111000 = 0xF8;
  if (str->size <= 0)
    return 0;
  b = str->ptr.pu8;
  if ((b[0] & _10000000) == 0) {
    *c = *b;
    return 1;
  }
  if ((b[0] & _11100000) == _11000000) {
    if (str->size < 2)
      return 0;
    if ((b[1] & _11000000) != _10000000)
      return -1;
    x[0] = b[0] & _00011111;
    x[1] = b[1] & _00111111;
    *c = (x[0] << 6) | x[1];
    return 2;
  }
  if ((b[0] & _11110000) == _11100000) {
    if (str->size < 3)
      return 0;
    if ((b[1] & _11000000) != _10000000)
      return -1;
    if ((b[2] & _11000000) != _10000000)
      return -1;
    x[0] = b[0] & _00001111;
    x[1] = b[1] & _00111111;
    x[2] = b[2] & _00111111;
    *c = (x[0] << 12) | (x[1] << 6) | x[2];
    return 3;
  }
  if ((b[0] & _11111000) == _11110000) {
    if (str->size < 4)
      return 0;
    if ((b[1] & _11000000) != _10000000)
      return -1;
    if ((b[2] & _11000000) != _10000000)
      return -1;
    if ((b[3] & _11000000) != _10000000)
      return -1;
    x[0] = b[0] & _00000111;
    x[1] = b[1] & _00111111;
    x[2] = b[2] & _00111111;
    x[3] = b[3] & _00111111;
    *c = (x[0] << 18) | (x[1] << 12) | (x[2] << 6) | x[3];
    return 4;
  }
  return -1;
}

sw str_position_1 (const s_str *str, const char *token)
{
  character c;
  uw len;
  sw r;
  sw result = 0;
  s_str s;
  assert(str);
  assert(token);
  len = strlen(token);
  s = *str;
  while (1) {
    if (s.size < len)
      return -1;
    if (! memcmp(s.ptr.pchar, token, len))
      return result;
    if ((r = str_read_character_utf8(&s, &c)) < 0)
      return -1;
    result += r;
  }
}

sw str_read_u8 (s_str *str, u8 *c)
{
  if (str->size <= 0)
    return 0;
  str->size--;
  *c = *str->ptr.pu8;
  str->ptr.pu8++;
  return 1;
}

sw str_read_character_utf8 (s_str *str, character *c)
{
  sw size;
  if (str->size == 0)
    return 0;
  size = str_peek_character_utf8(str, c);
  if (size < 0)
    return size;
  str->size -= size;
  str->ptr.p = str->ptr.pchar + size;
  return size;
}

sw str_rindex_character (const s_str *str, character c, sw start,
                         sw end)
{
  uw end_pos;
  uw pos = 0;
  sw r;
  sw result = -1;
  s_str s = {0};
  character tmp = 0;
  uw start_pos;
  assert(str);
  if (! str_sw_pos_to_uw(start, str->size, &start_pos)) {
    err_write_1("str_rindex_character: str_sw_pos_to_uw start: ");
    err_inspect_str(str);
    err_write_1("\n");
    return -1;
  }
  if (! str_sw_pos_to_uw(end, str->size, &end_pos)) {
    err_write_1("str_rindex_character: str_sw_pos_to_uw end: ");
    err_inspect_str(str);
    err_write_1("\n");
    return -1;
  }
  s = *str;
  while ((r = str_read_character_utf8(&s, &tmp)) > 0 &&
         pos < end_pos) {
    if (pos >= start_pos &&
        c == tmp)
      result = pos;
    pos += r;
  }
  return result;
}

s_list ** str_split_words (const s_str *str, s_list **dest)
{
  s_buf buf;
  s_list **t;
  s_str   *t_str;
  s_list  *tmp;
  buf_init_str_const(&buf, str);
  tmp = NULL;
  t = &tmp;
  while (1) {
    *t = list_new(NULL);
    (*t)->tag.type = TAG_STR;
    t_str = &(*t)->tag.data.str;
    if (! buf_read_word_into_str(&buf, t_str)) {
      if (! buf_read_to_str(&buf, t_str))
        goto clean;
      if (! t_str->size)
        *t = list_delete(*t);
      break;
    }
    if (t_str->size)
      t = &(*t)->next.data.list;
    else
      *t = list_delete(*t);
  }
  *dest = tmp;
  return dest;
 clean:
  list_delete_all(tmp);
  return NULL;  
}

s_tag * str_size (const s_str *str, s_tag *dest)
{
  s_tag tmp;
  tag_init_uw(&tmp, str->size);
  tag_integer_reduce(&tmp);
  *dest = tmp;
  return dest;
}

s_list ** str_split (const s_str *str, const s_str *separator,
                     s_list **dest)
{
  s_buf buf;
  const s_str *sep = separator;
  s_str   *t_str;
  s_list **t;
  s_list  *tmp;
  buf_init_str_const(&buf, str);
  tmp = NULL;
  t = &tmp;
  while (1) {
    *t = list_new(NULL);
    (*t)->tag.type = TAG_STR;
    t_str = &(*t)->tag.data.str;
    if (! buf_read_until_str_into_str(&buf, sep, t_str)) {
      if (! buf_read_to_str(&buf, t_str))
        goto clean;
      break;
    }
    t = &(*t)->next.data.list;
  }
  *dest = tmp;
  return dest;
 clean:
  list_delete_all(tmp);
  return NULL;
}

s_list ** str_split_list (const s_str *str,
                          const s_list * const *sep,
                          s_list **dest)
{
  s_buf buf;
  s_str   *t_str;
  s_list **t;
  s_list  *tmp;
  buf_init_str_const(&buf, str);
  tmp = NULL;
  t = &tmp;
  while (1) {
    *t = list_new(NULL);
    (*t)->tag.type = TAG_STR;
    t_str = &(*t)->tag.data.str;
    if (! buf_read_until_list_into_str(&buf, sep, t_str)) {
      if (! buf_read_to_str(&buf, t_str))
        goto clean;
      break;
    }
    t = &(*t)->next.data.list;
  }
  *dest = tmp;
  return dest;
 clean:
  list_delete_all(tmp);
  return NULL;
}

bool * str_starts_with (const s_str *str, const s_str *start,
                        bool *dest)
{
  assert(str);
  assert(start);
  if (! start->size) {
    *dest = true;
    return dest;
  }
  if (str->size < start->size) {
    *dest = false;
    return dest;
  }
  *dest = ! memcmp(str->ptr.p, start->ptr.p, start->size);
  return dest;
}

bool * str_starts_with_case_insensitive (const s_str *str,
                                         const s_str *start,
                                         bool *dest)
{
  character str_c;
  s_str     str_i;
  character start_c;
  s_str     start_i;
  assert(str);
  assert(start);
  if (! start->size) {
    *dest = true;
    return dest;
  }
  if (str->size < start->size) {
    *dest = false;
    return dest;
  }
  str_i = *str;
  start_i = *start;
  while (start_i.size > 0) {
    if (str_read_character_utf8(&start_i, &start_c) < 0)
      return NULL;
    if (str_read_character_utf8(&str_i, &str_c) < 0)
      return NULL;
    start_c = character_to_lower(start_c);
    str_c = character_to_lower(str_c);
    if (start_c != str_c) {
      *dest = false;
      return dest;
    }
  }
  *dest = true;
  return dest;
}

uw * str_sw_pos_to_uw (sw pos, uw max_pos, uw *dest)
{
  assert(dest);
  if (pos >= 0) {
    if ((uw) pos > max_pos) {
      err_write_1("str_sw_pos_to_uw: index too large: ");
      err_inspect_sw(&pos);
      err_write_1(" > ");
      err_inspect_uw(&max_pos);
      err_write_1("\n");
      assert(! "str_sw_pos_to_uw: index too large");
      return NULL;
    }
    *dest = (uw) pos;
  }
  else {
    if (max_pos > SW_MAX || pos + 1 >= (sw) -max_pos)
      *dest = max_pos + pos + 1;
    else {
      err_write_1("str_sw_pos_to_uw: index too low: ");
      err_inspect_sw(&pos);
      err_write_1(" < -");
      err_inspect_uw_decimal(&max_pos);
      err_write_1("\n");
      assert(! "str_sw_pos_to_uw: index too low");
      return NULL;
    }
  }
  return dest;
}

s_str * str_to_hex (const s_str *src, s_str *dest)
{
  s_buf buf;
  sw size;
  assert(src);
  assert(dest);
  if (src->size == 0)
    return str_init_empty(dest);
  size = src->size * 2;
  buf_init_alloc(&buf, size);
  buf_str_to_hex(&buf, src);
  return buf_to_str(&buf, dest);
}

s_ident * str_to_ident (const s_str *src, s_ident *ident)
{
  const s_sym *sym;
  sym = str_to_sym(src);
  return ident_init(ident, NULL, sym);
}

const s_sym * str_to_sym (const s_str *src)
{
  const s_sym *sym;
  sym = sym_find(src);
  if (! sym)
    sym = sym_new(src);
  return sym;
}
