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
#ifndef LIBC3_FRAME_H
#define LIBC3_FRAME_H

#include "types.h"

/* stack-allocation compatible functions */
s_frame * frame_clean (s_frame *frame);
s_frame * frame_init (s_frame *frame, s_frame *next);

/* constructors */
s_frame * frame_new (s_frame *next);

/* destructors */
s_frame * frame_delete (s_frame *frame);
void      frame_delete_all (s_frame *frame);

/* modifiers */
s_frame * frame_binding_new (s_frame *frame, const s_sym *name,
                             const s_tag *value);

/* observers */
const s_tag * frame_get (const s_frame *frame, const s_sym *sym);

#endif /* LIBC3_FRAME_H */
