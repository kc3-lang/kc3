/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include "buf.h"
#include "buf_inspect.h"
#include "error.h"

void error_print (s_buf *buf, const s_error_handler *error_handler)
{
  buf_write_1(buf, "error: ");
  buf_inspect_tag(buf, &error_handler->tag);
  buf_write_1(buf, "\nBacktrace:\n");
  error_print_backtrace(buf, error_handler->backtrace);
}

void error_print_backtrace (s_buf *buf, const s_list *backtrace)
{
  while (backtrace) {
    buf_inspect_tag(buf, &backtrace->tag);
    buf_write_1(buf, "\n");
  }
}
