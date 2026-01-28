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
#include "branch.h"

p_list * kc3_git_branch_list (git_repository **repo, p_list *dest)
{
  const git_error *e;
  git_branch_iterator *iter;
  s_list *l;
  const char *pchar;
  git_reference *ref;
  git_branch_t   ref_type;
  s_list *tmp = NULL;
  if (git_branch_iterator_new(&iter, *repo, GIT_BRANCH_LOCAL)) {
    e = git_error_last();
    err_write_1("kc3_git_branch_list: git_branch_iterator_new: ");
    err_puts(e->message);
    return NULL;
  }
  while (! git_branch_next(&ref, &ref_type, iter)) {
    if (git_branch_name(&pchar, ref)) {
      e = git_error_last();
      err_write_1("kc3_git_branch_list: git_branch_name: ");
      err_puts(e->message);
      list_delete_all(tmp);
      git_reference_free(ref);
      git_branch_iterator_free(iter);
      return NULL;
    }
    if (! (l = list_new_str_1_alloc(pchar, tmp))) {
      list_delete_all(tmp);
      git_branch_iterator_free(iter);
      return NULL;
    }
    tmp = l;
    git_reference_free(ref);
  }
  git_branch_iterator_free(iter);
  *dest = tmp;
  return dest;
}

git_reference ** kc3_git_branch_lookup (git_reference **ref,
                                        git_repository **repo,
                                        s_str *name)
{
  const git_error *e;
  git_reference *tmp;
  if (git_branch_lookup(&tmp, *repo, name->ptr.pchar,
                        GIT_BRANCH_LOCAL)) {
    e = git_error_last();
    err_write_1("kc3_git_branch_lookup: ");
    err_puts(e->message);
  }
  *ref = tmp;
  return ref;
}

s_str * kc3_git_branch_name (s_str *dest, git_reference **ref)
{
  const char *a;
  const git_error *e;
  s_str tmp;
  assert(dest);
  assert(ref);
  assert(*ref);
  if (git_branch_name(&a, *ref)) {
    e = git_error_last();
    err_write_1("kc3_git_branch_name: ");
    err_puts(e->message);
  }
  if (! str_init_1_alloc(&tmp, a))
    return NULL;
  *dest = tmp;
  return dest;
}
