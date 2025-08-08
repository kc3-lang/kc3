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
#include "endian.h"
#include "file.h"
#include "ht.h"
#include "io.h"
#include "pstruct_type.h"
#include "struct_type.h"
#include "sym.h"
#include "list.h"
#include "str.h"
#include "tag.h"
#include "tag_init.h"
#include "types.h"
#include "marshall.h"

#define DEF_MARSHALL(type)                                            \
  s_marshall * marshall_ ## type (s_marshall *m, bool heap, type src) \
  {                                                                   \
    s_buf *buf;                                                       \
    type le;                                                          \
    sw r;                                                             \
    assert(m);                                                        \
    le = _Generic(src,                                                \
                  s16:     htole16(src),                              \
                  u16:     htole16(src),                              \
                  s32:     htole32(src),                              \
                  u32:     htole32(src),                              \
                  s64:     htole64(src),                              \
                  u64:     htole64(src),                              \
                  default: src);                                      \
    buf = heap ? &m->heap : &m->buf;                                  \
    if ((r = buf_write_ ## type(buf, le)) <= 0)                       \
      return NULL;                                                    \
    if (heap)                                                         \
      m->heap_pos += r;                                               \
    else                                                              \
      m->buf_pos += r;                                                \
    return m;                                                         \
  }

#define MARSHALL_P(name, type)                                        \
  s_marshall * marshall_p ## name (s_marshall *m, bool heap,          \
                                   type data)                         \
  {                                                                   \
    assert(m);                                                        \
    bool present = false;                                             \
    if (! m)                                                          \
      return NULL;                                                    \
    if (! marshall_heap_pointer(m, heap, data, &present))             \
      return NULL;                                                    \
    if (! present && data)                                            \
      return marshall_ ## name(m, true, data);                        \
    return m;                                                         \
  }


s_marshall * marshall_array (s_marshall *m, bool heap,
                             const s_array *array)
{
  uw i = 0;
  assert(m);
  assert(array);
  if (! marshall_psym(m, heap, array->array_type) ||
      ! marshall_uw(m, heap, array->dimension_count))
    return NULL;
  i = 0;
  while (i < array->dimension_count) {
    if (! marshall_uw(m, heap, array->dimensions[i].count) ||
        ! marshall_uw(m, heap, array->dimensions[i].item_size))
      return NULL;
    i++;
  }
  if (! marshall_psym(m, heap, array->element_type) ||
      ! marshall_data(m, heap, array->array_type, array->data) ||
      ! marshall_uw(m, heap, array->size))
    return NULL;
  i = 0;
  while (i < array->count) {
    if (! marshall_tag(m, heap, &array->tags[i]))
      return NULL;
    i++;
  }
  return m;
}

DEF_MARSHALL(bool)

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
  if (! marshall_pcallable(m, heap, call->pcallable))
    return NULL;
  return m;
}

