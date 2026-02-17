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
#include <string.h>
#include <git2.h>
#include "files.h"

static s_map * files_set_entry (const git_tree_entry *entry,
                                const char *name, s_map *map,
                                uw index);

static s_map * files_set_entry (const git_tree_entry *entry,
                                const char *name, s_map *map,
                                uw index)
{
  char hash[128] = {0};
  git_filemode_t mode;
  const git_oid *oid;
  s_map *sub_map;
  git_object_t type;
  const s_sym *type_sym;
  assert(entry);
  assert(map);
  type = git_tree_entry_type(entry);
  switch (type) {
  case GIT_OBJECT_COMMIT:
    type_sym = sym_1("commit");
    break;
  case GIT_OBJECT_TREE:
    type_sym = sym_1("tree");
    break;
  case GIT_OBJECT_BLOB:
    type_sym = sym_1("blob");
    break;
  default:
    err_write_1("kc3_git: files_set_entry: skipping entry type ");
    err_inspect_s32(type);
    err_write_1("\n");
    return map;
  }
  oid = git_tree_entry_id(entry);
  git_oid_tostr(hash, sizeof(hash) - 1, oid);
  mode = git_tree_entry_filemode(entry);
  if (! name)
    name = git_tree_entry_name(entry);
  tag_init_str_1_alloc(map->key + index, name);
  tag_init_map(map->value + index, 4);
  sub_map = &map->value[index].data.map;
  tag_init_psym(sub_map->key + 0, sym_1("name"));
  tag_init_psym(sub_map->key + 1, sym_1("type"));
  tag_init_psym(sub_map->key + 2, sym_1("mode"));
  tag_init_psym(sub_map->key + 3, sym_1("hash"));
  tag_init_str_1_alloc(sub_map->value + 0, name);
  tag_init_psym(       sub_map->value + 1, type_sym);
  tag_init_s32(        sub_map->value + 2, mode);
  tag_init_str_1_alloc(sub_map->value + 3, hash);
  return map;
}

s_map * kc3_git_files (git_repository **repo, const s_str *branch,
                       const s_str *path, s_map *dest)
{
  uw count;
  git_tree_entry *entry = NULL;
  uw i;
  git_object *obj = NULL;
  uw rev_size;
  char *rev;
  const git_tree_entry *sub_entry;
  git_tree *sub_tree;
  s_map tmp = {0};
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
  if (! path->size ||
      (path->size == 1 &&
       path->ptr.pchar[0] == '.'))
    sub_tree = tree;
  else {
    if (git_tree_entry_bypath(&entry, tree, path->ptr.pchar)) {
      git_object_free(obj);
      free(rev);
      map_init(&tmp, 0);
      *dest = tmp;
      return dest;
    }
    type = git_tree_entry_type(entry);
    switch (type) {
    case GIT_OBJECT_BLOB:
      if (! map_init(&tmp, 1)) {
        git_tree_entry_free(entry);
        git_object_free(obj);
        free(rev);
        return NULL;
      }
      if (! files_set_entry(entry, path->ptr.pchar, &tmp, 0)) {
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
  if (! map_init(&tmp, count)) {
    if (sub_tree != (git_tree *) obj)
      git_tree_free(sub_tree);
    git_tree_entry_free(entry);
    git_object_free(obj);
    free(rev);
    return NULL;
  }
  i = 0;
  while (i < count) {
    sub_entry = git_tree_entry_byindex(sub_tree, i);
    if (! files_set_entry(sub_entry, NULL, &tmp, i)) {
      map_clean(&tmp);
      if (sub_tree != (git_tree *) obj)
        git_tree_free(sub_tree);
      git_tree_entry_free(entry);
      git_object_free(obj);
      free(rev);
      return NULL;
    }
    i++;
  }
  if (sub_tree != (git_tree *) obj)
    git_tree_free(sub_tree);
  git_tree_entry_free(entry);
  git_object_free(obj);
  free(rev);
  *dest = tmp;
  return dest;

}
