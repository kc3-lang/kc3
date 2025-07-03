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
#include <string.h>
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "callable.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "fn.h"
#include "ident.h"
#include "list.h"
#include "pcallable.h"
#include "tag.h"

sw call_arity (const s_call *call)
{
  return list_length(call->arguments);
}

void call_clean (s_call *call)
{
  assert(call);
  list_delete_all(call->arguments);
  if (call->pcallable)
    pcallable_clean(&call->pcallable);
}

bool call_get (s_call *call)
{
  return env_call_get(env_global(), call);
}

s_call * call_init (s_call *call)
{
  assert(call);
  *call = (s_call) {0};
  return call;
}

s_call * call_init_1 (s_call *call, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_call(&buf, call);
  if (r < 0 || (uw) r != len) {
    err_write_1("call_init_1: invalid call: ");
    err_write_1(p);
    err_write_1(": ");
    err_inspect_sw(&r);
    err_write_1(" != ");
    err_inspect_uw(&len);
    err_write_1("\n");
    return NULL;
  }
  return call;
}

s_call * call_init_call_cast (s_call *call, const s_sym *type)
{
  s_list *next;
  s_call tmp = {0};
  tmp.ident.module = type;
  tmp.ident.sym = &g_sym_cast;
  next = list_new(NULL);
  if (! next)
    return NULL;
  tmp.arguments = list_new(next);
  if (! tmp.arguments) {
    list_delete_all(next);
    return NULL;
  }
  tag_init_sym(&tmp.arguments->tag, type);
  *call = tmp;
  return call;
}

s_call * call_init_cast (s_call *call, const s_sym * const *type,
                         s_tag *tag)
{
  assert(call);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_CALL:
    return call_init_copy(call, &tag->data.call);
  default:
    break;
  }
  err_write_1("call_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Call)
    err_puts(" to Call");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka Call");
  }
  assert(! "call_init_cast: cannot cast to Call");
  return NULL;
}

s_call * call_init_copy (s_call *call, s_call *src)
{
  s_call tmp = {0};
  assert(src);
  assert(call);
  if (! ident_init_copy(&tmp.ident, &src->ident) ||
      ! list_init_copy(&tmp.arguments, &src->arguments))
    return NULL;
  if (src->pcallable &&
      ! (tmp.pcallable = callable_new_copy(src->pcallable))) {
    list_delete_all(tmp.arguments);
    return NULL;
  }
  *call = tmp;
  return call;
}

s_call * call_init_op (s_call *call)
{
  s_list *arg;
  s_call tmp = {0};
  assert(call);
  arg = list_new(NULL);
  if (! arg)
    return NULL;
  tmp.arguments = list_new(arg);
  if (! tmp.arguments) {
    list_delete(arg);
    return NULL;
  }
  *call = tmp;
  return call;
}

s_call * call_init_op_unary (s_call *call)
{
  s_call tmp = {0};
  assert(call);
  tmp.arguments = list_new(NULL);
  *call = tmp;
  return call;
}
