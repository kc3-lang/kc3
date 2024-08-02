/* kc3
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
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_save.h"
#include "character.h"
#include "error.h"
#include "ratio.h"
#include "str.h"
#include "sym.h"

void buf_clean (s_buf *buf)
{
  assert(buf);
  if (buf->free)
    free(buf->ptr.p);
}

void buf_delete (s_buf *buf)
{
  assert(buf);
  buf_clean(buf);
  free(buf);
}

void buf_empty (s_buf *buf)
{
  buf->wpos = 0;
  buf->rpos = 0;
}

sw buf_f (s_buf *buf, const char *fmt, ...)
{
  va_list ap;
  sw r;
  va_start(ap, fmt);
  r = buf_vf(buf, fmt, ap);
  va_end(ap);
  return r;
}

sw buf_flush (s_buf *buf)
{
  assert(buf);
  if (buf->flush)
    return buf->flush(buf);
  return buf->size - buf->wpos;
}

sw buf_ignore (s_buf *buf, uw size)
{
  u8 b;
  character c;
  uw i = 0;
  sw r;
  assert(buf);
  if (size == 0)
    return 0;
  if ((r = buf_refill(buf, size)) < 0)
    return r;
  if ((uw) r < size)
    return -1;
  if (buf->line >= 0) {
    while (i < size) {
      if ((r = buf_read_character_utf8(buf, &c)) < 0)
        return r;
      if (r > 0) {
        i += r;
        continue;
      }
      if ((r = buf_read_u8(buf, &b)) < 0)
        return r;
      if (r > 0) {
          buf->column++;
        i += r;
        continue;
      }
      error("buf_ignore");
      return -1;
    }
    assert(i == size);
  }
  else
    buf->rpos += size;
  return size;
}

sw buf_ignore_line (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  while ((r = buf_peek_character_utf8(buf, &c)) > 0) {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      return r;
    result += csize;
    if (c == '\n')
      break;
  }
  if (r < 0)
    return r;
  return result;
}

sw buf_ignore_newline (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  buf_save_init(buf, &save);
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c)) {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      goto clean;
    result += csize;
    if (c == '\n') {
      r = result;
      goto clean;
    }
  }
  buf_save_restore_rpos(buf, &save);
  r = 0;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_ignore_spaces (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  assert(buf);
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c)) {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      return r;
    result += csize;
  }
  if (! result && r < 0)
    return r;
  return result;
}

sw buf_ignore_spaces_but_newline (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  assert(buf);
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c) &&
         c != '\n') {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      return r;
    result += csize;
  }
  return result;
}

s_buf * buf_init (s_buf *buf, bool p_free, uw size, char *p)
{
  s_buf tmp = {0};
  assert(buf);
  tmp.free = p_free;
  tmp.line = 1;
  tmp.ptr.pchar = p;
  tmp.size = size;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_const (s_buf *buf, uw size, const char *p)
{
  s_buf tmp = {0};
  assert(buf);
  tmp.line = 1;
  tmp.ptr.pchar = (char *) p;
  tmp.read_only = true;
  tmp.size = size;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_1 (s_buf *buf, bool p_free, char *p)
{
  uw len;
  s_buf tmp = {0};
  assert(buf);
  assert(p);
  len = strlen(p);
  buf_init(&tmp, p_free, len, p);
  tmp.wpos = len;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_1_const (s_buf *buf, const char *p)
{
  uw len;
  s_buf tmp = {0};
  assert(buf);
  assert(p);
  len = strlen(p);
  buf_init(&tmp, false, len, (char *) p);
  tmp.read_only = true;
  tmp.wpos = len;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_1_copy (s_buf *buf, const char *p)
{
  uw size;
  s_buf tmp = {0};
  assert(buf);
  assert(p);
  size = strlen(p);
  if (! buf_init_alloc(&tmp, size))
    return NULL;
  memcpy(tmp.ptr.p, p, size);
  tmp.wpos = size;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_alloc (s_buf *buf, uw size)
{
  char *p;
  assert(buf);
  if (! size)
    return buf_init(buf, false, 0, NULL);
  p = alloc(size + 1);
  if (! p)
    return NULL;
  return buf_init(buf, true, size, p);
}

s_buf * buf_init_str (s_buf *buf, bool p_free, s_str *p)
{
  s_buf tmp = {0};
  assert(buf);
  assert(p);
  buf_init(&tmp, p_free, p->size, (char *) p->ptr.pchar);
  tmp.wpos = p->size;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_str_const (s_buf *buf, const s_str *p)
{
  s_buf tmp = {0};
  assert(buf);
  assert(p);
  buf_init_const(&tmp, p->size, p->ptr.pchar);
  tmp.wpos = p->size;
  *buf = tmp;
  return buf;
}

s_buf * buf_init_str_copy (s_buf *buf, const s_str *str)
{
  s_buf tmp = {0};
  assert(buf);
  assert(str);
  buf_init_alloc(&tmp, str->size);
  memcpy(tmp.ptr.p, str->ptr.p, str->size);
  tmp.wpos = str->size;
  *buf = tmp;
  return buf;
}

s_buf * buf_new (bool p_free, uw size, char *p)
{
  s_buf *buf;
  buf = alloc(sizeof(s_buf));
  if (! buf)
    return NULL;
  buf_init(buf, p_free, size, p);
  return buf;
}

s_buf * buf_new_alloc (uw size)
{
  s_buf *buf;
  buf = alloc(sizeof(s_buf));
  if (! buf)
    return NULL;
  if (! buf_init_alloc(buf, size)) {
    free(buf);
    return NULL;
  }
  return buf;
}

sw buf_peek_1 (s_buf *buf, const char *p)
{
  s_str stra;
  assert(buf);
  str_init_1(&stra, NULL, p);
  return buf_peek_str(buf, &stra);
}

sw buf_peek_next_character_utf8 (s_buf *buf, character *c)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  character tmp;
  assert(buf);
  assert(c);
  buf_save_init(buf, &save);
  if ((r = buf_read_character_utf8(buf, &tmp)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_peek_character_utf8(buf, &tmp)) <= 0)
    goto restore;
  result += r;
  *c = tmp;
  r = result;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_peek_character_utf8 (s_buf *buf, character *c)
{
  assert(buf);
  assert(c);
  const u8 *b;
  sw r;
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
  if ((r = buf_refill(buf, 1)) < 0)
    return r;
  if (r < 1)
    return -1;
  b = buf->ptr.pu8 + buf->rpos;
  if ((b[0] & _10000000) == 0) {
    *c = b[0];
    return 1;
  }
  if ((b[0] & _11100000) == _11000000) {
    if ((r = buf_refill(buf, 2)) < 0)
      return r;
    if (r < 2)
      return -1;
    if ((b[1] & _11000000) != _10000000)
      return 0;
    x[0] = b[0] & _00011111;
    x[1] = b[1] & _00111111;
    *c = (x[0] << 6) | x[1];
    return 2;
  }
  if ((b[0] & _11110000) == _11100000) {
    if ((r = buf_refill(buf, 3)) < 0)
      return r;
    if (r < 3)
      return -1;
    if ((b[1] & _11000000) != _10000000)
      return 0;
    if ((b[2] & _11000000) != _10000000)
      return 0;
    x[0] = b[0] & _00001111;
    x[1] = b[1] & _00111111;
    x[2] = b[2] & _00111111;
    *c = (x[0] << 12) | (x[1] << 6) | x[2];
    return 3;
  }
  if ((b[0] & _11111000) == _11110000) {
    if ((r = buf_refill(buf, 4)) < 0)
      return r;
    if (r < 4)
      return -1;
    if ((b[1] & _11000000) != _10000000)
      return 0;
    if ((b[2] & _11000000) != _10000000)
      return 0;
    if ((b[3] & _11000000) != _10000000)
      return 0;
    x[0] = b[0] & _00000111;
    x[1] = b[1] & _00111111;
    x[2] = b[2] & _00111111;
    x[3] = b[3] & _00111111;
    *c = (x[0] << 18) | (x[1] << 12) | (x[2] << 6) | x[3];
    return 4;
  }
  return 0;
}

sw buf_peek_f32 (s_buf *buf, f32 *p)
{
  const sw size = sizeof(f32);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size) {
    assert(! "buf error");
    return -1;
  }
  if (buf->rpos + size > buf->wpos)
    return 0;
  *p = *((f32 *) (buf->ptr.pu8 + buf->rpos));
  return size;
}

sw buf_peek_f64 (s_buf *buf, f64 *p)
{
  const sw size = sizeof(f64);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size) {
    assert(! "buf error");
    return -1;
  }
  if (buf->rpos + size > buf->wpos)
    return 0;
  *p = *((f64 *) (buf->ptr.pu8 + buf->rpos));
  return size;
}

sw buf_peek_s8 (s_buf *buf, s8 *p)
{
    const sw size = sizeof(s8);
    assert(buf);
    assert(p);
    if (buf->rpos > buf->wpos ||
        buf->wpos > buf->size) {
        assert(! "buf error");
        return -1;
    }
    if (buf->rpos + size > buf->wpos)
        return 0;
    *p = *((s8 *) (buf->ptr.pu8 + buf->rpos));
    return size;
}

sw buf_peek_s16 (s_buf *buf, s16 *p)
{
    const sw size = sizeof(s16);
    assert(buf);
    assert(p);
    if (buf->rpos > buf->wpos ||
        buf->wpos > buf->size) {
        assert(! "buf error");
        return -1;
    }
    if (buf->rpos + size > buf->wpos)
        return 0;
    *p = *((s16 *) (buf->ptr.pu8 + buf->rpos));
    return size;
}

sw buf_peek_s32 (s_buf *buf, s32 *p)
{
  const sw size = sizeof(s32);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size) {
    assert(! "buf error");
    return -1;
  }
  if (buf->rpos + size > buf->wpos)
    return 0;
  *p = *((s32 *) (buf->ptr.pu8 + buf->rpos));
  return size;
}

sw buf_peek_s64 (s_buf *buf, s64 *p)
{
  const sw size = sizeof(s64);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size) {
    assert(! "buf error");
    return -1;
  }
  if (buf->rpos + size > buf->wpos)
    return 0;
  *p = *((s64 *) (buf->ptr.pu8 + buf->rpos));
  return size;
}

sw buf_peek_str (s_buf *buf, const s_str *src)
{
  sw size;
  assert(buf);
  assert(src);
  if (! src->size)
    return 0;
  if (buf->rpos > buf->wpos) {
    assert(buf->rpos <= buf->wpos);
    return -1;
  }
  if (buf->wpos > buf->size) {
    assert(buf->wpos <= buf->size);
    return -1;
  }
  size = buf->wpos - buf->rpos;
  if ((uw) size < src->size) {
    if (memcmp(buf->ptr.pchar + buf->rpos, src->ptr.p, size))
      return 0;
  }
  if (buf->rpos + src->size > buf->wpos &&
      buf_refill(buf, src->size) < (sw) src->size)
    return 0;
  if (memcmp(buf->ptr.pchar + buf->rpos, src->ptr.p, src->size))
    return 0;
  return src->size;
}

sw buf_peek_to_str (s_buf *buf, s_str *dest)
{
  sw size;
  assert(buf);
  assert(dest);
  if (buf->rpos > buf->wpos)
    return -1;
  if (buf->wpos > buf->size)
    return -1;
  size = buf->wpos - buf->rpos;
  if (size == 0) {
    str_init_empty(dest);
    return 0;
  }
  str_init_alloc(dest, size, buf->ptr.pchar + buf->rpos);
  return size;
}

sw buf_peek_u8 (s_buf *buf, u8 *p)
{
  sw r;
  const sw size = sizeof(u8);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos) {
    assert(! "buf_peek_u8: rpos > wpos");
    return -1;
  }
  if (buf->wpos > buf->size) {
    assert(! "buf_peek_u8: wpos > size");
    return -1;
  }
  if (buf->rpos + size > buf->wpos) {
    if ((r = buf_refill(buf, size)) < 0)
      return r;
    if (r < size)
      return -1;
  }
  if (buf->rpos + size > buf->wpos) {
    assert(! "buf_peek_u8: buffer overflow");
    return -1;
  }
  *p = buf->ptr.pu8[buf->rpos];
  return size;
}

sw buf_peek_u16 (s_buf *buf, u16 *p)
{
  const sw size = sizeof(u16);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size) {
    assert(! "buf error");
    return -1;
  }
  if (buf->rpos + size > buf->wpos)
    return 0;
  *p = *((u16 *) (buf->ptr.pu8 + buf->rpos));
  return size;
}

sw buf_peek_u32 (s_buf *buf, u32 *p)
{
  const sw size = sizeof(u32);
  assert(buf);
  assert(p);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size) {
    assert(! "buf error");
    return -1;
  }
  if (buf->rpos + size > buf->wpos)
    return 0;
  *p = *((u32 *) (buf->ptr.pu8 + buf->rpos));
  return size;
}

sw buf_peek_u64 (s_buf *buf, u64 *p)
{
    const sw size = sizeof(u64);
    assert(buf);
    assert(p);
    if (buf->rpos > buf->wpos ||
        buf->wpos > buf->size) {
        assert(! "buf error");
        return -1;
    }
    if (buf->rpos + size > buf->wpos)
        return 0;
    *p = *((u64 *) (buf->ptr.pu8 + buf->rpos));
    return size;
}

sw buf_read_1 (s_buf *buf, const char *p)
{
  s_str str;
  assert(buf);
  str_init_1(&str, NULL, p);
  return buf_read_str(buf, &str);
}

sw buf_read_character_utf8 (s_buf *buf, character *p)
{
  sw r;
  r = buf_peek_character_utf8(buf, p);
  if (r > 0) {
    if (buf->line >= 0) {
      if (*p == '\n') {
        buf->column = 0;
        buf->line++;
      }
      else
        buf->column++;
    }
    buf->rpos += r;
  }
  return r;
}

sw buf_read_f32 (s_buf *buf, f32 *p)
{
  sw r;
  r = buf_peek_f32(buf, p);
  if (r > 0) {
    assert(r == sizeof(f32));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_f64 (s_buf *buf, f64 *p)
{
  sw r;
  r = buf_peek_f64(buf, p);
  if (r > 0) {
    assert(r == sizeof(f64));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_s8(s_buf *buf, s8 *p)
{
  sw r;
  r = buf_peek_s8(buf, p);
  if (r > 0) {
    assert(r == sizeof(s8));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_s16(s_buf *buf, s16 *p)
{
  sw r;
  r = buf_peek_s16(buf, p);
  if (r > 0) {
    assert(r == sizeof(s16));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_s32(s_buf *buf, s32 *p)
{
  sw r;
  r = buf_peek_s32(buf, p);
  if (r > 0) {
    assert(r == sizeof(s32));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_s64(s_buf *buf, s64 *p)
{
  sw r;
  r = buf_peek_s64(buf, p);
  if (r > 0) {
    assert(r == sizeof(s64));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_str (s_buf *buf, const s_str *src)
{
  sw r;
  r = buf_peek_str(buf, src);
  if (r > 0)
    return buf_ignore(buf, r);
  return r;
}

sw buf_read_sym (s_buf *buf, const s_sym *src)
{
  character c;
  sw r;
  s_buf_save save;
  buf_save_init(buf, &save);
  r = buf_read_str(buf, &src->str);
  if (r > 0 &&
      buf_peek_character_utf8(buf, &c) > 0 &&
      ! sym_character_is_reserved(c))
    goto restore;
  goto clean;
 restore:
  r = 0;
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

s_str * buf_read_to_str (s_buf *buf, s_str *dest)
{
  sw r;
  sw size;
  assert(buf);
  assert(dest);
  if (buf->rpos > buf->wpos) {
    err_puts("buf_read_to_str: buf->rpos > buf->wpos");
    assert(! "buf_read_to_str: buf->rpos > buf->wpos");
    return NULL;
  }
  if (buf->wpos > buf->size) {
    err_puts("buf_read_to_str: buf->wpos > buf->size");
    assert(! "buf_read_to_str: buf->wpos > buf->size");
    return NULL;
  }
  size = buf->wpos - buf->rpos;
  if (! size) {
    str_init_empty(dest);
    return dest;
  }
  if (! str_init_alloc(dest, size, buf->ptr.pchar + buf->rpos)) {
    err_puts("buf_read_to_str: str_init_alloc");
    assert(! "buf_read_to_str: str_init_alloc");
    return NULL;
  }
  r = buf_ignore(buf, size);
  if (r < 0) {
    err_puts("buf_read_to_str: buf_ignore");
    assert(! "buf_read_to_str: buf_ignore");
    str_clean(dest);
    return NULL;
  }
  return dest;
}

sw buf_read_u8 (s_buf *buf, u8 *p)
{
  sw r;
  r = buf_peek_u8(buf, p);
  if (r > 0)
    buf->rpos += r;
  return r;
}

sw buf_read_u16 (s_buf *buf, u16 *p)
{
  sw r;
  r = buf_peek_u16(buf, p);
  if (r > 0) {
    assert(r == sizeof(u16));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_u32 (s_buf *buf, u32 *p)
{
  sw r;
  r = buf_peek_u32(buf, p);
  if (r > 0) {
    assert(r == sizeof(u32));
    buf->rpos += r;
  }
  return r;
}

sw buf_read_u64 (s_buf *buf, u64 *p)
{
  sw r;
  r = buf_peek_u64(buf, p);
  if (r > 0) {
    assert(r == sizeof(u64));
    buf->rpos += r;
  }
  return r;
}

s_str * buf_read_until_1_into_str(s_buf *buf, const char *end, s_str *dest)
{
  character c;
  sw r;
  s_buf_save save;
  s_buf tmp;
  buf_save_init(buf, &save);
  while (1) {
    if ((r = buf_read_1(buf, end)) < 0)
      goto restore;
    if (r) {
      buf_init(&tmp, false, buf->size, buf->ptr.pchar);
      tmp.rpos = save.rpos;
      tmp.wpos = buf->rpos - strlen(end);
      if (! buf_read_to_str(&tmp, dest))
        goto restore;
      buf_save_clean(buf, &save);
      return dest;
    }
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
  }
 restore:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return NULL;
}

sw buf_read_until_character_into_str (s_buf *buf, character end, s_str *dest)
{
  s_str end_str;
  sw r;
  s_str tmp;
  if (! str_init_character(&end_str, end))
    return -1;
  if ((r = buf_read_until_str_into_str(buf, &end_str, &tmp)) > 0)
    *dest = tmp;
  str_clean(&end_str);
  return r;
}

sw buf_read_until_str_into_str (s_buf *buf, const s_str *end, s_str *dest)
{
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf tmp;
  buf_save_init(buf, &save);
  while (1) {
    if ((r = buf_read_str(buf, end)) < 0)
      goto restore;
    if (r) {
      result += r;
      buf_init(&tmp, false, buf->size, buf->ptr.pchar);
      tmp.rpos = save.rpos;
      tmp.wpos = buf->rpos;
      if (! buf_read_to_str(&tmp, dest)) {
        r = -1;
        goto restore;
      }
      r = result;
      goto clean;
    }
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
    result += r;
  }
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_refill (s_buf *buf, sw size)
{
  sw r = buf->wpos - buf->rpos;
  assert(buf);
  if (size < 0) {
    err_puts("buf_refill: size < 0");
    assert(! "buf_refill: size < 0");
    return -1;
  }
  if (buf->read_only ||
      ! size)
    return r;
  if (buf->rpos + size > buf->wpos) {
    if ((r = buf_refill_compact(buf)) < 0) {
      err_puts("buf_refill: buf_refill_compact");
      assert(! "buf_refill: buf_refill_compact");
      return r;
    }
    if (buf->refill)
      while (1) {
        if ((r = buf->refill(buf)) < 0)
          return r;
        if (! r)
          return -1;
        if (buf->wpos - buf->rpos >= (uw) size)
          break;
      }
  }
  r = buf->wpos - buf->rpos;
  return r;
}

sw buf_refill_compact (s_buf *buf)
{
  uw min_rpos;
  s_buf_save *save;
  uw size;
  assert(buf);
  if (buf->read_only)
    return 0;
  min_rpos = buf_save_min_rpos(buf);
  if (min_rpos > buf->wpos) {
    assert(min_rpos <= buf->wpos);
    return -1;
  }
  if (buf->wpos > buf->size) {
    assert(buf->wpos <= buf->size);
    return -1;
  }
  if (min_rpos > 0) {
    if (min_rpos == buf->wpos) {
      buf->rpos = 0;
      buf->wpos = 0;
    }
    else {
      size = buf->wpos - min_rpos;
      assert(size < buf->size);
      memmove(buf->ptr.p,
              buf->ptr.pchar + min_rpos,
              size);
      buf->rpos -= min_rpos;
      save = buf->save;
      while (save) {
        save->rpos -= min_rpos;
        save->wpos -= min_rpos;
        save = save->next;
      }
      buf->wpos = size;
    }
    return 1;
  }
  return 0;
}

s_str * buf_slice_to_str (const s_buf *buf, uw start, uw end,
                          s_str *dest)
{
  s_str tmp;
  assert(buf);
  assert(dest);
  if (start > buf->wpos) {
    err_puts("buf_slice_to_str: start > wpos");
    assert(! "buf_slice_to_str: start > wpos");
    return NULL;
  }
  if (end < start) {
    err_puts("buf_slice_to_str: end < start");
    assert(! "buf_slice_to_str: end < start");
    return NULL;
  }
  if (end > buf->wpos) {
    err_puts("buf_slice_to_str: end > wpos");
    assert(! "buf_slice_to_str: end > wpos");
    return NULL;
  }
  str_init(&tmp, NULL, end - start, buf->ptr.pchar + start);
  return str_init_copy(dest, &tmp);
}

sw buf_str_to_hex (s_buf *buf, const s_str *src)
{
  const u8 *b;
  sw size;
  uw i;
  if (src->size == 0)
    return 0;
  size = src->size * 2;
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  b = src->ptr.pu8;
  i = 0;
  while (i++ < src->size)
    buf_u8_to_hex(buf, b++);
  return size;
}

s_str * buf_to_str (const s_buf *buf, s_str *str)
{
  void *p_free;
  assert(buf);
  assert(str);
  p_free = buf->free ? buf->ptr.p : NULL;
  return str_init(str, p_free, buf->size, buf->ptr.p);
}

sw buf_u8_to_hex (s_buf *buf, const u8 *x)
{
  u8 digit;
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  digit = *x >> 4;
  if (digit < 10)
    r = buf_write_u8(buf, digit + '0');
  else
    r = buf_write_u8(buf, digit - 10 + 'A');
  if (r <= 0)
    goto clean;
  result += r;
  digit = *x & 0xF;
  if (digit < 10)
    r = buf_write_u8(buf, digit + '0');
  else
    r = buf_write_u8(buf, digit - 10 + 'A');
  if (r <= 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_u8_to_hex_size(const u8 *u)
{
  (void) u;
  return 2;
}

sw buf_vf (s_buf *buf, const char *fmt, va_list ap)
{
  sw r;
  s_str *str;
  str = str_new_vf(fmt, ap);
  r = buf_write_str(buf, str);
  str_delete(str);
  return r;
}

sw buf_write (s_buf *buf, const void *data, uw len)
{
  s_str str;
  str_init(&str, NULL, len, data);
  return buf_write_str(buf, &str);
}

sw buf_write_1 (s_buf *buf, const char *p)
{
  s_str stra;
  str_init_1(&stra, NULL, p);
  return buf_write_str(buf, &stra);
}

sw buf_write_character_utf8 (s_buf *buf, character c)
{
  sw size = character_utf8_size(c);
  if (size <= 0)
    return size;
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size) {
    return -1;
  }
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  character_utf8(c, buf->ptr.pchar + buf->wpos);
  buf->wpos += size;
  return size;
}

sw buf_write_f32 (s_buf *buf, f32 x)
{
  const sw size = 4;
  assert(buf);
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size)
    return -1;
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((f32 *) (buf->ptr.pu8 + buf->wpos)) = x;
  buf->wpos += size;
  return size;
}

sw buf_write_f64 (s_buf *buf, f64 v)
{
  const sw size = 8;
  assert(buf);
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size)
    return -1;
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((f64 *) (buf->ptr.pu8 + buf->wpos)) = v;
  buf->wpos += size;
  return size;
}

sw buf_write_s8 (s_buf *buf, s8 x)
{
  const sw size = 1;
  assert(buf);
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size)
    return -1;
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  buf->ptr.ps8[buf->wpos] = x;
  buf->wpos += size;
  return size;
}

sw buf_write_s16 (s_buf *buf, s16 x)
{
  const sw size = 2;
  assert(buf);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((s16 *) (buf->ptr.pu8 + buf->wpos)) = x;
  buf->wpos += size;
  return size;
}

sw buf_write_s32 (s_buf *buf, s32 x)
{
  const sw size = 4;
  assert(buf);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((s32 *) (buf->ptr.pu8 + buf->wpos)) = x;
  buf->wpos += size;
  return size;
}

sw buf_write_s64 (s_buf *buf, s64 v)
{
  const sw size = 8;
  assert(buf);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((s64 *) (buf->ptr.pu8 + buf->wpos)) = v;
  buf->wpos += size;
  return size;
}

sw buf_write_str (s_buf *buf, const s_str *src)
{
  sw r;
  assert(buf);
  assert(src);
  if (buf->wpos > buf->size)
    return -1;
  if (buf->wpos + src->size > buf->size &&
      (r = buf_flush(buf)) < (sw) src->size)
    return -1;
  if (buf->wpos + src->size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  memcpy(buf->ptr.pu8 + buf->wpos, src->ptr.p, src->size);
  buf->wpos += src->size;
  buf_flush(buf);
  return src->size;
}

sw buf_write_u8 (s_buf *buf, u8 x)
{
  const sw size = 1;
  assert(buf);
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size) {
    return -1;
  }
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  buf->ptr.pu8[buf->wpos] = x;
  buf->wpos += size;
  return size;
}

sw buf_write_u16 (s_buf *buf, u16 x)
{
  const sw size = 2;
  assert(buf);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((u16 *) (buf->ptr.pu8 + buf->wpos)) = x;
  buf->wpos += size;
  return size;
}

sw buf_write_u32 (s_buf *buf, u32 x)
{
  const sw size = 4;
  assert(buf);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((u32 *) (buf->ptr.pu8 + buf->wpos)) = x;
  buf->wpos += size;
  return size;
}

sw buf_write_u64 (s_buf *buf, u64 v)
{
  const sw size = 8;
  assert(buf);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  *((u64 *) (buf->ptr.pu8 + buf->wpos)) = v;
  buf->wpos += size;
  return size;
}

sw buf_xfer (s_buf *dest, s_buf *src, uw size)
{
  sw r = 0;
  assert(dest);
  assert(src);
  if (size == 0)
    return 0;
  assert(src->rpos <= src->wpos);
  assert(dest->rpos <= dest->wpos);
  if (src->rpos + size > src->wpos &&
      (r = buf_refill(src, size)) < (sw) size) {
    if (r < 0)
      return r;
    return 0;
  }
  if (dest->wpos + size > dest->size &&
      (r = buf_flush(dest)) < (sw) size) {
    if (r < 0)
      return r;
    return -1;
  }
  memcpy(dest->ptr.ps8 + dest->wpos, src->ptr.ps8 + src->rpos, size);
  src->rpos += size;
  dest->wpos += size;
  return size;
}

sw buf_xfer_reverse (s_buf *src, s_buf *dest)
{
  sw size;
  assert(src);
  assert(dest);
  size = src->wpos - src->rpos;
  if (dest->wpos + size > dest->size &&
      buf_flush(dest) < size)
    return -1;
  for (sw i = 0; i < size; i++)
    dest->ptr.ps8[dest->wpos + i] = src->ptr.ps8[src->wpos - 1 - i];
  src->rpos += size;
  dest->wpos += size;
  return size;
}
