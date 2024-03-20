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

bool module_ensure_loaded (const s_sym *module, s_facts *facts)
{
  s_facts_with_cursor cursor;
  s_tag tag_module_name;
  s_tag tag_is_a;
  s_tag tag_module;
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_module, &g_sym_module);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_module_name,
      &tag_is_a, &tag_module,     /* module exists */
      NULL, NULL });
  if (! facts_with_cursor_next(&cursor)) {
    if (! module_load(module, facts)) {
      err_write_1("module_ensure_loaded: module not found: ");
      err_puts(module->str.ptr.pchar);
      assert(! "module_ensure_loaded: module not found");
      facts_with_cursor_clean(&cursor);
      return false;
    }
  }
  facts_with_cursor_clean(&cursor);
  return module_maybe_reload(module, facts);
}

bool module_load (const s_sym *module, s_facts *facts)
{
  return env_module_load(&g_c3_env, module, facts);
}

s_tag * module_load_time (const s_sym *module, s_facts *facts,
                          s_tag *dest)
{
  s_facts_with_cursor cursor;
  s_tag tag_module_name;
  s_tag tag_load_time;
  s_tag tag_time_var;
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_load_time, &g_sym_load_time);
  tag_init_var(&tag_time_var);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_module_name, &tag_load_time, &tag_time_var, NULL, NULL });
  if (! facts_with_cursor_next(&cursor)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  facts_with_cursor_clean(&cursor);
  *dest = tag_time_var;
  return dest;
}

bool module_maybe_reload (const s_sym *module, s_facts *facts)
{
  return env_module_maybe_reload(&g_c3_env, module, facts);
}

s_str * module_path (const s_sym *module, const s_str *prefix,
                     s_str *dest)
{
  character b = 0;
  character c;
  s_buf in;
  s_buf out;
  sw out_size;
  sw r;
  assert(dest);
  assert(module);
  buf_init_str(&in, false, (s_str *) &module->str);
  out_size = module_path_size(module, prefix);
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
  if ((r = buf_write_1(&out, FACTS_EXT)) < 0)
    goto error;
  return buf_to_str(&out, dest);
 error:
  buf_clean(&out);
  err_puts("module_path: error");
  assert(! "module_path: error");
  return NULL;
}

sw module_path_size (const s_sym *module, const s_str *prefix)
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
  result += strlen(FACTS_EXT);
  return result;
}
