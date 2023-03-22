/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
/**
 * @file buf.h
 * @brief Byte buffer.
 *
 * Structure to manipulate byte buffers.
 */
#ifndef BUF_H
#define BUF_H

#include <assert.h>
#include <err.h>
#include <stdarg.h>
#include <stdlib.h>
#include "types.h"

#define BUF_SIZE 10240

extern const sw buf_u8_to_hex_size;
extern const sw buf_inspect_str_byte_size;

/* Stack constructors, call buf_clean after use. */
s_buf * buf_init (s_buf *buf, bool free, uw size, s8 *p);
s_buf * buf_init_1 (s_buf *buf, const s8 *p);
s_buf * buf_init_alloc (s_buf *buf, uw size);
s_buf * buf_init_str (s_buf *buf, const s_str *src);

/* Constructors, call buf_delete after use. */
s_buf * buf_new (bool free, uw size, s8 *p);
s_buf * buf_new_alloc (uw bytes);
s_buf * buf_new_str (s_str *str);

/* Destructors */
void buf_clean (s_buf *buf);
void buf_delete (s_buf *buf);

/* Modifiers */
sw      buf_f (s_buf *buf, const char *fmt, ...);
sw      buf_flush (s_buf *buf);
sw      buf_ignore (s_buf *buf, uw size);
sw      buf_ignore_line (s_buf *buf);
sw      buf_ignore_newline (s_buf *buf);
sw      buf_ignore_spaces (s_buf *buf);
sw      buf_peek_1 (s_buf *buf, const s8 *p);
sw      buf_peek_character_utf8 (s_buf *buf, character *p);
sw      buf_peek_f32 (s_buf *buf, f32 *p);
sw      buf_peek_f64 (s_buf *buf, f64 *p);
sw      buf_peek_s8 (s_buf *buf, s8 *p);
sw      buf_peek_s16 (s_buf *buf, s16 *p);
sw      buf_peek_s32 (s_buf *buf, s32 *p);
sw      buf_peek_s64 (s_buf *buf, s64 *p);
sw      buf_peek_str (s_buf *buf, const s_str *src);
sw      buf_peek_u8 (s_buf *buf, u8 *p);
sw      buf_peek_u16 (s_buf *buf, u16 *p);
sw      buf_peek_u32 (s_buf *buf, u32 *p);
sw      buf_peek_u64 (s_buf *buf, u64 *p);
sw      buf_read_integer (s_buf *buf, s_integer *dst);
sw      buf_read_character_utf8 (s_buf *buf, character *p);
sw      buf_read_1 (s_buf *buf, const s8 *p);
sw      buf_read_f32 (s_buf *buf, f32 *p);
sw      buf_read_f64 (s_buf *buf, f64 *p);
sw      buf_read_s8 (s_buf *buf, s8 *p);
sw      buf_read_s16 (s_buf *buf, s16 *p);
sw      buf_read_s32 (s_buf *buf, s32 *p);
sw      buf_read_s64 (s_buf *buf, s64 *p);
sw      buf_read_str (s_buf *buf, const s_str *src);
sw      buf_read_to_str (s_buf *buf, s_str *dest);
sw      buf_read_u8 (s_buf *buf, u8 *p);
sw      buf_read_u16 (s_buf *buf, u16 *p);
sw      buf_read_u32 (s_buf *buf, u32 *p);
sw      buf_read_u64 (s_buf *buf, u64 *p);
sw      buf_read_integer (s_buf *buf, s_integer *dst);
sw      buf_refill (s_buf *buf, sw size);
sw      buf_refill_compact (s_buf *buf);
s_buf * buf_restore (s_buf *buf, const s_buf *save);
sw      buf_seek (s_buf *buf, sw offset, u8 whence);
sw      buf_str_to_hex (s_buf *buf, const s_str *src);
sw      buf_str_to_hex_size (const s_str *src);
sw      buf_u8_to_hex (s_buf *buf, u8 x);
sw      buf_vf (s_buf *buf, const char *fmt, va_list ap);
sw      buf_write_1 (s_buf *buf, const s8 *p);
sw      buf_write_character_utf8 (s_buf *buf, character c);
sw      buf_write_f32 (s_buf *buf, f32 x);
sw      buf_write_f64 (s_buf *buf, f64 x);
sw      buf_write_s8 (s_buf *buf, s8 i);
sw      buf_write_s16 (s_buf *buf, s16 i);
sw      buf_write_s32 (s_buf *buf, s32 i);
sw      buf_write_s64 (s_buf *buf, s64 i);
sw      buf_write_str (s_buf *buf, const s_str *src);
sw      buf_write_u8 (s_buf *buf, u8 i);
sw      buf_write_u16 (s_buf *buf, u16 i);
sw      buf_write_u32 (s_buf *buf, u32 i);
sw      buf_write_u64 (s_buf *buf, u64 i);
sw      buf_xfer (s_buf *buf, s_buf *src, uw size);
sw      buf_xfer_reverse(s_buf *src, s_buf *dest);

/* Observers. */
s_str * buf_to_str (const s_buf *buf, s_str *dest);

#endif /* BUF_H */
