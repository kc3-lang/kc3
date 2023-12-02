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
/**
 * @file struct_type.h
 * @brief Struct type : [key: value, ...]
 *
 * A structure type is defined using defstruct.
 * There can be only one defstruct per module.
 * The struct type can be referred to by just the module name.
 * The rest of the module should contain operations on that struct.
 */
#ifndef LIBC3_STRUCT_TYPE_H
#define LIBC3_STRUCT_TYPE_H

#include "types.h"

/* Stack-allocation compatible functions, call struct_type_clean after
   use. */
void            struct_type_clean (s_struct_type *s);
s_struct_type * struct_type_init (s_struct_type *s, const s_sym *module);
s_struct_type * struct_type_init_1 (s_struct_type *s, const s8 *p);

/* Heap-allocation functions, call struct_type_delete after use. */
void            struct_type_delete (s_struct_type *s);
s_struct_type * struct_type_new (const s_sym *module);
s_struct_type * struct_type_new_1 (const s8 *p);

#endif /* LIBC3_STRUCT_TYPE_H */
