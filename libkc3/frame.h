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
#ifndef LIBKC3_FRAME_H
#define LIBKC3_FRAME_H

#include "types.h"

/* Stack-allocation compatible functions, call frame_clean after use. */
s_frame * frame_clean (s_frame *frame);
s_frame * frame_init (s_frame *frame, s_frame *next);
s_frame * frame_init_copy (s_frame *frame, s_frame *src);

/* Constructors. */
s_frame * frame_new (s_frame *next);
s_frame * frame_new_copy (const s_frame *src);
s_frame * frame_new_ref (s_frame *frame);

/* Destructors. */
s_frame * frame_delete (s_frame *frame);
void      frame_delete_all (s_frame *frame);

/* Operators. */
s_tag *   frame_binding_new_void (s_frame *frame, const s_sym *name);
s_frame * frame_binding_new (s_frame *frame, const s_sym *name,
                             s_tag *value);
s_frame * frame_binding_new_copy (s_frame *frame, s_binding *src);
s_tag *   frame_binding_new_var (s_frame *frame);
s_frame * frame_binding_delete (s_frame *frame, const s_sym *name);
s_frame * frame_binding_replace (s_frame *frame, const s_sym *name,
                                 s_tag *value);
s_tag *   frame_get (s_frame *frame, const s_sym *sym);
s_tag *   frame_get_w (s_frame *frame, const s_sym *sym);
s_frame * frame_replace (s_frame *frame, const s_sym *sym,
                         s_tag *value);

#endif /* LIBKC3_FRAME_H */
