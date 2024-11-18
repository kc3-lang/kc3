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
#include <string.h>
#include "assert.h"
#include "buf.h"
#include "buf_parse.h"
#include "data.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

s_tag * var_assign (const s_var *var, s_tag *value, s_tag *dest)
{
  assert(var);
  assert(value);
  assert(dest);
  if (! var_set(var, value))
    return NULL;
  return tag_init_copy(dest, value);
}

s_var * var_init (s_var *var, s_tag *ptr, const s_sym *type)
{
  s_var tmp = {0};
  assert(var);
  assert(ptr);
  assert(type);
  if (! sym_is_module(type)) {
    err_write_1("var_init: invalid type: ");
    err_inspect_sym(&type);
    err_write_1("\n");
    assert(! "var_init: invalid type");
    return NULL;
  }
  tmp.ptr = ptr;
  tmp.type = type;
  *var = tmp;
  return var;
}

s_var * var_init_1 (s_var *var, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  s_var tmp;
  assert(var);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_var(&buf, &tmp);
  if (r < 0 || (uw) r != len) {
    err_puts("var_init_1: invalid var");
    assert(! "var_init_1: invalid var");
    return NULL;
  }
  *var = tmp;
  return var;
}

s_var * var_init_cast (s_var *var, const s_sym * const *type,
                       s_tag *src)
{
  void *data;
  s_tag tag = {0};
  s_var tmp = {0};
  assert(var);
  assert(type);
  assert(src);
  if (*type == &g_sym_Var) {
    tmp.type = &g_sym_Tag;
    switch (src->type) {
    case TAG_PTR: tmp.ptr = src->data.ptr.p;              break;
    case TAG_S8:  tmp.ptr = (s_tag *) (uw) src->data.s8;  break;
    case TAG_S16: tmp.ptr = (s_tag *) (uw) src->data.s16; break;
    case TAG_S32: tmp.ptr = (s_tag *) (uw) src->data.s32; break;
    case TAG_S64: tmp.ptr = (s_tag *) (uw) src->data.s64; break;
    case TAG_SW:  tmp.ptr = (s_tag *) (uw) src->data.sw;  break;
    case TAG_U8:  tmp.ptr = (s_tag *) (uw) src->data.u8;  break;
    case TAG_U16: tmp.ptr = (s_tag *) (uw) src->data.u16; break;
    case TAG_U32: tmp.ptr = (s_tag *) (uw) src->data.u32; break;
    case TAG_U64: tmp.ptr = (s_tag *) (uw) src->data.u64; break;
    case TAG_UW:  tmp.ptr = (s_tag *) src->data.uw;       break;
    default: goto invalid_cast;
    }
    *var = tmp;
    return var;
  }
  if (! sym_to_tag_type(*type, &tag.type))
    return NULL;
  if (! tag_to_pointer(&tag, *type, &data))
    return NULL;
  if (! data_init_cast(data, type, src))
    return NULL;
  *var->ptr = tag;
  return var;
 invalid_cast:
  err_puts("var_init_cast: cannot cast to Var");
  assert(! "var_init_cast: cannot cast to Var");
  return NULL;
}

s_var * var_init_copy (s_var *var, const s_var *src)
{
  assert(var);
  assert(src);
  *var = *src;
  return var;
}

bool * var_is_unbound (const s_var *var, bool *dest)
{
  assert(var);
  assert(dest);
  *dest = var->ptr->type == TAG_VAR;
  return dest;
}

const s_var * var_reset (const s_var *var)
{
  s_tag tmp = {0};
  assert(var);
  assert(var->ptr);
  tmp.type = TAG_VAR;
  var_init(&tmp.data.var, var->ptr, var->type);
  tag_clean(var->ptr);
  *var->ptr = tmp;
  return var;
}

const s_var * var_set (const s_var *var, s_tag *value)
{
  const s_sym *value_type;
  assert(var);
  assert(value);
  if (var->type != &g_sym_Tag) {
    if (! tag_type(value, &value_type))
      return NULL;
    if (var->type != value_type) {
      err_write_1("var_set: type mismatch: ");
      err_inspect_sym(&var->type);
      err_write_1(" != ");
      err_inspect_sym(&value_type);
      err_write_1("\n");
      assert(! "var_set: type mismatch");
      return NULL;
    }
  }
  if (! tag_init_copy(var->ptr, value))
    return NULL;
  return var;
}
