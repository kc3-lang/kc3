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
#ifndef LIBC3_IO_H
#define LIBC3_IO_H

#include "types.h"

#define ERR_INSPECT(name, type)                                        \
  sw err_inspect_ ## name (const type *x)                              \

#define IO_INSPECT(name, type)                                         \
  sw io_inspect_ ## name (const type *x)                               \

/* error output */
sw err_inspect (const s_tag *x);
sw err_inspect_fn_pattern (const s_list *x);
ERR_INSPECT(list, s_list);
sw err_puts (const s8 *x);

/* standard output */
sw io_inspect (const s_tag *x);
IO_INSPECT(fact, s_fact);
IO_INSPECT(str,  s_str);
sw io_puts (const s8 *x);

#endif /* LIBC3_IO_H */
