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
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "data.h"
#include "env.h"
#include "list.h"
#include "map.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s_struct * struct_allocate (s_struct *s)
{
  s_struct tmp;
  assert(s);
  assert(s->type);
  assert(! s->data);
  tmp = *s;
  tmp.data = alloc(tmp.type->size);
  if (! tmp.data)
    return NULL;
  *s = tmp;
  return s;
}

void struct_clean (s_struct *s)
{
  uw i;
  const s_sym *sym;
  assert(s);
  assert(s->type);
  if (s->data) {
    if (s->type->clean)
      s->type->clean(s->data);
    if (s->type->must_clean) {
      i = 0;
      while (i < s->type->map.count) {
        if (tag_type(s->type->map.value + i, &sym))
          data_clean(sym, (s8 *) s->data + s->type->offset[i]);
        i++;
      }
    }
    free(s->data);
  }
  if (s->tag) {
    i = 0;
    while (i < s->type->map.count) {
      tag_clean(s->tag + i);
      i++;
    }
    free(s->tag);
  }
}

void struct_delete (s_struct *s)
{
  assert(s);
  struct_clean(s);
  free(s);
}

bool struct_find_key_index (const s_struct *s, const s_sym *key,
                            uw *dest)
{
  uw i = 0;
  assert(s);
  assert(key);
  while (i < s->type->map.count) {
    assert(s->type->map.key[i].type == TAG_SYM);
    if (s->type->map.key[i].data.sym == key) {
      *dest = i;
      return true;
    }
    i++;
  }
  return false;
}

void * struct_get (s_struct *s, const s_sym *key)
{
  uw i = 0;
  assert(s);
  assert(key);
  if (! struct_find_key_index(s, key, &i))
    return NULL;
  assert(i < s->type->map.count);
  return (u8 *) s->data + s->type->offset[i];
}

const s_sym ** struct_get_sym (s_struct *s, const s_sym *key)
{
  return (const s_sym **) struct_get(s, key);
}

s_tag * struct_get_tag (s_struct *s, const s_sym *key)
{
  return (s_tag *) struct_get(s, key);
}

u8 struct_get_u8 (s_struct *s, const s_sym *key)
{
  return *(u8 *) struct_get(s, key);
}

s_struct * struct_init (s_struct *s, const s_sym *module)
{
  s_struct tmp = {0};
  assert(s);
  assert(module);
  if (! struct_type_find(module, &tmp.type)) {
    err_write_1("struct_init: struct_type_find(");
    err_inspect_sym(&module);
    err_puts(")");
    return NULL;
  }
  if (! tmp.type) {
    err_write_1("struct_init: struct_type not found: ");
    err_inspect_sym(&module);
    err_write_1("\n");
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
  // FIXME
  return s;
}

s_struct * struct_init_cast (s_struct *s, const s_sym * const *type,
                             const s_tag *tag)
{
  assert(s);
  assert(tag);
  switch (tag->type) {
  case TAG_STRUCT:
    if (*type == tag->data.struct_.type->module)
      return struct_init_copy(s, &tag->data.struct_);
  default:
    break;
  }
  err_write_1("struct_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to %");
  err_inspect_sym(type);
  err_write_1("{}\n");
  assert(! "struct_init_cast: cannot cast to Struct");
  return NULL;
}

s_struct * struct_init_copy (s_struct *s, const s_struct *src)
{
  uw i;
  const s_sym *sym;
  s_struct tmp = {0};
  assert(s);
  assert(src);
  assert(src->type);
  tmp.type = src->type;
  if (src->data) {
    tmp.data = alloc(tmp.type->size);
    if (! tmp.data)
      return NULL;
    i = 0;
    while (i < tmp.type->map.count) {
      if (! tag_type(tmp.type->map.value + i, &sym) ||
          ! data_init_copy(sym, (s8 *) tmp.data + tmp.type->offset[i],
                           (s8 *) src->data + tmp.type->offset[i]))
        goto ko;
      i++;
    }
  }
  else if (src->tag) {
    tmp.tag = alloc(tmp.type->map.count * sizeof(s_tag));
    if (! tmp.tag)
      return NULL;
    i = 0;
    while (i < tmp.type->map.count) {
      if (! tag_init_copy(tmp.tag + i, src->tag + i))
        goto ko;
      i++;
    }
  }
  *s = tmp;
  return s;
 ko:
  struct_clean(&tmp);
  return NULL;
}

s_struct * struct_init_from_lists (s_struct *s, const s_sym *module,
                                   const s_list *keys,
                                   const s_list *values)
{
  uw i;
  const s_list *k;
  s_struct tmp = {0};
  const s_list *v;
  assert(s);
  assert(module);
  assert(list_length(keys) == list_length(values));
  if (! struct_init(&tmp, module))
    return NULL;
  tmp.tag = alloc(tmp.type->map.count * sizeof(s_tag));
  if (! tmp.tag)
    return NULL;
  k = keys;
  v = values;
  while (k && v) {
    assert(k->tag.type == TAG_SYM);
    if (k->tag.type != TAG_SYM) {
      err_write_1("struct_init_from_lists: key that is not a symbol: ");
      err_puts(tag_type_to_string(k->tag.type));
      assert(! "struct_init_from_lists: key that is not a symbol");
      goto ko;
    }
    if (! struct_find_key_index(&tmp, k->tag.data.sym, &i)) {
      err_write_1("struct_init_from_lists:"
                  " cannot find key in defstruct: ");
      err_puts(k->tag.data.sym->str.ptr.pchar);
      assert(! "struct_init_from_lists: cannot find key in defstruct");
      goto ko;
    }
    if (! tag_init_copy(tmp.tag + i, &v->tag))
      goto ko;
    k = list_next(k);
    v = list_next(v);
  }
  i = 0;
  while (i < tmp.type->map.count) {
    if (! tmp.tag[i].type)
      if (! tag_init_copy(tmp.tag + i, tmp.type->map.value + i))
        goto ko;
    i++;
  }
  *s = tmp;
  return s;
 ko:
  struct_clean(&tmp);
  return NULL;
}

s_struct * struct_init_with_data (s_struct *s, const s_sym *module,
                                  void *data)
{
  s_struct tmp = {0};
  assert(s);
  assert(module);
  if (! struct_type_find(module, &tmp.type))
    return NULL;
  if (! tmp.type) {
    err_write_1("struct_init_with_data: struct_type not found: ");
    err_inspect_sym(&module);
    err_write_1("\n");
    return NULL;
  }
  tmp.data = data;
  *s = tmp;
  return s;
}

s_struct * struct_new (const s_sym *module)
{
  s_struct *s;
  assert(module);
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
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
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
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
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
  if (! struct_init_copy(s, src)) {
    free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_set (s_struct *s, const s_sym *key,
                       const s_tag *value)
{
  void *data;
  const void *data_src;
  uw i;
  const s_sym *type_sym;
  assert(s);
  assert(s->type->map.count);
  assert(key);
  assert(value);
  i = 0;
  while (i < s->type->map.count) {
    if (s->type->map.key[i].type == TAG_SYM &&
        s->type->map.key[i].data.sym == key) {
      if (! tag_type(s->type->map.value + i, &type_sym))
        return NULL;
      data = (s8 *) s->data + s->type->offset[i];
      if (! tag_to_const_pointer(value, type_sym, &data_src))
        return NULL;
      data_clean(type_sym, data);
      if (! data_init_copy(type_sym, data, data_src))
        return NULL;
      return s;
    }
    i++;
  }
  return NULL;
}
