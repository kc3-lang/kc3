/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "type.h"

void     type_clean (s_type *t);
s_type * type_copy (const s_type *t, s_type *dest);
s_type * type_init (s_type *t, const s_ident *ident);
sw       type_size (const s_type *t);
bool     type_to_tag_type (const s_type *t, e_tag_type *tag_type);
