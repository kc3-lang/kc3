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
#ifndef LIBC3_ARRAY_H
#define LIBC3_ARRAY_H

#include "types.h"

/* Stack-allocation compatible functions, call array_clean after use. */
void      array_clean (s_array *a);
s_array * array_init (s_array *a, const s_sym *type,
                      uw dimension, const uw *dimensions);
s_array * array_init_1 (s_array *a, s8 *p);
s_array * array_init_copy (s_array *a, const s_array *src);

/* Observers */
s_str *            array_inspect (const s_array *array, s_str *dest);
void *             array_data (const s_array *a, const uw *address);
s_tag *            array_data_tag (s_tag *a, const s_tag *address,
                                   s_tag *dest);
uw                 array_type_size (const s_sym *type);
f_buf_inspect      array_type_to_buf_inspect (const s_sym *type);
f_buf_inspect_size array_type_to_buf_inspect_size (const s_sym *type);
f_init_copy        array_type_to_init_copy (const s_sym *type);
e_tag_type         array_type_to_tag_type (const s_sym *type);

#endif /* LIBC3_ARRAY_H */
