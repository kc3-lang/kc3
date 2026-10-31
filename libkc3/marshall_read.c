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
#include <string.h>

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
#include "data.h"
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
#include "ptag.h"
#include "ptuple.h"
#include "pvar.h"
#include "ratio.h"
#include "rwlock.h"
#include "set__fact.h"
#include "set__tag.h"
#include "set_item__fact.h"
#include "set_item__tag.h"
#include "skiplist__fact.h"
#include "skiplist_node__fact.h"
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

#define DEF_MARSHALL_READ_PSET_ITEM(name, type, magic)                 \
  s_marshall_read * marshall_read_pset_item__ ## name                  \
  (s_marshall_read *mr, bool heap, p_set_item__ ## name *dest)         \
  {                                                                    \
    p_set_item__ ## name tmp;                                          \
    p_set_item__ ## name present = NULL;                               \
    u64 offset = 0;                                                    \
    assert(mr);                                                        \
    assert(dest);                                                      \
    if (! marshall_read_1(mr, heap, magic)) {                          \
      err_puts("marshall_read_pset_item: marshall_read_1 magic");      \
      err_inspect_buf(heap ? mr->heap : mr->buf);                      \
      return marshall_read_ht_abort(mr);                              \
    }                                                                  \
    if (! marshall_read_heap_pointer(mr, heap, &offset,                \
                                     (void **) &present,              \
                                     sym_1(magic))) {                \
      err_puts("marshall_read_pset_item: marshall_read_heap_pointer"); \
      return marshall_read_ht_abort(mr);                              \
    }                                                                  \
    if (! offset) {                                                    \
      *dest = NULL;                                                    \
      return mr;                                                       \
    }                                                                  \
    if (present) {                                                     \
      *dest = present;                                                 \
      return mr;                                                       \
    }                                                                  \
    if (buf_seek(mr->heap, mr->heap_start + (s64) offset,              \
                 SEEK_SET) < 0) {                                      \
      err_puts("marshall_read_pset_item: buf_seek");                   \
      return marshall_read_ht_abort(mr);                              \
    }                                                                  \
    if (! (tmp = alloc(sizeof(s_set_item__ ## name))))                 \
      return marshall_read_ht_abort(mr);                              \
    if (! marshall_read_set_item__ ## name (mr, true, tmp)) {          \
      err_puts("marshall_read_pset_item: marshall_read");              \
      set_item_delete__ ## name(tmp);                                   \
      return marshall_read_ht_abort(mr);                              \
    }                                                                  \
    if (! marshall_read_ht_add(mr, offset, tmp, sym_1(magic))) {       \
      err_puts("marshall_read_pset_item: marshall_read_ht_add");       \
      set_item_delete__ ## name(tmp);                                   \
      return marshall_read_ht_abort(mr);                              \
    }                                                                  \
    *dest = tmp;                                                       \
    return mr;                                                         \
  }

#define DEF_MARSHALL_READ_SET_ITEM(name, type, magic)                 \
  s_marshall_read * marshall_read_set_item__ ## name                  \
  (s_marshall_read *mr, bool heap, s_set_item__ ## name *dest)        \
  {                                                                   \
    if (! marshall_read_uw(mr, heap, &dest->hash) ||                  \
        ! marshall_read_uw(mr, heap, &dest->usage)) {                 \
      err_puts("marshall_read_set_item: marshall_read_uw");           \
      assert(! "marshall_read_set_item: marshall_read_uw");           \
      return NULL;                                                    \
    }                                                                 \
    if (! marshall_read_ ## name(mr, heap, &dest->data)) {            \
      err_puts("marshall_read_set_item: marshall_read_" # name);      \
      assert(! "marshall_read_set_item: marshall_read_" # name);      \
      return NULL;                                                    \
    }                                                                 \
    if (! marshall_read_pset_item__ ## name(mr, heap, &dest->next)) { \
      err_puts("marshall_read_set_item:"                              \
               " marshall_read_pset_item");                           \
      assert(!("marshall_read_set_item:"                              \
               " marshall_read_pset_item"));                          \
      return NULL;                                                    \
    }                                                                 \
    return mr;                                                        \
  }

#define DEF_MARSHALL_READ_SET(name, type, magic)                       \
  s_marshall_read * marshall_read_set__ ## name                        \
  (s_marshall_read *mr, bool heap, s_set__ ## name *dest)              \
  {                                                                    \
    uw collisions = 0;                                                 \
    uw count = 0;                                                      \
    uw max = 0;                                                        \
    uw i;                                                              \
    s_set__ ## name tmp = {0};                                         \
    if (! mr || ! dest) {                                              \
      err_puts("marshall_read_set: invalid argument");                 \
      assert(! "marshall_read_set: invalid argument");                 \
      return NULL;                                                     \
    }                                                                  \
    if (! marshall_read_1(mr, heap, magic)) {                          \
      err_puts("marshall_read_set: marshall_read_1 magic");            \
      assert(! "marshall_read_set: marshall_read_1 magic");            \
      return NULL;                                                     \
    }                                                                  \
    if (! marshall_read_uw(mr, heap, &collisions) ||                   \
        ! marshall_read_uw(mr, heap, &count) ||                        \
        ! marshall_read_uw(mr, heap, &max)) {                          \
      err_puts("marshall_read_set: marshall_read_uw");                 \
      assert(! "marshall_read_set: marshall_read_uw");                 \
      return NULL;                                                     \
    }                                                                  \
    if (! max ||                                                       \
        max > UW_MAX / sizeof(s_set_item__ ## name *)) {               \
      err_puts("marshall_read_set: invalid max");                      \
      assert(! "marshall_read_set: invalid max");                      \
      return NULL;                                                     \
    }                                                                  \
    if (! set_init__ ## name (&tmp, max)) {                            \
      err_puts("marshall_read_set: set_init");                         \
      assert(! "marshall_read_set: set_init");                         \
      return NULL;                                                     \
    }                                                                  \
    i = 0;                                                             \
    while (i < max) {                                                  \
      if (! marshall_read_pset_item__ ## name(mr, heap,                \
                                              tmp.items + i)) {        \
        err_puts("marshall_read_set: marshall_read_pset_item_"         \
                 # name);                                              \
        assert(! "marshall_read_set: marshall_read_pset_item_"         \
               # name);                                                \
        set_clean__ ## name(&tmp);                                     \
        return NULL;                                                   \
      }                                                                \
      i++;                                                             \
    }                                                                  \
    *dest = tmp;                                                       \
    dest->collisions = collisions;                                     \
    dest->count = count;                                               \
    dest->max = max;                                                   \
    return mr;                                                         \
  }

static void marshall_read_ht_clean (s_marshall_read *mr);
static s_marshall_read * marshall_read_list_1 (s_marshall_read *mr,
                                              bool heap, s_list *dest,
                                              bool borrow_tail);
static void marshall_read_list_node_delete (p_list list);
static s_marshall_read * marshall_read_plist_1 (s_marshall_read *mr,
                                               bool heap, p_list *dest,
                                               bool borrow);
static s_marshall_read * marshall_read_tag_1 (s_marshall_read *mr,
                                             bool heap, s_tag *dest,
                                             bool borrow_list);

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
  uw size;
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
  if (tmp.dimension_count > UW_MAX / sizeof(s_array_dimension)) {
    err_puts("marshall_read_array: invalid dimension count");
    assert(! "marshall_read_array: invalid dimension count");
    return NULL;
  }
  tmp.dimensions = alloc(sizeof(s_array_dimension) *
                         tmp.dimension_count);
  if (! tmp.dimensions)
    return NULL;
  i = 0;
  while (i < tmp.dimension_count) {
    if (! marshall_read_uw(mr, heap, &tmp.dimensions[i].count) ||
        ! marshall_read_uw(mr, heap, &tmp.dimensions[i].item_size))
      goto ko;
    i++;
  }
  if (! marshall_read_uw(mr, heap, &tmp.count) ||
      ! marshall_read_uw(mr, heap, &tmp.size) ||
      ! marshall_read_bool(mr, heap, &has_data))
    goto ko;
  if (! tmp.array_type || ! tmp.element_type ||
      ! sym_is_array_type(tmp.array_type) ||
      sym_array_type(tmp.array_type) != tmp.element_type ||
      ! sym_type_size(tmp.element_type, &item_size) || ! item_size)
    goto invalid_layout;
  /* Check the packed layout before allocating or decoding any elements. */
  size = item_size;
  i = tmp.dimension_count;
  while (i) {
    i--;
    if (tmp.dimensions[i].item_size != size ||
        (tmp.dimensions[i].count &&
         size > UW_MAX / tmp.dimensions[i].count))
      goto invalid_layout;
    size *= tmp.dimensions[i].count;
  }
  if (tmp.size != size || tmp.count != size / item_size ||
      (has_data && ! size))
    goto invalid_layout;
  if (has_data) {
    if (! array_allocate(&tmp))
      goto ko;
    data = tmp.data;
    i = 0;
    while (i < tmp.count) {
      if (! marshall_read_data(mr, heap, tmp.element_type, data)) {
        tmp.count = i + 1;
        goto ko;
      }
      data = (u8 *) data + item_size;
      i++;
    }
  }
  else {
    if (! marshall_read_bool(mr, heap, &has_tags))
      goto ko;
    if (has_tags) {
      if (tmp.count > UW_MAX / sizeof(s_tag)) {
        err_puts("marshall_read_array: invalid tag count");
        assert(! "marshall_read_array: invalid tag count");
        goto ko;
      }
      tmp.tags = alloc(sizeof(s_tag) * tmp.count);
      if (! tmp.tags)
        goto ko;
      i = 0;
      while (i < tmp.count) {
        if (! marshall_read_tag(mr, heap, tmp.tags + i)) {
          tmp.count = i + 1;
          goto ko;
        }
        i++;
      }
    }
  }
  *dest = tmp;
  return mr;
 invalid_layout:
  err_puts("marshall_read_array: invalid layout");
  assert(! "marshall_read_array: invalid layout");
 ko:
  array_clean(&tmp);
  return NULL;
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
    tail = &(*tail)->next.data.td_plist;
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
    tail = &(*tail)->next.data.td_plist;
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
  FILE *buf_fp = NULL;
  FILE *heap_fp = NULL;
  assert(mr);
  if (mr->source && mr->buf_owned) {
    if (mr->buf && mr->buf->user_ptr)
      buf_fp = buf_file_fp(mr->buf);
    if (mr->heap && mr->heap->user_ptr)
      heap_fp = buf_file_fp(mr->heap);
  }
  if (mr->buf && mr->heap && mr->buf->user_ptr != mr->heap->user_ptr)
    buf_file_close(mr->heap);
  if (mr->buf && mr->buf_owned)
    buf_file_close(mr->buf);
  if (mr->buf && mr->buf_owned)
    buf_clean(mr->buf);
  if (mr->buf && mr->heap && mr->buf->ptr.p_pvoid != mr->heap->ptr.p_pvoid)
    buf_clean(mr->heap);
  if (mr->heap && mr->heap->ptr.p_pvoid == mr->buf->ptr.p_pvoid)
    rwlock_delete(mr->heap->rwlock);
  if (mr->heap && mr->buf != mr->heap)
    alloc_free(mr->heap);
  if (mr->buf && mr->buf_owned)
    alloc_free(mr->buf);
  if (heap_fp)
    fclose(heap_fp);
  if (buf_fp && buf_fp != heap_fp)
    fclose(buf_fp);
  marshall_read_ht_clean(mr);
}

s_marshall_read * marshall_read_complex (s_marshall_read *mr,
                                         bool heap,
                                         s_complex *dest)
{
  s_complex tmp = {0};
  assert(mr);
  assert(dest);
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
  if (type == &g_sym_Facts_star)
    return marshall_read_pfacts(mr, heap, data);
  if (type == &g_sym_PtrFree)
    return marshall_read_ptr_free(mr, heap, data);
  if (sym_is_pointer_type(type, NULL)) {
    s_pointer p = {0};
    if (! marshall_read_pointer(mr, heap, &p))
      return NULL;
    *(void **) data = p.ptr.p_pvoid;
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
  if (! st)
    return NULL;
  s.pstruct_type = st;
  s.data = data;
  return marshall_read_struct(mr, heap, &s);
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
    if (! kc3_dlopen(&dlopen_list->tag.data.td_str, &dl)) {
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
  u64 tmp;
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

s_marshall_read * marshall_read_fact_1 (s_marshall_read *mr,
                                        bool heap,
                                        s_fact *dest)
{
  p_set_item__tag object = NULL;
  p_set_item__tag predicate = NULL;
  p_set_item__tag subject = NULL;
  s_fact tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3FACT1_") ||
      ! marshall_read_pset_item__tag(mr, heap, &subject) ||
      ! marshall_read_pset_item__tag(mr, heap, &predicate) ||
      ! marshall_read_pset_item__tag(mr, heap, &object) ||
      ! marshall_read_uw(mr, heap, &tmp.id))
    return NULL;
  tmp.subject = subject ? &subject->data : NULL;
  tmp.predicate = predicate ? &predicate->data : NULL;
  tmp.object = object ? &object->data : NULL;
  *dest = tmp;
  return mr;
}

static s_marshall_read * marshall_read_facts_log
(s_marshall_read *mr, bool heap, s_facts *facts)
{
  s_str after_dump_path = {0};
  s_env *env;
  s_str path = {0};
  env = env_global();
  if (! mr || ! facts || ! env)
    return NULL;
  if (! marshall_read_str(mr, heap, &path)) {
    err_puts("marshall_read_facts_log: marshall_read_str: path");
    assert(! "marshall_read_facts_log: marshall_read_str: path");
    goto ko;
  }
  if (! marshall_read_str(mr, heap, &after_dump_path)) {
    err_puts("marshall_read_facts_log: marshall_read_str: after dump path");
    assert(! "marshall_read_facts_log: marshall_read_str: after dump path");
    goto ko;
  }
  if (! after_dump_path.size &&
      ! log_path_to_binary_path(&path, &after_dump_path)) {
    err_puts("marshall_read_facts_log: log_path_to_binary_path");
    assert(! "marshall_read_facts_log: log_path_to_binary_path");
    goto ko;
  }
  if (env->trace) {
    err_write_1("marshall_read_facts_log: facts_open_file_after_dump: ");
    err_inspect_str(&after_dump_path);
    err_write_1("\n");
  }
  if (facts_open_file_after_dump(facts, &after_dump_path) < 0) {
    err_write_1("marshall_read_facts_log: facts_open_file_after_dump: ");
    err_inspect_str(&after_dump_path);
    err_write_1(": ERROR\n");
    assert(! "marshall_read_facts_log: facts_open_file_after_dump");
    goto ko;
  }
  if (env->trace) {
    err_write_1("marshall_read_facts_log: after dump log: ");
    err_inspect_str(&after_dump_path);
    err_write_1(": OK\n");
  }
  if (facts_save_binary_file(facts, &path) < 0) {
    err_write_1("marshall_read_facts_log: facts_save_binary_file: ");
    err_inspect_str(&path);
    err_write_1("\n");
    assert(! "marshall_read_facts_log: facts_save_binary_file");
    goto ko;
  }
  if (! facts_open_memoized_register(facts, &path)) {
    err_write_1("marshall_read_facts_log: facts_open_memoized_register: ");
    err_inspect_str(&path);
    err_write_1("\n");
    assert(! "marshall_read_facts_log: facts_open_memoized_register");
    goto ko;
  }
  str_clean(&after_dump_path);
  str_clean(&path);
  return mr;
 ko:
  str_clean(&after_dump_path);
  str_clean(&path);
  return NULL;
}

s_marshall_read * marshall_read_facts_1 (s_marshall_read *mr,
                                         bool heap, s_facts *facts)
{
  s_env *env;
  s_set__fact facts_set = {0};
  bool has_log = false;
  p_skiplist__fact index = NULL;
  p_skiplist__fact index_osp = NULL;
  p_skiplist__fact index_pos = NULL;
  p_skiplist__fact index_spo = NULL;
  bool log_format = false;
  bool new_format = false;
  bool pointer_indexes;
  s_set__tag tags = {0};
  assert(mr);
  assert(facts);
  if (buf_peek_1(heap ? mr->heap : mr->buf, "_KC3FACTS3_") > 0) {
    log_format = true;
    new_format = true;
    if (! marshall_read_1(mr, heap, "_KC3FACTS3_") ||
        ! marshall_read_uw(mr, heap, &facts->id))
      return NULL;
  }
  else if (buf_peek_1(heap ? mr->heap : mr->buf,
                      "_KC3FACTS2_") > 0) {
    new_format = true;
    if (! marshall_read_1(mr, heap, "_KC3FACTS2_") ||
        ! marshall_read_uw(mr, heap, &facts->id))
      return NULL;
  }
  else if (! marshall_read_1(mr, heap, "_KC3FACTS1_"))
    return NULL;
  if (! marshall_read_set__tag(mr, heap, &tags) ||
      ! marshall_read_set__fact(mr, heap, &facts_set))
    return NULL;
  pointer_indexes =
    buf_peek_1(heap ? mr->heap : mr->buf, "_KC3PSKIPLISTFACT_") > 0;
  if (log_format && ! pointer_indexes) {
    err_puts("marshall_read_facts_1: invalid V3 fact index");
    assert(! "marshall_read_facts_1: invalid V3 fact index");
    return NULL;
  }
  if (pointer_indexes) {
    if (! marshall_read_pskiplist__fact(mr, heap, &index) ||
        ! marshall_read_pskiplist__fact(mr, heap, &index_spo) ||
        ! marshall_read_pskiplist__fact(mr, heap, &index_pos) ||
        ! marshall_read_pskiplist__fact(mr, heap, &index_osp))
      return NULL;
    if (! index || ! index_spo || ! index_pos || ! index_osp) {
      err_puts("marshall_read_facts_1: null fact index");
      assert(! "marshall_read_facts_1: null fact index");
      return NULL;
    }
  }
  else {
    if (! facts->index || ! facts->index_spo || ! facts->index_pos ||
        ! facts->index_osp ||
        ! marshall_read_skiplist__fact(mr, heap, facts->index) ||
        ! marshall_read_skiplist__fact(mr, heap, facts->index_spo) ||
        ! marshall_read_skiplist__fact(mr, heap, facts->index_pos) ||
        ! marshall_read_skiplist__fact(mr, heap, facts->index_osp))
      return NULL;
  }
  if (! marshall_read_uw(mr, heap, &facts->next_id))
    return NULL;
  if (pointer_indexes) {
    if (facts->index)
      skiplist_delete__fact(facts->index);
    if (facts->index_spo)
      skiplist_delete__fact(facts->index_spo);
    if (facts->index_pos)
      skiplist_delete__fact(facts->index_pos);
    if (facts->index_osp)
      skiplist_delete__fact(facts->index_osp);
    facts->index = index;
    facts->index_spo = index_spo;
    facts->index_pos = index_pos;
    facts->index_osp = index_osp;
  }
  if (facts->facts.items)
    set_clean__fact(&facts->facts);
  if (facts->tags.items)
    set_clean__tag(&facts->tags);
  facts->tags = tags;
  facts->facts = facts_set;
  if (new_format) {
    env = env_global();
#if HAVE_PTHREAD
    mutex_lock(&env->next_facts_id_mutex);
#endif
    if (facts->id >= env->next_facts_id)
      env->next_facts_id = facts->id + 1;
#if HAVE_PTHREAD
    mutex_unlock(&env->next_facts_id_mutex);
#endif
  }
  facts->index->compare = compare_fact_id;
  facts->index_spo->compare = compare_fact_spo;
  facts->index_pos->compare = compare_fact_pos;
  facts->index_osp->compare = compare_fact_osp;
  facts->ref_count = 1;
#if HAVE_PTHREAD
  rwlock_init(&facts->rwlock);
  mutex_init(&facts->ref_count_mutex);
#endif
  if (log_format &&
      (! marshall_read_bool(mr, heap, &has_log) ||
       (has_log && ! marshall_read_facts_log(mr, heap, facts))))
    return NULL;
  return mr;
}

s_marshall_read * marshall_read_facts (s_marshall_read *mr,
                                       bool heap, s_facts *facts)
{
  uw count;
  s_fact fact = {0};
  bool has_log = false;
  uw i;
  if (! mr || ! facts || ! env_global())
    return NULL;
  if (buf_peek_1(heap ? mr->heap : mr->buf, "_KC3FACTS3_") > 0 ||
      buf_peek_1(heap ? mr->heap : mr->buf, "_KC3FACTS2_") > 0 ||
      buf_peek_1(heap ? mr->heap : mr->buf, "_KC3FACTS1_") > 0)
    return marshall_read_facts_1(mr, heap, facts);
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
  if (has_log && ! marshall_read_facts_log(mr, heap, facts))
    return NULL;
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
  return NULL;
}

s_marshall_read * marshall_read_header (s_marshall_read *mr)
{
  return marshall_read_header_limit(mr, UW_MAX / sizeof(s_list *),
                                    SW_MAX, SW_MAX);
}

s_marshall_read * marshall_read_header_limit (s_marshall_read *mr,
                                             uw heap_count_max,
                                             uw heap_size_max,
                                             uw buf_size_max)
{
  u64 buf_size;
  u64 heap_count;
  u64 heap_offset;
  u64 heap_size;
  s_marshall_header mh = {0};
  s_str str = {0};
  assert(mr);
  if (! buf_read(mr->buf, sizeof(s_marshall_header), &str))
    return NULL;
  if (str.size != sizeof(s_marshall_header)) {
    err_puts("marshall_read_header: buf_read !="
             " sizeof(s_marshall_header)");
    str_clean(&str);
    return NULL;
  }
  memcpy(&mh, str.ptr.p_pvoid, sizeof(mh));
  str_clean(&str);
  if (le64toh(mh.le_magic) != MARSHALL_MAGIC) {
    err_puts("marshall_read_header: invalid magic");
    return NULL;
  }
  heap_offset = le64toh(mh.le_heap_offset);
  heap_count = le64toh(mh.le_heap_count);
  heap_size = le64toh(mh.le_heap_size);
  buf_size = le64toh(mh.le_buf_size);
  if (heap_count > UW_MAX / sizeof(s_list *) ||
      heap_offset > SW_MAX ||
      heap_size > SW_MAX - sizeof(s_marshall_header) ||
      buf_size > SW_MAX - sizeof(s_marshall_header) - heap_size ||
      heap_offset > SW_MAX - heap_size) {
    err_puts("marshall_read_header: size out of range");
    return NULL;
  }
  if ((heap_size && ! heap_count) || heap_count > heap_size) {
    err_puts("marshall_read_header: invalid heap count");
    return NULL;
  }
  if (heap_count > heap_count_max || heap_size > heap_size_max ||
      buf_size > buf_size_max) {
    err_puts("marshall_read_header: message limit exceeded");
    return NULL;
  }
  marshall_read_ht_clean(mr);
  mr->ht = (s_ht) {0};
  if (heap_count) {
    if (! ht_init(&mr->ht, &g_sym_Tuple, heap_count)) {
      err_puts("marshall_read_header: ht_init");
      return NULL;
    }
    mr->ht.compare = marshall_ht_compare;
    mr->ht.hash = marshall_ht_hash;
  }
  mr->heap_offset = heap_offset;
  mr->heap_count = heap_count;
  mr->heap_size = heap_size;
  mr->buf_size = buf_size;
  mr->ht_failed = false;
  return mr;
}

static s_marshall_read * marshall_read_ht_abort (s_marshall_read *mr)
{
  /* Decoding failure may free objects already referenced by the cache. */
  if (mr)
    mr->ht_failed = true;
  return NULL;
}

static bool marshall_read_ht_key (s_marshall_read *mr, u64 offset,
                                  p_sym type, s64 *dest)
{
  u64 base;
  bool pointer;
  if (! mr || mr->ht_failed || ! type || ! type->str.size ||
      ! mr->ht.items || offset < sizeof(s_marshall_header))
    return false;
  pointer = sym_is_pointer_type(type, NULL);
  if (offset - sizeof(s_marshall_header) > mr->heap_size ||
      (! pointer && offset - sizeof(s_marshall_header) == mr->heap_size))
    return false;
  base = mr->heap_start > 0 ? (u64) mr->heap_start : mr->heap_offset;
  if (offset > SW_MAX || base > SW_MAX - offset ||
      mr->heap_start < -(s64) sizeof(s_marshall_header))
    return false;
  *dest = base + offset;
  /* Custom marshallers may emit a native object at the same wire offset.
     Keep opaque pointees in a separate cache domain. */
  if (pointer)
    *dest = -*dest;
  return true;
}

s_marshall_read * marshall_read_heap_pointer (s_marshall_read *mr,
                                              bool heap,
                                              u64 *offset,
                                              void **present,
                                              p_sym type)
{
  s64 global_offset;
  s_tag key = {0};
  s_tag *ptag = NULL;
  s_tuple *entry;
  assert(mr);
  assert(offset);
  assert(present);
  if (mr->ht_failed || ! marshall_read_offset(mr, heap, offset))
    return marshall_read_ht_abort(mr);
  if (! *offset) {
    *present = NULL;
    return mr;
  }
  if (! marshall_read_ht_key(mr, *offset, type, &global_offset) ||
      ! tag_init_ptuple(&key, 3))
    return marshall_read_ht_abort(mr);
  tag_init_s64(key.data.td_ptuple->tag, global_offset);
  /* A zero pointer reserves the offset until decoding completes. */
  tag_init_uw(key.data.td_ptuple->tag + 1, 0);
  tag_init_psym(key.data.td_ptuple->tag + 2, type);
  if (! ht_get(&mr->ht, &key, &ptag)) {
    if (! ht_add(&mr->ht, &key)) {
      tag_clean(&key);
      return marshall_read_ht_abort(mr);
    }
    tag_clean(&key);
    *present = NULL;
    return mr;
  }
  tag_clean(&key);
  if (ptag->type != TAG_PTUPLE || ! (entry = ptag->data.td_ptuple) ||
      entry->count != 3 || entry->tag[1].type != TAG_UW ||
      entry->tag[2].type != TAG_PSYM ||
      entry->tag[2].data.td_psym != type || ! entry->tag[1].data.td_uw)
    return marshall_read_ht_abort(mr);
  *present = (void *) entry->tag[1].data.td_uw;
  return mr;
}

s_marshall_read * marshall_read_ht_add (s_marshall_read *mr,
                                        u64 offset, const void *p,
                                        p_sym type)
{
  s64 global_offset;
  s_tag key = {0};
  s_tag *ptag = NULL;
  s_tuple *entry;
  if (! p || ! marshall_read_ht_key(mr, offset, type, &global_offset) ||
      ! tag_init_ptuple(&key, 2))
    return marshall_read_ht_abort(mr);
  tag_init_s64(key.data.td_ptuple->tag, global_offset);
  if (! ht_get(&mr->ht, &key, &ptag)) {
    tag_clean(&key);
    return marshall_read_ht_abort(mr);
  }
  tag_clean(&key);
  if (ptag->type != TAG_PTUPLE || ! (entry = ptag->data.td_ptuple) ||
      entry->count != 3 || entry->tag[1].type != TAG_UW ||
      entry->tag[2].type != TAG_PSYM ||
      entry->tag[2].data.td_psym != type || entry->tag[1].data.td_uw)
    return marshall_read_ht_abort(mr);
  entry->tag[1].data.td_uw = (uw) p;
  return mr;
}

void marshall_read_ht_clean (s_marshall_read *mr)
{
  assert(mr);
  if (mr->ht.items)
    ht_clean(&mr->ht);
  while (mr->ht_ptag_list) {
    if (mr->ht_ptag_list->tag.type == TAG_PLIST) {
      /* Each retained list entry owns one canonical node. Tail links
         borrow other cached nodes, so do not recursively delete them. */
      marshall_read_list_node_delete
        (mr->ht_ptag_list->tag.data.td_plist);
      mr->ht_ptag_list->tag.data.td_plist = NULL;
    }
    else
      ptag_clean((p_tag *) &mr->ht_ptag_list->tag.data.td_ptr.p_pvoid);
    mr->ht_ptag_list = list_delete(mr->ht_ptag_list);
  }
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
  if (! (mr->heap = alloc(sizeof(s_buf)))) {
    err_puts("marshall_read_init: heap allocation error");
    assert(! "marshall_read_init: heap allocation error");
    return NULL;
  }
  if (! buf_init_alloc(mr->heap, BUF_SIZE)) {
    alloc_free(mr->heap);
    mr->heap = NULL;
    return NULL;
  }
  if (! (mr->buf = alloc(sizeof(s_buf)))) {
    err_puts("marshall_read_init: buffer allocation error");
    assert(! "marshall_read_init: buffer allocation error");
    buf_clean(mr->heap);
    alloc_free(mr->heap);
    return NULL;
  }
  if (! buf_init_alloc(mr->buf, BUF_SIZE)) {
    alloc_free(mr->buf);
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
  if (! (mr->heap = alloc(sizeof(s_buf)))) {
    err_puts("marshall_read_init_buf: heap allocation error");
    assert(! "marshall_read_init_buf: heap allocation error");
    return NULL;
  }
  if (! buf_init_alloc(mr->heap, BUF_SIZE)) {
    alloc_free(mr->heap);
    mr->heap = NULL;
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
  str.ptr.p_pchar = p;
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
    goto ko_close;
  }
  mr->heap_start = 0;
  return mr;
 ko_close:
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
  if (! mr->heap->rwlock) {
    alloc_free(mr->heap);
    buf_clean(mr->buf);
    alloc_free(mr->buf);
    mr->heap = NULL;
    mr->buf = NULL;
    return NULL;
  }
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
    mp_set_u32(&tmp.in_int, 0);
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
  if (mp_unpack(&tmp.in_int, count, MP_LSB_FIRST, digit_size,
                MP_LITTLE_ENDIAN, nail_bits,
                buf->ptr.p_pu8 + buf->rpos) != MP_OKAY) {
    err_puts("marshall_read_integer: mp_unpack failed");
    assert(! "marshall_read_integer: mp_unpack failed");
    integer_clean(&tmp);
    return NULL;
  }
  if (sign) {
    if (mp_neg(&tmp.in_int, &tmp.in_int) != MP_OKAY) {
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
  return marshall_read_list_1(mr, heap, dest, false);
}

static s_marshall_read * marshall_read_list_1 (s_marshall_read *mr,
                                              bool heap, s_list *dest,
                                              bool borrow_tail)
{
  assert(mr);
  assert(dest);
  *dest = (s_list) {0};
  if (! marshall_read_1(mr, heap, "_KC3LIST_")) {
    err_puts("marshall_read_list: marshall_read_1 magic");
    assert(! "marshall_read_list: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_tag(mr, heap, &dest->tag))
    return NULL;
  if (! marshall_read_tag_1(mr, heap, &dest->next, borrow_tail)) {
    tag_clean(&dest->tag);
    return NULL;
  }
  return mr;
}

static void marshall_read_list_node_delete (p_list list)
{
  if (list->next.type == TAG_PLIST) {
    list->next.data.td_plist = NULL;
    tag_clean(&list->next);
  }
  list_delete(list);
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Call)) {
    err_puts("marshall_read_pcall: marshall_read_heap_pointer");
    return marshall_read_ht_abort(mr);
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcall_init_copy(dest, &present)) {
      err_puts("marshall_read_pcall: pcall_init_copy");
      assert(! "marshall_read_pcall: pcall_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_pcall: buf_seek");
    return marshall_read_ht_abort(mr);
  }
  if (! (tmp = alloc(sizeof(s_call)))) {
    err_puts("marshall_read_pcall: alloc");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_call(mr, true, tmp)) {
    err_puts("marshall_read_pcall: marshall_read_call");
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Call)) {
    err_puts("marshall_read_pcall: marshall_read_ht_add");
    call_delete(tmp);
    return marshall_read_ht_abort(mr);
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Callable)) {
    err_puts("marshall_read_pcallable: marshall_read_heap_pointer");
    return marshall_read_ht_abort(mr);
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcallable_init_copy(dest, &present)) {
      err_puts("marshall_read_pcallable: pcallable_init_copy");
      assert(! "marshall_read_pcallable: pcallable_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_pcallable: buf_seek");
    return marshall_read_ht_abort(mr);
  }
  if (! (tmp = alloc(sizeof(s_callable)))) {
    err_puts("marshall_read_pcallable: alloc");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_callable(mr, true, tmp)) {
    err_puts("marshall_read_pcallable: marshall_read_callable");
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Callable)) {
    err_puts("marshall_read_pcallable: marshall_read_ht_add");
    callable_delete(tmp);
    return marshall_read_ht_abort(mr);
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Complex))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pcomplex_init_copy(dest, &present)) {
      err_puts("marshall_read_pcomplex: pcomplex_init_copy");
      assert(! "marshall_read_pcomplex: pcomplex_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_complex))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_complex(mr, true, tmp)) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Complex)) {
    complex_delete(tmp);
    return marshall_read_ht_abort(mr);
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Facts)) {
    err_puts("marshall_read_pfacts: marshall_read_heap_pointer");
    return marshall_read_ht_abort(mr);
  }
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pfacts_init_copy(dest, &present)) {
      err_puts("marshall_read_pfacts: pfacts_init_copy");
      assert(! "marshall_read_pfacts: pfacts_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_pfacts: buf_seek");
    return marshall_read_ht_abort(mr);
  }
  if (! (tmp = facts_new())) {
    err_puts("marshall_read_pfacts: facts_new");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_facts(mr, true, tmp)) {
    err_puts("marshall_read_pfacts: marshall_read_facts");
    facts_delete(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Facts)) {
    err_puts("marshall_read_pfacts: marshall_read_ht_add");
    facts_delete(tmp);
    return marshall_read_ht_abort(mr);
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, sym_1("Frame")))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pframe_init_copy(dest, &present)) {
      err_puts("marshall_read_pframe: pframe_init_copy");
      assert(! "marshall_read_pframe: pframe_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_frame))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_frame(mr, true, tmp)) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, sym_1("Frame"))) {
    frame_delete(tmp);
    return marshall_read_ht_abort(mr);
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_plist (s_marshall_read *mr,
                                       bool heap,
                                       p_list *dest)
{
  return marshall_read_plist_1(mr, heap, dest, false);
}

static s_marshall_read * marshall_read_plist_1 (s_marshall_read *mr,
                                               bool heap, p_list *dest,
                                               bool borrow)
{
  p_list keep;
  u64 offset = 0;
  p_list tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PLIST_")) {
    err_puts("marshall_read_plist: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &tmp, &g_sym_List))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (! tmp) {
    if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
        ! (tmp = alloc(sizeof(s_list))))
      return marshall_read_ht_abort(mr);
    if (! marshall_read_list_1(mr, true, tmp, true)) {
      alloc_free(tmp);
      return marshall_read_ht_abort(mr);
    }
    if (! (keep = list_new_plist(tmp, mr->ht_ptag_list))) {
      marshall_read_list_node_delete(tmp);
      return marshall_read_ht_abort(mr);
    }
    mr->ht_ptag_list = keep;
    if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_List))
      return marshall_read_ht_abort(mr);
  }
  /* Only canonical next links may borrow cache nodes. Every public
     result owns an independent list, including on the first decode. */
  if (borrow)
    *dest = tmp;
  else if (! plist_init_copy(dest, &tmp)) {
    err_puts("marshall_read_plist: plist_init_copy");
    return marshall_read_ht_abort(mr);
  }
  return mr;
}

s_marshall_read * marshall_read_pointer_1 (s_marshall_read *mr,
                                           bool heap,
                                           s_pointer *dest)
{
  s_call call = {0};
  s_env *env = NULL;
  s_ident ident = {0};
  p_list list = NULL;
  p_callable pcallable = NULL;
  p_tag ptag = NULL;
  s_tag tag_1 = {0};
  p_sym pointer_type;
  p_sym target_type;
  uw id;
  u64 offset = 0;
  void *present = NULL;
  s_pointer tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3POINTER1_") ||
      ! marshall_read_psym(mr, heap, &target_type) ||
      ! marshall_read_uw(mr, heap, &id) ||
      ! target_type || ! target_type->str.size ||
      ! (pointer_type = sym_target_to_pointer_type(target_type)) ||
      ! marshall_read_heap_pointer(mr, heap, &offset,
                                   &present, pointer_type))
    return marshall_read_ht_abort(mr);
  if (offset && ! present) {
    if (! marshall_read_pcallable(mr, heap, &pcallable))
      return marshall_read_ht_abort(mr);
    if (! (ptag = tag_new_pcallable_copy(&pcallable)) ||
        ! (list = list_new_ptr(ptag, mr->ht_ptag_list))) {
      if (ptag)
        tag_delete(ptag);
      pcallable_clean(&pcallable);
      return marshall_read_ht_abort(mr);
    }
    mr->ht_ptag_list = list;
    env = env_global();
    ident.module = target_type;
    ident.sym = &g_sym_marshall_read;
    call_init(&call);
    call.ident = ident;
    call.pcallable = pcallable;
    call.arguments = list_new_pointer
      (NULL, &g_sym_MarshallRead, mr, list_new_bool
       (heap, NULL));
    if (! env_eval_call(env, &call, &tag_1)) {
      call_clean(&call);
      return marshall_read_ht_abort(mr);
    }
    if (tag_1.type != TAG_POINTER ||
        tag_1.data.td_pointer.target_type != target_type) {
      tag_clean(&tag_1);
      call_clean(&call);
      return marshall_read_ht_abort(mr);
    }
    present = tag_1.data.td_pointer.ptr.p_pvoid;
    if (! marshall_read_ht_add(mr, offset, present, pointer_type)) {
      tag_clean(&tag_1);
      call_clean(&call);
      return marshall_read_ht_abort(mr);
    }
    tag_clean(&tag_1);
    call_clean(&call);
  }
  if (! pointer_init(&tmp, NULL, target_type, present))
    return marshall_read_ht_abort(mr);
  tmp.id = id;
  env = env_global();
#if HAVE_PTHREAD
  mutex_lock(&env->next_pointer_id_mutex);
#endif
  if (id >= env->next_pointer_id)
    env->next_pointer_id = id + 1;
#if HAVE_PTHREAD
  mutex_unlock(&env->next_pointer_id_mutex);
#endif
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
  s_tag tag = {0};
  s_tag tag_1 = {0};
  p_sym pointer_type;
  p_sym target_type;
  u64 offset = 0;
  void *present = NULL;
  s_pointer tmp = {0};
  assert(mr);
  assert(dest);
  if (buf_peek_1(heap ? mr->heap : mr->buf,
                 "_KC3POINTER1_") > 0)
    return marshall_read_pointer_1(mr, heap, dest);
  if (! marshall_read_1(mr, heap, "_KC3POINTER_")) {
    err_puts("marshall_read_pointer: marshall_read_1 magic");
    err_inspect_buf(heap ? mr->heap : mr->buf);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_psym(mr, heap, &target_type)) {
    err_puts("marshall_read_pointer: marshall_read_psym");
    return marshall_read_ht_abort(mr);
  }
  if (! target_type || ! target_type->str.size ||
      ! (pointer_type = sym_target_to_pointer_type(target_type)))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   &present, pointer_type)) {
    err_puts("marshall_read_pointer: marshall_read_heap_pointer");
    return marshall_read_ht_abort(mr);
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
      return marshall_read_ht_abort(mr);
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
      return marshall_read_ht_abort(mr);
    }
    if (tag_1.type != TAG_POINTER ||
        tag_1.data.td_pointer.target_type != target_type) {
      err_write_1("marshall_read_pointer: ");
      err_inspect_ident(&ident);
      err_write_1("\n");
      tag_clean(&tag_1);
      tag_clean(&tag);
      call_clean(&call);
      return marshall_read_ht_abort(mr);
    }
    present = tag_1.data.td_pointer.ptr.p_pvoid;
    if (! marshall_read_ht_add(mr, offset, present, pointer_type)) {
      tag_clean(&tag_1);
      tag_clean(&tag);
      call_clean(&call);
      return marshall_read_ht_abort(mr);
    }
    tag_clean(&tag_1);
    tag_clean(&tag);
    call_clean(&call);
  }
  if (! pointer_init(&tmp, NULL, target_type, present)) {
    err_puts("marshall_read_pointer: pointer_init");
    return marshall_read_ht_abort(mr);
  }
  *dest = tmp;
  return mr;
}

DEF_MARSHALL_READ_PSET_ITEM(fact, s_fact, "_KC3PSETITEMFACT_")
DEF_MARSHALL_READ_PSET_ITEM(tag,  s_tag,  "_KC3PSETITEMTAG_")

s_marshall_read *
marshall_read_pskiplist_node__fact (s_marshall_read *mr, bool heap,
                                    p_skiplist_node__fact *dest)
{
  u8 height;
  u64 offset = 0;
  p_skiplist_node__fact present = NULL;
  u64 pos;
  p_skiplist_node__fact tmp;
  if (! mr || ! dest ||
      ! marshall_read_1(mr, heap, "_KC3PSKIPLISTNODEFACT_") ||
      ! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present,
                                   sym_1("Skiplist.Node.Fact")))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = present;
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset,
               SEEK_SET) < 0 ||
      ! buf_tell_r(mr->heap, &pos) ||
      ! marshall_read_1(mr, true, "_KC3SKIPLISTNODEFACT_") ||
      ! marshall_read_u8(mr, true, &height) ||
      ! height ||
      buf_seek(mr->heap, pos, SEEK_SET) < 0 ||
      ! (tmp = alloc(SKIPLIST_NODE_SIZE__fact(height))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_skiplist_node__fact(mr, true, tmp) ||
      ! marshall_read_ht_add(mr, offset, tmp, sym_1("Skiplist.Node.Fact"))) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  *dest = tmp;
  return mr;
}

s_marshall_read *
marshall_read_skiplist_node__fact (s_marshall_read *mr, bool heap,
                                   s_skiplist_node__fact *dest)
{
  u8 height;
  uw i;
  p_skiplist_node__fact *links;
  p_set_item__fact item = NULL;
  if (! mr || ! dest ||
      ! marshall_read_1(mr, heap, "_KC3SKIPLISTNODEFACT_") ||
      ! marshall_read_u8(mr, heap, &height) ||
      ! height ||
      ! marshall_read_pset_item__fact(mr, heap, &item))
    return NULL;
  skiplist_node_init(dest, item ? &item->data : NULL, height);
  links = SKIPLIST_NODE_LINKS__fact(dest);
  i = 0;
  while (i < height) {
    if (! marshall_read_pskiplist_node__fact(mr, heap, links + i))
      return NULL;
    i++;
  }
  return mr;
}

s_marshall_read *
marshall_read_pskiplist__fact (s_marshall_read *mr, bool heap,
                               p_skiplist__fact *dest)
{
  u8 max_height;
  u64 offset = 0;
  p_skiplist__fact present = NULL;
  u64 pos;
  p_skiplist__fact tmp;
  uw length;
  if (! mr || ! dest ||
      ! marshall_read_1(mr, heap, "_KC3PSKIPLISTFACT_") ||
      ! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present,
                                   sym_1("Skiplist.Fact")))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    *dest = present;
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset,
               SEEK_SET) < 0 ||
      ! buf_tell_r(mr->heap, &pos) ||
      ! marshall_read_1(mr, true, "_KC3SKIPLISTFACT_") ||
      ! marshall_read_uw(mr, true, &length) ||
      ! marshall_read_u8(mr, true, &max_height) ||
      ! max_height ||
      buf_seek(mr->heap, pos, SEEK_SET) < 0 ||
      ! (tmp = alloc(SKIPLIST_SIZE__fact(max_height))))
    return marshall_read_ht_abort(mr);
  tmp->max_height = max_height;
  if (! marshall_read_skiplist__fact(mr, true, tmp) ||
      ! marshall_read_ht_add(mr, offset, tmp, sym_1("Skiplist.Fact"))) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  *dest = tmp;
  return mr;
}

