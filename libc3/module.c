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
#include <string.h>
#include "buf.h"
#include "character.h"
#include "compare.h"
#include "env.h"
#include "facts.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "file.h"
#include "module.h"
#include "tag.h"
#include "sym.h"

bool module_ensure_loaded (const s_sym *module)
{
  return env_module_ensure_loaded(&g_c3_env, module);
}

bool * module_has_ident (const s_sym *module, const s_ident *ident,
                         s_facts *facts, bool *dest)
{
  const s_fact *fact;
  s_tag tag_ident;
  s_tag tag_module_name;
  s_tag tag_operator;
  s_tag tag_symbol;
  tag_init_ident(&tag_ident, ident);
  tag_init_sym(  &tag_module_name, module);
  tag_init_sym(  &tag_operator, &g_sym_operator);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  if (! facts_find_fact_by_tags(facts, &tag_module_name,
                                &tag_symbol, &tag_ident, &fact))
    return NULL;
  if (! fact &&
      ! facts_find_fact_by_tags(facts, &tag_module_name,
                                &tag_operator, &tag_ident, &fact))
    return NULL;
  *dest = fact ? true : false;
  return dest;
}

bool * module_has_symbol (const s_sym *module, const s_sym *sym,
                          s_facts *facts, bool *dest)
{
  s_ident ident;
  ident.module = module;
  ident.sym = sym;
  return module_has_ident(module, &ident, facts, dest);
}

bool * module_is_loading (const s_sym *module, bool *dest)
{
  return env_module_is_loading(&g_c3_env, module, dest);
}

bool module_load (const s_sym *module)
{
  return env_module_load(&g_c3_env, module);
}

s_tag * module_load_time (const s_sym *module, s_tag *dest)
{
  return env_module_load_time(&g_c3_env, module, dest);
}

bool module_maybe_reload (const s_sym *module)
{
  return env_module_maybe_reload(&g_c3_env, module);
}

s_str * module_path (const s_sym *module, const s_str *prefix,
                     const char *ext, s_str *dest)
{
  character b = 0;
  character c;
  s_buf in;
  s_buf out;
  sw out_size;
  sw r;
  s_str *result;
  assert(dest);
  assert(module);
  buf_init_str(&in, false, (s_str *) &module->str);
  out_size = module_path_size(module, prefix, ext);
  buf_init_alloc(&out, out_size);
  if ((r = buf_write_str(&out, prefix)) < 0)
    goto error;
  while ((r = buf_read_character_utf8(&in, &c)) > 0) {
    if (c == '.')
      c = '/';
    else if (character_is_uppercase(c)) {
      if (character_is_lowercase(b)) {
        if ((r = buf_write_1(&out, "_")) < 0)
          goto error;
      }
      b = c;
      c = character_to_lower(c);
    }
    else
      b = c;
    if ((r = buf_write_character_utf8(&out, c)) < 0)
      goto error;
  }
  if ((r = buf_write_1(&out, ext)) < 0)
    goto error;
  result = buf_to_str(&out, dest);
  return result;
 error:
  buf_clean(&out);
  err_puts("module_path: error");
  assert(! "module_path: error");
  return NULL;
}

sw module_path_size (const s_sym *module, const s_str *prefix, const char *ext)
{
  character b = 0;
  character c;
  s_buf in;
  sw r;
  sw result;
  assert(prefix);
  assert(module);
  buf_init_str(&in, false, (s_str *) &module->str);
  result = prefix->size;
  while ((r = buf_read_character_utf8(&in, &c)) > 0) {
    if (c == '.')
      c = '/';
    else if (character_is_uppercase(c)) {
      if (character_is_lowercase(b))
        result += strlen("_");
      b = c;
      c = character_to_lower(c);
    }
    else
      b = c;
    result += character_utf8_size(c);
  }
  result += strlen(ext);
  return result;
}
