/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
  return env_module_ensure_loaded(&g_kc3_env, module);
}

bool * module_has_ident (const s_sym *module, const s_ident *ident,
                         bool *dest)
{
  return env_module_has_ident(&g_kc3_env, module, ident, dest);
}

bool * module_has_symbol (const s_sym *module, const s_sym *sym,
                          bool *dest)
{
  return env_module_has_symbol(&g_kc3_env, module, sym, dest);
}

bool * module_is_loading (const s_sym *module, bool *dest)
{
  return env_module_is_loading(&g_kc3_env, module, dest);
}

bool module_load (const s_sym *module)
{
  return env_module_load(&g_kc3_env, module);
}

const s_time ** module_load_time (const s_sym *module,
                                  const s_time **dest)
{
  return env_module_load_time(&g_kc3_env, module, dest);
}

bool module_maybe_reload (const s_sym *module)
{
  return env_module_maybe_reload(&g_kc3_env, module);
}

s_str * module_path (const s_sym *module, const s_str *prefix,
                     const char *ext, s_str *dest)
{
  char a[BUF_SIZE];
  character b = 0;
  character c;
  s_buf in;
  s_buf out;
  sw r;
  s_str *result;
  assert(dest);
  assert(module);
  buf_init_str(&in, false, (s_str *) &module->str);
  buf_init(&out, false, sizeof(a), a);
  if ((r = buf_write_str(&out, prefix)) < 0)
    goto error;
  while ((r = buf_read_character_utf8(&in, &c)) > 0) {
    if (c == '.') {
      b = c;
      c = '/';
    }
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
  if (! (result = buf_read_to_str(&out, dest)))
    goto error;
  buf_clean(&out);
  return result;
 error:
  buf_clean(&out);
  err_puts("module_path: error");
  assert(! "module_path: error");
  return NULL;
}
