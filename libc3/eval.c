/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
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
#include "env.h"
#include "eval.h"
#include "frame.h"
#include "list.h"
#include "tag.h"

s_tag * eval_call_fn (s_env *env, s_call *call, s_tag *dest)
{
  s_arg *args;
  s_list *call_args;
  s_frame frame;
  s_fn *fn;
  s_tag tmp;
  assert(env);
  assert(call);
  assert(dest);
  fn = call->fn;
  assert(fn);
  frame_init(&frame, env->frame);
  frame.bindings = fn->bindings;
  args = fn->args;
  call_args = call->arguments;
  while (args) {
    if (! call_args) {
      assert(! "eval_call_fn: missing argument");
      errx(1, "eval_call_fn: missing argument");
      return NULL;
    }
    /* TODO: check type */
    eval_tag(env, &call_args->tag, &tmp);
    frame.bindings = binding_new(args->name, &call_args->tag,
                                 frame.bindings);
    args = args->next;
    call_args = list_next(call_args);
  }
  if (call_args) {
    assert(! "eval_call_fn: too many arguments");
    errx(1, "eval_call_fn: too many arguments");
    return NULL;
  }
  env->frame = &frame;
  eval_progn(env, fn->algo, dest);
  env->frame = frame_clean(&frame);
  return dest;
}

s_tag * eval_call_macro (s_env *env, s_call *call, s_tag *dest)
{
  s_tag *expanded;
  assert(env);
  assert(call);
  assert(dest);
  (void) env;
  (void) call;
  (void) expanded;
  return dest;
}

const s_tag * eval_ident (s_env *env, s_ident *ident)
{
  const s_tag *tag;
  assert(env);
  assert(ident);
  if (! (tag = frame_get(env->frame, ident->sym))) {
    assert(! "eval_ident: unbound variable");
    errx(1, "eval_ident: unbound variable");
  }
  return tag;
}

s_tag * eval_progn (s_env *env, s_list *program, s_tag *dest)
{
  s_tag tmp;
  assert(env);
  assert(program);
  assert(dest);
  while (program) {
    eval_tag(env, &program->tag, &tmp);
    program = list_next(program);
  }
  *dest = tmp;
  return dest;
}

s_tag * eval_tag (s_env *env, s_tag *tag, s_tag *dest)
{
  switch (tag->type.type) {
  case TAG_VOID: return tag_init_void(dest);
  case TAG_CALL:
    assert(! "eval_tag: invalid tag type: TAG_CALL");
    errx(1, "eval_tag: invalid tag type TAG_CALL");
    return NULL;
  case TAG_CALL_FN:
    return eval_call_fn(env, &tag->data.call, dest);
  case TAG_CALL_MACRO:
    return eval_call_macro(env, &tag->data.call, dest);
  case TAG_IDENT:
    return tag_copy(eval_ident(env, &tag->data.ident), dest);
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FN:
  case TAG_INTEGER:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_S8:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_U8:
  case TAG_VAR:
    return tag_copy(tag, dest);
  }
  assert(! "eval_tag: invalid tag");
  errx(1, "eval_tag: invalid tag");
  return NULL;
}