s_marshall_read *
marshall_read_skiplist__fact (s_marshall_read *mr, bool heap,
                              s_skiplist__fact *dest)
{
  t_skiplist_height *height_table;
  uw i;
  uw length;
  u8 max_height_alloc;
  u8 max_height;
  p_skiplist_node__fact head = NULL;
  if (! mr || ! dest)
    return NULL;
  max_height_alloc = dest->max_height;
  if (! marshall_read_1(mr, heap, "_KC3SKIPLISTFACT_") ||
      ! marshall_read_uw(mr, heap, &length) ||
      ! marshall_read_u8(mr, heap, &max_height) ||
      ! max_height || max_height > max_height_alloc)
    return NULL;
  dest->length = length;
  dest->max_height = max_height;
  height_table = SKIPLIST_HEIGHT_TABLE__fact(dest);
  i = 0;
  while (i < max_height) {
    if (! marshall_read_u64(mr, heap, height_table + i))
      return NULL;
    i++;
  }
  if (! marshall_read_pskiplist_node__fact(mr, heap, &head))
    return NULL;
  dest->head = head;
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Struct))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pstruct_init_copy(dest, &present)) {
      err_puts("marshall_read_pstruct: pstruct_init_copy");
      assert(! "marshall_read_pstruct: pstruct_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_struct))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_struct(mr, true, tmp)) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Struct)) {
    struct_delete(tmp);
    return marshall_read_ht_abort(mr);
  }
  *dest = tmp;
  return mr;
}

