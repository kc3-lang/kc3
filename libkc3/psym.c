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
#include "alloc.h"
#include "assert.h"
#include "list.h"
#include "psym.h"
#include "str.h"
#include "sym.h"
#include "tag_type.h"

uw g_psym_anon_serial = 0;

uw * psym_anon_serial (uw *dest)
{
  *dest = ++g_psym_anon_serial;
  return dest;
}

void psym_anon_serial_set (uw x)
{
  g_psym_anon_serial = x;
}

p_sym * psym_init_1 (p_sym *sym, const char *p)
{
  assert(sym);
  assert(p);
  *sym = sym_1(p);
  return sym;
}

p_sym * psym_init_anon (p_sym *sym, const s_str *prefix)
{
  s_list *list;
  uw     serial;
  s_str *serial_str = NULL;
  s_str str = {0};
  const s_sym *tmp = NULL;
  const s_str underscore = {{NULL}, 1, {"_"}};
  list = list_new(list_new(list_new(NULL)));
  list_next(list_next(list))->tag.type = TAG_STR;
  serial_str = &list_next(list_next(list))->tag.data.str;
  list_next(list)->tag.type = TAG_STR;
  list_next(list)->tag.data.str = underscore;
  list->tag.type = TAG_STR;
  str_init_copy(&list->tag.data.str, prefix);
  while (1) {
    psym_anon_serial(&serial);
    str_clean(serial_str);
    str_init_uw(serial_str, serial);
    if (! str_init_concatenate_list(&str, list)) {
      err_puts("sym_new_anon: str_init_concatenate_list");
      assert(! "sym_new_anon: str_init_concatenate_list");
      break;
    }
    if (! sym_find(&str)) {
      tmp = str_to_sym(&str);
      str_clean(&str);
      list_delete_all(list);
      *sym = tmp;
      return sym;
    }
    str_clean(&str);
  }
  str_clean(&str);
  list_delete_all(list);
  return NULL;
}

p_sym * psym_init_cast (p_sym *sym, const s_sym * const *type,
                        const s_tag *tag)
{
  assert(sym);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_PSYM:
    return psym_init_copy(sym, &tag->data.psym);
  case TAG_STR:
    return psym_init_str(sym, &tag->data.str);
  default:
    break;
  }
  err_write_1("psym_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Sym)
    err_puts(" to Sym");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Sym");
  }
  assert(! "psym_init_cast: cannot cast to Sym");
  return NULL;
}

p_sym * psym_init_copy (p_sym *sym, p_sym const *src)
{
  assert(src);
  assert(sym);
  *sym = *src;
  return sym;
}

p_sym * psym_init_str (p_sym *sym, const s_str *src)
{
  const s_sym *tmp = NULL;
  tmp = sym_find(src);
  if (! tmp)
    tmp = sym_new(src);
  if (! tmp)
    return NULL;
  *sym = tmp;
  return sym;
}

bool psym_type_is_integer (p_sym const *type)
{
  return sym_type_is_integer(*type);
}

uw * psym_type_size (p_sym const *type, uw *size)
{
  return sym_type_size(*type, size);
}
