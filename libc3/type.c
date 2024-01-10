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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "type.h"

const s_sym * type_pointer (const s_sym *type)
{
  uw len;
  char *mem;
  s_str str;
  const s_sym *tmp;
  assert(type);
  len = type->str.size + 2;
  if (! (mem = malloc(len)))
    errx(1, "type_pointer: out of memory");
  memcpy(mem, type->str.ptr.pchar, type->str.size);
  memcpy(mem + type->str.size, "*", 2);
  str_init(&str, mem, len, mem);
  tmp = str_to_sym(&str);
  str_clean(&str);
  return tmp;
}
