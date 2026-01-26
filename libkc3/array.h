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
#ifndef LIBKC3_ARRAY_H
#define LIBKC3_ARRAY_H

#include "types.h"

/* Stack-allocation compatible functions, call array_clean after use. */
void      array_clean (s_array *a);
s_array * array_init (s_array *a, const s_sym *array_type,
                      uw dimension, const uw *dimensions);
s_array * array_init_1 (s_array *a, const char *p);
s_array * array_init_cast (s_array *a, const s_sym * const *type,
                           const s_tag *tag);
s_array * array_init_copy (s_array *a, const s_array *src);
s_array * array_init_copy_shallow (s_array *a, const s_array *src);
s_array * array_init_void (s_array *array);

/* Observers */
void *             array_data (const s_array *a, const uw *address);
s_tag *            array_data_tag (const s_array *a,
                                   const s_array *address,
                                   s_tag *dest);

/* Operators */
s_tag *   array_access (s_array *a, s_list *key, s_tag *dest);
s_array * array_allocate (s_array *a);
s_array * array_data_set (s_array *a, const uw *address, void *data);
s_array * array_free (s_array *a);

#endif /* LIBKC3_ARRAY_H */
