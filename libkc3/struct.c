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
#include "buf.h"
#include "buf_parse.h"
#include "data.h"
#include "env.h"
#include "env_eval.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "mutex.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s_tag * struct_access (s_struct *s, s_list *key, s_tag *dest)
{
  s_tag *first;
  s_list *next;
  s_tag *r;
  s_tag tag;
  assert(s);
  assert(key);
  assert(dest);
  first = &key->tag;
  next = list_next(key);
  if (first->type != TAG_PSYM) {
    err_write_1("struct_access: key is not a Sym: (");
    err_inspect_struct(s);
    err_write_1(", ");
    err_inspect_tag(first);
    err_write_1(")\n");
    assert(! "struct_access: key is not a Sym");
    return NULL;
  }
  if (! struct_access_sym(s, first->data.td_psym, &tag)) {
    err_write_1("struct_access: struct_access_sym(");
    err_inspect_struct(s);
    err_write_1(", ");
    err_inspect_tag(first);
    err_write_1(")\n");
    err_stacktrace();
    assert(! "struct_access: struct_access_sym");
    return NULL;
  }
  if (! next) {
    *dest = tag;
    return dest;
  }
  r = kc3_access(&tag, &next, dest);
  tag_clean(&tag);
  return r;
}

s_tag * struct_access_sym (s_struct *s, const s_sym *key, s_tag *dest)
{
  void *data;
  s_struct_type *st;
  const s_sym *type;
  s_tag tmp = {0};
  void *tmp_data;
  if (! struct_get_var_type(s, key, &type))
    return NULL;
  data = struct_get_w(s, key);
  if (! data)
    return NULL;
  if (type != &g_sym_Tag) {
    if (! sym_to_tag_type(type, &tmp.type))
      return NULL;
    if (sym_is_pointer_type(type, NULL)) {
      tmp.data.td_pointer.ptr.p_pvoid = *(void **) data;
      tmp.data.td_pointer.target_type = sym_pointer_to_target_type(type);
      tmp.data.td_pointer.pointer_type = type;
      *dest = tmp;
      return dest;
    }
    if (type != &g_sym_Time) {
      if (! pstruct_type_find(type, &st))
        return NULL;
      if (st) {
        tag_init_pstruct_with_type(&tmp, st);
        if (! struct_allocate(tmp.data.td_pstruct))
          return NULL;
      }
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
  assert(s->pstruct_type);
  if (s->data) {
    err_puts("struct_allocate: data != NULL");
    assert(! "struct_allocate: data != NULL");
    return NULL;
  }
  tmp = *s;
  tmp.data = alloc(tmp.pstruct_type->size);
  if (! tmp.data)
    return NULL;
  tmp.free_data = true;
  *s = tmp;
  return s;
}

void struct_clean (s_struct *s)
{
  s_list args = {0};
  s_env *env = NULL;
  uw i;
  s_tag result = {0};
  const s_sym *type;
  assert(s);
  assert(s->pstruct_type);
  env = env_global();
  assert(env);
#if HAVE_PTHREAD
  mutex_clean(&s->mutex);
#endif
  if (s->data) {
    if (s->pstruct_type->clean) {
      list_init_ptr(&args, s->data, NULL);
      if (! env_eval_call_callable_args(env, s->pstruct_type->clean,
                                        &args, &result)) {
        err_puts("struct_clean: env_eval_call_callable_args");
        assert(! "struct_clean: env_eval_call_callable_args");
        list_clean(&args);
      }
      tag_clean(&result);
    }
    else {
      if (s->pstruct_type->must_clean) {
        i = 0;
        while (i < s->pstruct_type->map.count) {
          if (! tag_type_var(s->pstruct_type->map.value + i, &type))
            goto ko;
          data_clean(type, (s8 *) s->data + s->pstruct_type->offset[i]);
        ko:
          i++;
        }
      }
    }
    if (s->free_data)
      alloc_free(s->data);
  }
  if (s->tag) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      tag_clean(s->tag + i);
      i++;
    }
    alloc_free(s->tag);
  }
  struct_type_delete(s->pstruct_type);
}

