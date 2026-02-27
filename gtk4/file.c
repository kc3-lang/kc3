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
#include "file.h"

GFile ** kc3_gtk4_file_new_for_uri (GFile **dest,
                                    const s_str *uri)
{
  GFile *tmp;
  if (! (tmp = g_file_new_for_uri(uri->ptr.p_pchar)))
    return NULL;
  *dest = tmp;
  return dest;
}
