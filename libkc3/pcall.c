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
#include "buf_parse.h"
#include "call.h"
#include "pcall.h"
#include "sym.h"
#include "tag_type.h"

void pcall_clean (p_call *pcall)
{
  assert(pcall);
  call_delete(*pcall);
}

s_ident * pcall_ident (p_call *pcall, s_ident *dest)
{
  if (! pcall || ! dest) {
    err_puts("pcall_ident: invalid argument");
    assert(! "pcall_ident: invalid argument");
    return NULL;
  }
  *dest = (*pcall)->ident;
  return dest;
}

p_call * pcall_init (p_call *pcall)
{
  p_call tmp = NULL;
  if (! (tmp = call_new()))
    return NULL;
  *pcall = tmp;
  return pcall;
}

p_call * pcall_init_1 (p_call *pcall, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_pcall(&buf, pcall);
  buf_clean(&buf);
  if (r < 0 || (uw) r != len) {
    err_write_1("call_init_1: invalid call: ");
    err_write_1(p);
    err_write_1(": ");
    err_inspect_sw(r);
    err_write_1(" != ");
    err_inspect_uw(len);
    err_write_1("\n");
    return NULL;
  }
  return pcall;
}

p_call * pcall_init_call_cast (p_call *pcall, const s_sym *type)
{
  p_call tmp;
  if (! (tmp = alloc(sizeof(s_call))))
    return NULL;
  if (! call_init_call_cast(tmp, type)) {
    alloc_free(tmp);
    return NULL;
  }
  *pcall = tmp;
  return pcall;
}

p_call * pcall_init_cast (p_call *pcall, p_sym *type, s_tag *tag)
{
  assert(pcall);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_PCALL:
    return pcall_init_copy(pcall, &tag->data.pcall);
  default:
    break;
  }
  err_write_1("call_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Call)
    err_puts(" to Call");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Call");
  }
  assert(! "call_init_cast: cannot cast to Call");
  return NULL;
}

p_call * pcall_init_copy (p_call *pcall, p_call *src)
{
  p_call tmp;
  assert(pcall);
  assert(src);
  if (env_global()->pass_by_copy) {
    if (! (tmp = call_new_copy(*src))) {
      err_puts("pcall_init_copy: call_new_copy");
      assert(! "pcall_init_copy: call_new_copy");
      return NULL;
    }
  }
  else {
    if (! (tmp = call_new_ref(*src))) {
      err_puts("pcall_init_copy: call_new_ref");
      assert(! "pcall_init_copy: call_new_ref");
      return NULL;
    }
  }
  *pcall = tmp;
  return pcall;
}

const s_sym ** pcall_sym (p_call *pcall, const s_sym **dest)
{
  if (! pcall || ! *pcall || ! (*pcall)->ident.sym || ! dest) {
    err_puts("pcall_sym: invalid argument");
    assert(! "pcall_sym: invalid argument");
    return NULL;
  }
  *dest = (*pcall)->ident.sym;
  return dest;
}