void struct_delete (s_struct *s)
{
  s_list *l;
  p_list *prev;
  assert(s);
  if (env_cleaning(false)) {
    l = env_global()->freelist;
    while (l) {
      if (l->tag.type == TAG_PSTRUCT &&
          l->tag.data.td_pstruct == s)
        break;
      l = list_next(l);
    }
    if (! l) {
      l = list_new_struct(s, env_global()->freelist);
      if (l)
        env_global()->freelist = l;
    }
  }
  if (env_global()->pass_by_copy)
    assert(s->ref_count == 1);
  else {
#if HAVE_PTHREAD
    mutex_lock(&s->mutex);
#endif
    if (s->ref_count <= 0) {
      err_puts("struct_delete: invalid reference count");
      assert(! "struct_delete: invalid reference count");
#if HAVE_PTHREAD
      mutex_unlock(&s->mutex);
#endif
      return;
    }
    if (--s->ref_count) {
#if HAVE_PTHREAD
      mutex_unlock(&s->mutex);
#endif
      return;
    }
#if HAVE_PTHREAD
    mutex_unlock(&s->mutex);
#endif
  }
  struct_clean(s);
  if (env_cleaning(false)) {
    prev = &env_global()->freelist;
    while (*prev) {
      if ((*prev)->tag.type == TAG_PSTRUCT &&
          (*prev)->tag.data.td_pstruct == s) {
        l = *prev;
        *prev = list_next(l);
        alloc_free(l);
        break;
      }
      prev = &(*prev)->next.data.td_plist;
    }
  }
  alloc_free(s);
}

uw * struct_find_key_index (const s_struct *s, const s_sym *key,
                            uw *dest)
{
  return struct_type_find_key_index(s->pstruct_type, key, dest);
}

const void * struct_get (const s_struct *s, const s_sym *key)
{
  uw i = 0;
  assert(s);
  assert(key);
  if (! struct_find_key_index(s, key, &i))
    return NULL;
  assert(i < s->pstruct_type->map.count);
  return (u8 *) s->data + s->pstruct_type->offset[i];
}

const s_sym ** struct_get_sym (const s_struct *s, const s_sym *key)
{
  return (const s_sym **) struct_get(s, key);
}

s_tag * struct_get_tag (s_struct *s, const s_sym *key)
{
  return (s_tag *) struct_get(s, key);
}

const s_sym ** struct_get_type (const s_struct *s, const s_sym *key,
                                const s_sym **dest)
{
  s_tag tag_key;
  const s_sym *tmp = NULL;
  tag_init_psym(&tag_key, key);
  if (! map_get_type(&s->pstruct_type->map, &tag_key, &tmp))
    return NULL;
  *dest = tmp;
  return NULL;
}

u8 struct_get_u8 (const s_struct *s, const s_sym *key)
{
  return *(u8 *) struct_get(s, key);
}

uw struct_get_uw (const s_struct *s, const s_sym *key)
{
  return *(uw *) struct_get(s, key);
}

const s_sym ** struct_get_var_type (const s_struct *s, const s_sym *key,
                                    const s_sym **dest)
{
  s_tag tag_key;
  tag_init_psym(&tag_key, key);
  return map_get_var_type(&s->pstruct_type->map, &tag_key, dest);
}

void * struct_get_w (s_struct *s, const s_sym *key)
{
  uw i = 0;
  assert(s);
  assert(key);
  if (! s->data) {
    err_puts("struct_get_w: struct is not allocated");
    assert(! "struct_get_w: struct is not allocated");
    return NULL;
  }
  if (! struct_find_key_index(s, key, &i))
    return NULL;
  assert(i < s->pstruct_type->map.count);
  return (u8 *) s->data + s->pstruct_type->offset[i];
}

