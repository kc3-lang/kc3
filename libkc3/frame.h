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
#ifndef LIBKC3_FRAME_H
#define LIBKC3_FRAME_H

#include "types.h"

/* Stack-allocation compatible functions, call frame_clean after use. */
s_frame * frame_clean (s_frame *frame);
s_frame * frame_init (s_frame *frame, s_frame *next);

/* Constructors. */
s_frame * frame_new (s_frame *next);

/* Destructors. */
s_frame * frame_delete (s_frame *frame);
void      frame_delete_all (s_frame *frame);

/* Operators. */
s_frame * frame_binding_new (s_frame *frame, const s_sym *name,
                                 const s_tag *value);
s_tag *   frame_get_w (s_frame *frame, const s_sym *sym);

/* Observers. */
const s_tag * frame_get (const s_frame *frame, const s_sym *sym);

#endif /* LIBKC3_FRAME_H */