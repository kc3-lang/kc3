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
#include "ptr.h"
#include "sym.h"
#include "tag_type.h"

void ptr_delete (u_ptr_w *ptr)
{
  assert(ptr);
  alloc_free(ptr);
}

u_ptr_w * ptr_init (u_ptr_w *ptr, void *p)
{
  u_ptr_w tmp = {0};
  assert(ptr);
  tmp.p = p;
  *ptr = tmp;
  return ptr;
}

u_ptr_w * ptr_init_cast (u_ptr_w *p,
                         const s_sym * const *type,
                         const s_tag *tag)
{
  assert(p);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_F32:      p->p = (void *) ((uw) tag->data.f32); return p;
  case TAG_F64:      p->p = (void *) ((uw) tag->data.f64); return p;
  case TAG_INTEGER:
    p->p = (void *) integer_to_uw(&tag->data.integer);     return p;
  case TAG_PSTRUCT:  p->p = tag->data.pstruct;             return p;
  case TAG_PTR:      p->p = tag->data.ptr.p;               return p;
  case TAG_PTR_FREE: p->p = tag->data.ptr_free.p;          return p;
  case TAG_PVAR:     p->p = &tag->data.pvar->tag;          return p;
  case TAG_S8:       p->p = (void *) ((uw) tag->data.s8);  return p;
  case TAG_S16:      p->p = (void *) ((uw) tag->data.s16); return p;
  case TAG_S32:      p->p = (void *) ((uw) tag->data.s32); return p;
  case TAG_S64:      p->p = (void *) ((uw) tag->data.s64); return p;
  case TAG_SW:       p->p = (void *) ((uw) tag->data.sw);  return p;
  case TAG_U8:       p->p = (void *) ((uw) tag->data.u8);  return p;
  case TAG_U16:      p->p = (void *) ((uw) tag->data.u16); return p;
  case TAG_U32:      p->p = (void *) ((uw) tag->data.u32); return p;
  case TAG_U64:      p->p = (void *) ((uw) tag->data.u64); return p;
  case TAG_UW:       p->p = (void *) ((uw) tag->data.uw);  return p;
  default:
    break;
  }
  err_write_1("ptr_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Ptr)
    err_puts(" to Ptr");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Ptr");
  }
  assert(! "ptr_init_cast: cannot cast to Ptr");
  return NULL;
}

u_ptr_w * ptr_init_copy (u_ptr_w *ptr, const u_ptr_w *src)
{
  assert(ptr);
  assert(src);
  *ptr = *src;
  return ptr;
}

u_ptr_w * ptr_new (void *p)
{
  u_ptr_w *ptr;
  ptr = alloc(sizeof(u_ptr_w));
  if (! ptr)
    return NULL;
  if (! ptr_init(ptr, p)) {
    alloc_free(ptr);
    return NULL;
  }
  return ptr;
}

u_ptr_w * ptr_new_copy (const u_ptr_w *src)
{
  u_ptr_w *ptr;
  assert(src);
  ptr = alloc(sizeof(u_ptr_w));
  if (! ptr)
    return NULL;
  if (! ptr_init_copy(ptr, src)) {
    alloc_free(ptr);
    return NULL;
  }
  return ptr;
}
