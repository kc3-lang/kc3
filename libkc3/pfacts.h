/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_PFACTS_H
#define LIBKC3_PFACTS_H

#include "types.h"
#include <config.h>

/* Initialization functions, call pfacts_clean after use. */
void      pfacts_clean (p_facts *pfacts);
p_facts * pfacts_init (p_facts *pfacts);
p_facts * pfacts_init_cast (p_facts *pfacts, const p_sym *type,
                            s_tag *src);
p_facts * pfacts_init_copy (p_facts *pfacts, p_facts *src);

#endif /* LIBKC3_PFACTS_H */
