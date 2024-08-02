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
#include "data.h"
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

void * struct_type_copy_data (const s_struct_type *st, void *dest,
                              const void *src)
{
  uw count;
  uw i;
  uw offset;
  const s_sym *sym;
  assert(st);
  assert(dest);
  assert(src);
  i = 0;
  count = st->map.count;
  while (i < count) {
    if (! tag_type(st->map.value + i, &sym))
      return NULL;
    offset = st->offset[i];
    if (! data_init_copy(sym, (s8 *) dest + offset,
                         (s8 *) src + offset))
      return NULL;
    i++;
  }
  return dest;
}

void struct_type_delete (s_struct_type *st)
{
  assert(st);
  struct_type_clean(st);
  free(st);
}

bool * struct_type_exists (const s_sym *module, bool *dest)
{
  return env_struct_type_exists(&g_kc3_env, module, dest);
}

const s_struct_type ** struct_type_find (const s_sym *module,
                                         const s_struct_type **dest)
{
  return env_struct_type_find(&g_kc3_env, module, dest);
}

s_struct_type * struct_type_init (s_struct_type *st,
                                  const s_sym *module,
                                  const s_list *spec)
{
  uw count;
  uw i;
  bool must_clean = false;
  uw offset;
  const s_list *s;
  uw size;
  s_struct_type tmp = {0};
  const s_tuple *tuple;
  const s_sym *type;
  assert(st);
  assert(module);
  assert(spec);
  count = list_length(spec);
  tmp.module = module;
  if (! map_init(&tmp.map, count))
    return NULL;
  tmp.offset = alloc(count * sizeof(uw));
  if (! tmp.offset) {
    map_clean(&tmp.map);
    return NULL;
  }
  tmp.must_clean = false;
  offset = 0;
  i = 0;
  s = spec;
  while (s) {
    if (s->tag.type != TAG_TUPLE || s->tag.data.tuple.count != 2) {
      err_puts("struct_type_init: invalid spec");
      assert(! "struct_type_init: invalid spec");
      map_clean(&tmp.map);
      free(tmp.offset);
      return NULL;
    }
    tuple = &s->tag.data.tuple;
    if (! tag_size(tuple->tag + 1, &size)) {
      map_clean(&tmp.map);
      free(tmp.offset);
      return NULL;
    }
    tag_init_copy(tmp.map.key + i,   tuple->tag + 0);
    tag_init_copy(tmp.map.value + i, tuple->tag + 1);
    tag_type(tmp.map.value + i, &type);
    if (! sym_must_clean(type, &must_clean)) {
      map_clean(&tmp.map);
      free(tmp.offset);
      return NULL;
    }
    if (must_clean)
      tmp.must_clean = true;
    offset = struct_type_padding(offset, size);
    tmp.offset[i] = offset;
    offset += size;
    i++;
    s = list_next(s);
  }
  tmp.size = offset;
  *st = tmp;
  return st;
}

s_struct_type * struct_type_init_cast (s_struct_type *st,
                                       const s_sym * const *type,
                                       const s_tag *tag)
{
  assert(st);
  assert(tag);
  switch (tag->type) {
  case TAG_STRUCT_TYPE:
    if (*type == &g_sym_StructType)
      return struct_type_init_copy(st, &tag->data.struct_type);
  default:
    break;
  }
  err_write_1("struct_type_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to StructType");
  assert(! "struct_type_init_cast: cannot cast to StructType");
  return NULL;
}

s_struct_type * struct_type_init_update_clean (s_struct_type *st,
                                               const s_struct_type *src,
                                               const s_cfn *clean)
{
  s_struct_type tmp = {0};
  assert(st);
  assert(src);
  assert(clean);
  if (! struct_type_init_copy(&tmp, src))
    return NULL;
  tmp.clean = (f_clean) clean->ptr.f;
  *st = tmp;
  return st;
}

s_struct_type * struct_type_init_copy (s_struct_type *st,
                                       const s_struct_type *src)
{
  s_struct_type tmp = {0};
  assert(st);
  assert(src);
  assert(src->module);
  assert(src->map.count);
  tmp.clean = src->clean;
  if (! map_init_copy(&tmp.map, &src->map))
    return NULL;
  tmp.module = src->module;
  tmp.must_clean = src->must_clean;
  tmp.offset = alloc(tmp.map.count * sizeof(uw));
  if (! tmp.offset) {
    map_clean(&tmp.map);
    return NULL;
  }
  memcpy(tmp.offset, src->offset, tmp.map.count * sizeof(uw));
  tmp.size = src->size;
  *st = tmp;
  return st;
}

s_struct_type * struct_type_init_from_env (s_struct_type *st,
                                           const s_sym *module,
                                           s_env *env)
{
  s_list *spec;
  assert(st);
  assert(module);
  assert(env);
  if (! env_struct_type_get_spec(env, module, &spec) ||
      ! spec)
    return NULL;
  if (! struct_type_init(st, module, spec))
    return NULL;
  list_delete_all(spec);
  return st;
}

s_struct_type * struct_type_new (const s_sym *module,
                                 const s_list *spec)
{
  s_struct_type *st;
  assert(module);
  st = alloc(sizeof(s_struct_type));
  if (! st)
    return NULL;
  if (! struct_type_init(st, module, spec)) {
    free(st);
    return NULL;
  }
  return st;
}

uw struct_type_padding (uw offset, uw size)
{
  unsigned int align = 1;
  if (size >= 16)
    align = 16;
  else if (size >= 8)
    align = 8;
  return (offset + align - 1) / align * align;
}
