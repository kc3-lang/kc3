/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "character.h"
#include "ident.h"
#include "str.h"
#include "sym.h"

e_bool str_character_is_reserved (character c)
{
  return ! character_is_printable(c) ||
    c == '"' ||
    c == '\\';
}

void str_clean (s_str *str)
{
  assert(str);
  if (str->free.p)
    free(str->free.p);
}

s_str * str_copy (const s_str *src, s_str *dest)
{
  return str_init_dup(dest, src);
}

sw str_compare (const s_str *a, const s_str *b)
{
  sw r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  if (a->size < b->size)
    return -1;
  if (a->size > b->size)
    return 1;
  r = memcmp(a->ptr.p, b->ptr.p, a->size);
  if (r < 0)
    return -1;
  if (r > 0)
    return 1;
  return 0;
}

void str_delete (s_str *str)
{
  str_clean(str);
  free(str);
}

e_bool str_has_reserved_characters (const s_str *src)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, src->size, src->ptr.p);
  while ((r = str_read_character(&stra, &c)) > 0) {
    if (str_character_is_reserved(c))
      return true;
  }
  if (r < 0)
    return true;
  return false;
}

t_hash_context * str_hash_update (t_hash_context *context,
                                  const s_str *str)
{
  assert(str);
  hash_update(context, &str->size, sizeof(str->size));
  return hash_update(context, str->ptr.p, str->size);
}

s_str * str_init (s_str *str, s8 *free, uw size, const s8 *p)
{
  assert(str);
  str->free.p = free;
  str->size = size;
  str->ptr.p = p;
  return str;
}

s_str * str_init_1 (s_str *str, s8 *free, const s8 *p)
{
  assert(str);
  str->free.p = free;
  str->size = strlen(p);
  str->ptr.p = p;
  return str;
}

s_str * str_init_alloc (s_str *str, uw size, const s8 *p)
{
  assert(str);
  str->free.p = malloc(size);
  str->size = size;
  str->ptr.p = str->free.p;
  if (! str->ptr.p)
    err(1, "out of memory");
  memcpy(str->free.p, p, size);
  return str;
}

s_str * str_init_dup (s_str *str, const s_str *src)
{
  assert(str);
  assert(src);
  str->free.p = malloc(src->size);
  str->size = src->size;
  str->ptr.p = str->free.p;
  memcpy(str->free.p, src->ptr.p, str->size);
  return str;
}

s_str * str_init_dup_1 (s_str *str, const s8 *src)
{
  uw len;
  assert(str);
  assert(src);
  len = strlen(src);
  str->free.p = malloc(len + 1);
  str->size = len;
  str->ptr.p = str->free.p;
  memcpy(str->free.p, src, len + 1);
  return str;
}

s_str * str_init_empty (s_str *str)
{
  assert(str);
  str->free.p = NULL;
  str->size = 0;
  str->ptr.p = NULL;
  return str;
}

s_str * str_init_str (s_str *str, const s_str *src)
{
  assert(str);
  assert(src);
  str->free = src->free;
  str->size = src->size;
  str->ptr = src->ptr;
  return str;
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

s_str * str_new (s8 *free, uw size, const s8 *p)
{
  s_str *str;
  str = malloc(sizeof(s_str));
  if (! str)
    err(1, "out of memory");
  str_init(str, free, size, p);
  return str;
}

s_str * str_new_1 (s8 *free, const s8 *s)
{
  size_t len = strlen(s);
  s_str *str = str_new(free, len, s);
  return str;
}

s_str * str_new_cpy (uw size, const s8 *p)
{
  s8 *a;
  s_str *str;
  a = malloc(size);
  if (! a)
    err(1, "out of memory");
  memcpy(a, p, size);
  str = str_new(a, size, a);
  return str;
}

s_str * str_new_dup (const s_str *src)
{
  s8 *n;
  s_str *str;
  assert(src);
  n = malloc(src->size);
  memcpy(n, src->ptr.p, src->size);
  str = str_new(n, src->size, n);
  return str;
}

s_str * str_new_empty ()
{
  s_str *str;
  str = str_new(NULL, 0, NULL);
  return str;
}

s_str * str_new_f (const char *fmt, ...)
{
  va_list ap;
  s_str *str;
  va_start(ap, fmt);
  str = str_new_vf(fmt, ap);
  va_end(ap);
  return str;
}

s_str * str_new_vf (const char *fmt, va_list ap)
{
  int len;
  char *s;
  s_str *str;
  len = vasprintf(&s, fmt, ap);
  if (len < 0)
    err(1, "vasprintf");
  str = str_new(s, len, s);
  return str;
}

sw str_peek_character (const s_str *str, character *c)
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

sw str_read (s_str *str, u8 *c)
{
  if (str->size <= 0)
    return 0;
  str->size--;
  *c = *str->ptr.pu8;
  str->ptr.pu8++;
  return 1;
}

sw str_read_character (s_str *str, character *c)
{
  sw size;
  if (str->size == 0)
    return 0;
  size = str_peek_character(str, c);
  if (size < 0)
    return size;
  str->size -= size;
  str->ptr.p = str->ptr.ps8 + size;
  return size;
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
  return ident_init(ident, sym);
}

const s_sym * str_to_sym (const s_str *src)
{
  const s_sym *sym;
  sym = sym_find(src);
  if (! sym)
    sym = sym_new(src);
  return sym;
}
