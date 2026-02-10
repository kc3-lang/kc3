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
#include "address_of.h"
#include "assert.h"
#include "struct.h"
#include "sym.h"

s_pointer * address_of_struct (s_struct *s, const s_sym *key,
                               s_pointer *dest)
{
  uw i = 0;
  s_pointer tmp = {0};
  const s_sym *type;
  assert(s);
  assert(key);
  assert(dest);
  if (! struct_find_key_index(s, key, &i)) {
    err_puts("address_of_struct: struct_find_key_index");
    assert(! "address_of_struct: struct_find_key_index");
    return NULL;
  }
  if (! struct_get_var_type(s, key, &type)) {
    err_puts("address_of_struct: struct_get_var_type");
    assert(! "address_of_struct: struct_get_var_type");
    return NULL;
  }
  tmp.target_type = type;
  if (! (tmp.pointer_type = sym_target_to_pointer_type(type))) {
    err_puts("address_of_struct: sym_target_to_pointer_type");
    assert(! "address_of_struct: sym_target_to_pointer_type");
    return NULL;
  }
  tmp.ptr.p = (u8 *) s->data + s->pstruct_type->offset[i];
  *dest = tmp;
  return dest;
}
