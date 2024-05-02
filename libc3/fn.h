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
/**
 * @file fn.h
 * Function type.
 *
 * Syntax : "fn" (type? "(" ((type space)? sym ","?)* ")" "{" prog "}")+
 */
#ifndef LIBC3_FN_H
#define LIBC3_FN_H

#include "types.h"

/* Stack-allocation compatible functions, call fn_clean after use. */
void   fn_clean (s_fn *fn);
s_fn * fn_init (s_fn *fn, const s_sym *module);
s_fn * fn_init_1 (s_fn *fn, char *p);
s_fn * fn_init_cast (s_fn *fn, const s_sym * const *type,
                     const s_tag *tag);
s_fn * fn_init_copy (s_fn *fn, const s_fn *src);

/* Heap-allocation functions, call fn_delete after use. */
void   fn_delete (s_fn *fn);
s_fn * fn_new (const s_sym *module);
s_fn * fn_new_copy (const s_fn *fn);

/* Observers. */
s8 fn_arity (const s_fn *fn);

#endif /* LIBC3_FN_H */
