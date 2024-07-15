/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_PCOW_H
#define LIBKC3_PCOW_H

#include "types.h"

/* Stack-allocation compatible functions, call pcow_clean after use. */
void     pcow_clean (s_cow **p);
s_cow ** pcow_init (s_cow **p, const s_sym *type);
s_cow ** pcow_init_cast (s_cow **p, const s_sym * const *type,
                         const s_tag *src);
s_cow ** pcow_init_copy (s_cow **p, const s_cow * const *src);

#endif /* LIBKC3_PCOW_H */
