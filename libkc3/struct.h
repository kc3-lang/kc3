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
/**
 * @file tag.h
 * @brief Struct : %Module{key: value, ...}
 *
 */
#ifndef LIBKC3_STRUCT_H
#define LIBKC3_STRUCT_H

#include "types.h"

/* Stack-allocation compatible functions, call struct_clean after use. */
void       struct_clean (s_struct *s);
s_struct * struct_init (s_struct *s, const s_sym *module);
s_struct * struct_init_1 (s_struct *s, const s8 *p);
s_struct * struct_init_cast (s_struct *s, const s_sym * const *type,
                             const s_tag *tag);
s_struct * struct_init_copy (s_struct *s, const s_struct *src);
s_struct * struct_init_from_lists (s_struct *s, const s_sym *module,
                                   const s_list *keys,
                                   const s_list *values);
s_struct * struct_init_with_data (s_struct *s, const s_sym *module,
                                  void *data);

/* Heap-allocation functions, call struct_delete after use. */
void       struct_delete (s_struct *s);
s_struct * struct_new (const s_sym *module);
s_struct * struct_new_1 (const s8 *p);
s_struct * struct_new_copy (const s_struct *src);
s_struct * struct_new_with_data (const s_sym *module, void *data);

/* Operators. */
s_struct * struct_allocate (s_struct *s);
s_struct * struct_set (s_struct *s, const s_sym *key,
                       const s_tag *value);

/* Observers. */
s_tag *        struct_access (const s_struct *s, const s_sym *key,
                              s_tag *dest);
bool           struct_find_key_index (const s_struct *s, const s_sym *key,
                                      uw *dest);
const void *   struct_get (const s_struct *s, const s_sym *key);
const s_sym ** struct_get_type (const s_struct *s, const s_sym *key,
                                const s_sym **dest);
const s_sym ** struct_get_sym (const s_struct *s, const s_sym *key);
const s_tag *  struct_get_tag (const s_struct *s, const s_sym *key);
u8             struct_get_u8 (const s_struct *s, const s_sym *key);

#endif /* LIBKC3_STRUCT_H */
