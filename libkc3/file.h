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
 * @file file.h
 * @brief file operations.
 *
 * Structure to manipulate files.
 *
 * Arguments :
 *  - mode: one of `[:r, :rw, :rwx, :rx, :w, :wx, :x]`
 */
#ifndef LIBKC3_FILE_H
#define LIBKC3_FILE_H

#include "types.h"

/* Observers */
bool          file_access (const s_str *path, const s_sym *mode);
sw            file_copy (const char *from, const char *to);
s_str *       file_dirname (const s_str *path, s_str *dest);
s_str *       file_ext (const s_str *path, s_str *dest);
s_time *      file_mtime (const s_str *path, s_time *dest);
s_str *       file_name (const s_str *path, s_str *dest);
s_str *       file_read_max (const s_str *path, uw max, s_str *dest);
s_str *       file_read_all (const s_str *path, s_str *dest);
s_str *       file_search (const s_str *suffix, const s_sym *mode,
                           s_str *dest);
s_file_stat * file_stat (const s_str *path, s_file_stat *dest);

/* Operators. */
s_str *    file_pwd (s_str *dest);
FILE *     file_open (const char *path, const char *mode);
s32 *      file_open_r (const s_str *path, s32 *dest);
s32 *      file_open_w (const s_str *path, s32 *dest);

#endif /* LIBKC3_FILE_H */
