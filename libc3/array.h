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
#ifndef ARRAY_H
#define ARRAY_H

#include "types.h"

void      array_clean (s_array *a);
s_array * array_copy (const s_array *src, s_array *dest);
s_array * array_init (s_array *a, e_tag_type type, uw dimension,
                      const uw *dimensions);
s_array * array_init_1 (s_array *a, s8 *p);
void *    array_data (const s_array *a, const uw *address);
s_tag *   array_data_tag (s_tag *a, s_tag *address, s_tag *dest);

#endif /* ARRAY_H */
