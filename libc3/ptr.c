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

void ptr_delete (u_ptr_w *ptr)
{
  assert(ptr);
  free(ptr);
}

u_ptr_w * ptr_init (u_ptr_w *ptr, void *p)
{
  u_ptr_w tmp = {0};
  assert(ptr);
  tmp.p = p;
  *ptr = tmp;
  return ptr;
}

u_ptr_w * ptr_init_copy (u_ptr_w *ptr, const u_ptr_w *src)
{
  u_ptr_w tmp = {0};
  assert(ptr);
  assert(src);
  tmp.p = src->p;
  *ptr = tmp;
  return ptr;
}

u_ptr_w * ptr_new (void *p)
{
  u_ptr_w *ptr;
  ptr = calloc(1, sizeof(u_ptr_w));
  if (! ptr) {
    warn("ptr_new: ptr");
    return NULL;
  }
  if (! ptr_init(ptr, p)) {
    free(ptr);
    return NULL;
  }
  return ptr;
}

u_ptr_w * ptr_new_copy (const u_ptr_w *src)
{
  u_ptr_w *ptr;
  assert(src);
  ptr = calloc(1, sizeof(u_ptr_w));
  if (! ptr) {
    warn("ptr_new: ptr");
    return NULL;
  }
  if (! ptr_init_copy(ptr, src)) {
    free(ptr);
    return NULL;
  }
  return ptr;
}