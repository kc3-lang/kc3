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
#include "alloc.h"
#include "assert.h"
#include "integer.h"
#include "ptr_free.h"
#include "sym.h"
#include "tag_type.h"

void ptr_free_clean (u_ptr_w *ptr_free)
{
  assert(ptr_free);
  alloc_free(ptr_free->p);
}

void ptr_free_delete (u_ptr_w *ptr_free)
{
  assert(ptr_free);
  ptr_free_clean(ptr_free);
  alloc_free(ptr_free);
}

u_ptr_w * ptr_free_init (u_ptr_w *ptr_free, void *p)
{
  u_ptr_w tmp = {0};
  assert(ptr_free);
  tmp.p = p;
  *ptr_free = tmp;
  return ptr_free;
}

u_ptr_w * ptr_free_init_cast (u_ptr_w *p,
                              const s_sym * const *type,
                              const s_tag *tag)
{
  assert(p);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_F32: p->p = (void *) ((uw) tag->data.f32);  return p;
  case TAG_F64: p->p = (void *) ((uw) tag->data.f64);  return p;
  case TAG_INTEGER:
    p->p = (void *) integer_to_uw(&tag->data.integer); return p;
  case TAG_PTR: p->p = tag->data.ptr.p;                return p;
  case TAG_PTR_FREE: p->p = tag->data.ptr_free.p;      return p;
  case TAG_S8:  p->p = (void *) ((uw) tag->data.s8);   return p;
  case TAG_S16: p->p = (void *) ((uw) tag->data.s16);  return p;
  case TAG_S32: p->p = (void *) ((uw) tag->data.s32);  return p;
  case TAG_S64: p->p = (void *) ((uw) tag->data.s64);  return p;
  case TAG_SW:  p->p = (void *) ((uw) tag->data.sw);   return p;
  case TAG_U8:  p->p = (void *) ((uw) tag->data.u8);   return p;
  case TAG_U16: p->p = (void *) ((uw) tag->data.u16);  return p;
  case TAG_U32: p->p = (void *) ((uw) tag->data.u32);  return p;
  case TAG_U64: p->p = (void *) ((uw) tag->data.u64);  return p;
  case TAG_UW:  p->p = (void *) ((uw) tag->data.uw);   return p;
  default: break;
  }
  err_write_1("ptr_free_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_PtrFree)
    err_puts(" to PtrFree");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka PtrFree");
  }
  assert(! "ptr_free_init_cast: cannot cast to PtrFree");
  return NULL;
}

u_ptr_w * ptr_free_init_copy (u_ptr_w *ptr_free, const u_ptr_w *src)
{
  u_ptr_w tmp = {0};
  assert(ptr_free);
  assert(src);
  (void) src;
  *ptr_free = tmp;
  return ptr_free;
}

u_ptr_w * ptr_free_new (void *p)
{
  u_ptr_w *ptr_free;
  ptr_free = alloc(sizeof(u_ptr_w));
  if (! ptr_free)
    return NULL;
  if (! ptr_free_init(ptr_free, p)) {
    alloc_free(ptr_free);
    return NULL;
  }
  return ptr_free;
}

u_ptr_w * ptr_free_new_copy (const u_ptr_w *src)
{
  u_ptr_w *ptr_free;
  assert(src);
  ptr_free = alloc(sizeof(u_ptr_w));
  if (! ptr_free)
    return NULL;
  if (! ptr_free_init_copy(ptr_free, src)) {
    alloc_free(ptr_free);
    return NULL;
  }
  return ptr_free;
}
