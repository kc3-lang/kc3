/*/home/lyzer/.local/bin/lvim kc3
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
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "assert.h"
#include "alloc.h"
#include "buf.h"
#include "buf_file.h"
#include "call.h"
#include "counter.h"
#include "endian.h"
#include "env_eval.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "file.h"
#include "ht.h"
#include "io.h"
#include "pstruct_type.h"
#include "rwlock.h"
#include "struct_type.h"
#include "sym.h"
#include "list.h"
#include "psym.h"
#include "rwlock.h"
#include "str.h"
#include "tag.h"
#include "tag_init.h"
#include "types.h"
#include "marshall.h"

#define DEF_MARSHALL(type, magic)                                     \
  s_marshall * marshall_ ## type (s_marshall *m, bool heap, type src) \
  {                                                                   \
    s_buf *buf;                                                       \
    type le;                                                          \
    sw r;                                                             \
    if (! m) {                                                        \
      err_puts("marshall_" # type ": invalid argument");              \
      assert(! "marshall_" # type ": invalid argument");              \
      return NULL;                                                    \
    }                                                                 \
    le = _Generic(src,                                                \
                  s16:     htole16(src),                              \
                  u16:     htole16(src),                              \
                  s32:     htole32(src),                              \
                  u32:     htole32(src),                              \
                  s64:     htole64(src),                              \
                  u64:     htole64(src),                              \
                  default: src);                                      \
    buf = heap ? &m->heap : &m->buf;                                  \
    if ((r = buf_write_1(buf, magic)) <= 0)                           \
      return NULL;                                                    \
    if (heap)                                                         \
      m->heap_pos += r;                                               \
    else                                                              \
      m->buf_pos += r;                                                \
    if ((r = buf_write_ ## type(buf, le)) <= 0)                       \
      return NULL;                                                    \
    if (heap)                                                         \
      m->heap_pos += r;                                               \
    else                                                              \
      m->buf_pos += r;                                                \
    return m;                                                         \
  }

#define DEF_MARSHALL_P(name, magic, type)                             \
  s_marshall * marshall_p ## name (s_marshall *m, bool heap,          \
                                   const type *data)                  \
  {                                                                   \
    assert(m);                                                        \
    bool present = false;                                             \
    if (! m || ! data) {                                              \
      err_puts("marshall_p" # name ": invalid argument");             \
      assert(! "marshall_p" # name ": invalid argument");             \
      return NULL;                                                    \
    }                                                                 \
    if (! marshall_1(m, heap, magic)) {                               \
      err_puts("marshall_p" # name ": marshall_1 magic");             \
      assert(! "marshall_p" # name ": marshall_1 magic");             \
      return NULL;                                                    \
    }                                                                 \
    if (! marshall_heap_pointer(m, heap, *data, &present)) {          \
      err_puts("marshall_p" # name ": marshall_heap_pointer");        \
      assert(! "marshall_p" # name ": marshall_heap_pointer");        \
      return NULL;                                                    \
    }                                                                 \
    if (! present && *data &&                                         \
        ! marshall_ ## name(m, true, *data)) {                        \
      err_puts("marshall_p" # name ": marshall_" # name);             \
      assert(! "marshall_p" # name ": marshall_" # name);             \
      return NULL;                                                    \
    }                                                                 \
    return m;                                                         \
  }

s_marshall * marshall_1 (s_marshall *m, bool heap,
                         const char *p)
{
  s_buf *buf = NULL;
  sw r;
  if (! m || ! p || ! p[0]) {
    err_puts("marshall_1: invalid argument");
    assert(! "marshall_1: invalid argument");
    return NULL;
  }
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write_1(buf, p)) <= 0) {
    err_puts("marshall_1: buf_write_1");
    assert(! "marshall_1: buf_write_1");
    return NULL;
  }
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

s_marshall * marshall_array (s_marshall *m, bool heap,
                             const s_array *array)
{
  u8 *data;
  uw i = 0;
  uw item_size;
  assert(m);
  assert(array);
  if (! marshall_psym(m, heap, &array->array_type) ||
      ! marshall_psym(m, heap, &array->element_type) ||
      ! marshall_uw(m, heap, array->dimension_count))
    return NULL;
  if (! array->dimension_count)
    return m;
  i = 0;
  while (i < array->dimension_count) {
    if (! marshall_uw(m, heap, array->dimensions[i].count) ||
        ! marshall_uw(m, heap, array->dimensions[i].item_size))
      return NULL;
    i++;
  }
  if (! marshall_uw(m, heap, array->count) ||
      ! marshall_uw(m, heap, array->size) ||
      ! marshall_bool(m, heap, array->data ? true : false))
    return NULL;
  if (array->data) {
    data = array->data;
    item_size = array->dimensions[array->dimension_count - 1].item_size;
    i = 0;
    while (i < array->count) {
      if (! marshall_data(m, heap, array->element_type, data))
        return NULL;
      data += item_size;
      i++;
    }
  }
  else {
    if (! marshall_bool(m, heap, array->tags ? true : false))
      return NULL;
    if (array->tags) {
      i = 0;
      while (i < array->count) {
        if (! marshall_tag(m, heap, array->tags + i))
          return NULL;
        i++;
      }
    }
  }
  return m;
}

DEF_MARSHALL(bool, "_KC3BOOL_")

s_marshall * marshall_call (s_marshall *m, bool heap,
                            const s_call *call)
{
  s_list *list;
  assert(m);
  assert(call);
  if (! m || ! call ||
      ! marshall_ident(m, heap, &call->ident))
    return NULL;
  list = call->arguments;
  if (! marshall_uw(m, heap, list_length(list)))
    return NULL;
  while (list) {
    if (! marshall_tag(m, heap, &list->tag))
      return NULL;
    list = list_next(list);
  }
  if (! marshall_pcallable(m, heap, &call->pcallable))
    return NULL;
  return m;
}

s_marshall * marshall_callable (s_marshall *m, bool heap,
                                const s_callable *callable)
{
  assert(m);
  assert(callable);
  if (! m || ! callable || callable->ref_count <= 0) {
    err_puts("marshall_callable: invalid argument");
    assert(! "marshall_callable: invalid argument");
    return NULL;
  }
  if (! marshall_u8(m, heap, callable->type)) {
    err_puts("marshall_callable: marshall_u8");
    return NULL;
  }
  switch (callable->type) {
  case CALLABLE_VOID:
    err_puts("marshall_callable: CALLABLE_VOID");
    assert(! "marshall_callable: CALLABLE_VOID");
    return NULL;
  case CALLABLE_CFN:
    return marshall_cfn(m, heap, &callable->data.cfn);
  case CALLABLE_FN:
    return marshall_fn(m, heap, &callable->data.fn);
  }
  err_write_1("marshall_callable: unknown callable type: ");
  err_inspect_u32_decimal(callable->type);
  err_write_1("\n");
  assert(! "marshall_callable: unknown callable type");
  return NULL;
}

s_marshall * marshall_cfn (s_marshall *m, bool heap, const s_cfn *cfn)
{
  const s_list *arg_type;
  assert(m);
  assert(cfn);
  if (! m || ! cfn)
    return NULL;
  if (cfn->arity != list_length(cfn->arg_types)) {
    err_puts("marshall_cfn: invalid arity");
    assert(! "marshall_cfn: invalid arity");
    return NULL;
  }
  if (! marshall_ident(m, heap, &cfn->name) ||
      ! marshall_bool(m, heap, cfn->macro) ||
      ! marshall_bool(m, heap, cfn->special_operator) ||
      ! marshall_psym(m, heap, &cfn->result_type) ||
      ! marshall_psym(m, heap, &cfn->c_name) ||
      ! marshall_u8(m, heap, cfn->arity))
    return NULL;
  arg_type = cfn->arg_types;
  while (arg_type) {
    if (arg_type->tag.type != TAG_PSYM) {
      err_puts("marshall_cfn: invalid arg type");
      assert(! "marshall_cfn: invalid arg type");
      return NULL;
    }
    if (! marshall_psym(m, heap, &arg_type->tag.data.psym))
      return NULL;
    arg_type = list_next(arg_type);
  }
  return m;
}

s_marshall * marshall_character (s_marshall *m, bool heap,
                                 character src)
{
  sw r;
  s_buf *buf;
  assert(m);
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write_character_utf8(buf, src)) <= 0)
    return NULL;
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

void marshall_clean (s_marshall *m)
{
  assert(m);
  buf_clean(&m->buf);
  buf_clean(&m->heap);
  ht_clean(&m->ht);
}

s_marshall * marshall_complex (s_marshall *m, bool heap,
                               const s_complex *c)
{
  assert(m);
  assert(c);
  if (! m || ! c ||
      ! marshall_tag(m, heap, &c->x) ||
      ! marshall_tag(m, heap, &c->y))
    return NULL;
  return m;
}

s_marshall * marshall_data (s_marshall *m, bool heap, p_sym type,
                            void *data)
{
  s_struct s = {0};
  p_struct_type st = NULL;
  assert(m);
  assert(type);
  assert(data);
  if (! m || ! type || ! data) {
    err_puts("marshall_data: invalid argument");
    assert(! "marshall_data: invalid argument");
    return NULL;
  }
  if (type == &g_sym_Array ||
      sym_is_array_type(type)) {
    if (! marshall_array(m, heap, data)) {
      err_puts("marshall_data: marshall_array");
      assert(! "marshall_data: marshall_array");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Bool) {
    if (! marshall_bool(m, heap, *(bool *)data)) {
      err_puts("marshall_data: marshall_bool");
      assert(! "marshall_data: marshall_bool");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Call) {
    if (! marshall_pcall(m, heap, data)) {
      err_puts("marshall_data: marshall_pcall");
      assert(! "marshall_data: marshall_pcall");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn) {
    if (! marshall_pcallable(m, heap, data)) {
      err_puts("marshall_data: marshall_pcallable");
      assert(! "marshall_data: marshall_pcallable");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Character) {
    if (! marshall_character(m, heap, *(character *)data)) {
      err_puts("marshall_data: marshall_character");
      assert(! "marshall_data: marshall_character");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Complex) {
    if (! marshall_complex(m, heap, data)) {
      err_puts("marshall_data: marshall_complex");
      assert(! "marshall_data: marshall_complex");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Cow) {
    if (! marshall_pcow(m, heap, data)) {
      err_puts("marshall_data: marshall_pcow");
      assert(! "marshall_data: marshall_pcow");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_F32) {
    if (! marshall_f32(m, heap, *(f32 *) data)) {
      err_puts("marshall_data: marshall_f32");
      assert(! "marshall_data: marshall_f32");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_F64) {
    if (! marshall_f64(m, heap, *(f64 *) data)) {
      err_puts("marshall_data: marshall_f64");
      assert(! "marshall_data: marshall_f64");
      return NULL;
    }
    return m;
  }
#if HAVE_F80
  if (type == &g_sym_F80) {
    if (! marshall_f80(m, heap, *(f80 *) data)) {
      err_puts("marshall_data: marshall_f80");
      assert(! "marshall_data: marshall_f80");
      return NULL;
    }
    return m;
  }
#endif
#if HAVE_F128
  if (type == &g_sym_F128) {
    if (! marshall_f128(m, heap, *(f128 *) data)) {
      err_puts("marshall_data: marshall_f128");
      assert(! "marshall_data: marshall_f128");
      return NULL;
    }
    return m;
  }
#endif
  if (type == &g_sym_Fact) {
    if (! marshall_fact(m, heap, data)) {
      err_puts("marshall_data: marshall_fact");
      assert(! "marshall_data: marshall_fact");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Ident) {
    if (! marshall_ident(m, heap, data)) {
      err_puts("marshall_data: marshall_ident");
      assert(! "marshall_data: marshall_ident");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Integer) {
    if (! marshall_integer(m, heap, data)) {
      err_puts("marshall_data: marshall_integer");
      assert(! "marshall_data: marshall_integer");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_List) {
    if (! marshall_plist(m, heap, data)) {
      err_puts("marshall_data: marshall_plist");
      assert(! "marshall_data: marshall_plist");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Map) {
    if (! marshall_map(m, heap, data)) {
      err_puts("marshall_data: marshall_map");
      assert(! "marshall_data: marshall_map");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Ptag) {
    if (! marshall_ptag(m, heap, data)) {
      err_puts("marshall_data: marshall_ptag");
      assert(! "marshall_data: marshall_ptag");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Ptr) {
    if (! marshall_ptr(m, heap, *(void **) data)) {
      err_puts("marshall_data: marshall_ptr");
      assert(! "marshall_data: marshall_ptr");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_PtrFree) {
    if (! marshall_ptr_free(m, heap, *(void **) data)) {
      err_puts("marshall_data: marshall_ptr_free");
      assert(! "marshall_data: marshall_ptr_free");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Quote) {
    if (! marshall_quote(m, heap, data)) {
      err_puts("marshall_data: marshall_quote");
      assert(! "marshall_data: marshall_quote");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_S8) {
    if (! marshall_s8(m, heap, *(s8 *) data)) {
      err_puts("marshall_data: marshall_s8");
      assert(! "marshall_data: marshall_s8");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_S16) {
    if (! marshall_s16(m, heap, *(s16 *) data)) {
      err_puts("marshall_data: marshall_s16");
      assert(! "marshall_data: marshall_s16");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_S32) {
    if (! marshall_s32(m, heap, *(s32 *) data)) {
      err_puts("marshall_data: marshall_s32");
      assert(! "marshall_data: marshall_s32");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_S64) {
    if (! marshall_s64(m, heap, *(s64 *) data)) {
      err_puts("marshall_data: marshall_s64");
      assert(! "marshall_data: marshall_s64");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Str) {
    if (! marshall_str(m, heap, data)) {
      err_puts("marshall_data: marshall_str");
      assert(! "marshall_data: marshall_str");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Struct) {
    if (! marshall_pstruct(m, heap, data)) {
      err_puts("marshall_data: marshall_pstruct");
      assert(! "marshall_data: marshall_pstruct");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_StructType) {
    if (! marshall_pstruct_type(m, heap, data)) {
      err_puts("marshall_data: marshall_pstruct_type");
      assert(! "marshall_data: marshall_pstruct_type");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Sw) {
    if (! marshall_sw(m, heap, *(sw *) data)) {
      err_puts("marshall_data: marshall_sw");
      assert(! "marshall_data: marshall_sw");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Sym) {
    if (! marshall_psym(m, heap, data)) {
      err_puts("marshall_data: marshall_psym");
      assert(! "marshall_data: marshall_psym");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Tag) {
    if (! marshall_tag(m, heap, data)) {
      err_puts("marshall_data: marshall_tag");
      assert(! "marshall_data: marshall_tag");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Time) {
    if (! marshall_time(m, heap, data)) {
      err_puts("marshall_data: marshall_time");
      assert(! "marshall_data: marshall_time");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Tuple) {
    if (! marshall_tuple(m, heap, data)) {
      err_puts("marshall_data: marshall_tuple");
      assert(! "marshall_data: marshall_tuple");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_U8) {
    if (! marshall_u8(m, heap,  *(u8 *) data)) {
      err_puts("marshall_data: marshall_u8");
      assert(! "marshall_data: marshall_u8");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_U16) {
    if (! marshall_u16(m, heap, *(u16 *) data)) {
      err_puts("marshall_data: marshall_u16");
      assert(! "marshall_data: marshall_u16");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_U32) {
    if (! marshall_u32(m, heap, *(u32 *) data)) {
      err_puts("marshall_data: marshall_u32");
      assert(! "marshall_data: marshall_u32");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_U64) {
    if (! marshall_u64(m, heap, *(u64 *) data)) {
      err_puts("marshall_data: marshall_u64");
      assert(! "marshall_data: marshall_u64");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Uw) {
    if (! marshall_uw(m, heap,  *(uw *) data)) {
      err_puts("marshall_data: marshall_uw");
      assert(! "marshall_data: marshall_uw");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Var) {
    if (! marshall_pvar(m, heap, data)) {
      err_puts("marshall_data: marshall_pvar");
      assert(! "marshall_data: marshall_pvar");
      return NULL;
    }
    return m;
  }
  if (type == &g_sym_Void)
    return m;
  if (! pstruct_type_find(type, &st)) {
    err_puts("marshall_data: pstruct_type_find");
    assert(! "marshall_data: pstruct_type_find");
    return NULL;
  }
  if (st) {
    s.pstruct_type = st;
    s.data = data;
    if (! marshall_struct(m, heap, &s)) {
      err_puts("marshall_data: marshall_struct");
      assert(! "marshall_data: marshall_struct");
      return NULL;
    }
  }
  return m;
}

void marshall_delete (s_marshall *m)
{
  marshall_clean(m);
  free(m);
}

s_marshall * marshall_do_block (s_marshall *m, bool heap,
                                const s_do_block *do_block)
{
  uw i;
  assert(m);
  assert(do_block);
  if (! m || ! do_block ||
      ! marshall_uw(m, heap, do_block->count) ||
      ! marshall_bool(m, heap, do_block->short_form))
    return NULL;
  i = 0;
  while (i < do_block->count) {
    if (! marshall_tag(m, heap, do_block->tag + i))
      return NULL;
    i++;
  }
  return m;
}

s_marshall * marshall_env (s_marshall *m, bool heap, const s_env *env)
{
  if (! m || ! env) {
    err_puts("marshall_env: invalid argument");
    assert(! "marshall_env: invalid argument");
    return NULL;
  }
  if (! marshall_1(m, heap, "_KC3ENV_")) {
    err_puts("marshall_env: marshall_1 magic");
    assert(! "marshall_env: marshall_1 magic");
    return NULL;
  }
  if (! marshall_plist(m, heap, &env->dlopen_list)) {
    err_puts("marshall_env: marshall_plist dlopen_plist");
    assert(! "marshall_env: marshall_plist dlopen_plist");
    return NULL;
  }
  if (! marshall_facts(m, heap, env->facts)) {
    err_puts("marshall_env: marshall_facts");
    assert(! "marshall_env: marshall_facts");
    return NULL;
  }
  if (! marshall_ops(m, heap, env->ops)) {
    err_puts("marshall_env: marshall_ops");
    assert(! "marshall_env: marshall_ops");
    return NULL;
  }
  if (! marshall_uw(m, heap, g_psym_anon_serial)) {
    err_puts("marshall_env: marshall_uw anonymous Sym serial");
    assert(! "marshall_env: marshall_uw anonymous Sym serial");
    return NULL;
  }
  if (! marshall_env_counters(m, heap, env)) {
    err_puts("marshall_env: marshall_env_counters");
    assert(! "marshall_env: marshall_env_counters");
    return NULL;
  }
  return m;
}

s_marshall * marshall_env_counters (s_marshall *m, bool heap,
                                    const s_env *env)
{
  s_counter *counter = NULL;
  s_ht_iterator ht_iter = {0};
  s_tag *item = NULL;
  if (! m || ! env || ! env->counter_ht || ! env->counter_ht->size) {
    err_puts("marshall_env_counters: invalid argument");
    assert(! "marshall_env_counters: invalid argument");
    return NULL;
  }
  if (! marshall_uw(m, heap, env->counter_ht->count)) {
    err_puts("marshall_env_counters: marshall_uw");
    assert(! "marshall_env_counters: marshall_uw");
    return NULL;
  }
  if (! env->counter_ht->count)
    return m;
  if (! ht_iterator_init(&ht_iter, env->counter_ht))
    return NULL;
  while (ht_iterator_next(&ht_iter, &item)) {
    if (item->type != TAG_PSTRUCT ||
        ! item->data.pstruct ||
        ! item->data.pstruct->pstruct_type ||
        item->data.pstruct->pstruct_type->module != &g_sym_Counter ||
        ! (counter = item->data.pstruct->data)) {
      err_puts("marshall_env_counters: invalid counter in hash table");
      assert(! "marshall_env_counters: invalid counter in hash table");
      return NULL;
    }
    if (! marshall_ident(m, heap, &counter->ident)) {
      err_puts("marshall_env_counters: marshall_ident");
      assert(! "marshall_env_counters: marshall_ident");
      return NULL;
    }
    if (! tag_is_integer(&counter->count)) {
      err_puts("marshall_env_counters: counter is not an integer");
      assert(! "marshall_env_counters: counter is not an integer");
      return NULL;
    }
    if (! marshall_tag(m, heap, &counter->count)) {
      err_puts("marshall_env_counters: marshall_tag");
      assert(! "marshall_env_counters: marshall_tag");
      return NULL;
    }
  }
  return m;
}
      
sw marshall_env_to_file (const s_env *env, const s_str *path)
{
  s_marshall m = {0};
  sw result = -1;
  if (! marshall_init(&m))
    return -1;
  if (! marshall_env(&m, false, env) ||
      (result = marshall_to_file(&m, path)) <= 0)
    goto ko;
  marshall_clean(&m);
  return result;
 ko:
  marshall_clean(&m);
  return -1;
}

DEF_MARSHALL(f32, "_KC3F32_")
DEF_MARSHALL(f64, "_KC3F64_")

#if HAVE_F80

s_marshall * marshall_f80 (s_marshall *m, bool heap, f80 src)
{
  s_buf *buf;
  const u8 zero[16] = {0};
  sw r;
  sw result = 0;
  if (! m) {
    err_puts("marshall_f80: invalid argument");
    assert(! "marshall_f80: invalid argument");
    return NULL;
  }
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write_1(buf, "_KC3F80_")) <= 0)
    return NULL;
  result += r;
  if ((r = buf_write_f80(buf, src)) <= 0)
    return NULL;
  result += r;
  if (sizeof(f80) < 16) {
    if ((r = buf_write(buf, zero, 16 - sizeof(f80))) <= 0)
      return NULL;
    result += r;
  }
  if (heap)
    m->heap_pos += result;
  else
    m->buf_pos += result;
  return m;
}

#endif

#if HAVE_F128
DEF_MARSHALL(f128, "_KC3F128_")
#endif

s_marshall * marshall_fact (s_marshall *m, bool heap,
                            const s_fact *fact)
{
  assert(m);
  if (! m ||
      ! marshall_1(m, heap, "_KC3FACT_") ||
      ! marshall_ptag(m, heap, &fact->subject) ||
      ! marshall_ptag(m, heap, &fact->predicate) ||
      ! marshall_ptag(m, heap, &fact->object) ||
      ! marshall_uw(m, heap, fact->id))
    return NULL;
  return m;
}

s_marshall * marshall_facts (s_marshall *m, bool heap, s_facts *facts)
{
  s_facts_cursor cursor;
  s_fact *fact;
  uw i;
  s_tag object;
  s_tag predicate;
  s_tag subject;
  assert(m);
  assert(facts);
#if HAVE_PTHREAD
  rwlock_r(&facts->rwlock);
#endif
  if (! marshall_1(m, heap, "_KC3FACTS_")) {
    err_puts("marshall_facts: marshall_1");
    assert(! "marshall_facts: marshall_1");
    return NULL;
  }
  if (! marshall_uw(m, heap, facts->facts.count)) {
    err_puts("marshall_facts: marshall_uw");
    assert(! "marshall_facts: marshall_uw");
    goto ko;
  }
  if (facts->facts.count) {
    tag_init_pvar(&subject,   &g_sym_Tag);
    tag_init_pvar(&predicate, &g_sym_Tag);
    tag_init_pvar(&object,    &g_sym_Tag);
    if (! facts_with_0_id(facts, &cursor, subject.data.pvar,
                          predicate.data.pvar, object.data.pvar)) {
      err_puts("marshall_facts: facts_with_0_id");
      assert(! "marshall_facts: facts_with_0_id");
      goto ko;
    }
    i = 0;
    while (i < facts->facts.count) {
      if (! facts_cursor_next(&cursor, &fact)) {
        err_puts("marshall_facts: facts_cursor_next");
        assert(! "marshall_facts: facts_cursor_next");
        goto ko;
      }
      if (! fact) {
        err_write_1("marshall_facts: invalid facts count (i = ");
        err_inspect_uw_decimal(i);
        err_write_1(", count = ");
        err_inspect_uw_decimal(facts->facts.count);
        err_puts(")");
        assert(! "marshall_facts: invalid facts count");
        goto ko;
      }
      if (false) {
        err_write_1("marshall_facts: #");
        err_inspect_uw_decimal(fact->id);
        err_write_1("\n");
      }
      if (! marshall_fact(m, heap, fact)) {
        err_puts("marshall_facts: marshall_fact");
        assert(! "marshall_facts: marshall_fact");
        goto ko;
      }
      i++;
    }
    if (! facts_cursor_next(&cursor, &fact)) {
      err_puts("marshall_facts: facts_cursor_next");
      assert(! "marshall_facts: facts_cursor_next");
      goto ko;
    }
    if (fact) {
      err_write_1("marshall_facts: invalid facts count (i = ");
      err_inspect_uw_decimal(i);
      err_write_1(", count = ");
      err_inspect_uw_decimal(facts->facts.count);
      err_puts(")");
      assert(! "marshall_facts: invalid facts count");
      goto ko;
    }
    tag_clean(&subject);
    tag_clean(&predicate);
    tag_clean(&object);
  }
  if (! marshall_bool(m, heap, facts->log ? true : false)) {
    err_puts("marshall_facts: marshall_bool log");
    assert(! "marshall_facts: marshall_bool log");
    return NULL;
  }
  if (facts->log) {
    if (! marshall_str(m, heap, &facts->log->path)) {
      err_puts("marshall_facts: marshall_str log path");
      assert(! "marshall_facts: marshall_str log path");
      return NULL;
    }
    if (! marshall_str(m, heap, &facts->log->binary_path)) {
      err_puts("marshall_facts: marshall_str log binary path");
      assert(! "marshall_facts: marshall_str log binary path");
      return NULL;
    }
  }
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return m;
 ko:
  facts_cursor_clean(&cursor);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return NULL;
}

s_marshall * marshall_fn (s_marshall *m, bool heap, const s_fn *fn)
{
  s_fn_clause *clause;
  uw           clause_count = 0;
  assert(m);
  assert(fn);
  if (! m || ! fn)
    return NULL;
  clause = fn->clauses;
  while (clause) {
    clause_count++;
    clause = clause->next;
  }
  if (! marshall_1(m, heap, "_KC3FN_") ||
      ! marshall_bool(m, heap, fn->macro) ||
      ! marshall_bool(m, heap, fn->special_operator) ||
      ! marshall_ident(m, heap, &fn->name) ||
      ! marshall_psym(m, heap, &fn->module) ||
      ! marshall_uw(m, heap, clause_count))
    return NULL;
  clause = fn->clauses;
  while (clause) {
    if (! marshall_plist(m, heap, &clause->pattern) ||
        ! marshall_do_block(m, heap, &clause->algo))
      return NULL;
    clause = clause->next;
  }
  if (! marshall_pframe(m, heap, &fn->frame))
    return NULL;
  return m;
}

s_marshall * marshall_frame (s_marshall *m, bool heap,
                             const s_frame *frame)
{
  s_binding *binding;
  uw         binding_count = 0;
  assert(m);
  assert(frame);
  if (! m || ! frame)
    return NULL;
  if (! marshall_1(m, heap, "_KC3FRAME_"))
    return NULL;
  binding = frame->bindings;
  while (binding) {
    binding_count++;
    binding = binding->next;
  }
  if (! marshall_uw(m, heap, binding_count))
    return NULL;
  binding = frame->bindings;
  while (binding) {
    if (! marshall_psym(m, heap, &binding->name) ||
        ! marshall_tag(m, heap, &binding->value))
      return NULL;
    binding = binding->next;
  }
  if (! marshall_pframe(m, heap, &frame->next))
    return NULL;
  return m;
}

s_marshall * marshall_heap_pointer (s_marshall *m, bool heap,
                                    const void *p, bool *present)
{
  s_tag key = {0};
  s_tag *ptag = NULL;
  s_tag tag = {0};
  u64 offset;
  assert(m);
  if (! p)
    return marshall_offset(m, heap, 0);
  tag_init_tuple(&key, 2);
  key.data.tuple.tag[0].type = TAG_U64;
  key.data.tuple.tag[0].data.u64 = (u64) p;
  if (ht_get(&m->ht, &key, &ptag)) {
    *present = true;
    goto ok;
  }
  *present = false;
  tag_init_tuple(&tag, 2);
  tag.data.tuple.tag[0].data.u64 = (u64) p;
  tag.data.tuple.tag[0].type = TAG_U64;
  tag.data.tuple.tag[1].data.u64 =
    sizeof(s_marshall_header) +
    m->heap_pos +
    (heap ? sizeof(u64) : 0);
  tag.data.tuple.tag[1].type = TAG_U64;
  if (! ht_add(&m->ht, &tag))
    goto ko;
  m->heap_count++;
  ptag = &tag;
 ok:
  if (ptag->type != TAG_TUPLE ||
      ptag->data.tuple.count != 2 ||
      ptag->data.tuple.tag[1].type != TAG_U64) {
    err_puts("marshall_heap_pointer: invalid offset in hash table");
    err_inspect_tag(&tag);
    err_write_1("\n");
    assert(! "marshall_heap_pointer: invalid offset in hash table");
    goto ko;
  }
  offset = ptag->data.tuple.tag[1].data.u64;
  if (! marshall_offset(m, heap, offset))
    goto ko;
  tag_clean(&key);
  tag_clean(&tag);
  return m;
 ko:
  tag_clean(&key);
  tag_clean(&tag);
  return NULL;
}

s_marshall * marshall_ident (s_marshall *m, bool heap,
                             const s_ident *ident)
{
  assert(m);
  assert(ident);
  if (! m || ! ident ||
      ! marshall_1(m, heap, "_KC3IDENT_") ||
      ! marshall_psym(m, heap, &ident->module) ||
      ! marshall_psym(m, heap, &ident->sym))
    return NULL;
  return m;
}

s_marshall * marshall_init (s_marshall *m)
{
  s_marshall tmp = {0};
  if (! ht_init(&tmp.ht, &g_sym_Tag, 1024) ||
    ! buf_init_alloc(&tmp.heap, 128 * 1024 * 1024))
    return NULL;
  if (! buf_init_alloc(&tmp.buf, 128 * 1024 * 1024)) {
    buf_delete(&tmp.heap);
    return NULL;
  }
  *m = tmp;
  return m;
}

s_marshall * marshall_integer (s_marshall *m, bool heap,
                               const s_integer *i)
{
  s_buf *buf;
  sw r;
  assert(m);
  assert(i);
  buf = heap ? &m->heap : &m->buf;
  if (! marshall_1(m, heap, "_KC3INTEGER_"))
    return NULL;
  if ((r = buf_write_integer(buf, i)) <= 0)
    return NULL;
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

s_marshall * marshall_list (s_marshall *m, bool heap,
                            const s_list *list)
{
  assert(m);
  assert(list);
  if (! marshall_1(m, heap, "_KC3LIST_") ||
      ! marshall_tag(m, heap, &list->tag) ||
      ! marshall_tag(m, heap, &list->next))
    return NULL;
  return m;
}

s_marshall * marshall_map (s_marshall *m, bool heap, const s_map *map)
{
  uw i = 0;
  assert(m);
  assert(map);
  if (! marshall_1(m, heap, "_KC3MAP_") ||
      ! marshall_uw(m, heap, map->count))
    return NULL;
  while (i < map->count) {
    if (! marshall_tag(m, heap, map->key + i) ||
        ! marshall_tag(m, heap, map->value + i))
      return NULL;
    i++;
  }
  return m;
}

s_marshall * marshall_new (void)
{
  s_marshall *m;
  if (! (m = alloc(sizeof(s_marshall))))
    return NULL;
  if (! marshall_init(m)) {
    free(m);
    return NULL;
  }
  return m;
}

s_marshall * marshall_offset (s_marshall *m, bool heap, u64 src)
{
  s_buf *buf;
  u64 le;
  sw r;
  if (! m) {
    err_puts("marshall_offset: invalid argument");
    assert(! "marshall_offset: invalid argument");
    return NULL;
  }
  le = htole64(src);
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write_u64(buf, le)) <= 0)
    return NULL;
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

s_marshall * marshall_op (s_marshall *m, bool heap,
                          const s_op *op)
{
  assert(m);
  assert(op);
  if (! marshall_1(m, heap, "_KC3OP_") ||
      ! marshall_psym(m, heap, &op->sym) ||
      ! marshall_u8(m, heap, op->arity) ||
      ! marshall_bool(m, heap, op->special) ||
      ! marshall_u8(m, heap, op->precedence) ||
      ! marshall_u8(m, heap, op->associativity) ||
      ! marshall_pcallable(m, heap, &op->pcallable)) {
    err_puts("marshall_op: marshall error");
    assert(! "marshall_op: marshall error");
    return NULL;
  }
  return m;
}

s_marshall * marshall_ops (s_marshall *m, bool heap, s_ops *ops)
{
  s_ht_iterator ht_i = {0};
  uw i;
  s_op  *op;
  s_tag *op_tag;
  if (! m || ! ops) {
    err_puts("marshall_ops: invalid argument");
    assert(! "marshall_ops: invalid argument");
    return NULL;
  }
  if (! marshall_1(m, heap, "_KC3OPS_")) {
    err_puts("marshall_ops: marshall_1 magic");
    assert(! "marshall_ops: marshall_1 magic");
    return NULL;
  }
#if HAVE_PTHREAD
  rwlock_r(&ops->ht.rwlock);
#endif
  if (! marshall_uw(m, heap, ops->ht.count))
    goto ko;
  if (ops->ht.count) {
    if (! ht_iterator_init(&ht_i, &ops->ht)) {
      err_puts("marshall_ops: ht_iterator_init");
      assert(! "marshall_ops: ht_iterator_init");
      goto ko;
    }
    i = 0;
    while (i < ops->ht.count) {
      if (! ht_iterator_next(&ht_i, &op_tag)) {
        err_puts("marshall_ops: ht_iterator_next");
        assert(! "marshall_ops: ht_iterator_next");
        goto ko;
      }
      if (! op_tag) {
        err_puts("marshall_ops: invalid ht count");
        assert(! "marshall_ops: invalid ht count");
        goto ko;
      }
      if (op_tag->type != TAG_PSTRUCT ||
          ! op_tag->data.pstruct ||
          op_tag->data.pstruct->pstruct_type->module != &g_sym_KC3_Op ||
          ! (op = op_tag->data.pstruct->data)) {
        err_puts("marshall_ops: invalid op");
        assert(! "marshall_ops: invalid op");
        goto ko;
      }
      if (! marshall_op(m, heap, op)) {
        err_puts("marshall_ops: marshall_op");
        assert(! "marshall_ops: marshall_op");
        goto ko;
      }
      i++;
    }
  }
#if HAVE_PTHREAD
  rwlock_unlock_r(&ops->ht.rwlock);
#endif
  return m;
  ko:
#if HAVE_PTHREAD
  rwlock_unlock_r(&ops->ht.rwlock);
#endif
  return NULL;
}

DEF_MARSHALL_P(call,        "_KC3PCALL_",       p_call)
DEF_MARSHALL_P(callable,    "_KC3PCALLABLE_",   p_callable)
DEF_MARSHALL_P(complex,     "_KC3PCOMPLEX_",    p_complex)
DEF_MARSHALL_P(cow,         "_KC3PCOW_",        p_cow)
DEF_MARSHALL_P(facts,       "_KC3PFACTS_",      p_facts)
DEF_MARSHALL_P(frame,       "_KC3PFRAME_",      p_frame)
DEF_MARSHALL_P(list,        "_KC3PLIST_",       p_list)

s_marshall * marshall_pointer (s_marshall *m, bool heap,
                               const s_pointer *pointer)
{
  s_call call = {0};
  s_env *env = NULL;
  s_ident ident = {0};
  void *p;
  bool present;
  s_tag tag = {0};
  s_tag tmp = {0};
  assert(m);
  assert(pointer);
  if (! m || ! pointer)
    return NULL;
  if (! marshall_1(m, heap, "_KC3POINTER_")) {
    err_puts("marshall_pointer : marshall_1 magic");
    assert(! "marshall_pointer : marshall_1 magic");
    return NULL;
  }
  if (! marshall_psym(m, heap, &pointer->target_type))
    return NULL;
  p = pointer->ptr.p;
  if (! marshall_heap_pointer(m, heap, p, &present))
    return NULL;
  if (p && ! present) {
    env = env_global();
    ident.module = pointer->target_type;
    ident.sym = &g_sym_marshall;
    if (! env_ident_get(env, &ident, &tag) ||
        tag.type != TAG_PCALLABLE) {
      err_write_1("marshall_pointer: Callable not found: ");
      err_inspect_ident(&ident);
      err_write_1("\n");
      return NULL;
    }
    call_init(&call);
    call.ident = ident;
    call.arguments = list_new_pointer
      (NULL, &g_sym_Marshall, m, list_new_bool
       (true, list_new_pointer
        (pointer->pointer_type, pointer->target_type, pointer->ptr.p,
         NULL)));
    if (! env_eval_call(env, &call, &tmp)) {
      err_puts("marshall_pointer: env_eval_call");
      tag_clean(&tag);
      call_clean(&call);
      return NULL;
    }
    tag_clean(&tmp);
    tag_clean(&tag);
    call_clean(&call);
  }
  return m;
}

DEF_MARSHALL_P(struct,      "_KC3PSTRUCT_",     p_struct)
DEF_MARSHALL_P(struct_type, "_KC3PSTRUCTTYPE_", p_struct_type)
DEF_MARSHALL_P(sym,         "_KC3PSYM_",        p_sym)
DEF_MARSHALL_P(tag,         "_KC3PTAG_",        p_tag)

s_marshall * marshall_ptr (s_marshall *m, bool heap, void *p)
{
  assert(m);
  if (! marshall_1(m, heap, "_KC3PTR_"))
    return NULL;
  if (! p)
    return marshall_uw(m, heap, 0);
  err_puts("marshall_ptr: non-null pointer.");
  assert(! "marshall_ptr: non-null pointer.");
  return NULL;
}

s_marshall * marshall_ptr_free (s_marshall *m, bool heap,
                                void *p)
{
  assert(m);
  if (! marshall_1(m, heap, "_KC3PTRFREE_"))
    return NULL;
  if (! p)
    return marshall_uw(m, heap, 0);
  err_puts("marshall_ptr_free: non-null pointer.");
  assert(! "marshall_ptr_free: non-null pointer.");
  return NULL;
}

DEF_MARSHALL_P(var, "_KC3PVAR_", p_var)

s_marshall * marshall_quote (s_marshall *m, bool heap,
                             const s_quote *quote)
{
  assert(m);
  assert(quote);
  assert(quote->tag);
  if (! m || ! quote || ! quote->tag) {
    err_puts("marshall_quote: invalid argument");
    assert(! "marshall_quote: invalid argument");
    return NULL;
  }
  if (! marshall_1(m, heap, "_KC3QUOTE_") ||
      ! marshall_tag(m, heap, quote->tag))
    return NULL;
  return m;
}

s_marshall * marshall_ratio (s_marshall *m, bool heap,
                             const s_ratio *ratio)
{
  assert(m);
  assert(ratio);
  if (! marshall_1(m, heap, "_KC3RATIO_") ||
      ! marshall_integer(m, heap, &ratio->numerator) ||
      ! marshall_integer(m, heap, &ratio->denominator))
    return NULL;
  return m;
}

DEF_MARSHALL(s8, "_KC3S8_")
DEF_MARSHALL(s16, "_KC3S16_")
DEF_MARSHALL(s32, "_KC3S32_")
DEF_MARSHALL(s64, "_KC3S64_")

sw marshall_size (const s_marshall *m)
{
  if (! m)
    return -1;
  return sizeof(s_marshall_header) + m->heap_pos + m->buf_pos;
}

s_marshall * marshall_str (s_marshall *m, bool heap, const s_str *src)
{
  sw r;
  s_buf *buf;
  if (! m || ! src) {
    err_puts("marshall_str: invalid argument");
    assert(! "marshall_str: invalid argument");
    return NULL;
  }
  if (! marshall_1(m, heap, "_KC3STR_")) {
    err_puts("marshall_str: marshall_1 magic");
    assert(! "marshall_str: marshall_1 magic");
    return NULL;
  }
  if (! marshall_u32(m, heap, src->size)) {
    err_puts("marshall_str: marshall_u32");
    assert(! "marshall_str: marshall_u32");
    return NULL;
  }
  if (! src->size)
    return m;
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write(buf, src->ptr.pchar, src->size)) <= 0) {
    err_puts("marshall_str: buf_write");
    assert(! "marshall_str: buf_write");
    return NULL;
  }
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

s_marshall * marshall_struct (s_marshall *m, bool heap,
                              const s_struct *s)
{
  uw i;
  uw offset;
  p_sym type = NULL;
  if (! m || ! s) {
    err_puts("marshall_struct: invalid argument");
    assert(! "marshall_struct: invalid argument");
    return NULL;
  }
  if (! marshall_1(m, heap, "_KC3STRUCT_")) {
    err_puts("marshall_struct: marshall_1 magic");
    assert(! "marshall_struct: marshall_1 magic");
    return NULL;
  }
  if (! marshall_pstruct_type(m, heap, &s->pstruct_type)) {
    err_puts("marshall_struct: marshall_pstruct_type");
    assert(! "marshall_struct: marshall_pstruct_type");
    return NULL;
  }
  if (! marshall_bool(m, heap, s->tag ? true : false)) {
    err_puts("marshall_struct: marshall_bool has_tag");
    assert(! "marshall_struct: marshall_bool has_tag");
    return NULL;
  }
  if (s->tag) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      if (! marshall_tag(m, heap, s->tag + i)) {
        err_puts("marshall_struct: marshall_tag");
        assert(! "marshall_struct: marshall_tag");
        return NULL;
      }
      i++;
    }
  }
  if (! marshall_bool(m, heap, s->data ? true : false)) {
    err_puts("marshall_struct: marshall_bool has_data");
    assert(! "marshall_struct: marshall_bool has_data");
    return NULL;
  }
  if (s->data) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      offset = s->pstruct_type->offset[i];
      if (! tag_type_var(s->pstruct_type->map.value + i, &type) ||
          ! type) {
        err_puts("marshall_struct: tag_type_var");
        assert(! "marshall_struct: tag_type_var");
        return NULL;
      }
      if (! marshall_data(m, heap, type, (u8 *) s->data + offset)) {
        err_puts("marshall_struct: marshall_data");
        assert(! "marshall_struct: marshall_data");
        return NULL;
      }
      i++;
    }
  }
  return m;
}

s_marshall * marshall_struct_type (s_marshall *m, bool heap,
                                   const s_struct_type *st)
{
  assert(m);
  assert(st);
  assert(st->module);
  if (! m || ! st || ! st->module)
    return NULL;
  if (! marshall_1(m, heap, "_KC3STRUCTTYPE_")) {
    err_puts("marshall_struct_type: marshall_1 magic");
    assert(! "marshall_struct_type: marshall_1 magic");
    return NULL;
  }
  if (! marshall_psym(m, heap, &st->module) ||
      ! marshall_map(m, heap, &st->map) ||
      ! marshall_pcallable(m, heap, &st->clean)) {
    err_write_1("marshall_struct_type: inner fields: ");
    err_inspect_sym(st->module);
    err_write_1("\n");
    assert(! "marshall_struct_type: inner fields");
    return NULL;
  }
  return m;
}

s_marshall * marshall_sym (s_marshall *m, bool heap,
                           const s_sym *sym)
{
  if (! m || ! sym) {
    err_puts("marshall_sym: invalid argument");
    assert(! "marshall_sym: invalid argument");
    return NULL;
  }
  if (! marshall_1(m, heap, "_KC3SYM_")) {
    err_puts("marshall_sym: marshall_1 magic");
    assert(! "marshall_sym: marshall_1 magic");
    return NULL;
  }
  if (! marshall_str(m, heap, &sym->str)) {
    err_puts("marshall_sym: marshall_str");
    assert(! "marshall_sym: marshall_str");
    return NULL;
  }
  return m;
}

s_marshall * marshall_sw (s_marshall *m, bool heap, sw src)
{
  if (! marshall_1(m, heap, "_KC3SW_"))
    return NULL;
  if (! marshall_s64(m, heap, src))
    return NULL;
  return m;
}

s_marshall * marshall_tag (s_marshall *m, bool heap, const s_tag *tag)
{
  u8 type;
  assert(m);
  assert(tag);
  type = tag->type;
  if (! marshall_1(m, heap, "_KC3TAG_")) {
    err_puts("marshall_tag: marshall_1 magic");
    assert(! "marshall_tag: marshall_1 magic");
    return NULL;
  }
  if (! marshall_u8(m, heap, type)) {
    err_puts("marshall_tag: marshall_u8");
    assert(! "marshall_tag: marshall_u8");
    return NULL;
  }
  switch (tag->type) {
  case TAG_VOID: return m;
  case TAG_ARRAY: return marshall_array(m, heap, &tag->data.array);
  case TAG_BOOL:  return marshall_bool(m, heap, tag->data.bool_);
  case TAG_CHARACTER:
    return marshall_character(m, heap, tag->data.character);
  case TAG_DO_BLOCK:
    return marshall_do_block(m, heap, &tag->data.do_block);
  case TAG_F32:   return marshall_f32(m, heap, tag->data.f32);
  case TAG_F64:   return marshall_f64(m, heap, tag->data.f64);
#if HAVE_F80
  case TAG_F80:   return marshall_f80(m, heap, tag->data.f80);
#endif
#if HAVE_F128
  case TAG_F128:  return marshall_f128(m, heap, tag->data.f128);
#endif
  case TAG_FACT:  return marshall_fact(m, heap, &tag->data.fact);
  case TAG_IDENT: return marshall_ident(m, heap, &tag->data.ident);
  case TAG_INTEGER:
    return marshall_integer(m, heap, &tag->data.integer);
  case TAG_MAP:   return marshall_map(m, heap, &tag->data.map);
  case TAG_PCALL: return marshall_pcall(m, heap, &tag->data.pcall);
  case TAG_PCALLABLE:
    return marshall_pcallable(m, heap, &tag->data.pcallable);
  case TAG_PCOMPLEX:
    return marshall_pcomplex(m, heap, &tag->data.pcomplex);
  case TAG_PCOW:  return marshall_pcow(m, heap, &tag->data.pcow);
  case TAG_PFACTS:
    return marshall_pfacts(m, heap, &tag->data.pfacts);
  case TAG_PLIST: return marshall_plist(m, heap, &tag->data.plist);
  case TAG_POINTER:
    return marshall_pointer(m, heap, &tag->data.pointer);
  case TAG_PSTRUCT:
    return marshall_pstruct(m, heap, &tag->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return marshall_pstruct_type(m, heap, &tag->data.pstruct_type);
  case TAG_PSYM:  return marshall_psym(m, heap, &tag->data.psym);
  case TAG_PTAG:  return marshall_ptag(m, heap, &tag->data.ptag);
  case TAG_PTR:   return marshall_ptr(m, heap, tag->data.ptr.p);
  case TAG_PTR_FREE:
    return marshall_ptr_free(m, heap, tag->data.ptr_free.p);
  case TAG_PVAR:  return marshall_pvar(m, heap, &tag->data.pvar);
  case TAG_QUOTE: return marshall_quote(m, heap, &tag->data.quote);
  case TAG_RATIO: return marshall_ratio(m, heap, &tag->data.ratio);
  case TAG_S8:    return marshall_s8(m, heap, tag->data.s8);
  case TAG_S16:   return marshall_s16(m, heap, tag->data.s16);
  case TAG_S32:   return marshall_s32(m, heap, tag->data.s32);
  case TAG_S64:   return marshall_s64(m, heap, tag->data.s64);
  case TAG_STR:   return marshall_str(m, heap, &tag->data.str);
  case TAG_SW:    return marshall_sw(m, heap, tag->data.sw);
  case TAG_TIME:  return marshall_time(m, heap, &tag->data.time);
  case TAG_TUPLE: return marshall_tuple(m, heap, &tag->data.tuple);
  case TAG_U8:    return marshall_u8(m, heap, tag->data.u8);
  case TAG_U16:   return marshall_u16(m, heap, tag->data.u16);
  case TAG_U32:   return marshall_u32(m, heap, tag->data.u32);
  case TAG_U64:   return marshall_u64(m, heap, tag->data.u64);
  case TAG_UNQUOTE:
    return marshall_unquote(m, heap, &tag->data.unquote);
  case TAG_UW:    return marshall_uw(m, heap, tag->data.uw);
  }
  err_write_1("marshall_tag: unknown tag type : ");
  err_inspect_u8_decimal(type);
  err_write_1("\n");
  assert(! "marshall_tag: unknown tag type");
  return NULL;
}

s_marshall *marshall_time(s_marshall *m, bool heap, const s_time *time)
{
  assert(m);
  assert(time);
  if (! m || ! time ||
      ! marshall_bool(m, heap, time->tag ? true : false))
    return NULL;
  if (time->tag) {
    if (! marshall_tag(m, heap, time->tag) ||
        ! marshall_tag(m, heap, time->tag + 1))
      return NULL;
  }
  else if (! marshall_sw(m, heap, time->tv_sec) ||
           ! marshall_sw(m, heap, time->tv_nsec))
    return NULL;
  return m;
}

sw marshall_to_buf (s_marshall *m, s_buf *out)
{
  s_marshall_header mh = {0};
  sw r;
  sw result = 0;
  assert(m);
  assert(out);
  mh.le_magic = htole64(MARSHALL_MAGIC);
  mh.le_heap_count = htole64(m->heap_count);
  mh.le_heap_size = htole64(m->heap_pos);
  mh.le_buf_size = htole64(m->buf_pos);
  if ((r = buf_write(out, &mh, sizeof(mh))) != sizeof(mh)) {
    err_puts("marshall_to_buf: buf_write marshall header");
    assert(! "marshall_to_buf: buf_write marshall header");
    return -1;
  }
  result += r;
  if ((r = buf_xfer(out, &m->heap, m->heap_pos)) != m->heap_pos) {
    err_puts("marshall_to_buf: buf_xfer heap");
    assert(! "marshall_to_buf: buf_xfer heap");
    return -1;
  }
  result += r;
  if ((r = buf_xfer(out, &m->buf, m->buf_pos)) != m->buf_pos) {
    err_puts("marshall_to_buf: buf_xfer buf");
    assert(! "marshall_to_buf: buf_xfer buf");
    return -1;
  }
  result += r;
  return result;
}

sw marshall_to_file (s_marshall *m, const s_str *path)
{
  FILE *fp;
  s_buf out;
  sw r;
  sw result = 0;
  assert(m);
  assert(path);
  if (! buf_init_alloc(&out, 257 * 1024 * 1024))
    return -1;
  if (! (fp = file_open(path, "wb")) ||
      ! buf_file_open_w(&out, fp) ||
      (r = marshall_to_buf(m, &out)) <= 0) {
    buf_clean(&out);
    return -1;
  }
  result = r;
  buf_file_close(&out);
  buf_clean(&out);
  return result;
}

s_str * marshall_to_str (s_marshall *m, s_str *dest)
{
  s_buf out;
  sw r;
  assert(m);
  assert(dest);
  if (! buf_init_alloc(&out, 1024 * 1024))
    return NULL;
  if ((r = marshall_to_buf(m, &out)) <= 0) {
    buf_clean(&out);
    return NULL;
  }
  if (buf_read_to_str(&out, dest) <= 0) {
    buf_clean(&out);
    return NULL;
  }
  buf_clean(&out);
  return dest;
}

s_marshall * marshall_tuple (s_marshall *m, bool heap,
                             const s_tuple *tuple)
{
  uw i = 0;
  assert(m);
  assert(tuple);
  if (! m || ! tuple || ! tuple->count
      || ! marshall_uw(m, heap, tuple->count))
    return NULL;
  while (i < tuple->count) {
    if (! marshall_tag(m, heap, tuple->tag + i))
      return NULL;
    i++;
  }
  return m;
}

DEF_MARSHALL(u8, "_KC3U8_")
DEF_MARSHALL(u16, "_KC3U16_")
DEF_MARSHALL(u32, "_KC3U32_")
DEF_MARSHALL(u64, "_KC3U64_")

s_marshall * marshall_uw (s_marshall *m, bool heap, uw src)
{
  if (! marshall_1(m, heap, "_KC3UW_"))
    return NULL;
  if (! marshall_u64(m, heap, htole64(src)))
    return NULL;
  return m;
}

s_marshall *marshall_unquote(s_marshall *m, bool heap,
                             const s_unquote *unquote)
{
  assert(m);
  assert(unquote);
  assert(unquote->tag);
  if (! unquote->tag || ! marshall_tag(m, heap, unquote->tag))
    return NULL;
  return m;
}

s_marshall * marshall_var (s_marshall *m, bool heap, const s_var *var)
{
  assert(m);
  assert(var);
  if (! m || ! var ||
      ! marshall_1(m, heap, "_KC3VAR_") ||
      ! marshall_ident(m, heap, &var->name) ||
      ! marshall_psym(m, heap, &var->type) ||
      ! marshall_bool(m, heap, var->bound) ||
      (var->bound &&
       ! marshall_tag(m, heap, &var->tag)))
    return NULL;
  return m;
}

s_marshall * marshall_void (s_marshall *m, bool heap)
{
  (void) heap;
  return m;
}

#define DEF_MARSHALL_STUB(name, type)                                  \
  PROTO_MARSHALL(name, type)                                           \
  {                                                                    \
    (void) m;                                                          \
    (void) heap;                                                       \
    (void) src;                                                        \
    err_puts("marshall_" # name ": not implemented");                  \
    return NULL;                                                       \
  }

DEF_MARSHALL_STUB(cow, const s_cow *)
