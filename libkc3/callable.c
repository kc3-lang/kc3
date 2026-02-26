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
#include <unistd.h>
#include "alloc.h"
#include "assert.h"
#include "callable.h"
#include "cfn.h"
#include "env.h"
#include "fn.h"
#include "list.h"
#include "mutex.h"
#include "tag.h"

s8 callable_arity (const s_callable *callable)
{
  assert(callable);
  switch (callable->type) {
  case CALLABLE_CFN:
    return callable->data.cfn.arity;
  case CALLABLE_FN:
    return fn_arity(&callable->data.fn);
  case CALLABLE_VOID:
    return -1;
  }
  err_puts("callable_arity: unknown callable type");
  assert(! "callable_arity: unknown callable type");
  return -1;
}

void callable_delete (s_callable *callable)
{
  s_list *l;
  p_list *prev;
  assert(callable);
  if (env_cleaning(false)) {
    l = env_global()->freelist;
    while (l) {
      if (l->tag.type == TAG_PCALLABLE &&
          l->tag.data.pcallable == callable)
        break;
      l = list_next(l);
    }
    if (! l) {
      l = list_new_callable(callable, env_global()->freelist);
      if (l)
        env_global()->freelist = l;
    }
  }
#if HAVE_PTHREAD
  if (callable->mutex.ready)
    mutex_lock(&callable->mutex);
#endif
  if (env_global()->pass_by_copy)
    assert(callable->ref_count == 1);
  else {
    if (callable->ref_count <= 0) {
      err_puts("callable_delete: invalid ref count");
      assert(! "callable_delete: invalid ref count");
      goto clean;
    }
    if (--callable->ref_count > 0)
      goto clean;
  }
  switch (callable->type) {
  case CALLABLE_CFN: cfn_clean(&callable->data.cfn); break;
  case CALLABLE_FN:  fn_clean(&callable->data.fn);   break;
  case CALLABLE_VOID:                                break;
  }
#if HAVE_PTHREAD
  if (callable->mutex.ready) {
    mutex_unlock(&callable->mutex);
    mutex_clean(&callable->mutex);
  }
#endif
  if (env_cleaning(false)) {
    prev = &env_global()->freelist;
    while (*prev) {
      if ((*prev)->tag.type == TAG_PCALLABLE &&
          (*prev)->tag.data.pcallable == callable) {
        l = *prev;
        *prev = list_next(l);
        alloc_free(l);
        break;
      }
      prev = &(*prev)->next.data.plist;
    }
  }
  alloc_free(callable);
  return;
 clean:
#if HAVE_PTHREAD
  mutex_unlock(&callable->mutex);
#endif
  return;
}

s_callable * callable_new (void)
{
  s_callable *callable;
  if (! (callable = alloc(sizeof(s_callable))))
    return NULL;
  callable->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&callable->mutex);
#endif
  return callable;
}

s_callable * callable_new_copy (s_callable *src)
{
  s_callable *tmp;
  if (! (tmp = callable_new()))
    return NULL;
  tmp->type = src->type;
  switch (tmp->type) {
  case CALLABLE_CFN:
    if (! cfn_init_copy(&tmp->data.cfn, &src->data.cfn))
      goto ko;
    break;
  case CALLABLE_FN:
    if (! fn_init_copy(&tmp->data.fn, &src->data.fn))
      goto ko;
    break;
  case CALLABLE_VOID:
    break;
  }
  return tmp;
 ko:
  callable_delete(tmp);
  return NULL;
}

s_callable * callable_new_ref (s_callable *callable)
{
  assert(callable);
#if HAVE_PTHREAD
  mutex_lock(&callable->mutex);
#endif
  if (callable->ref_count <= 0) {
    err_puts("callable_new_ref: invalid reference count");
    assert(! "callable_new_ref: invalid reference count");
    abort();
  }
  callable->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&callable->mutex);
#endif
  return callable;
}

s_callable * callable_set_name (s_callable *callable,
                                const s_ident *name)
{
  assert(callable);
  assert(name);
  switch (callable->type) {
  case CALLABLE_VOID:
    err_puts("callable_set_name: uninitialized callable");
    assert(! "callable_set_name: uninitialized callable");
    return NULL;
  case CALLABLE_CFN:
    if (! cfn_set_name(&callable->data.cfn, name))
      return NULL;
    return callable;
  case CALLABLE_FN:
    if (! fn_set_name(&callable->data.fn, name))
      return NULL;
    return callable;
  }
  err_puts("callable_set_name: unknown callable type");
  assert(! "callable_set_name: unknown callable type");
  return NULL;
}

s_callable * callable_set_name_if_null (s_callable *callable,
                                        const s_ident *name)
{
  assert(callable);
  assert(name);
  switch (callable->type) {
  case CALLABLE_VOID:
    err_puts("callable_set_name_if_null: uninitialized callable");
    assert(! "callable_set_name_if_null: uninitialized callable");
    return NULL;
  case CALLABLE_CFN:
    if (! cfn_set_name_if_null(&callable->data.cfn, name))
      return NULL;
    return callable;
  case CALLABLE_FN:
    if (! fn_set_name_if_null(&callable->data.fn, name))
      return NULL;
    return callable;
  }
  err_puts("callable_set_name_if_null: unknown callable type");
  assert(! "callable_set_name_if_null: unknown callable type");
  return NULL;
}

s_callable * callable_set_special (s_callable *callable, bool special)
{
  assert(callable);
  switch (callable->type) {
  case CALLABLE_CFN:
    callable->data.cfn.special_operator = special;
    return callable;
  case CALLABLE_FN:
    callable->data.fn.special_operator = special;
    return callable;
  case CALLABLE_VOID:
    err_puts("callable_set_special: CALLABLE_TYPE_VOID");
    assert(! "callable_set_special: CALLABLE_TYPE_VOID");
    return NULL;
  }
  err_puts("callable_set_special: unknown callable type");
  assert(! "callable_set_special: unknown callable type");
  return NULL;
}
