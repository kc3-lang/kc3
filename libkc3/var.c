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
#include "data.h"
#include "mutex.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

s_tag * var_assign (s_var *var, s_tag *value, s_tag *dest)
{
  assert(var);
  assert(value);
  assert(dest);
  if (! var_set(var, value))
    return NULL;
  return tag_init_copy(dest, value);
}

void var_clean (s_var *var)
{
  assert(var);
  if (var->bound)
    tag_clean(&var->tag);
#if HAVE_PTHREAD
  mutex_clean(&var->mutex);
#endif
}

void var_delete (s_var *var)
{
#if HAVE_PTHREAD
  mutex_lock(&var->mutex);
#endif
  if (var->ref_count <= 0) {
    err_puts("var_delete: invalid reference count");
    assert(! "var_delete: invalid reference count");
    abort();
  }
  if (--var->ref_count) {
#if HAVE_PTHREAD
    mutex_unlock(&var->mutex);
#endif
    return;
  }
#if HAVE_PTHREAD
  mutex_unlock(&var->mutex);
#endif
  var_clean(var);
  free(var);
}

s_var * var_init (s_var *var, const s_sym *type)
{
  s_var tmp = {0};
  assert(var);
  assert(type);
  if (! sym_is_module(type)) {
    err_write_1("var_init: invalid type: ");
    err_inspect_sym(&type);
    err_write_1("\n");
    assert(! "var_init: invalid type");
    return NULL;
  }
  tmp.type = type;
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
  tmp.ref_count = 1;
  *var = tmp;
  return var;
}

s_var * var_init_copy (s_var *var, s_var *src)
{
  s_var tmp = {0};
  assert(var);
  assert(src);
  if (! var_init(&tmp, src->type))
    return NULL;
  tmp.bound = src->bound;
  if (tmp.bound)
    tag_init_copy(&tmp.tag, &src->tag);
  *var = tmp;
  return var;
}

bool * var_is_unbound (const s_var *var, bool *dest)
{
  assert(var);
  assert(dest);
  *dest = ! var->bound;
  return dest;
}

s_var * var_new (const s_sym *type)
{
  s_var *tmp = NULL;
  assert(type);
  if (! (tmp = alloc(sizeof(s_var))))
    return NULL;
  if (! var_init(tmp, type)) {
    free(tmp);
    return NULL;
  }
  return tmp;
}

s_var * var_new_copy (s_var *src)
{
  s_var *tmp = NULL;
  assert(src);
  if (! (tmp = alloc(sizeof(s_var))))
    return NULL;
  if (! var_init_copy(tmp, src)) {
    free(tmp);
    return NULL;
  }
  return tmp;
}

s_var * var_new_ref (s_var *src)
{
  assert(src);
#if HAVE_PTHREAD
  mutex_lock(&src->mutex);
#endif
  if (src->ref_count <= 0) {
    err_puts("var_new_ref: invalid reference count");
    assert(! "var_new_ref: invalid reference count");
    abort();
  }
  src->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&src->mutex);
#endif
  return src;
}

s_var * var_reset (s_var *var)
{
  assert(var);
#if HAVE_PTHREAD
  mutex_lock(&var->mutex);
#endif
  if (var->bound)
    tag_clean(&var->tag);
  var->bound = false;
#if HAVE_PTHREAD
  mutex_unlock(&var->mutex);
#endif
  return var;
}

s_var * var_set (s_var *var, s_tag *value)
{
  const s_sym *value_type;
  assert(var);
  assert(value);
  if (var->bound) {
    err_puts("var_set: var is already bound");
    assert(! "var_set: var is already bound");
    return NULL;
  }
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
  if (! tag_init_copy(&var->tag, value))
    return NULL;
  var->bound = true;
  if (false) {
    err_write_1("var_set: ");
    err_inspect_var(var);
    err_write_1("\n");
  }
  return var;
}