s_struct * struct_init (s_struct *s, const s_sym *module)
{
  p_struct_type st = NULL;
  assert(s);
  *s = (s_struct) {0};
  if (module) {
    if (! pstruct_type_find(module, &st)) {
      err_write_1("struct_init: struct_type_find(");
      err_inspect_sym(module);
      err_puts(")");
      assert("struct_init: struct_type_find");
      return NULL;
    }
    if (! st) {
      err_write_1("struct_init: struct_type not found: ");
      err_inspect_sym(module);
      err_write_1("\n");
      assert(! "struct_init: struct_type not found");
      return NULL;
    }
    if (! pstruct_type_init_copy(&s->pstruct_type, &st)) {
      err_write_1("struct_init: pstruct_type_init_copy: ");
      err_inspect_sym(module);
      err_write_1("\n");
      assert("struct_init: pstruct_type_init_copy");
      return NULL;
    }
  }
  s->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&s->mutex);
#endif
  return s;
}

s_struct * struct_init_copy (s_struct *s, s_struct *src)
{
  uw i;
  s_tag *key = NULL;
  const s_sym *type;
  s_tag *value = NULL;
  assert(s);
  assert(src);
  assert(src->pstruct_type);
  *s = (s_struct) {0};
  s->ref_count = 1;
  if (! pstruct_type_init_copy(&s->pstruct_type, &src->pstruct_type))
    return NULL;
  if (src->data) {
    s->data = alloc(s->pstruct_type->size);
    if (! s->data)
      return NULL;
    s->free_data = true;
    i = 0;
    while (i < s->pstruct_type->map.count) {
      key = s->pstruct_type->map.key + i;
      value = s->pstruct_type->map.value + i;
      if (key->data.td_psym->str.ptr.p_pchar[0] != '_') {
        if (value->type == TAG_PVAR)
          type = value->data.td_pvar->type;
        else if (! tag_type(value, &type))
          goto ko;
        if (! data_init_copy(type, (s8 *) s->data +
                             s->pstruct_type->offset[i],
                             (s8 *) src->data +
                             s->pstruct_type->offset[i]))
          goto ko;
      }
      i++;
    }
  }
  else if (src->tag) {
    s->tag = alloc(s->pstruct_type->map.count * sizeof(s_tag));
    if (! s->tag)
      return NULL;
    i = 0;
    while (i < s->pstruct_type->map.count) {
      if (! tag_init_copy(s->tag + i, src->tag + i))
        goto ko;
      i++;
    }
  }
#if HAVE_PTHREAD
  mutex_init(&s->mutex);
#endif
  return s;
 ko:
  struct_clean(s);
  return NULL;
}

s_struct * struct_init_from_lists (s_struct *s, const s_sym *module,
                                   s_list *keys,
                                   s_list *values)
{
  uw i;
  s_list *k;
  s_list *v;
  assert(s);
  assert(module);
  assert(list_length(keys) == list_length(values));
  if (! struct_init(s, module))
    return NULL;
  s->tag = alloc(s->pstruct_type->map.count * sizeof(s_tag));
  if (! s->tag)
    return NULL;
  i = 0;
  while (i < s->pstruct_type->map.count) {
    k = keys;
    v = values;
    while (k && v) {
      if (k->tag.type != TAG_PSYM) {
        err_write_1("struct_init_from_lists: key that is not a"
                    " symbol: ");
        err_puts(tag_type_to_string(k->tag.type));
        assert(! "struct_init_from_lists: key that is not a symbol");
        goto ko;
      }
      if (k->tag.data.td_psym->str.ptr.p_pchar[0] != '_') {
        if (k->tag.data.td_psym ==
            s->pstruct_type->map.key[i].data.td_psym) {
          if (! tag_init_copy(s->tag + i, &v->tag))
            goto ko;
          goto next;
        }
      }
      k = list_next(k);
      v = list_next(v);
    }
    if (! tag_init_copy(s->tag + i, s->pstruct_type->map.value + i))
      goto ko;
  next:
    i++;
  }
  return s;
 ko:
  struct_clean(s);
  return NULL;
}

