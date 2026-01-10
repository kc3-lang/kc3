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
#include <string.h>
#include <git2.h>
#include "log.h"

typedef struct git_log_options s_git_log_options;
typedef struct git_log_state   s_git_log_state;

struct git_log_options {
  int show_diff;
  int show_log_size;
  int skip, limit;
  int min_parents, max_parents;
  git_time_t before;
  git_time_t after;
  const char *author;
  const char *committer;
  const char *grep;
};

struct git_log_state {
  int hide;
  git_repository *repo;
  const char *repodir;
  git_revwalk *walker;
  int sorting;
  int revisions;
};

static int      log_add_revision(s_git_log_state *s,
                                 const char *revstr);
static int      log_match_with_parent (git_commit *commit,
                                       int i,
                                       git_diff_options *opts);
static p_list * log_push_commit (p_list *log_tail,
                                 git_commit *commit);
static int      log_push_rev (s_git_log_state *s,
                              git_object *obj,
                              int hide);

p_list * kc3_git_log (git_repository **repo,
                      const s_str *branch_name, const s_str *path,
                      const s_tag *skip, const s_tag *limit,
                      p_list *dest)
{
  git_commit *commit = NULL;
  s32 count = 0;
  git_diff_options diffopts =
    {GIT_DIFF_OPTIONS_VERSION, 0, GIT_SUBMODULE_IGNORE_UNSPECIFIED,
     {NULL, 0}, NULL, NULL, NULL, 3, 0, 0, 0, 0, 0, 0};
  s32 i;
  git_oid oid = {0};
  s_git_log_options opt = {0};
  int parents = 0;
  char path_pchar[PATH_MAX + 1] = {0};
  char *path_pchar_p = path_pchar;
  int printed = 0;
  git_pathspec *ps = NULL;
  s_git_log_state s = {0};
  const s_sym *sym_S32 = &g_sym_S32;
  p_list *tail;
  p_list tmp = NULL;
  if (! repo || ! *repo || ! branch_name || ! branch_name->size ||
      ! path || ! skip || ! limit || ! dest) {
    err_puts("kc3_git_log: invalid argument");
    return NULL;
  }
  s.repo = *repo;
  s.sorting = GIT_SORT_TIME;
  if (log_add_revision(&s, branch_name->ptr.pchar)) {
    err_write_1("kc3_git_log: branch not found: ");
    err_inspect_str(branch_name);
    err_write_1("\n");
    return NULL;
  }
  opt.max_parents = -1;
  if (! s32_init_cast(&opt.skip, &sym_S32, skip) || opt.skip < 0)
    return NULL;
  if (! s32_init_cast(&opt.limit, &sym_S32, limit) || opt.limit < 0)
    return NULL;
  if (path->size) {
    if (path->size > PATH_MAX) {
      err_puts("kc3_git_log: path->size > PATH_MAX");
      return NULL;
    }
    memcpy(path_pchar, path->ptr.pchar, path->size);
    diffopts.pathspec.strings = &path_pchar_p;
    diffopts.pathspec.count = 1;
    git_pathspec_new(&ps, &diffopts.pathspec);
  }
  tail = &tmp;
  while (! git_revwalk_next(&oid, s.walker)) {
    if (git_commit_lookup(&commit, s.repo, &oid)) {
      err_puts("kc3_git_log: git_commit_lookup");
      return NULL;
    }
    parents = git_commit_parentcount(commit);
    if (parents < opt.min_parents ||
        (opt.max_parents > 0 && parents > opt.max_parents))
      continue;
    if (diffopts.pathspec.count > 0) {
      int unmatched = parents;
      if (parents == 0) {
        git_tree *tree = NULL;
        if (git_commit_tree(&tree, commit)) {
          err_puts("kc3_git_log: git_commit_tree");
          git_commit_free(commit);
          return NULL;
        }
        if (git_pathspec_match_tree(NULL, tree,
                                    GIT_PATHSPEC_NO_MATCH_ERROR,
                                    ps) != 0)
          unmatched = 1;
        git_tree_free(tree);
      } else if (parents == 1) {
        unmatched = log_match_with_parent(commit, 0, &diffopts) ? 0 : 1;
      } else {
        for (i = 0; i < parents; ++i) {
          if (log_match_with_parent(commit, i, &diffopts))
            unmatched--;
        }
      }
      if (unmatched > 0)
        continue;
    }
    if (count++ < opt.skip)
      continue;
    if (opt.limit != -1 && printed++ >= opt.limit) {
      git_commit_free(commit);
      break;
    }
    if (! (tail = log_push_commit(tail, commit))) {
      err_puts("kc3_git_log: log_push_commit");
      git_commit_free(commit);
      return NULL;
    }
    // TODO
  }
  *dest = tmp;
  return dest;
}

