/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
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
#include "u8.h"

s8 fact_compare (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = tag_compare(a->subject, b->subject)))
    return r;
  if ((r = tag_compare(a->predicate, b->predicate)))
    return r;
  r = tag_compare(a->object, b->object);
  return r;
}

s8 fact_compare_unbound_var_count (const s_fact *a,
                                   const s_fact *b)
{
  u8 ca;
  u8 cb;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  ca = 0;
  cb = 0;
  if (tag_is_unbound_var(a->subject))
    ca++;
  if (tag_is_unbound_var(a->predicate))
    ca++;
  if (tag_is_unbound_var(a->object))
    ca++;
  if (tag_is_unbound_var(b->subject))
    cb++;
  if (tag_is_unbound_var(b->predicate))
    cb++;
  if (tag_is_unbound_var(b->object))
    cb++;
  return u8_compare(ca, cb);
}

s8 fact_compare_pos (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = tag_compare(a->predicate, b->predicate)))
    return r;
  if ((r = tag_compare(a->object, b->object)))
    return r;
  r = tag_compare(a->subject, b->subject);
  return r;
}

s8 fact_compare_osp (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = tag_compare(a->object, b->object)))
    return r;
  if ((r = tag_compare(a->subject, b->subject)))
    return r;
  r = tag_compare(a->predicate, b->predicate);
  return r;
}

s_fact * fact_copy (const s_fact *src, s_fact *dest)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}

uw fact_hash (const s_fact *fact)
{
  t_hash_context context;
  assert(fact);
  hash_init(&context);
  fact_hash_update(&context, fact);
  return hash_result(&context);
}

t_hash_context * fact_hash_update (t_hash_context *context,
                                   const s_fact *fact)
{
  tag_hash_update(context, fact->subject);
  tag_hash_update(context, fact->predicate);
  return tag_hash_update(context, fact->object);
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
