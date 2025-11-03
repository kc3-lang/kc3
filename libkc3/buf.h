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
/**
 * @file buf.h
 * @brief Byte buffer.
 *
 * Structure to manipulate byte buffers.
 */
#ifndef LIBKC3_BUF_H
#define LIBKC3_BUF_H

#include <stdarg.h>
#include <stdlib.h>
#include "types.h"

#define BUF_SIZE 65536

#define PROTO_BUF_PEEK(type)                                           \
  sw buf_peek_ ## type (s_buf *buf, type *dest)

#define PROTO_BUF_READ(type)                                           \
  sw buf_read_ ## type (s_buf *buf, type *dest)

#define PROTO_BUF_WRITE(type)                                          \
  sw buf_write_ ## type (s_buf *buf, type src)

/* Stack-allocation compatible functions, call buf_clean after use. */
void    buf_clean (s_buf *buf);
s_buf * buf_init (s_buf *buf, bool p_free, uw size, char *p);
s_buf * buf_init_const (s_buf *buf, uw size, const char *p);
s_buf * buf_init_1 (s_buf *buf, bool p_free, char *p);
s_buf * buf_init_1_const (s_buf *buf, const char *p);
s_buf * buf_init_alloc (s_buf *buf, uw size);
s_buf * buf_init_str (s_buf *buf, bool free, s_str *p);
s_buf * buf_init_str_const (s_buf *buf, const s_str *p);
s_buf * buf_init_str_copy (s_buf *buf, const s_str *str);

/* Heap-allocation compatible functions, call buf_delete after use. */
void    buf_delete (s_buf *buf);
s_buf * buf_new (bool free, uw size, char *p);
s_buf * buf_new_1 (bool free, uw size, char *p);
s_buf * buf_new_alloc (uw bytes);
s_buf * buf_new_str (s_str *str);

/* Operators. */
void      buf_empty (s_buf *buf);
sw        buf_f (s_buf *buf, const char *fmt, ...);
sw        buf_flush (s_buf *buf);
sw        buf_ignore (s_buf *buf, uw size);
sw        buf_ignore_line (s_buf *buf);
sw        buf_ignore_newline (s_buf *buf);
sw        buf_ignore_spaces (s_buf *buf);
sw        buf_ignore_spaces_but_newline (s_buf *buf);
sw        buf_peek_1 (s_buf *buf, const char *p);
PROTO_BUF_PEEK(bool);
sw        buf_peek_character_utf8 (s_buf *buf, character *p);
PROTO_BUF_PEEK(f32);
PROTO_BUF_PEEK(f64);
#if HAVE_F80
PROTO_BUF_PEEK(f80);
#endif
#if HAVE_FLOAT128
PROTO_BUF_PEEK(f128);
#endif
sw        buf_peek_next_character_utf8 (s_buf *buf, character *p);
PROTO_BUF_PEEK(s8);
PROTO_BUF_PEEK(s16);
PROTO_BUF_PEEK(s32);
PROTO_BUF_PEEK(s64);
PROTO_BUF_PEEK(sw);
sw        buf_peek_str (s_buf *buf, const s_str *src);
sw        buf_peek_to_str (s_buf *buf, s_str *dest);
PROTO_BUF_PEEK(u8);
PROTO_BUF_PEEK(u16);
PROTO_BUF_PEEK(u32);
PROTO_BUF_PEEK(u64);
PROTO_BUF_PEEK(uw);
sw        buf_read_character_utf8 (s_buf *buf, character *p);
s_str *   buf_read (s_buf *buf, uw size, s_str *dest);
sw        buf_read_1 (s_buf *buf, const char *p);
PROTO_BUF_READ(bool);
PROTO_BUF_READ(f32);
PROTO_BUF_READ(f64);
#if HAVE_F80
PROTO_BUF_READ(f80);
#endif
#if HAVE_FLOAT128
PROTO_BUF_READ(f128);
#endif
sw        buf_read_integer (s_buf *buf, s_integer *dest);
sw        buf_read_line (s_buf *buf, s_str *dest);
s_str *   buf_read_max (s_buf *buf, s_str *dest);
PROTO_BUF_READ(s8);
PROTO_BUF_READ(s16);
PROTO_BUF_READ(s32);
PROTO_BUF_READ(s64);
sw        buf_read_str (s_buf *buf, const s_str *src);
PROTO_BUF_READ(sw);
sw        buf_read_sym (s_buf *buf, const s_sym *src);
sw        buf_read_to_str (s_buf *buf, s_str *dest);
sw        buf_read_to_sym (s_buf *buf, const s_sym **dest);
sw        buf_read_word_into_str(s_buf *buf, s_str *dest); 
PROTO_BUF_READ(u8);
PROTO_BUF_READ(u16);
PROTO_BUF_READ(u32);
PROTO_BUF_READ(u64);
sw        buf_read_until_1_into_str(s_buf *buf, const char *end,
                                    s_str *dest);
