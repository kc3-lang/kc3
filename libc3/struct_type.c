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
#include "tag_init.h"
#include "tag_type.h"

void struct_type_clean (s_struct_type *st)
{
  assert(st);
  map_clean(&st->map);
  free(st->offset);
}

void struct_type_delete (s_struct_type *st)
{
  assert(st);
  struct_type_clean(st);
  free(st);
}

s_struct_type * struct_type_init (s_struct_type *st, const s_sym *module,
                                  const s_list *spec)
{
  uw count;
  uw i;
  uw offset;
  const s_list *s;
  uw size;
  const s_tuple *tuple;
  assert(st);
  assert(module);
  assert(spec);
  count = list_length(spec);
  st->module = module;
  if (! map_init(&st->map, count))
    return NULL;
  st->offset = calloc(count, sizeof(uw));
  if (! st->offset) {
    warn("struct_type_init: offset array of size %lu", count);
    map_clean(&st->map);
    return NULL;
  }
  offset = 0;
  i = 0;
  s = spec;
  while (s) {
    if (s->tag.type != TAG_TUPLE || s->tag.data.tuple.count != 2) {
      warn("struct_type_init: invalid spec");
      map_clean(&st->map);
      free(st->offset);
      return NULL;
    }
    tuple = &s->tag.data.tuple;
    if (! tag_size(tuple->tag + 1, &size)) {
      map_clean(&st->map);
      free(st->offset);
      return NULL;
    }
    tag_init_copy(st->map.key + i,   tuple->tag + 0);
    tag_init_copy(st->map.value + i, tuple->tag + 1);
    offset = struct_type_padding(offset, size);
    st->offset[i] = offset;
    offset += size;
    i++;
    s = list_next(s);
  }
  st->size = offset;
  return st;
}

s_struct_type * struct_type_init_copy (s_struct_type *s,
                                       const s_struct_type *src)
{
  s_struct_type tmp;
  assert(s);
  assert(src);
  assert(src->module);
  assert(src->map.count);
  tmp.module = src->module;
  if (! map_init_copy(&tmp.map, &src->map))
    return NULL;
  tmp.offset = calloc(tmp.map.count, sizeof(uw));
  if (! tmp.offset) {
    warn("struct_type_init_copy: offset array of size %lu",
         tmp.map.count);
    map_clean(&tmp.map);
    return NULL;
  }
  memcpy(tmp.offset, src->offset, tmp.map.count * sizeof(uw));
  tmp.size = src->size;
  *s = tmp;
  return s;
}

s_struct_type * struct_type_init_from_env (s_struct_type *st,
                                           const s_sym *module,
                                           s_env *env)
{
  s_list *spec;
  assert(st);
  assert(module);
  assert(env);
  if (! env_get_struct_type_spec(env, module, &spec) ||
      ! spec)
    return NULL;
  if (! struct_type_init(st, module, spec)) {
    list_delete_all(spec);
    return NULL;
  }
  list_delete_all(spec);
  return st;
}

s_struct_type * struct_type_new (const s_sym *module,
                                 const s_list *spec)
{
  s_struct_type *st;
  assert(module);
  st = calloc(1, sizeof(s_struct_type));
  if (! st) {
    warn("struct_type_new: %s: calloc", module->str.ptr.ps8);
    return NULL;
  }
  if (! struct_type_init(st, module, spec)) {
    free(st);
    return NULL;
  }
  return st;
}

uw struct_type_padding (uw offset, uw size)
{
  unsigned int align = 1;
  if (size == 2)
    align = 2;
  else if (size == 4)
    align = 4;
  else if (size == 8)
    align = 8;
  else if (size == 16)
    align = 16;
  return (offset + align - 1) / align * align;
}
