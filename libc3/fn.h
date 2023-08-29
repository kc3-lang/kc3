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
/**
 * @file fn.h
 * Function type.
 *
 * Syntax : "fn" (type? "(" ((type space)? sym ","?)* ")" "{" prog "}")+
 */
#ifndef FN_H
#define FN_H

#include "types.h"

/* stack-allocation compatible functions */
void   fn_clean (s_fn *fn);
s_fn * fn_init (s_fn *fn);
s_fn * fn_init_1 (s_fn *fn, s8 *p);

/* constructors */
s_fn * fn_new ();
s_fn * fn_new_copy (const s_fn *fn);

/* destructors */
void   fn_delete (s_fn *fn);

/* observers */
s_fn * fn_copy (const s_fn *src, s_fn *dest);

#endif /* FN_H */
