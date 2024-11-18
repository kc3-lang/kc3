/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "hash.h"
#include "fact.h"
#include "sym.h"
#include "tag.h"

uw * fact_hash_uw (const s_fact *fact, uw *dest)
{
  t_hash hash;
  assert(fact);
  hash_init(&hash);
  if (! hash_update_fact(&hash, fact))
    return NULL;
  *dest = hash_to_uw(&hash);
  return dest;
}

s_fact * fact_init (s_fact *fact, s_tag *subject,
                    s_tag *predicate, s_tag *object)
{
  assert(fact);
  fact->subject = subject;
  fact->predicate = predicate;
  fact->object = object;
  fact->id = 0;
  return fact;
}

s_fact * fact_init_cast (s_fact *fact, const s_sym * const *type,
                         s_tag *tag)
{
  assert(fact);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_FACT:
    return fact_init_copy(fact, &tag->data.fact);
  default:
    break;
  }
  err_write_1("fact_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Fact)
    err_puts(" to Fact");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka Fact");
  }
  assert(! "fact_init_cast: cannot cast to Fact");
  return NULL;
}

s_fact * fact_init_copy (s_fact *fact, const s_fact *src)
{
  assert(src);
  assert(fact);
  *fact = *src;
  return fact;
}

void fact_r (s_fact_w *fact, s_fact *dest)
{
  s_fact tmp = {0};
  tmp.subject = &fact->subject;
  tmp.predicate = &fact->predicate;
  tmp.object = &fact->object;
  tmp.id = fact->id;
  *dest = tmp;
}

void fact_w_clean (s_fact_w *fact)
{
  tag_clean(&fact->subject);
  tag_clean(&fact->predicate);
  tag_clean(&fact->object);
}

s_fact_w * fact_w_eval (s_fact_w *fact, s_fact_w *dest)
{
  return env_fact_w_eval(&g_kc3_env, fact, dest);
}

s_fact_w * fact_w_init (s_fact_w *fact)
{
  s_fact_w tmp = {0};
  assert(fact);
  *fact = tmp;
  return fact;
}

s_fact_w * fact_w_init_cast (s_fact_w *fact, const s_sym * const *type,
                             s_tag *tag)
{
  assert(fact);
  assert(type);
  assert(*type);
  assert(tag);
  switch (tag->type) {
  case TAG_FACT:
    return fact_w_init_fact(fact, &tag->data.fact);
  default:
    break;
  }
  err_write_1("fact_w_init_cast: cannot cast from ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to ");
  if (*type == &g_sym_FactW)
    err_puts("FactW");
  else {
    err_inspect_sym(type);
    err_puts(" aka FactW");
  }
  assert(! "fact_w_init_cast: cannot cast");
  return NULL;
}

s_fact_w * fact_w_init_fact (s_fact_w *fact, s_fact *src)
{
  s_fact_w tmp = {0};
  if (! tag_init_copy(&tmp.subject, src->subject))
    return NULL;
  if (! tag_init_copy(&tmp.predicate, src->predicate)) {
    tag_clean(&tmp.subject);
    return NULL;
  }
  if (! tag_init_copy(&tmp.object, src->object)) {
    tag_clean(&tmp.predicate);
    tag_clean(&tmp.subject);
    return NULL;
  }
  *fact = tmp;
  return fact;
}