static int log_push_rev (s_git_log_state *s,
                         git_object *obj,
                         int hide)
{
  int res = 0;
  hide ^= s->hide;
  if (!s->walker) {
    if (git_revwalk_new(&s->walker, s->repo)) {
      res = -1;
      goto error;
    }
    git_revwalk_sorting(s->walker, s->sorting);
  }
  if (!obj) {
    if (git_revwalk_push_head(s->walker)) {
      res = -2;
      goto error;
    }
  }
  else if (hide) {
    if (git_revwalk_hide(s->walker, git_object_id(obj))) {
      res = -3;
      goto error;
    }
  }
  else
    if (git_revwalk_push(s->walker, git_object_id(obj))) {
      res = -4;
      goto error;
    }
 error:
  return res;
}

static int log_add_revision (s_git_log_state *s,
                             const char *revstr)
{
  git_revspec revs = {0};
  int hide = 0;
  int res = 0;
  if (! revstr)
    return log_push_rev(s, NULL, hide);
  if (*revstr == '^') {
    revs.flags = GIT_REVSPEC_SINGLE;
    hide = !hide;
    if (git_revparse_single(&revs.from, s->repo, revstr + 1) < 0) {
      res = -1;
      goto error;
    }
  }
  else if (git_revparse(&revs, s->repo, revstr) < 0) {
    res = -2;
    goto error;
  }
  if ((revs.flags & GIT_REVSPEC_SINGLE) != 0)
    log_push_rev(s, revs.from, hide);
  else {
    log_push_rev(s, revs.to, hide);
    if ((revs.flags & GIT_REVSPEC_MERGE_BASE) != 0) {
      git_oid base;
      if (git_merge_base(&base, s->repo,
                         git_object_id(revs.from),
                         git_object_id(revs.to))) {
        res = -3;
        goto error;
      }
      if (git_object_lookup(&revs.to, s->repo, &base,
                            GIT_OBJECT_COMMIT)) {
        res = -4;
        goto error;
      }
      if (log_push_rev(s, revs.to, hide)) {
        res = -5;
        goto error;
      }
    }
    if (log_push_rev(s, revs.from, !hide)) {
      res = -6;
      goto error;
    }
  }
 error:
  git_object_free(revs.from);
  git_object_free(revs.to);
  return res;
}

static int log_match_with_parent (git_commit *commit,
                                  int i,
                                  git_diff_options *opts)
{
  git_commit *parent = NULL;
  git_tree *tree[2] = {NULL, NULL};
  git_diff *diff = NULL;
  int ndeltas = 0;
  int res = 0;
  if (git_commit_parent(&parent, commit, (size_t) i)) {
    res = -1;
    goto error;
  }
  if (git_commit_tree(&tree[0], parent)) {
    res = -2;
    goto error;
  }
  if (git_commit_tree(&tree[1], commit)) {
    res = -3;
    goto error;
  }
  if (git_diff_tree_to_tree(&diff, git_commit_owner(commit),
                            tree[0], tree[1], opts)) {
    res = -4;
    goto error;
  }
  ndeltas = (int) git_diff_num_deltas(diff);
  res = ndeltas > 0;
 error:
  git_diff_free(diff);
  git_tree_free(tree[0]);
  git_tree_free(tree[1]);
  git_commit_free(parent);
  return res;
}

static p_list * log_push_commit (p_list *log_tail,
                                 git_commit *commit)
{
  char buf[GIT_OID_HEXSZ + 1];
  int i;
  int count;
  s_map *map;
  const git_signature *sig;
  p_list tmp;
  if (! (tmp = list_new_map(6, NULL)))
    return NULL;
  map = &tmp->tag.data.map;
  tag_init_psym(map->key + 0, sym_1("author_email"));
  tag_init_psym(map->key + 1, sym_1("message"));
  tag_init_psym(map->key + 2, sym_1("author_name"));
  tag_init_psym(map->key + 3, sym_1("parents"));
  tag_init_psym(map->key + 4, sym_1("hash"));
  tag_init_psym(map->key + 5, sym_1("date"));
  p_list parents = NULL;
  git_oid_tostr(buf, sizeof(buf), git_commit_id(commit));
  tag_init_str_1_alloc(map->value + 1, git_commit_message(commit));
  tag_init_str_1_alloc(map->value + 4, buf);
  if ((count = git_commit_parentcount(commit)) > 1) {
    for (i = 0; i < count; ++i) {
      git_oid_tostr(buf, sizeof(buf),
                    git_commit_parent_id(commit, i));
      parents = list_new(parents);
      if (! tag_init_str_1_alloc(&parents->tag, buf))
        goto ko;
    }
  }
  tag_init_plist(map->value + 3, parents);
  if ((sig = git_commit_author(commit)) != NULL) {
    tag_init_s32(map->value + 5, sig->when.time +
                 sig->when.offset * 60);
    tag_init_str_1_alloc(map->value + 2, sig->name);
    tag_init_str_1_alloc(map->value + 0, sig->email);
  }
  *log_tail = tmp;
  return &(*log_tail)->next.data.plist;
 ko:
  list_delete_all(tmp);
  return NULL;
}
