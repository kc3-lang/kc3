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
#include "base32.h"
#include "buf.h"
#include "buf_parse.h"
#include "io.h"
#include "kc3_main.h"
#include "str.h"

static const char *g_base64 =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static s8 base64_decode_char (u8 c)
{
  if (c >= 'A' && c <= 'Z')
    return c - 'A';
  if (c >= 'a' && c <= 'z')
    return c - 'a' + 26;
  if (c >= '0' && c <= '9')
    return c - '0' + 52;
  if (c == '+')
    return 62;
  if (c == '/')
    return 63;
  return -1;
}

s_str * base64_decode (s_str *str, s_str *dest)
{
  u8 buf[4];
  uw i;
  s_buf in;
  uw len;
  s_buf out;
  s8 r;
  s_str tmp;
  u8 c;
  if (! buf_init_alloc(&out, str->size * 3 / 4 + 3))
    return NULL;
  buf_init_const(&in, str->size, str->ptr.p_pchar);
  while (in.rpos < in.wpos) {
    len = 0;
    for (i = 0; i < 4; i++) {
      buf[i] = 0;
      if (in.rpos < in.wpos) {
        if (buf_read_u8(&in, &c) > 0) {
          r = base64_decode_char(c);
          if (r >= 0) {
            buf[i] = r;
            len++;
          }
        }
      }
    }
    if (len > 1)
      buf_write_u8(&out, (buf[0] << 2) | (buf[1] >> 4));
    if (len > 2)
      buf_write_u8(&out, (buf[1] << 4) | (buf[2] >> 2));
    if (len > 3)
      buf_write_u8(&out, (buf[2] << 6) | buf[3]);
  }
  if (buf_read_to_str(&out, &tmp) < 0) {
    buf_clean(&out);
    return NULL;
  }
  buf_clean(&out);
  *dest = tmp;
  return dest;
}

s_str * base64_encode (s_str *str, s_str *dest)
{
  u8 buf[3];
  char *d;
  uw dest_size;
  uw i;
  s_buf in;
  uw len;
  u32 n;
  s_str tmp;
  if (! str->size)
    return str_init_empty(dest);
  dest_size = ((str->size + 2) / 3) * 4;
  if (! str_init_alloc(&tmp, dest_size))
    return NULL;
  d = tmp.free.p_pchar;
  buf_init_const(&in, str->size, str->ptr.p_pchar);
  i = 0;
  while (in.rpos < in.wpos) {
    len = in.wpos - in.rpos;
    if (len > 3)
      len = 3;
    buf_read_u8(&in, &buf[0]);
    if (len > 1)
      buf_read_u8(&in, &buf[1]);
    else
      buf[1] = 0;
    if (len > 2)
      buf_read_u8(&in, &buf[2]);
    else
      buf[2] = 0;
    n = (buf[0] << 16) | (buf[1] << 8) | buf[2];
    d[i++] = g_base64[(n >> 18) & 63];
    d[i++] = g_base64[(n >> 12) & 63];
    if (len > 1)
      d[i++] = g_base64[(n >> 6) & 63];
    else
      d[i++] = '=';
    if (len > 2)
      d[i++] = g_base64[n & 63];
    else
      d[i++] = '=';
  }
  d[i] = 0;
  *dest = tmp;
  return dest;
}
