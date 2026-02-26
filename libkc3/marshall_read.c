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
#include <dlfcn.h>

#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "buf.h"
#include "buf_file.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "complex.h"
#include "do_block.h"
#include "endian.h"
#include "env_eval.h"
#include "fact.h"
#include "facts.h"
#include "file.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ht.h"
#include "integer.h"
#include "kc3_main.h"
#include "list.h"
#include "log.h"
#include "map.h"
#include "marshall.h"
#include "marshall_read.h"
#include "mutex.h"
#include "op.h"
#include "ops.h"
#include "pcall.h"
#include "pcallable.h"
#include "pcomplex.h"
#include "pfacts.h"
#include "pframe.h"
#include "plist.h"
#include "pointer.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "psym.h"
#include "pvar.h"
#include "rwlock.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "time.h"
#include "tuple.h"
#include "types.h"
#include "var.h"

#define DEF_MARSHALL_READ(name, magic, type)                           \
  s_marshall_read * marshall_read_ ## name (s_marshall_read *mr,       \
                                            bool heap,                 \
                                            type *dest)                \
  {                                                                    \
    s_buf *buf = NULL;                                                 \
    assert(mr);                                                        \
    assert(dest);                                                      \
    buf = heap ? mr->heap : mr->buf;                                   \
    if (buf_read_1(buf, (magic)) <= 0) {                               \
      err_puts("marshall_read_"#name": buf_read_1 magic");             \
      assert(! "marshall_read_"#name": buf_read_1 magic");             \
      return NULL;                                                     \
    }                                                                  \
    if (buf_read_ ## name (buf, dest) <= 0) {                          \
      err_puts("marshall_read_" # name ": buf_read_" # name);          \
      assert(! "marshall_read_" # name ": buf_read_" # name);          \
      return NULL;                                                     \
    }                                                                  \
    return mr;                                                         \
  }

#define DEF_MARSHALL_READ_LETOH(name, magic, type, bits)               \
  s_marshall_read * marshall_read_ ## name (s_marshall_read *mr,       \
                                            bool heap,                 \
                                            type *dest)                \
  {                                                                    \
    s_buf *buf = NULL;                                                 \
    assert(mr);                                                        \
    assert(dest);                                                      \
    buf = heap ? mr->heap : mr->buf;                                   \
    if (buf_read_1(buf, (magic)) <= 0) {                               \
      err_puts("marshall_read_"#name": buf_read_1 magic");             \
      assert(! "marshall_read_"#name": buf_read_1 magic");             \
      return NULL;                                                     \
    }                                                                  \
    if (buf_read_ ## name (buf, dest) <= 0) {                          \
      err_puts("marshall_read_" # name ": buf_read_" # name);          \
      assert(! "marshall_read_" # name ": buf_read_" # name);          \
      return NULL;                                                     \
    }                                                                  \
    *dest = le ## bits ## toh(*dest);                                  \
    return mr;                                                         \
  }

static void marshall_read_ht_clean (s_marshall_read *mr);
static s8 marshall_read_ht_compare (const s_tag *a, const s_tag *b);
static uw marshall_read_ht_hash (const s_tag *tag);

s_marshall_read * marshall_read_1 (s_marshall_read *mr, bool heap,
                                   const char *p)
{
  s_buf *buf = NULL;
  if (! mr || ! p || ! p[0]) {
    err_puts("marshall_read_1: invalid argument");
    assert(! "marshall_read_1: invalid argument");
    return NULL;
  }
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, p) <= 0) {
    err_puts("marshall_read_1: buf_read_1");
    err_write_1("Expected: ");
    err_puts(p);
    err_puts("Buffer contents:");
    err_inspect_buf(buf);
    assert(! "marshall_read_1: buf_read_1");
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_array (s_marshall_read *mr,
                                       bool heap,
                                       s_array *dest)
{
  u8 *data = NULL;
  bool has_data;
  bool has_tags;
  uw i = 0;
  uw item_size = 0;
  s_array tmp = {0};
  if (! mr || ! dest)
    return NULL;
  if (! marshall_read_psym(mr, heap, &tmp.array_type) ||
      ! marshall_read_psym(mr, heap, &tmp.element_type) ||
      ! marshall_read_uw(mr, heap, &tmp.dimension_count))
    return NULL;
  if (! tmp.dimension_count) {
    *dest = tmp;
    return mr;
  }
  tmp.dimensions = alloc(sizeof(s_array_dimension) *
                         tmp.dimension_count);
  i = 0;
  while (i < tmp.dimension_count) {
    if (! marshall_read_uw(mr, heap, &tmp.dimensions[i].count) ||
        ! marshall_read_uw(mr, heap, &tmp.dimensions[i].item_size))
      return NULL;
    i++;
  }
  if (! marshall_read_uw(mr, heap, &tmp.count) ||
      ! marshall_read_uw(mr, heap, &tmp.size) ||
      ! marshall_read_bool(mr, heap, &has_data))
    return NULL;
  if (has_data) {
    array_allocate(&tmp);
    data = tmp.data;
    item_size = tmp.dimensions[tmp.dimension_count - 1].item_size;
    i = 0;
    while (i < tmp.count) {
      if (! marshall_read_data(mr, heap, tmp.element_type, data))
        return NULL;
      data = (u8 *) data + item_size;
      i++;
    }
  }
  else {
    if (! marshall_read_bool(mr, heap, &has_tags))
      return NULL;
    if (has_tags) {
      tmp.tags = alloc(sizeof(s_tag) * tmp.count);
      i = 0;
      while (i < tmp.count) {
        if (! marshall_read_tag(mr, heap, tmp.tags + i))
          return NULL;
        i++;
      }
    }
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_array_data (s_marshall_read *mr,
                                            bool heap,
                                            s_array *dest)
{
  (void) mr;
  (void) heap;
  (void) dest;
  return NULL;
  /*
  for (u8 i = 0; i <= dest->count; i++) {
    if (marshall_read_uw(mr, heap, &((uw *) dest->data)[i]))
      return NULL;
  }
  return mr;
  */
}

DEF_MARSHALL_READ(bool, "_KC3BOOL_", bool)

s_marshall_read * marshall_read_call (s_marshall_read *mr,
                                      bool heap,
                                      s_call *dest)
{
  uw i;
  uw len;
  s_list **tail;
  s_call tmp = {0};
  assert(mr);
  assert(dest);
  call_init(&tmp);
  if (! marshall_read_ident(mr, heap, &tmp.ident) ||
      ! marshall_read_uw(mr, heap, &len))
    goto ko;
  tail = &tmp.arguments;
  i = 0;
  while (i < len) {
    if (! (*tail = list_new(NULL)) ||
        ! marshall_read_tag(mr, heap, &(*tail)->tag))
      goto ko;
    tail = &(*tail)->next.data.plist;
    i++;
  }
  if (! marshall_read_pcallable(mr, heap, &tmp.pcallable))
    goto ko;
  *dest = tmp;
  return mr;
 ko:
  call_clean(&tmp);
  return NULL;
}

s_marshall_read * marshall_read_callable (s_marshall_read *mr,
                                          bool heap,
                                          s_callable *dest)
{
  assert(mr);
  assert(dest);
  s_callable tmp = {0};
  u8 type;
  if (! marshall_read_u8(mr, heap, &type)) {
    err_puts("marshall_read_callable: marshall_read_u8");
    assert(! "marshall_read_callable: marshall_read_u8");
    return NULL;
  }
  tmp.type = type;
  switch (tmp.type) {
  case CALLABLE_VOID:
    err_puts("marshall_read: CALLABLE_VOID");
    assert(! "marshall_read: CALLABLE_VOID");
    return NULL;
  case CALLABLE_CFN:
    if (! marshall_read_cfn(mr, heap, &tmp.data.cfn)) {
      err_puts("marshall_read_callable: marshall_read_cfn");
      assert(! "marshall_read_callable: marshall_read_cfn");
      return NULL;
    }
    goto ok;
  case CALLABLE_FN:
    if (! marshall_read_fn(mr, heap, &tmp.data.fn)) {
      err_puts("marshall_read_callable: marshall_read_fn");
      assert(! "marshall_read_callable: marshall_read_fn");
      return NULL;
    }
    goto ok;
  }
  err_puts("marshall_read_callable: unknown callable type");
  assert(! "marshall_read_callable: unknown callable type");
  return NULL;
 ok:
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  mutex_init(&dest->mutex);
#endif
  return mr;
}

s_marshall_read * marshall_read_cfn (s_marshall_read *mr,
                                     bool heap,
                                     s_cfn *dest)
{
  assert(mr);
  assert(dest);
  u8 i;
  p_sym psym;
  p_list *tail;
  s_cfn tmp = {0};
  if (! marshall_read_ident(mr, heap, &tmp.name) ||
      ! marshall_read_bool(mr, heap, &tmp.macro) ||
      ! marshall_read_bool(mr, heap, &tmp.special_operator) ||
      ! marshall_read_psym(mr, heap, &tmp.result_type) ||
      ! marshall_read_psym(mr, heap, &tmp.c_name) ||
      ! marshall_read_u8(mr, heap, &tmp.arity)) {
    err_puts("marshall_read_cfn: read_error");
    assert(! "marshall_read_cfn: read_error");
    return NULL;
  }
  tail = &tmp.arg_types;
  i = 0;
  while (i < tmp.arity) {
    if (! marshall_read_psym(mr, heap, &psym)) {
      err_puts("marshall_read_cfn: marshall_read_psym");
      assert(! "marshall_read_cfn: marshall_read_psym");
      return NULL;
    }
    if (psym == &g_sym_Result)
      tmp.arg_result = true;
    if (! (*tail = list_new_psym(psym, NULL))) {
      err_puts("marshall_read_cfn: list_new_psym");
      assert(! "marshall_read_cfn: list_new_psym");
      return NULL;
    }
    tail = &(*tail)->next.data.plist;
    i++;
  }
  if (! cfn_link(&tmp) ||
      ! cfn_prep_cif(&tmp)) {
    cfn_clean(&tmp);
    return NULL;
  }
  tmp.cif_ready = true;
  *dest = tmp;
#if HAVE_PTHREAD
  mutex_init(&dest->cif_mutex);
#endif
  return mr;
}

s_marshall_read * marshall_read_character (s_marshall_read *mr,
                                           bool heap,
                                           character *dest)
{
  s_buf *buf = {0};
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  return buf_read_character_utf8(buf, dest) < 0 ? NULL : mr;
}

s_marshall_read * marshall_read_chunk (s_marshall_read *mr)
{
  s_buf *buf;
  assert(mr);
  mr->heap_start = -((s64) sizeof(s_marshall_header));
  if (mr->heap_size) {
    if (buf_xfer(mr->heap, mr->buf, mr->heap_size) !=
        (sw) mr->heap_size) {
      err_puts("marshall_read_chunk: buf_xfer heap");
      assert(! "marshall_read_chunk: buf_xfer heap");
      return NULL;
    }
  }
  if (mr->buf_size) {
    buf = alloc(sizeof(s_buf));
    if (! buf) {
      err_puts("marshall_read_chunk: alloc buf");
      assert(! "marshall_read_chunk: alloc buf");
      return NULL;
    }
    if (! buf_init_alloc(buf, mr->buf_size)) {
      err_puts("marshall_read_chunk: buf_init_alloc");
      assert(! "marshall_read_chunk: buf_init_alloc");
      alloc_free(buf);
      return NULL;
    }
    if (buf_xfer(buf, mr->buf, mr->buf_size) !=
        (sw) mr->buf_size) {
      err_puts("marshall_read_chunk: buf_xfer buf");
      assert(! "marshall_read_chunk: buf_xfer buf");
      buf_clean(buf);
      alloc_free(buf);
      return NULL;
    }
    mr->buf = buf;
  }
  return mr;
}

s_marshall_read * marshall_read_chunk_reset (s_marshall_read *mr)
{
  assert(mr);
  mr->heap_offset += mr->heap_size;
  mr->heap_size = 0;
  mr->buf_size = 0;
  buf_empty(mr->heap);
  if (mr->source && mr->buf != mr->source) {
    buf_clean(mr->buf);
    alloc_free(mr->buf);
    mr->buf = mr->source;
    mr->buf_owned = false;
  }
  return mr;
}

s_marshall_read * marshall_read_chunk_file (s_marshall_read *mr)
{
  u64 pos;
  assert(mr);
  if (! marshall_read_header(mr))
    return NULL;
  if (! buf_tell_r(mr->buf, &pos))
    return NULL;
  mr->heap_start = (s64) pos - (s64) sizeof(s_marshall_header);
  if (buf_seek(mr->heap, pos, SEEK_SET) <= 0)
    return NULL;
  if (buf_seek(mr->buf, pos + mr->heap_size, SEEK_SET) <= 0)
    return NULL;
  return mr;
}

void marshall_read_clean (s_marshall_read *mr)
{
  assert(mr);
  if (mr->buf && mr->heap && mr->buf->user_ptr != mr->heap->user_ptr)
    buf_file_close(mr->heap);
  if (mr->buf && mr->buf_owned)
    buf_file_close(mr->buf);
  if (mr->buf && mr->buf_owned)
    buf_clean(mr->buf);
  if (mr->buf && mr->heap && mr->buf->ptr.p != mr->heap->ptr.p)
    buf_clean(mr->heap);
  if (mr->heap && mr->heap->ptr.p == mr->buf->ptr.p)
    rwlock_delete(mr->heap->rwlock);
  if (mr->heap && mr->buf != mr->heap)
    alloc_free(mr->heap);
  if (mr->buf && mr->buf_owned)
    alloc_free(mr->buf);
  marshall_read_ht_clean(mr);
}

s_marshall_read * marshall_read_complex (s_marshall_read *mr,
                                         bool heap,
                                         s_complex *dest)
{
  s_complex tmp = {0};
  assert(mr);
  assert(mr);
  if (! marshall_read_tag(mr, heap, &tmp.x))
    return NULL;
  if (! marshall_read_tag(mr, heap, &tmp.y)) {
    tag_clean(&tmp.x);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read *marshall_read_cow (s_marshall_read *mr,
                                    bool heap,
                                    s_cow *dest)
{
  s_cow tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_sym(mr, heap, &tmp.type) ||
      ! marshall_read_list(mr, heap, tmp.list))
        return NULL;
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  if (! mutex_init(&dest->mutex)) {
    list_clean(dest->list);
    return NULL;
  }
#endif
  return mr;
}

s_marshall_read * marshall_read_data (s_marshall_read *mr, bool heap,
                                      p_sym type, void *data)
{
  s_struct s = {0};
  p_struct_type st = NULL;
  assert(mr);
  assert(type);
  assert(data);
  if (! mr || ! type || ! data)
    return NULL;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return marshall_read_array(mr, heap, data);
  if (type == &g_sym_Bool)
    return marshall_read_bool(mr, heap, data);
  if (type == &g_sym_Call)
    return marshall_read_pcall(mr, heap, data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return marshall_read_pcallable(mr, heap, data);
  if (type == &g_sym_Character)
    return marshall_read_character(mr, heap, data);
  if (type == &g_sym_Complex)
    return marshall_read_pcomplex(mr, heap, data);
  if (type == &g_sym_Cow)
    return marshall_read_pcow(mr, heap, data);
  if (type == &g_sym_F32)
    return marshall_read_f32(mr, heap, data);
  if (type == &g_sym_F64)
    return marshall_read_f64(mr, heap, data);
#if HAVE_F80
  if (type == &g_sym_F80)
    return marshall_read_f80(mr, heap, data);
#endif
#if HAVE_F128
  if (type == &g_sym_F128)
    return marshall_read_f128(mr, heap, data);
#endif
  if (type == &g_sym_Fact)
    return marshall_read_fact(mr, heap, data);
  if (type == &g_sym_Ident)
    return marshall_read_ident(mr, heap, data);
  if (type == &g_sym_Integer)
    return marshall_read_integer(mr, heap, data);
  if (type == &g_sym_List)
    return marshall_read_plist(mr, heap, data);
  if (type == &g_sym_Map)
    return marshall_read_map(mr, heap, data);
  if (type == &g_sym_Ptr)
    return marshall_read_ptr(mr, heap, data);
  if (type == &g_sym_PtrFree)
    return marshall_read_ptr_free(mr, heap, data);
  if (sym_is_pointer_type(type, NULL)) {
    s_pointer p = {0};
    if (! marshall_read_pointer(mr, heap, &p))
      return NULL;
    *(void **) data = p.ptr.p;
    return mr;
  }
  if (type == &g_sym_Quote)
    return marshall_read_quote(mr, heap, data);
  if (type == &g_sym_S8)
    return marshall_read_s8(mr, heap, data);
  if (type == &g_sym_S16)
    return marshall_read_s16(mr, heap, data);
  if (type == &g_sym_S32)
    return marshall_read_s32(mr, heap, data);
  if (type == &g_sym_S64)
    return marshall_read_s64(mr, heap, data);
  if (type == &g_sym_Str)
    return marshall_read_str(mr, heap, data);
  if (type == &g_sym_Struct)
    return marshall_read_pstruct(mr, heap, data);
  if (type == &g_sym_StructType)
    return marshall_read_pstruct_type(mr, heap, data);
  if (type == &g_sym_Sw)
    return marshall_read_sw(mr, heap, data);
  if (type == &g_sym_Sym)
    return marshall_read_psym(mr, heap, data);
  if (type == &g_sym_Tag)
    return marshall_read_tag(mr, heap, data);
  if (type == &g_sym_Time)
    return marshall_read_time(mr, heap, data);
  if (type == &g_sym_Tuple)
    return marshall_read_tuple(mr, heap, data);
  if (type == &g_sym_U8)
    return marshall_read_u8(mr, heap, data);
  if (type == &g_sym_U16)
    return marshall_read_u16(mr, heap, data);
  if (type == &g_sym_U32)
    return marshall_read_u32(mr, heap, data);
  if (type == &g_sym_U64)
    return marshall_read_u64(mr, heap, data);
  if (type == &g_sym_Uw)
    return marshall_read_uw(mr, heap, data);
  if (type == &g_sym_Var)
    return marshall_read_pvar(mr, heap, data);
  if (type == &g_sym_Void)
    return mr;
  if (! pstruct_type_find(type, &st))
    return NULL;
  if (st) {
    s.pstruct_type = st;
    s.data = data;
    marshall_read_struct(mr, heap, &s);
  }
  return mr;
}

void marshall_read_delete (s_marshall_read *mr)
{
  assert(mr);
  marshall_read_clean(mr);
  alloc_free(mr);
}

s_marshall_read * marshall_read_dimensions (s_marshall_read *mr,
                                            bool heap,
                                            s_array_dimension *dest)
{
  s_array_dimension tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_uw(mr, heap, &tmp.count) ||
      ! marshall_read_uw(mr, heap, &tmp.item_size))
    return NULL;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_do_block (s_marshall_read *mr,
                                          bool heap,
                                          s_do_block *dest)
{
  uw count;
  uw i;
  bool short_form;
  s_do_block tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_uw(mr, heap, &count)  ||
      ! marshall_read_bool(mr, heap, &short_form) ||
      ! do_block_init(&tmp, count))
    return NULL;
  tmp.count = count;
  tmp.short_form = short_form;
  i = 0;
  while (i < count) {
    if (! marshall_read_tag(mr, heap, tmp.tag + i)) {
      do_block_clean(&tmp);
      return NULL;
    }
    i++;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_env (s_marshall_read *mr,
                                     bool heap, s_env *env)
{
  void   *dl;
  s_list *dlopen_list_reverse = NULL;
  s_list *dlopen_list = NULL;
  if (! mr || ! env) {
    err_puts("marshall_read_env: invalid argument");
    assert(! "marshall_read_env: invalid argument");
    return NULL;
  }
  if (! marshall_read_1(mr, heap, "_KC3ENV_")) {
    err_puts("marshall_read_env: marshall_read_1 magic");
    assert(! "marshall_read_env: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_plist(mr, heap, &dlopen_list_reverse)) {
    err_puts("marshall_read_env: marshall_read_plist dlopen_list");
    assert(! "marshall_read_env: marshall_read_plist dlopen_list");
    return NULL;
  }
  if (! plist_reverse(&dlopen_list_reverse, &dlopen_list)) {
    err_puts("marshall_read_env: plist dlopen_list");
    assert(! "marshall_read_env: plist dlopen_list");
    return NULL;
  }
  list_delete_all(dlopen_list_reverse);
  while (dlopen_list) {
    if (! kc3_dlopen(&dlopen_list->tag.data.str, &dl)) {
      err_puts("marshall_read_env: dlopen_list has a non-Str");
      assert(! "marshall_read_env: dlopen_list has a non-Str");
      return NULL;
    }
    dlopen_list = list_delete(dlopen_list);
  }
  if (! env->facts &&
      ! (env->facts = facts_new())) {
    err_puts("marshall_read_env: facts_new");
    assert(! "marshall_read_env: facts_new");
    return NULL;
  }
  if (! marshall_read_facts(mr, heap, env->facts)) {
    err_puts("marshall_read_env: marshall_read_facts");
    assert(! "marshall_read_env: marshall_read_facts");
    return NULL;
  }
  if (! env->ops &&
      ! (env->ops = ops_new())) {
    err_puts("marshall_read_env: ops_new");
    assert(! "marshall_read_env: ops_new");
    return NULL;
  }
  if (! marshall_read_ops(mr, heap, env->ops)) {
    err_puts("marshall_read_env: marshall_read_ops");
    assert(! "marshall_read_env: marshall_read_ops");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &g_psym_anon_serial)) {
    err_puts("marshall_read_env: marshall_read_uw anon Sym serial");
    assert(! "marshall_read_env: marshall_read_uw anon Sym serial");
    return NULL;
  }
  return mr;
}

sw marshall_read_env_from_file (s_env *env, const s_str *path)
{
  s_marshall_read mr = {0};
  sw result = -1;
  if (! env || ! path || ! path->size) {
    err_puts("marshall_read_env_from_file: invalid argument");
    assert(! "marshall_read_env_from_file: invalid argument");
    return -1;
  }
  if (! marshall_read_init_file(&mr, path)) {
    err_puts("marshall_read_env_from_file: marshall_read_init_file");
    assert(! "marshall_read_env_from_file: marshall_read_init_file");
    return -1;
  }
  if ((result = marshall_read_size(&mr)) <= 0) {
    err_puts("marshall_read_env_from_file: marshall_read_size");
    assert(! "marshall_read_env_from_file: marshall_read_size");
    goto clean;
  }
  if (! marshall_read_env(&mr, false, env)) {
    err_puts("marshall_read_env_from_file: marshall_read_env");
    assert(! "marshall_read_env_from_file: marshall_read_env");
    goto clean;
  }
  marshall_read_clean(&mr);
  return result;
 clean:
  marshall_read_clean(&mr);
  return -1;
}

s_marshall_read * marshall_read_f32 (s_marshall_read *mr,
                                     bool heap,
                                     f32 *dest)
{
  s_buf *buf = NULL;
  union { f32 f; u32 i; } u;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3F32_") <= 0) {
    err_puts("marshall_read_f32: buf_read_1 magic");
    assert(! "marshall_read_f32: buf_read_1 magic");
    return NULL;
  }
  if (buf_read_u32(buf, &u.i) <= 0) {
    err_puts("marshall_read_f32: buf_read_u32");
    assert(! "marshall_read_f32: buf_read_u32");
    return NULL;
  }
  u.i = le32toh(u.i);
  *dest = u.f;
  return mr;
}

s_marshall_read * marshall_read_f64 (s_marshall_read *mr,
                                     bool heap,
                                     f64 *dest)
{
  s_buf *buf = NULL;
  union { f64 f; u64 i; } u;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3F64_") <= 0) {
    err_puts("marshall_read_f64: buf_read_1 magic");
    assert(! "marshall_read_f64: buf_read_1 magic");
    return NULL;
  }
  if (buf_read_u64(buf, &u.i) <= 0) {
    err_puts("marshall_read_f64: buf_read_u64");
    assert(! "marshall_read_f64: buf_read_u64");
    return NULL;
  }
  u.i = le64toh(u.i);
  *dest = u.f;
  return mr;
}

#if HAVE_F80

s_marshall_read * marshall_read_f80 (s_marshall_read *mr,
                                     bool heap,
                                     f80 *dest)
{
  s_buf *buf = NULL;
  f80 tmp;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3F80_") <= 0) {
    err_puts("marshall_read_f80: buf_read_1 magic");
    assert(! "marshall_read_f80: buf_read_1 magic");
    return NULL;
  }
  if (buf_read_f80(buf, &tmp) <= 0) {
    err_puts("marshall_read_f80: buf_read_f80");
    assert(! "marshall_read_f80: buf_read_f80");
    return NULL;
  }
  *dest = tmp;
  return mr;
}
#endif

#if HAVE_F128

s_marshall_read * marshall_read_f128 (s_marshall_read *mr,
                                      bool heap,
                                      f128 *dest)
{
  s_buf *buf = NULL;
  union { f128 f; u64 i[2]; } u;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3F128_") <= 0) {
    err_puts("marshall_read_f128: buf_read_1 magic");
    assert(! "marshall_read_f128: buf_read_1 magic");
    return NULL;
  }
  if (buf_read_u64(buf, &u.i[0]) <= 0 ||
      buf_read_u64(buf, &u.i[1]) <= 0) {
    err_puts("marshall_read_f128: buf_read_u64");
    assert(! "marshall_read_f128: buf_read_u64");
    return NULL;
  }
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
  u.i[0] = le64toh(u.i[0]);
  u.i[1] = le64toh(u.i[1]);
  tmp = u.i[0];
  u.i[0] = u.i[1];
  u.i[1] = tmp;
#endif
  *dest = u.f;
  return mr;
}

#endif

s_marshall_read * marshall_read_fact (s_marshall_read *mr,
                                      bool heap,
                                      s_fact *dest)
{
  s_fact tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3FACT_")) {
    err_puts("marshall_read_fact: marshall_read_1 magic");
    assert(! "marshall_read_fact: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_ptag(mr, heap, &tmp.subject)) {
    err_puts("marshall_read_fact: marshall_read_ptag subject");
    assert(! "marshall_read_fact: marshall_read_ptag subject");
    return NULL;
  }
  if (! marshall_read_ptag(mr, heap, &tmp.predicate)) {
    err_puts("marshall_read_fact: marshall_read_ptag predicate");
    assert(! "marshall_read_fact: marshall_read_ptag predicate");
    tag_clean(tmp.subject);
    return NULL;
  }
  if (! marshall_read_ptag(mr, heap, &tmp.object)) {
    err_puts("marshall_read_fact: marshall_read_ptag object");
    assert(! "marshall_read_fact: marshall_read_ptag object");
    tag_clean(tmp.subject);
    tag_clean(tmp.predicate);
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &tmp.id)) {
    err_puts("marshall_read_fact: marshall_read_uw id");
    assert(! "marshall_read_fact: marshall_read_uw id");
    tag_clean(tmp.subject);
    tag_clean(tmp.predicate);
    tag_clean(tmp.object);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_facts (s_marshall_read *mr,
                                       bool heap, s_facts *facts)
{
  s_str binary_path = {0};
  uw count;
  s_env *env;
  s_fact fact = {0};
  bool has_log = false;
  uw i;
  s_str path = {0};
  env = env_global();
  if (! mr || ! facts || ! env)
    return NULL;
  if (! marshall_read_1(mr, heap, "_KC3FACTS_")) {
    err_puts("marshall_read_facts: marshall_read_1 magic");
    assert(! "marshall_read_facts: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &count)) {
    err_puts("marshall_read_facts: marshall_read_uw count");
    assert(! "marshall_read_facts: marshall_read_uw count");
    return NULL;
  }
  i = 0;
  while (i < count) {
    if (false) {
      err_write_1("marshall_read_facts: #");
      err_inspect_uw_decimal(i);
      err_write_1("\n");
    }
    if (! marshall_read_fact(mr, heap, &fact)) {
      err_puts("marshall_read_facts: marshall_read_fact");
      assert(! "marshall_read_facts: marshall_read_fact");
      return NULL;
    }
    if (! facts_add_fact(facts, &fact)) {
      fact_clean_all(&fact);
      return NULL;
    }
    if (false) {
      err_inspect_fact(&fact);
      err_write_1("\n");
    }
    fact_clean_all(&fact);
    i++;
  }
  if (! marshall_read_bool(mr, heap, &has_log)) {
    err_puts("marshall_read_facts: marshall_bool log");
    assert(! "marshall_read_facts: marshall_bool log");
    return NULL;
  }
  if (has_log) {
    if (! marshall_read_str(mr, heap, &path)) {
      err_puts("marshall_read_facts: marshall_str path");
      assert(! "marshall_read_facts: marshall_str path");
      return NULL;
    }
    if (! marshall_read_str(mr, heap, &binary_path)) {
      err_puts("marshall_read_facts: marshall_str binary path");
      assert(! "marshall_read_facts: marshall_str binary path");
      str_clean(&path);
      return NULL;
    }
    if (! binary_path.size) {
      if (! log_path_to_binary_path(&path, &binary_path)) {
        err_puts("marshall_read_facts: log_path_to_binary_path");
        assert(! "marshall_read_facts: log_path_to_binary_path");
        str_clean(&path);
        return NULL;
      }
    }
    if (env->trace) {
      err_write_1("marshall_read_facts: facts_open_file_after_dump: ");
      err_inspect_str(&binary_path);
      err_write_1("\n");
    }
    if (facts_open_file_after_dump(facts, &binary_path) < 0) {
      err_write_1("marshall_read_facts: facts_open_file_after_dump: ");
      err_inspect_str(&binary_path);
      err_write_1(": ERROR\n");
      assert(! "marshall_read_facts: facts_open_file_after_dump");
      str_clean(&binary_path);
      str_clean(&path);
      return NULL;
    }
    if (env->trace) {
      err_write_1("marshall_read_facts: binary log: ");
      err_inspect_str(&binary_path);
      err_write_1(": OK\n");
    }
    if (facts_save_binary_file(facts, &path) < 0) {
      err_write_1("marshall_read_facts: facts_save_binary_file: ");
      err_inspect_str(&path);
      err_write_1("\n");
      assert(! "marshall_read_facts: facts_save_binary_file");
      str_clean(&binary_path);
      str_clean(&path);
      return NULL;
    }
    str_clean(&binary_path);
    str_clean(&path);
  }
  return mr;
}

s_marshall_read * marshall_read_fn (s_marshall_read *mr, bool heap,
                                    s_fn *dest)
{
  s_fn_clause **clause;
  uw clause_count;
  uw i;
  s_fn tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3FN_")) {
    err_puts("marshall_read_fn: marshall_read_1 magic");
    assert(! "marshall_read_fn: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_bool(mr, heap, &tmp.macro)) {
    err_puts("marshall_read_fn: marshall_read_bool macro");
    return NULL;
  }
  if (! marshall_read_bool(mr, heap, &tmp.special_operator)) {
    err_puts("marshall_read_fn: marshall_read_bool special_operator");
    return NULL;
  }
  if (! marshall_read_ident(mr, heap, &tmp.name)) {
    err_puts("marshall_read_fn: marshall_read_ident name");
    return NULL;
  }
  if (! marshall_read_psym(mr, heap, &tmp.module)) {
    err_puts("marshall_read_fn: marshall_read_psym module");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &clause_count)) {
    err_puts("marshall_read_fn: marshall_read_uw clause_count");
    return NULL;
  }
  clause = &tmp.clauses;
  i = 0;
  while (i < clause_count) {
    if (! (*clause = alloc(sizeof(s_fn_clause))) ||
        ! marshall_read_plist(mr, heap, &(*clause)->pattern) ||
        ! marshall_read_do_block(mr, heap, &(*clause)->algo)) {
      fn_clean(&tmp);
      return NULL;
    }
    clause = &(*clause)->next;
    i++;
  }
  if (! marshall_read_pframe(mr, heap, &tmp.frame)) {
    err_puts("marshall_read_fn: marshall_read_pframe frame");
    fn_clean(&tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_frame (s_marshall_read *mr, bool heap,
                                       s_frame *dest)
{
  uw          binding_count;
  s_binding **binding;
  uw i;
  s_frame tmp = {0};
  if (! marshall_read_1(mr, heap, "_KC3FRAME_")) {
    err_puts("marshall_read_frame: marshall_read_1 magic");
    assert(! "marshall_read_frame: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &binding_count)) {
    err_puts("marshall_read_frame: marshall_read_uw binding_count");
    assert(! "marshall_read_frame: marshall_read_uw binding_count");
    return NULL;
  }
  binding = &tmp.bindings;
  i = 0;
  while (i < binding_count) {
    if (! (*binding = alloc(sizeof(s_binding))))
      goto ko;
    if (! marshall_read_psym(mr, heap, &(*binding)->name)) {
      err_puts("marshall_read_frame: marshall_read_psym name");
      goto ko;
    }
    if (! marshall_read_tag(mr, heap, &(*binding)->value)) {
      err_puts("marshall_read_frame: marshall_read_tag value");
      goto ko;
    }
    binding = &(*binding)->next;
    i++;
  }
  if (! marshall_read_pframe(mr, heap, &tmp.next)) {
      err_puts("marshall_read_frame: marshall_read_pframe next");
      goto ko;
    }
  *dest = tmp;
  return mr;
 ko:
  frame_clean(&tmp);
  assert("marshall_read_frame: error");
  return NULL;
}

s_marshall_read * marshall_read_header (s_marshall_read *mr)
{
  s_marshall_header mh = {0};
  s_str str = {0};
  s_marshall_read tmp;
  assert(mr);
  tmp = *mr;
  if (! buf_read(tmp.buf, sizeof(s_marshall_header), &str))
    return NULL;
  if (str.size != sizeof(s_marshall_header)) {
    err_puts("marshall_read_header: buf_read !="
             " sizeof(s_marshall_header)");
    assert(!("marshall_read_header: buf_read !="
             " sizeof(s_marshall_header)"));
    return NULL;
  }
  mh = *(s_marshall_header *) str.ptr.p;
  if (le64toh(mh.le_magic) != MARSHALL_MAGIC) {
    err_puts("marshall_read_header: invalid magic");
    assert(! "marshall_read_header: invalid magic");
    return NULL;
  }
  tmp.heap_offset = le64toh(mh.le_heap_offset);
  tmp.heap_count = le64toh(mh.le_heap_count);
  tmp.heap_size = le64toh(mh.le_heap_size);
  tmp.buf_size = le64toh(mh.le_buf_size);
  if (tmp.heap_size && ! tmp.heap_count) {
    err_puts("marshall_read_header: invalid heap count");
    assert(! "marshall_read_header: invalid heap count");
    return NULL;
  }
  *mr = tmp;
  if (mr->heap_count && ! mr->ht.items) {
    if (! ht_init(&mr->ht, &g_sym_Tuple, mr->heap_count)) {
      err_puts("marshall_read_header: ht_init");
      assert(! "marshall_read_header: ht_init");
      return NULL;
    }
    mr->ht.compare = marshall_read_ht_compare;
    mr->ht.hash = marshall_read_ht_hash;
  }
  str_clean(&str);
  return mr;
}

s_marshall_read * marshall_read_heap_pointer (s_marshall_read *mr,
                                              bool heap,
                                              u64 *offset,
                                              void **present)
{
  assert(mr);
  assert(offset);
  assert(present);
  s64 global_offset;
  s_tag key = {0};
  s_tag *ptag = NULL;
  s_tag tag = {0};
  if (! marshall_read_offset(mr, heap, offset))
    return NULL;
  if (! *offset) {
    *present = NULL;
    return mr;
  }
  if (mr->heap_start <= 0)
    global_offset = (s64) mr->heap_offset + (s64) *offset;
  else
    global_offset = mr->heap_start + (s64) *offset;
  if (! tag_init_ptuple(&key, 2))
    return NULL;
  tag_init_s64(key.data.ptuple->tag, global_offset);
  if (! ht_get(&mr->ht, &key, &ptag)) {
    *present = NULL;
    tag_clean(&key);
    return mr;
  }
  tag_clean(&key);
  if (ptag->type != TAG_PTUPLE ||
      ptag->data.ptuple->tag[1].type != TAG_UW) {
    err_puts("marshall_read_heap_pointer: invalid tag in ht");
    assert(! "marshall_read_heap_pointer: invalid tag in ht");
  }
  *present = (void *) ptag->data.ptuple->tag[1].data.uw;
  tag_clean(&tag);
  return mr;
}

s_marshall_read * marshall_read_ht_add (s_marshall_read *mr,
                                        u64 offset, const void *p)
{
  s64 global_offset;
  s_tag tag = {0};
  if (mr->heap_start <= 0)
    global_offset = (s64) mr->heap_offset + (s64) offset;
  else
    global_offset = mr->heap_start + (s64) offset;
  if (! tag_init_ptuple(&tag, 2))
    return NULL;
  tag_init_s64(tag.data.ptuple->tag, global_offset);
  tag_init_uw(tag.data.ptuple->tag + 1, (uw) p);
  if (! ht_add(&mr->ht, &tag)) {
    tag_clean(&tag);
    return NULL;
  }
  tag_clean(&tag);
  return mr;
}

void marshall_read_ht_clean (s_marshall_read *mr)
{
  assert(mr);
  if (mr->ht.items)
    ht_clean(&mr->ht);
}

s8 marshall_read_ht_compare (const s_tag *a, const s_tag *b)
{
  assert(a);
  assert(b);
  assert(a->type == TAG_PTUPLE);
  assert(b->type == TAG_PTUPLE);
  if (a->type < b->type)
    return -1;
  if (a->type > b->type)
    return 1;
  if (a->type != TAG_PTUPLE)
    return COMPARE_ERROR;
  return compare_tag(a->data.ptuple->tag, b->data.ptuple->tag);
}

uw  marshall_read_ht_hash (const s_tag *tag)
{
  t_hash h;
  assert(tag);
  assert(tag->type == TAG_PTUPLE);
  if (tag->type != TAG_PTUPLE)
    abort();
  hash_init(&h);
  hash_update_tag(&h, tag->data.ptuple->tag);
  return hash_to_uw(&h);
}

s_marshall_read * marshall_read_ident (s_marshall_read *mr, bool heap,
                                       s_ident *dest)
{
  s_ident tmp_id = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3IDENT_")) {
    err_puts("marshall_read_ident: marshall_read_1 magic");
    assert(! "marshall_read_ident: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_psym(mr, heap, &tmp_id.module) ||
      ! marshall_read_psym(mr, heap, &tmp_id.sym))
    return NULL;
  *dest = tmp_id;
  return mr;
}

s_marshall_read * marshall_read_init (s_marshall_read *mr)
{
  assert(mr);
  *mr = (s_marshall_read) {0};
  if (! (mr->heap = alloc(sizeof(s_buf))) ||
      ! buf_init_alloc(mr->heap, BUF_SIZE)) {
    err_puts("marshall_read_init: heap allocation error");
    assert(! "marshall_read_init: heap allocation error");
    return NULL;
  }
  if (! (mr->buf = alloc(sizeof(s_buf))) ||
      ! buf_init_alloc(mr->buf, BUF_SIZE)) {
    err_puts("marshall_read_init: buffer allocation error");
    assert(! "marshall_read_init: buffer allocation error");
    buf_clean(mr->heap);
    alloc_free(mr->heap);
    return NULL;
  }
  mr->buf_owned = true;
  return mr;
}

s_marshall_read * marshall_read_init_buf (s_marshall_read *mr,
                                          s_buf *buf)
{
  assert(mr);
  assert(buf);
  *mr = (s_marshall_read) {0};
  if (! (mr->heap = alloc(sizeof(s_buf))) ||
      ! buf_init_alloc(mr->heap, BUF_SIZE)) {
    err_puts("marshall_read_init_buf: heap allocation error");
    assert(! "marshall_read_init_buf: heap allocation error");
    return NULL;
  }
  mr->buf = buf;
  mr->source = buf;
  return mr;
}

s_marshall_read * marshall_read_init_1 (s_marshall_read *mr,
                                        const char *p, uw size)
{
  s_str str = {0};
  if (! mr || ! p || ! size) {
    err_puts("marshall_read_init_1: invalid argument");
    assert(! "marshall_read_init_1: invalid argument");
    return NULL;
  }
  str.size = size;
  str.ptr.pchar = p;
  return marshall_read_init_str(mr, &str);
}

s_marshall_read * marshall_read_init_file (s_marshall_read *mr,
                                           const s_str *path)
{
  const char *error_msg = "unknown error";
  FILE *fp;
  assert(mr);
  assert(path);
  *mr = (s_marshall_read) {0};
  if (! marshall_read_init(mr))
    return NULL;
  if (! (fp = file_open(path, "rb"))) {
    error_msg = "file_open buf";
    goto ko;
  }
  if (! buf_file_open_r(mr->buf, fp)) {
    error_msg = "buf_file_open_r buf";
    fclose(fp);
    goto ko;
  }
  mr->source = mr->buf;
  if (! marshall_read_header(mr)) {
    error_msg = "marshall_read_header";
    goto ko_close;
  }
  if (buf_seek(mr->buf, sizeof(s_marshall_header) +
               mr->heap_size, SEEK_SET) <= 0) {
    error_msg = "buf_seek buf";
    goto ko_close;
  }
  if (! (fp = file_open(path, "rb"))) {
    error_msg = "file_open heap";
    goto ko_close;
  }
  if (! buf_file_open_r(mr->heap, fp)) {
    error_msg = "buf_file_open_r heap";
    fclose(fp);
    goto ko_close;
  }
  if (buf_seek(mr->heap, sizeof(s_marshall_header), SEEK_SET) <= 0) {
    error_msg = "buf_seek heap";
    buf_file_close(mr->heap);
    goto ko_close;
  }
  mr->heap_start = 0;
  return mr;
 ko_close:
  buf_file_close(mr->buf);
 ko:
  err_write_1("marshall_read_init_file: ");
  err_inspect_str(path);
  err_write_1(": ");
  err_puts(error_msg);
  assert(! "marshall_read_init_file: error");
  marshall_read_clean(mr);
  return NULL;
}

s_marshall_read * marshall_read_init_str (s_marshall_read *mr,
                                          const s_str *dest)
{
  assert(mr);
  *mr = (s_marshall_read) {0};
  if (! (mr->buf = alloc(sizeof(s_buf)))) {
    err_puts("marshall_read_init_str: buf allocation error");
    assert(! "marshall_read_init_str: buf allocation error");
    return NULL;
  }
  if (! buf_init_str_copy(mr->buf, dest)) {
    err_puts("marshall_read_init_str: buf_init_str_copy");
    assert(! "marshall_read_init_str: buf_init_str_copy");
    alloc_free(mr->buf);
    return NULL;
  }
  if (! marshall_read_header(mr)) {
    err_puts("marshall_read_init_str: marshall_read_header");
    assert(! "marshall_read_init_str: marshall_read_header");
    buf_clean(mr->buf);
    alloc_free(mr->buf);
    return NULL;
  }
  mr->buf->rpos = sizeof(s_marshall_header) + mr->heap_size;
  if (mr->buf->rpos + mr->buf_size != mr->buf->wpos) {
    err_puts("marshall_read_init_str: invalid buffer size");
    assert(! "marshall_read_init_str: invalid buffer size");
    buf_clean(mr->buf);
    alloc_free(mr->buf);
    return NULL;
  }
  if (! (mr->heap = alloc(sizeof(s_buf)))) {
    err_puts("marshall_read_init_str: heap allocation error");
    assert(! "marshall_read_init_str: heap allocation error");
    buf_clean(mr->buf);
    alloc_free(mr->buf);
    return NULL;
  }
  *mr->heap = *mr->buf;
  mr->heap->free = false;
  mr->heap->rpos = sizeof(s_marshall_header);
  mr->heap->wpos = mr->heap->rpos + mr->heap_size;
#if HAVE_PTHREAD
  mr->heap->rwlock = rwlock_new();
#endif
  mr->buf_owned = true;
  return mr;
}

s_marshall_read * marshall_read_integer (s_marshall_read *mr, bool heap,
                                         s_integer *dest)
{
  const sw digit_size = 4;
  const sw nail_bits = 4;
  s_buf *buf;
  u32 count;
  u8 sign;
  sw size;
  s_integer tmp;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3INTEGER_") <= 0) {
    err_puts("marshall_read_integer: buf_read_1 magic");
    assert(! "marshall_read_integer: buf_read_1 magic");
    return NULL;
  }
  if (buf_read_u8(buf, &sign) <= 0) {
    err_puts("marshall_read_integer: buf_read_u8 sign");
    assert(! "marshall_read_integer: buf_read_u8 sign");
    return NULL;
  }
  if (buf_read_u32(buf, &count) <= 0) {
    err_puts("marshall_read_integer: buf_read_u32 count");
    assert(! "marshall_read_integer: buf_read_u32 count");
    return NULL;
  }
  count = le32toh(count);
  integer_init(&tmp);
  if (count == 0) {
    mp_set_u32(&tmp.mp_int, 0);
    *dest = tmp;
    return mr;
  }
  size = count * digit_size;
  if (buf->rpos + size > buf->wpos) {
    sw r;
    if ((r = buf_refill(buf, size)) < 0) {
      integer_clean(&tmp);
      return NULL;
    }
    if (buf->rpos + size > buf->wpos) {
      err_puts("marshall_read_integer: short read");
      assert(! "marshall_read_integer: short read");
      integer_clean(&tmp);
      return NULL;
    }
  }
  if (mp_unpack(&tmp.mp_int, count, MP_LSB_FIRST, digit_size,
                MP_LITTLE_ENDIAN, nail_bits,
                buf->ptr.pu8 + buf->rpos) != MP_OKAY) {
    err_puts("marshall_read_integer: mp_unpack failed");
    assert(! "marshall_read_integer: mp_unpack failed");
    integer_clean(&tmp);
    return NULL;
  }
  if (sign) {
    if (mp_neg(&tmp.mp_int, &tmp.mp_int) != MP_OKAY) {
      err_puts("marshall_read_integer: mp_neg failed");
      assert(! "marshall_read_integer: mp_neg failed");
      integer_clean(&tmp);
      return NULL;
    }
  }
  buf->rpos += size;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_list (s_marshall_read *mr, bool heap,
                                      s_list *dest)
{
  s_list tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3LIST_")) {
    err_puts("marshall_read_list: marshall_read_1 magic");
    assert(! "marshall_read_list: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_tag(mr, heap, &tmp.tag))
    return NULL;
  if (! marshall_read_tag(mr, heap, &tmp.next)) {
    tag_clean(&tmp.tag);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_map (s_marshall_read *mr,
                                     bool heap,
                                     s_map *dest)
{
  s_map tmp = {0};
  uw count;
  uw i = 0;
  assert(mr);
  assert(mr);
  if (! marshall_read_1(mr, heap, "_KC3MAP_")) {
    err_puts("marshall_read_map: marshall_read_1 magic");
    assert(! "marshall_read_map: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &count) ||
      (count && ! map_init(&tmp, count)))
    return NULL;
  tmp.count = count;
  while (i < tmp.count) {
    if (! marshall_read_tag(mr, heap, tmp.key + i) ||
        ! marshall_read_tag(mr, heap, tmp.value + i)) {
      map_clean(&tmp);
      return NULL;
    }
    i++;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_new (void)
{
  s_marshall_read *mr = NULL;
  if (! (mr = alloc(sizeof(s_marshall_read))))
    return NULL;
  if (! marshall_read_init(mr)) {
    alloc_free(mr);
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_new_str (const s_str *input)
{
  s_marshall_read *mr = NULL;
  if (! (mr = alloc(sizeof(s_marshall_read))))
    return NULL;
  if (! marshall_read_init_str(mr, input)) {
    alloc_free(mr);
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_offset (s_marshall_read *mr,
                                        bool heap, u64 *dest)
{
  s_buf *buf = NULL;
  u64 tmp = 0;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_u64(buf, &tmp) <= 0) {
    err_puts("marshall_read_offset: buf_read_u64");
    assert(! "marshall_read_offset: buf_read_u64");
    return NULL;
  }
  *dest = le64toh(tmp);
  return mr;
}

s_marshall_read * marshall_read_op (s_marshall_read *mr,
                                    bool heap, s_op *op)
{
  if (! mr || ! op) {
    err_puts("marshall_read_op: invalid argument");
    assert(! "marshall_read_op: invalid argument");
    return NULL;
  }
  if (! marshall_read_1(mr, heap, "_KC3OP_") ||
      ! marshall_read_psym(mr, heap, &op->sym) ||
      ! marshall_read_u8(mr, heap, &op->arity) ||
      ! marshall_read_bool(mr, heap, &op->special) ||
      ! marshall_read_u8(mr, heap, &op->precedence) ||
      ! marshall_read_u8(mr, heap, &op->associativity) ||
      ! marshall_read_pcallable(mr, heap, &op->pcallable)) {
    err_puts("marshall_read_op: marshall read error");
    assert(! "marshall_read_op: marshall read error");
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_ops (s_marshall_read *mr,
                                     bool heap, s_ops *ops)
{
  uw count = 0;
  uw i;
  s_op op = {0};
  if (! marshall_read_1(mr, heap, "_KC3OPS_")) {
    err_puts("marshall_read_ops: marshall_read_1 magic");
    assert(! "marshall_read_ops: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &count))
    return NULL;
  if (! count)
    return mr;
  i = 0;
  while (i < count) {
    if (! marshall_read_op(mr, heap, &op)) {
      err_puts("marshall_read_ops: marshall_read_op");
      assert(! "marshall_read_ops: marshall_read_op");
      return NULL;
    }
    if (! ops_add(ops, &op)) {
      err_puts("marshall_read_ops: ops_add");
      assert(! "marshall_read_ops: ops_add");
      op_clean(&op);
      return NULL;
    }
    if (! env_global()->pass_by_copy &&
        op.pcallable->ref_count < 2) {
      err_puts("marshall_read_ops: invalid op ref count");
      assert(! "marshall_read_ops: invalid op ref count");
      op_clean(&op);
      return NULL;
    }
    op_clean(&op);
    i++;
  }
  return mr;
}

s_marshall_read * marshall_read_pcall (s_marshall_read *mr,
                                       bool heap,
                                       p_call *dest)
{
  u64 offset = 0;
  p_call present = NULL;
  p_call tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PCALL_")) {
    err_puts("marshall_read_pcall: marshall_read_1 magic");
    assert(! "marshall_read_pcall: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present)) {
    err_puts("marshall_read_pcall: marshall_read_heap_pointer");
    assert(! "marshall_read_pcall: marshall_read_heap_pointer");
    return NULL;
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcall_init_copy(dest, &present)) {
      err_puts("marshall_read_pcall: pcall_init_copy");
      assert(! "marshall_read_pcall: pcall_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_pcall: buf_seek");
    assert(! "marshall_read_pcall: buf_seek");
    return NULL;
  }
  if (! (tmp = alloc(sizeof(s_call)))) {
    err_puts("marshall_read_pcall: alloc");
    assert(! "marshall_read_pcall: alloc");
    return NULL;
  }
  if (! marshall_read_call(mr, true, tmp)) {
    err_puts("marshall_read_pcall: marshall_read_call");
    assert(! "marshall_read_pcall: marshall_read_call");
    alloc_free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    err_puts("marshall_read_pcall: marshall_read_ht_add");
    assert(! "marshall_read_pcall: marshall_read_ht_add");
    call_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pcallable (s_marshall_read *mr,
                                           bool heap,
                                           p_callable *dest)
{
  u64 offset = 0;
  p_callable present = NULL;
  p_callable tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PCALLABLE_")) {
    err_puts("marshall_read_pcallable: marshall_read_1 magic");
    assert(! "marshall_read_pcallable: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present)) {
    err_puts("marshall_read_pcallable: marshall_read_heap_pointer");
    assert(! "marshall_read_pcallable: marshall_read_heap_pointer");
    return NULL;
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcallable_init_copy(dest, &present)) {
      err_puts("marshall_read_pcallable: pcallable_init_copy");
      assert(! "marshall_read_pcallable: pcallable_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_pcallable: buf_seek");
    assert(! "marshall_read_pcallable: buf_seek");
    return NULL;
  }
  if (! (tmp = alloc(sizeof(s_callable)))) {
    err_puts("marshall_read_pcallable: alloc");
    assert(! "marshall_read_pcallable: alloc");
    return NULL;
  }
  if (! marshall_read_callable(mr, true, tmp)) {
    err_puts("marshall_read_pcallable: marshall_read_callable");
    assert(! "marshall_read_pcallable: marshall_read_callable");
    alloc_free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    err_puts("marshall_read_pcallable: marshall_read_ht_add");
    assert(! "marshall_read_pcallable: marshall_read_ht_add");
    callable_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pcomplex (s_marshall_read *mr,
                                          bool heap,
                                          p_complex *dest)
{
  u64 offset = 0;
  p_complex present = NULL;
  p_complex tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PCOMPLEX_")) {
    err_puts("marshall_read_pcomplex: marshall_read_1 magic");
    assert(! "marshall_read_pcomplex: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcomplex_init_copy(dest, &present)) {
      err_puts("marshall_read_pcomplex: pcomplex_init_copy");
      assert(! "marshall_read_pcomplex: pcomplex_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_complex))))
    return NULL;
  if (! marshall_read_complex(mr, true, tmp) ||
      ! marshall_read_ht_add(mr, offset, tmp)) {
    complex_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pcow  (s_marshall_read *mr,
                                       bool heap,
                                       p_cow *dest)
{
  (void) mr;
  (void) heap;
  (void) dest;
  return NULL;
  // return marshall_read_cow(mr, heap, *dest);
}

s_marshall_read * marshall_read_pfacts (s_marshall_read *mr,
                                        bool heap,
                                        p_facts *dest)
{
  p_facts tmp;
  p_facts present = NULL;
  u64 offset = 0;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PFACTS_")) {
    err_puts("marshall_read_pfacts: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    assert(! "marshall_read_pfacts: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present)) {
    err_puts("marshall_read_pfacts: marshall_read_heap_pointer");
    assert(! "marshall_read_pfacts: marshall_read_heap_pointer");
    return NULL;
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pfacts_init_copy(dest, &present)) {
      err_puts("marshall_read_pfacts: pfacts_init_copy");
      assert(! "marshall_read_pfacts: pfacts_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_pfacts: buf_seek");
    assert(! "marshall_read_pfacts: buf_seek");
    return NULL;
  }
  if (! (tmp = facts_new())) {
    err_puts("marshall_read_pfacts: facts_new");
    assert(! "marshall_read_pfacts: facts_new");
    return NULL;
  }
  if (! marshall_read_facts(mr, true, tmp)) {
    err_puts("marshall_read_pfacts: marshall_read_facts");
    assert(! "marshall_read_pfacts: marshall_read_facts");
    facts_delete(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    err_puts("marshall_read_pfacts: marshall_read_ht_add");
    assert(! "marshall_read_pfacts: marshall_read_ht_add");
    facts_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pframe (s_marshall_read *mr,
                                        bool heap,
                                        p_frame *dest)
{
  u64 offset = 0;
  p_frame present = NULL;
  p_frame tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PFRAME_")) {
    err_puts("marshall_read_pframe: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    assert(! "marshall_read_pframe: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pframe_init_copy(dest, &present)) {
      err_puts("marshall_read_pframe: pframe_init_copy");
      assert(! "marshall_read_pframe: pframe_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_frame))))
    return NULL;
  if (! marshall_read_frame(mr, true, tmp)) {
    alloc_free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    frame_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_plist (s_marshall_read *mr,
                                       bool heap,
                                       p_list *dest)
{
  u64 offset = 0;
  p_list present = NULL;
  p_list tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PLIST_")) {
    err_puts("marshall_read_plist: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    assert(! "marshall_read_plist: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! plist_init_copy(dest, &present)) {
      err_puts("marshall_read_plist: plist_init_copy");
      assert(! "marshall_read_plist: plist_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_list))))
    return NULL;
  if (! marshall_read_list(mr, true, tmp)) {
    alloc_free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    list_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pointer (s_marshall_read *mr,
                                         bool heap,
                                         s_pointer *dest)
{
  s_call call = {0};
  s_env *env = NULL;
  s_ident ident = {0};
  s_tag tag;
  s_tag tag_1;
  p_sym target_type;
  u64 offset = 0;
  void *present = NULL;
  s_pointer tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3POINTER_")) {
    err_puts("marshall_read_pointer: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    assert(! "marshall_read_pointer: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_psym(mr, heap, &target_type)) {
    err_puts("marshall_read_pointer: marshall_read_psym");
    assert(! "marshall_read_pointer: marshall_read_psym");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present)) {
    err_puts("marshall_read_pointer: marshall_read_heap_pointer");
    assert(! "marshall_read_pointer: marshall_read_heap_pointer");
    return NULL;
  }
  if (offset && ! present) {
    env = env_global();
    ident.module = target_type;
    ident.sym = &g_sym_marshall_read;
    if (! env_ident_get(env, &ident, &tag) ||
        tag.type != TAG_PCALLABLE) {
      err_write_1("marshall_read_pointer: Callable not found: ");
      err_inspect_ident(&ident);
      err_write_1("\n");
      return NULL;
    }
    call_init(&call);
    call.ident = ident;
    call.arguments = list_new_pointer
      (NULL, &g_sym_MarshallRead, mr, list_new_bool
       (heap, NULL));
    if (! env_eval_call(env, &call, &tag_1)) {
      err_write_1("marshall_read_pointer: ");
      err_inspect_call(&call);
      err_write_1("\n");
      tag_clean(&tag);
      call_clean(&call);
      return NULL;
    }
    if (tag_1.type != TAG_POINTER ||
        tag_1.data.pointer.target_type != target_type) {
      err_write_1("marshall_read_pointer: ");
      err_inspect_ident(&ident);
      err_write_1("\n");
      tag_clean(&tag_1);
      tag_clean(&tag);
      call_clean(&call);
      return NULL;
    }
    present = tag_1.data.pointer.ptr.p;
    tag_clean(&tag_1);
    tag_clean(&tag);
    call_clean(&call);
  }
  if (! pointer_init(&tmp, NULL, target_type, present)) {
    err_puts("marshall_read_pointer: pointer_init");
    assert(! "marshall_read_pointer: pointer_init");
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pstruct (s_marshall_read *mr,
                                         bool heap,
                                         p_struct *dest)
{
  u64 offset = 0;
  p_struct present = NULL;
  p_struct tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PSTRUCT_")) {
    err_puts("marshall_read_pstruct: marshall_read_1 magic");
    assert(! "marshall_read_pstruct: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pstruct_init_copy(dest, &present)) {
      err_puts("marshall_read_pstruct: pstruct_init_copy");
      assert(! "marshall_read_pstruct: pstruct_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_struct))))
    return NULL;
  if (! marshall_read_struct(mr, true, tmp)) {
    alloc_free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    struct_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

// TODO: use sym_must_clean
s_marshall_read * marshall_read_pstruct_type (s_marshall_read *mr,
                                              bool heap,
                                              p_struct_type *dest)
{
  u64 offset = 0;
  p_struct_type present = NULL;
  p_struct_type tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PSTRUCTTYPE_")) {
    err_puts("marshall_read_pstruct_type: marshall_read_1 magic");
    assert(! "marshall_read_pstruct_type: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    pstruct_type_init_copy(dest, &present);
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_struct_type))))
    return NULL;
  if (! marshall_read_struct_type(mr, true, tmp)) {
    pstruct_type_clean(&tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    pstruct_type_clean(&tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_psym (s_marshall_read *mr,
                                      bool heap,
                                      p_sym *dest)
{
  u64 offset = 0;
  void *present = NULL;
  p_sym tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PSYM_")) {
    err_puts("marshall_read_psym: marshall_read_1 magic");
    assert(! "marshall_read_psym: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (present || ! offset) {
    *dest = present;
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_psym: buf_seek failed");
    return NULL;
  }
  if (! marshall_read_sym(mr, true, &tmp)) {
    err_puts("marshall_read_psym: marshall_read_sym failed");
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    err_puts("marshall_read_psym: marshall_read_ht_add failed");
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_ptag (s_marshall_read *mr,
                                      bool heap,
                                      p_tag *dest)
{
  u64 offset = 0;
  p_tag present = NULL;
  p_tag tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PTAG_")) {
    err_puts("marshall_read_ptag: marshall_read_1 magic");
    assert(! "marshall_read_ptag: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (present || ! offset) {
    *dest = present;
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_ptag: buf_seek");
    assert(! "marshall_read_ptag: buf_seek");
    return NULL;
  }
  if (! (tmp = alloc(sizeof(s_tag))))
    return NULL;
  if (! marshall_read_tag(mr, true, tmp)) {
    err_puts("marshall_read_ptag: marshall_read_tag");
    assert(! "marshall_read_ptag: marshall_read_tag");
    alloc_free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    tag_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_ptr (s_marshall_read *mr,
                                     bool heap,
                                     u_ptr_w *dest)
{
  assert(mr);
  assert(dest);
  u_ptr_w tmp = {0};
  if (! marshall_read_1(mr, heap, "_KC3PTR_")) {
    err_puts("marshall_read_ptr: marshall_read_1 magic");
    assert(! "marshall_read_ptr: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &tmp.uw))
    return NULL;
  if (tmp.uw)
    return NULL;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_ptr_free (s_marshall_read *mr,
                                          bool heap,
                                          u_ptr_w *dest)
{
  assert(mr);
  assert(dest);
  u_ptr_w tmp = {0};
  if (! marshall_read_1(mr, heap, "_KC3PTRFREE_")) {
    err_puts("marshall_read_ptr_free: marshall_read_1 magic");
    assert(! "marshall_read_ptr_free: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_uw(mr, heap, &tmp.uw))
    return NULL;
  if (tmp.uw)
    return NULL;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_pvar (s_marshall_read *mr,
                                      bool heap,
                                      p_var *dest)
{
  u64 offset = 0;
  p_var present = NULL;
  p_var tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PVAR_")) {
    err_puts("marshall_read_pvar: marshall_read_1 magic");
    assert(! "marshall_read_pvar: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pvar_init_copy(dest, &present)) {
      err_puts("marshall_read_pvar: pvar_init_copy");
      assert(! "marshall_read_pvar: pvar_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_var))))
    return NULL;
  if (! marshall_read_var(mr, true, tmp) ||
      ! marshall_read_ht_add(mr, offset, tmp)) {
    var_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_quote (s_marshall_read *mr,
                                      bool heap,
                                      s_quote *dest)
{
  s_quote tmp = {0};
  assert(mr);
  assert(mr);
  if (! marshall_read_1(mr, heap, "_KC3QUOTE_")) {
    err_puts("marshall_read_quote: marshall_read_1 magic");
    assert(! "marshall_read_quote: marshall_read_1 magic");
    return NULL;
  }
  if (! (tmp.tag = alloc(sizeof(s_tag))))
    return NULL;
  if (! marshall_read_tag(mr, heap, tmp.tag)) {
    alloc_free(tmp.tag);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_ratio (s_marshall_read *mr,
                                      bool heap,
                                      s_ratio *dest)
{
  s_ratio tmp = {0};
  if (! mr || ! dest) {
    err_puts("marshall_read_ratio: invalid argument");
    assert(! "marshall_read_ratio: invalid argument");
    return NULL;
  }
  if (! marshall_read_1(mr, heap, "_KC3RATIO_")) {
    err_puts("marshall_read_ratio: marshall_read_1 magic");
    assert(! "marshall_read_ratio: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_integer(mr, heap, &tmp.numerator) ||
      ! marshall_read_integer(mr, heap, &tmp.denominator))
    return NULL;
  *dest = tmp;
  return mr;
}

DEF_MARSHALL_READ(s8, "_KC3S8_", s8)
DEF_MARSHALL_READ_LETOH(s16, "_KC3S16_", s16, 16)
DEF_MARSHALL_READ_LETOH(s32, "_KC3S32_", s32, 32)
DEF_MARSHALL_READ_LETOH(s64, "_KC3S64_", s64, 64)

sw marshall_read_size (const s_marshall_read *mr)
{
  return sizeof(s_marshall_header) + mr->heap_size + mr->buf_size;
}

s_marshall_read * marshall_read_str (s_marshall_read *mr,
                                     bool heap, s_str *dest)
{
  s_buf *buf = {0};
  u32 size = 0;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3STR_")) {
    err_puts("marshall_read_str: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    assert(! "marshall_read_str: marshall_read_1 magic");
    return NULL;
  }
  buf = heap ? mr->heap : mr->buf;
  if (! marshall_read_u32(mr, heap, &size))
    return NULL;
  if (! buf_read(buf, size, dest))
    return NULL;
  return mr;
}

s_marshall_read * marshall_read_struct (s_marshall_read *mr,
                                        bool heap,
                                        s_struct *dest)
{
  bool has_tags;
  bool has_data;
  uw i;
  uw offset;
  s_struct tmp = {0};
  p_sym type = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3STRUCT_")) {
    err_puts("marshall_read_struct: marshall_read_1 magic");
    assert(! "marshall_read_struct: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_pstruct_type(mr, heap, &tmp.pstruct_type))
    return NULL;
  if (! marshall_read_bool(mr, heap, &has_tags)) {
    err_puts("marshall_read_struct: marshall_read_bool has tags");
    assert(! "marshall_read_struct: marshall_read_bool has tags");    
    return NULL;
  }
  if (has_tags) {
    if (! (tmp.tag = alloc(tmp.pstruct_type->map.count *
                           sizeof(s_tag))))
      return NULL;
    i = 0;
    while (i < tmp.pstruct_type->map.count) {
      if (! marshall_read_tag(mr, heap, tmp.tag + i))
        goto ko;
      i++;
    }
  }
  if (! marshall_read_bool(mr, heap, &has_data)) {
    err_puts("marshall_read_struct: marshall_read_bool has data");
    assert(! "marshall_read_struct: marshall_read_bool has data");    
    goto ko;
  }
  if (has_data) {
    if (! (tmp.data = alloc(tmp.pstruct_type->size)))
      goto ko;
    tmp.free_data = true;
    i = 0;
    while (i < tmp.pstruct_type->map.count) {
      offset = tmp.pstruct_type->offset[i];
      if (! tag_type_var(tmp.pstruct_type->map.value + i, &type) ||
          ! type)
        goto ko;
      if (! marshall_read_data(mr, heap, type, (u8 *) tmp.data + offset))
        goto ko;
      i++;
    }
  }
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  if (! mutex_init(&dest->mutex)) {
    struct_clean(dest);
    return NULL;
  }
#endif
  return mr;
ko:
  struct_clean(&tmp);
  return NULL;
}

s_marshall_read * marshall_read_struct_type (s_marshall_read *mr,
                                             bool heap,
                                             s_struct_type *dest)
{
  s_struct_type tmp = {0};
  assert(mr);
  assert(mr);
  if (! marshall_read_1(mr, heap, "_KC3STRUCTTYPE_")) {
    err_puts("marshall_read_str: marshall_read_1 magic");
    assert(! "marshall_read_str: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_psym(mr, heap, &tmp.module) ||
      ! marshall_read_map(mr, heap, &tmp.map))
    return NULL;
  if (! marshall_read_pcallable(mr, heap, &tmp.clean))
    return NULL;
  tmp.ref_count = 1;
  if (tmp.map.count)
    struct_type_update_map(&tmp);
  *dest = tmp;
#if HAVE_PTHREAD
  if (! mutex_init(&dest->mutex))
    return NULL;
#endif
  return mr;
}

s_marshall_read * marshall_read_sw (s_marshall_read *mr,
                                    bool heap,
                                    sw *dest)
{
  s64 s;
  if (! marshall_read_1(mr, heap, "_KC3SW_")) {
    err_puts("marshall_read_sw: marshall_read_1 magic");
    assert(! "marshall_read_sw: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_s64(mr, heap, &s)) {
    err_puts("marshall_read_sw: marshall_read_s64");
    assert(! "marshall_read_sw: marshall_read_s64");
    return NULL;
  }
  if (s < SW_MIN || s > SW_MAX) {
    err_puts("marshall_read_sw: value out of range");
    assert(! "marshall_read_sw: value out of range");
    return NULL;
  }
  *dest = s;
  return mr;
}

s_marshall_read * marshall_read_sym (s_marshall_read *mr,
                                     bool heap, p_sym *dest)
{
  s_str tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3SYM_")) {
    err_puts("marshall_read_sym: marshall_read_1 magic");
    assert(! "marshall_read_sym: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_str(mr, heap, &tmp))
    return NULL;
  if (! (*dest = str_to_sym(&tmp))) {
    str_clean(&tmp);
    return NULL;
  }
  str_clean(&tmp);
  return mr;
}

s_marshall_read * marshall_read_tag (s_marshall_read *mr, bool heap,
                                     s_tag *dest)
{
  u8 type = 0;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3TAG_")) {
    err_puts("marshall_read_tag: marshall_read_1 magic");
    assert(! "marshall_read_tag: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_u8(mr, heap, &type)) {
    err_puts("marshall_read_tag: marshall_read_u8 type");
    assert(! "marshall_read_tag: marshall_read_u8 type");
    return NULL;
  }
  dest->type = type;
  switch (dest->type) {
  case TAG_VOID:
    return mr;
  case TAG_ARRAY:
    return marshall_read_array(mr, heap, &dest->data.array);
  case TAG_DO_BLOCK:
    return marshall_read_do_block(mr, heap, &dest->data.do_block);
  case TAG_BOOL:
    return marshall_read_bool(mr, heap, &dest->data.bool_);
  case TAG_CHARACTER:
    return marshall_read_character(mr, heap, &dest->data.character);
  case TAG_F32:
    return marshall_read_f32(mr, heap, &dest->data.f32);
  case TAG_F64:
    return marshall_read_f64(mr, heap, &dest->data.f64);
#if HAVE_F80
  case TAG_F80:
    return marshall_read_f80(mr, heap, &dest->data.f80);
#endif
#if HAVE_F128
  case TAG_F128:
    return marshall_read_f128(mr, heap, &dest->data.f128);
#endif
  case TAG_FACT:
    return marshall_read_fact(mr, heap, &dest->data.fact);
  case TAG_IDENT:
    return marshall_read_ident(mr, heap, &dest->data.ident);
  case TAG_INTEGER:
    return marshall_read_integer(mr, heap, &dest->data.integer);
  case TAG_MAP:
    return marshall_read_map(mr, heap, &dest->data.map);
  case TAG_PCALL:
    return marshall_read_pcall(mr, heap, &dest->data.pcall);
  case TAG_PCALLABLE:
    return marshall_read_pcallable(mr, heap, &dest->data.pcallable);
  case TAG_PCOMPLEX:
    return marshall_read_pcomplex(mr, heap, &dest->data.pcomplex);
  case TAG_PCOW:
    return marshall_read_pcow(mr, heap, &dest->data.pcow);
  case TAG_PFACTS:
    return marshall_read_pfacts(mr, heap, &dest->data.pfacts);
  case TAG_PLIST:
    return marshall_read_plist(mr, heap, &dest->data.plist);
  case TAG_POINTER:
    return marshall_read_pointer(mr, heap, &dest->data.pointer);
  case TAG_PSTRUCT:
    return marshall_read_pstruct(mr, heap, &dest->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return marshall_read_pstruct_type(mr, heap,
                                      &dest->data.pstruct_type);
  case TAG_PSYM:
    return marshall_read_psym(mr, heap, &dest->data.psym);
  case TAG_PTR:
    return marshall_read_ptr(mr, heap, &dest->data.ptr);
  case TAG_PTR_FREE:
    return marshall_read_ptr_free(mr, heap, &dest->data.ptr_free);
  case TAG_QUOTE:
    return marshall_read_quote(mr, heap, &dest->data.quote);
  case TAG_RATIO:
    return marshall_read_ratio(mr, heap, &dest->data.ratio);
  case TAG_STR:
    return marshall_read_str(mr, heap, &dest->data.str);
  case TAG_S8:
    return marshall_read_s8(mr, heap, &dest->data.s8);
  case TAG_S16:
    return marshall_read_s16(mr, heap, &dest->data.s16);
  case TAG_S32:
    return marshall_read_s32(mr, heap, &dest->data.s32);
  case TAG_S64:
    return marshall_read_s64(mr, heap, &dest->data.s64);
  case TAG_SW:
    return marshall_read_sw(mr, heap, &dest->data.sw);
  case TAG_TIME:
    return marshall_read_time(mr, heap, &dest->data.time);
  case TAG_PTUPLE:
    return marshall_read_ptuple(mr, heap, &dest->data.ptuple);
  case TAG_U8:
    return marshall_read_u8(mr, heap, &dest->data.u8);
  case TAG_U16:
    return marshall_read_u16(mr, heap, &dest->data.u16);
  case TAG_U32:
    return marshall_read_u32(mr, heap, &dest->data.u32);
  case TAG_U64:
    return marshall_read_u64(mr, heap, &dest->data.u64);
  case TAG_UNQUOTE:
    return marshall_read_unquote(mr, heap, &dest->data.unquote);
  case TAG_UW:
    return marshall_read_uw(mr, heap, &dest->data.uw);
  case TAG_PVAR:
    return marshall_read_pvar(mr, heap, &dest->data.pvar);
  }
  err_write_1("marshall_read_tag: unknown tag type: ");
  err_inspect_u8_decimal(type);
  err_write_1("\n");
  assert(! "marshall_read_tag: unknown tag type");
  return NULL;
}

s_marshall_read * marshall_read_time (s_marshall_read *mr,
                                      bool heap,
                                      s_time *dest)
{
  bool has_tags;
  s_time tmp = {0};
  assert(mr);
  assert(mr);
  if (! marshall_read_bool(mr, heap, &has_tags))
    return NULL;
  if (has_tags) {
    if (! (tmp.tag = alloc(2 * sizeof(s_tag))))
      return NULL;
    if (! marshall_read_tag(mr, heap, tmp.tag))
      goto ko;
    if (! marshall_read_tag(mr, heap, tmp.tag + 1))
      goto ko;
  }
  else if (! marshall_read_sw(mr, heap, &tmp.tv_sec) ||
           ! marshall_read_sw(mr, heap, &tmp.tv_nsec))
    goto ko;
  *dest = tmp;
  return mr;
 ko:
  time_clean(&tmp);
  return NULL;
}

s_marshall_read * marshall_read_tuple (s_marshall_read *mr,
                                      bool heap,
                                      s_tuple *dest)
{
    uw count;
    uw i = 0;
    assert(mr);
    assert(dest);
    if (! marshall_read_uw(mr, heap, &count))
      return NULL;
    if (! tuple_init(dest, count))
      return NULL;
    while (i < count) {
      if (! marshall_read_tag(mr, heap, dest->tag + i)) {
        err_puts("marshall_read_tuple: read_tag error");
        assert(! "marshall_read_tuple: read_tag error");
        tuple_clean(dest);
        return NULL;
      }
      i++;
    }
    return mr;
}

s_marshall_read * marshall_read_ptuple (s_marshall_read *mr,
                                        bool heap,
                                        p_tuple *dest)
{
  s_buf *buf;
  u64 offset = 0;
  p_tuple present = NULL;
  p_tuple tuple = NULL;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3PTUPLE_") > 0) {
    if (! marshall_read_heap_pointer(mr, heap, &offset,
                                     (void **) &present))
      return NULL;
    if (! offset) {
      *dest = NULL;
      return mr;
    }
    if (present) {
      *dest = tuple_new_ref(present);
      return mr;
    }
    if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
        ! (tuple = alloc(sizeof(s_tuple))))
      return NULL;
    if (! marshall_read_tuple(mr, true, tuple)) {
      alloc_free(tuple);
      return NULL;
    }
    if (! marshall_read_ht_add(mr, offset, tuple)) {
      tuple_delete(tuple);
      return NULL;
    }
    *dest = tuple;
    return mr;
  }
  tuple = alloc(sizeof(s_tuple));
  if (! tuple)
    return NULL;
  if (! marshall_read_tuple(mr, heap, tuple)) {
    alloc_free(tuple);
    return NULL;
  }
  *dest = tuple;
  return mr;
}

s_marshall_read * marshall_read_unquote (s_marshall_read *mr,
                                         bool heap,
                                         s_unquote *dest)
{
    s_unquote tmp = {0};
    assert(mr);
    assert(mr);
    if (! marshall_read_tag(mr, heap, tmp.tag))
      return NULL;
    *dest = tmp;
    return mr;
}

DEF_MARSHALL_READ(u8, "_KC3U8_", u8)
DEF_MARSHALL_READ_LETOH(u16, "_KC3U16_", u16, 16)
DEF_MARSHALL_READ_LETOH(u32, "_KC3U32_", u32, 32)
DEF_MARSHALL_READ_LETOH(u64, "_KC3U64_", u64, 64)

s_marshall_read * marshall_read_uw (s_marshall_read *mr,
                                    bool heap,
                                    uw *dest)
{
  u64 u;
  if (! marshall_read_1(mr, heap, "_KC3UW_")) {
    err_puts("marshall_read_uw: marshall_read_1 magic");
    assert(! "marshall_read_uw: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_u64(mr, heap, &u)) {
    err_puts("marshall_read_uw: marshall_read_u64");
    assert(! "marshall_read_uw: marshall_read_u64");
    return NULL;
  }
  if (u > UW_MAX) {
    err_puts("marshall_read_uw: value out of range");
    assert(! "marshall_read_uw: value out of range");
    return NULL;
  }
  *dest = u;
  return mr;
}

s_marshall_read * marshall_read_var (s_marshall_read *mr,
                                     bool heap,
                                     s_var *dest)
{
  s_var tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3VAR_") ||
      ! marshall_read_ident(mr, heap, &tmp.name) ||
      ! marshall_read_psym(mr, heap, &tmp.type) ||
      ! marshall_read_bool(mr, heap, &tmp.bound) ||
      (tmp.bound &&
       ! marshall_read_tag(mr, heap, &tmp.tag)))
    return NULL;
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  if (! mutex_init(&dest->mutex)) {
    var_clean(dest);
    return NULL;
  }
#endif
  return mr;
}