// TODO: use sym_must_clean
s_marshall_read * marshall_read_pstruct_type (s_marshall_read *mr,
                                              bool heap,
                                              p_struct_type *dest)
{
  p_tag keep = NULL;
  p_list list = NULL;
  u64 offset = 0;
  p_struct_type present = NULL;
  p_struct_type tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PSTRUCTTYPE_")) {
    err_puts("marshall_read_pstruct_type: marshall_read_1 magic");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_StructType))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pstruct_type_init_copy(dest, &present)) {
      err_puts("marshall_read_pstruct_type: pstruct_type_init_copy");
      assert(! "marshall_read_pstruct_type: pstruct_type_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_struct_type))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_struct_type(mr, true, tmp)) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! (keep = tag_new())) {
    pstruct_type_clean(&tmp);
    return marshall_read_ht_abort(mr);
  }
  keep->type = TAG_PSTRUCT_TYPE;
  keep->data.td_pstruct_type = tmp;
  if (! (list = list_new_ptr(keep, mr->ht_ptag_list))) {
    tag_delete(keep);
    return marshall_read_ht_abort(mr);
  }
  mr->ht_ptag_list = list;
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_StructType) ||
      ! pstruct_type_init_copy(dest, &tmp))
    return marshall_read_ht_abort(mr);
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   &present, &g_sym_Sym))
    return marshall_read_ht_abort(mr);
  if (present || ! offset) {
    *dest = present;
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_psym: buf_seek failed");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_sym(mr, true, &tmp)) {
    err_puts("marshall_read_psym: marshall_read_sym failed");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Sym)) {
    err_puts("marshall_read_psym: marshall_read_ht_add failed");
    return marshall_read_ht_abort(mr);
  }
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_ptag (s_marshall_read *mr,
                                      bool heap,
                                      p_tag *dest)
{
  p_list list = NULL;
  u64 offset = 0;
  p_tag present = NULL;
  p_tag tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3PTAG_")) {
    err_puts("marshall_read_ptag: marshall_read_1 magic");
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Tag))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! ptag_init_copy(dest, &present))
      return marshall_read_ht_abort(mr);
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0) {
    err_puts("marshall_read_ptag: buf_seek");
    return marshall_read_ht_abort(mr);
  }
  if (! (tmp = alloc(sizeof(s_tag))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_tag(mr, true, tmp)) {
    err_puts("marshall_read_ptag: marshall_read_tag");
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! (list = list_new_ptr(tag_new_ref(tmp), mr->ht_ptag_list))) {
    tag_delete(tmp);
    return marshall_read_ht_abort(mr);
  }
  mr->ht_ptag_list = list;
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Tag)) {
    tag_delete(tmp);
    return marshall_read_ht_abort(mr);
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
  if (! marshall_read_uw(mr, heap, &tmp.p_uw))
    return NULL;
  if (tmp.p_uw)
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
  if (! marshall_read_uw(mr, heap, &tmp.p_uw))
    return NULL;
  if (tmp.p_uw)
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
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_heap_pointer(mr, heap, &offset,
                                   (void **) &present, &g_sym_Var))
    return marshall_read_ht_abort(mr);
  if (! offset) {
    *dest = NULL;
    return mr;
  }
  if (present) {
    if (! pvar_init_copy(dest, &present)) {
      err_puts("marshall_read_pvar: pvar_init_copy");
      assert(! "marshall_read_pvar: pvar_init_copy");
      return marshall_read_ht_abort(mr);
    }
    return mr;
  }
  if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
      ! (tmp = alloc(sizeof(s_var))))
    return marshall_read_ht_abort(mr);
  if (! marshall_read_var(mr, true, tmp)) {
    alloc_free(tmp);
    return marshall_read_ht_abort(mr);
  }
  if (! marshall_read_ht_add(mr, offset, tmp, &g_sym_Var)) {
    var_delete(tmp);
    return marshall_read_ht_abort(mr);
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
    tag_clean(tmp.tag);
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
      ! marshall_read_integer(mr, heap, &tmp.denominator)) {
    ratio_clean(&tmp);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

DEF_MARSHALL_READ(s8, "_KC3S8_", s8)
DEF_MARSHALL_READ_LETOH(s16, "_KC3S16_", s16, 16)
DEF_MARSHALL_READ_LETOH(s32, "_KC3S32_", s32, 32)
DEF_MARSHALL_READ_LETOH(s64, "_KC3S64_", s64, 64)
DEF_MARSHALL_READ_SET(fact, s_fact, "_KC3SETFACT_")
DEF_MARSHALL_READ_SET(tag,  s_tag,  "_KC3SETTAG_")
DEF_MARSHALL_READ_SET_ITEM(tag,  s_tag, "_KC3SETITEMTAG_")

s_marshall_read * marshall_read_set_item__fact
(s_marshall_read *mr, bool heap, s_set_item__fact *dest)
{
  if (! marshall_read_uw(mr, heap, &dest->hash) ||
      ! marshall_read_uw(mr, heap, &dest->usage) ||
      ! marshall_read_fact_1(mr, heap, &dest->data) ||
      ! marshall_read_pset_item__fact(mr, heap, &dest->next))
    return NULL;
  return mr;
}

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

static bool marshall_read_struct_field_layout (p_sym type, uw *size,
                                               u8 *align,
                                               bool *must_clean)
{
  static const p_sym builtin[] = {
    &g_sym_Array, &g_sym_Block, &g_sym_Bool, &g_sym_Call,
    &g_sym_Callable, &g_sym_Character, &g_sym_Complex, &g_sym_Cow,
    &g_sym_F32, &g_sym_F64,
#if HAVE_F80
    &g_sym_F80,
#endif
#if HAVE_F128
    &g_sym_F128,
#endif
    &g_sym_Fact, &g_sym_Facts, &g_sym_Ident, &g_sym_Integer,
    &g_sym_List, &g_sym_Map, &g_sym_Pointer, &g_sym_Ptr,
    &g_sym_PtrFree, &g_sym_Quote, &g_sym_Ratio, &g_sym_S8,
    &g_sym_S16, &g_sym_S32, &g_sym_S64, &g_sym_Str,
    &g_sym_Struct, &g_sym_StructType, &g_sym_Sw, &g_sym_Sym,
    &g_sym_Tag, &g_sym_Time, &g_sym_Tuple, &g_sym_U8,
    &g_sym_U16, &g_sym_U32, &g_sym_U64, &g_sym_Uw,
    &g_sym_Var, &g_sym_Void
  };
  uw i;
  p_struct_type st = NULL;
  if (! type || ! type->str.size)
    return false;
  if (sym_is_array_type(type))
    type = &g_sym_Array;
  if (type == &g_sym_Cfn || type == &g_sym_Fn)
    type = &g_sym_Callable;
  if (sym_is_pointer_type(type, NULL)) {
    *size = sizeof(void *);
    *align = _Alignof(void *);
    *must_clean = type == &g_sym_Facts_star;
    return true;
  }
  for (i = 0; i < sizeof(builtin) / sizeof(*builtin); i++) {
    if (type != builtin[i])
      continue;
    if (! sym_type_size(type, size) ||
        ! sym_type_alignment(type, align))
      return false;
    if (type == &g_sym_Array || type == &g_sym_Fact ||
        type == &g_sym_Facts)
      *must_clean = true;
    else if (! sym_must_clean(type, must_clean))
      return false;
    return true;
  }
  if (! pstruct_type_find(type, &st) || ! st)
    return false;
  *size = st->size;
  *align = st->align_max;
  *must_clean = st->must_clean || st->clean;
  return true;
}

static bool marshall_read_struct_type_layout (s_struct_type *st,
                                              bool serialized)
{
  u8 align;
  u8 align_max = 1;
  uw end = 0;
  uw i;
  bool must_clean;
  bool needs_clean = false;
  uw offset;
  uw padding;
  uw size;
  p_sym type;
  bool valid = true;
  s_tag *value;
  if (st->map.count > UW_MAX / sizeof(uw) ||
      st->map.count > UW_MAX / sizeof(s_tag) ||
      (st->map.count && (! st->map.key || ! st->map.value)))
    return false;
  for (i = 0; i < st->map.count; i++) {
    value = st->map.value + i;
    if ((value->type == TAG_PVAR && ! value->data.td_pvar) ||
        (value->type == TAG_PSTRUCT && ! value->data.td_pstruct)) {
      value->type = TAG_VOID;
      valid = false;
    }
  }
  if (! valid || ! st->module)
    return false;
  if (serialized) {
    if (! st->align_max ||
        (st->align_max & (st->align_max - 1)) ||
        st->size > SW_MAX || st->size % st->align_max ||
        (st->map.count && ! st->offset))
      return false;
  }
  else if (st->map.count &&
           ! (st->offset = alloc(st->map.count * sizeof(uw))))
    return false;
  for (i = 0; i < st->map.count; i++) {
    value = st->map.value + i;
    if (st->map.key[i].type != TAG_PSYM ||
        ! st->map.key[i].data.td_psym ||
        (value->type == TAG_PSTRUCT &&
         ! value->data.td_pstruct->pstruct_type) ||
        ! tag_type_var(value, &type) || ! type)
      return false;
    if (! marshall_read_struct_field_layout(type, &size, &align,
                                            &must_clean)) {
      if (value->type != TAG_PSTRUCT)
        return false;
      size = value->data.td_pstruct->pstruct_type->size;
      align = value->data.td_pstruct->pstruct_type->align_max;
      must_clean = value->data.td_pstruct->pstruct_type->must_clean ||
        value->data.td_pstruct->pstruct_type->clean;
    }
    if (! align || (align & (align - 1)))
      return false;
    if (align > align_max)
      align_max = align;
    needs_clean |= must_clean;
    if (serialized) {
      offset = st->offset[i];
      if (offset < end || offset % align || offset > st->size ||
          size > st->size - offset)
        return false;
    }
    else {
      padding = (align - end % align) % align;
      if (padding > SW_MAX - end || size > SW_MAX - end - padding)
        return false;
      offset = end + padding;
      st->offset[i] = offset;
    }
    end = offset + size;
  }
  if (serialized) {
    if (st->align_max < align_max)
      return false;
  }
  else {
    padding = (align_max - end % align_max) % align_max;
    if (padding > SW_MAX - end)
      return false;
    st->size = end + padding;
    st->align_max = align_max;
  }
  st->must_clean = needs_clean;
  return true;
}

static bool marshall_read_struct_type_match (const s_struct_type *a,
                                             const s_struct_type *b)
{
  uw i;
  p_sym a_type;
  p_sym b_type;
  if (! a || a->module != b->module || a->size != b->size ||
      a->align_max != b->align_max || a->map.count != b->map.count ||
      (a->map.count && (! a->offset || ! a->map.key || ! a->map.value)))
    return false;
  for (i = 0; i < a->map.count; i++) {
    if (a->offset[i] != b->offset[i] ||
        compare_tag(a->map.key + i, b->map.key + i) ||
        ! tag_type_var(a->map.value + i, &a_type) ||
        ! tag_type_var(b->map.value + i, &b_type) ||
        a_type != b_type)
      return false;
  }
  return true;
}

s_marshall_read * marshall_read_struct (s_marshall_read *mr,
                                        bool heap,
                                        s_struct *dest)
{
  bool has_tags;
  bool has_data;
  uw data_count = 0;
  uw i;
  uw offset;
  uw tag_count = 0;
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
  if (! tmp.pstruct_type ||
      ! marshall_read_struct_type_layout(tmp.pstruct_type, true) ||
      (dest->data &&
       ! marshall_read_struct_type_match(dest->pstruct_type,
                                        tmp.pstruct_type)))
    goto ko;
  if (! marshall_read_bool(mr, heap, &has_tags)) {
    err_puts("marshall_read_struct: marshall_read_bool has tags");
    assert(! "marshall_read_struct: marshall_read_bool has tags");    
    goto ko;
  }
  if (dest->data && has_tags)
    goto ko;
  if (has_tags && tmp.pstruct_type->map.count) {
    if (! (tmp.tag = alloc(tmp.pstruct_type->map.count *
                           sizeof(s_tag))))
      goto ko;
    i = 0;
    while (i < tmp.pstruct_type->map.count) {
      if (! marshall_read_tag(mr, heap, tmp.tag + i))
        goto ko;
      tag_count++;
      i++;
    }
  }
  if (! marshall_read_bool(mr, heap, &has_data)) {
    err_puts("marshall_read_struct: marshall_read_bool has data");
    assert(! "marshall_read_struct: marshall_read_bool has data");    
    goto ko;
  }
  if (dest->data && ! has_data)
    goto ko;
  if (has_data) {
    if (! (tmp.data = alloc(tmp.pstruct_type->size ?
                            tmp.pstruct_type->size : 1)))
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
      data_count++;
      i++;
    }
  }
  if (dest->data) {
    memcpy(dest->data, tmp.data, tmp.pstruct_type->size);
    alloc_free(tmp.data);
    tmp.data = NULL;
    struct_clean(&tmp);
    return mr;
  }
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  mutex_init(&dest->mutex);
#endif
  return mr;
ko:
  for (i = 0; i < data_count; i++) {
    tag_type_var(tmp.pstruct_type->map.value + i, &type);
    data_clean(type, (u8 *) tmp.data + tmp.pstruct_type->offset[i]);
  }
  alloc_free(tmp.data);
  for (i = 0; i < tag_count; i++)
    tag_clean(tmp.tag + i);
  alloc_free(tmp.tag);
  if (tmp.pstruct_type)
    struct_type_delete(tmp.pstruct_type);
  return NULL;
}

