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
#include "assert.h"
#include "compare.h"
#include "counter.h"
#include "ht.h"
#include "ident.h"
#include "io.h"
#include "mutex.h"
#include "sym.h"
#include "tag.h"

s_ht g_counter_ht = {0};

void counter_clean (s_counter *counter)
{
  assert(counter);
  tag_clean(&counter->count);
}

s_tag * counter_decrement (s_counter *counter, s_tag *positive,
                           s_tag *dest)
{
  s_tag tmp = {0};
  assert(counter);
  assert(positive);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(&counter->mutex);
#endif
  if (! tag_is_positive_integer(positive)) {
    err_puts("counter_decrement: tag_is_positive_integer");
    assert(! "counter_decrement: tag_is_positive_integer");
    goto clean;
  }
  if (! tag_sub(&counter->count, positive, &tmp)) {
    err_puts("counter_decrement: tag_sub");
    assert(! "counter_decrement: tag_sub");
    tag_clean(&tmp);
    goto clean;
  }
  tag_clean(&counter->count);
  if (! tag_init_copy(&counter->count, &tmp)) {
    tag_clean(&tmp);
    goto clean;
  }
#if HAVE_PTHREAD
  mutex_unlock(&counter->mutex);
#endif
  *dest = tmp;
  return dest;
 clean:
#if HAVE_PTHREAD
  mutex_unlock(&counter->mutex);
#endif
  return NULL;
}

void counter_delete (s_counter *counter)
{
  assert(counter);
  counter_clean(counter);
  free(counter);
}

void counter_ht_clean (s_ht *ht)
{
  if (! ht)
    ht = &g_counter_ht;
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
  return hash;
}

s_ht * counter_ht_init (s_ht *ht)
{
  s_ht tmp = {0};
  if (! ht)
    ht = &g_counter_ht;
  ht_init(&tmp, &g_sym_Ident, 1024);
  tmp.compare = counter_ht_compare;
  tmp.hash = counter_ht_hash;
  *ht = tmp;
  return ht;
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
  tag_init_copy(&tmp.count, value);
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
