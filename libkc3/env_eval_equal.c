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
#include "env_eval_equal.h"
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
bool env_eval_equal_cow (s_env *env, bool macro, s_cow *a,
                         s_cow *b, s_cow **dest)
{
  s8 r;
  p_cow tmp = NULL;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  (void) env;
  if ((r = compare_sym(a->type, b->type))) {
    err_puts("env_eval_equal_cow: type mismatch");
    assert(! "env_eval_equal_cow: type mismatch");
    return false;
  }
  if ((r = compare_tag(cow_read_only(a), cow_read_only(b)))) {
    err_puts("env_eval_equal_cow: value mismatch");
    assert(! "env_eval_equal_cow: value mismatch");
    return false;
  }
  if (macro) {
    tmp = cow_new_tag_copy(a->type, cow_read_only(a));
    if (! tmp)
      return false;
  }
  else
    if (! env_eval_pcow(env, &a, &tmp))
      return false;
  cow_freeze(tmp);
  *dest = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_equal_list (s_env *env, bool macro, s_list *a,
                          s_list *b, s_list **dest)
{
  s_list *a_next;
  s_list *b_next;
  s_list *tmp = NULL;
  s_list **t;
  assert(env);
  assert(dest);
  t = &tmp;
  while (1) {
    if (! a && ! b) {
      *t = NULL;
      goto ok;
    }
    if (! a)
      goto ko;
    if (! b)
      goto ko;
    *t = list_new(NULL);
    if (! *t)
      goto ko;
    if (! env_eval_equal_tag(env, macro, &a->tag, &b->tag,
                             &(*t)->tag))
      goto ko;
    a_next = list_next(a);
    b_next = list_next(b);
    if (! a_next || ! b_next) {
      if (! env_eval_equal_tag(env, macro, &a->next, &b->next,
                               &(*t)->next))
        goto ko;
      goto ok;
    }
    a = a_next;
    b = b_next;
    if (dest)
      t = &(*t)->next.data.plist;
  }
 ok:
  *dest = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

// TODO: unwind protect
bool env_eval_equal_map (s_env *env, bool macro, s_map *a,
                         s_map *b, s_map *dest)
{
  s_map *c;
  uw i;
  uw j;
  s_tag tmp = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (! a->count) {
    map_init_copy(dest, b);
    return true;
  }
  if (! b->count) {
    map_init_copy(dest, a);
    return true;
  }
  if (a->count > b->count) {
    c = a;
    a = b;
    b = c;
  }
  i = 0;
  while (i < a->count) {
    j = 0;
    while (j < b->count) {
      if (! compare_tag(a->key + i, b->key + j)) {
        if (! env_eval_equal_tag(env, macro, a->value + i, b->value + j,
                                 &tmp)) {
          return false;
        }
        tag_clean(&tmp);
        goto next;
      }
      j++;
    }
    return false;
  next:
    i++;
  }
  map_init_copy(dest, b);
  return true;
}

// TODO: unwind_protect
bool env_eval_equal_tag (s_env *env, bool macro, s_tag *a,
                         s_tag *b, s_tag *dest)
{
  bool is_unbound_a;
  bool is_unbound_b;
  bool is_var_a = false;
  bool is_var_b = false;
  s_tag tmp_a;
  s_tag tmp_b;
  s_tag *var_a = NULL;
  s_tag *var_b = NULL;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  tag_init_void(&tmp_a);
  tag_init_void(&tmp_b);
  if (a->type == TAG_PVAR) {
    var_a = &a->data.pvar->tag;
    if (a->data.pvar->bound) {
      while (a->data.pvar->bound) {
        a = var_a;
        var_a = &a->data.pvar->tag;
      }
    }
    else
      is_var_a = true;
  }
  if (b->type == TAG_PVAR) {
    var_b = &b->data.pvar->tag;
    if (b->data.pvar->bound) {
      while (b->data.pvar->bound) {
        b = var_b;
        var_b = &b->data.pvar->tag;
      }
    }
    else
      is_var_b = true;
  }
  is_unbound_a = a->type == TAG_IDENT;
  is_unbound_b = ! macro && (b->type == TAG_IDENT);
  if (is_unbound_a && is_unbound_b) {
    err_write_1("env_eval_equal_tag: unbound equal on both sides: ");
    err_inspect_ident(&a->data.ident);
    err_write_1(" = ");
    err_inspect_ident(&b->data.ident);
    err_write_1(".\nTo match an existing variable please use the"
                " pin operator, e.g.: ");
    err_inspect_ident(&a->data.ident);
    err_write_1(" = ^ ");
    err_inspect_ident(&b->data.ident);
    err_write_1("\n");
    return false;
  }
  if (is_unbound_a || is_var_a) {
    if (macro)
      tag_init_copy(dest, b);
    else
      env_eval_tag(env, b, dest);
    if (is_var_a) {
      if (! var_set(a->data.pvar, dest))
        return false;
    }
    else {
      if (! frame_replace(env->frame, a->data.ident.sym, dest))
        return false;
    }
    return true;
  }
  if (is_unbound_b || is_var_b) {
    if (macro)
      tag_init_copy(dest, a);
    else
      env_eval_tag(env, a, dest);
    if (is_var_b) {
      if (! var_set(b->data.pvar, dest))
        return false;
    }
    else {
      if (! frame_replace(env->frame, b->data.ident.sym, dest))
        return false;
    }
    return true;
  }
  if (! macro &&
      a->type == TAG_PCALL) {
    if (! env_eval_tag(env, a, &tmp_a))
      return false;
    if (! env_eval_equal_tag(env, macro, &tmp_a, b, dest)) {
      tag_clean(&tmp_a);
      return false;
    }
    tag_clean(&tmp_a);
    return true;
  }
  if (! macro &&
      b->type == TAG_PCALL) {
    if (! env_eval_tag(env, b, &tmp_b))
      return false;
    if (! env_eval_equal_tag(env, macro, a, &tmp_b, dest)) {
      tag_clean(&tmp_b);
      return false;
    }
    tag_clean(&tmp_b);
    return true;
  }
  a = tag_resolve_cow(a);
  b = tag_resolve_cow(b);
  switch (a->type) {
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
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    switch (b->type) {
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
    case TAG_RATIO:
    case TAG_S8:
    case TAG_S16:
    case TAG_S32:
    case TAG_S64:
    case TAG_SW:
    case TAG_U8:
    case TAG_U16:
    case TAG_U32:
    case TAG_U64:
    case TAG_UW:
      if (compare_tag(a, b)) {
        return false;
      }
      tag_init_copy(dest, a);
      return true;
    default:
      break;
    }
  default:
    break;
  }
  if (a->type != b->type) {
    if (! env->silence_errors) {
      err_write_1("env_eval_equal_tag: type mismatch: ");
      err_inspect_tag(a);
      err_write_1(" != ");
      err_inspect_tag(b);
      err_write_1("\n");
      err_stacktrace();
    }
    return false;
  }
  switch (a->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_MAP:
    dest->type = TAG_MAP;
    return env_eval_equal_map(env, macro, &a->data.map, &b->data.map,
                              &dest->data.map);
  case TAG_PLIST:
    tag_init_plist(dest, NULL);
    return env_eval_equal_list(env, macro, a->data.plist, b->data.plist,
                               &dest->data.plist);
  /*
  case TAG_PSTRUCT:
    dest->type = TAG_PSTRUCT;
    return env_eval_equal_struct(env, macro, a->data.pstruct,
                                 b->data.pstruct, &dest->data.pstruct);
  */
  case TAG_TIME:
    dest->type = TAG_TIME;
    return env_eval_equal_time(env, macro, &a->data.time,
                               &b->data.time, &dest->data.time);
  case TAG_TUPLE:
    dest->type = TAG_TUPLE;
    return env_eval_equal_tuple(env, macro, &a->data.tuple,
                                &b->data.tuple, &dest->data.tuple);
  case TAG_ARRAY:
  case TAG_DO_BLOCK:
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PCALL:
  case TAG_PCALLABLE:
  case TAG_PFACTS:
  case TAG_POINTER:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_PVAR:
  case TAG_QUOTE:
  case TAG_STR:
    if (compare_tag(a, b)) {
      if (false) {
        err_puts("env_eval_equal_tag: value mismatch");
        err_stacktrace();
      }
      return false;
    }
    tag_init_copy(dest, a);
    return true;
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
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UNQUOTE:
  case TAG_UW:
    break;
  }
  err_puts("env_eval_equal_tag: invalid tag");
  assert(! "env_eval_equal_tag: invalid tag");
  abort();
  return false;
}

// TODO: unwind_protect
bool env_eval_equal_time (s_env *env, bool macro, s_time *a,
                          s_time *b, s_time *dest)
{
  s_tag *a2;
  s_tag  a_tag[2] = {0};
  s_tag *b2;
  s_tag  b_tag[2] = {0};
  const s_sym *sym_Sw = &g_sym_Sw;
  s_time tmp = {0};
  s_tag  tmp_tag[2] = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (! (a2 = a->tag)) {
    a2 = a_tag;
    a2[0].type = TAG_SW;
    a2[0].data.sw = a->tv_sec;
    a2[1].type = TAG_SW;
    a2[1].data.sw = a->tv_nsec;
  }
  if (! (b2 = b->tag)) {
    b2 = b_tag;
    b2[0].type = TAG_SW;
    b2[0].data.sw = b->tv_sec;
    b2[1].type = TAG_SW;
    b2[1].data.sw = b->tv_nsec;
  }
  if (! env_eval_equal_tag(env, macro, a2, b2, tmp_tag)) {
    return false;
  }
  if (! env_eval_equal_tag(env, macro, a2 + 1, b2 + 1, tmp_tag + 1)) {
    tag_clean(tmp_tag);
    return false;
  }
  if (! sw_init_cast(&tmp.tv_sec, &sym_Sw, tmp_tag)) {
    err_puts("env_eval_equal_time: sw_init_cast (tv_sec)");
    assert(! "env_eval_equal_time: sw_init_cast (tv_sec)");
    tag_clean(tmp_tag + 1);
    tag_clean(tmp_tag);
    return false;
  }
  if (! sw_init_cast(&tmp.tv_nsec, &sym_Sw, tmp_tag + 1)) {
    err_puts("env_eval_equal_time: sw_init_cast (tv_nsec)");
    assert(! "env_eval_equal_time: sw_init_cast (tv_nsec)");
    tag_clean(tmp_tag + 1);
    tag_clean(tmp_tag);
    return false;
  }
  *dest = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_equal_tuple (s_env *env, bool macro, s_tuple *a,
                           s_tuple *b, s_tuple *dest)
{
  uw i;
  s_tuple tmp = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (a->count != b->count) {
    err_puts("env_eval_equal_tuple: tuple arity mismatch");
    assert(! "env_eval_equal_tuple: tuple arity mismatch");
    return false;
  }
  tuple_init(&tmp, a->count);
  i = 0;
  while (i < a->count) {
    if (! env_eval_equal_tag(env, macro, a->tag + i, b->tag + i,
                             tmp.tag + i)) {
      if (false) {
        err_write_1("env_eval_equal_tuple: tuple tag mismatch index ");
        err_inspect_uw(i);
        err_write_1("\n");
        assert(! "env_eval_equal_tuple: tuple tag mismatch");
      }
      tuple_clean(&tmp);
      return false;
    }
    i++;
  }
  *dest = tmp;
  return true;
}
