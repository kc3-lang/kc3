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
#include "alloc.h"
#include "assert.h"
#include "env.h"
#include "list.h"
#include "memleak.h"
#include <stdlib.h>

void * alloc (uw size)
{
  void *a;
  s_env *env;
  s_list *stacktrace;
  if (! size) {
    err_puts("alloc: zero size");
    assert(! "alloc: zero size");
    return NULL;
  }
  a = calloc(1, size);
  if (! a) {
    err_puts("alloc: failed to allocate memory");
    assert(! "alloc: failed to allocate memory");
    return NULL;
  }
  if (g_memleak_enabled) {
    env = env_global();
    stacktrace = env ? list_new_copy_all(env->stacktrace) : NULL;
    memleak_add(a, size, stacktrace);
  }
  return a;
}

void alloc_free (void *ptr)
{
  if (ptr) {
    if (g_memleak_enabled)
      memleak_remove(ptr);
    free(ptr);
  }
}
