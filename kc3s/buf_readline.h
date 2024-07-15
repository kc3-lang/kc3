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
/**
 * @file buf_readline.h
 * @brief Readline backend for s_buf.
 */
#ifndef BUF_READLINE_H
#define BUF_READLINE_H

void    buf_readline_close (s_buf *buf);
s_buf * buf_readline_open_r (s_buf *buf, const char *prompt);

#endif /* BUF_READLINE_H */
