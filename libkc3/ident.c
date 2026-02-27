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
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "character.h"
#include "env.h"
#include "facts.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "ident.h"
#include "module.h"
#include "str.h"
#include "sym.h"
#include "tag.h"

bool ident_character_is_reserved (character c)
{
  return (character_is_space(c) ||
          c == '#' ||
          c == '(' ||
          c == ')' ||
          c == ',' ||
          c == '.' ||
          c == ';' ||
          c == '[' ||
          c == ']' ||
          c == '{' ||
          c == '}');
}

bool ident_first_character_is_reserved (character c)
{
  return (character_is_digit(c) ||
          character_is_uppercase(c) ||
          character_is_space(c) ||
          c == '"' ||
          c == '\'' ||
          c == '(' ||
          c == ')' ||
          c == ':' ||
          c == '[' ||
          c == ']' ||
          c == '{' ||
          c == '}');
}

s_tag * ident_get (const s_ident *ident, s_tag *dest)
{
  return env_ident_get(env_global(), ident, dest);
}

uw * ident_hash_uw (const s_ident *ident, uw *dest)
{
  t_hash hash;
  assert(ident);
  hash_init(&hash);
  if (! hash_update_ident(&hash, ident))
    return NULL;
  *dest = hash_to_uw(&hash);
  return dest;
}

s_ident * ident_init (s_ident *ident, const s_sym *module,
                      const s_sym *sym)
{
  assert(ident);
  assert(sym);
  ident->module = module;
  ident->sym = sym;
  return ident;
}

s_ident * ident_init_1 (s_ident *ident, const char *p)
{
  s_str tmp = {0};
  str_init_1(&tmp, NULL, p);
  str_to_ident(&tmp, ident);
  return ident;
}

s_ident * ident_init_cast (s_ident *ident, const s_sym * const *type,
                           const s_tag *tag)
{
  switch (tag->type) {
  case TAG_IDENT:
    *ident = tag->data.td_ident;
    return ident;
  default:
    break;
  }
  err_write_1("ident_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Ident)
    err_puts(" to Ident");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Ident");
  }
  assert(! "ident_init_cast: cannot cast to Ident");
  return NULL;
}

s_ident * ident_init_copy (s_ident *ident, const s_ident *src)
{
  ident->module = src->module;
  ident->sym = src->sym;
  return ident;
}

bool * ident_is_special_operator (const s_ident *ident, bool *dest)
{
  return env_ident_is_special_operator(env_global(), ident, dest);
}

s_ident * ident_resolve_module (const s_ident *ident, s_ident *dest)
{
  return env_ident_resolve_module(env_global(), ident, dest);
}

bool ident_to_tag_type (const s_ident *ident, e_tag_type *dest)
{
  assert(ident);
  assert(dest);
  return sym_to_tag_type(ident->sym, dest);
}
