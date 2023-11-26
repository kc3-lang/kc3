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

#define PROTOTYPE_ERR_INSPECT(name, type)                              \
  sw err_inspect_ ## name (const type *x)                              \

#define PROTOTYPE_IO_INSPECT(name, type)                               \
  sw io_inspect_ ## name (const type *x)                               \

#define PROTOTYPES_ERR_IO_INSPECT(name, type)                          \
  PROTOTYPE_ERR_INSPECT(name, type);                                   \
  PROTOTYPE_IO_INSPECT(name, type)

/* error output */
sw err_inspect (const s_tag *x);
sw err_puts (const s8 *x);

/* standard output */
sw io_inspect (const s_tag *x);
sw io_puts (const s8 *x);

PROTOTYPES_ERR_IO_INSPECT(fact,       s_fact);
PROTOTYPES_ERR_IO_INSPECT(fn_pattern, s_list);
PROTOTYPES_ERR_IO_INSPECT(list,       s_list *);
PROTOTYPES_ERR_IO_INSPECT(str,        s_str);
PROTOTYPES_ERR_IO_INSPECT(tag,        s_tag);

#endif /* LIBC3_IO_H */
