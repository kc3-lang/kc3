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
#ifndef KC3_PDF_NAME_H
#define KC3_PDF_NAME_H

#include "types.h"

extern p_pdf_name_list g_pdf_name_list;

/* Heap-allocation functions. */
void pdf_name_list_delete_all (p_pdf_name_list *name_list);

/* Operators. */
p_pdf_name pdf_name_1 (p_pdf_name_list *name_list, const char *pchar);
p_pdf_name pdf_name_find (p_pdf_name_list *name_list,
                          const s_str *str);
p_pdf_name pdf_name_from_str (p_pdf_name_list *name_list,
                              const s_str *str);
p_pdf_name pdf_name_new (p_pdf_name_list *name_list,
                         const s_str *str);

#endif /* KC3_PDF_NAME_H */