s_marshall_read * marshall_read_struct_type_1 (s_marshall_read *mr,
                                               bool heap,
                                               s_struct_type *dest)
{
  uw i;
  s_struct_type tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_1(mr, heap, "_KC3STRUCTTYPE1_") ||
      ! marshall_read_psym(mr, heap, &tmp.module) ||
      ! marshall_read_map(mr, heap, &tmp.map))
    return NULL;
  if (tmp.map.count &&
      ! (tmp.offset = alloc(tmp.map.count * sizeof(uw))))
    goto ko;
  i = 0;
  while (i < tmp.map.count) {
    if (! marshall_read_uw(mr, heap, tmp.offset + i))
      goto ko;
    i++;
  }
  if (! marshall_read_u8(mr, heap, &tmp.align_max) ||
      ! marshall_read_uw(mr, heap, &tmp.size) ||
      ! marshall_read_pcallable(mr, heap, &tmp.clean) ||
      ! marshall_read_bool(mr, heap, &tmp.must_clean))
    goto ko;
  if (! marshall_read_struct_type_layout(&tmp, true)) {
    err_puts("marshall_read_struct_type: invalid layout");
    goto ko;
  }
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  mutex_init(&dest->mutex);
#endif
  return mr;
 ko:
  struct_type_clean(&tmp);
  return NULL;
}

