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
#ifndef LIBKC3_ERROR_H
#define LIBKC3_ERROR_H

#include "types.h"

#define error(message)                                                 \
  do {                                                                 \
    err_puts(message);                                                 \
    assert(! message);                                                 \
    abort();                                                           \
  } while (0)

void error_print (s_buf *buf, const s_error_handler *error_handler);
void error_print_stacktrace (s_buf *buf, const s_list *stacktrace);

#endif /* LIBKC3_ERROR_H */
