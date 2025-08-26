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
#include "assert.h"
#include "data.h"
#include "pointer.h"
#include "sym.h"
#include "tag.h"

s_tag * pointer_deref (const s_pointer *pointer, s_tag *dest)
{
  s_tag tmp = {0};
  void *tmp_data = NULL;
  assert(pointer);
  if (! sym_to_tag_type(pointer->target_type, &tmp.type)) {
    err_puts("pointer_deref: sym_to_tag_type");
    assert(! "pointer_deref: sym_to_tag_type");
    return NULL;
  }
  if (! tag_to_pointer(&tmp, pointer->target_type, &tmp_data)) {
    err_puts("pointer_deref: tag_to_pointer");
    assert(! "pointer_deref: tag_to_pointer");
    return NULL;
  }
  if (! data_init_copy(pointer->target_type, tmp_data,
                       pointer->ptr.p)) {
    err_puts("pointer_deref: data_init_copy");
    assert(! "pointer_deref: data_init_copy");
    return NULL;
  }
  *dest = tmp;
  return dest;
}

s_pointer * pointer_init (s_pointer *pointer,
                          const s_sym *pointer_type,
                          const s_sym *target_type,
                          void *p)
{
  s_pointer tmp = {0};
  if (! pointer_type && ! target_type)
    target_type = &g_sym_Tag;
  if (! pointer_type)
    pointer_type = sym_target_to_pointer_type(target_type);
  else if (! target_type)
    target_type = sym_pointer_to_target_type(pointer_type);
  tmp.pointer_type = pointer_type;
  tmp.target_type = target_type;
  tmp.ptr.p = p;
  *pointer = tmp;
  return pointer;
}

s_pointer * pointer_init_cast (s_pointer *pointer,
                               const p_sym *type,
                               s_tag *tag)
{
  s_pointer tmp = {0};
  assert(pointer);
  assert(type);
  assert(tag);
  tmp.pointer_type = *type;
  if (! tag_type(tag, &tmp.target_type)) {
    err_puts("pointer_init_cast: tag_type");
    assert(! "pointer_init_cast: tag_type");
    return NULL;
  }
  if (! sym_is_pointer_type(tmp.pointer_type, tmp.target_type)) {
    err_puts("pointer_init_cast: sym_is_pointer_type");
    assert(! "pointer_init_cast: sym_is_pointer_type");
    return NULL;
  }
  if (! tag_to_pointer(tag, tmp.target_type, &tmp.ptr.p)) {
    err_puts("pointer_init_cast: tag_to_pointer");
    assert(! "pointer_init_cast: tag_to_pointer");
    return NULL;
  }
  *pointer = tmp;
  return pointer;
}

s_pointer * pointer_init_copy (s_pointer *pointer,
                               const s_pointer *src)
{
  assert(pointer);
  assert(src);
  *pointer = *src;
  return pointer;
}
