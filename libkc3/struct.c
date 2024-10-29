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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_parse.h"
#include "data.h"
#include "env.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s_tag * struct_access (const s_struct *s, const s_list * const *key,
                       s_tag *dest)
{
  const s_tag *first;
  const s_list *next;
  s_tag *r;
  s_tag tag;
  assert(s);
  assert(key);
  assert(dest);
  first = &(*key)->tag;
  next = list_next(*key);
  if (first->type != TAG_SYM) {
    err_write_1("struct_access: key is not a Sym: (");
    err_inspect_struct(s);
    err_write_1(", ");
    err_inspect_tag(first);
    err_write_1(")\n");
    assert(! "struct_access: key is not a Sym");
    return NULL;
  }
  if (! next)
    return struct_access_sym(s, first->data.sym, dest);
  if (! struct_access_sym(s, first->data.sym, &tag)) {
    err_write_1("struct_access: map_get(");
    err_inspect_struct(s);
    err_write_1(", ");
    err_inspect_tag(first);
    err_write_1(")\n");
    assert(! "struct_access: map_get");
    return NULL;
  }
  r = kc3_access(&tag, &next, dest);
  tag_clean(&tag);
  return r;
}

s_tag * struct_access_sym (const s_struct *s, const s_sym *key, s_tag *dest)
{
  const void *data;
  const s_struct_type *st;
  const s_sym *type;
  s_tag tmp = {0};
  void *tmp_data;
  if (! struct_get_var_type(s, key, &type))
    return NULL;    
  data = struct_get(s, key);
  if (! data)
    return NULL;
  if (type != &g_sym_Tag) {
    if (! sym_to_tag_type(type, &tmp.type))
      return NULL;
    if (! struct_type_find(type, &st))
      return NULL;
    if (st) {
      tmp.data.struct_.type = st;
      if (! struct_allocate(&tmp.data.struct_))
        return NULL;
    }
  }
  if (! tag_to_pointer(&tmp, type, &tmp_data))
    return NULL;
  if (! data_init_copy(type, tmp_data, data))
    return NULL;
  *dest = tmp;
  return dest;
}