sw        buf_read_until_character_into_str (s_buf *buf, character end,
                                             s_str *dest);
sw        buf_read_until_list_into_buf (s_buf *buf, const s_list *end,
                                        s_buf *dest);
sw        buf_read_until_list_into_str (s_buf *buf,
                                        p_list *end,
                                        s_str *dest);

sw        buf_read_until_space_into_str (s_buf *buf, s_str *dest);
sw        buf_read_until_str_into_buf (s_buf *buf, const s_str *end,
                                       s_buf *dest);
sw        buf_read_until_str_into_file (s_buf *buf, const s_str *end,
                                        s_str *path);
sw        buf_read_until_str_into_str (s_buf *buf, const s_str *end,
                                       s_str *dest);
PROTO_BUF_READ(uw);
sw        buf_refill (s_buf *buf, sw size);
sw        buf_refill_compact (s_buf *buf);
s_buf *   buf_restore (s_buf *buf, const s_buf *save);
s64       buf_seek (s_buf *buf, s64 position, s8 from);
s64       buf_seek_set (s_buf *buf, s64 position);
s_str *   buf_slice_to_str (s_buf *buf, uw start, uw end,
                            s_str *dest);
sw        buf_str_to_hex (s_buf *buf, const s_str *src);
sw        buf_str_to_hex_size (const s_str *src);
u64 *     buf_tell_r (s_buf *buf, u64 *position);
u64 *     buf_tell_w (s_buf *buf, u64 *position);
s_str *   buf_to_str (s_buf *buf, s_str *dest);
s_ratio * buf_to_ratio (s_buf *buf, s_ratio *ratio);
u64 *     buf_total_size (s_buf *buf, u64 *dest);
sw        buf_u8_to_hex (s_buf *buf, const u8 *x);
sw        buf_u8_to_hex_size (const u8 *x);
sw        buf_vf (s_buf *buf, const char *fmt, va_list ap);
sw        buf_write (s_buf *buf, const void *data, uw len);
sw        buf_write_1 (s_buf *buf, const char *p);
sw        buf_write_1_size (s_pretty *pretty, const char *p);
PROTO_BUF_WRITE(bool);
sw        buf_write_character_utf8 (s_buf *buf, character c);
sw        buf_write_character_utf8_size (s_pretty *pretty, character c);
PROTO_BUF_WRITE(f32);
PROTO_BUF_WRITE(f64);
#if HAVE_F80
PROTO_BUF_WRITE(f80);
#endif
#if HAVE_FLOAT128
PROTO_BUF_WRITE(f128);
#endif
sw        buf_write_integer (s_buf *buf, const s_integer *src);
PROTO_BUF_WRITE(s8);
PROTO_BUF_WRITE(s16);
PROTO_BUF_WRITE(s32);
PROTO_BUF_WRITE(s64);
sw        buf_write_str (s_buf *buf, const s_str *src);
sw        buf_write_str_size (s_pretty *pretty, const s_str *src);
sw        buf_write_str_without_indent (s_buf *buf, const s_str *src);
sw        buf_write_str_without_indent_size (s_pretty *pretty,
                                             const s_str *src);
PROTO_BUF_WRITE(sw);
PROTO_BUF_WRITE(u8);
PROTO_BUF_WRITE(u16);
PROTO_BUF_WRITE(u32);
PROTO_BUF_WRITE(u64);
PROTO_BUF_WRITE(uw);
sw        buf_xfer (s_buf *buf, s_buf *src, uw size);
sw        buf_xfer_reverse(s_buf *src, s_buf *dest);

/* Observers. */

#endif /* LIBKC3_BUF_H */
