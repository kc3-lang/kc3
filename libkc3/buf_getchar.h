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
 * @file buf_getchar.h
 * @brief getchar backend for s_buf.
 */
#ifndef LIBKC3_BUF_GETCHAR_H
#define LIBKC3_BUF_GETCHAR_H

/* Operators. */
void    buf_getchar_close (s_buf *buf);
s_buf * buf_getchar_open_r (s_buf *buf);

#endif /* LIBKC3_BUF_GETCHAR_H */
