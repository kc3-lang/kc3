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
#include "../libkc3/kc3.h"
#include <git2.h>
#include "reference.h"

void kc3_git_reference_free (git_reference **repo)
{
  git_reference_free(*repo);
}

git_reference ** kc3_git_reference_lookup (git_reference **ref,
                                           git_repository **repo,
                                           s_str *name)
{
  const git_error *e;
  git_reference *tmp;
  if (git_reference_lookup(&tmp, *repo, name->ptr.pchar)) {
    e = git_error_last();
    err_write_1("kc3_git_reference_lookup: ");
    err_puts(e->message);
  }
  *ref = tmp;
  return ref;
}
