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
 * @file pstruct.h
 * @brief Pointer to s_struct.
 *
 */
#ifndef LIBKC3_PSTRUCT_H
#define LIBKC3_PSTRUCT_H

#include "types.h"

/* Stack-allocation compatible functions, call pstruct_clean
   after use. */
void       pstruct_clean (p_struct *s);
p_struct * pstruct_init (p_struct *s, const s_sym *module);
p_struct * pstruct_init_1 (p_struct *s, const char *p);
p_struct * pstruct_init_cast (p_struct *s, const s_sym * const *type,
                              s_tag *tag);
p_struct * pstruct_init_copy (p_struct *s, p_struct *src);
p_struct * pstruct_init_copy_data (p_struct *s, const s_sym *module,
                                   void *data);
p_struct * pstruct_init_put (p_struct *s, p_struct src,
                             const s_sym *key, s_tag *value);
p_struct * pstruct_init_with_data (p_struct *s, const s_sym *module,
                                   void *data, bool free_data);
p_struct * pstruct_init_with_type (p_struct *s, s_struct_type *type);

#endif /* LIBKC3_STRUCT_H */
