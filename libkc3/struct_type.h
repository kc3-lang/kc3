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
/**
 * @file struct_type.h
 * @brief Struct type : [key: value, ...]
 *
 * A structure type is defined using defstruct.
 * There can be only one defstruct per module.
 * The struct type can be referred to by just the module name.
 * The rest of the module should contain operations on that struct.
 */
#ifndef LIBKC3_STRUCT_TYPE_H
#define LIBKC3_STRUCT_TYPE_H

#include "types.h"

/* Stack-allocation functions, call struct_type_clean after use. */
void            struct_type_clean (s_struct_type *st);
s_struct_type * struct_type_init (s_struct_type *st,
                                  const s_sym *module,
                                  const s_list *spec);
s_struct_type * struct_type_init_copy (s_struct_type *st,
                                       s_struct_type *src);

/* Heap-allocation functions, call struct_type_delete after use. */
void            struct_type_delete (s_struct_type *st);
s_struct_type * struct_type_new (const s_sym *module,
                                 const s_list *spec);
s_struct_type * struct_type_new_copy (s_struct_type *src);
s_struct_type * struct_type_new_ref (s_struct_type *src);

/* Observers. */
uw     struct_type_compute_size (uw offset, u8 align_max);
void * struct_type_copy_data (const s_struct_type *st,
                              void *dest,
                              const void *src);
bool * struct_type_exists (const s_sym *module,
                           bool *dest);
uw *   struct_type_find_key_index (const s_struct_type *st,
                                   const s_sym *key,
                                   uw *dest);
uw     struct_type_padding (uw offset, u8 align, u8 *max);

/* Operators */
s_struct_type * struct_type_update_map (s_struct_type *st);

#endif /* LIBKC3_STRUCT_TYPE_H */
