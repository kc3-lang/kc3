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
#ifndef LIBKC3_GIT_BRANCH_H
#define LIBKC3_GIT_BRANCH_H

#include "../libkc3/types.h"
#include <git2.h>

git_reference ** kc3_git_branch_lookup (git_reference **ref,
                                        git_repository **repo,
                                        s_str *name);
s_str * kc3_git_branch_name (s_str *dest, git_reference **ref);

#endif /* LIBKC3_GIT_BRANCH_H */
