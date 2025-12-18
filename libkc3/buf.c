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

#include <string.h>
#include <unistd.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_fd.h"
#include "buf_save.h"
#include "character.h"
#include "error.h"
#include "file.h"
#include "integer.h"
#include "list.h"
#include "pretty.h"
#include "ratio.h"
#include "rwlock.h"
#include "str.h"
#include "sym.h"

#if HAVE_PTHREAD

#define DEF_BUF_PEEK(type)                                            \
  sw buf_peek_ ## type (s_buf *buf, type *dest)                       \
  {                                                                   \
    sw r;                                                             \
    const sw size = sizeof(type);                                     \
    assert(buf);                                                      \
    assert(dest);                                                     \
    rwlock_w(buf->rwlock);                                            \
    if (buf->rpos > buf->wpos ||                                      \
        buf->wpos > buf->size) {                                      \
      err_puts("buf error");                                          \
      assert(! "buf error");                                          \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    if (buf->rpos + size > buf->wpos) {                               \
      if ((r = buf_refill(buf, size)) < 0)                            \
        goto clean;                                                   \
      if (r < size) {                                                 \
        r = -1;                                                       \
        goto clean;                                                   \
      }                                                               \
    }                                                                 \
    if (buf->rpos + size > buf->wpos) {                               \
      assert(! "buf_peek_" # type ": buffer overflow");               \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    *dest = *((type *) (buf->ptr.pchar + buf->rpos));                 \
    r = size;                                                         \
  clean:                                                              \
    rwlock_unlock_w(buf->rwlock);                                     \
    return r;                                                         \
  }

#define DEF_BUF_READ(type)                                            \
  sw buf_read_ ## type (s_buf *buf, type *dest)                       \
  {                                                                   \
    sw r;                                                             \
    assert(buf);                                                      \
    assert(dest);                                                     \
    rwlock_w(buf->rwlock);                                            \
    r = buf_peek_ ## type(buf, dest);                                 \
    if (r > 0) {                                                      \
      assert(r == sizeof(type));                                      \
      buf->rpos += r;                                                 \
    }                                                                 \
    rwlock_unlock_w(buf->rwlock);                                     \
    return r;                                                         \
  }

#define DEF_BUF_WRITE(type)                                           \
  sw buf_write_ ## type (s_buf *buf, type src)                        \
  {                                                                   \
    sw r;                                                             \
    const sw size = sizeof(type);                                     \
    assert(buf);                                                      \
    rwlock_w(buf->rwlock);                                            \
    if (buf->wpos + size > buf->size &&                               \
        buf_flush(buf) < size) {                                      \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    if (buf->wpos + size > buf->size) {                               \
      err_puts("buf_write_" # type ": buffer overflow");              \
      assert(! "buf_write_" # type ": buffer overflow");              \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    *((type *) (buf->ptr.pu8 + buf->wpos)) = src;                     \
    buf->wpos += size;                                                \
    r = size;                                                         \
  clean:                                                              \
    rwlock_unlock_w(buf->rwlock);                                     \
    return r;                                                         \
  }

#else /* HAVE_PTHREAD */

#define DEF_BUF_PEEK(type)                                            \
  sw buf_peek_ ## type (s_buf *buf, type *dest)                       \
  {                                                                   \
    sw r;                                                             \
    const sw size = sizeof(type);                                     \
    assert(buf);                                                      \
    assert(dest);                                                     \
    if (buf->rpos > buf->wpos ||                                      \
        buf->wpos > buf->size) {                                      \
      err_puts("buf error");                                          \
      assert(! "buf error");                                          \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    if (buf->rpos + size > buf->wpos) {                               \
      if ((r = buf_refill(buf, size)) < 0)                            \
        goto clean;                                                   \
      if (r < size) {                                                 \
        r = -1;                                                       \
        goto clean;                                                   \
      }                                                               \
    }                                                                 \
    if (buf->rpos + size > buf->wpos) {                               \
      assert(! "buf_peek_" # type ": buffer overflow");               \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    *dest = *((type *) (buf->ptr.pchar + buf->rpos));                 \
    r = size;                                                         \
  clean:                                                              \
    return r;                                                         \
  }

#define DEF_BUF_READ(type)                                            \
  sw buf_read_ ## type (s_buf *buf, type *dest)                       \
  {                                                                   \
    sw r;                                                             \
    assert(buf);                                                      \
    assert(dest);                                                     \
    r = buf_peek_ ## type(buf, dest);                                 \
    if (r > 0) {                                                      \
      assert(r == sizeof(type));                                      \
      buf->rpos += r;                                                 \
    }                                                                 \
    return r;                                                         \
  }

#define DEF_BUF_WRITE(type)                                           \
  sw buf_write_ ## type (s_buf *buf, type src)                        \
  {                                                                   \
    sw r;                                                             \
    const sw size = sizeof(type);                                     \
    assert(buf);                                                      \
    if (buf->wpos + size > buf->size &&                               \
        buf_flush(buf) < size) {                                      \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    if (buf->wpos + size > buf->size) {                               \
      err_puts("buf_write_" # type ": buffer overflow");              \
      assert(! "buf_write_" # type ": buffer overflow");              \
      r = -1;                                                         \
      goto clean;                                                     \
    }                                                                 \
    *((type *) (buf->ptr.pu8 + buf->wpos)) = src;                     \
    buf->wpos += size;                                                \
    r = size;                                                         \
  clean:                                                              \
    return r;                                                         \
  }

#endif /* HAVE_PTHREAD */

void buf_clean (s_buf *buf)
{
  assert(buf);
#if HAVE_PTHREAD
  if (buf->rwlock)
    rwlock_delete(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  buf->wpos = 0;
  buf->rpos = 0;
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if ((r = buf_refill(buf, size)) < 0) {
    err_puts("buf_ignore: buf_refill");
    goto clean;
  }
  if ((uw) r < size) {
    if (false)
      err_puts("buf_ignore: buf_refill < size");
    r = -1;
    goto clean;
  }
  while (i < size) {
    if ((r = buf_read_character_utf8(buf, &c)) > 0) {
      i += r;
      continue;
    }
    if ((r = buf_read_u8(buf, &b)) < 0) {
      err_puts("buf_ignore: failed to read");
      assert(! "buf_ignore: failed to read");
      goto clean;
    }
    if (r > 0) {
        buf->pretty.column++;
      i += r;
      continue;
    }
    err_puts("buf_ignore: failed to read");
    assert(! "buf_ignore: failed to read");
    r = -1;
    goto clean;
  }
  // assert(i == size);
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_ignore_line (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  while ((r = buf_peek_character_utf8(buf, &c)) > 0) {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      goto clean;
    result += csize;
    if (c == '\n')
      break;
  }
  if (r < 0)
    goto clean;
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_ignore_spaces (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  assert(buf);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c)) {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      goto clean;
    result += csize;
  }
  if (! result && r < 0)
    goto clean;
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_ignore_spaces_but_newline (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  assert(buf);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c) &&
         c != '\n') {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      goto clean;
    result += csize;
  }
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

s_buf * buf_init (s_buf *buf, bool p_free, uw size, char *p)
{
  s_buf tmp = {0};
 assert(buf);
  tmp.free = p_free;
  tmp.line = 1;
  tmp.ptr.pchar = p;
  tmp.size = size;
#if HAVE_PTHREAD
  tmp.rwlock = rwlock_new();
#endif
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
  tmp.wpos = size;
#if HAVE_PTHREAD
  tmp.rwlock = rwlock_new();
#endif
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
    return buf_init(buf, false, 0, "");
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
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if ((r = buf_refill(buf, 1)) < 0)
    goto clean;
  if (r < 1) {
    r = -1;
    goto clean;
  }
  b = buf->ptr.pu8 + buf->rpos;
  if ((b[0] & _10000000) == 0) {
    *c = b[0];
    r = 1;
    goto clean;
  }
  if ((b[0] & _11100000) == _11000000) {
    if ((r = buf_refill(buf, 2)) < 0)
      goto clean;
    if (r < 2) {
      r = -1;
      goto clean;
    }
    if ((b[1] & _11000000) != _10000000) {
      r = 0;
      goto clean;
    }
    x[0] = b[0] & _00011111;
    x[1] = b[1] & _00111111;
    *c = (x[0] << 6) | x[1];
    r = 2;
    goto clean;
  }
  if ((b[0] & _11110000) == _11100000) {
    if ((r = buf_refill(buf, 3)) < 0)
      goto clean;
    if (r < 3) {
      r = -1;
      goto clean;
    }
    if ((b[1] & _11000000) != _10000000) {
      r = 0;
      goto clean;
    }
    if ((b[2] & _11000000) != _10000000) {
      r = 0;
      goto clean;
    }
    x[0] = b[0] & _00001111;
    x[1] = b[1] & _00111111;
    x[2] = b[2] & _00111111;
    *c = (x[0] << 12) | (x[1] << 6) | x[2];
    r = 3;
    goto clean;
  }
  if ((b[0] & _11111000) == _11110000) {
    if ((r = buf_refill(buf, 4)) < 0)
      goto clean;
    if (r < 4) {
      r = -1;
      goto clean;
    }
    if ((b[1] & _11000000) != _10000000) {
      r = 0;
      goto clean;
    }
    if ((b[2] & _11000000) != _10000000) {
      r = 0;
      goto clean;
    }
    if ((b[3] & _11000000) != _10000000) {
      r = 0;
      goto clean;
    }
    x[0] = b[0] & _00000111;
    x[1] = b[1] & _00111111;
    x[2] = b[2] & _00111111;
    x[3] = b[3] & _00111111;
    *c = (x[0] << 18) | (x[1] << 12) | (x[2] << 6) | x[3];
    r = 4;
    goto clean;
  }
  r = 0;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

DEF_BUF_PEEK(f32)
DEF_BUF_PEEK(f64)
#if HAVE_F80
DEF_BUF_PEEK(f80)
#endif
#if HAVE_F128
DEF_BUF_PEEK(f128)
#endif
DEF_BUF_PEEK(s8)
DEF_BUF_PEEK(s16)
DEF_BUF_PEEK(s32)
DEF_BUF_PEEK(s64)
DEF_BUF_PEEK(bool)

sw buf_peek_str (s_buf *buf, const s_str *src)
{
  sw r;
  sw size;
  assert(buf);
  assert(src);
  if (! src->size)
    return 0;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (buf->rpos > buf->wpos) {
    err_puts("buf_peek_str: buf error: rpos > wpos");
    assert(! "buf_peek_str: buf error: rpos > wpos");
    r = -1;
    goto clean;
  }
  if (buf->wpos > buf->size) {
    err_puts("buf_peek_str: buf error: wpos > size");
    assert(! "buf_peek_str: buf error: wpos > size");
    r = -1;
    goto clean;
  }
  size = buf->wpos - buf->rpos;
  if ((uw) size < src->size) {
    if (memcmp(buf->ptr.pchar + buf->rpos, src->ptr.p, size)) {
      r = 0;
      goto clean;
    }
  }
  if (buf->rpos + src->size > buf->wpos &&
      buf_refill(buf, src->size) < (sw) src->size) {
    r = 0;
    goto clean;
  }
  if (memcmp(buf->ptr.pchar + buf->rpos, src->ptr.p, src->size)) {
    r = 0;
    goto clean;
  }
  r = src->size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

DEF_BUF_PEEK(sw)

sw buf_peek_to_str (s_buf *buf, s_str *dest)
{
  sw r;
  sw size;
  assert(buf);
  assert(dest);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (buf->rpos > buf->wpos) {
    r = -1;
    goto clean;
  }
  if (buf->wpos > buf->size) {
    r = -1;
    goto clean;
  }
  size = buf->wpos - buf->rpos;
  if (size == 0) {
    str_init_empty(dest);
    r = 0;
    goto clean;
  }
  str_init_alloc_copy(dest, size, buf->ptr.pchar + buf->rpos);
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

DEF_BUF_PEEK(u8)
DEF_BUF_PEEK(u16)
DEF_BUF_PEEK(u32)
DEF_BUF_PEEK(u64)
DEF_BUF_PEEK(uw)

s_str * buf_read (s_buf *buf, uw size, s_str *dest)
{
  char *p;
  sw r;
  s_str *result = NULL;
  s_str tmp = {0};
  assert(buf);
  if (! size)
    return str_init_empty(dest);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (buf->rpos > buf->wpos)
    goto clean;
  if (buf->rpos + size > buf->wpos &&
      (r = buf_refill(buf, size)) < (sw) size)
    goto clean;
  if (buf->rpos + size > buf->wpos) {
    assert(! "buffer overflow");
    goto clean;
  }
  p = alloc(size + 1);
  if (! p)
    goto clean;
  str_init(&tmp, p, size, p);
  memcpy(p, buf->ptr.ps8 + buf->rpos, size);
  buf->rpos += size;
  *dest = tmp;
  result = dest;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return result;
}

sw buf_read_1 (s_buf *buf, const char *p)
{
  s_str str;
  assert(buf);
  str_init_1(&str, NULL, p);
  return buf_read_str(buf, &str);
}

DEF_BUF_READ(bool)

sw buf_read_character_utf8 (s_buf *buf, character *p)
{
  sw r;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  r = buf_peek_character_utf8(buf, p);
  if (r > 0) {
    if (buf->line >= 0) {
      if (*p == '\n') {
        buf->pretty.column = 0;
        buf->line++;
      }
      else
        buf->pretty.column++;
    }
    buf->rpos += r;
  }
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

DEF_BUF_READ(f32)
DEF_BUF_READ(f64)
#if HAVE_F80
DEF_BUF_READ(f80)
#endif
#if HAVE_F128
DEF_BUF_READ(f128)
#endif

sw buf_read_integer (s_buf *buf, s_integer *dest)
{
  const sw digit_size = 4;
  const sw nail_bits = 4;
  u32 count;
  sw r;
  sw result = 0;
  u8 sign;
  sw size;
  s_integer tmp;
  assert(buf);
  assert(dest);
  if ((r = buf_read_u8(buf, &sign)) <= 0)
    return r;
  result += r;
  if ((r = buf_read_u32(buf, &count)) <= 0)
    return r;
  result += r;
  integer_init(&tmp);
  if (count == 0) {
    mp_set_u32(&tmp.mp_int, 0);
    *dest = tmp;
    return result;
  }
  size = count * digit_size;
  if (buf->rpos + size > buf->wpos) {
    if ((r = buf_refill(buf, size)) < 0) {
      integer_clean(&tmp);
      return r;
    }
    if (buf->rpos + size > buf->wpos) {
      err_puts("buf_read_integer: short read");
      assert(! "buf_read_integer: short read");
      integer_clean(&tmp);
      return -1;
    }
  }
  if (mp_unpack(&tmp.mp_int, count, MP_LSB_FIRST, digit_size,
                MP_LITTLE_ENDIAN, nail_bits,
                buf->ptr.pu8 + buf->rpos) != MP_OKAY) {
    err_puts("buf_read_integer: mp_unpack failed");
    assert(! "buf_read_integer: mp_unpack failed");
    integer_clean(&tmp);
    return -1;
  }
  if (sign) {
    if (mp_neg(&tmp.mp_int, &tmp.mp_int) != MP_OKAY) {
      err_puts("buf_read_integer: mp_neg failed");
      assert(! "buf_read_integer: mp_neg failed");
      integer_clean(&tmp);
      return -1;
    }
  }
  buf->rpos += size;
  result += size;
  *dest = tmp;
  return result;
}

sw buf_read_line (s_buf *buf, s_str *dest)
{
  return buf_read_until_1_into_str(buf, "\n", dest);
}

s_str * buf_read_max (s_buf *buf, s_str *dest)
{
  char *p;
  sw r;
  s_str *result = NULL;
  sw size;
  s_str tmp = {0};
  assert(buf);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (buf->rpos > buf->wpos)
    goto clean;
  if ((r = buf_refill(buf, buf->size)) < 0)
    goto clean;
  size = r;
  if (! size)
    return str_init_empty(dest);
  if (buf->rpos + size > buf->wpos) {
    assert(! "buffer overflow");
    goto clean;
  }
  p = alloc(size + 1);
  if (! p)
    goto clean;
  str_init(&tmp, p, size, p);
  memcpy(p, buf->ptr.ps8 + buf->rpos, size);
  buf->rpos += size;
  *dest = tmp;
  result = dest;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return result;
}

DEF_BUF_READ(s8)
DEF_BUF_READ(s16)
DEF_BUF_READ(s32)
DEF_BUF_READ(s64)
DEF_BUF_READ(sw)

sw buf_read_str (s_buf *buf, const s_str *src)
{
  sw r;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  r = buf_peek_str(buf, src);
  if (r > 0)
    r = buf_ignore(buf, r);
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_read_sym (s_buf *buf, const s_sym *src)
{
  character c;
  sw r;
  s_buf_save save;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
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
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_read_to_str (s_buf *buf, s_str *dest)
{
  sw r;
  sw size;
  s_str tmp = {0};
  assert(buf);
  assert(dest);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (buf->rpos > buf->wpos) {
    err_puts("buf_read_to_str: buf->rpos > buf->wpos");
    assert(! "buf_read_to_str: buf->rpos > buf->wpos");
    r = -1;
    goto clean;
  }
  if (buf->wpos > buf->size) {
    err_puts("buf_read_to_str: buf->wpos > buf->size");
    assert(! "buf_read_to_str: buf->wpos > buf->size");
    r = -1;
    goto clean;
  }
  size = buf->wpos - buf->rpos;
  if (! size) {
    str_init_empty(dest);
    r = 0;
    goto clean;
  }
  if (! str_init_alloc_copy(&tmp, size, buf->ptr.pchar + buf->rpos)) {
    err_puts("buf_read_to_str: str_init_alloc_copy");
    assert(! "buf_read_to_str: str_init_alloc_copy");
    r = -1;
    goto clean;
  }
  if ((r = buf_ignore(buf, size)) < 0) {
    err_puts("buf_read_to_str: buf_ignore");
    assert(! "buf_read_to_str: buf_ignore");
    str_clean(&tmp);
    goto clean;
  }
  *dest = tmp;
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_read_to_sym (s_buf *buf, p_sym *dest)
{
  sw r;
  s_str str = {0};
  p_sym sym;
  if ((r = buf_read_to_str(buf, &str)) > 0) {
    sym = str_to_sym(&str);
    str_clean(&str);
    *dest = sym;
  }
  return r;
}

DEF_BUF_READ(u8)
DEF_BUF_READ(u16)
DEF_BUF_READ(u32)
DEF_BUF_READ(u64)

sw buf_read_until_1_into_str (s_buf *buf, const char *end,
                              s_str *dest)
{
  s_str str;
  str_init_1(&str, NULL, end);
  return buf_read_until_str_into_str(buf, &str, dest);
}

sw buf_read_until_character_into_str (s_buf *buf, character end,
                                           s_str *dest)
{
  s_str end_str;
  sw r;
  if (! str_init_character(&end_str, end))
    return -1;
  r = buf_read_until_str_into_str(buf, &end_str, dest);
  str_clean(&end_str);
  return r;
}

sw buf_read_until_space_into_str (s_buf *buf, s_str *dest)
{
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf tmp;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  buf_save_init(buf, &save);
  while (1) {
    if ((r = buf_peek_character_utf8(buf, &c)) < 0)
      goto restore;
    if (r && character_is_space(c)) {
      buf_init(&tmp, false, buf->size, buf->ptr.pchar);
      tmp.rpos = save.rpos;
      tmp.wpos = buf->rpos;
      if (buf_read_to_str(&tmp, dest) <= 0) {
	buf_clean(&tmp);
        goto restore;
      }
      buf_clean(&tmp);
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
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_read_until_list_into_str (s_buf *buf,
                                 p_list *end,
                                 s_str *dest)
{
  const s_list *e;
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf tmp;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  buf_save_init(buf, &save);
  while (1) {
    e = *end;
    if (e->tag.type != TAG_STR || ! e->tag.data.str.size) {
      err_puts("buf_read_until_list_into_str: invalid end List of Str");
      assert(! "buf_read_until_list_into_str: invalid end List of Str");
    }
    while (e) {
      if ((r = buf_read_str(buf, &e->tag.data.str)) < 0) {
        if (false)
          err_puts("buf_read_until_list_into_str: buf_read_str");
        goto restore;
      }
      if (r) {
        buf_init(&tmp, false, buf->size, buf->ptr.pchar);
        tmp.rpos = save.rpos;
        tmp.wpos = buf->rpos - e->tag.data.str.size;
        if (buf_read_to_str(&tmp, dest) <= 0) {
          err_puts("buf_read_until_list_into_str: buf_read_to_str");
          goto restore;
        }
        r = result;
        goto clean;
      }
      e = list_next(e);
    }
    if ((r = buf_read_character_utf8(buf, &c)) <= 0) {
      if (false)
        err_puts("buf_read_until_list_into_str: "
                 "buf_read_character_utf8");
      goto restore;
    }
  }
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return result;
}

sw buf_read_until_str_into_buf (s_buf *buf, const s_str *end,
				s_buf *dest)
{
  u8 c;
  sw r;
  sw result = 0;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
  rwlock_w(dest->rwlock);
#endif
  while (1) {
    if ((r = buf_read_str(buf, end)) < 0) {
      if (true)
        err_puts("buf_read_until_str_into_buf: buf_read_str");
      goto clean;
    }
    if (r) {
      result += r;
      r = result;
      goto clean;
    }
    if ((r = buf_read_u8(buf, &c)) <= 0) {
      if (true) {
        err_puts("buf_read_until_str_into_buf: buf_read_u8");
        err_inspect_buf(buf);
      }
      goto clean;
    }
    if ((r = buf_write_u8(dest, c)) <= 0) {
      if (true)
        err_puts("buf_read_until_str_into_buf: buf_write_u8");
      goto clean;
    }
    result++;
  }
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
  rwlock_unlock_w(dest->rwlock);
#endif
  return r;
}

sw buf_read_until_str_into_file (s_buf *buf, const s_str *end,
                                 s_str *path)
{
  t_fd fd;
  s_buf dest = {0};
  sw r;
  if (! file_open_w(path, &fd)) {
    err_puts("buf_read_until_str_into_file: file_open_w");
    return -1;
  }
  if (! buf_init_alloc(&dest, end->size + 1)) {
    err_puts("buf_read_until_str_into_file: buf_init_alloc");
    close(fd);
    return -1;
  }
  if (! buf_fd_open_w(&dest, fd)) {
    err_puts("buf_read_until_str_into_file: buf_fd_open_w");
    buf_clean(&dest);
    close(fd);
    return -1;
  }
  if ((r = buf_read_until_str_into_buf(buf, end, &dest)) < 0) {
    err_puts("buf_read_until_str_into_file: "
             "buf_read_until_str_into_buf");
    buf_fd_close(&dest);
    buf_clean(&dest);
    close(fd);
    return -1;
  }
  buf_fd_close(&dest);
  buf_clean(&dest);
  close(fd);
  return r;
}

sw buf_read_until_str_into_str (s_buf *buf, const s_str *end,
                                s_str *dest)
{
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf tmp;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  buf_save_init(buf, &save);
  while (1) {
    if ((r = buf_read_str(buf, end)) < 0) {
      if (false)
        err_puts("buf_read_until_str_into_str: buf_read_str");
      goto restore;
    }
    if (r) {
      result += r;
      buf_init(&tmp, false, buf->size, buf->ptr.pchar);
      tmp.rpos = save.rpos;
      tmp.wpos = buf->rpos - end->size;
      if (buf_read_to_str(&tmp, dest) < 0) {
        err_puts("buf_read_until_str_into_str: buf_read_to_str");
        r = -1;
	buf_clean(&tmp);
        goto restore;
      }
      r = result;
      buf_clean(&tmp);
      goto clean;
    }
    if ((r = buf_read_character_utf8(buf, &c)) <= 0) {
      if (false)
        err_puts("buf_read_until_str_into_str: "
                 "buf_read_character_utf8");
      goto restore;
    }
    result += r;
  }
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_read_word_into_str(s_buf *buf, s_str *dest)
{
  character c;
  sw r;
  sw r1 = 0;
  sw result = 0;
  s_buf_save save;
  s_buf tmp;
  buf_save_init(buf, &save);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         ! character_is_alphanum(c)) {
    buf_ignore(buf, r);
    result += r;
  }
  while (1) {
    if ((r = buf_read_character_utf8(buf, &c)) < 0) {
      if (false)
        err_puts("buf_read_word_into_str: buf_read_character_utf8");
      goto restore;
    }
    result += r;
    if (r && ! character_is_alphanum(c)) {
      r1 = r;
      while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
             ! character_is_alphanum(c)) {
        buf_ignore(buf, r);
        result += r;
        r1 += r;
      }
      buf_init(&tmp, false, buf->size, buf->ptr.pchar);
      tmp.rpos = save.rpos;
      tmp.wpos = buf->rpos - r1;
      if ((r = buf_read_to_str(&tmp, dest)) <= 0) {
        err_puts("buf_read_word_into_str: buf_read_to_str");
	buf_clean(&tmp);
        goto restore;
      }
      buf_clean(&tmp);
      r = result;
      goto clean;
    }
  }
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

DEF_BUF_READ(uw)

sw buf_refill (s_buf *buf, sw size)
{
  sw r;
  assert(buf);
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  r = buf->wpos - buf->rpos;
  if (size < 0) {
    err_puts("buf_refill: size < 0");
    assert(! "buf_refill: size < 0");
    r = -1;
    goto clean;
  }
  if (buf->read_only ||
      ! size)
    goto clean;
  if (buf->rpos + size > buf->wpos) {
    if ((r = buf_refill_compact(buf)) < 0)
      goto clean;
    if (buf->refill)
      while (1) {
        if ((r = buf->refill(buf)) < 0)
          goto clean;
        if (! r) {
          r = -1;
          goto clean;
        }
        if (buf->wpos - buf->rpos >= (uw) size)
          break;
      }
  }
  r = buf->wpos - buf->rpos;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_refill_compact (s_buf *buf)
{
  uw min_rpos;
  sw r;
  s_buf_save *save;
  uw size;
  assert(buf);
  if (buf->read_only)
    return 0;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  min_rpos = buf_save_min_rpos(buf);
  if (min_rpos > buf->wpos) {
    assert(min_rpos <= buf->wpos);
    r = -1;
    goto clean;
  }
  if (buf->wpos > buf->size) {
    assert(buf->wpos <= buf->size);
    r = -1;
    goto clean;
  }
  if (min_rpos > 0) {
    if (min_rpos == buf->wpos) {
      buf->rpos = 0;
      buf->wpos = 0;
      save = buf->save;
      while (save) {
        save->rpos -= min_rpos;
        save->wpos -= min_rpos;
        save = save->next;
      }
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
  }
  r = 1;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

s64 buf_seek (s_buf *buf, s64 position, s8 from)
{
  if (buf->seek)
    return buf->seek(buf, position, from);
  switch (from) {
  case SEEK_CUR:
    return buf_seek_set(buf, buf->rpos + position);
  case SEEK_END:
    return buf_seek_set(buf, buf->size + position);
  case SEEK_SET:
    return buf_seek_set(buf, position);
  }
  err_puts("buf_seek: invalid 'from' argument");
  assert(! "buf_seek: invalid 'from' argument");
  return -1;
}

s64 buf_seek_set (s_buf *buf, s64 position)
{
  if (position < 0) {
    err_puts("buf_seek_set: negative position");
    assert(! "buf_seek_set: negative position");
    return -1;
  }
  if (buf->wpos > buf->size) {
    err_puts("buf_seek_set: buf wpos > size");
    assert(! "buf_seek_set: buf wpos > size");
    return -1;
  }
  if ((uw) position > buf->wpos) {
    err_puts("buf_seek_set: position > buf wpos");
    assert(! "buf_seek_set: position > buf wpos");
    return -1;
  }
  buf->rpos = position;
  return position;
}

s_str * buf_slice_to_str (s_buf *buf, uw start, uw end,
                          s_str *dest)
{
  s_str *result = NULL;
  s_str tmp;
  assert(buf);
  assert(dest);
#if HAVE_PTHREAD
  rwlock_r(buf->rwlock);
#endif
  if (start > buf->wpos) {
    err_puts("buf_slice_to_str: start > wpos");
    assert(! "buf_slice_to_str: start > wpos");
    goto clean;
  }
  if (end < start) {
    err_puts("buf_slice_to_str: end < start");
    assert(! "buf_slice_to_str: end < start");
    goto clean;
  }
  if (end > buf->wpos) {
    err_puts("buf_slice_to_str: end > wpos");
    assert(! "buf_slice_to_str: end > wpos");
    goto clean;
  }
  str_init(&tmp, NULL, end - start, buf->ptr.pchar + start);
  result = str_init_copy(dest, &tmp);
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_r(buf->rwlock);
#endif
  return result;
}

sw buf_str_to_hex (s_buf *buf, const s_str *src)
{
  const u8 *b;
  sw r;
  sw size;
  uw i;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (src->size == 0) {
    r = 0;
    goto clean;
  }
  size = src->size * 2;
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    r = -1;
    goto clean;
  }
  b = src->ptr.pu8;
  i = 0;
  while (i++ < src->size)
    if ((r = buf_u8_to_hex(buf, b++)) <= 0)
      goto clean;
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

u64 * buf_tell_r (s_buf *buf, u64 *dest)
{
  if (buf->tell)
    return buf->tell(buf, dest);
  *dest = buf->rpos;
  return dest;
}

u64 * buf_tell_w (s_buf *buf, u64 *dest)
{
  if (buf->tell)
    return buf->tell(buf, dest);
  *dest = buf->wpos;
  return dest;
}

s_str * buf_to_str (s_buf *buf, s_str *str)
{
  void *p_free;
  s_str *result = NULL;
  assert(buf);
  assert(str);
  p_free = buf->free ? buf->ptr.p : NULL;
  result = str_init(str, p_free, buf->size, buf->ptr.p);
  buf->free = false;
  buf_clean(buf);
  return result;
}

sw buf_u8_to_hex (s_buf *buf, const u8 *x)
{
  u8 digit;
  sw r;
  sw result = 0;
  s_buf_save save;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  buf_save_init(buf, &save);
  digit = *x >> 4;
  if (digit < 10)
    r = buf_write_character_utf8(buf, digit + '0');
  else
    r = buf_write_character_utf8(buf, digit - 10 + 'A');
  if (r <= 0)
    goto clean;
  result += r;
  digit = *x & 0xF;
  if (digit < 10)
    r = buf_write_character_utf8(buf, digit + '0');
  else
    r = buf_write_character_utf8(buf, digit - 10 + 'A');
  if (r <= 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_u8_to_hex_size (const u8 *u)
{
  (void) u;
  return 2;
}

u64 * buf_total_size (s_buf *buf, u64 *dest)
{
  assert(buf);
  assert(dest);
  if (buf->total_size)
    return buf->total_size(buf, dest);
  *dest = buf->wpos;
  return dest;
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
  sw r;
  assert(buf);
  if (! len)
    return 0;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (buf->wpos > buf->size) {
    err_puts("buf_write: buf error: wpos > size");
    assert(! "buf_write: buf error: wpos > size");
    r = -1;
    goto clean;
  }
  if (buf->wpos + len > buf->size &&
      (r = buf_flush(buf)) < (sw) len) {
    err_puts("buf_write: buffer is too small");
    assert(! "buf_write: buffer is too small");
    r = -1;
    goto clean;
  }
  if (buf->wpos + len > buf->size) {
    err_puts("buf_write: buffer overflow");
    assert(! "buf_write: buffer overflow");
    r = -1;
    goto clean;
  }
  memcpy(buf->ptr.ps8 + buf->wpos, data, len);
  buf->wpos += len;
  buf_flush(buf);
  r = len;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_write_1 (s_buf *buf, const char *p)
{
  s_str str;
  if (buf && p) {
    str_init_1(&str, NULL, p);
    return buf_write_str(buf, &str);
  }
  return -1;
}

sw buf_write_1_size (s_pretty *pretty, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return buf_write_str_size(pretty, &str);
}

DEF_BUF_WRITE(bool)

sw buf_write_character_utf8 (s_buf *buf, character c)
{
  sw csize;
  sw r;
  sw size;
  csize = character_utf8_size(c);
  if (csize <= 0) {
    err_puts("buf_write_character_utf8: character_utf8_size");
    assert(! "buf_write_character_utf8: character_utf8_size");
    return csize;
  }
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  if (c == '\n')
    size = csize + buf->pretty.base_column;
  else
    size = csize;
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size) {
    r = -1;
    goto clean;
  }
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    r = -1;
    goto clean;
  }
  character_utf8(c, buf->ptr.pchar + buf->wpos);
  if (c == '\n') {
    memset(buf->ptr.pchar + buf->wpos + csize, ' ',
           buf->pretty.base_column);
    buf->pretty.column = buf->pretty.base_column;
  }
  else
    buf->pretty.column++;
  buf->wpos += size;
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_write_character_utf8_size (s_pretty *pretty, character c)
{
  sw csize;
  sw size;
  csize = character_utf8_size(c);
  if (csize <= 0)
    return csize;
  if (c == '\n') {
    size = csize + pretty->base_column;
    pretty->column = pretty->base_column;
  }
  else {
    size = csize;
    pretty->column++;
  }
  return size;
}

DEF_BUF_WRITE(f32)
DEF_BUF_WRITE(f64)
#if HAVE_F80
DEF_BUF_WRITE(f80)
#endif
#if HAVE_F128
DEF_BUF_WRITE(f128)
#endif
DEF_BUF_WRITE(s8)
DEF_BUF_WRITE(s16)
DEF_BUF_WRITE(s32)
DEF_BUF_WRITE(s64)

sw buf_write_str (s_buf *buf, const s_str *src)
{
  u8 byte;
  character c;
  sw r;
  sw result = 0;
  s_str s;
  assert(buf);
  assert(src);
  s = *src;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  while (1) {
    if ((r = str_read_character_utf8(&s, &c)) > 0) {
      if ((r = buf_write_character_utf8(buf, c)) < 0)
        goto clean;
      result += r;
    }
    else if ((r = str_read_u8(&s, &byte)) > 0) {
      if ((r = buf_write_u8(buf, byte)) < 0)
        goto clean;
      result += r;
    }
    else
      break;
  }
  buf_flush(buf);
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_write_str_size (s_pretty *pretty, const s_str *src)
{
  u8 byte;
  character c;
  sw r;
  sw result = 0;
  s_str s;
  assert(pretty);
  assert(src);
  s = *src;
  while (1) {
    if ((r = str_read_character_utf8(&s, &c)) > 0) {
      if ((r = buf_write_character_utf8_size(pretty, c)) < 0)
        goto clean;
      result += r;
    }
    else if ((r = str_read_u8(&s, &byte)) > 0)
      result += r;
    else
      break;
  }
  r = result;
 clean:
  return r;
}

sw buf_write_str_without_indent (s_buf *buf, const s_str *src)
{
  s_pretty_save pretty_save;
  sw r;
#if HAVE_PTHREAD
  rwlock_w(buf->rwlock);
#endif
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_at_column(&buf->pretty, 0);
  r = buf_write_str(buf, src);
  pretty_save_clean(&pretty_save, &buf->pretty);
#if HAVE_PTHREAD
  rwlock_unlock_w(buf->rwlock);
#endif
  return r;
}

sw buf_write_str_without_indent_size (s_pretty *pretty,
                                      const s_str *src)
{
  s_pretty_save pretty_save;
  sw r;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_at_column(pretty, 0);
  r = buf_write_str_size(pretty, src);
  pretty_save_clean(&pretty_save, pretty);
  return r;
}

DEF_BUF_WRITE(sw)
DEF_BUF_WRITE(u8)
DEF_BUF_WRITE(u16)
DEF_BUF_WRITE(u32)
DEF_BUF_WRITE(u64)
DEF_BUF_WRITE(uw)

sw buf_write_integer (s_buf *buf, const s_integer *src)
{
  const sw digit_size = 4;
  const sw nail_bits = 4;
  uw count;
  sw r;
  sw result = 0;
  u8 sign;
  sw size;
  uw written;
  assert(buf);
  assert(src);
  count = mp_pack_count(&src->mp_int, nail_bits, digit_size);
  if (count > U32_MAX)
    ERROR("buf_write_integer: count is too large");
  size = count * digit_size;
  sign = mp_isneg(&src->mp_int) ? 1 : 0;
  if ((r = buf_write_u8(buf, sign)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_u32(buf, count)) <= 0)
    return r;
  result += r;
  if (count == 0)
    return result;
  if (buf->wpos + size > buf->size &&
      buf_flush(buf) < size) {
    return -1;
  }
  if (buf->wpos + size > buf->size) {
    err_puts("buf_write_integer: buffer overflow");
    assert(! "buf_write_integer: buffer overflow");
    return -1;
  }
  if (mp_pack(buf->ptr.pu8 + buf->wpos, count, &written,
              MP_LSB_FIRST, digit_size, MP_LITTLE_ENDIAN, nail_bits,
              &src->mp_int) != MP_OKAY) {
    err_puts("buf_write_integer: mp_pack failed");
    assert(! "buf_write_integer: mp_pack failed");
    return -1;
  }
  if (written != count) {
    err_puts("buf_write_integer: mp_pack count mismatch");
    assert(! "buf_write_integer: mp_pack count mismatch");
    return -1;
  }
  buf->wpos += size;
  result += size;
  return result;
}

sw buf_xfer (s_buf *dest, s_buf *src, uw size)
{
  sw r = 0;
  assert(dest);
  assert(src);
  if (size == 0)
    return 0;
#if HAVE_PTHREAD
  rwlock_w(dest->rwlock);
  rwlock_w(src->rwlock);
#endif
  assert(src->rpos <= src->wpos);
  assert(dest->rpos <= dest->wpos);
  if (src->rpos + size > src->wpos &&
      (r = buf_refill(src, size)) < (sw) size) {
    if (r < 0)
      goto clean;
    r = 0;
    goto clean;
  }
  if (dest->wpos + size > dest->size &&
      (r = buf_flush(dest)) < (sw) size) {
    if (r < 0)
      goto clean;
    r = -1;
    goto clean;
  }
  memcpy(dest->ptr.ps8 + dest->wpos, src->ptr.ps8 + src->rpos, size);
  src->rpos += size;
  dest->wpos += size;
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(src->rwlock);
  rwlock_unlock_w(dest->rwlock);
#endif
  return r;
}

sw buf_xfer_reverse (s_buf *src, s_buf *dest)
{
  sw r;
  sw size;
  assert(src);
  assert(dest);
#if HAVE_PTHREAD
  rwlock_w(src->rwlock);
  rwlock_w(dest->rwlock);
#endif
  size = src->wpos - src->rpos;
  if (dest->wpos + size > dest->size &&
      buf_flush(dest) < size) {
    r = -1;
    goto clean;
  }
  for (sw i = 0; i < size; i++)
    dest->ptr.ps8[dest->wpos + i] = src->ptr.ps8[src->wpos - 1 - i];
  src->rpos += size;
  dest->wpos += size;
  r = size;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(dest->rwlock);
  rwlock_unlock_w(src->rwlock);
#endif
  return r;
}
