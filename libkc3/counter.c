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
#include "alloc.h"
#include "assert.h"
#include "compare.h"
#include "counter.h"
#include "ht.h"
#include "ident.h"
#include "io.h"
#include "mutex.h"
#include "sym.h"
#include "tag.h"

void counter_clean (s_counter *counter)
{
  assert(counter);
  tag_clean(&counter->count);
#if HAVE_PTHREAD
  if (counter->mutex)
    mutex_delete(counter->mutex);
#endif
}

s_tag * counter_decrease (s_counter *counter, s_tag *positive,
                           s_tag *dest)
{
  s_tag tmp = {0};
  assert(counter);
  assert(positive);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(counter->mutex);
#endif
  if (! tag_is_positive_integer(positive)) {
    err_puts("counter_decrease: tag_is_positive_integer");
    assert(! "counter_decrease: tag_is_positive_integer");
    goto clean;
  }
  if (! tag_sub(&counter->count, positive, &tmp)) {
    err_puts("counter_decrease: tag_sub");
    assert(! "counter_decrease: tag_sub");
    tag_clean(&tmp);
    goto clean;
  }
  tag_clean(&counter->count);
  if (! tag_init_copy(&counter->count, &tmp)) {
    tag_clean(&tmp);
    goto clean;
  }
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  *dest = tmp;
  return dest;
 clean:
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  return NULL;
}

void counter_delete (s_counter *counter)
{
  assert(counter);
  counter_clean(counter);
  free(counter);
}

s_counter ** counter_find (const s_ident *ident, s_counter **dest)
{
  s_env *env;
  assert(ident);
  assert(dest);
  env = env_global();
  assert(env);
  return counter_ht_find(env->counter_ht, ident, dest);
}

s_tag * counter_get (s_counter *counter, s_tag *dest)
{
  s_tag *result;
  assert(counter);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(counter->mutex);
#endif
  result = tag_init_copy(dest, &counter->count);
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  return result;
}

void counter_ht_clean (s_ht *ht)
{
  ht_clean(ht);
}

s8 counter_ht_compare (const s_tag *a, const s_tag *b)
{
  s_counter *counter_a;
  s_counter *counter_b;
  if (! a ||
      a->type != TAG_PSTRUCT ||
      ! a->data.pstruct ||
      ! a->data.pstruct->pstruct_type ||
      a->data.pstruct->pstruct_type->module != &g_sym_Counter ||
      ! (counter_a = a->data.pstruct->data) ||
      ! b ||
      b->type != TAG_PSTRUCT ||
      ! b->data.pstruct ||
      ! b->data.pstruct->pstruct_type ||
      b->data.pstruct->pstruct_type->module != &g_sym_Counter ||
      ! (counter_b = b->data.pstruct->data)) {
    err_puts("counter_ht_compare: invalid argument");
    assert(! "counter_ht_compare: invalid argument");
    abort();
  }
  return compare_ident(&counter_a->ident, &counter_b->ident);
}

void counter_ht_delete (s_ht *ht)
{
  assert(ht);
  ht_delete(ht);
}

s_counter ** counter_ht_find (s_ht *ht, const s_ident *ident,
                              s_counter **dest)
{
  s_counter key = {0};
  s_tag     key_tag = {0};
  s_tag *value_tag = NULL;
  key.ident = *ident;
  if (! key.ident.module)
    key.ident.module = env_global()->current_defmodule;
  if (! key.ident.module)
    return NULL;
  if (! tag_init_pstruct_with_data(&key_tag, &g_sym_Counter, &key,
                                   false))
    return NULL;
  if (! ht_get(ht, &key_tag, &value_tag))
    return NULL;
  if (value_tag->type != TAG_PSTRUCT ||
      ! value_tag->data.pstruct ||
      ! value_tag->data.pstruct->pstruct_type ||
      value_tag->data.pstruct->pstruct_type->module != &g_sym_Counter ||
      ! value_tag->data.pstruct->data) {
    err_puts("counter_ht_find: ht_get: invalid value");
    assert(! "counter_ht_find: ht_get: invalid value");
    return NULL;
  }
  *dest = value_tag->data.pstruct->data;
  return dest;
}

uw counter_ht_hash (const s_tag *x)
{
  s_counter *counter;
  uw hash;
  if (! x ||
      x->type != TAG_PSTRUCT ||
      ! x->data.pstruct ||
      ! x->data.pstruct->pstruct_type ||
      x->data.pstruct->pstruct_type->module != &g_sym_Counter ||
      ! (counter = x->data.pstruct->data)) {
    err_puts("counter_ht_hash: invalid argument");
    assert(! "counter_ht_hash: invalid argument");
    abort();
  }
  ident_hash_uw(&counter->ident, &hash);
  if (false) {
    err_write_1("counter_ht_hash: ");
    err_inspect_uw_hexadecimal(hash);
    err_write_1("\n");
  }
  return hash;
}

s_ht * counter_ht_init (s_ht *ht)
{
  if (! ht_init(ht, &g_sym_Ident, 1024))
    return NULL;
  ht->compare = counter_ht_compare;
  ht->hash = counter_ht_hash;
  return ht;
}

s_ht * counter_ht_new (void)
{
  s_ht *ht;
  if (! (ht = alloc(sizeof(s_ht))))
    return NULL;
  if (! counter_ht_init(ht)) {
    free(ht);
    return NULL;
  }
  return ht;
}

s_tag * counter_increase (s_counter *counter, s_tag *positive,
                           s_tag *dest)
{
  s_tag tmp = {0};
  assert(counter);
  assert(positive);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(counter->mutex);
#endif
  if (! tag_is_positive_integer(positive)) {
    err_write_1("counter_increase: tag_is_positive_integer: ");
    err_inspect_tag(positive);
    err_write_1("\n");
    assert(! "counter_increase: tag_is_positive_integer");
    goto clean;
  }
  if (! tag_add(&counter->count, positive, &tmp)) {
    err_puts("counter_increase: tag_sub");
    assert(! "counter_increase: tag_sub");
    tag_clean(&tmp);
    goto clean;
  }
  tag_clean(&counter->count);
  if (! tag_init_copy(&counter->count, &tmp)) {
    tag_clean(&tmp);
    goto clean;
  }
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  *dest = tmp;
  return dest;
 clean:
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  return NULL;
}

s_counter * counter_init (s_counter *counter, s_ident *ident,
                          s_tag *value)
{
  s_counter tmp = {0};
  assert(counter);
  assert(ident);
  assert(ident->module);
  assert(ident->sym);
  assert(value);
  if (! tag_is_integer(value)) {
    err_puts("counter_init: value is not an integer");
    assert(! "counter_init: value is not an integer");
    return NULL;
  }
  tmp.ident = *ident;
  if (! tag_init_copy(&tmp.count, value))
    return NULL;
#if HAVE_PTHREAD
  if (! (tmp.mutex = mutex_new())) {
    tag_clean(&tmp.count);
    return NULL;
  }
#endif
  *counter = tmp;
  return counter;
}

s_counter * counter_new (s_ident *ident, s_tag *value)
{
  s_counter *counter = NULL;
  assert(ident);
  assert(ident->module);
  assert(ident->sym);
  assert(value);
  if (! (counter = alloc(sizeof(s_counter))))
    return NULL;
  if (! counter_init(counter, ident, value)) {
    free(counter);
    return NULL;
  }
  return counter;
}
