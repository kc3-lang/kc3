/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "ptr.h"

void ptr_delete (s_ptr *ptr)
{
  free(ptr);
}

s_ptr * ptr_init (s_ptr *ptr, const s_sym *type, void *p)
{
  assert(ptr);
  ptr->type = type;
  ptr->p = p;
  return ptr;
}

s_ptr * ptr_new (const s_sym *type, void *p)
{
  s_ptr *ptr;
  ptr = calloc(1, sizeof(s_ptr));
  if (! ptr) {
    warn("ptr_new: ptr");
    return NULL;
  }
  if (! ptr_init(ptr, type, p)) {
    free(ptr);
    return NULL;
  }
  return ptr;
}
