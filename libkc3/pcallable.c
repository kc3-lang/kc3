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
#include "pcallable.h"
#include "tag.h"

void pcallable_clean (p_callable *callable)
{
  bool nullify;
  if (*callable) {
    nullify = (*callable)->ref_count == 1;
    callable_delete(*callable);
    if (nullify)
      *callable = NULL;
  }
}

p_callable * pcallable_init (p_callable *callable)
{
  p_callable tmp;
  if (! (tmp = callable_new()))
    return NULL;
  *callable = tmp;
  return callable;
}

p_callable * pcallable_init_cast (p_callable *callable,
                                  const s_sym * const *type,
                                  const s_tag *tag)
{
  const s_sym *tag_type_;
  (void) callable;
  switch (tag->type) {
  case TAG_PCALLABLE:
    err_puts("pcallable_init_cast: not implemented.");
    assert(! "pcallable_init_cast: not implemented.");
    return NULL;
  default:
    break;
  }
  err_write_1("pcallable_init_cast: cannot cast from ");
  tag_type(tag, &tag_type_);
  err_inspect_sym(&tag_type_);
  err_write_1(" to ");
  err_inspect_sym(type);
  err_write_1(" aka Callable.\n");
  assert(! "pcallable_init_cast: cannot cast to Callable.");
  return NULL;
}

p_callable * pcallable_init_copy (p_callable *callable,
                                  p_callable *src)
{
  p_callable tmp;
  if (! (tmp = callable_new_ref(*src)))
    return NULL;
  *callable = tmp;
  return callable;
}
