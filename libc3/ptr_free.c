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
#include "integer.h"
#include "ptr_free.h"
#include "tag_type.h"

u_ptr_w * ptr_free_cast (const s_tag *tag, u_ptr_w *dest)
{
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_F32: dest->p = (void *) ((uw) tag->data.f32);  return dest;
  case TAG_F64: dest->p = (void *) ((uw) tag->data.f64);  return dest;
  case TAG_INTEGER:
    dest->p = (void *) integer_to_uw(&tag->data.integer); return dest;
  case TAG_PTR: dest->p = tag->data.ptr.p;                return dest;
  case TAG_PTR_FREE: dest->p = tag->data.ptr_free.p;      return dest;
  case TAG_S8:  dest->p = (void *) ((uw) tag->data.s8);          return dest;
  case TAG_S16: dest->p = (void *) ((uw) tag->data.s16);        return dest;
  case TAG_S32: dest->p = (void *) ((uw) tag->data.s32);        return dest;
  case TAG_S64: dest->p = (void *) ((uw) tag->data.s64);        return dest;
  case TAG_SW:  dest->p = (void *) ((uw) tag->data.sw);         return dest;
  case TAG_U8:  dest->p = (void *) ((uw) tag->data.u8);         return dest;
  case TAG_U16: dest->p = (void *) ((uw) tag->data.u16);        return dest;
  case TAG_U32: dest->p = (void *) ((uw) tag->data.u32);        return dest;
  case TAG_U64: dest->p = (void *) ((uw) tag->data.u64);        return dest;
  case TAG_UW:  dest->p = (void *) ((uw) tag->data.uw);         return dest;
  default: break;
  }
  warnx("ptr_free_cast: cannot cast %s to PtrFree",
        tag_type_to_string(tag->type));
  return NULL;
}

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