s_marshall * marshall_callable (s_marshall *m, bool heap,
                                const s_callable *callable)
{
  assert(m);
  assert(callable);
  if (! m || ! callable)
    return NULL;
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
      ! marshall_psym(m, heap, cfn->result_type) ||
      ! marshall_psym(m, heap, cfn->c_name) ||
      ! marshall_u8(m, heap, cfn->arity))
    return NULL;
  arg_type = cfn->arg_types;
  while (arg_type) {
    if (arg_type->tag.type != TAG_PSYM) {
      err_puts("marshall_cfn: invalid arg type");
      assert(! "marshall_cfn: invalid arg type");
      return NULL;
    }
    if (! marshall_psym(m, heap, arg_type->tag.data.psym))
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

DEF_MARSHALL(f32)
DEF_MARSHALL(f64)
DEF_MARSHALL(f128)

s_marshall * marshall_fact (s_marshall *m, bool heap,
                            const s_fact *fact)
{
  assert(m);
  if (! m ||
      ! marshall_ptag(m, heap, fact->subject) ||
      ! marshall_ptag(m, heap, fact->predicate) ||
      ! marshall_ptag(m, heap, fact->object) ||
      ! marshall_uw(m, heap, fact->id))
    return NULL;
  return m;
}

s_marshall * marshall_fn (s_marshall *m, bool heap, const s_fn *fn)
{
  s_fn_clause *clause;
  assert(m);
  assert(fn);
  assert(! fn->frame || ! fn->frame->next);
  if (! m || ! fn)
    return NULL;
  if (! marshall_bool(m, heap, fn->macro) ||
      ! marshall_bool(m, heap, fn->special_operator) ||
      ! marshall_ident(m, heap, &fn->name) ||
      ! marshall_psym(m, heap, fn->module))
    return NULL;
  clause = fn->clauses;
  while (clause) {
    if (! marshall_list(m, heap, clause->pattern) ||
        ! marshall_do_block(m, heap, &clause->algo))
        return NULL;
    clause = clause->next;
  }
 if (fn->frame && ! marshall_frame(m, heap, fn->frame))
    return NULL;
  return m;
}

s_marshall * marshall_frame (s_marshall *m, bool heap,
                             const s_frame *frame)
{
  s_binding *bindings;
  assert(m);
  assert(frame);
  if (! m || ! frame)
    return NULL;
  bindings = frame->bindings;
  while (bindings) {
    if (! marshall_psym(m, heap, bindings->name) ||
        ! marshall_tag(m, heap, &bindings->value))
      return NULL;
    bindings = bindings->next;
  }
  if (! marshall_pframe(m, heap, frame->next))
    return NULL;
  return m;
}

s_marshall * marshall_heap_pointer (s_marshall *m, bool heap,
                                    const void *p, bool *present)
{
  s_tag key = {0};
  s_tag tag = {0};
  u64 offset;
  assert(m);
  if (! p)
    return marshall_u64(m, heap, 0);
  tag_init_tuple(&key, 2);
  key.data.tuple.tag[0].type = TAG_U64;
  key.data.tuple.tag[0].data.u64 = (u64) p;
  if (ht_get(&m->ht, &key, &tag)) {
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
 ok:
  if (tag.type != TAG_TUPLE ||
      tag.data.tuple.count != 2 ||
      tag.data.tuple.tag[1].type != TAG_U64) {
    err_puts("marshall_heap_pointer: invalid offset in hash table");
    err_inspect_tag(&tag);
    err_write_1("\n");
    assert(! "marshall_heap_pointer: invalid offset in hash table");
    goto ko;
  }
  offset = tag.data.tuple.tag[1].data.u64;
  if (! marshall_u64(m, heap, offset))
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
      ! marshall_psym(m, heap, ident->module) ||
      ! marshall_psym(m, heap, ident->sym))
    return NULL;
  return m;
}

s_marshall * marshall_init (s_marshall *m)
{
  s_marshall tmp = {0};
  if (! ht_init(&tmp.ht, &g_sym_Tag, 1024) ||
    ! buf_init_alloc(&tmp.heap, 1024024))
    return NULL;
  if (! buf_init_alloc(&tmp.buf, 1024024)) {
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
  if (! marshall_tag(m, heap, &list->tag) ||
      ! marshall_tag(m, heap, &list->next))
    return NULL;
  return m;
}

s_marshall *marshall_map (s_marshall *m, bool heap, const s_map *map)
{
  uw i = 0;
  assert(m);
  assert(map);
  assert(map->count);
  assert(map->key);
  assert(map->value);
  if (! marshall_uw(m, heap, map->count))
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
  if (marshall_init(m) == NULL) {
    free(m);
    return NULL;
  }
  return m;
}

MARSHALL_P(callable, p_callable)
MARSHALL_P(cow, p_cow)
MARSHALL_P(frame, p_frame)
MARSHALL_P(list, p_list)
MARSHALL_P(struct, p_struct)
MARSHALL_P(struct_type, p_struct_type)
MARSHALL_P(sym, p_sym)
MARSHALL_P(tag, p_tag)

s_marshall * marshall_ptr (s_marshall *m, bool heap, u_ptr_w ptr)
{
  assert(m);
  if (! ptr.p)
    return marshall_uw(m, heap, 0);
  err_puts("marshall_ptr: non-null pointer.");
  assert(! "marshall_ptr: non-null pointer.");
  return NULL;
}

s_marshall * marshall_ptr_free (s_marshall *m, bool heap,
                                u_ptr_w ptr_free)
{
  assert(m);
  if (! ptr_free.p)
    return marshall_uw(m, heap, 0);
  err_puts("marshall_ptr_free: non-null pointer.");
  assert(! "marshall_ptr_free: non-null pointer.");
  return NULL;
}

MARSHALL_P(var, p_var)

s_marshall * marshall_quote (s_marshall *m, bool heap,
                             const s_quote *quote)
{
  assert(m);
  assert(quote);
  assert(quote->tag);
  if (! quote->tag || ! marshall_tag(m, heap, quote->tag))
    return NULL;
  return m;
}

s_marshall * marshall_ratio (s_marshall *m, bool heap,
                             const s_ratio *ratio)
{
  assert(m);
  assert(ratio);
  if (! marshall_integer(m, heap, &ratio->numerator) ||
      ! marshall_integer(m, heap, &ratio->denominator))
    return NULL;
  return m;
}

DEF_MARSHALL(s8)
DEF_MARSHALL(s16)
DEF_MARSHALL(s32)
DEF_MARSHALL(s64)

s_marshall * marshall_str (s_marshall *m, bool heap, const s_str *src)
{
  sw r;
  s_buf *buf;
  
  assert(m);
  assert(src);
  if (! marshall_u32(m, heap, src->size))
    return NULL;
  if (! src->size)
    return m;
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write(buf, src->ptr.pchar, src->size)) <= 0)
    return NULL;
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

// TODO: convert f_clean to callable
s_marshall * marshall_struct_type (s_marshall *m, bool heap,
                                   const s_struct_type *st)
{
  uw i;
  assert(m);
  assert(st);
  assert(st->module);
  assert(st->offset);
  if (! m || ! st || ! st->module || ! st->offset ||
      ! marshall_psym(m, heap, st->module) ||
      ! marshall_map(m, heap, &st->map))
    return NULL;
  i = 0;
  while (i < st->map.count) {
    if (! marshall_uw(m, heap, st->offset[i]))
      return NULL;
    i++;
  }
  if (! marshall_uw(m, heap, st->size))
    return NULL;
  return m;
}

s_marshall * marshall_struct (s_marshall *m, bool heap,
                              const s_struct *s)
{
  uw i;
  uw offset;
  p_sym type = NULL;
  assert(m);
  assert(s);
  if (! m || ! s ||
      ! marshall_pstruct_type(m, heap, s->pstruct_type) ||
      ! marshall_bool(m, heap, s->tag ? true : false))
    return NULL;
  if (s->tag) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      if (! marshall_tag(m, heap, s->tag + i))
        return NULL;
      i++;
    }
  }
  if (! marshall_bool(m, heap, s->data ? true : false))
    return NULL;
  if (s->data) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      offset = s->pstruct_type->offset[i];
      if (! tag_type_var(s->pstruct_type->map.value + i, &type))
        return NULL;
      if (! marshall_data(m, heap, type, (u8 *) s->data + offset))
        return NULL;
      i++;
    }
  }
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
  if (! m || ! type || ! data)
    return NULL;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return marshall_array(m, heap, data);
  if (type == &g_sym_Bool)
    return marshall_bool(m, heap, *(bool *)data);
  if (type == &g_sym_Call)
    return marshall_call(m, heap, data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return marshall_pcallable(m, heap, data);
  if (type == &g_sym_Character)
    return marshall_character(m, heap, *(character *)data);
  if (type == &g_sym_Complex)
    return marshall_complex(m, heap, data);
  if (type == &g_sym_Cow)
    return marshall_pcow(m, heap, data);
  if (type == &g_sym_F32)
    return marshall_f32(m, heap, *(f32 *) data);
  if (type == &g_sym_F64)
    return marshall_f64(m, heap, *(f64 *) data);
  if (type == &g_sym_F128)
    return marshall_f128(m, heap, *(f128 *) data);
  if (type == &g_sym_Fact)
    return marshall_fact(m, heap, data);
  if (type == &g_sym_Ident)
    return marshall_ident(m, heap, data);
  if (type == &g_sym_Integer)
    return marshall_integer(m, heap, data);
  if (type == &g_sym_List)
    return marshall_plist(m, heap, data);
  if (type == &g_sym_Map)
    return marshall_map(m, heap, data);
  if (type == &g_sym_Ptag)
    return marshall_ptag(m, heap, data);
  if (type == &g_sym_Ptr)
    return marshall_ptr(m, heap, *(u_ptr_w *) data);
  if (type == &g_sym_PtrFree)
    return marshall_ptr_free(m, heap, *(u_ptr_w *) data);
  if (type == &g_sym_Quote)
    return marshall_quote(m, heap, data);
  if (type == &g_sym_S8)
    return marshall_s8(m, heap, *(s8 *) data);
  if (type == &g_sym_S16)
    return marshall_s16(m, heap, *(s16 *) data);
  if (type == &g_sym_S32)
    return marshall_s32(m, heap, *(s32 *) data);
  if (type == &g_sym_S64)
    return marshall_s64(m, heap, *(s64 *) data);
  if (type == &g_sym_Str)
    return marshall_str(m, heap, data);
  if (type == &g_sym_Struct)
    return marshall_pstruct(m, heap, data);
  if (type == &g_sym_StructType)
    return marshall_pstruct_type(m, heap, data);
  if (type == &g_sym_Sw)
    return marshall_sw(m, heap, *(sw *) data);
  if (type == &g_sym_Sym)
    return marshall_sym(m, heap, data);
  if (type == &g_sym_Tag)
    return marshall_tag(m, heap, data);
  if (type == &g_sym_Time)
    return marshall_time(m, heap, data);
  if (type == &g_sym_Tuple)
    return marshall_tuple(m, heap, data);
  if (type == &g_sym_U8)
    return marshall_u8(m, heap,  *(u8 *) data);
  if (type == &g_sym_U16)
    return marshall_u16(m, heap, *(u16 *) data);
  if (type == &g_sym_U32)
    return marshall_u32(m, heap, *(u32 *) data);
  if (type == &g_sym_U64)
    return marshall_u64(m, heap, *(u64 *) data);
  if (type == &g_sym_Uw)
    return marshall_uw(m, heap,  *(uw *) data);
  if (type == &g_sym_Var)
    return marshall_pvar(m, heap, data);
  if (type == &g_sym_Void)
    return m;
  if (! pstruct_type_find(type, &st))
    return NULL;
  if (st) {
    s.pstruct_type = st;
    s.data = data;
    marshall_struct(m, heap, &s);
  }
  return m;
}

