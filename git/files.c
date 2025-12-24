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
#include "files.h"

static p_list * files_push_entry (const git_tree_entry *entry,
                                  const char *name, p_list *dest);

static p_list * files_push_entry (const git_tree_entry *entry,
                                  const char *name, p_list *dest)
{
  char hash[64];
  s_map *map;
  git_filemode_t mode;
  p_list tmp;
  git_object_t type;
  const s_sym *type_sym;
  assert(entry);
  assert(dest);
  if (! name)
    name = git_tree_entry_name(entry);
  mode = git_tree_entry_filemode(entry);
  type = git_tree_entry_type(entry);
  type_sym = (type == GIT_OBJECT_TREE) ? sym_1("tree") :
    sym_1("blob");
  if (! (tmp = list_new_map(4, *dest)))
    return NULL;
  map = &tmp->tag.data.map;
  tag_init_psym(map->key + 0, sym_1("name"));
  tag_init_psym(map->key + 1, sym_1("type"));
  tag_init_psym(map->key + 2, sym_1("mode"));
  tag_init_psym(map->key + 3, sym_1("hash"));
  tag_init_str_1_alloc(map->value + 0, name);
  tag_init_psym(       map->value + 1, type_sym);
  tag_init_s32(        map->value + 2, mode);
  tag_init_str_1_alloc(map->value + 3, hash);
  *dest = tmp;
  return dest;
}

p_list * kc3_git_files (git_repository **repo, const s_str *branch,
                        const s_str *path, p_list *dest)
{
  uw count;
  git_tree_entry *entry = NULL;
  git_object *obj = NULL;
  uw rev_size;
  char *rev;
  const git_tree_entry *sub_entry;
  git_tree *sub_tree;
  p_list tmp = NULL;
  git_tree *tree;
  git_object_t type;
  rev_size = branch->size + 8;
  if (! (rev = alloc(rev_size)))
    return NULL;
  memcpy(rev, branch->ptr.p, branch->size);
  memcpy(rev + branch->size, "^{tree}", 7);
  if (git_revparse_single(&obj, *repo, rev)) {
    err_puts("kc3_git_files: git_revparse_single");
    free(rev);
    return NULL;
  }
  tree = (git_tree *) obj;
  if (! path->ptr.pchar[0] ||
      ! strcmp(path->ptr.pchar, "."))
    sub_tree = tree;
  else {
    if (git_tree_entry_bypath(&entry, tree, path->ptr.pchar)) {
      err_puts("kc3_git_files: git_tree_entry_bypath");
      git_object_free(obj);
      free(rev);
      return NULL;
    }
    type = git_tree_entry_type(entry);
    switch (type) {
    case GIT_OBJECT_BLOB:
      if (! files_push_entry(entry, path->ptr.pchar, &tmp)) {
        git_tree_entry_free(entry);
        git_object_free(obj);
        free(rev);
        return NULL;
      }
      git_tree_entry_free(entry);
      git_object_free(obj);
      free(rev);
      *dest = tmp;
      return dest;
    case GIT_OBJECT_TREE:
      if (git_tree_lookup(&sub_tree, *repo,
                          git_tree_entry_id(entry))) {
        err_puts("kc3_git_files: git_tree_lookup");
        git_tree_entry_free(entry);
        git_object_free(obj);
        free(rev);
        return NULL;
      }
      break;
    default:
      err_puts("kc3_git_files: git_tree_entry_type: unknown type");
      git_tree_entry_free(entry);
      git_object_free(obj);
      free(rev);
      return NULL;
    }
  }
  count = git_tree_entrycount(sub_tree);
  while (count--) {
    sub_entry = git_tree_entry_byindex(sub_tree, count);
    if (! files_push_entry(sub_entry, NULL, &tmp)) {
        git_tree_entry_free(entry);
        git_object_free(obj);
        free(rev);
        return NULL;
    }
  }
  if (sub_tree != (git_tree *) obj)
    git_tree_free(sub_tree);
  git_tree_entry_free(entry);
  git_object_free(obj);
  free(rev);
  *dest = tmp;
  return dest;

}
