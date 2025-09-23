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
#include "../libkc3/kc3.h"
#include "pdf_name.h"

typedef p_sym      p_pdf_name;
typedef p_sym_list p_pdf_name_list;

static p_pdf_name *g_pdf_name_list = NULL;

const p_pdf_name * pdf_name_find (p_pdf_name_list list,
                                  const s_str *str)
{
  s_pdf_name_list *name_list;
  name_list = g_pdf_name_list;
  while (name_list) {
    const s_pdf_name *pdf_name = name_list->name;
    if (compare_str(str, &name->str) == 0)
      return name;
    name_list = name_list->next;
  }
  return NULL;
}

const p_pdf_name * pdf_name_from_str (const s_str *str)
{
  const s_pdf_name *name;
  name = pdf_name_find(str);
  if (! name)
    name = pdf_name_new(str);
  return name;
}
