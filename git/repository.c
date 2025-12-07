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
#include "repository.h"

git_repository ** kc3_git_repository_init (git_repository **repo,
                                           s_str *path)
{
  const git_error *e;
  git_repository *tmp;
  if (git_repository_init(&tmp, path->ptr.pchar, false)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_init: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
  }
  *repo = tmp;
  return repo;
}

git_repository ** kc3_git_repository_init_bare (git_repository **repo,
                                                s_str *path)
{
  const git_error *e;
  git_repository *tmp;
  if (git_repository_init(&tmp, path->ptr.pchar, true)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_init_bare: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
  }
  *repo = tmp;
  return repo;
}
