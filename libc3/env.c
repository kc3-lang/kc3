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
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "env.h"
#include "error_handler.h"
#include "frame.h"
#include "list.h"
#include "str.h"
#include "tag.h"

void env_clean (s_env *env)
{
  assert(env);
  frame_delete_all(env->frame);
  error_handler_delete_all(env->error_handler);
}

void env_error_f (s_env *env, const char *fmt, ...)
{
  va_list ap;
  s_tag tag;
  assert(env);
  assert(fmt);
  va_start(ap, fmt);
  tag.type.type = TAG_STR;
  str_init_vf(&tag.data.str, fmt, ap);
  va_end(ap);
  env_error_tag(env, &tag);
}

void env_error_tag (s_env *env, const s_tag *tag)
{
  s_error_handler *error_handler;
  assert(env);
  assert(tag);
  error_handler = env->error_handler;
  if (error_handler) {
    tag_copy(tag, &error_handler->tag);
    error_handler->backtrace = env->backtrace;
    env_longjmp(env, &error_handler->jmp_buf);
    /* never reached */
    return;
  }
  if (buf_file_is_open(&env->err)) {
    buf_write_1(&env->err, "error: ");
    buf_inspect_tag(&env->err, tag);
    buf_write_1(&env->err, "\n");
    return;
  }
}

s_tag * env_eval_call_fn (s_env *env, s_call *call, s_tag *dest)
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
      assert(! "env_eval_call_fn: missing argument");
      errx(1, "env_eval_call_fn: missing argument");
      return NULL;
    }
    /* TODO: check type */
    env_eval_tag(env, &call_args->tag, &tmp);
    frame.bindings = binding_new(args->name, &call_args->tag,
                                 frame.bindings);
    args = args->next;
    call_args = list_next(call_args);
  }
  if (call_args) {
    assert(! "env_eval_call_fn: too many arguments");
    errx(1, "env_eval_call_fn: too many arguments");
    return NULL;
  }
  env->frame = &frame;
  env_eval_progn(env, fn->algo, dest);
  env->frame = frame_clean(&frame);
  return dest;
}

s_tag * env_eval_call_macro (s_env *env, s_call *call, s_tag *dest)
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

const s_tag * env_eval_ident (s_env *env, s_ident *ident)
{
  const s_tag *tag;
  assert(env);
  assert(ident);
  if (! (tag = frame_get(env->frame, ident->sym))) {
    assert(! "env_eval_ident: unbound variable");
    errx(1, "env_eval_ident: unbound variable");
  }
  return tag;
}

s_tag * env_eval_progn (s_env *env, s_list *program, s_tag *dest)
{
  s_tag tmp;
  assert(env);
  assert(program);
  assert(dest);
  while (program) {
    env_eval_tag(env, &program->tag, &tmp);
    program = list_next(program);
  }
  *dest = tmp;
  return dest;
}

s_tag * env_eval_tag (s_env *env, s_tag *tag, s_tag *dest)
{
  switch (tag->type.type) {
  case TAG_VOID: return tag_init_void(dest);
  case TAG_CALL:
    assert(! "env_eval_tag: invalid tag type: TAG_CALL");
    errx(1, "env_eval_tag: invalid tag type TAG_CALL");
    return NULL;
  case TAG_CALL_FN:
    return env_eval_call_fn(env, &tag->data.call, dest);
  case TAG_CALL_MACRO:
    return env_eval_call_macro(env, &tag->data.call, dest);
  case TAG_IDENT:
    return tag_copy(env_eval_ident(env, &tag->data.ident), dest);
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
  assert(! "env_eval_tag: invalid tag");
  errx(1, "env_eval_tag: invalid tag");
  return NULL;
}

s_env * env_init (s_env *env)
{
  assert(env);
  env->frame = NULL;
  env->error_handler = NULL;
  return env;
}

void env_longjmp (s_env *env, jmp_buf *jmp_buf)
{
  if (env->unwind_protect && *jmp_buf > env->unwind_protect->buf) {
    s_unwind_protect *unwind_protect = env->unwind_protect;
    while (unwind_protect->next && *jmp_buf > unwind_protect->next->buf) {
      unwind_protect->jmp = &unwind_protect->next->buf;
      unwind_protect = unwind_protect->next;
    }
    unwind_protect->jmp = jmp_buf;
    longjmp(env->unwind_protect->buf, 1);
  }
  longjmp(*jmp_buf, 1);
}

void env_pop_error_handler (s_env *env)
{
  if (env->error_handler)
    env->error_handler = error_handler_delete(env->error_handler);
}

void env_pop_unwind_protect (s_env *env)
{
  if (env->unwind_protect)
    env->unwind_protect = env->unwind_protect->next;
}

void env_push_error_handler (s_env *env, s_error_handler *error_handler)
{
  tag_init_void(&error_handler->tag);
  error_handler->next = env->error_handler;
  env->error_handler = error_handler;
}

void env_push_unwind_protect (s_env *env,
                              s_unwind_protect *unwind_protect)
{
  unwind_protect->next = env->unwind_protect;
  env->unwind_protect = unwind_protect;
}

s_tag * env_unwind_protect (s_env *env, s_tag *protected, s_list *cleanup,
                            s_tag *dest)
{
  s_tag tmp;
  s_unwind_protect unwind_protect;
  assert(env);
  assert(protected);
  if (setjmp(unwind_protect.buf)) {
    env_pop_unwind_protect(env);
    env_eval_progn(env, cleanup, &tmp);
    longjmp(*unwind_protect.jmp, 1);
  }
  env_eval_tag(env, protected, dest);
  env_pop_unwind_protect(env);
  env_eval_progn(env, cleanup, &tmp);
  return dest;
}
