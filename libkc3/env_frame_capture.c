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
#include "assert.h"
#include "env_frame_capture.h"
#include "list.h"

s_frame * env_frame_capture_tag (s_env *env, s_frame *frame,
                                 s_tag *tag)
{
  assert(env);
  assert(frame);
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:
    return env_frame_capture_array(env, frame, &tag->data.array);
  case TAG_DO_BLOCK:
    return env_frame_capture_do_block(env, frame, &tag->data.do_block);
  case TAG_CALL:
    return env_frame_capture_call(env, frame, &tag->data.call);
  case TAG_COW:
    return env_frame_capture_cow(env, frame, tag->data.cow);
  case TAG_FACT:
    return env_frame_capture_fact(env, frame, &tag->data.fact);
  case TAG_LIST:
    return env_frame_capture_list(env, frame, tag->data.list);
  case TAG_MAP:
    return env_frame_capture_map(env, frame, &tag->data.map);
  case TAG_PTAG:
    return env_frame_capture_tag(env, frame, tag->data.ptag);
  case TAG_QUOTE:
    return env_frame_capture_quote(env, frame, &tag->data.quote);
  case TAG_TUPLE:
    return env_frame_capture_tuple(env, frame, &tag->data.tuple);
  case TAG_UNQUOTE:
    return env_frame_capture_unquote(env, frame, &tag->data.unquote);
  case TAG_VAR:
    return env_frame_capture_var(env, frame, &tag->data.var);
  case TAG_IDENT:
    return env_frame_capture_ident(env, frame, &tag->data.ident);
  case TAG_PCALLABLE:
    return env_frame_capture_callable(env, frame, tag->data.pcallable);
  case TAG_PSTRUCT:
    return env_frame_capture_struct(env, frame, tag->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return env_frame_capture_struct_type(env, frame,
                                         tag->data.pstruct_type);
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_COMPLEX:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_INTEGER:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_SW:
  case TAG_SYM:
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
    clause = clause->next_clause;
  }
  return frame;
}

s_frame * env_frame_capture_list (s_env *env, s_frame *frame,
                                  s_list *list)
{
  s_list *l = list;
  assert(env);
  assert(frame);
  assert(list);
  while (l) {
    if (! env_frame_capture_tag(env, frame, &l->tag))
      return NULL;
    l = list_next(l);
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
  if (var->ptr &&
      ! env_frame_capture_tag(env, frame, var->ptr))
    return NULL;
  return frame;
}
