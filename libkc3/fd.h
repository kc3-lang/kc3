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
 * @file fd.h
 * @brief File descriptor operations.
 *
 * Structure to manipulate file descriptors.
 */
#ifndef LIBKC3_FD_H
#define LIBKC3_FD_H

#include "types.h"

/* Operators. */
s_str * fd_read_until_eof (s32 fd, s_str *dest);
bool *  fd_set_blocking (s32 fd, bool blocking, bool *result);

#endif /* LIBKC3_FD_H */
