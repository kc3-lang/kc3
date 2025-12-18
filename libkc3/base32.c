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
#include "assert.h"
#include "base32.h"
#include "buf.h"
#include "buf_parse.h"
#include "io.h"
#include "kc3_main.h"
#include "str.h"

s_str * base32_decode (s_str *str, s_str *dest)
{
  u8 buf[8];
  uw i;
  s_buf in;
  uw len;
  s_buf out;
  sw r;
  s_str tmp;
  if (! buf_init_alloc(&out, str->size * 5 / 8))
    return NULL;
  buf_init_const(&in, str->size, str->ptr.pchar);
  while (in.rpos < in.wpos) {
    len = 0;
    for (i = 0; i < 8; i++) {
      buf[i] = 0;
      if ((r = buf_parse_digit(&in, &g_kc3_base32, &buf[i])) > 0)
        len++;
      else
        buf_read_u8(&in, &buf[i]);
    }
    if (len > 1)
      buf_write_u8(&out, (buf[0] << 3) | (buf[1] >> 2));
    if (len > 3)
      buf_write_u8(&out, (buf[1] << 6) | (buf[2] << 1) | (buf[3] >> 4));
    if (len > 4)
      buf_write_u8(&out, (buf[3] << 4) | (buf[4] >> 1));
    if (len > 6)
      buf_write_u8(&out, (buf[4] << 7) | (buf[5] << 2) | (buf[6] >> 3));
    if (len > 7)
      buf_write_u8(&out, (buf[6] << 5) | buf[7]);
  }
  buf_clean(&in);
  if (buf_read_to_str(&out, &tmp) < 0) {
    buf_clean(&out);
    return NULL;
  }
  buf_clean(&out);
  *dest = tmp;
  return dest;
}

s_str * base32_encode (s_str *str, s_str *dest)
{
  u8 buf[5];
  u8 digit;
  uw i;
  s_buf in;
  uw len;
  s_buf out;
  s_str tmp;
  if (! buf_init_alloc(&out, (str->size + 4) / 5 * 8))
    return NULL;
  buf_init_const(&in, str->size, str->ptr.pchar);
  while (in.rpos < in.wpos) {
    len = in.wpos - in.rpos;
    if (len > 5)
      len = 5;
    i = 0;
    while (i < len) {
      buf_read_u8(&in, buf + i);
      i++;
    }
    while (i < 5) {
      buf[i] = 0;
      i++;
    }
    digit = (buf[0] >> 3) & 0x1f;
    buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
    digit = ((buf[0] << 2) | (buf[1] >> 6)) & 0x1f;
    buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
    if (len > 1) {
      digit = (buf[1] >> 1) & 0x1f;
      buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
      digit = ((buf[1] << 4) | (buf[2] >> 4)) & 0x1f;
      buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
    }
    else {
      buf_write_u8(&out, '=');
      buf_write_u8(&out, '=');
    }
    if (len > 2) {
      digit = ((buf[2] << 1) | (buf[3] >> 7)) & 0x1f;
      buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
    }
    else
      buf_write_u8(&out, '=');
    if (len > 3) {
      digit = (buf[3] >> 2) & 0x1f;
      buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
      digit = ((buf[3] << 3) | (buf[4] >> 5)) & 0x1f;
      buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
    }
    else {
      buf_write_u8(&out, '=');
      buf_write_u8(&out, '=');
    }
    if (len > 4) {
      digit = buf[4] & 0x1f;
      buf_write_u8(&out, g_kc3_base32.ptr.pchar[digit]);
    }
    else
      buf_write_u8(&out, '=');
  }
  buf_clean(&in);
  if (! buf_to_str(&out, &tmp)) {
    buf_clean(&in);
    buf_clean(&out);
    return NULL;
  }
  *dest = tmp;
  return dest;
}
