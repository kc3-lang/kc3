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
#include <unistd.h>
#include "alloc.h"
#include "assert.h"
#include "callable.h"
#include "cfn.h"
#include "fn.h"
#include "mutex.h"
#include "tag.h"

void callable_delete (s_callable *callable)
{
  assert(callable);
#if HAVE_PTHREAD
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
  mutex_unlock(&callable->mutex);
  mutex_clean(&callable->mutex);
#endif
  free(callable);
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
  tmp->ref_count = 1;
  return tmp;
 ko:
  free(tmp);
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
