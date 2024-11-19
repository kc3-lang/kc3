/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "tag.h"

void callable_delete (s_callable *callable)
{
  assert(callable);
  /* FIXME: lock callable lock. */
  if (callable->reference_count <= 0)
    goto clean;
  if (--callable->reference_count > 0)
    goto clean;
  switch (callable->type) {
  case CALLABLE_CFN: cfn_clean(&callable->data.cfn); break;
  case CALLABLE_FN:  fn_clean(&callable->data.fn);   break;
  case CALLABLE_VOID:                                break;
  }
  /* FIXME: unlock callable lock. */
  free(callable);
  return;
 clean:
  /* FIXME: unlock callable lock. */
  return;
}

s_callable * callable_new (void)
{
  s_callable *callable;
  if (! (callable = alloc(sizeof(s_callable))))
    return NULL;
  return callable;
}

s_callable * callable_new_copy (s_callable *src)
{
  s_callable *tmp;
  if (! (tmp = callable_new()))
    return NULL;
  tmp->type = src->type;
  switch (src->type) {
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
  tmp->reference_count = 1;
  return tmp;
 ko:
  free(tmp);
  return NULL;
}

s_callable * callable_new_ref (s_callable *callable)
{
  assert(callable);
  assert(callable->reference_count > 0);
  callable->reference_count++;
  return callable;
}

void p_callable_clean (p_callable *callable)
{
  callable_delete(*callable);
}

p_callable * p_callable_init (p_callable *callable)
{
  p_callable tmp;
  if (! (tmp = callable_new()))
    return NULL;
  *callable = tmp;
  return callable;
}
p_callable * p_callable_init_cast (p_callable *callable,
                                   const s_sym * const *type,
                                   const s_tag *tag)
{
  const s_sym *tag_type_;
  (void) callable;
  switch (tag->type) {
  case TAG_CALLABLE:
    err_puts("p_callable_init_cast: not implemented.");
    assert(! "p_callable_init_cast: not implemented.");
    return NULL;
  default:
    break;
  }
  err_write_1("p_callable_init_cast: cannot cast from ");
  tag_type(tag, &tag_type_);
  err_inspect_sym(&tag_type_);
  err_write_1(" to ");
  err_inspect_sym(type);
  err_write_1(" aka Callable.\n");
  assert(! "p_callable_init_cast: cannot cast to Callable.");
  return NULL;
}

p_callable * p_callable_init_copy (p_callable *callable,
                                   p_callable *src)
{
  p_callable tmp;
  if (! (tmp = callable_new_ref(*src)))
    return NULL;
  *callable = tmp;
  return callable;
}
