/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
 * @file buf_linenoise.h
 * @brief Linenoise backend for s_buf.
 */
#ifndef BUF_LINENOISE_H
#define BUF_LINENOISE_H

void    buf_linenoise_close (s_buf *buf);
s_buf * buf_linenoise_open_r (s_buf *buf, const char *prompt);

#endif /* BUF_LINENOISE_H */
