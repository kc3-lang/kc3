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
#include "buf_parse.h"
#include "pvar.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

void pvar_clean (p_var *pvar)
{
  var_delete(*pvar);
}

p_var * pvar_init (p_var *pvar, const s_sym *type)
{
  p_var tmp = NULL;
  if (! (tmp = var_new(type)))
    return NULL;
  *pvar = tmp;
  return pvar;
}

p_var * pvar_init_1 (p_var *pvar, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  s_var *tmp = NULL;
  assert(pvar);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_pvar(&buf, &tmp);
  buf_clean(&buf);
  if (r < 0 || (uw) r != len) {
    err_puts("pvar_init_1: invalid var");
    assert(! "pvar_init_1: invalid var");
    return NULL;
  }
  *pvar = tmp;
  return pvar;
}

p_var * pvar_init_cast (p_var *pvar, const s_sym * const *type,
                        s_tag *src)
{
  p_var tmp = NULL;
  assert(pvar);
  assert(type);
  assert(src);
  if (*type == &g_sym_Var) {
    switch (src->type) {
    case TAG_PTR: tmp = src->data.ptr.p;              break;
    case TAG_S8:  tmp = (p_var) (uw) src->data.s8;  break;
    case TAG_S16: tmp = (p_var) (uw) src->data.s16; break;
    case TAG_S32: tmp = (p_var) (uw) src->data.s32; break;
    case TAG_S64: tmp = (p_var) (uw) src->data.s64; break;
    case TAG_SW:  tmp = (p_var) (uw) src->data.sw;  break;
    case TAG_U8:  tmp = (p_var) (uw) src->data.u8;  break;
    case TAG_U16: tmp = (p_var) (uw) src->data.u16; break;
    case TAG_U32: tmp = (p_var) (uw) src->data.u32; break;
    case TAG_U64: tmp = (p_var) (uw) src->data.u64; break;
    case TAG_UW:  tmp = (p_var) src->data.uw;       break;
    default: goto invalid_cast;
    }
    *pvar = tmp;
    return pvar;
  }
  if (! pvar_init(&tmp, *type))
    return NULL;
  if (! tag_init_copy(&tmp->tag, src))
    return NULL;
  tmp->bound = true;
  *pvar = tmp;
  return pvar;
 invalid_cast:
  err_puts("pvar_init_cast: cannot cast to Var");
  assert(! "pvar_init_cast: cannot cast to Var");
  return NULL;
}

p_var * pvar_init_copy (p_var *pvar, p_var *src)
{
  s_var *tmp = NULL;
  tmp = var_new_ref(*src);
  if (! tmp)
    return NULL;
  *pvar = tmp;
  return pvar;
}
