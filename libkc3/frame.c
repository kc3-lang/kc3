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
#include "alloc.h"
#include "assert.h"
#include "binding.h"
#include "frame.h"
#include "list.h"
#include "sym.h"
#include "tag.h"

s_tag * frame_binding_new (s_frame *frame, const s_sym *name)
{
  s_binding *b;
  b = binding_new(name, frame->bindings);
  if (! b)
    return NULL;
  frame->bindings = b;
  tag_init_var(&b->value, &g_sym_Tag);
  return &b->value;
}

s_frame * frame_binding_new_copy (s_frame *frame, const s_sym *name,
                                  s_tag *value)
{
  s_tag *tag;
  frame_binding_new(frame, name);
  tag = binding_get_w(frame->bindings, name);
  if (tag == NULL) {
    err_puts("frame_binding_new_copy: binding new");
    assert(! "frame_binding_new_copy: binding new");
    return NULL;
  }
  if (value->type == TAG_VAR) {
    tag_init_var(tag, value->data.var.type);
    value->data.var.ptr = tag->data.var.ptr;
  }
  else if (! tag_init_copy(tag, value)) {
    err_puts("frame_binding_new_copy: tag_init_copy");
    assert(! "frame_binding_new_copy: tag_init_copy");
    frame = frame_binding_delete(frame, name);
    return NULL;
  }
  return frame;
}

s_frame * frame_binding_delete (s_frame *frame, const s_sym *name)
{
  s_binding **b;
  b = binding_find(&frame->bindings, name);
  *b = binding_delete(*b);
  return frame;
}

s_frame * frame_binding_replace (s_frame *frame, const s_sym *name,
                                 const s_tag *value)
{
  s_tag *tag;
  tag = binding_get_w(frame->bindings, name);
  if (tag) {
    tag_clean(tag);
    if (! tag_init_copy(tag, value)) {
      err_puts("frame_binding_new_copy: tag_init_copy 1");
      assert(! "frame_binding_new_copy: tag_init_copy 1");
      return NULL;
    }
    return frame;
  }
  frame_binding_new(frame, name);
  tag = binding_get_w(frame->bindings, name);
  if (! tag) {
    err_puts("frame_binding_new_copy: binding new");
    assert(! "frame_binding_new_copy: binding new");
    return NULL;
  }
  if (! tag_init_copy(tag, value)) {
    err_puts("frame_binding_new_copy: tag_init_copy 2");
    assert(! "frame_binding_new_copy: tag_init_copy 2");
    frame = frame_binding_delete(frame, name);
    return NULL;
  }
  return frame;
}

s_frame * frame_clean (s_frame *frame)
{
  s_frame *next;
  assert(frame);
  next = frame->next;
  binding_delete_all(frame->bindings);
  if (frame->fn_frame)
    frame_delete_all(frame->fn_frame);
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
  s_frame *f;
  f = frame;
  while (f)
    f = frame_delete(f);
}

const s_tag * frame_get (const s_frame *frame, const s_sym *sym)
{
  const s_frame *f;
  const s_tag *result;
  assert(sym);
  f = frame;
  while (f) {
    result = binding_get(f->bindings, sym);
    if (result)
      return result;
    if (f->fn_frame) {
      result = frame_get(f->fn_frame, sym);
      if (result)
        return result;
    }
    f = f->next;
  }
  return NULL;
}

s_tag * frame_get_w (s_frame *frame, const s_sym *sym)
{
  s_frame *f;
  s_tag *result;
  assert(sym);
  f = frame;
  while (f) {
    result = binding_get_w(f->bindings, sym);
    if (result)
      return result;
    if (f->fn_frame) {
      result = frame_get_w(f->fn_frame, sym);
      if (result)
        return result;
    }
    f = f->next;
  }
  return NULL;
}

s_frame * frame_init (s_frame *frame, s_frame *next,
                      const s_frame *fn_frame)
{
  s_frame tmp = {0};
  assert(frame);
  tmp.next = next;
  tmp.fn_frame = frame_new_copy(fn_frame);
  *frame = tmp;
  return frame;
}

s_frame * frame_new (s_frame *next, const s_frame *fn_frame)
{
  s_frame *frame;
  frame = alloc(sizeof(s_frame));
  if (! frame)
    return NULL;
  if (! frame_init(frame, next, fn_frame)) {
    free(frame);
    return NULL;
  }
  return frame;
}

s_frame * frame_new_copy (const s_frame *src)
{
  s_frame **f;
  s_frame  *frame;
  const s_frame *s;
  frame = NULL;
  f = &frame;
  s = src;
  while (s) {
    *f = frame_new(NULL, s->fn_frame);
    if (s->bindings &&
        ! ((*f)->bindings = binding_new_copy(s->bindings)))
      goto clean;
    f = &(*f)->next;
    s = s->next;
  }
  return frame;
 clean:
  frame_delete_all(frame);
  return NULL;
}

s_frame * frame_replace (s_frame *frame, const s_sym *sym,
                         s_tag *value)
{
  s_tag *result;
  assert(sym);
  result = binding_get_w(frame->bindings, sym);
  if (result) {
    tag_clean(result);
    if (value->type == TAG_VAR) {
      tag_init_var(result, value->data.var.type);
      value->data.var.ptr = result->data.var.ptr;
    }
    else
      tag_init_copy(result, value);
    return frame;
  }
  return frame_binding_new_copy(frame, sym, value);
}
