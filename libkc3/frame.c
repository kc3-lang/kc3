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
#include "alloc.h"
#include "assert.h"
#include "binding.h"
#include "fn.h"
#include "frame.h"
#include "ident.h"
#include "list.h"
#include "mutex.h"
#include "sym.h"
#include "tag.h"

s_frame * frame_binding_delete (s_frame *frame, const s_sym *name)
{
  s_binding **b;
  b = binding_find(&frame->bindings, name);
  *b = binding_delete(*b);
  return frame;
}

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
  if (! (tag = frame_binding_new(frame, name))) {
    err_puts("frame_binding_new_copy: frame_binding_new");
    assert(! "frame_binding_new_copy: frame_binding_new");
    return NULL;
  }
  if (value->type == TAG_VAR) {
    tag_init_var(tag, value->data.var.type);
    value->data.var.ptr = tag->data.var.ptr;
  }
  else {
    if (! tag_init_copy(tag, value)) {
      err_puts("frame_binding_new_copy: tag_init_copy");
      assert(! "frame_binding_new_copy: tag_init_copy");
      frame = frame_binding_delete(frame, name);
      return NULL;
    }
    if (tag->type == TAG_PCALLABLE &&
        tag->data.pcallable &&
        tag->data.pcallable->type == CALLABLE_FN)
      fn_set_name_if_null(&tag->data.pcallable->data.fn, NULL, name);
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
  tag_init_var(&b->value, &g_sym_Tag);
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
    frame_delete(frame->fn_frame);
  return next;
}

s_frame * frame_delete (s_frame *frame)
{
  s_frame *next = NULL;
  if (frame) {
#if HAVE_PTHREAD
    mutex_lock(&frame->mutex);
#endif
    next = frame->next;
    if (frame->ref_count <= 0) {
      err_puts("frame_delete: invalid reference count");
      assert(! "frame_delete: invalid reference count");
      abort();
    }
    if (--frame->ref_count) {
#if HAVE_PTHREAD
      mutex_unlock(&frame->mutex);
#endif
      return next;
    }
#if HAVE_PTHREAD
    mutex_unlock(&frame->mutex);
#endif
    frame_clean(frame);
    free(frame);
  }
  return next;
}

void frame_delete_all (s_frame *frame)
{
  s_frame *f;
  bool must_clean = true;
  f = frame;
  while (f && must_clean) {
#if HAVE_PTHREAD
    mutex_lock(&f->mutex);
#endif
    must_clean = (f->ref_count == 1);
#if HAVE_PTHREAD
    mutex_unlock(&f->mutex);
#endif
    f = frame_delete(f);
  }
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
  f = frame;
  while (f) {
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
    f = f->next;
  }
  f = frame;
  while (f) {
    if (f->fn_frame) {
      result = frame_get_w(f->fn_frame, sym);
      if (result)
        return result;
    }
    f = f->next;
  }
  return NULL;
}

s_frame * frame_init (s_frame *frame, s_frame *next, s_frame *fn_frame)
{
  s_frame tmp = {0};
  assert(frame);
  tmp.next = next;
  tmp.fn_frame = frame_new_ref(fn_frame);
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
  tmp.ref_count = 1;
  *frame = tmp;
  return frame;
}

s_frame * frame_new (s_frame *next, s_frame *fn_frame)
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
    *f = frame_new(NULL, frame_new_copy(s->fn_frame));
    frame_delete((*f)->fn_frame); // decrement ref_count
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

s_frame * frame_new_ref (s_frame *src)
{
  if (src) {
#if HAVE_PTHREAD
    mutex_lock(&src->mutex);
#endif
    if (src->ref_count <= 0) {
      err_puts("frame_new_ref: invalid reference count");
      assert(! "frame_new_ref: invalid reference count");
      abort();
    }
    src->ref_count++;
#if HAVE_PTHREAD
    mutex_unlock(&src->mutex);
#endif
  }
  return src;
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
    else {
      if (! tag_init_copy(result, value)) {
        err_puts("frame_replace: tag_init_copy");
        assert(! "frame_replace: tag_init_copy");
        return NULL;
      }
      if (result->type == TAG_PCALLABLE &&
          result->data.pcallable &&
          result->data.pcallable->type == CALLABLE_FN)
        fn_set_name_if_null(&result->data.pcallable->data.fn, NULL,
                            sym);
    }
    return frame;
  }
  return frame_binding_new_copy(frame, sym, value);
}
