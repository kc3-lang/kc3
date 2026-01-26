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
#ifndef LIBKC3_ASSERT_H
#define LIBKC3_ASSERT_H

#include <stdlib.h>
#include "env.h"
#include "io.h"

#ifdef DEBUG
# define assert(test)                                                  \
  do {                                                                 \
    sw assert_line = __LINE__;                                         \
    if (! (test)) {                                                    \
      s_env *env;                                                      \
      env = env_global();                                              \
      if (! env)                                                       \
        env = env_default();                                           \
      if (! env)                                                       \
        abort();                                                       \
      if (env->argv && env->argv[0]) {                                 \
        err_write_1(env->argv[0]);                                     \
        err_write_1(": ");                                             \
      }                                                                \
      err_write_1("assertion failed: ");                               \
      err_write_1(__FILE__);                                           \
      err_write_1(":");                                                \
      err_inspect_sw_decimal(assert_line);                             \
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

#endif /* LIBKC3_ASSERT_H */
