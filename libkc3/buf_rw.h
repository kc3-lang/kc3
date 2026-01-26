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
/**
 * @file buf_rw.h
 * @brief Read/write byte buffers.
 *
 * Structure to manipulate read/write byte buffers.
 */
#ifndef LIBKC3_BUF_RW_H
#define LIBKC3_BUF_RW_H

#include "types.h"

/* Stack-allocation compatible functions, call buf_rw_clean after
   use. */
void       buf_rw_clean (s_buf_rw *buf_rw);
s_buf_rw * buf_rw_init_alloc (s_buf_rw *buf_rw, uw size);

/* Operators. */
void       buf_rw_fd_close (s_buf_rw *buf_rw);
s_buf_rw * buf_rw_fd_open (s_buf_rw *buf_rw, s32 fd);

#endif /* LIBKC3_BUF_RW_H */
