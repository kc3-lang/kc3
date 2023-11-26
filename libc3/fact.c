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
#include "buf.h"
#include "buf_inspect.h"
#include "hash.h"
#include "fact.h"
#include "tag.h"

uw fact_hash_uw (const s_fact *fact)
{
  t_hash hash;
  assert(fact);
  hash_init(&hash);
  hash_update_fact(&hash, fact);
  return hash_to_uw(&hash);
}

s_fact * fact_init (s_fact *fact, const s_tag *subject,
                    const s_tag *predicate, const s_tag *object)
{
  assert(fact);
  fact->subject = subject;
  fact->predicate = predicate;
  fact->object = object;
  fact->id = 0;
  return fact;
}

s_fact * fact_init_copy (s_fact *fact, const s_fact *src)
{
  assert(src);
  assert(fact);
  *fact = *src;
  return fact;
}

s_str * fact_inspect (const s_fact *fact, s_str *dest)
{
  s_buf buf;
  sw size;
  if ((size = buf_inspect_fact_size(fact)) < 0) {
    assert(! "fact_inspect: size error");
    errx(1, "fact_inspect: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_fact(&buf, fact) != size) {
    assert(! "fact_inspect: inspect error");
    errx(1, "fact_inspect: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_fact * fact_r (const s_fact_w *fact)
{
  return (s_fact *) fact;
}

void fact_w_clean (s_fact_w *fact)
{
  tag_delete(fact->subject);
  fact->subject = NULL;
  tag_delete(fact->predicate);
  fact->predicate = NULL;
  tag_delete(fact->object);
  fact->object = NULL;
}
