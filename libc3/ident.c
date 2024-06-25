/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
  return env_ident_get(&g_c3_env, ident, dest);
}

bool ident_has_reserved_characters (const s_ident *ident)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, ident->sym->str.size, ident->sym->str.ptr.p);
  if ((r = str_read_character_utf8(&stra, &c)) > 0) {
    if (ident_first_character_is_reserved(c))
      return true;
    while ((r = str_read_character_utf8(&stra, &c)) > 0) {
      if (ident_character_is_reserved(c))
        return true;
    }
  }
  if (r < 0)
    return true;
  return false;
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
  s_str tmp;
  str_init_1(&tmp, NULL, p);
  str_to_ident(&tmp, ident);
  return ident;
}

s_ident * ident_init_cast (s_ident *ident, const s_sym * const *type,
                           const s_tag *tag)
{
  switch (tag->type) {
  case TAG_IDENT:
    *ident = tag->data.ident;
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
    err_inspect_sym(type);
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

bool * ident_is_special_operator (const s_ident *ident, bool *dest)
{
  return env_ident_is_special_operator(&g_c3_env, ident, dest);
}

s_ident * ident_resolve_module (const s_ident *ident, s_ident *dest)
{
  return env_ident_resolve_module(&g_c3_env, ident, dest);
}

bool ident_to_tag_type (const s_ident *ident, e_tag_type *dest)
{
  assert(ident);
  assert(dest);
  return sym_to_tag_type(ident->sym, dest);
}
