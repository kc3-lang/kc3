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
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "buf.h"
#include "call.h"
#include "callable.h"
#include "compare.h"
#include "complex.h"
#include "do_block.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ht.h"
#include "list.h"
#include "map.h"
#include "marshall.h"
#include "marshall_read.h"
#include "mutex.h"
#include "pcallable.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "rwlock.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "time.h"
#include "var.h"

#define DEF_MARSHALL_READ(name, type)                                  \
  s_marshall_read * marshall_read_ ## name (s_marshall_read *mr,       \
                                            bool heap,                 \
                                            type *dest)                \
  {                                                                    \
    s_buf *buf = {0};                                                  \
    assert(mr);                                                        \
    assert(dest);                                                      \
    buf = heap ? &mr->heap : &mr->buf;                                 \
    if (buf_read_ ## name (buf, dest) <= 0) {                          \
      err_puts("marshall_read_" # name ": buf_read_" # name);          \
      assert(! "marshall_read_" # name ": buf_read_" # name);          \
      return NULL;                                                     \
    }                                                                  \
    return mr;                                                         \
  }

static s8 marshall_read_ht_compare (const s_tag *a, const s_tag *b);
static uw marshall_read_ht_hash (const s_tag *tag);

DEF_MARSHALL_READ(bool, bool)

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
  if (tmp.dimension_count) {
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
        if (! marshall_read_data(mr, heap, tmp.array_type, data))
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
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_array_data (s_marshall_read *mr,
                                            bool heap,
                                            s_array *dest)
{
  for (u8 i = 0; i <= dest->count; i++) {
    if (marshall_read_uw(mr, heap, &((uw *)dest->data)[i]))
      return NULL;
  }
  return mr;
}

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
    goto ok;
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
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
  *dest = tmp;
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
    if (! (*tail = list_new_psym(psym, NULL))) {
      err_puts("marshall_read_cfn: list_new_psym");
      assert(! "marshall_read_cfn: list_new_psym");
      return NULL;
    }
    tail = &(*tail)->next.data.plist;
    i++;
  }
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
  tmp.ready = true;
#endif
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_character (s_marshall_read *mr,
                                           bool heap,
                                           character *dest)
{
  s_buf *buf = {0};
  assert(mr);
  assert(dest);
  buf = heap ? &mr->heap : &mr->buf;
  return buf_read_character_utf8(buf, dest) < 0 ? NULL : mr;
}

void marshall_read_clean (s_marshall_read *mr)
{
  assert(mr);
  buf_clean(&mr->buf);
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
  if (! mutex_init(&tmp.mutex)) {
    list_clean(tmp.list);
    return NULL;
  }
  tmp.ref_count = 1;
  *dest = tmp;
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
    return marshall_read_call(mr, heap, data);
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
  if (type == &g_sym_F128)
    return marshall_read_f128(mr, heap, data);
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
  if (type == &g_sym_Ptag)
    return marshall_read_ptag(mr, heap, data);
  if (type == &g_sym_Ptr)
    return marshall_read_ptr(mr, heap, data);
  if (type == &g_sym_PtrFree)
    return marshall_read_ptr_free(mr, heap, data);
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
  free(mr);
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

DEF_MARSHALL_READ(f32, f32)
DEF_MARSHALL_READ(f64, f64)
DEF_MARSHALL_READ(f128, f128)

s_marshall_read * marshall_read_fact (s_marshall_read *mr,
                                      bool heap,
                                      s_fact *dest)
{
  s_fact tmp = {0};
  assert(mr);
  assert(mr);
  if (! marshall_read_ptag(mr, heap, &tmp.subject))
    return NULL;
  if (! marshall_read_ptag(mr, heap, &tmp.predicate)) {
    tag_clean(tmp.subject);
    return NULL;
  }
  if (! marshall_read_ptag(mr, heap, &tmp.object)       ||
      ! marshall_read_uw(mr, heap, &tmp.id)) {
    tag_clean(tmp.subject);
    tag_clean(tmp.predicate);
    return NULL;
  }
  *dest = tmp;
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
  if (! marshall_read_bool(mr, heap, &tmp.macro) ||
      ! marshall_read_bool(mr, heap, &tmp.special_operator) ||
      ! marshall_read_ident(mr, heap, &tmp.name) ||
      ! marshall_read_psym(mr, heap, &tmp.module) ||
      ! marshall_read_uw(mr, heap, &clause_count))
    return NULL;
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
  if (! marshall_read_uw(mr, heap, &binding_count))
    return NULL;
  binding = &tmp.bindings;
  i = 0;
  while (i < binding_count) {
    if (! (*binding = alloc(sizeof(s_binding))) ||
        ! marshall_read_psym(mr, heap, &(*binding)->name) ||
        ! marshall_read_tag(mr, heap, &(*binding)->value))
      goto ko;
    binding = &(*binding)->next;
    i++;
  }
  *dest = tmp;
  return mr;
 ko:
  frame_clean(&tmp);
  return NULL;
}

s_marshall_read * marshall_read_header (s_marshall_read *mr)
{
  s_marshall_header mh = {0};
  s_str str = {0};
  s_marshall_read tmp;
  assert(mr);
  tmp = *mr;
  if (! buf_read(&tmp.buf, sizeof(s_marshall_header), &str)) {
    err_puts("marshall_read_header: buf_read");
    assert(! "marshall_read_header: buf_read");
    return NULL;
  }
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
  tmp.heap_count = le64toh(mh.le_heap_count);
  tmp.heap_size = le64toh(mh.le_heap_size);
  tmp.buf_size = le64toh(mh.le_buf_size);
  if (tmp.heap_size && ! tmp.heap_count) {
    err_puts("marshall_read_header: invalid heap count");
    assert(! "marshall_read_header: invalid heap count");
    return NULL;
  }
  if (tmp.heap_count &&
      ! ht_init(&tmp.ht, &g_sym_Tuple, tmp.heap_count)) {
    err_puts("marshall_read_header: ht_init");
    assert(! "marshall_read_header: ht_init");
    return NULL;
  }
  tmp.ht.compare = marshall_read_ht_compare;
  tmp.ht.hash = marshall_read_ht_hash;
  *mr = tmp;
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
  s_tag key = {0};
  s_tag tag = {0};
  if (! marshall_read_u64(mr, heap, offset))
    return NULL;
  if (! *offset) {
    *present = NULL;
    return mr;
  }
  if (! tag_init_tuple(&key, 2))
    return NULL;
  tag_init_u64(key.data.tuple.tag, *offset);
  if (! ht_get(&mr->ht, &key, &tag)) {
    *present = NULL;
    return mr;
  }
  if (tag.type != TAG_TUPLE ||
      tag.data.tuple.tag[1].type != TAG_UW) {
    err_puts("marshall_read_heap_pointer: invalid tag in ht");
    assert(! "marshall_read_heap_pointer: invalid tag in ht");
  }
  *present = (void *)tag.data.tuple.tag[1].data.uw;
  return mr;
}

s_marshall_read * marshall_read_ht_add (s_marshall_read *mr,
                                        u64 offset, const void *p)
{
  s_tag tag = {0};
  if (! tag_init_tuple(&tag, 2))
    return NULL;
  tag_init_u64(tag.data.tuple.tag, offset);
  tag_init_uw(tag.data.tuple.tag, (uw) p);
  if (! ht_add(&mr->ht, &tag)) {
    tag_clean(&tag);
    return NULL;
  }
  return mr;
}

s8 marshall_read_ht_compare (const s_tag *a, const s_tag *b)
{
  assert(a);
  assert(b);
  assert(a->type == TAG_TUPLE);
  assert(b->type == TAG_TUPLE);
  if (a->type < b->type)
    return -1;
  if (a->type > b->type)
    return 1;
  if (a->type != TAG_TUPLE)
    return COMPARE_ERROR;
  return compare_tag(a->data.tuple.tag, b->data.tuple.tag);
}

uw  marshall_read_ht_hash (const s_tag *tag)
{
  t_hash h;
  assert(tag);
  assert(tag->type == TAG_TUPLE);
  if (tag->type != TAG_TUPLE)
    abort();
  hash_init(&h);
  hash_update_tag(&h, tag->data.tuple.tag);
  return hash_to_uw(&h);
}

s_marshall_read * marshall_read_ident (s_marshall_read *mr, bool heap,
                                       s_ident *dest)
{
  s_ident tmp_id = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_psym(mr, heap, &tmp_id.module) ||
      ! marshall_read_psym(mr, heap, &tmp_id.sym))
    return NULL;
  *dest = tmp_id;
  return mr;
}

s_marshall_read * marshall_read_init (s_marshall_read *mr)
{
  s_marshall_read tmp = {0};
  assert(mr);
  if (! buf_init_alloc(&tmp.heap, BUF_SIZE)) {
    err_puts("marshall_read_init: heap allocation error");
    assert(! "marshall_read_init: heap allocation error");
    return NULL;
  }
  if (! buf_init_alloc(&tmp.buf, BUF_SIZE)) {
    err_puts("marshall_read_init: buffer allocation error");
    assert(! "marshall_read_init: buffer allocation error");
    buf_clean(&tmp.heap);
    return NULL;
  }
  *mr = tmp;
  return mr;
}

s_marshall_read * marshall_read_init_1 (s_marshall_read *mr,
                                        const char *p, uw size)
{
  s_str str = {0};
  assert(mr);
  if (! p || ! size)
    return NULL;
  str.size = size;
  str.ptr.pchar = p;
  return marshall_read_init_str(mr, &str);
}

s_marshall_read * marshall_read_init_str (s_marshall_read *mr,
                                          const s_str *dest)
{
  s_marshall_read tmp = {0};
  assert(mr);
  if (! buf_init_str_copy(&tmp.buf, dest)) {
    err_puts("marshall_read_init_str: buf_init_str_copy");
    assert(! "marshall_read_init_str: buf_init_str_copy");
    return NULL;
  }
  if (! marshall_read_header(&tmp)) {
    err_puts("marshall_read_init_str: marshall_read_header");
    assert(! "marshall_read_init_str: marshall_read_header");
    buf_clean(&tmp.buf);
    return NULL;
  }
  tmp.buf.rpos = sizeof(s_marshall_header) + tmp.heap_size;
  if (tmp.buf.rpos + tmp.buf_size != tmp.buf.wpos) {
    err_puts("marshall_read_init_str: invalid buffer size");
    assert(! "marshall_read_init_str: invalid buffer size");
    buf_clean(&tmp.buf);
    return NULL;
  }
  tmp.heap = tmp.buf;
  tmp.heap.free = false;
  tmp.heap.rpos = sizeof(s_marshall_header);
  tmp.heap.wpos = tmp.heap.rpos + tmp.heap_size;
#ifdef HAVE_PTHREAD
  rwlock_init(&tmp.heap.rwlock);
#endif
  *mr = tmp;
  return mr;
}

DEF_MARSHALL_READ(integer, s_integer)

s_marshall_read * marshall_read_list (s_marshall_read *mr, bool heap,
                                      s_list *dest)
{
  s_list tmp = {0};
  assert(mr);
  assert(dest);
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
    free(mr);
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
    free(mr);
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_pcallable (s_marshall_read *mr,
                                           bool heap,
                                           p_callable *dest)
{
  u64 offset = 0;
  void *present = NULL;
  p_callable tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present)) {
    err_puts("marshall_read_pcallable: marshall_read_heap_pointer");
    assert(! "marshall_read_pcallable: marshall_read_heap_pointer");
    return NULL;
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcallable_init_copy(dest, (p_callable *) &present)) {
      err_puts("marshall_read_pcallable: pcallable_init_copy");
      assert(! "marshall_read_pcallable: pcallable_init_copy");
      return NULL;
    }
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset) {
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
    err_puts("marshall_read_pcallable: read_callable");
    assert(! "marshall_read_pcallable: read_callable");
    free(tmp);
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
  void *present = NULL;
  p_complex tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = complex_new_ref(present);
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
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

s_marshall_read * marshall_read_pframe (s_marshall_read *mr,
                                       bool heap,
                                       p_frame *dest)
{
  u64 offset = 0;
  void *present = NULL;
  p_frame tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = frame_new_ref(present);
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
      ! (tmp = alloc(sizeof(s_frame))))
    return NULL;
  if (! marshall_read_frame(mr, true, tmp) ||
      ! marshall_read_ht_add(mr, offset, tmp)) {
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
  void *present = NULL;
  p_list tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = list_new_ref(present);
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
      ! (tmp = alloc(sizeof(s_list))))
    return NULL;
  if (! marshall_read_list(mr, true, tmp)) {
    free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    list_delete(tmp);
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
  void *present = NULL;
  p_struct tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = struct_new_ref(present);
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
      ! (tmp = alloc(sizeof(s_struct))))
    return NULL;
  if (! marshall_read_struct(mr, true, tmp)) {
    free(tmp);
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
  void *present = NULL;
  p_struct_type tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = struct_type_new_ref(present);
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
      ! (tmp = alloc(sizeof(s_struct_type))))
    return NULL;
  if (! marshall_read_struct_type(mr, true, tmp)) {
    free(tmp);
    return NULL;
  }
  if (! marshall_read_ht_add(mr, offset, tmp)) {
    struct_type_delete(tmp);
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
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (present || ! offset) {
    *dest = present;
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
      ! marshall_read_sym(mr, true, &tmp) ||
      ! marshall_read_ht_add(mr, offset, tmp))
    return NULL;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_ptag  (s_marshall_read *mr,
                                       bool heap,
                                       p_tag *dest)
{
  u64 offset = 0;
  void *present = NULL;
  p_tag tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (present || ! offset) {
    *dest = present;
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
      ! (tmp = alloc(sizeof(s_tag))))
    return NULL;
  if (! marshall_read_tag(mr, true, tmp)) {
    free(tmp);
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
  void *present = NULL;
  p_var tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = var_new_ref(present);
    return mr;
  }
  if (buf_seek(&mr->heap, (s64) offset, SEEK_SET) != (s64) offset ||
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
    if (! marshall_read_tag(mr, heap, tmp.tag))
      return NULL;
    *dest = tmp;
    return mr;
}

s_marshall_read * marshall_read_ratio (s_marshall_read *mr,
                                      bool heap,
                                      s_ratio *dest)
{
    s_ratio tmp = {0};
    assert(mr);
    assert(mr);
    if (! marshall_read_integer(mr, heap, &tmp.numerator) ||
        ! marshall_read_integer(mr, heap, &tmp.denominator))
      return NULL;
    *dest = tmp;
    return mr;
}

DEF_MARSHALL_READ(s8, s8)
DEF_MARSHALL_READ(s16, s16)
DEF_MARSHALL_READ(s32, s32)
DEF_MARSHALL_READ(s64, s64)

s_marshall_read * marshall_read_str (s_marshall_read *mr,
                                     bool heap, s_str *dest)
{
  s_buf *buf = {0};
  u32 size = 0;
  assert(mr);
  assert(dest);
  buf = heap ? &mr->heap : &mr->buf;
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
  if (! marshall_read_pstruct_type(mr, heap, &tmp.pstruct_type))
    return NULL;
  if (! marshall_read_bool(mr, heap, &has_tags))
    return NULL;
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
  if (! marshall_read_bool(mr, heap, &has_data))
    goto ko;
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
  if (! mutex_init(&tmp.mutex))
    goto ko;
  tmp.mutex_ready = true;
  *dest = tmp;
  return mr;
ko:
  struct_clean(&tmp);
  return NULL;
}

// TODO: convert f_clean to p_callable
s_marshall_read * marshall_read_struct_type (s_marshall_read *mr,
                                             bool heap,
                                             s_struct_type *dest)
{
  uw i;
  s_struct_type tmp = {0};
  assert(mr);
  assert(mr);
  if (! marshall_read_psym(mr, heap, &tmp.module) ||
      ! marshall_read_map(mr, heap, &tmp.map) ||
      ! (tmp.offset = alloc(tmp.map.count * sizeof(uw))))
    return NULL;
  i = 0;
  while (i < tmp.map.count) {
    if (! marshall_read_uw(mr, heap, tmp.offset + i))
      return NULL;
    i++;
  }
  if (! marshall_read_uw(mr, heap, &tmp.size) ||
      ! marshall_read_pcallable(mr, heap, &tmp.clean) ||
      ! marshall_read_bool(mr, heap, &tmp.must_clean) ||
      ! mutex_init(&tmp.mutex))
    return NULL;
  tmp.ref_count = 1;
  *dest = tmp;
  return mr;
}

DEF_MARSHALL_READ(sw, sw)

s_marshall_read * marshall_read_sym (s_marshall_read *mr,
                                     bool heap, p_sym *dest)
{
  s_str tmp = {0};
  assert(mr);
  assert(dest);
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
  marshall_read_u8(mr, heap, &type);
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
    case TAG_CALL:
      return marshall_read_call(mr, heap, &dest->data.call);
    case TAG_CHARACTER:
      return marshall_read_character(mr, heap, &dest->data.character);
    case TAG_F32:
      return marshall_read_f32(mr, heap, &dest->data.f32);
    case TAG_F64:
      return marshall_read_f64(mr, heap, &dest->data.f64);
    case TAG_F128:
      return marshall_read_f128(mr, heap, &dest->data.f128);
    case TAG_FACT:
      return marshall_read_fact(mr, heap, &dest->data.fact);
    case TAG_IDENT:
      return marshall_read_ident(mr, heap, &dest->data.ident);
    case TAG_INTEGER:
      return marshall_read_integer(mr, heap, &dest->data.integer);
    case TAG_MAP:
      return marshall_read_map(mr, heap, &dest->data.map);
    case TAG_PCALLABLE:
      return marshall_read_pcallable(mr, heap, &dest->data.pcallable);
    case TAG_PCOMPLEX:
      return marshall_read_pcomplex(mr, heap, &dest->data.pcomplex);
    case TAG_PCOW:
      return marshall_read_pcow(mr, heap, &dest->data.pcow);
    case TAG_PLIST:
      return marshall_read_plist(mr, heap, &dest->data.plist);
    case TAG_PSTRUCT:
      return marshall_read_pstruct(mr, heap, &dest->data.pstruct);
    case TAG_PSTRUCT_TYPE:
      return marshall_read_pstruct_type(mr, heap,
                                        &dest->data.pstruct_type);
    case TAG_PSYM:
      return marshall_read_psym(mr, heap, &dest->data.psym);
    case TAG_PTAG:
      return marshall_read_ptag(mr, heap, &dest->data.ptag);
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
    case TAG_TUPLE:
      return marshall_read_tuple(mr, heap, &dest->data.tuple);
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
    s_tuple tmp = {0};
    uw i = 0;
    assert(mr);
    assert(dest);
    if (! marshall_read_uw(mr, heap, &tmp.count))
      return NULL;
    tmp.tag = alloc(sizeof(s_tag) * tmp.count);
    if (! tmp.tag)
      return NULL;
    while (i < tmp.count) {
      if (! marshall_read_tag(mr, heap, &tmp.tag[i])) {
        err_puts("marshall_read_tuple: read_tag error");
        assert(! "marshall_read_tuple: read_tag error");
        free(tmp.tag);
        return NULL;
      }
      i++;
    }
    *dest = tmp;
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

DEF_MARSHALL_READ(u8,  u8)
DEF_MARSHALL_READ(u16, u16)
DEF_MARSHALL_READ(u32, u32)
DEF_MARSHALL_READ(u64, u64)
DEF_MARSHALL_READ(uw, uw)

s_marshall_read * marshall_read_var (s_marshall_read *mr,
                                     bool heap,
                                     s_var *dest)
{
  s_var tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_ident(mr, heap, &tmp.name) ||
      ! marshall_read_psym(mr, heap, &tmp.type) ||
      ! marshall_read_bool(mr, heap, &tmp.bound) ||
      (tmp.bound &&
       ! marshall_read_tag(mr, heap, &tmp.tag)))
    return NULL;
  tmp.ref_count = 1;
  if (! mutex_init(&tmp.mutex)) {
    var_clean(&tmp);
    return NULL;
  }
  tmp.ready = true;
  *dest = tmp;
  return mr;
}

// more complex types :

#define DEF_MARSHALL_READ_STUB(name, type)                             \
  PROTO_MARSHALL_READ(name, type)                                      \
  {                                                                    \
    (void) mr;                                                         \
    (void) heap;                                                       \
    (void) dest;                                                       \
    err_puts("marshall_read_" # name ": not implemented");             \
    assert(! "marshall_read_" # name ": not implemented");             \
    return NULL;                                                       \
  }

