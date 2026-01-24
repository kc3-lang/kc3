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
#include "alloc.h"
#include "assert.h"
#include "callable.h"
#include "pcallable.h"
#include "pstruct_type.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"

void pstruct_type_clean (p_struct_type *st)
{
  struct_type_delete(*st);
}

p_struct_type * pstruct_type_find (const s_sym *module,
                                   p_struct_type *dest)
{
  return env_pstruct_type_find(env_global(), module, dest);
}

p_struct_type * pstruct_type_init (p_struct_type *st,
                                   const s_sym *module,
                                   const s_list *spec)
{
  s_struct_type *tmp;
  if (! (tmp = struct_type_new(module, spec)))
    return NULL;
  *st = tmp;
  return st;
}

p_struct_type * pstruct_type_init_cast (p_struct_type *st,
                                        p_sym *type,
                                        s_tag *tag)
{
  assert(st);
  assert(tag);
  switch (tag->type) {
  case TAG_PSTRUCT_TYPE:
    if (*type == &g_sym_StructType)
      return pstruct_type_init_copy(st, &tag->data.pstruct_type);
  default:
    break;
  }
  err_write_1("struct_type_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to StructType");
  assert(! "struct_type_init_cast: cannot cast to StructType");
  return NULL;
}

p_struct_type * pstruct_type_init_copy (p_struct_type *st,
                                        p_struct_type *src)
{
  s_struct_type *tmp;
  if (env_global()->pass_by_copy)
    tmp = struct_type_new_copy(*src);
  else
    tmp = struct_type_new_ref(*src);
  if (! tmp)
    return NULL;
  *st = tmp;
  return st;
}

p_struct_type * pstruct_type_init_clean (p_struct_type *st,
                                         s_struct_type *src,
                                         p_callable clean)
{
  p_struct_type tmp = NULL;
  assert(st);
  assert(src);
  assert(clean);
  if (! (tmp = alloc(sizeof(s_struct_type))))
    return NULL;
  if (! struct_type_init_copy(tmp, src)) {
    free(tmp);
    return NULL;
  }
  if (tmp->clean)
    pcallable_clean(&tmp->clean);
  if (! pcallable_init_copy(&tmp->clean, &clean)) {
    struct_type_delete(tmp);
    return NULL;
  }
  *st = tmp;
  return st;
}