s_struct * struct_init_with_data (s_struct *s, const s_sym *module,
                                  void *data, bool free_data)
{
  p_struct_type st;
  assert(s);
  assert(module);
  if (! pstruct_type_find(module, &st))
    return NULL;
  if (! st) {
    err_write_1("struct_init_with_data: struct_type not found: ");
    err_inspect_sym(module);
    err_write_1("\n");
    assert(! "struct_init_with_data: struct_type not found");
    return NULL;
  }
  *s = (s_struct) {0};
  if (! pstruct_type_init_copy(&s->pstruct_type, &st))
    return NULL;
  s->data = data;
  s->free_data = free_data;
  s->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&s->mutex);
#endif
  return s;
}

s_struct * struct_init_with_type (s_struct *s, s_struct_type *st)
{
  assert(s);
  assert(st);
  *s = (s_struct) {0};
  pstruct_type_init_copy(&s->pstruct_type, &st);
  s->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&s->mutex);
#endif
  return s;
}

s_struct * struct_new (const s_sym *module)
{
  s_struct *s;
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
  if (! struct_init(s, module)) {
    alloc_free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_new_copy (s_struct *src)
{
  s_struct *s;
  assert(src);
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
  if (! struct_init_copy(s, src)) {
    alloc_free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_new_ref (s_struct *s)
{
  assert(s);
#if HAVE_PTHREAD
  mutex_lock(&s->mutex);
#endif
  if (s->ref_count <= 0) {
    err_puts("struct_new_ref: invalid reference count");
    assert(! "struct_new_ref: invalid reference count");
    return NULL;
  }
  s->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&s->mutex);
#endif
  return s;
}

s_struct * struct_new_with_data (const s_sym *module, void *data,
                                 bool free_data)
{
  s_struct *s;
  assert(module);
  assert(data);
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
  if (! struct_init_with_data(s, module, data, free_data)) {
    alloc_free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_new_with_type (s_struct_type *st)
{
  s_struct *s;
  assert(st);
  s = alloc(sizeof(s_struct));
  if (! s)
    return NULL;
  if (! struct_init_with_type(s, st)) {
    alloc_free(s);
    return NULL;
  }
  return s;
}

uw * struct_offset (const s_struct *s, const s_sym * const *key,
                    uw *dest)
{
  uw i = 0;
  assert(s);
  assert(s->pstruct_type);
  assert(key);
  if (! struct_find_key_index(s, *key, &i)) {
    err_write_1("struct_offset: key not found: ");
    err_inspect_psym(key);
    err_write_1("\n");
    return NULL;
  }
  assert(i < s->pstruct_type->map.count);
  *dest = s->pstruct_type->offset[i];
  return dest;
}

s_struct * struct_set (s_struct *s, const s_sym *key,
                       s_tag *value)
{
  void *data;
  void *data_src;
  uw i;
  const s_sym *type_sym;
  assert(s);
  assert(s->pstruct_type->map.count);
  assert(key);
  assert(value);
  i = 0;
  while (i < s->pstruct_type->map.count) {
    if (s->pstruct_type->map.key[i].type != TAG_PSYM) {
      err_puts("struct_set: struct type map key is not a Sym");
      assert(! "struct_set: struct type map key is not a Sym");
      return NULL;
    }
    if (s->pstruct_type->map.key[i].data.td_psym == key) {
      if (s->pstruct_type->map.key[i].data.td_psym->str.ptr.p_pchar[0]
          == '_') {
        err_write_1("struct_set: cannot set read only member ");
        err_inspect_sym(s->pstruct_type->map.key[i].data.td_psym);
        err_write_1("\n");
        assert(! "struct_set: cannot set read only member");
        return NULL;
      }
      if (! tag_type(s->pstruct_type->map.value + i, &type_sym)) {
        err_puts("struct_set: tag_type");
        assert(! "struct_set: tag_type");
        return NULL;
      }
      if (type_sym == &g_sym_Var)
        type_sym = s->pstruct_type->map.value[i].data.td_pvar->type;
      data = (s8 *) s->data + s->pstruct_type->offset[i];
      if (! tag_to_pointer(value, type_sym, &data_src)) {
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
  err_inspect_sym(key);
  err_write_1("\n");
  assert(! "struct_set: key not found");
  return NULL;
}
