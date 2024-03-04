/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
//#include <stdarg.h>
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "character.h"
#include "ident.h"
#include "list.h"
#include "operator.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

#define DEF_STR_INIT(type)                                             \
  s_str * str_init_ ## type (s_str *str, type x)                       \
  {                                                                    \
    s_buf buf;                                                         \
    sw size;                                                           \
    size = buf_inspect_ ## type ## _size(&x);                          \
    if (size <= 0)                                                     \
      return NULL;                                                     \
    buf_init_alloc(&buf, size);                                        \
    buf_inspect_ ## type(&buf, &x);                                    \
    assert(buf.wpos == (uw) size);                                     \
    return buf_to_str(&buf, str);                                      \
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
  s_str tmp;
  character tmp_c;
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

bool str_has_reserved_characters (const s_str *src)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, src->size, src->ptr.p);
  while ((r = str_read_character_utf8(&stra, &c)) > 0) {
    if (str_character_is_reserved(c))
      return true;
  }
  if (r < 0)
    return true;
  return false;
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

s_str * str_init_alloc (s_str *str, uw size, const char *p)
{
  s_str tmp;
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

s_str * str_init_cast (s_str *str, const s_tag *tag)
{
  assert(str);
  assert(tag);
  switch (tag->type) {
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
  default:
    break;
  }
  err_write_1("str_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Str");
  assert(! "str_init_cast: cannot cast to Str");
  return NULL;
}

s_str * str_init_cat (s_str *str, const s_str *a, const s_str *b)
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

DEF_STR_INIT(s8)
DEF_STR_INIT(s16)
DEF_STR_INIT(s32)
DEF_STR_INIT(s64)
DEF_STR_INIT(sw)

s_str * str_init_slice (s_str *str, const s_str *src, sw start, sw end)
{
  s_buf buf;
  s_str tmp = {0};
  assert(str);
  assert(src);
  buf_init(&buf, false, src->size, (char *) src->ptr.pchar);
  if (! str_sw_pos_to_uw(start, src->size, &buf.rpos) ||
      ! str_sw_pos_to_uw(end, src->size, &buf.wpos))
    return NULL;
  if (buf.rpos > buf.wpos) {
    err_write_1("str_init_slice: invalid positions: ");
    err_inspect_uw(&buf.rpos);
    err_write_1(" > ");
    err_inspect_uw(&buf.wpos);
    err_write_1("\n");
    assert(! "str_init_slice: invalid positions");
    return NULL;
  }
  if (! buf_read_to_str(&buf, &tmp))
    return NULL;
  *str = tmp;
  return str;
}

DEF_STR_INIT(u8)
DEF_STR_INIT(u16)
DEF_STR_INIT(u32)
DEF_STR_INIT(u64)
DEF_STR_INIT(uw)

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

s_str * str_inspect (const s_str *src, s_str *dest)
{
  s_buf buf;
  sw size;
  size = buf_inspect_str_size(src);
  if (size <= 0)
    return NULL;
  buf_init_alloc(&buf, size);
  buf_inspect_str(&buf, src);
  assert(buf.wpos == (uw) size);
  return buf_to_str(&buf, dest);
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
  character c;
  s_buf in_buf;
  s_list  *list;
  s_list **list_end;
  s_list  *list_start = NULL;
  s_ident op;
  s_buf out_buf = {0};
  sw r;
  s_tag *right;
  s_tag tmp = {0};
  s_tag tmp1 = {0};
  buf_init_str(&in_buf, false, (s_str *) str);
  if (! buf_init_alloc(&out_buf, str->size))
    goto restore;
  list_end = &list_start;
  while (1) {
    r = buf_read_1(&in_buf, "#{");
    if (r < 0)
      break;
    if (r > 0) {
      if (out_buf.wpos > 0) {
        *list_end = list_new(NULL);
        (*list_end)->tag.type = TAG_STR;
        buf_read_to_str(&out_buf, &(*list_end)->tag.data.str);
        list_end = &(*list_end)->next.data.list;
        out_buf.rpos = out_buf.wpos = 0;
      }
      *list_end = list_new(NULL);
      r = buf_parse_tag(&in_buf, &(*list_end)->tag);
      if (r <= 0)
        goto restore;
      list_end = &(*list_end)->next.data.list;
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
    *list_end = list_new(NULL);
    (*list_end)->tag.type = TAG_STR;
    buf_read_to_str(&out_buf, &(*list_end)->tag.data.str);
  }
  if (! list_start)
    tag_init_str_empty(&tmp);
  else {
    list = list_start;
    if (list->tag.type == TAG_STR)
      tmp = list->tag;
    else {
      if (! tag_init_call_cast(&tmp, &g_sym_Str))
        goto restore;
      tmp.data.call.arguments->tag = list->tag;
    }
    list = list_next(list);
    while (list) {
      tmp1 = (s_tag) {0};
      tmp1.type = TAG_CALL;
      if (! call_init_op(&tmp1.data.call))
        goto restore;
      op.module = &g_sym_C3;
      op.sym = &g_sym__plus;
      operator_resolve(&op, 2, &tmp1.data.call.ident);
      tmp1.data.call.arguments->tag = tmp;
      right = &tmp1.data.call.arguments->next.data.list->tag;
      if (list->tag.type == TAG_STR)
        *right = list->tag;
      else {
        if (! tag_init_call_cast(right, &g_sym_Str))
          goto restore;
        right->data.call.arguments->tag = list->tag;
      }
      tmp = tmp1;
      list = list_next(list);
    }
  }
  *dest = tmp;
  while (list_start) {
    list = list_next(list_start);
    free(list_start);
    list_start = list;
  }
  buf_clean(&out_buf);
  return true;
 restore:
  list_delete_all(list_start);
  buf_clean(&out_buf);
  return false;
}

sw str_peek_character_utf8 (const s_str *str, character *c)
{
  assert(str);
  assert(c);
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

uw * str_rindex_character (const s_str *str, character c, uw *dest)
{
  uw i = 0;
  sw result = -1;
  s_str s;
  character tmp;
  s = *str;
  while (str_read_character_utf8(&s, &tmp) > 0) {
    if (c == tmp)
      result = i;
    i++;
  }
  if (result >= 0) {
    *dest = result;
    return dest;
  }
  return NULL;
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
    if (max_pos > SW_MAX || pos >= (sw) -max_pos)
      *dest = max_pos - pos;
    else {
      err_write_1("str_sw_pos_to_uw: index too low: ");
      err_inspect_sw(&pos);
      err_write_1(" < -");
      err_inspect_uw(&max_pos);
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