s_marshall_read * marshall_read_struct_type (s_marshall_read *mr,
                                             bool heap,
                                             s_struct_type *dest)
{
  s_struct_type tmp = {0};
  assert(mr);
  assert(mr);
  if (buf_peek_1(heap ? mr->heap : mr->buf,
                 "_KC3STRUCTTYPE1_") > 0)
    return marshall_read_struct_type_1(mr, heap, dest);
  if (! marshall_read_1(mr, heap, "_KC3STRUCTTYPE_")) {
    err_puts("marshall_read_str: marshall_read_1 magic");
    assert(! "marshall_read_str: marshall_read_1 magic");
    return NULL;
  }
  if (! marshall_read_psym(mr, heap, &tmp.module) ||
      ! marshall_read_map(mr, heap, &tmp.map))
    return NULL;
  if (! marshall_read_pcallable(mr, heap, &tmp.clean) ||
      ! marshall_read_struct_type_layout(&tmp, false))
    goto ko;
  tmp.ref_count = 1;
  *dest = tmp;
#if HAVE_PTHREAD
  mutex_init(&dest->mutex);
#endif
  return mr;
 ko:
  struct_type_clean(&tmp);
  return NULL;
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
  return marshall_read_tag_1(mr, heap, dest, false);
}

static s_marshall_read * marshall_read_tag_1 (s_marshall_read *mr,
                                             bool heap, s_tag *dest,
                                             bool borrow_list)
{
  s_marshall_read *result;
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
  dest->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&dest->ref_count_mutex);
