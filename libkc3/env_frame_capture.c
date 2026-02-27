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
#include "assert.h"
#include "env_frame_capture.h"
#include "frame.h"
#include "list.h"

s_frame * env_frame_capture_array (s_env *env, s_frame *frame,
                                   s_array *array)
{
  uw i = 0;
  assert(env);
  assert(frame);
  assert(array);
  if (array->tags && ! array->data) {
    while (i < array->count) {
      if (! env_frame_capture_tag(env, frame, array->tags + i))
        return NULL;
      i++;
    }
  }
  return frame;
}

s_frame * env_frame_capture_call (s_env *env, s_frame *frame,
                                  s_call *call)
{
  assert(env);
  assert(frame);
  assert(call);
  if (! env_frame_capture_ident(env, frame, &call->ident))
    return NULL;
  return env_frame_capture_list(env, frame, call->arguments);
}

s_frame * env_frame_capture_callable (s_env *env, s_frame *frame,
                                      s_callable *c)
{
  assert(env);
  assert(frame);
  assert(c);
  switch (c->type) {
  case CALLABLE_VOID:
  case CALLABLE_CFN:
    return frame;
  case CALLABLE_FN:
    return env_frame_capture_fn(env, frame, &c->data.fn);
  }
  err_puts("env_frame_capture_callable: unknown callable type");
  assert(! "env_frame_capture_callable: unknown callable type");
  return NULL;
}

s_frame * env_frame_capture_do_block (s_env *env, s_frame *frame,
                                      s_do_block *do_block)
{
  uw i = 0;
  assert(env);
  assert(frame);
  assert(do_block);
  while (i < do_block->count) {
    if (! env_frame_capture_tag(env, frame, do_block->tag + i))
      return NULL;
    i++;
  }
  return frame;
}

s_frame * env_frame_capture_fact (s_env *env, s_frame *frame,
                                  s_fact *fact)
{
  assert(env);
  assert(frame);
  assert(fact);
  if (! env_frame_capture_tag(env, frame, fact->subject) ||
      ! env_frame_capture_tag(env, frame, fact->predicate) ||
      ! env_frame_capture_tag(env, frame, fact->object))
    return NULL;
  return frame;
}

s_frame * env_frame_capture_fn (s_env *env, s_frame *frame,
                                s_fn *fn)
{
  s_fn_clause *clause;
  assert(env);
  assert(frame);
  assert(fn);
  clause = fn->clauses;
  while (clause) {
    if (! env_frame_capture_list(env, frame, clause->pattern))
      return NULL;
    if (! env_frame_capture_do_block(env, frame, &clause->algo))
      return NULL;
    clause = clause->next;
  }
  return frame;
}

s_frame * env_frame_capture_ident (s_env *env, s_frame *frame,
                                   s_ident *ident)
{
  s_tag *value;
  assert(env);
  assert(frame);
  assert(ident);
  if (ident->module)
    return frame;
  else {
    value = env_frames_get(env, ident->sym);
    if (! value)
      return frame;
  }
  if (! frame_replace(frame, ident->sym, value))
    return NULL;
  return frame;
}

s_frame * env_frame_capture_list (s_env *env, s_frame *frame,
                                  s_list *list)
{
  s_list *l = list;
  assert(env);
  assert(frame);
  while (l) {
    if (! env_frame_capture_tag(env, frame, &l->tag))
      return NULL;
    l = list_next(l);
  }
  return frame;
}

s_frame * env_frame_capture_map (s_env *env, s_frame *frame,
                                 s_map *map)
{
  uw i = 0;
  assert(env);
  assert(frame);
  assert(map);
  while (i < map->count) {
    if (! env_frame_capture_tag(env, frame, map->value + i))
      return NULL;
    i++;
  }
  return frame;
}

s_frame * env_frame_capture_quote (s_env *env, s_frame *frame,
                                   s_quote *quote)
{
  assert(env);
  assert(frame);
  assert(quote);
  if (! env_frame_capture_tag(env, frame, quote->tag))
    return NULL;
  return frame;
}

s_frame * env_frame_capture_struct (s_env *env, s_frame *frame,
                                    s_struct *s)
{
  uw i = 0;
  assert(env);
  assert(frame);
  assert(s);
  if (s->tag && ! s->data) {
    while (i < s->pstruct_type->map.count) {
      if (! env_frame_capture_tag(env, frame, s->tag + i))
        return NULL;
      i++;
    }
  }
  return frame;
}

s_frame * env_frame_capture_tag (s_env *env, s_frame *frame,
                                 s_tag *tag)
{
  assert(env);
  assert(frame);
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:
    return env_frame_capture_array(env, frame, &tag->data.td_array);
  case TAG_DO_BLOCK:
    return env_frame_capture_do_block(env, frame, &tag->data.td_do_block);
  case TAG_FACT:
    return env_frame_capture_fact(env, frame, &tag->data.td_fact);
  case TAG_MAP:
    return env_frame_capture_map(env, frame, &tag->data.td_map);
  case TAG_PCALL:
    return env_frame_capture_call(env, frame, tag->data.td_pcall);
  case TAG_PLIST:
    return env_frame_capture_list(env, frame, tag->data.td_plist);
  case TAG_PVAR:
    return env_frame_capture_var(env, frame, tag->data.td_pvar);
  case TAG_QUOTE:
    return env_frame_capture_quote(env, frame, &tag->data.td_quote);
  case TAG_PTUPLE:
    return env_frame_capture_tuple(env, frame, tag->data.td_ptuple);
  case TAG_UNQUOTE:
    return env_frame_capture_unquote(env, frame, &tag->data.td_unquote);
  case TAG_IDENT:
    return env_frame_capture_ident(env, frame, &tag->data.td_ident);
  case TAG_PCALLABLE:
    return env_frame_capture_callable(env, frame, tag->data.td_pcallable);
  case TAG_PSTRUCT:
    return env_frame_capture_struct(env, frame, tag->data.td_pstruct);
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_INTEGER:
  case TAG_PCOMPLEX:
  case TAG_PCOW:
  case TAG_PFACTS:
  case TAG_POINTER:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_SW:
  case TAG_TIME:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VOID:
    return frame;
  }
  err_puts("env_frame_capture_tag: unknown tag type");
  assert(! "env_frame_capture_tag: unknown tag type");
  return NULL;
}

s_frame * env_frame_capture_tuple (s_env *env, s_frame *frame,
                                   s_tuple *tuple)
{
  uw i = 0;
  assert(env);
  assert(frame);
  assert(tuple);
  while (i < tuple->count) {
    if (! env_frame_capture_tag(env, frame, tuple->tag + i))
      return NULL;
    i++;
  }
  return frame;
}

s_frame * env_frame_capture_unquote (s_env *env, s_frame *frame,
                                     s_unquote *unquote)
{
  assert(env);
  assert(frame);
  assert(unquote);
  if (! env_frame_capture_tag(env, frame, unquote->tag))
    return NULL;
  return frame;
}

s_frame * env_frame_capture_var (s_env *env, s_frame *frame,
                                 s_var *var)
{
  assert(env);
  assert(frame);
  assert(var);
  if (var->bound &&
      ! env_frame_capture_tag(env, frame, &var->tag))
    return NULL;
  return frame;
}
