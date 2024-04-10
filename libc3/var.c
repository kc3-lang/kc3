/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "sym.h"
#include "tag.h"
#include "var.h"

s_tag * var_init_cast (s_tag *tag, const s_sym *type, const s_tag *src)
{
  void *data;
  s_tag tmp;
  assert(tag);
  assert(type);
  assert(type != &g_sym_Var);
  assert(src);
  if (type == &g_sym_Var) {
    err_puts("var_init_cast: cannot cast to Var");
    assert(! "var_init_cast: cannot cast to Var");
    return NULL;
  }
  if (! sym_to_tag_type(type, &tmp.type))
    return NULL;
  if (! tag_to_pointer(&tmp, type, &data))
    return NULL;
  if (! data_init_cast(data, type, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * var_init_copy (s_tag *tag, const s_tag *src)
{
  assert(tag);
  assert(src);
  assert(src->type == TAG_VAR);
  if (src->type != TAG_VAR)
    return NULL;
  tag->type = TAG_VAR;
  tag->data.var = src->data.var;
  return tag;
}
