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
#ifndef LIBC3_ARG_H
#define LIBC3_ARG_H

#include "types.h"

/* stack-allocation compatible functions */
s_arg * arg_init (s_arg *arg);

/* constructors */
s_arg * arg_new (void);

/* destructors */
s_arg * arg_delete (s_arg *arg);
void    arg_delete_all (s_arg *arg);

/* observers */
uw arg_length (s_arg *args);

#endif /* LIBC3_ARG_H */
