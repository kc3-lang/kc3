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
  buf_write_1(buf, "\nStacktrace:\n");
  error_print_stacktrace(buf, error_handler->stacktrace);
}

void error_print_stacktrace (s_buf *buf, const s_list *stacktrace)
{
  while (stacktrace) {
    buf_inspect_tag(buf, &stacktrace->tag);
    buf_write_1(buf, "\n");
  }
}
