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
#include "hash.h"
#include "list.h"
#include "str.h"
#include "tag.h"
#include "tag_type.h"
#include "void.h"

#define HASH_UPDATE_DEF(type)                                          \
  bool hash_update_##type (t_hash *hash, const type *x)                \
  {                                                                    \
    const s8 t[] = #type;                                              \
    assert(hash);                                                      \
    assert(x);                                                         \
    return hash_update(hash, t, sizeof(t)) &&                          \
      hash_update(hash, x, sizeof(type));                              \
  }

void hash_clean (t_hash *hash)
{
  *hash = (t_hash) {0};
}

void hash_init (t_hash *hash)
{
  assert(hash);
  SHA1Init(hash);
}

uw hash_to_uw (t_hash *hash)
{
  u8 digest[SHA1_DIGEST_LENGTH];
  SHA1Final(digest, hash);
  return *((uw *) digest);
}

u64 hash_to_u64 (t_hash *hash)
{
  u8 digest[SHA1_DIGEST_LENGTH];
  SHA1Final(digest, hash);
  return *((u64 *) digest);
}

bool hash_update (t_hash *hash, const void *data, uw size)
{
  assert(hash);
  assert(data);
  SHA1Update(hash, data, size);
  return true;
}

bool hash_update_1 (t_hash *hash, const s8 *p)
{
  uw len;
  const s8 type[] = "s8*";
  assert(hash);
  assert(p);
  len = strlen(p);
  return hash_update(hash, type, sizeof(type)) &&
    hash_update(hash, &len, sizeof(len)) &&
    hash_update(hash, p, strlen(p));
}

bool hash_update_array (t_hash *hash, const s_array *a)
{
  uw i = 0;
  const s8 type[] = "array";
  assert(hash);
  assert(a);
  if (! hash_update(hash, type, sizeof(type)) ||
      ! hash_update(hash, &a->dimension, sizeof(a->dimension)) ||
      ! hash_update(hash, &a->type, sizeof(a->type)))
    return false;
  while (i < a->dimension) {
    if (! hash_update(hash, &a->dimensions[i].count,
                      sizeof(a->dimensions[i].count)))
      return false;
    i++;
  }
  if (a->data) {
    if (! hash_update(hash, a->data, a->size))
      return false;
  }
  else if (a->tags) {
    i = 0;
    while (i < a->count) {
      if (! hash_update_tag(hash, a->tags + i))
        return false;
      i++;
    }
  }
  return true;
}

bool hash_update_bool (t_hash *hash, const bool *x)
{
  bool b;
  const s8 type[] = "bool";
  assert(hash);
  assert(x);
  b = x ? 1 : 0;
  return hash_update(hash, type, sizeof(type)) &&
    hash_update(hash, &b, sizeof(b));
}

bool hash_update_call (t_hash *hash, const s_call *call)
{
  const s8 type[] = "call";
  assert(hash);
  assert(call);
  return hash_update(hash, type, sizeof(type)) &&
    hash_update_ident(hash, &call->ident) &&
    hash_update_list(hash, (const s_list * const *) &call->arguments);
}

bool hash_update_cfn (t_hash *hash, const s_cfn *cfn)
{
  const s8 type[] = "cfn";
  assert(hash);
  assert(cfn);
  return hash_update(hash, type, sizeof(type)) &&
    hash_update_sym(hash, &cfn->name) &&
    hash_update_list(hash, (const s_list * const *) &cfn->arg_types);
}

HASH_UPDATE_DEF(character)

HASH_UPDATE_DEF(f32)

HASH_UPDATE_DEF(f64)

bool hash_update_fact (t_hash *hash, const s_fact *fact)
{
  const s8 type[] = "fact";
  assert(hash);
  assert(fact);
  assert(fact->subject);
  assert(fact->predicate);
  assert(fact->object);
  return hash_update(hash, type, sizeof(type)) &&
    hash_update_tag(hash, fact->subject) &&
    hash_update_tag(hash, fact->predicate) &&
    hash_update_tag(hash, fact->object);
}

bool hash_update_fn (t_hash *hash, const s_fn *fn)
{
  const s8 type[] = "fn";
  assert(hash);
  assert(fn);
  return hash_update(hash, type, sizeof(type)) &&
    hash_update_bool(hash, &fn->macro) &&
    hash_update_bool(hash, &fn->special_operator) &&
    hash_update_fn_clauses(hash, fn->clauses);
}

bool hash_update_fn_clauses (t_hash *hash, const s_fn_clause *clauses)
{
  uw count = 0;
  const s_fn_clause *f;
  const s8 type[] = "fn_clauses";
  assert(hash);
  assert(clauses);
  if (! hash_update(hash, type, sizeof(type)))
    return false;
  f = clauses;
  while (f) {
    count++;
    f = f->next_clause;
  }
  if (! hash_update_uw(hash, &count))
    return false;
  f = clauses;
  while (f) {
    if (! hash_update_list(hash, (const s_list * const *)
                           &f->pattern) ||
        ! hash_update_list(hash, (const s_list * const *)
                           &f->algo))
      return false;
    f = f->next_clause;
  }
  return true;
}

