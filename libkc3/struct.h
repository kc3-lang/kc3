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
 * @file struct.h
 * @brief Struct : %Module{key: value, ...}
 *
 */
#ifndef LIBKC3_STRUCT_H
#define LIBKC3_STRUCT_H

#include "types.h"

/* Stack-allocation compatible functions, call struct_clean
   after use. */
void       struct_clean (s_struct *s);
s_struct * struct_init (s_struct *s, const s_sym *module);
s_struct * struct_init_copy (s_struct *s, s_struct *src);
s_struct * struct_init_from_lists (s_struct *s, const s_sym *module,
                                   s_list *keys,
                                   s_list *values);
s_struct * struct_init_with_data (s_struct *s, const s_sym *module,
                                  void *data, bool free_data);
s_struct * struct_init_with_type (s_struct *s, p_struct_type type);

/* Heap-allocation functions, call struct_delete after use. */
void       struct_delete (s_struct *s);
s_struct * struct_new (const s_sym *module);
s_struct * struct_new_copy (s_struct *src);
s_struct * struct_new_ref (s_struct *src);
s_struct * struct_new_with_data (const s_sym *module, void *data,
                                 bool free_data);
s_struct * struct_new_with_type (p_struct_type type);

/* Operators. */
s_tag *    struct_access (s_struct *s, s_list *key, s_tag *dest);
s_tag *    struct_access_sym (s_struct *s, const s_sym *key,
                              s_tag *dest);
s_struct * struct_allocate (s_struct *s);
s_tag *    struct_get_tag (s_struct *s, const s_sym *key);
void *     struct_get_w (s_struct *s, const s_sym *key);
s_struct * struct_set (s_struct *s, const s_sym *key,
                       s_tag *value);

/* Observers. */
uw *           struct_find_key_index (const s_struct *s,
                                      const s_sym *key, uw *dest);
const void *   struct_get (const s_struct *s, const s_sym *key);
const s_sym ** struct_get_type (const s_struct *s, const s_sym *key,
                                const s_sym **dest);
const s_sym ** struct_get_sym (const s_struct *s, const s_sym *key);
u8             struct_get_u8 (const s_struct *s, const s_sym *key);
const s_sym ** struct_get_var_type (const s_struct *s, const s_sym *key,
                                    const s_sym **dest);
uw *           struct_offset (const s_struct *s,
                              const s_sym * const *key,
                              uw *dest);

#endif /* LIBKC3_STRUCT_H */
