/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "callable.h"
#include "data.h"
#include "env.h"
#include "list.h"
#include "map.h"
#include "mutex.h"
#include "pcallable.h"
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
  if (st->clean)
    callable_delete(st->clean);
#if HAVE_PTHREAD
  mutex_clean(&st->mutex);
#endif
}

uw struct_type_compute_size (uw offset, u8 align_max)
{
  if (false) {
    err_write_1("struct_type_compute_size: offset=");
    err_inspect_uw_decimal(offset);
    err_write_1(" align_max=");
    err_inspect_u8_decimal(align_max);
    err_write_1("\n");
  }
  return (offset + (align_max - 1)) / align_max * align_max;
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
    if (st->map.value[i].type == TAG_PVAR)
      type = st->map.value[i].data.pvar->type;
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
    if (! env_cleaning(false) && --st->ref_count) {
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

uw * struct_type_find_key_index (const s_struct_type *st,
                                 const s_sym *key, uw *dest)
{
  uw i = 0;
  assert(st);
  assert(key);
  while (i < st->map.count) {
    assert(st->map.key[i].type == TAG_PSYM);
    if (st->map.key[i].data.psym == key) {
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
  u8 align;
  uw count;
  uw i;
  bool must_clean = false;
  uw offset;
  const s_list *s;
  uw size;
  const s_tuple *tuple;
  const s_sym *type;
  assert(st);
  assert(module);
  if (false) {
    err_write_1("struct_type_init: ");
    err_inspect_sym(module);
    err_write_1("\n");
  }
  count = list_length(spec);
  *st = (s_struct_type) {0};
  st->align_max = 1;
  st->module = module;
  if (count) {
    if (! map_init(&st->map, count))
      return NULL;
    st->offset = alloc(count * sizeof(uw));
    if (! st->offset) {
      map_clean(&st->map);
      return NULL;
    }
    st->must_clean = false;
    offset = 0;
    i = 0;
    s = spec;
    while (s) {
      if (s->tag.type != TAG_TUPLE || s->tag.data.tuple.count != 2) {
        err_puts("struct_type_init: invalid spec");
        assert(! "struct_type_init: invalid spec");
        map_clean(&st->map);
        free(st->offset);
        return NULL;
      }
      tuple = &s->tag.data.tuple;
      if (tuple->tag[1].type == TAG_PVAR) {
        type = tuple->tag[1].data.pvar->type;
        if (! sym_type_size(type, &size)) {
          map_clean(&st->map);
          free(st->offset);
          return NULL;
        }
      }
      else if (! tag_size(tuple->tag + 1, &size)) {
        map_clean(&st->map);
        free(st->offset);
        return NULL;
      }
      tag_init_copy(st->map.key + i,   tuple->tag + 0);
      tag_init_copy(st->map.value + i, tuple->tag + 1);
      if (! tag_alignment(st->map.value + i, &align)) {
        map_clean(&st->map);
        free(st->offset);
        return NULL;
      }
      tag_type_var(st->map.value + i, &type);
      if (! sym_must_clean(type, &must_clean)) {
        map_clean(&st->map);
        free(st->offset);
        return NULL;
      }
      if (must_clean)
        st->must_clean = true;
      offset = struct_type_padding(offset, align, &st->align_max);
      st->offset[i] = offset;
      offset += size;
      i++;
      s = list_next(s);
    }
    st->size = struct_type_compute_size(offset, st->align_max);
  }
  st->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&st->mutex);
#endif
  return st;
}

s_struct_type * struct_type_init_copy (s_struct_type *st,
                                       s_struct_type *src)
{
  assert(st);
  assert(src);
  assert(src->module);
  *st = (s_struct_type) {0};
  if (src->clean &&
      ! pcallable_init_copy(&st->clean, &src->clean)) {
    err_puts("struct_type_init_copy: pcallable_init_copy clean");
    assert(! "struct_type_init_copy: pcallable_init_copy clean");
    return NULL;
  }
  if (! map_init_copy(&st->map, &src->map))
    return NULL;
  st->module = src->module;
  st->must_clean = src->must_clean;
  if (st->map.count) {
    st->offset = alloc(st->map.count * sizeof(uw));
    if (! st->offset) {
      map_clean(&st->map);
      return NULL;
    }
    memcpy(st->offset, src->offset, st->map.count * sizeof(uw));
  }
  st->size = src->size;
  st->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&st->mutex);
#endif
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

s_struct_type * struct_type_new_copy (s_struct_type *src)
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

uw struct_type_padding (uw offset, u8 align, u8 *max)
{
  if (align < 1)
    align = 1;
  if (align > *max)
    *max = align;
  return (offset + (align - 1)) / align * align;
}

s_struct_type * struct_type_update_map (s_struct_type *st)
{
  u8 align;
  uw i;
  bool must_clean;
  uw offset = 0;
  uw size = 0;
  s_struct_type tmp;
  const s_sym *type;
  assert(st);
  assert(st->map.count);
  tmp = *st;
  tmp.align_max = 1;
  if (! (tmp.offset = alloc(tmp.map.count * sizeof(uw))))
    return NULL;
  tmp.must_clean = false;
  i = 0;
  while (i < tmp.map.count) {
    if (! tag_type_var(tmp.map.value + i, &type)) {
      free(tmp.offset);
      return NULL;
    }
    if (tmp.map.value[i].type == TAG_PVAR) {
      if (! sym_type_size(type, &size)) {
        free(tmp.offset);
        return NULL;
      }
      if (! sym_type_alignment(type, &align)) {
        free(tmp.offset);
        return NULL;
      }
    }
    else {
      if (! tag_size(tmp.map.value + i, &size)) {
        free(tmp.offset);
        return NULL;
      }
      if (! tag_alignment(tmp.map.value + i, &align)) {
        free(tmp.offset);
        return NULL;
      }
    }
    offset = struct_type_padding(offset, align, &tmp.align_max);
    tmp.offset[i] = offset;
    offset += size;
    tag_type_var(tmp.map.value + i, &type);
    if (! sym_must_clean(type, &must_clean)) {
      free(tmp.offset);
      return NULL;
    }
    if (must_clean)
      tmp.must_clean = true;
    i++;
  }
  tmp.size = struct_type_compute_size(offset, tmp.align_max);
  *st = tmp;
  return st;
}
