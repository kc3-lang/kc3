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
 * @file buf_readline.h
 * @brief Readline backend for s_buf.
 */
#ifndef BUF_READLINE_H
#define BUF_READLINE_H

#include "../libkc3/types.h"

void    buf_readline_close (s_buf *buf);
s_buf * buf_readline_open_r (s_buf *buf);

#endif /* BUF_READLINE_H */
