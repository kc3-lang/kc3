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
#include <string.h>
#include "alloc.h"
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
#include "mutex.h"
#include "pcallable.h"
#include "plist.h"
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
#if HAVE_PTHREAD
  mutex_clean(&call->mutex);
#endif
}

void call_delete (s_call *call)
{
  assert(call);
  if (call->ref_count <= 0) {
    err_puts("call_delete: invalid ref count");
    assert(! "call_delete: invalid ref count");
    return;
  }
#if HAVE_PTHREAD
  mutex_lock(&call->mutex);
#endif
  if (false) {
    err_write_1("call_delete: 0x");
    err_inspect_uw_hexadecimal((uw) call);
    err_write_1(" ref count = ");
    err_inspect_uw_decimal(call->ref_count);
    err_write_1("\n");
  }
  call->ref_count--;
  if (call->ref_count) {
#if HAVE_PTHREAD
    mutex_unlock(&call->mutex);
#endif
    return;
  }
#if HAVE_PTHREAD
  mutex_unlock(&call->mutex);
#endif
  call_clean(call);
  free(call);
}

bool call_get (s_call *call)
{
  return env_call_get(env_global(), call);
}

s_call * call_init (s_call *call)
{
  assert(call);
  *call = (s_call) {0};
  call->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&call->mutex);
#endif
  return call;
}

s_call * call_init_call_cast (s_call *call, const s_sym *type)
{
  s_list *next;
  s_call tmp;
  call_init(&tmp);
  tmp.ident.module = type;
  tmp.ident.sym = &g_sym_cast;
  next = list_new(NULL);
  if (! next) {
    call_clean(&tmp);
    return NULL;
  }
  tmp.arguments = list_new(next);
  if (! tmp.arguments) {
    call_clean(&tmp);
    list_delete_all(next);
    return NULL;
  }
  tag_init_psym(&tmp.arguments->tag, type);
  *call = tmp;
  return call;
}

s_call * call_init_copy (s_call *call, s_call *src)
{
  s_call tmp = {0};
  assert(src);
  assert(call);
  call_init(&tmp);
  if (! ident_init_copy(&tmp.ident, &src->ident) ||
      ! plist_init_copy(&tmp.arguments, &src->arguments)) {
    call_clean(&tmp);
    return NULL;
  }
  if (src->pcallable &&
      ! pcallable_init_copy(&tmp.pcallable, &src->pcallable)) {
    call_clean(&tmp);
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
  call_init(&tmp);
  arg = list_new(NULL);
  if (! arg) {
    call_clean(&tmp);
    return NULL;
  }
  tmp.arguments = list_new(arg);
  if (! tmp.arguments) {
    list_delete(arg);
    call_clean(&tmp);
    return NULL;
  }
  *call = tmp;
  return call;
}

s_call * call_init_op_unary (s_call *call)
{
  s_call tmp = {0};
  assert(call);
  call_init(&tmp);
  tmp.arguments = list_new(NULL);
  *call = tmp;
  return call;
}

s_call * call_new (void)
{
  s_call *call;
  if (! (call = alloc(sizeof(s_call))))
    return NULL;
  if (! call_init(call)) {
    free(call);
    return NULL;
  }
  return call;
}

s_call * call_new_copy (s_call *src)
{
  s_call *call;
  if (! (call = alloc(sizeof(s_call))))
    return NULL;
  if (! call_init_copy(call, src)) {
    free(call);
    return NULL;
  }
  return call;
}

s_call * call_new_ref (s_call *src)
{
  assert(src);
#if HAVE_PTHREAD
  mutex_lock(&src->mutex);
#endif
  if (src->ref_count <= 0) {
    err_puts("call_new_ref: invalid ref count");
    assert(! "call_new_ref: invalid ref count");
#if HAVE_PTHREAD
    mutex_unlock(&src->mutex);
#endif
    return NULL;
  }
  src->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&src->mutex);
#endif
  return src;
}
