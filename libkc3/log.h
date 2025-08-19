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
#ifndef LIBKC3_LOG_H
#define LIBKC3_LOG_H

#include "types.h"

/* Stack-allocation compatible functions, call log_clean after use. */
void    log_clean (s_log *log);
s_log * log_init (s_log *log);

/* Heap-allocation functions, call log_delete after use. */
void    log_delete (s_log *log);
s_log * log_new (void);

/* Operators. */
void log_close (s_log *log);
s_log * log_open (s_log *log, FILE *fp, const s_str *path);
s_log * log_open_binary (s_log *log, FILE *fp, const s_str *path);

/* Misc. */
s_str * log_path_to_binary_path (const s_str *path, s_str *dest);

#endif /* LIBKC3_LOG_H */
