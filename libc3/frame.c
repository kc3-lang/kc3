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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "binding.h"
#include "frame.h"
#include "list.h"

void frame_binding_new (s_frame *frame, const s_sym *name,
                        const s_tag *value)
{
  frame->bindings = binding_new(name, value, frame->bindings);
}

s_frame * frame_clean (s_frame *frame)
{
  s_frame *next;
  assert(frame);
  next = frame->next;
  binding_delete_all(frame->bindings);
  return next;
}

s_frame * frame_delete (s_frame *frame)
{
  s_frame *next = NULL;
  if (frame) {
    next = frame_clean(frame);
    free(frame);
  }
  return next;
}

void frame_delete_all (s_frame *frame)
{
  while (frame)
    frame = frame_delete(frame);
}

const s_tag * frame_get (const s_frame *frame, const s_sym *sym)
{
  return binding_get(frame->bindings, sym);
}

s_frame * frame_init (s_frame *frame, s_frame *next)
{
  assert(frame);
  frame->bindings = NULL;
  frame->next = next;
  return frame;
}

s_frame * frame_new (s_frame *next)
{
  s_frame *frame;
  if (! (frame = malloc(sizeof(s_frame))))
    errx(1, "frame_new: out of memory");
  return frame_init(frame, next);
}