#endif
  dest->type = type;
  if (borrow_list && type == TAG_PLIST)
    result = marshall_read_plist_1(mr, heap, &dest->data.td_plist, true);
  else
    result = marshall_read_tag_data(mr, heap, dest);
  if (! result) {
#if HAVE_PTHREAD
    mutex_clean(&dest->ref_count_mutex);
#endif
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_tag_data (s_marshall_read *mr,
                                          bool heap, s_tag *dest)
{
  assert(mr);
  assert(dest);
  switch (dest->type) {
  case TAG_VOID:
    return mr;
  case TAG_ARRAY:
    return marshall_read_array(mr, heap, &dest->data.td_array);
  case TAG_DO_BLOCK:
    return marshall_read_do_block(mr, heap, &dest->data.td_do_block);
  case TAG_BOOL:
    return marshall_read_bool(mr, heap, &dest->data.td_bool_);
  case TAG_CHARACTER:
    return marshall_read_character(mr, heap, &dest->data.td_character);
  case TAG_F32:
    return marshall_read_f32(mr, heap, &dest->data.td_f32);
  case TAG_F64:
    return marshall_read_f64(mr, heap, &dest->data.td_f64);
#if HAVE_F80
  case TAG_F80:
    return marshall_read_f80(mr, heap, &dest->data.td_f80);
#endif
#if HAVE_F128
  case TAG_F128:
    return marshall_read_f128(mr, heap, &dest->data.td_f128);
#endif
  case TAG_FACT:
    return marshall_read_fact(mr, heap, &dest->data.td_fact);
  case TAG_IDENT:
    return marshall_read_ident(mr, heap, &dest->data.td_ident);
  case TAG_INTEGER:
    return marshall_read_integer(mr, heap, &dest->data.td_integer);
  case TAG_MAP:
    return marshall_read_map(mr, heap, &dest->data.td_map);
  case TAG_PCALL:
    return marshall_read_pcall(mr, heap, &dest->data.td_pcall);
  case TAG_PCALLABLE:
    return marshall_read_pcallable(mr, heap, &dest->data.td_pcallable);
  case TAG_PCOMPLEX:
    return marshall_read_pcomplex(mr, heap, &dest->data.td_pcomplex);
  case TAG_PCOW:
    return marshall_read_pcow(mr, heap, &dest->data.td_pcow);
  case TAG_PFACTS:
    return marshall_read_pfacts(mr, heap, &dest->data.td_pfacts);
  case TAG_PLIST:
    return marshall_read_plist(mr, heap, &dest->data.td_plist);
  case TAG_POINTER:
    return marshall_read_pointer(mr, heap, &dest->data.td_pointer);
  case TAG_PSTRUCT:
    return marshall_read_pstruct(mr, heap, &dest->data.td_pstruct);
  case TAG_PSTRUCT_TYPE:
    return marshall_read_pstruct_type(mr, heap,
                                      &dest->data.td_pstruct_type);
  case TAG_PSYM:
    return marshall_read_psym(mr, heap, &dest->data.td_psym);
  case TAG_PTR:
    return marshall_read_ptr(mr, heap, &dest->data.td_ptr);
  case TAG_PTR_FREE:
    return marshall_read_ptr_free(mr, heap, &dest->data.td_ptr_free);
  case TAG_QUOTE:
    return marshall_read_quote(mr, heap, &dest->data.td_quote);
  case TAG_RATIO:
    return marshall_read_ratio(mr, heap, &dest->data.td_ratio);
  case TAG_STR:
    return marshall_read_str(mr, heap, &dest->data.td_str);
  case TAG_S8:
    return marshall_read_s8(mr, heap, &dest->data.td_s8);
  case TAG_S16:
    return marshall_read_s16(mr, heap, &dest->data.td_s16);
  case TAG_S32:
    return marshall_read_s32(mr, heap, &dest->data.td_s32);
  case TAG_S64:
    return marshall_read_s64(mr, heap, &dest->data.td_s64);
  case TAG_SW:
    return marshall_read_sw(mr, heap, &dest->data.td_sw);
  case TAG_TIME:
    return marshall_read_time(mr, heap, &dest->data.td_time);
  case TAG_PTUPLE:
    return marshall_read_ptuple(mr, heap, &dest->data.td_ptuple);
  case TAG_U8:
    return marshall_read_u8(mr, heap, &dest->data.td_u8);
  case TAG_U16:
    return marshall_read_u16(mr, heap, &dest->data.td_u16);
  case TAG_U32:
    return marshall_read_u32(mr, heap, &dest->data.td_u32);
  case TAG_U64:
    return marshall_read_u64(mr, heap, &dest->data.td_u64);
  case TAG_UNQUOTE:
    return marshall_read_unquote(mr, heap, &dest->data.td_unquote);
  case TAG_UW:
    return marshall_read_uw(mr, heap, &dest->data.td_uw);
  case TAG_PVAR:
    return marshall_read_pvar(mr, heap, &dest->data.td_pvar);
  }
  err_write_1("marshall_read_tag: unknown tag type: ");
  err_inspect_u8_decimal(dest->type);
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
  p_tag keep;
  p_list list;
  u64 offset = 0;
  p_tuple present = NULL;
  p_tuple tuple = NULL;
  assert(mr);
  assert(dest);
  buf = heap ? mr->heap : mr->buf;
  if (buf_read_1(buf, "_KC3PTUPLE_") > 0) {
    if (! marshall_read_heap_pointer(mr, heap, &offset,
                                     (void **) &present, &g_sym_Tuple))
      return marshall_read_ht_abort(mr);
    if (! offset) {
      *dest = NULL;
      return mr;
    }
    if (present) {
      if (! ptuple_init_copy(dest, &present))
        return marshall_read_ht_abort(mr);
      return mr;
    }
    if (buf_seek(mr->heap, mr->heap_start + (s64) offset, SEEK_SET) < 0 ||
        ! (tuple = alloc(sizeof(s_tuple))))
      return marshall_read_ht_abort(mr);
    if (! marshall_read_tuple(mr, true, tuple)) {
      alloc_free(tuple);
      return marshall_read_ht_abort(mr);
    }
    if (! (keep = tag_new())) {
      tuple_delete(tuple);
      return marshall_read_ht_abort(mr);
    }
    keep->type = TAG_PTUPLE;
    keep->data.td_ptuple = tuple;
    if (! (list = list_new_ptr(keep, mr->ht_ptag_list))) {
      tag_delete(keep);
      return marshall_read_ht_abort(mr);
    }
    mr->ht_ptag_list = list;
    if (! marshall_read_ht_add(mr, offset, tuple, &g_sym_Tuple) ||
        ! ptuple_init_copy(dest, &tuple))
      return marshall_read_ht_abort(mr);
    return mr;
  }
  tuple = alloc(sizeof(s_tuple));
  if (! tuple)
    return marshall_read_ht_abort(mr);
  if (! marshall_read_tuple(mr, heap, tuple)) {
    alloc_free(tuple);
    return marshall_read_ht_abort(mr);
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
    if (! (tmp.tag = alloc(sizeof(s_tag))))
      return NULL;
    if (! marshall_read_tag(mr, heap, tmp.tag)) {
      tag_clean(tmp.tag);
      alloc_free(tmp.tag);
      return NULL;
    }
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
  s_env *env;
  bool new_format = false;
  s_var tmp = {0};
  assert(mr);
  assert(dest);
  if (buf_peek_1(heap ? mr->heap : mr->buf, "_KC3VAR1_") > 0) {
    new_format = true;
    if (! marshall_read_1(mr, heap, "_KC3VAR1_") ||
        ! marshall_read_uw(mr, heap, &tmp.id))
      return NULL;
  }
  else if (! marshall_read_1(mr, heap, "_KC3VAR_"))
    return NULL;
  if (! marshall_read_ident(mr, heap, &tmp.name) ||
      ! marshall_read_psym(mr, heap, &tmp.type) ||
      ! marshall_read_bool(mr, heap, &tmp.bound) ||
      (tmp.bound &&
       ! marshall_read_tag(mr, heap, &tmp.tag)))
    return NULL;
  env = env_global();
#if HAVE_PTHREAD
  mutex_lock(&env->next_var_id_mutex);
#endif
  if (new_format) {
    if (tmp.id >= env->next_var_id)
      env->next_var_id = tmp.id + 1;
  }
  else
    tmp.id = env->next_var_id++;
#if HAVE_PTHREAD
  mutex_unlock(&env->next_var_id_mutex);
#endif
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
