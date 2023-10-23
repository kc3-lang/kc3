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
 * @file file.h
 * @brief file operations.
 *
 * Structure to manipulate files.
 */
#ifndef FILE_H
#define FILE_H

#include "types.h"

/* Observers */
int     file_copy (const char *from, const char *to);
s_tag * file_mtime (const s_str *path, s_tag *dest);

#endif /* FILE_H */
