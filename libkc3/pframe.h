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
#ifndef LIBKC3_PFRAME_H
#define LIBKC3_PFRAME_H

#include "types.h"

/* Initialization functions, call pframe_clean after use. */
void      pframe_clean (p_frame *pframe);
p_frame * pframe_init_cast (p_frame *pframe, p_sym *type, s_tag *src);
p_frame * pframe_init_copy (p_frame *pframe, p_frame *src);

#endif /* LIBKC3_PFRAME_H */
