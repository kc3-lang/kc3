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
 * @file buf_file.h
 * @brief s_buf interface to UNIX file descriptors.
 */
#ifndef LIBKC3_BUF_FD_H
#define LIBKC3_BUF_FD_H

#include <stdio.h>
#include "types.h"

/* Operators. */
s_buf * buf_fd_open_r (s_buf *buf, s64 fd);
s_buf * buf_fd_open_w (s_buf *buf, s64 fd);
void    buf_fd_close (s_buf *buf);

#endif /* LIBKC3_BUF_FD_H */
