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
#include "tag_type.h"

#define HASH_UPDATE_DEF(type)                                          \
  void hash_update_##type (t_hash *hash, const type *x)                \
  {                                                                    \
    const s8 t[] = #type;                                              \
    assert(hash);                                                      \
    assert(x);                                                         \
    hash_update(hash, t, sizeof(t));                                   \
    hash_update(hash, x, sizeof(type));                                \
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

void hash_update (t_hash *hash, const void *data, uw size)
{
  assert(hash);
  assert(data);
  SHA1Update(hash, data, size);
}

void hash_update_1 (t_hash *hash, const s8 *p)
{
  uw len;
  const s8 type[] = "s8*";
  assert(hash);
  assert(p);
  hash_update(hash, type, sizeof(type));
  len = strlen(p);
  hash_update(hash, &len, sizeof(len));
  hash_update(hash, p, strlen(p));
}

void hash_update_array (t_hash *hash, const s_array *a)
{
  uw i = 0;
  const s8 type[] = "array";
  assert(hash);
  assert(a);
  hash_update(hash, type, sizeof(type));
  hash_update(hash, &a->dimension, sizeof(a->dimension));
  hash_update(hash, &a->type, sizeof(a->type));
  while (i < a->dimension) {
    hash_update(hash, &a->dimensions[i].count,
                sizeof(a->dimensions[i].count));
    i++;
  }
  hash_update(hash, a->data, a->size);
}

void hash_update_bool (t_hash *hash, const bool *x)
{
  bool b;
  const s8 type[] = "bool";
  assert(hash);
  assert(x);
  hash_update(hash, type, sizeof(type));
  b = x ? 1 : 0;
  hash_update(hash, &b, sizeof(b));
}

void hash_update_call (t_hash *hash, const s_call *call)
{
  const s8 type[] = "call";
  assert(hash);
  assert(call);
  hash_update(hash, type, sizeof(type));
  hash_update_ident(hash, &call->ident);
  hash_update_list(hash, (const s_list * const *) &call->arguments);
}

void hash_update_cfn (t_hash *hash, const s_cfn *cfn)
{
  const s8 type[] = "cfn";
  assert(hash);
  assert(cfn);
  hash_update(hash, type, sizeof(type));
  hash_update_sym(hash, &cfn->name);
  hash_update_list(hash, (const s_list * const *) &cfn->arg_types);
}

HASH_UPDATE_DEF(character)

HASH_UPDATE_DEF(f32)

HASH_UPDATE_DEF(f64)

void hash_update_fact (t_hash *hash, const s_fact *fact)
{
  const s8 type[] = "fact";
  assert(hash);
  assert(fact);
  assert(fact->subject);
  assert(fact->predicate);
  assert(fact->object);
  hash_update(hash, type, sizeof(type));
  hash_update_tag(hash, fact->subject);
  hash_update_tag(hash, fact->predicate);
  hash_update_tag(hash, fact->object);
}

void hash_update_fn (t_hash *hash, const s_fn *fn)
{
  const s8 type[] = "fn";
  assert(hash);
  assert(fn);
  hash_update(hash, type, sizeof(type));
  hash_update_bool(hash, &fn->macro);
  hash_update_bool(hash, &fn->special_operator);
  hash_update_fn_clauses(hash, fn->clauses);
}

void hash_update_fn_clauses (t_hash *hash, const s_fn_clause *clauses)
{
  uw count = 0;
  const s_fn_clause *f;
  const s8 type[] = "fn_clauses";
  assert(hash);
  assert(clauses);
  hash_update(hash, type, sizeof(type));
  f = clauses;
  while (f) {
    count++;
    f = f->next_clause;
  }
  hash_update_uw(hash, &count);
  f = clauses;
  while (f) {
    hash_update_list(hash, (const s_list * const *) &f->pattern);
    hash_update_list(hash, (const s_list * const *) &f->algo);
    f = f->next_clause;
  }
}

void hash_update_ident (t_hash *hash, const s_ident *ident)
{
  assert(hash);
  assert(ident);
  hash_update_s8(hash, "_");
  if (ident->module) {
    hash_update_sym(hash, &ident->module);
    hash_update_s8(hash, ".");
  }
  hash_update_sym(hash, &ident->sym);
}

void hash_update_integer (t_hash *hash, const s_integer *i)
{
  int j = 0;
  mp_digit *digit;
  const s8 type[] = "integer";
  assert(hash);
  assert(i);
  digit = i->mp_int.dp;
  hash_update(hash, type, 7);
  hash_update(hash, &i->mp_int.used, sizeof(i->mp_int.used));
  while (j < i->mp_int.used) {
    hash_update(hash, digit, sizeof(*digit));
    digit++;
    j++;
  }
}

/* FIXME: circular lists */
void hash_update_list (t_hash *hash, const s_list * const *list)
{
  uw count;
  const s_list *l;
  const s_list *last;
  const s8 type[] = "list";
  assert(hash);
  assert(list);
  l = *list;
  hash_update(hash, type, sizeof(type));
  count = list_length(l);
  hash_update_uw(hash, &count);
  if (l) {
    while (l) {
      hash_update_tag(hash, &l->tag);
      last = l;
      l = list_next(l);
    }
    hash_update_tag(hash, &last->next);
  }
}

void hash_update_map (t_hash *hash, const s_map *map)
{
  uw i = 0;
  const s8 type[] = "map";
  assert(hash);
  assert(map);
  hash_update(hash, type, strlen(type));
  hash_update(hash, &map->count, sizeof(map->count));
  while (i < map->count) {
    hash_update_tag(hash, map->key + i);
    hash_update_tag(hash, map->value + i);
    i++;
  }
}

void hash_update_ptag (t_hash *hash, const p_tag *ptag)
{
  const s8 type[] = "ptag";
  assert(hash);
  assert(ptag);
  hash_update(hash, type, sizeof(type));
  hash_update(hash, ptag, sizeof(ptag));
}

void hash_update_ptr (t_hash *hash, const u_ptr_w *ptr)
{
  const s8 type[] = "ptr";
  hash_update(hash, type, strlen(type));
  hash_update(hash, &ptr->p, sizeof(void *));
}

void hash_update_ptr_free (t_hash *hash, const u_ptr_w *ptr_free)
{
  const s8 type[] = "ptr_free";
  hash_update(hash, type, strlen(type));
  hash_update(hash, &ptr_free->p, sizeof(void *));
}

void hash_update_quote (t_hash *hash, const s_quote *x)
{
  const s8 type[] = "quote";
  hash_update(hash, type, strlen(type));
  hash_update_tag(hash, x->tag);
}

HASH_UPDATE_DEF(s8)

HASH_UPDATE_DEF(s16)

HASH_UPDATE_DEF(s32)

HASH_UPDATE_DEF(s64)

HASH_UPDATE_DEF(sw)

void hash_update_str (t_hash *hash, const s_str *str)
{
  s8 type[] = "str";
  assert(hash);
  assert(str);
  hash_update(hash, type, strlen(type));
  hash_update(hash, &str->size, sizeof(str->size));
  hash_update(hash, str->ptr.p, str->size);
}

void hash_update_struct (t_hash *hash, const s_struct *s)
{
  f_hash_update hash_update_value;
  uw i = 0;
  s8 type[] = "struct";
  assert(hash);
  assert(s);
  hash_update(hash, type, sizeof(type));
  hash_update(hash, &s->type.map.count, sizeof(s->type.map.count));
  while (i < s->type.map.count) {
    hash_update_tag(hash, s->type.map.key + i);
    if (! tag_type_to_hash_update(s->type.map.value[i].type,
                                  &hash_update_value))
      exit(1);
    hash_update_value(hash, (s8 *) s->data + s->type.offset[i]);
    i++;
  }
}

void hash_update_sym (t_hash *hash, const s_sym * const *sym)
{
  s8 type[] = "sym";
  assert(hash);
  assert(sym);
  hash_update(hash, type, sizeof(type));
  hash_update_str(hash, &(*sym)->str);
}

void hash_update_tag (t_hash *hash, const s_tag *tag)
{
  u8 tag_type;
  s8 type[] = "tag";
  assert(hash);
  assert(tag);
  hash_update(hash, type, strlen(type));
  tag_type = tag->type;
  hash_update_u8(hash, &tag_type);
  switch (tag->type) {
  case TAG_ARRAY: hash_update_array(hash, &tag->data.array);     break;
  case TAG_BOOL: hash_update_bool(hash, &tag->data.bool);        break;
  case TAG_CALL: hash_update_call(hash, &tag->data.call);        break;
  case TAG_CFN: hash_update_cfn(hash, &tag->data.cfn);           break;
  case TAG_CHARACTER:
    hash_update_character(hash, &tag->data.character);           break;
  case TAG_F32: hash_update_f32(hash, &tag->data.f32);           break;
  case TAG_F64: hash_update_f64(hash, &tag->data.f64);           break;
  case TAG_FACT: hash_update_fact(hash, &tag->data.fact);        break;
  case TAG_FN: hash_update_fn(hash, &tag->data.fn);              break;
  case TAG_IDENT: hash_update_ident(hash, &tag->data.ident);     break;
  case TAG_INTEGER:
    hash_update_integer(hash, &tag->data.integer);               break;
  case TAG_LIST:
    hash_update_list(hash, (const s_list * const *) &tag->data.list);
    break;
  case TAG_MAP: hash_update_map(hash, &tag->data.map);           break;
  case TAG_PTAG: hash_update_ptag(hash, &tag->data.ptag);        break;
  case TAG_PTR: hash_update_ptr(hash, &tag->data.ptr);           break;
  case TAG_PTR_FREE:
    hash_update_ptr_free(hash, &tag->data.ptr_free);             break;
  case TAG_QUOTE: hash_update_quote(hash, &tag->data.quote);     break;
  case TAG_S8: hash_update_s8(hash, &tag->data.s8);              break;
  case TAG_S16: hash_update_s16(hash, &tag->data.s16);           break;
  case TAG_S32: hash_update_s32(hash, &tag->data.s32);           break;
  case TAG_S64: hash_update_s64(hash, &tag->data.s64);           break;
  case TAG_SW: hash_update_sw(hash, &tag->data.sw);              break;
  case TAG_STR: hash_update_str(hash, &tag->data.str);           break;
  case TAG_STRUCT: hash_update_struct(hash, &tag->data.struct_); break;
  case TAG_SYM: hash_update_sym(hash, &tag->data.sym);           break;
  case TAG_TUPLE: hash_update_tuple(hash, &tag->data.tuple);     break;
  case TAG_U8: hash_update_u8(hash, &tag->data.u8);              break;
  case TAG_U16: hash_update_u16(hash, &tag->data.u16);           break;
  case TAG_U32: hash_update_u32(hash, &tag->data.u32);           break;
  case TAG_U64: hash_update_u64(hash, &tag->data.u64);           break;
  case TAG_UW: hash_update_uw(hash, &tag->data.uw);              break;
  case TAG_VAR: hash_update_var(hash, NULL);                     break;
  case TAG_VOID: hash_update_void(hash, NULL);                   break;
  }
}

void hash_update_tuple (t_hash *hash, const s_tuple *tuple)
{
  uw i = 0;
  assert(tuple);
  hash_update(hash, &tuple->count, sizeof(tuple->count));
  while (i < tuple->count) {
    hash_update_tag(hash, tuple->tag + i);
    i++;
  }
}

HASH_UPDATE_DEF(u8)
HASH_UPDATE_DEF(u16)
HASH_UPDATE_DEF(u32)
HASH_UPDATE_DEF(u64)
HASH_UPDATE_DEF(uw)

void hash_update_var (t_hash *hash, const void *x)
{
  s8 type[] = "var";
  (void) x;
  assert(hash);
  hash_update(hash, type, strlen(type));
}

void hash_update_void (t_hash *hash, const void *x)
{
  s8 type[] = "void";
  (void) x;
  assert(hash);
  hash_update(hash, type, strlen(type));
}