s_struct * struct_allocate (s_struct *s)
{
  s_struct tmp = {0};
  assert(s);
  assert(s->type);
  if (s->data) {
    err_puts("struct_allocate: data != NULL");
    assert(! "struct_allocate: data != NULL");
    return NULL;
  }
  tmp = *s;
  tmp.data = alloc(tmp.type->size);
  if (! tmp.data)
    return NULL;
  tmp.free_data = true;
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
    if (s->free_data)
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

uw * struct_find_key_index (const s_struct *s, const s_sym *key,
                            uw *dest)
{
  return struct_type_find_key_index(s->type, key, dest);
}

const void * struct_get (const s_struct *s, const s_sym *key)
{
  uw i = 0;
  assert(s);
  assert(key);
  if (! struct_find_key_index(s, key, &i))
    return NULL;
  assert(i < s->type->map.count);
  return (u8 *) s->data + s->type->offset[i];
}

const s_sym ** struct_get_sym (const s_struct *s, const s_sym *key)
{
  return (const s_sym **) struct_get(s, key);
}

const s_tag * struct_get_tag (const s_struct *s, const s_sym *key)
{
  return (s_tag *) struct_get(s, key);
}

const s_sym ** struct_get_type (const s_struct *s, const s_sym *key,
                                const s_sym **dest)
{
  s_tag tag_key;
  tag_init_sym(&tag_key, key);
  return map_get_type(&s->type->map, &tag_key, dest);
}

u8 struct_get_u8 (const s_struct *s, const s_sym *key)
{
  return *(u8 *) struct_get(s, key);
}

const s_sym ** struct_get_var_type (const s_struct *s, const s_sym *key,
                                    const s_sym **dest)
{
  s_tag tag_key;
  tag_init_sym(&tag_key, key);
  return map_get_var_type(&s->type->map, &tag_key, dest);
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

s_struct * struct_init_1 (s_struct *s, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  s_struct tmp = {0};
  s_struct tmp2 = {0};
  assert(s);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_struct(&buf, &tmp);
  if (r < 0 || (uw) r != len) {
    err_puts("struct_init_1: invalid struct");
    assert(! "struct_init_1: invalid struct");
    if (r > 0)
      struct_clean(&tmp);
    return NULL;
  }
  if (false) {
    err_write_1("\nstruct_init_1: tmp = ");
    err_inspect_struct(&tmp);
    err_write_1("\n");
  }
  if (! env_eval_struct(&g_kc3_env, &tmp, &tmp2)) {
    err_puts("struct_init_1: env_eval_struct");
    assert(! "struct_init_1: env_eval_struct");
    struct_clean(&tmp);
    return NULL;
  }
  struct_clean(&tmp);
  *s = tmp2;
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
  s_struct tmp = {0};
  const s_sym *type;
  assert(s);
  assert(src);
  assert(src->type);
  tmp.type = src->type;
  if (! tmp.type->size)
    return NULL;
  if (src->data) {
    tmp.data = alloc(tmp.type->size);
    if (! tmp.data)
      return NULL;
    tmp.free_data = true;
    i = 0;
    while (i < tmp.type->map.count) {
      if (tmp.type->map.value[i].type == TAG_VAR)
        type = tmp.type->map.value[i].data.var.type;
      else if (! tag_type(tmp.type->map.value + i, &type))
        goto ko;
      if (! data_init_copy(type,
                           (s8 *) tmp.data + tmp.type->offset[i],
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
  i = 0;
  while (i < tmp.type->map.count) {
    k = keys;
    v = values;
    while (k && v) {
      if (k->tag.type != TAG_SYM) {
        err_write_1("struct_init_from_lists: key that is not a"
                    " symbol: ");
        err_puts(tag_type_to_string(k->tag.type));
        assert(! "struct_init_from_lists: key that is not a symbol");
        goto ko;
      }
      if (k->tag.data.sym == tmp.type->map.key[i].data.sym) {
        if (! tag_init_copy(tmp.tag + i, &v->tag))
          goto ko;
        goto next;
      }
      k = list_next(k);
      v = list_next(v);
    }
    if (! tag_init_copy(tmp.tag + i, tmp.type->map.value + i))
      goto ko;
  next:
    i++;
  }
  *s = tmp;
  return s;
 ko:
  struct_clean(&tmp);
  return NULL;
}

s_struct * struct_init_with_data (s_struct *s, const s_sym *module,
                                  void *data, bool free_data)
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
  tmp.free_data = free_data;
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

s_struct * struct_new_1 (const char *p)
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

uw * struct_offset (const s_struct *s, const s_sym * const *key,
                    uw *dest)
{
  uw i = 0;
  assert(s);
  assert(s->type);
  assert(key);
  if (! struct_find_key_index(s, *key, &i)) {
    err_write_1("struct_offset: key not found: ");
    err_inspect_sym(key);
    err_write_1("\n");
    return NULL;
  }
  assert(i < s->type->map.count);
  *dest = s->type->offset[i];
  return dest;
}

s_struct * struct_put (const s_struct *s, const s_sym *key,
                       const s_tag *value, s_struct *dest)
{
  s_struct tmp;
  if (! struct_init_copy(&tmp, s)) {
    err_puts("struct_put: struct_init_copy");
    return NULL;
  }
  if (! struct_set(&tmp, key, value)) {
    err_puts("struct_put: struct_set");
    struct_clean(&tmp);
    return NULL;
  }
  *dest = tmp;
  return dest;
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
      if (! tag_type(s->type->map.value + i, &type_sym)) {
        err_puts("struct_set: tag_type");
        assert(! "struct_set: tag_type");
        return NULL;
      }
      if (type_sym == &g_sym_Var)
        type_sym = s->type->map.value[i].data.var.type;
      data = (s8 *) s->data + s->type->offset[i];
      if (! tag_to_const_pointer(value, type_sym, &data_src)) {
        err_puts("struct_set: tag_to_const_pointer");
        assert(! "struct_set: tag_to_const_pointer");
        return NULL;
      }
      data_clean(type_sym, data);
      if (! data_init_copy(type_sym, data, data_src)) {
        err_puts("struct_set: data_init_copy");
        assert(! "struct_set: data_init_copy");
        return NULL;
      }
      return s;
    }
    i++;
  }
  err_write_1("struct_set: key not found: ");
  err_inspect_sym(&key);
  err_write_1("\n");
  assert(! "struct_set: key not found");
  return NULL;
}
