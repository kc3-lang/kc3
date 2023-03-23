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
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "ident.h"
#include "list.h"

void call_clean (s_call *call)
{
  assert(call);
  list_delete_all(call->arguments);
}

s_call * call_copy (const s_call *src, s_call *dest)
{
  assert(src);
  assert(dest);
  ident_copy(&src->ident, &dest->ident);
  list_copy(src->arguments, &dest->arguments);
  dest->cfn = src->cfn;
  dest->fn = src->fn;
  return dest;
}

s_call * call_init (s_call *call)
{
  assert(call);
  bzero(call, sizeof(s_call));
  return call;
}

s_call * call_init_1 (s_call *call, const s8 *p)
{
  s_buf buf;
  buf_init_1(&buf, p);
  if (buf_parse_call(&buf, call) != (sw) strlen(p))
    errx(1, "invalid call: %s", p);
  buf_clean(&buf);
  return call;
}

s_call * call_init_op (s_call *call)
{
  assert(call);
  bzero(call, sizeof(s_call));
  call->arguments = list_new(list_new(NULL));
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
