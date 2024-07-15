/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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

s_var * var_init (s_var *var, const s_sym *type)
{
  assert(var);
  assert(type);
  if (! sym_is_module(type)) {
    err_write_1("var_init: invalid type: ");
    err_inspect_sym(&type);
    err_write_1("\n");
    assert(! "var_init: invalid type");
    return NULL;
  }
  var->type = type;
  return var;
}

s_tag * var_init_cast (s_tag *tag, const s_sym * const *type,
                       const s_tag *src)
{
  void *data;
  s_tag tmp;
  assert(tag);
  assert(type);
  assert(src);
  if (*type == &g_sym_Var) {
    err_puts("var_init_cast: cannot cast to Var");
    assert(! "var_init_cast: cannot cast to Var");
    return NULL;
  }
  if (! sym_to_tag_type(*type, &tmp.type))
    return NULL;
  if (! tag_to_pointer(&tmp, *type, &data))
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

s_tag * var_set (s_tag *var, const s_tag *value)
{
  const s_sym *value_type;
  const s_sym *var_type;
  assert(var);
  assert(value);
  if (var->type != TAG_VAR) {
    err_puts("var_set: not a TAG_VAR");
    assert(! "var_set: not a TAG_VAR");
    return NULL;
  }
  var_type = var->data.var.type;
  if (var_type != &g_sym_Tag) {
    if (! tag_type(value, &value_type))
      return NULL;
    if (var_type != value_type) {
      err_write_1("var_set: type mismatch: ");
      err_inspect_sym(&var_type);
      err_write_1(" != ");
      err_inspect_sym(&value_type);
      err_write_1("\n");
      assert(! "var_set: type mismatch");
      return NULL;
    }
  }
  return tag_init_copy(var, value);
}
