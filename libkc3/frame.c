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
#include "alloc.h"
#include "assert.h"
#include "binding.h"
#include "fn.h"
#include "frame.h"
#include "ident.h"
#include "list.h"
#include "sym.h"
#include "tag.h"

s_frame * frame_binding_delete (s_frame *frame, const s_sym *name)
{
  s_binding **b;
  b = binding_find(&frame->bindings, name);
  *b = binding_delete(*b);
  return frame;
}

s_tag * frame_binding_new_void (s_frame *frame, const s_sym *name)
{
  s_binding *b;
  b = binding_new(name, frame->bindings);
  if (! b)
    return NULL;
  frame->bindings = b;
  return tag_init_void(&b->value);
}

s_frame * frame_binding_new (s_frame *frame, const s_sym *name,
                             s_tag *value)
{
  s_ident ident = {0};
  s_tag *tag;
  if (! (tag = frame_binding_new_void(frame, name))) {
    err_puts("frame_binding_new_copy: frame_binding_new");
    assert(! "frame_binding_new_copy: frame_binding_new");
    return NULL;
  }
  if (! tag_init_copy(tag, value)) {
    err_puts("frame_binding_new_copy: tag_init_copy");
    assert(! "frame_binding_new_copy: tag_init_copy");
    frame = frame_binding_delete(frame, name);
    return NULL;
  }
  ident.module = env_global()->current_defmodule;
  ident.sym = name;
  if (! tag_set_name_if_null(tag, &ident))
    return NULL;
  return frame;
}

s_frame * frame_binding_new_copy (s_frame *frame, s_binding *src)
{
  while (src) {
    if (! frame_binding_new(frame, src->name, &src->value))
      return NULL;
    src = src->next;
  }
  return frame;
}

s_tag * frame_binding_new_var (s_frame *frame)
{
  s_binding *b;
  b = binding_new(NULL, frame->bindings);
  if (! b)
    return NULL;
  frame->bindings = b;
  tag_init_pvar(&b->value, &g_sym_Tag);
  return &b->value;
}

s_frame * frame_binding_replace (s_frame *frame, const s_sym *name,
                                 s_tag *value)
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
  frame_binding_new_void(frame, name);
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
  return next;
}

s_frame * frame_delete (s_frame *frame)
{
  s_frame *next = NULL;
  if (frame) {
    next = frame_clean(frame);
    alloc_free(frame);
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

s_tag * frame_get (s_frame *frame, const s_sym *sym)
{
  s_frame *f;
  s_tag *result;
  assert(sym);
  f = frame;
  while (f) {
    result = binding_get(f->bindings, sym);
    if (result)
      return result;
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
    f = f->next;
  }
  return NULL;
}

s_frame * frame_init (s_frame *frame, s_frame *next)
{
  s_frame tmp = {0};
  assert(frame);
  tmp.next = next;
  *frame = tmp;
  return frame;
}
s_frame * frame_init_copy (s_frame *frame, s_frame *src)
{
  s_frame tmp = {0};
  if (! frame || ! src) {
    err_puts("frame_init_copy: invalid argument");
    assert(! "frame_init_copy: invalid argument");
    return NULL;
  }
  if (! frame_init(&tmp, NULL))
    return NULL;
  if (src->bindings &&
      ! (tmp.bindings = binding_new_copy(src->bindings))) {
    frame_clean(&tmp);
    return NULL;
  }
  *frame = tmp;
  return frame;
}

s_frame * frame_new (s_frame *next)
{
  s_frame *frame;
  frame = alloc(sizeof(s_frame));
  if (! frame)
    return NULL;
  if (! frame_init(frame, next)) {
    alloc_free(frame);
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
    *f = frame_new(NULL);
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

s_frame * frame_new_ref (s_frame *frame)
{
  return frame;
}

s_frame * frame_replace (s_frame *frame, const s_sym *sym,
                         s_tag *value)
{
  s_ident ident = {0};
  s_tag *result;
  assert(sym);
  result = binding_get_w(frame->bindings, sym);
  if (result) {
    tag_clean(result);
    if (value->type == TAG_PVAR) {
      tag_init_pvar_copy(result, &value->data.td_pvar);
    }
    else {
      if (! tag_init_copy(result, value)) {
        err_puts("frame_replace: tag_init_copy");
        assert(! "frame_replace: tag_init_copy");
        return NULL;
      }
      ident.module = env_global()->current_defmodule;
      ident.sym = sym;
      tag_set_name_if_null(result, &ident);
    }
    return frame;
  }
  return frame_binding_new(frame, sym, value);
}
