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
#include "block.h"
#include "env.h"
#include "io.h"
#include "tag.h"

s_block * block_init (s_block *block, const s_sym *name)
{
  s_env *env;
  s_block tmp = {0};
  assert(block);
  env = env_global();
  assert(env);
  tmp.name = name;
  tmp.next = env->block;
  *block = tmp;
  env->block = block;
  return block;
}

void block_clean (s_block *block)
{
  s_env *env;
  assert(block);
  env = env_global();
  assert(env);
  assert(env->block == block);
  env->block = block->next;
}

s_block ** block_find (const s_sym *name)
{
  s_block **block;
  block = &env_global()->block;
  if (! name)
    return block;
  while (*block && (*block)->name != name)
    block = &(*block)->next;
  return block;
}

void block_jump (s_block *block)
{
  env_longjmp(env_global(), &block->buf);
  abort();
}

void block_return (s_block *block, s_tag *value)
{
  block->tag = *value;
  block_jump(block);
  abort();
}

void block_return_from (const s_sym *name, s_tag *value)
{
  s_block **block;
  block = block_find(name);
  if (! block || ! *block) {
    err_write_1("block_return_from: ");
    if (name)
      err_inspect_sym(name);
    else
      err_write_1("void");
    err_puts(": no matching block found");
    err_stacktrace();
    abort();
  }
  block_return(*block, value);
  abort();
}