s_marshall * marshall_sym (s_marshall *m, bool heap,
                           const s_sym *sym)
{
  assert(m);
  if (! m || ! sym)
    return NULL;
  if (! marshall_str(m, heap, &sym->str))
    return NULL;
  return m;
}

DEF_MARSHALL(sw)

s_marshall * marshall_tag (s_marshall *m, bool heap, const s_tag *tag)
{
  u8 type;
  assert(m);
  assert(tag);
  type = tag->type;
  marshall_u8(m, heap, type);
  switch (tag->type) {
  case TAG_VOID: return m;
  case TAG_ARRAY: return marshall_array(m, heap, &tag->data.array);
  case TAG_BOOL:  return marshall_bool(m, heap, tag->data.bool_);
  case TAG_CALL:  return marshall_call(m, heap, &tag->data.call);
  case TAG_CHARACTER:
    return marshall_character(m, heap, tag->data.character);
  case TAG_DO_BLOCK:
    return marshall_do_block(m, heap, &tag->data.do_block);
  case TAG_F32:   return marshall_f32(m, heap, tag->data.f32);
  case TAG_F64:   return marshall_f64(m, heap, tag->data.f64);
  case TAG_F128:  return marshall_f128(m, heap, tag->data.f128);
  case TAG_FACT:  return marshall_fact(m, heap, &tag->data.fact);
  case TAG_IDENT: return marshall_ident(m, heap, &tag->data.ident);
  case TAG_INTEGER:
    return marshall_integer(m, heap, &tag->data.integer);
  case TAG_MAP:   return marshall_map(m, heap, &tag->data.map);
  case TAG_PCALLABLE:
    return marshall_pcallable(m, heap, tag->data.pcallable);
  case TAG_PCOMPLEX:
    return marshall_complex(m, heap, tag->data.pcomplex);
  case TAG_PCOW:  return marshall_pcow(m, heap, tag->data.pcow); 
  case TAG_PLIST: return marshall_plist(m, heap, tag->data.plist);
  case TAG_PSTRUCT:
    return marshall_pstruct(m, heap, tag->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return marshall_pstruct_type(m, heap, tag->data.pstruct_type);
  case TAG_PSYM:  return marshall_sym(m, heap, tag->data.psym);
  case TAG_PTAG:
    return marshall_ptag(m, heap, tag->data.ptag);
  case TAG_PTR:   return marshall_ptr(m, heap, tag->data.ptr);
  case TAG_PTR_FREE:
    return marshall_ptr_free(m, heap, tag->data.ptr_free);
  case TAG_PVAR:  return marshall_pvar(m, heap, tag->data.pvar);
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
  else if (! marshall_uw(m, heap, time->tv_sec) ||
           ! marshall_uw(m, heap, time->tv_nsec))
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
  if ((r = buf_write(out, &mh, sizeof(mh))) != sizeof(mh))
    return -1;
  result += r;
  if ((r = buf_xfer(out, &m->heap, m->heap_pos)) != m->heap_pos)
    return -1;
  result += r;
  if ((r = buf_xfer(out, &m->buf, m->buf_pos)) != m->buf_pos)
    return -1;
  result += r;
  return result;
}

sw marshall_to_file (s_marshall *m, const char *path)
{
  FILE *fp;
  s_buf out;
  sw r;
  sw result = 0;
  assert(m);
  assert(path);
  if (! buf_init_alloc(&out, 1024 * 1024))
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
  if (! buf_read_to_str(&out, dest)) {
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

DEF_MARSHALL(u8)
DEF_MARSHALL(u16)
DEF_MARSHALL(u32)
DEF_MARSHALL(u64)

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

DEF_MARSHALL(uw)

s_marshall *marshall_var(s_marshall *m, bool heap, const s_var *var)
{
  assert(m);
  assert(var);
  if (! m || ! var ||
      ! marshall_bool(m, heap, var->bound) ||
      ! marshall_sym(m, heap, var->type) || 
      ! marshall_tag(m, heap, &var->tag))
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

