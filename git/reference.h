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
#ifndef LIBKC3_GIT_REFERENCE_H
#define LIBKC3_GIT_REFERENCE_H

/* Heap-allocation functions, call *_free after use. */
void             kc3_git_reference_free (git_reference **ref);
git_reference ** kc3_git_reference_lookup (git_reference **ref,
                                           git_repository **repo,
                                           s_str *name);

#endif /* LIBKC3_GIT_REFERENCE_H */
