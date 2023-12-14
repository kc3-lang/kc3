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
#include <string.h>
#include "env.h"
#include "list.h"
#include "map.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

void struct_clean (s_struct *s)
{
  f_clean clean;
  s8 *data;
  uw i = 0;
  const s_sym *sym;
  assert(s);
  data = s->data;
  while (i < s->type.map.count) {
    if (tag_type(s->type.map.value + i, &sym)) {
      clean = sym_to_clean(sym);
      if (clean)
        clean(data + s->type.offset[i]);
      i++;
    }
  }
  if (s->free)
    free(data);
  struct_type_clean(&s->type);
}

void struct_delete (s_struct *s)
{
  assert(s);
  struct_clean(s);
  free(s);
}

s_struct * struct_init (s_struct *s, const s_sym *module)
{
  s_struct tmp = {0};
  assert(s);
  assert(module);
  if (! struct_type_init_from_env(&tmp.type, module, &g_c3_env))
    return NULL;
  tmp.free = true;
  tmp.data = calloc(1, tmp.type.size);
  if (! tmp.data) {
    warn("struct_init: data");
    assert(! "struct_init: data: failed to allocate memory");
    struct_type_clean(&tmp.type);
    return NULL;
  }
  *s = tmp;
  return s;
}

s_struct * struct_init_1 (s_struct *s, const s8 *p)
{
  assert(s);
  assert(p);
  (void) s;
  (void) p;
  return s;
}

s_struct * struct_init_copy (s_struct *s, const s_struct *src)
{
  f_clean clean;
  f_init_copy init_copy;
  uw i = 0;
  uw size;
  const s_sym *sym;
  s_struct tmp;
  assert(s);
  assert(src);
  if (! struct_type_init_copy(&tmp.type, &src->type))
    return NULL;
  tmp.free = true;
  tmp.data = calloc(1, tmp.type.size);
  while (i < tmp.type.map.count) {
    if (tag_type(tmp.type.map.value + i, &sym)) {
      init_copy = sym_to_init_copy(sym);
      if (init_copy) {
        if (! init_copy((s8 *) tmp.data + tmp.type.offset[i],
                        (s8 *) src->data + tmp.type.offset[i]))
          goto ko;
      }
      else {
        size = tag_size(tmp.type.map.value + i);
        memcpy((s8 *) tmp.data + tmp.type.offset[i],
               (s8 *) src->data + tmp.type.offset[i],
               size);
      }
    }
    i++;
  }
  *s = tmp;
  return s;
 ko:
  while (i > 0) {
    i--;
    tag_type(tmp.type.map.value + i, &sym);
    clean = sym_to_clean(sym);
    if (clean)
      clean((s8 *) tmp.data + tmp.type.offset[i]);
  }
  free(tmp.data);
  struct_type_clean(&tmp.type);
  return NULL;
}

s_struct * struct_init_from_lists (s_struct *s, const s_sym *module,
                                   const s_list *keys,
                                   const s_list *values)
{
  const s_list *k;
  s_struct tmp = {0};
  const s_list *v;
  assert(s);
  assert(module);
  assert(list_length(keys) == list_length(values));
  if (! struct_init(&tmp, module))
    return NULL;
  k = keys;
  v = values;
  while (k && v) {
    assert(k->tag.type == TAG_SYM);
    if (k->tag.type != TAG_SYM)
      errx(1, "struct_init_from_lists: key that is not a symbol: %s",
           tag_type_to_string(k->tag.type));
    if (! struct_set(&tmp, k->tag.data.sym, &v->tag))
      errx(1, "struct_init_from_lists: struct_set(%s) failed",
           k->tag.data.sym->str.ptr.ps8);
    k = list_next(k);
    v = list_next(v);
  }
  *s = tmp;
  return s;
}

s_struct * struct_init_with_data (s_struct *s, const s_sym *module,
                                  bool free_data, void *data)
{
  s_struct tmp = {0};
  assert(s);
  assert(module);
  if (! struct_type_init_from_env(&tmp.type, module, &g_c3_env))
    return NULL;
  tmp.free = free_data;
  tmp.data = data;
  *s = tmp;
  return s;
}

s_struct * struct_new (const s_sym *module)
{
  s_struct *s;
  assert(module);
  s = calloc(1, sizeof(s_struct));
  if (! s) {
    warn("struct_new: %s: calloc", module->str.ptr.ps8);
    return NULL;
  }
  if (! struct_init(s, module)) {
    free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_new_1 (const s8 *p)
{
  s_struct *s;
  assert(p);
  s = calloc(1, sizeof(s_struct));
  if (! s) {
    warn("struct_new_1: %s: calloc", p);
    return NULL;
  }
  if (! struct_init_1(s, p)) {
    free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_new_copy (const s_struct *src)
{
  s_struct *s;
  assert(src);
  s = calloc(1, sizeof(s_struct));
  if (! s) {
    warn("struct_new_copy: calloc");
    return NULL;
  }
  if (! struct_init_copy(s, src)) {
    free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_set (s_struct *s, const s_sym *key,
                       const s_tag *value)
{
  f_clean clean;
  void *data;
  const void *data_src;
  uw i;
  f_init_copy init_copy;
  e_tag_type type;
  assert(s);
  assert(s->type.map.count);
  assert(key);
  assert(value);
  i = 0;
  while (i < s->type.map.count) {
    if (s->type.map.key[i].type == TAG_SYM &&
        s->type.map.key[i].data.sym == key) {
      type = s->type.map.value[i].type;
      clean = tag_type_to_clean(type);
      init_copy = tag_type_to_init_copy(type);
      data = (s8 *) s->data + s->type.offset[i];
      clean(data);
      data_src = tag_to_const_pointer(value, tag_type_to_sym(type));
      if (! init_copy(data, data_src))
        return NULL;
      return s;
    }
    i++;
  }
  return NULL;
}
