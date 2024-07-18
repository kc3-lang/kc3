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
#ifndef LIBC3_PCOMPLEX_H
#define LIBC3_PCOMPLEX_H

#include "types.h"

/* Stack-allocation compatible functions, call pcomplex_clean
   after use. */
void         pcomplex_clean (s_complex **p);
s_complex ** pcomplex_init (s_complex **p);
s_complex ** pcomplex_init_cast (s_complex **p,
                                 const s_sym * const *type,
                                 const s_tag *src);
s_complex ** pcomplex_init_copy (s_complex **p,
				 const s_complex * const *src);

#endif /* LIBC3_PCOMPLEX_H */
