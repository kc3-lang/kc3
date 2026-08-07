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
#include "../libkc3/kc3.h"
#include "../libkc3/sym_ht.h"
#include "pdf_name.h"

#define PDF_NAME_LIST_SIZE 1024

s_pdf_name_list g_pdf_name_list = {0};

p_pdf_name pdf_name_1 (s_pdf_name_list *name_list, const char *pchar)
{
  s_str str;
  str_init_1(&str, NULL, pchar);
  return pdf_name_from_str(name_list, &str);
}

void kc3_pdf_name_list_delete_all (void)
{
  pdf_name_list_delete_all(&g_pdf_name_list);
}

p_pdf_name pdf_name_find (s_pdf_name_list *name_list,
                          const s_str *str)
{
  assert(name_list);
  assert(str);
  if (! name_list->ht)
    return NULL;
  return sym_ht_find(name_list->ht, str);
}

p_pdf_name pdf_name_from_str (s_pdf_name_list *name_list,
                              const s_str *str)
{
  p_pdf_name name;
  assert(str);
  if (! name_list)
    name_list = &g_pdf_name_list;
  if (! (name = pdf_name_find(name_list, str)))
    name = pdf_name_new(name_list, str);
  return name;
}

void pdf_name_list_delete_all (s_pdf_name_list *name_list)
{
  assert(name_list);
  if (name_list->ht)
    sym_ht_delete(name_list->ht);
  name_list->ht = NULL;
}

s_pdf_name_list * pdf_name_list_init (s_pdf_name_list *name_list)
{
  s_pdf_name_list tmp = {0};
  assert(name_list);
  if (! (tmp.ht = sym_ht_new(PDF_NAME_LIST_SIZE)))
    return NULL;
  *name_list = tmp;
  return name_list;
}

p_pdf_name pdf_name_new (s_pdf_name_list *name_list,
                         const s_str *str)
{
  s_sym *sym;
  p_pdf_name tmp;
  assert(name_list);
  assert(str);
  if (! name_list->ht && ! pdf_name_list_init(name_list))
    return NULL;
  if (! (sym = alloc(sizeof(s_sym))))
    return NULL;
  if (! str_init_copy(&sym->str, str)) {
    alloc_free(sym);
    return NULL;
  }
  if (! (tmp = sym_ht_register(name_list->ht, sym, sym))) {
    sym_delete(sym);
    return NULL;
  }
  return tmp;
}
