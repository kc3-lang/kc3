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
#ifndef LIBKC3_PTAG_H
#define LIBKC3_PTAG_H

#include "types.h"

p_tag * ptag_init_cast (p_tag *dest, const s_sym * const *type,
                        const s_tag *tag);
p_tag * ptag_init_copy (p_tag *dest, const p_tag *src);

#endif /* LIBKC3_PTAG_H */
