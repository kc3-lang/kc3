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
 * @file pstruct_type.h
 * @brief Pointer to struct type
 *
 */
#ifndef LIBKC3_PSTRUCT_TYPE_H
#define LIBKC3_PSTRUCT_TYPE_H

#include "types.h"

/* Stack-allocation functions, call pstruct_type_clean after use. */
void            pstruct_type_clean (p_struct_type *st);
p_struct_type * pstruct_type_init (p_struct_type *st,
                                   const s_sym *module,
                                   const s_list *spec);
p_struct_type * pstruct_type_init_cast (p_struct_type *st,
                                        p_sym *type,
                                        s_tag *tag);
p_struct_type * pstruct_type_init_copy (p_struct_type *st,
                                        p_struct_type *src);
p_struct_type * pstruct_type_init_clean (p_struct_type *st,
                                         s_struct_type *src,
                                         p_callable clean);

/* Operators. */
p_struct_type * pstruct_type_find (const s_sym *module,
                                   p_struct_type *dest);

#endif /* LIBKC3_STRUCT_TYPE_H */
