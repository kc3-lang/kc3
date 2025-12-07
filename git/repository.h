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
#ifndef LIBKC3_GIT_REPOSITORY_H
#define LIBKC3_GIT_REPOSITORY_H

/* Heap-allocation functions, call *_free after use. */
void              kc3_git_repository_free (git_repository **repo);
git_repository ** kc3_git_repository_init (git_repository **repo,
                                           s_str *path);
git_repository ** kc3_git_repository_init_bare (git_repository **repo,
                                                s_str *path);
git_repository ** kc3_git_repository_open (git_repository **repo,
                                           s_str *path);
git_repository ** kc3_git_repository_open_bare (git_repository **repo,
                                                s_str *path);

/* Observers. */
git_reference ** kc3_git_repository_head (git_repository **repo,
                                          git_reference **dest);

#endif /* LIBKC3_GIT_REPOSITORY_H */