bool hash_update_ident (t_hash *hash, const s_ident *ident)
{
  const s8 type[] = "ident";
  assert(hash);
  assert(ident);
  if (! hash_update(hash, type, sizeof(type)))
    return false;
  if (ident->module) {
    if (! hash_update_sym(hash, &ident->module) ||
        ! hash_update_s8(hash, "."))
      return false;
  }
  return hash_update_sym(hash, &ident->sym);
}

bool hash_update_integer (t_hash *hash, const s_integer *i)
{
  int j = 0;
  mp_digit *digit;
  const s8 type[] = "integer";
  assert(hash);
  assert(i);
  digit = i->mp_int.dp;
  if (! hash_update(hash, type, sizeof(type)) ||
      ! hash_update(hash, &i->mp_int.used, sizeof(i->mp_int.used)))
    return false;
  while (j < i->mp_int.used) {
    if (! hash_update(hash, digit, sizeof(*digit)))
      return false;
    digit++;
    j++;
  }
  return true;
}

/* FIXME: circular lists */
bool hash_update_list (t_hash *hash, const s_list * const *list)
{
  uw count;
  const s_list *l;
  const s_list *last;
  const s8 type[] = "list";
  assert(hash);
  assert(list);
  l = *list;
  count = list_length(l);
  if (! hash_update(hash, type, sizeof(type)) ||
      ! hash_update_uw(hash, &count))
    return false;
  if (l) {
    while (l) {
      if (! hash_update_tag(hash, &l->tag))
        return false;
      last = l;
      l = list_next(l);
    }
    if (! hash_update_tag(hash, &last->next))
      return false;
  }
  return true;
}

bool hash_update_map (t_hash *hash, const s_map *map)
{
  uw i = 0;
  const s8 type[] = "map";
  assert(hash);
  assert(map);
  if (! hash_update(hash, type, strlen(type)) ||
      ! hash_update(hash, &map->count, sizeof(map->count)))
    return false;
  while (i < map->count) {
    if (! hash_update_tag(hash, map->key + i) ||
        ! hash_update_tag(hash, map->value + i))
      return false;
    i++;
  }
  return true;
}

bool hash_update_ptag (t_hash *hash, const p_tag *ptag)
{
  const s8 type[] = "ptag";
  assert(hash);
  assert(ptag);
  if (! hash_update(hash, type, sizeof(type)))
    return false;
  return hash_update(hash, ptag, sizeof(ptag));
}

bool hash_update_ptr (t_hash *hash, const u_ptr_w *ptr)
{
  const s8 type[] = "ptr";
  if (! hash_update(hash, type, strlen(type)))
    return false;
  return hash_update(hash, &ptr->p, sizeof(void *));
}

bool hash_update_ptr_free (t_hash *hash, const u_ptr_w *ptr_free)
{
  const s8 type[] = "ptr_free";
  if (! hash_update(hash, type, strlen(type)))
    return false;
  return hash_update(hash, &ptr_free->p, sizeof(void *));
}

bool hash_update_quote (t_hash *hash, const s_quote *x)
{
  const s8 type[] = "quote";
  if (! hash_update(hash, type, strlen(type)))
    return false;
  return hash_update_tag(hash, x->tag);
}

HASH_UPDATE_DEF(s8)

HASH_UPDATE_DEF(s16)

HASH_UPDATE_DEF(s32)

HASH_UPDATE_DEF(s64)

HASH_UPDATE_DEF(sw)

bool hash_update_str (t_hash *hash, const s_str *str)
{
  s8 type[] = "str";
  assert(hash);
  assert(str);
  return hash_update(hash, type, strlen(type)) &&
    hash_update(hash, &str->size, sizeof(str->size)) &&
    hash_update(hash, str->ptr.p, str->size);
}

bool hash_update_struct (t_hash *hash, const s_struct *s)
{
  const void *data;
  uw i = 0;
  const s_sym *sym;
  s8 type[] = "struct";
  assert(hash);
  assert(s);
  if (! hash_update(hash, type, sizeof(type)) ||
      ! hash_update_sym(hash, &s->type.module) ||
      ! hash_update(hash, &s->type.map.count,
                    sizeof(s->type.map.count)))
    return false;
  while (i < s->type.map.count) {
    if (! hash_update_tag(hash, s->type.map.key + i))
      return false;
    if (! tag_type(s->type.map.value + i, &sym))
      return false;
    if (s->data)
      data = (s8 *) s->data + s->type.offset[i];
    else {
      if (s->tag) {
        if (! tag_to_const_pointer(s->tag + i, sym, &data))
          return false;
      }
      else {
        if (! tag_to_const_pointer(s->type.map.value + i, sym, &data))
          return false;
      }
    }
    if (! void_hash_update(sym, hash, data))
      return false;
    i++;
  }
  return true;
}

