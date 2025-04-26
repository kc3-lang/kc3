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
#include "data.h"
#include "env.h"
#include "list.h"
#include "map.h"
#include "mutex.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_init.h"
#include "tag_type.h"

void struct_type_clean (s_struct_type *st)
{
  assert(st);
  if (false) {
    err_write_1("struct_type_clean: ");
    err_inspect_struct_type(st);
    err_write_1("\n");
    err_stacktrace();
    abort();
  }
  map_clean(&st->map);
  free(st->offset);
}

void * struct_type_copy_data (const s_struct_type *st, void *dest,
                              const void *src)
{
  uw count;
  uw i;
  uw offset;
  const s_sym *type;
  assert(st);
  assert(dest);
  assert(src);
  i = 0;
  count = st->map.count;
  while (i < count) {
    if (st->map.value[i].type == TAG_VAR)
      type = st->map.value[i].data.var.type;
    else
      if (! tag_type(st->map.value + i, &type))
        return NULL;
    offset = st->offset[i];
    if (! data_init_copy(type, (s8 *) dest + offset,
                         (s8 *) src + offset))
      return NULL;
    i++;
  }
  return dest;
}

void struct_type_delete (s_struct_type *st)
{
  assert(st);
  if (env_global()->pass_by_copy)
    assert(st->ref_count == 1);
  else {
#if HAVE_PTHREAD
    mutex_lock(&st->mutex);
#endif
    if (st->ref_count <= 0) {
      err_puts("struct_type_delete: invalid reference count");
      assert(! "struct_type_delete: invalid reference count");
      abort();
    }
    if (--st->ref_count) {
#if HAVE_PTHREAD
      mutex_unlock(&st->mutex);
#endif
      return;
    }
#if HAVE_PTHREAD
    mutex_unlock(&st->mutex);
#endif
  }
  struct_type_clean(st);
  free(st);
}

bool * struct_type_exists (const s_sym *module, bool *dest)
{
  return env_struct_type_exists(env_global(), module, dest);
}

s_struct_type ** struct_type_find (const s_sym *module,
                                   s_struct_type **dest)
{
  return env_struct_type_find(env_global(), module, dest);
}

uw * struct_type_find_key_index (const s_struct_type *st,
                                 const s_sym *key, uw *dest)
{
  uw i = 0;
  assert(st);
  assert(key);
  while (i < st->map.count) {
    assert(st->map.key[i].type == TAG_SYM);
    if (st->map.key[i].data.sym == key) {
      *dest = i;
      return dest;
    }
    i++;
  }
  return NULL;
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
  if (false) {
    err_write_1("struct_type_init: ");
    err_inspect_sym(&module);
    err_write_1("\n");
  }
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
    if (tuple->tag[1].type == TAG_VAR) {
      type = tuple->tag[1].data.var.type;
      if (! sym_type_size(&type, &size)) {
        map_clean(&tmp.map);
        free(tmp.offset);
        return NULL;
      }
    }
    else if (! tag_size(tuple->tag + 1, &size)) {
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
  if (sizeof(long) == 4 && ! WIN64)
    tmp.size = (offset + 3) / 4 * 4;
  else {
#ifdef __APPLE__
    tmp.size = (offset + 7) / 8 * 8;
#else
    tmp.size = (offset + 15) / 16 * 16;
#endif
  }
  tmp.ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
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
  tmp.ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
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

s_struct_type * struct_type_new_copy (const s_struct_type *src)
{
  s_struct_type *st;
  assert(src);
  st = alloc(sizeof(s_struct_type));
  if (! st)
    return NULL;
  if (! struct_type_init_copy(st, src)) {
    free(st);
    return NULL;
  }
  return st;
}

s_struct_type * struct_type_new_ref (s_struct_type *st)
{
  assert(st);
#if HAVE_PTHREAD
  mutex_lock(&st->mutex);
#endif
  if (st->ref_count <= 0) {
    err_puts("struct_type_new_ref: invalid reference count");
    assert(! "struct_type_new_ref: invalid reference count");
#if HAVE_PTHREAD
    mutex_unlock(&st->mutex);
#endif
    return NULL;
  }
  st->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&st->mutex);
#endif
  return st;
}

uw struct_type_padding (uw offset, uw size)
{
  unsigned int align = 1;
  if (size >= 8)
    align = 8;
  return (offset + (align - 1)) / align * align;
}
