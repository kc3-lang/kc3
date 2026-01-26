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
 * @file fd.h
 * @brief File descriptor operations.
 *
 * Structure to manipulate file descriptors.
 */
#ifndef LIBKC3_FD_H
#define LIBKC3_FD_H

#include "types.h"

/* Operators. */
bool *  fd_is_a_tty (s64 fd, bool *dest);
s_str * fd_read_until_eof (s64 fd, s_str *dest);
bool    fd_set_blocking (s64 fd, bool blocking);

#endif /* LIBKC3_FD_H */
