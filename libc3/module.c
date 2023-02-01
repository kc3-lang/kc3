/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include "module.h"

s_module * module_load (s_module *module, const s_sym *name,
                        s_facts *facts)
{
  s_str path;
  assert(module);
  module->name = name;
  module->facts = facts;
  module_path_from_name(&path, name);
  facts_load()
}

s_str * module_path_from_name (s_str *str, const s_sym *name)
{
  character b = -1;
  character c;
  s_buf in;
  s_buf out;
  sw r;
  sw result;
  assert(str);
  assert(name);
  buf_init(&in, false, name->str.size, name->str.ptr.p);
  buf_init_alloc(&out, i * 2);
  while ((r = buf_read_character(&in, &c)) > 0) {
    if (c == '.')
      c = '/';
    else if (character_is_uppercase(c))
      if (character_is_lowercase(b)) {
        if ((r = buf_write_s8(&out, '_')) < 0)
          goto error;
        result += r;
        
             
      
      c = *in;
      if ((r = buf_write_s8(&out, '_')) < 0)
        goto error;
      result += r;
    in++;
  }
  
 error:
  buf_clean(&out);
  return NULL;
}
