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
#include "buf.h"
#include "buf_inspect.h"
#include "character.h"
#include "str.h"
#include "sym.h"

e_bool ident_character_is_reserved (character c)
{
  return (character_is_space(c) ||
          c == '#' ||
          c == '(' ||
          c == ')' ||
          c == ',' ||
          c == '.' ||
          c == ';' ||
          c == ']' ||
          c == '|' ||
          c == '}');
}

s_ident * ident_copy (const s_ident *src, s_ident *dest)
{
  dest->module_name = src->module_name;
  dest->sym = src->sym;
  return dest;
}

e_bool ident_first_character_is_reserved (character c)
{
  return (character_is_digit(c) ||
          character_is_uppercase(c) ||
          character_is_space(c) ||
          c == ':' ||
          c == '"');
}

e_bool ident_has_reserved_characters (const s_ident *ident)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, ident->sym->str.size, ident->sym->str.ptr.p);
  if ((r = str_read_character(&stra, &c)) > 0) {
    if (ident_first_character_is_reserved(c))
      return true;
    while ((r = str_read_character(&stra, &c)) > 0) {
      if (ident_character_is_reserved(c))
        return true;
    }
  }
  if (r < 0)
    return true;
  return false;
}

s_ident * ident_init (s_ident *ident, const s_sym *sym)
{
  assert(ident);
  assert(sym);
  ident->module_name = NULL;
  ident->sym = sym;
  return ident;
}

s_ident * ident_init_1 (s_ident *ident, const s8 *p)
{
  s_str tmp;
  str_init_1(&tmp, NULL, p);
  str_to_ident(&tmp, ident);
  return ident;
}

s_str * ident_inspect (const s_ident *ident, s_str *dest)
{
  sw r;
  sw size;
  s_buf buf;
  size = buf_inspect_ident_size(ident);
  if (size < 0)
    return NULL;
  buf_init_alloc(&buf, size);
  r = buf_inspect_ident(&buf, ident);
  if (r != size) {
    buf_clean(&buf);
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

void ident_resolve_module (s_ident *ident, const s_env *env)
{
  assert(env);
  assert(ident);
  if (! ident->module_name) {
    assert(env->current_module);
    ident->module_name = env->current_module->name;
  }
}

