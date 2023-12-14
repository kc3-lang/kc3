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
#include "ptr_free.h"

void ptr_free_clean (u_ptr_w *ptr_free)
{
  assert(ptr_free);
  free(ptr_free->p);
}

void ptr_free_delete (u_ptr_w *ptr_free)
{
  assert(ptr_free);
  ptr_free_clean(ptr_free);
  free(ptr_free);
}

u_ptr_w * ptr_free_init (u_ptr_w *ptr_free, void *p)
{
  u_ptr_w tmp = {0};
  assert(ptr_free);
  tmp.p = p;
  *ptr_free = tmp;
  return ptr_free;
}

u_ptr_w * ptr_free_init_copy (u_ptr_w *ptr_free, const u_ptr_w *src)
{
  u_ptr_w tmp = {0};
  assert(ptr_free);
  assert(src);
  tmp.p = src->p;
  *ptr_free = tmp;
  return ptr_free;
}

u_ptr_w * ptr_free_new (void *p)
{
  u_ptr_w *ptr_free;
  ptr_free = calloc(1, sizeof(u_ptr_w));
  if (! ptr_free) {
    warn("ptr_free_new: ptr_free");
    return NULL;
  }
  if (! ptr_free_init(ptr_free, p)) {
    free(ptr_free);
    return NULL;
  }
  return ptr_free;
}

u_ptr_w * ptr_free_new_copy (const u_ptr_w *src)
{
  u_ptr_w *ptr_free;
  assert(src);
  ptr_free = calloc(1, sizeof(u_ptr_w));
  if (! ptr_free) {
    warn("ptr_free_new: ptr_free");
    return NULL;
  }
  if (! ptr_free_init_copy(ptr_free, src)) {
    free(ptr_free);
    return NULL;
  }
  return ptr_free;
}
