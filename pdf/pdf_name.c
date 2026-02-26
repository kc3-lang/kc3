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
#include "pdf_name.h"

p_pdf_name_list g_pdf_name_list = NULL;

p_pdf_name pdf_name_1 (p_pdf_name_list *name_list, const char *pchar)
{
  s_str str;
  str_init_1(&str, NULL, pchar);
  return pdf_name_from_str(name_list, &str);
}

void kc3_pdf_name_list_delete_all (void)
{
  pdf_name_list_delete_all(&g_pdf_name_list);
}

p_pdf_name pdf_name_find (p_pdf_name_list *name_list,
                          const s_str *str)
{
  p_pdf_name_list list;
  assert(name_list);
  assert(str);
  list = *name_list;
  while (list) {
    p_pdf_name name = list->sym;
    if (compare_str(str, &name->str) == 0)
      return name;
    list = list->next;
  }
  return NULL;
}

p_pdf_name pdf_name_from_str (p_pdf_name_list *name_list,
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

void pdf_name_list_delete_all (p_pdf_name_list *name_list)
{
  p_pdf_name_list list;
  p_pdf_name_list next;
  assert(name_list);
  list = *name_list;
  while (list) {
    next = list->next;
    sym_delete(list->free_sym);
    alloc_free(list);
    list = next;
  }
  *name_list = NULL;
}

p_pdf_name pdf_name_new (p_pdf_name_list *name_list,
                         const s_str *str)
{
  s_sym *sym;
  s_sym_list *tmp;
  if (! (sym = alloc(sizeof(s_sym))))
    return NULL;
  if (! (tmp = alloc(sizeof(s_sym_list)))) {
    alloc_free(sym);
    return NULL;
  }
  str_init_copy(&sym->str, str);
  tmp->sym = sym;
  tmp->free_sym = sym;
  tmp->next = *name_list;
  *name_list = tmp;
  return sym;

}
