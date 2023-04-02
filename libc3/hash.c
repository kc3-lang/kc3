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

#define HASH_UPDATE_DEF(type)                                          \
  void hash_update_##type (t_hash *hash, type x)                       \
  {                                                                    \
    const s8 t[] = #type;                                              \
    assert(hash);                                                      \
    assert(x);                                                         \
    hash_update(hash, t, sizeof(t));                                   \
    hash_update(hash, &x, sizeof(x));                                  \
  }

void hash_clean (t_hash *hash)
{
  bzero(hash, sizeof(t_hash));
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
  const s8 type[] = "array";
  assert(hash);
  assert(a);
  hash_update(hash, type, sizeof(type));
  
}

void hash_update_bool (t_hash *hash, e_bool x)
{
  bool b = x ? 1 : 0;
  hash_update(hash, &b, sizeof(b));
}


void hash_update_call (t_hash *hash, const s_call *call)
{
  assert(hash);
  assert(call);
  hash_update_ident(hash, &call->ident);
  hash_update_list(hash, call->arguments);
}

void hash_update_cfn (t_hash *hash, const s_cfn *cfn)
{
  const s8 type[] = "cfn";
  assert(hash);
  assert(cfn);
  hash_update(hash, type, sizeof(type));
  hash_update_str(hash, &cfn->name);
  hash_update_list(hash, cfn->arg_types);
}

HASH_UPDATE_DEF(character)

HASH_UPDATE_DEF(f32)

HASH_UPDATE_DEF(f64)

void hash_update_fact (t_hash *hash, const s_fact *fact)
{
  const u8 type = 3;
  assert(hash);
  assert(fact);
  hash_update(hash, &type, sizeof(type));
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
  while (fn) {
    hash_update_list(hash, fn->pattern);
    hash_update_list(hash, fn->algo);
    fn = fn->next_clause;
  }
}

void hash_update_ident (t_hash *hash, const s_ident *ident)
{
  assert(hash);
  assert(ident);
  hash_update_u8(hash, '_');
  hash_update_sym(hash, ident->sym);
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
void hash_update_list (t_hash *hash, const s_list *list)
{
  const s_list *last;
  const s8 type[] = "list";
  hash_update(hash, type, sizeof(type));
  if (list) {
    while (list) {
      hash_update_tag(hash, &list->tag);
      last = list;
      list = list_next(list);
    }
    hash_update_tag(hash, &last->next);
  }
}

void hash_update_ptag (t_hash *hash, const p_tag ptag)
{
  const s8 type[] = "ptag";
  hash_update(hash, type, strlen(type));
  hash_update(hash, &ptag, sizeof(ptag));
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

void hash_update_str (t_hash *hash, const s_str *str)
{
  s8 type[] = "str";
  assert(hash);
  assert(str);
  hash_update(hash, type, strlen(type));
  hash_update(hash, &str->size, sizeof(str->size));
  hash_update(hash, str->ptr.p, str->size);
}

void hash_update_sym (t_hash *hash, const s_sym *sym)
{
  assert(hash);
  assert(sym);
  hash_update_u8(hash, ':');
  hash_update_str(hash, &sym->str);
}

void hash_update_tag (t_hash *hash, const s_tag *tag)
{
  assert(hash);
  assert(tag);
  hash_update_u64(hash, tag->type.type);
  switch (tag->type.type) {
  case TAG_VOID: break;
  case TAG_BOOL: hash_update_bool(hash, tag->data.bool);       break;
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    hash_update_call(hash, &tag->data.call);                   break;
  case TAG_CFN: hash_update_cfn(hash, &tag->data.cfn);         break;
  case TAG_CHARACTER:
    hash_update_character(hash, tag->data.character);          break;
  case TAG_F32: hash_update_f32(hash, tag->data.f32);          break;
  case TAG_F64: hash_update_f64(hash, tag->data.f64);          break;
  case TAG_FN: hash_update_fn(hash, tag->data.fn);             break;
  case TAG_IDENT: hash_update_ident(hash, &tag->data.ident);   break;
  case TAG_INTEGER:
    hash_update_integer(hash, &tag->data.integer);             break;
  case TAG_LIST: hash_update_list(hash, tag->data.list);       break;
  case TAG_PTAG: hash_update_ptag(hash, tag->data.ptag);       break;
  case TAG_QUOTE: hash_update_quote(hash, &tag->data.quote);   break;
  case TAG_S8: hash_update_s8(hash, tag->data.s8);             break;
  case TAG_S16: hash_update_s16(hash, tag->data.s16);          break;
  case TAG_S32: hash_update_s32(hash, tag->data.s32);          break;
  case TAG_S64: hash_update_s64(hash, tag->data.s64);          break;
  case TAG_STR: hash_update_str(hash, &tag->data.str);         break;
  case TAG_SYM: hash_update_sym(hash, tag->data.sym);          break;
  case TAG_TUPLE: hash_update_tuple(hash, &tag->data.tuple);   break;
  case TAG_U8: hash_update_u8(hash, tag->data.u8);             break;
  case TAG_U16: hash_update_u16(hash, tag->data.u16);          break;
  case TAG_U32: hash_update_u32(hash, tag->data.u32);          break;
  case TAG_U64: hash_update_u64(hash, tag->data.u64);          break;
  case TAG_VAR:
    assert(! "var hash update");
    errx(1, "var hash update");
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
