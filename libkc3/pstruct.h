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
/**
 * @file pstruct.h
 * @brief Pointer to s_struct.
 *
 */
#ifndef LIBKC3_PSTRUCT_H
#define LIBKC3_PSTRUCT_H

#include "types.h"

/* Stack-allocation compatible functions, call pstruct_clean after use. */
void       pstruct_clean (p_struct *s);
s_struct * pstruct_init (p_struct *s, const s_sym *module);
s_struct * pstruct_init_type (p_struct *s, const s_struct_type *type);
s_struct * pstruct_init_copy (p_struct *s, p_struct src);

#endif /* LIBKC3_STRUCT_H */