bool hash_update_sym (t_hash *hash, const s_sym * const *sym)
{
  s8 type[] = "sym";
  assert(hash);
  assert(sym);
  return hash_update(hash, type, sizeof(type)) &&
    hash_update_str(hash, &(*sym)->str);
}

bool hash_update_tag (t_hash *hash, const s_tag *tag)
{
  u8 tag_type;
  s8 type[] = "tag";
  assert(hash);
  assert(tag);
  if (! hash_update(hash, type, strlen(type)))
    return false;
  tag_type = tag->type;
  if (! hash_update_u8(hash, &tag_type))
    return false;
  switch (tag->type) {
  case TAG_ARRAY: return hash_update_array(hash, &tag->data.array);
  case TAG_BOOL:  return hash_update_bool(hash, &tag->data.bool);
  case TAG_CALL:  return hash_update_call(hash, &tag->data.call);
  case TAG_CFN:   return hash_update_cfn(hash, &tag->data.cfn);
  case TAG_CHARACTER:
    return hash_update_character(hash, &tag->data.character);
  case TAG_F32:   return hash_update_f32(hash, &tag->data.f32);
  case TAG_F64:   return hash_update_f64(hash, &tag->data.f64);
  case TAG_FACT:  return hash_update_fact(hash, &tag->data.fact);
  case TAG_FN:    return hash_update_fn(hash, &tag->data.fn);
  case TAG_IDENT: return hash_update_ident(hash, &tag->data.ident);
  case TAG_INTEGER:
    return hash_update_integer(hash, &tag->data.integer);
  case TAG_LIST:
    return hash_update_list(hash, (const s_list * const *)
                            &tag->data.list);
  case TAG_MAP:    hash_update_map(hash, &tag->data.map);
  case TAG_PTAG:   hash_update_ptag(hash, &tag->data.ptag);
  case TAG_PTR:    hash_update_ptr(hash, &tag->data.ptr);
  case TAG_PTR_FREE:
    hash_update_ptr_free(hash, &tag->data.ptr_free);
  case TAG_QUOTE: return hash_update_quote(hash, &tag->data.quote);
  case TAG_S8:    return hash_update_s8(hash, &tag->data.s8);
  case TAG_S16:   return hash_update_s16(hash, &tag->data.s16);
  case TAG_S32:   return hash_update_s32(hash, &tag->data.s32);
  case TAG_S64:   return hash_update_s64(hash, &tag->data.s64);
  case TAG_SW:    return hash_update_sw(hash, &tag->data.sw);
  case TAG_STR:   return hash_update_str(hash, &tag->data.str);
  case TAG_STRUCT:
    return hash_update_struct(hash, &tag->data.struct_);
  case TAG_SYM:   return hash_update_sym(hash, &tag->data.sym);
  case TAG_TUPLE: return hash_update_tuple(hash, &tag->data.tuple);
  case TAG_U8:    return hash_update_u8(hash, &tag->data.u8);
  case TAG_U16:   return hash_update_u16(hash, &tag->data.u16);
  case TAG_U32:   return hash_update_u32(hash, &tag->data.u32);
  case TAG_U64:   return hash_update_u64(hash, &tag->data.u64);
  case TAG_UW:    return hash_update_uw(hash, &tag->data.uw);
  case TAG_VAR:   return hash_update_var(hash, NULL);
  case TAG_VOID:  return hash_update_void(hash, NULL);
  }
  warnx("hash_update_tag: unknown tag type: %d", tag->type);
  assert(! "hash_update_tag: unknown tag type");
  return false;
}

bool hash_update_tuple (t_hash *hash, const s_tuple *tuple)
{
  uw i = 0;
  assert(tuple);
  if (! hash_update(hash, &tuple->count, sizeof(tuple->count)))
    return false;
  while (i < tuple->count) {
    if (! hash_update_tag(hash, tuple->tag + i))
      return false;
    i++;
  }
  return true;
}

HASH_UPDATE_DEF(u8)
HASH_UPDATE_DEF(u16)
HASH_UPDATE_DEF(u32)
HASH_UPDATE_DEF(u64)
HASH_UPDATE_DEF(uw)

bool hash_update_var (t_hash *hash, const void *x)
{
  s8 type[] = "var";
  (void) x;
  assert(hash);
  return hash_update(hash, type, strlen(type));
}

bool hash_update_void (t_hash *hash, const void *x)
{
  s8 type[] = "void";
  (void) x;
  assert(hash);
  return hash_update(hash, type, strlen(type));
}
