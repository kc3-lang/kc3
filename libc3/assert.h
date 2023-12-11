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
#ifndef LIBC3_ASSERT_H
#define LIBC3_ASSERT_H

#include "env.h"
#include "io.h"

void abort (void);

#ifdef DEBUG
# define assert(test)                                                  \
  do {                                                                 \
    sw assert_line = __LINE__;                                         \
    if (! (test)) {                                                    \
      if (g_c3_env.argv && g_c3_env.argv[0]) {                         \
        err_write_1(g_c3_env.argv[0]);                                 \
        err_write_1(": ");                                             \
      }                                                                \
      err_write_1("assertion failed: ");                               \
      err_write_1(__FILE__);                                           \
      err_write_1(":");                                                \
      err_inspect_sw(&assert_line);                                    \
      err_write_1(": ");                                               \
      err_write_1(__func__);                                           \
      err_write_1(": ");                                               \
      err_puts(# test);                                                \
      abort();                                                         \
    }                                                                  \
  } while(0)
#else
# define assert(test)
#endif

#endif /* LIBC3_ASSERT_H */
