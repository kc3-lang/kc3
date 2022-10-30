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
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "ident.h"
#include "list.h"

void call_clean (s_call *call)
{
  assert(call);
  list_delete(call->args);
}

s8 call_compare (const s_call *a, const s_call *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = ident_compare(&a->ident, &b->ident)))
    return r;
  return list_compare(a->args, b->args);
}

t_hash_context * call_hash_update (t_hash_context *context,
                                   const s_call *x)
{
  assert(x);
  ident_hash_update(context, &x->ident);
  return list_hash_update(context, x->args);
}

s_call * call_init_1 (s_call *call, const s8 *p)
{
  s_buf buf;
  buf_init_1(&buf, p);
  buf_parse_call(&buf, call);
  buf_clean(&buf);
  return call;
}

s_str * call_inspect (const s_call *call, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_call_size(call);
  if (size < 0) {
    assert(! "error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_call(&tmp, call);
  assert(tmp.wpos == tmp.size);
  return buf_to_str(&tmp, dest);
}
