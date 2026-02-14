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
#include <stdlib.h>
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "block.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "complex.h"
#include "cow.h"
#include "data.h"
#include "do_block.h"
#include "env.h"
#include "env_eval.h"
#include "env_eval_quote.h"
#include "fn.h"
#include "frame.h"
#include "ident.h"
#include "list.h"
#include "map.h"
#include "ops.h"
#include "pcallable.h"
#include "pstruct.h"
#include "pvar.h"
#include "struct.h"
#include "sw.h"
#include "tag.h"
#include "time.h"
#include "tuple.h"
#include "var.h"

// TODO: unwind_protect
bool env_eval_quote (s_env *env, s_quote *quote, s_tag *dest)
{
  bool r;
  assert(env);
  assert(quote);
  assert(dest);
  env->quote_level++;
  r = env_eval_quote_tag(env, quote->tag, dest);
  env->quote_level--;
  return r;
}

// TODO: unwind_protect
bool env_eval_quote_array (s_env *env, s_array *array,
                           s_tag *dest)
{
  uw i;
  s_tag *tag;
  s_array tmp = {0};
  s_tag  *tmp_tag;
  assert(env);
  assert(array);
  assert(dest);
  if (! array->dimension_count || array->data || ! array->tags) {
    if (! tag_init_array_copy(dest, array))
      return false;
    return true;
  }
  if (! array_init_copy_shallow(&tmp, array))
    return false;
  tag = array->tags;
  tmp.tags = alloc(tmp.count * sizeof(s_tag));
  if (! tmp.tags)
    goto ko;
  tmp_tag = tmp.tags;
  i = 0;
  while (i < array->count) {
    if (! env_eval_quote_tag(env, tag, tmp_tag))
      goto ko;
    tag++;
    tmp_tag++;
    i++;
  }
  dest->type = TAG_ARRAY;
  dest->data.array = tmp;
  return true;
 ko:
  array_clean(&tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_do_block (s_env *env, s_do_block *do_block,
                              s_tag *dest)
{
  uw i = 0;
  s_do_block tmp = {0};
  assert(env);
  assert(do_block);
  assert(dest);
  do_block_init(&tmp, do_block->count);
  while (i < do_block->count) {
    if (! env_eval_quote_tag(env, do_block->tag + i, tmp.tag + i))
      goto ko;
    i++;
  }
  dest->type = TAG_DO_BLOCK;
  dest->data.do_block = tmp;
  return true;
 ko:
  do_block_clean(&tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_call (s_env *env, s_call *call, s_tag *dest)
{
  s_list *arg;
  s_call   tmp = {0};
  s_list **tmp_arg_last;
  assert(call);
  assert(dest);
  call_init(&tmp);
  if (! ident_init_copy(&tmp.ident, &call->ident)) {
    call_clean(&tmp);
    return false;
  }
  tmp_arg_last = &tmp.arguments;
  arg = call->arguments;
  while (arg) {
    *tmp_arg_last = list_new(NULL);
    if (! env_eval_quote_tag(env, &arg->tag, &(*tmp_arg_last)->tag))
      goto ko;
    tmp_arg_last = &(*tmp_arg_last)->next.data.plist;
    arg = list_next(arg);
  }
  if (call->pcallable &&
      ! pcallable_init_copy(&tmp.pcallable, &call->pcallable))
    goto ko;
  dest->type = TAG_PCALL;
  if (! (dest->data.pcall = alloc(sizeof(s_call))))
    goto ko;
  *dest->data.pcall = tmp;
  return true;
 ko:
  call_clean(&tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_complex (s_env *env, s_complex *c,
                             s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(c);
  assert(dest);
  tmp.type = TAG_PCOMPLEX;
  tmp.data.pcomplex = complex_new();
  if (! tmp.data.pcomplex)
    return false;
  if (! env_eval_quote_tag(env, &c->x, &tmp.data.pcomplex->x) ||
      ! env_eval_quote_tag(env, &c->y, &tmp.data.pcomplex->y)) {
    complex_delete(tmp.data.pcomplex);
    return false;
  }
  *dest = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_quote_cow (s_env *env, s_cow *cow,
                         s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(cow);
  assert(dest);
  tmp.type = TAG_PCOW;
  tmp.data.pcow = cow_new(cow->type);
  if (! tmp.data.pcow)
    return false;
  if (! env_eval_quote_tag(env, cow_read_only(cow),
                           cow_read_write(tmp.data.pcow))) {
    cow_delete(tmp.data.pcow);
    return false;
  }
  cow_freeze(tmp.data.pcow);
  *dest = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_quote_list (s_env *env, s_list *list, s_tag *dest)
{
  s_list *next = NULL;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(env);
  assert(dest);
  while (list) {
    *tail = list_new(NULL);
    if (! *tail)
      goto ko;
    if (! env_eval_quote_tag(env, &list->tag, &(*tail)->tag))
      goto ko;
    next = list_next(list);
    if (! next)
      if (! env_eval_quote_tag(env, &list->next, &(*tail)->next))
        goto ko;
    tail = &(*tail)->next.data.plist;
    list = next;
  }
  dest->type = TAG_PLIST;
  dest->data.plist = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_map (s_env *env, s_map *map, s_tag *dest)
{
  s_map tmp = {0};
  uw i = 0;
  assert(env);
  assert(map);
  assert(dest);
  if (! map_init(&tmp, map->count))
    return false;
  while (i < tmp.count) {
    if (! env_eval_quote_tag(env, map->key + i, tmp.key + i) ||
        ! env_eval_quote_tag(env, map->value + i, tmp.value + i))
      goto ko;
    i++;
  }
  dest->type = TAG_MAP;
  dest->data.map = tmp;
  return true;
 ko:
  map_clean(&tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_quote (s_env *env, s_quote *quote, s_tag *dest)
{
  bool r;
  s_quote tmp = {0};
  assert(env);
  assert(quote);
  assert(dest);
  tmp.tag = tag_new();
  if (! tmp.tag)
    return false;
  env->quote_level++;
  r = env_eval_quote_tag(env, quote->tag, tmp.tag);
  env->quote_level--;
  if (! r)
    return false;
  dest->type = TAG_QUOTE;
  dest->data.quote = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_quote_struct (s_env *env, s_struct *s, s_tag *dest)
{
  uw i;
  p_struct_type st;
  s_tag tmp = {0};
  assert(env);
  assert(s);
  assert(dest);
  tmp.type = TAG_PSTRUCT;
  if (s->data || ! s->tag) {
    if (! pstruct_init_copy(&tmp.data.pstruct, &s))
      return false;
    *dest = tmp;
    return true;
  }
  pstruct_init_with_type(&tmp.data.pstruct, s->pstruct_type);
  st = tmp.data.pstruct->pstruct_type;
  tmp.data.pstruct->tag = alloc(st->map.count * sizeof(s_tag));
  if (! tmp.data.pstruct->tag)
    return false;
  i = 0;
  while (i < st->map.count) {
    if (! env_eval_quote_tag(env, s->tag + i,
                             tmp.data.pstruct->tag + i))
      goto ko;
    i++;
  }
  *dest = tmp;
  return true;
 ko:
  return false;
}

// Like tag_init_copy excepted that the unquote parts get evaluated.
bool env_eval_quote_tag (s_env *env, s_tag *tag, s_tag *dest)
{
  assert(env);
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    return env_eval_quote_array(env, &tag->data.array, dest);
  case TAG_DO_BLOCK:
    return env_eval_quote_do_block(env, &tag->data.do_block, dest);
  case TAG_MAP:
    return env_eval_quote_map(env, &tag->data.map, dest);
  case TAG_PCALL:
    return env_eval_quote_call(env, tag->data.pcall, dest);
  case TAG_PCOMPLEX:
    return env_eval_quote_complex(env, tag->data.pcomplex, dest);
  case TAG_PCOW:
    return env_eval_quote_cow(env, tag->data.pcow, dest);
  case TAG_PLIST:
    return env_eval_quote_list(env, tag->data.plist, dest);
  case TAG_PSTRUCT:
    return env_eval_quote_struct(env, tag->data.pstruct, dest);
  case TAG_QUOTE:
    return env_eval_quote_quote(env, &tag->data.quote, dest);
  case TAG_TIME:
    return env_eval_quote_time(env, &tag->data.time, dest);
  case TAG_PTUPLE:
    return env_eval_quote_tuple(env, tag->data.ptuple, dest);
  case TAG_UNQUOTE:
    return env_eval_quote_unquote(env, &tag->data.unquote, dest);
  case TAG_VOID:
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
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_INTEGER:
  case TAG_PCALLABLE:
  case TAG_PFACTS:
  case TAG_POINTER:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_PVAR:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    if (! tag_init_copy(dest, tag))
      return false;
    return true;
  }
  err_puts("env_eval_quote_tag: invalid tag type");
  assert(! "env_eval_quote_tag: invalid tag type");
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_time (s_env *env, s_time *time, s_tag *dest)
{
  s_time tmp = {0};
  assert(env);
  assert(time);
  assert(dest);
  if (time->tag) {
    if (! time_allocate(&tmp))
      return false;
    if (! env_eval_quote_tag(env, time->tag, tmp.tag)) {
      err_puts("env_eval_quote_time: env_eval_quote_tag: tv_sec");
      assert(! "env_eval_quote_time: env_eval_quote_tag: tv_sec");
      free(tmp.tag);
      return false;
    }
    if (! env_eval_quote_tag(env, time->tag + 1, tmp.tag + 1)) {
      err_puts("env_eval_quote_time: env_eval_quote_tag: tv_nsec");
      assert(! "env_eval_quote_time: env_eval_quote_tag: tv_nsec");
      tag_clean(tmp.tag);
      free(tmp.tag);
      return false;
    }
  }
  else {
    tmp.tv_sec = time->tv_sec;
    tmp.tv_nsec = time->tv_nsec;
    tmp.tag = NULL;
  }
  dest->type = TAG_TIME;
  dest->data.time = tmp;
  return true;
}

bool env_eval_quote_tuple (s_env *env, s_tuple *tuple, s_tag *dest)
{
  uw i = 0;
  p_tuple tmp;
  assert(env);
  assert(tuple);
  assert(dest);
  tmp = tuple_new(tuple->count);
  if (! tmp)
    return false;
  while (i < tuple->count) {
    if (! env_eval_quote_tag(env, tuple->tag + i, tmp->tag + i))
      goto ko;
    i++;
  }
  dest->type = TAG_PTUPLE;
  dest->data.ptuple = tmp;
  return true;
 ko:
  tuple_delete(tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_quote_unquote (s_env *env, s_unquote *unquote,
                             s_tag *dest)
{
  bool r;
  s_tag tmp = {0};
  assert(env);
  assert(unquote);
  assert(dest);
  if (env->unquote_level >= env->quote_level) {
    err_puts("env_eval_quote_unquote: unquote outside of a quote");
    assert(! "env_eval_quote_unquote: unquote outside of a quote");
    return false;
  }
  env->unquote_level++;
  if (env->unquote_level == 1) {
    if (env->quote_level == 1)
      r = env_eval_tag(env, unquote->tag, &tmp);
    else
      r = tag_init_copy(&tmp, unquote->tag) ? true : false;
  }
  else
    r = tag_init_unquote_copy(&tmp, unquote) ? true : false;
  env->unquote_level--;
  if (! r) {
    tag_clean(&tmp);
    return false;
  }
  *dest = tmp;
  return true;
}
