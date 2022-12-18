/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
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
 * @file buf_file.h
 * @brief s_buf interface to stdio.h FILE *.
 */
#ifndef BUF_FILE_H
#define BUF_FILE_H

#include <stdio.h>
#include "types.h"

/* observers */
e_bool    buf_file_is_open (s_buf *buf);

/* modifiers */
s_buf * buf_file_open_r (s_buf *buf, FILE *fp);
s_buf * buf_file_open_w (s_buf *buf, FILE *fp);
void    buf_file_close (s_buf *buf);

#endif /* BUF_FILE_H */
