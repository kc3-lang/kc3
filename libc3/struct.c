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

s_struct * struct_allocate (s_struct *s)
{
  s_struct tmp;
  assert(s);
  assert(! s->data);
  tmp = *s;
  tmp.free_data = true;
  tmp.data = calloc(1, tmp.type.size);
  if (! tmp.data) {
    warn("struct_allocate: data");
    assert(! "struct_allocate: data: failed to allocate memory");
    return NULL;
  }
  *s = tmp;
  return s;
}

void struct_clean (s_struct *s)
{
  f_clean clean;
  uw i;
  const s_sym *sym;
  assert(s);
  if (s->data) {
    i = 0;
    while (i < s->type.map.count) {
      if (tag_type(s->type.map.value + i, &sym) &&
          sym_to_clean(sym, &clean) &&
          clean)
        clean((s8 *) s->data + s->type.offset[i]);
      i++;
    }
    if (s->free_data)
      free(s->data);
  }
  if (s->tag) {
    i = 0;
    while (i < s->type.map.count) {
      tag_clean(s->tag + i);
      i++;
    }
    free(s->tag);
  }
  struct_type_clean(&s->type);
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
  while (i < s->type.map.count) {
    assert(s->type.map.key[i].type == TAG_SYM);
    if (s->type.map.key[i].data.sym == key) {
      *dest = i;
      return true;
    }
    i++;
  }
  return false;
}

s_struct * struct_init (s_struct *s, const s_sym *module)
{
  s_struct tmp = {0};
  assert(s);
  assert(module);
  if (! struct_type_init_from_env(&tmp.type, module, &g_c3_env))
    return NULL;
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

s_struct * struct_init_cast (s_struct *s, const s_tag *tag)
{
  assert(s);
  assert(tag);
  switch (tag->type) {
  case TAG_STRUCT:
    return struct_init_copy(s, &tag->data.struct_);
  default:
    break;
  }
  warnx("struct_init_cast: cannot cast %s to Struct",
        tag_type_to_string(tag->type));
  //assert(! "struct_init_cast: cannot cast to Struct");
  return NULL;
}

s_struct * struct_init_copy (s_struct *s, const s_struct *src)
{
  f_init_copy init_copy;
  uw i;
  uw size;
  const s_sym *sym;
  s_struct tmp = {0};
  assert(s);
  assert(src);
  if (! struct_type_init_copy(&tmp.type, &src->type))
    return NULL;
  if (src->data) {
    tmp.free_data = true;
    tmp.data = calloc(1, tmp.type.size);
    i = 0;
    while (i < tmp.type.map.count) {
      if (tag_type(tmp.type.map.value + i, &sym)) {
        if (! sym_to_init_copy(sym, &init_copy))
          goto ko;
        if (init_copy) {
          if (! init_copy((s8 *) tmp.data + tmp.type.offset[i],
                          (s8 *) src->data + tmp.type.offset[i]))
            goto ko;
        }
        else {
          if (! tag_size(tmp.type.map.value + i, &size))
            goto ko;
          if (size)
            memcpy((s8 *) tmp.data + tmp.type.offset[i],
                   (s8 *) src->data + tmp.type.offset[i],
                   size);
        }
      }
      i++;
    }
  }
  if (src->tag) {
    tmp.tag = calloc(tmp.type.map.count, sizeof(s_tag));
    i = 0;
    while (i < tmp.type.map.count) {
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
  tmp.tag = calloc(tmp.type.map.count, sizeof(s_tag));
  if (! tmp.tag) {
    warn("struct_init_from_lists: tag");
    assert(! "struct_init_from_lists: failed to allocate memory");
    return NULL;
  }
  k = keys;
  v = values;
  while (k && v) {
    assert(k->tag.type == TAG_SYM);
    if (k->tag.type != TAG_SYM) {
      warnx("struct_init_from_lists: key that is not a symbol: %s",
            tag_type_to_string(k->tag.type));
      assert(! "struct_init_from_lists: key that is not a symbol");
      goto ko;
    }
    if (! struct_find_key_index(&tmp, k->tag.data.sym, &i)) {
      warnx("struct_init_from_lists: cannot find key in defstruct: %s",
            k->tag.data.sym->str.ptr.ps8);
      assert(! "struct_init_from_lists: cannot find key in defstruct");
      goto ko;
    }
    if (! tag_init_copy(tmp.tag + i, &v->tag))
      goto ko;
    k = list_next(k);
    v = list_next(v);
  }
  i = 0;
  while (i < tmp.type.map.count) {
    if (! tmp.tag[i].type)
      if (! tag_init_copy(tmp.tag + i, tmp.type.map.value + i))
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
                                  bool free_data, void *data)
{
  s_struct tmp = {0};
  assert(s);
  assert(module);
  if (! struct_type_init_from_env(&tmp.type, module, &g_c3_env))
    return NULL;
  tmp.free_data = free_data;
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
  uw size;
  e_tag_type   type;
  const s_sym *type_sym;
  assert(s);
  assert(s->type.map.count);
  assert(key);
  assert(value);
  i = 0;
  while (i < s->type.map.count) {
    if (s->type.map.key[i].type == TAG_SYM &&
        s->type.map.key[i].data.sym == key) {
      type = s->type.map.value[i].type;
      if (! tag_type_to_clean(type, &clean) ||
          ! tag_type_to_init_copy(type, &init_copy) ||
          ! tag_type(s->type.map.value + i, &type_sym))
        return NULL;
      data = (s8 *) s->data + s->type.offset[i];
      if (! tag_to_const_pointer(value, type_sym, &data_src))
        return NULL;
      if (clean)
        clean(data);
      if (init_copy) {
        if (! init_copy(data, data_src))
          return NULL;
        return s;
      }
      else {
        if (! tag_size(s->type.map.value + i, &size))
          return NULL;
        if (size)
          memcpy((s8 *) data + s->type.offset[i], data_src, size);
      }
    }
    i++;
  }
  return NULL;
}
