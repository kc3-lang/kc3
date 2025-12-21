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
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_parse.h"
#include "eval.h"
#include "pstruct.h"
#include "struct.h"
#include "struct_type.h"
#include "tag.h"

void pstruct_clean (p_struct *s)
{
  bool nullify = (*s)->ref_count == 1;
  struct_delete(*s);
  if (nullify)
    *s = NULL;
}

p_struct * pstruct_init (p_struct *s, const s_sym *module)
{
  p_struct tmp;
  if (! (tmp = struct_new(module)))
    return NULL;
  *s = tmp;
  return s;
}

p_struct * pstruct_init_1 (p_struct *s, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  p_struct tmp = NULL;
  p_struct tmp2 = NULL;
  assert(s);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_pstruct(&buf, &tmp);
  buf_clean(&buf);
  if (r < 0 || (uw) r != len) {
    err_puts("struct_init_1: invalid struct");
    assert(! "struct_init_1: invalid struct");
    if (r > 0)
      pstruct_clean(&tmp);
    return NULL;
  }
  if (false) {
    err_write_1("\nstruct_init_1: tmp = ");
    err_inspect_struct(tmp);
    err_write_1("\n");
  }
  if (! eval_struct(tmp, &tmp2)) {
    err_puts("struct_init_1: env_eval_struct");
    assert(! "struct_init_1: env_eval_struct");
    pstruct_clean(&tmp);
    return NULL;
  }
  pstruct_clean(&tmp);
  *s = tmp2;
  return s;
}

p_struct * pstruct_init_cast (p_struct *s, const s_sym * const *type,
                              s_tag *tag)
{
  assert(s);
  assert(tag);
  switch (tag->type) {
  case TAG_PSTRUCT:
    if (*type == tag->data.pstruct->pstruct_type->module)
      return pstruct_init_copy(s, &tag->data.pstruct);
    break;
  case TAG_PTR:
    return pstruct_init_copy(s, tag->data.ptr.p);
  default:
    break;
  }
  err_write_1("struct_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to %");
  err_inspect_psym(type);
  err_write_1("{}\n");
  assert(! "struct_init_cast: cannot cast to Struct");
  return NULL;
}

p_struct * pstruct_init_copy (p_struct *s, p_struct *src)
{
  p_struct tmp;
  if (env_global()->pass_by_copy)
    tmp = struct_new_copy(*src);
  else
    tmp = struct_new_ref(*src);
  if (! tmp)
    return NULL;
  *s = tmp;
  return s;
}

p_struct * pstruct_init_copy_data (p_struct *s, const s_sym *module,
                                   void *data)
{
  p_struct tmp;
  if (! pstruct_init(&tmp, module)) {
    err_write_1("pstruct_init_copy_data: pstruct_init ");
    err_inspect_sym(module);
    err_write_1("\n");
    assert(! "pstruct_init_copy_data: pstruct_init");
    return NULL;
  }
  if (! struct_allocate(tmp) || ! tmp->data) {
    pstruct_clean(&tmp);
    return NULL;
  }
  if (! struct_type_copy_data(tmp->pstruct_type, tmp->data, data)) {
    pstruct_clean(&tmp);
    return NULL;
  }
  *s = tmp;
  return s;
}

p_struct * pstruct_init_put (p_struct *s, s_struct *src,
                             const s_sym *key, s_tag *value)
{
  p_struct tmp;
  if (! (tmp = struct_new_copy(src))) {
    err_puts("pstruct_init_put: struct_new_copy");
    return NULL;
  }
  if (! struct_set(tmp, key, value)) {
    err_puts("struct_put: struct_set");
    pstruct_clean(&tmp);
    return NULL;
  }
  *s = tmp;
  return s;
}

p_struct * pstruct_init_with_data (p_struct *s, const s_sym *module,
                                   void *data, bool free_data)
{
  p_struct tmp;
  if (! (tmp = struct_new_with_data(module, data, free_data)))
    return NULL;
  *s = tmp;
  return s;
}

p_struct * pstruct_init_with_type (p_struct *s, s_struct_type *st)
{
  p_struct tmp;
  if (! (tmp = struct_new_with_type(st)))
    return NULL;
  *s = tmp;
  return s;
}
