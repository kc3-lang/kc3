/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3.h"
#include <git2.h>
#include "branch.h"

s_str * kc3_git_branch_name (s_str *dest, git_reference **ref)
{
  const char *a;
  const git_error *e;
  assert(dest);
  assert(ref);
  assert(*ref);
  if (git_branch_name(&a, *ref)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_init: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
  }
  if (! str_init_1_alloc(&tmp, a))
    return NULL;
  *dest = tmp;
  return dest;
}
