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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <git2.h>
#include "log.h"

typedef struct git_log_options s_git_log_options;
typedef struct git_log_state   s_git_log_state;
typedef struct log_commit_graph s_log_commit_graph;
typedef struct log_commit_graph_entry s_log_commit_graph_entry;
typedef struct log_commit_graph_file s_log_commit_graph_file;

struct log_commit_graph_file {
  u8 *data;
  uw size;
  uw base_position;
  uw commit_count;
  const u8 *fanout;
  const u8 *oid_lookup;
  const u8 *commit_data;
  const u8 *extra_edges;
  uw extra_edge_count;
  const u8 *bloom_index;
  uw bloom_index_size;
  const u8 *bloom_data;
  uw bloom_data_size;
};

struct log_commit_graph {
  s_log_commit_graph_file *file;
  uw file_count;
  uw commit_count;
};

struct log_commit_graph_entry {
  const s_log_commit_graph_file *file;
  uw index;
};

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
static void     log_graph_clean (s_log_commit_graph *graph);
static int      log_graph_entry_find (const s_log_commit_graph *graph,
                                      const git_oid *oid,
                                      s_log_commit_graph_entry *entry);
static int      log_graph_entry_parent
                     (const s_log_commit_graph *graph,
                      const s_log_commit_graph_entry *entry,
                      uw index,
                      git_oid *oid,
                      git_oid *tree_oid);
static int      log_graph_entry_path_maybe_changed
                     (const s_log_commit_graph_entry *entry,
                      const char *path);
static uw       log_graph_entry_parent_count
                     (const s_log_commit_graph_entry *entry);
static void     log_graph_entry_tree_oid
                     (const s_log_commit_graph_entry *entry,
                      git_oid *tree_oid);
static int      log_graph_init (s_log_commit_graph *graph,
                                git_repository *repo);
static int      log_match_with_parent (git_commit *commit,
                                       int i,
                                       const char *path,
                                       git_odb *odb,
                                       const git_oid *commit_tree_oid,
                                       git_odb_object *commit_tree_object,
                                       git_oid *parent_oid_dest,
                                       git_odb_object **parent_tree_object_dest);
static p_list * log_push_commit (p_list *log_tail,
                                 git_commit *commit);
static int      log_push_rev (s_git_log_state *s,
                              git_object *obj,
                              int hide);
static int      log_tree_entry (git_odb *odb,
                                const git_oid *tree_oid,
                                const char *name,
                                size_t name_size,
                                bool *exists,
                                git_oid *entry_oid,
                                git_filemode_t *entry_filemode);
static int      log_tree_entry_object (git_odb_object *object,
                                       const char *name,
                                       size_t name_size,
                                       bool *exists,
                                       git_oid *entry_oid,
                                       git_filemode_t *entry_filemode);
static int      log_tree_path_changed (git_odb *odb,
                                       const git_oid *a,
                                       const git_oid *b,
                                       const char *path,
                                       git_odb_object *a_root_object,
                                       git_odb_object **b_root_object_dest);

p_list * kc3_git_log (git_repository **repo,
                      const s_str *branch_name, const s_str *path,
                      const s_tag *skip, const s_tag *limit,
                      p_list *dest)
{
  git_commit *commit = NULL;
  git_object *commit_object = NULL;
  s_log_commit_graph graph = {0};
  s_log_commit_graph_entry graph_entry = {0};
  bool graph_entry_valid;
  bool graph_valid = false;
  git_odb *odb = NULL;
  git_oid parent_oid = {0};
  git_oid parent_tree_oid = {0};
  git_odb_object *tree_object = NULL;
  git_odb_object *tree_object_next = NULL;
  git_oid tree_object_next_oid = {0};
  bool tree_object_next_valid = false;
  git_oid tree_oid = {0};
  s32 count = 0;
  s32 i;
  git_oid oid = {0};
  s_git_log_options opt = {0};
  int parents = 0;
  char path_pchar[PATH_MAX + 1] = {0};
  int printed = 0;
  int r;
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
  opt.max_parents = -1;
  if (! s32_init_cast(&opt.skip, &sym_S32, skip) || opt.skip < 0) {
    return NULL;
  }
  if (! s32_init_cast(&opt.limit, &sym_S32, limit) || opt.limit < 0) {
    return NULL;
  }
  if (! opt.limit) {
    *dest = NULL;
    return dest;
  }
  if (! path->size &&
      branch_name->size == GIT_OID_HEXSZ &&
      ! git_oid_fromstrn(&oid, branch_name->ptr.p_pchar,
                         branch_name->size)) {
    if (opt.skip > 0) {
      *dest = NULL;
      return dest;
    }
    if (git_commit_lookup(&commit, s.repo, &oid)) {
      err_puts("kc3_git_log: git_commit_lookup");
      return NULL;
    }
    parents = git_commit_parentcount(commit);
    if (parents < opt.min_parents ||
        (opt.max_parents > 0 && parents > opt.max_parents)) {
      git_commit_free(commit);
      *dest = NULL;
      return dest;
    }
    tail = &tmp;
    if (! log_push_commit(tail, commit)) {
      err_puts("kc3_git_log: log_push_commit");
      git_commit_free(commit);
      return NULL;
    }
    git_commit_free(commit);
    *dest = tmp;
    return dest;
  }
  if (! path->size && ! opt.skip && opt.limit == 1) {
    git_object *object = NULL;
    if (git_revparse_single(&object, s.repo,
                            branch_name->ptr.p_pchar)) {
      err_write_1("kc3_git_log: branch not found: ");
      err_inspect_str(branch_name);
      err_write_1("\n");
      return NULL;
    }
    if (git_object_peel(&commit_object, object, GIT_OBJECT_COMMIT)) {
      err_puts("kc3_git_log: git_object_peel");
      git_object_free(object);
      return NULL;
    }
    git_object_free(object);
    commit = (git_commit *) commit_object;
    parents = git_commit_parentcount(commit);
    if (parents < opt.min_parents ||
        (opt.max_parents > 0 && parents > opt.max_parents)) {
      git_commit_free(commit);
      *dest = NULL;
      return dest;
    }
    tail = &tmp;
    if (! log_push_commit(tail, commit)) {
      err_puts("kc3_git_log: log_push_commit");
      git_commit_free(commit);
      return NULL;
    }
    git_commit_free(commit);
    *dest = tmp;
    return dest;
  }
  if (path->size) {
    if (path->size > PATH_MAX) {
      err_puts("kc3_git_log: path->size > PATH_MAX");
      return NULL;
    }
    memcpy(path_pchar, path->ptr.p_pchar, path->size);
  }
  if (path->size && git_repository_odb(&odb, s.repo)) {
    err_puts("kc3_git_log: git_repository_odb");
    return NULL;
  }
  if (path->size && ! log_graph_init(&graph, s.repo))
    graph_valid = true;
  if (log_add_revision(&s, branch_name->ptr.p_pchar)) {
    err_write_1("kc3_git_log: branch not found: ");
    err_inspect_str(branch_name);
    err_write_1("\n");
    log_graph_clean(&graph);
    git_odb_free(odb);
    git_revwalk_free(s.walker);
    return NULL;
  }
  tail = &tmp;
  while (! git_revwalk_next(&oid, s.walker)) {
    if (! path->size) {
      if (count++ < opt.skip)
        continue;
      if (printed >= opt.limit)
        break;
    }
    if (tree_object_next_valid) {
      if (git_oid_equal(&oid, &tree_object_next_oid)) {
        tree_object = tree_object_next;
        tree_object_next = NULL;
      }
      else {
        git_odb_object_free(tree_object_next);
        tree_object_next = NULL;
      }
      tree_object_next_valid = false;
    }
    graph_entry_valid = path->size && graph_valid &&
      ! log_graph_entry_find(&graph, &oid, &graph_entry);
    if (graph_entry_valid) {
      uw parent_count = log_graph_entry_parent_count(&graph_entry);
      if (parent_count == UW_MAX)
        graph_entry_valid = false;
      else {
        parents = (int) parent_count;
        log_graph_entry_tree_oid(&graph_entry, &tree_oid);
      }
    }
    if (! graph_entry_valid) {
      if (git_commit_lookup(&commit, s.repo, &oid)) {
        err_puts("kc3_git_log: git_commit_lookup");
        goto ko;
      }
      parents = git_commit_parentcount(commit);
      git_oid_cpy(&tree_oid, git_commit_tree_id(commit));
    }
    if (graph_entry_valid &&
        ! log_graph_entry_path_maybe_changed(&graph_entry,
                                             path_pchar)) {
      git_odb_object_free(tree_object);
      tree_object = NULL;
      continue;
    }
    if (parents < opt.min_parents ||
        (opt.max_parents > 0 && parents > opt.max_parents)) {
      git_odb_object_free(tree_object);
      tree_object = NULL;
      git_commit_free(commit);
      commit = NULL;
      continue;
    }
    if (path->size) {
      int unmatched = parents;
      if (parents == 0) {
        r = log_tree_path_changed(odb, &tree_oid,
                                  NULL, path_pchar, tree_object, NULL);
        if (r < 0) {
          err_puts("kc3_git_log: log_tree_path_changed");
          goto ko;
        }
        unmatched = ! r;
      }
      else {
        for (i = 0; i < parents; ++i) {
          if (graph_entry_valid) {
            if (log_graph_entry_parent(&graph, &graph_entry, i,
                                       &parent_oid, &parent_tree_oid)) {
              err_puts("kc3_git_log: log_graph_entry_parent");
              goto ko;
            }
            r = log_tree_path_changed
              (odb, &tree_oid, &parent_tree_oid, path_pchar,
               tree_object, parents == 1 ? &tree_object_next : NULL);
          }
          else
            r = log_match_with_parent
              (commit, i, path_pchar, odb, &tree_oid, tree_object,
               parents == 1 ? &parent_oid : NULL,
               parents == 1 ? &tree_object_next : NULL);
          if (r < 0) {
            err_puts("kc3_git_log: log_match_with_parent");
            goto ko;
          }
          if (parents == 1 && tree_object_next) {
            git_oid_cpy(&tree_object_next_oid, &parent_oid);
            tree_object_next_valid = true;
          }
          if (r)
            unmatched--;
        }
      }
      if (unmatched > 0) {
        git_odb_object_free(tree_object);
        tree_object = NULL;
        git_commit_free(commit);
        commit = NULL;
        continue;
      }
      if (count++ < opt.skip) {
        git_odb_object_free(tree_object);
        tree_object = NULL;
        git_commit_free(commit);
        commit = NULL;
        continue;
      }
      if (printed >= opt.limit) {
        git_odb_object_free(tree_object);
        tree_object = NULL;
        git_commit_free(commit);
        commit = NULL;
        break;
      }
      git_odb_object_free(tree_object);
      tree_object = NULL;
    }
    printed++;
    if (! commit && git_commit_lookup(&commit, s.repo, &oid)) {
      err_puts("kc3_git_log: git_commit_lookup");
      goto ko;
    }
    if (! (tail = log_push_commit(tail, commit))) {
      err_puts("kc3_git_log: log_push_commit");
      goto ko;
    }
    git_commit_free(commit);
    commit = NULL;
  }
  git_odb_object_free(tree_object_next);
  log_graph_clean(&graph);
  git_odb_free(odb);
  git_revwalk_free(s.walker);
  *dest = tmp;
  return dest;
 ko:
  git_commit_free(commit);
  git_odb_object_free(tree_object);
  git_odb_object_free(tree_object_next);
  log_graph_clean(&graph);
  git_odb_free(odb);
  git_revwalk_free(s.walker);
  list_delete_all(tmp);
  return NULL;
}

static u32 log_graph_be32 (const u8 *p)
{
  return ((u32) p[0] << 24) | ((u32) p[1] << 16) |
    ((u32) p[2] << 8) | p[3];
}

static u64 log_graph_be64 (const u8 *p)
{
  return ((u64) log_graph_be32(p) << 32) | log_graph_be32(p + 4);
}

static void log_graph_clean (s_log_commit_graph *graph)
{
  uw i;
  if (! graph)
    return;
  i = 0;
  while (i < graph->file_count) {
    free(graph->file[i].data);
    i++;
  }
  free(graph->file);
  memset(graph, 0, sizeof(*graph));
}

static int log_graph_file_load (s_log_commit_graph_file *file,
                                const char *path,
                                uw base_position)
{
  const u8 *chunk;
  u8 chunk_count;
  const u8 *chunk_table;
  u32 id;
  FILE *input = NULL;
  uw i;
  u64 offset;
  u64 offset_next;
  long size;
  s_log_commit_graph_file tmp = {0};
  if (! (input = fopen(path, "rb")) ||
      fseek(input, 0, SEEK_END) ||
      (size = ftell(input)) < 0 ||
      fseek(input, 0, SEEK_SET) ||
      (uw) size < 8 + 12 + GIT_OID_RAWSZ ||
      ! (tmp.data = malloc((uw) size)) ||
      fread(tmp.data, 1, (uw) size, input) != (uw) size)
    goto ko;
  fclose(input);
  input = NULL;
  tmp.size = (uw) size;
  tmp.base_position = base_position;
  if (memcmp(tmp.data, "CGPH", 4) || tmp.data[4] != 1 ||
      tmp.data[5] != 1)
    goto ko;
  chunk_count = tmp.data[6];
  if (tmp.size < 8 + ((uw) chunk_count + 1) * 12 + GIT_OID_RAWSZ)
    goto ko;
  chunk_table = tmp.data + 8;
  i = 0;
  while (i < chunk_count) {
    id = log_graph_be32(chunk_table + i * 12);
    offset = log_graph_be64(chunk_table + i * 12 + 4);
    offset_next = log_graph_be64(chunk_table + (i + 1) * 12 + 4);
    if (offset > offset_next || offset_next > tmp.size - GIT_OID_RAWSZ)
      goto ko;
    chunk = tmp.data + (uw) offset;
    switch (id) {
    case 0x4f494446:
      if (offset_next - offset != 256 * 4)
        goto ko;
      tmp.fanout = chunk;
      break;
    case 0x4f49444c:
      tmp.oid_lookup = chunk;
      break;
    case 0x43444154:
      tmp.commit_data = chunk;
      break;
    case 0x45444745:
      if ((offset_next - offset) % 4)
        goto ko;
      tmp.extra_edges = chunk;
      tmp.extra_edge_count = (uw) ((offset_next - offset) / 4);
      break;
    case 0x42494458:
      tmp.bloom_index = chunk;
      tmp.bloom_index_size = (uw) (offset_next - offset);
      break;
    case 0x42444154:
      tmp.bloom_data = chunk;
      tmp.bloom_data_size = (uw) (offset_next - offset);
      break;
    default:
      break;
    }
    i++;
  }
  if (! tmp.fanout || ! tmp.oid_lookup || ! tmp.commit_data)
    goto ko;
  tmp.commit_count = log_graph_be32(tmp.fanout + 255 * 4);
  if ((uw) (tmp.oid_lookup - tmp.data) > tmp.size ||
      tmp.commit_count >
      (tmp.size - (uw) (tmp.oid_lookup - tmp.data)) / GIT_OID_RAWSZ ||
      (uw) (tmp.commit_data - tmp.data) > tmp.size ||
      tmp.commit_count >
      (tmp.size - (uw) (tmp.commit_data - tmp.data)) /
      (GIT_OID_RAWSZ + 16))
    goto ko;
  if ((tmp.bloom_index || tmp.bloom_data) &&
      (! tmp.bloom_index || ! tmp.bloom_data ||
       tmp.bloom_index_size != tmp.commit_count * 4 ||
       tmp.bloom_data_size < 12)) {
    tmp.bloom_index = NULL;
    tmp.bloom_data = NULL;
    tmp.bloom_index_size = 0;
    tmp.bloom_data_size = 0;
  }
  *file = tmp;
  return 0;
 ko:
  if (input)
    fclose(input);
  free(tmp.data);
  return -1;
}

static int log_graph_init (s_log_commit_graph *graph,
                           git_repository *repo)
{
  FILE *chain = NULL;
  char hash[GIT_OID_HEXSZ + 3];
  uw hash_size;
  git_buf objects = GIT_BUF_INIT;
  char path[PATH_MAX + 1];
  s_log_commit_graph_file *file;
  int result = -1;
  if (git_repository_is_shallow(repo) ||
      git_repository_item_path(&objects, repo,
                               GIT_REPOSITORY_ITEM_OBJECTS) ||
      snprintf(path, sizeof(path), "%s/info/commit-graphs/commit-graph-chain",
               objects.ptr) >= (int) sizeof(path))
    goto clean;
  chain = fopen(path, "rb");
  if (chain) {
    while (fgets(hash, sizeof(hash), chain)) {
      hash_size = strlen(hash);
      while (hash_size &&
             (hash[hash_size - 1] == '\n' || hash[hash_size - 1] == '\r'))
        hash[--hash_size] = 0;
      if (hash_size != GIT_OID_HEXSZ ||
          snprintf(path, sizeof(path),
                   "%s/info/commit-graphs/graph-%s.graph",
                   objects.ptr, hash) >= (int) sizeof(path))
        goto clean;
      file = realloc(graph->file,
                     (graph->file_count + 1) * sizeof(*file));
      if (! file)
        goto clean;
      graph->file = file;
      memset(graph->file + graph->file_count, 0, sizeof(*file));
      if (log_graph_file_load(graph->file + graph->file_count, path,
                              graph->commit_count))
        goto clean;
      graph->commit_count += graph->file[graph->file_count].commit_count;
      graph->file_count++;
    }
    if (ferror(chain) || ! graph->file_count)
      goto clean;
  }
  else {
    if (snprintf(path, sizeof(path), "%s/info/commit-graph",
                 objects.ptr) >= (int) sizeof(path))
      goto clean;
    if (! (graph->file = calloc(1, sizeof(*graph->file))) ||
        log_graph_file_load(graph->file, path, 0))
      goto clean;
    graph->file_count = 1;
    graph->commit_count = graph->file->commit_count;
  }
  result = 0;
 clean:
  if (chain)
    fclose(chain);
  git_buf_dispose(&objects);
  if (result)
    log_graph_clean(graph);
  return result;
}

static int log_graph_entry_at (const s_log_commit_graph *graph,
                               uw position,
                               s_log_commit_graph_entry *entry)
{
  uw i = graph->file_count;
  while (i) {
    const s_log_commit_graph_file *file = graph->file + --i;
    if (position >= file->base_position &&
        position - file->base_position < file->commit_count) {
      entry->file = file;
      entry->index = position - file->base_position;
      return 0;
    }
  }
  return -1;
}

static int log_graph_entry_find (const s_log_commit_graph *graph,
                                 const git_oid *oid,
                                 s_log_commit_graph_entry *entry)
{
  const s_log_commit_graph_file *file;
  uw high;
  uw i = graph->file_count;
  uw low;
  uw middle;
  int order;
  while (i) {
    file = graph->file + --i;
    low = oid->id[0] ?
      log_graph_be32(file->fanout + (oid->id[0] - 1) * 4) : 0;
    high = log_graph_be32(file->fanout + oid->id[0] * 4);
    while (low < high) {
      middle = low + (high - low) / 2;
      order = memcmp(oid->id,
                     file->oid_lookup + middle * GIT_OID_RAWSZ,
                     GIT_OID_RAWSZ);
      if (order < 0)
        high = middle;
      else if (order > 0)
        low = middle + 1;
      else {
        entry->file = file;
        entry->index = middle;
        return 0;
      }
    }
  }
  return -1;
}

static const u8 * log_graph_entry_data
                     (const s_log_commit_graph_entry *entry)
{
  return entry->file->commit_data +
    entry->index * (GIT_OID_RAWSZ + 16);
}

static void log_graph_entry_tree_oid
                     (const s_log_commit_graph_entry *entry,
                      git_oid *tree_oid)
{
  memcpy(tree_oid->id, log_graph_entry_data(entry), GIT_OID_RAWSZ);
}

static u32 log_graph_rotate_left (u32 value, u32 count)
{
  return (value << count) | (value >> (32 - count));
}

static u32 log_graph_murmur3 (u32 seed, const char *data, uw size,
                              u32 version)
{
  const u32 c1 = 0xcc9e2d51;
  const u32 c2 = 0x1b873593;
  u32 k;
  u32 k1 = 0;
  uw i = 0;
  while (i + 4 <= size) {
    if (version == 2)
      k = (u32) (u8) data[i] |
        (u32) (u8) data[i + 1] << 8 |
        (u32) (u8) data[i + 2] << 16 |
        (u32) (u8) data[i + 3] << 24;
    else
      k = (u32) (s32) (s8) data[i] |
        (u32) (s32) (s8) data[i + 1] << 8 |
        (u32) (s32) (s8) data[i + 2] << 16 |
        (u32) (s32) (s8) data[i + 3] << 24;
    k *= c1;
    k = log_graph_rotate_left(k, 15);
    k *= c2;
    seed ^= k;
    seed = log_graph_rotate_left(seed, 13) * 5 + 0xe6546b64;
    i += 4;
  }
  if (size - i >= 3)
    k1 ^= (version == 2 ? (u32) (u8) data[i + 2] :
           (u32) (s32) (s8) data[i + 2]) << 16;
  if (size - i >= 2)
    k1 ^= (version == 2 ? (u32) (u8) data[i + 1] :
           (u32) (s32) (s8) data[i + 1]) << 8;
  if (size - i >= 1) {
    k1 ^= version == 2 ? (u32) (u8) data[i] :
      (u32) (s32) (s8) data[i];
    k1 *= c1;
    k1 = log_graph_rotate_left(k1, 15);
    k1 *= c2;
    seed ^= k1;
  }
  seed ^= (u32) size;
  seed ^= seed >> 16;
  seed *= 0x85ebca6b;
  seed ^= seed >> 13;
  seed *= 0xc2b2ae35;
  seed ^= seed >> 16;
  return seed;
}

static bool log_graph_bloom_contains (const u8 *filter, uw filter_size,
                                      u32 hash_count, u32 version,
                                      const char *path, uw path_size)
{
  u32 hash_0 = log_graph_murmur3(0x293ae76f, path, path_size,
                                 version);
  u32 hash_1 = log_graph_murmur3(0x7e646e2c, path, path_size,
                                 version);
  u32 i = 0;
  u64 bit_count = filter_size * 8;
  while (i < hash_count) {
    u64 bit = (hash_0 + i * hash_1) % bit_count;
    if (! (filter[bit / 8] & ((u8) 1 << (bit & 7))))
      return false;
    i++;
  }
  return true;
}

static int log_graph_entry_path_maybe_changed
                     (const s_log_commit_graph_entry *entry,
                      const char *path)
{
  const s_log_commit_graph_file *file = entry->file;
  u32 end;
  const u8 *filter;
  uw filter_size;
  u32 hash_count;
  uw i;
  uw path_size;
  u32 start;
  u32 version;
  if (! file->bloom_index || ! file->bloom_data)
    return 1;
  version = log_graph_be32(file->bloom_data);
  hash_count = log_graph_be32(file->bloom_data + 4);
  if ((version != 1 && version != 2) || ! hash_count ||
      hash_count > 32)
    return 1;
  end = log_graph_be32(file->bloom_index + entry->index * 4);
  start = entry->index ?
    log_graph_be32(file->bloom_index + (entry->index - 1) * 4) : 0;
  if (end < start || end > file->bloom_data_size - 12 || end == start)
    return 1;
  filter = file->bloom_data + 12 + start;
  filter_size = end - start;
  path_size = strlen(path);
  while (path_size) {
    if (! log_graph_bloom_contains(filter, filter_size, hash_count,
                                   version, path, path_size))
      return 0;
    i = path_size;
    while (i && path[i - 1] != '/')
      i--;
    if (! i)
      break;
    path_size = i - 1;
  }
  return 1;
}

static uw log_graph_entry_parent_count
                     (const s_log_commit_graph_entry *entry)
{
  const u8 *data = log_graph_entry_data(entry);
  const s_log_commit_graph_file *file = entry->file;
  u32 parent_1 = log_graph_be32(data + GIT_OID_RAWSZ);
  u32 parent_2 = log_graph_be32(data + GIT_OID_RAWSZ + 4);
  uw position;
  uw result;
  u32 value;
  if (parent_1 == 0x70000000)
    return 0;
  if (parent_2 == 0x70000000)
    return 1;
  if (! (parent_2 & 0x80000000))
    return 2;
  position = parent_2 & 0x7fffffff;
  result = 1;
  while (position < file->extra_edge_count) {
    value = log_graph_be32(file->extra_edges + position * 4);
    result++;
    if (value & 0x80000000)
      return result;
    position++;
  }
  return UW_MAX;
}

static int log_graph_entry_parent
                     (const s_log_commit_graph *graph,
                      const s_log_commit_graph_entry *entry,
                      uw index,
                      git_oid *oid,
                      git_oid *tree_oid)
{
  const u8 *data = log_graph_entry_data(entry);
  s_log_commit_graph_entry parent;
  u32 parent_2;
  uw position;
  u32 value;
  if (! index)
    position = log_graph_be32(data + GIT_OID_RAWSZ);
  else {
    parent_2 = log_graph_be32(data + GIT_OID_RAWSZ + 4);
    if (! (parent_2 & 0x80000000)) {
      if (index != 1)
        return -1;
      position = parent_2;
    }
    else {
      position = (parent_2 & 0x7fffffff) + index - 1;
      if (position >= entry->file->extra_edge_count)
        return -1;
      value = log_graph_be32(entry->file->extra_edges + position * 4);
      position = value & 0x7fffffff;
    }
  }
  if (position == 0x70000000 ||
      log_graph_entry_at(graph, position, &parent))
    return -1;
  memcpy(oid->id,
         parent.file->oid_lookup + parent.index * GIT_OID_RAWSZ,
         GIT_OID_RAWSZ);
  log_graph_entry_tree_oid(&parent, tree_oid);
  return 0;
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
      git_object_free(revs.to);
      if (git_object_lookup(&revs.to, s->repo, &base,
                            GIT_OBJECT_COMMIT)) {
        revs.to = NULL;
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
                                  const char *path,
                                  git_odb *odb,
                                  const git_oid *commit_tree_oid,
                                  git_odb_object *commit_tree_object,
                                  git_oid *parent_oid_dest,
                                  git_odb_object **parent_tree_object_dest)
{
  git_commit *parent = NULL;
  int res = -1;
  if (git_commit_parent(&parent, commit, (size_t) i)) {
    goto error;
  }
  res = log_tree_path_changed(odb, commit_tree_oid,
                              git_commit_tree_id(parent), path,
                              commit_tree_object,
                              parent_tree_object_dest);
  if (res >= 0 && parent_oid_dest && parent_tree_object_dest)
    git_oid_cpy(parent_oid_dest, git_commit_id(parent));
 error:
  git_commit_free(parent);
  return res;
}

static int log_tree_entry (git_odb *odb,
                           const git_oid *tree_oid,
                           const char *name,
                           size_t name_size,
                           bool *exists,
                           git_oid *entry_oid,
                           git_filemode_t *entry_filemode)
{
  int result;
  git_odb_object *object = NULL;
  *exists = false;
  if (! tree_oid)
    return 0;
  if (git_odb_read(&object, odb, tree_oid))
    return -1;
  result = log_tree_entry_object(object, name, name_size, exists,
                                 entry_oid, entry_filemode);
  git_odb_object_free(object);
  return result;
}

static int log_tree_entry_object (git_odb_object *object,
                                  const char *name,
                                  size_t name_size,
                                  bool *exists,
                                  git_oid *entry_oid,
                                  git_filemode_t *entry_filemode)
{
  const u8 *data;
  const u8 *end;
  git_filemode_t mode;
  const u8 *p;
  const u8 *space;
  const u8 *zero;
  *exists = false;
  if (! object || git_odb_object_type(object) != GIT_OBJECT_TREE)
    return -1;
  data = git_odb_object_data(object);
  end = data + git_odb_object_size(object);
  p = data;
  while (p < end) {
    mode = 0;
    space = p;
    while (space < end && *space >= '0' && *space <= '7') {
      mode = (git_filemode_t) (mode * 8 + *space - '0');
      space++;
    }
    if (space >= end || *space != ' ')
      return -1;
    zero = space + 1;
    while (zero < end && *zero)
      zero++;
    if (zero >= end ||
        (size_t) (end - zero - 1) < GIT_OID_RAWSZ)
      return -1;
    if ((size_t) (zero - space - 1) == name_size &&
        ! memcmp(space + 1, name, name_size)) {
      memcpy(entry_oid->id, zero + 1, GIT_OID_RAWSZ);
      *entry_filemode = mode;
      *exists = true;
      return 0;
    }
    p = zero + 1 + GIT_OID_RAWSZ;
  }
  return 0;
}

static int log_tree_path_changed (git_odb *odb,
                                  const git_oid *a,
                                  const git_oid *b,
                                  const char *path,
                                  git_odb_object *a_root_object,
                                  git_odb_object **b_root_object_dest)
{
  bool a_entry_exists;
  git_filemode_t a_entry_filemode;
  git_oid a_entry_oid = {0};
  git_odb_object *a_root_owned = NULL;
  git_oid a_tree_oid = {0};
  bool a_tree_exists = a != NULL;
  bool b_entry_exists;
  git_filemode_t b_entry_filemode;
  git_oid b_entry_oid = {0};
  git_odb_object *b_root_owned = NULL;
  git_oid b_tree_oid = {0};
  bool b_tree_exists = b != NULL;
  bool first = true;
  int result = -1;
  const char *slash;
  size_t size;
  if (b_root_object_dest)
    *b_root_object_dest = NULL;
  if (a)
    git_oid_cpy(&a_tree_oid, a);
  if (b)
    git_oid_cpy(&b_tree_oid, b);
  if (a && ! a_root_object &&
      git_odb_read(&a_root_owned, odb, a))
    goto clean;
  if (b && git_odb_read(&b_root_owned, odb, b))
    goto clean;
  while (1) {
    slash = strchr(path, '/');
    size = slash ? (size_t) (slash - path) : strlen(path);
    if (first) {
      if (a_tree_exists &&
          log_tree_entry_object(a_root_object ? a_root_object :
                                a_root_owned, path, size,
                                &a_entry_exists, &a_entry_oid,
                                &a_entry_filemode))
        goto clean;
      if (! a_tree_exists)
        a_entry_exists = false;
      if (b_tree_exists &&
          log_tree_entry_object(b_root_owned, path, size,
                                &b_entry_exists, &b_entry_oid,
                                &b_entry_filemode))
        goto clean;
      if (! b_tree_exists)
        b_entry_exists = false;
    }
    else if (log_tree_entry(odb,
                            a_tree_exists ? &a_tree_oid : NULL,
                            path, size, &a_entry_exists, &a_entry_oid,
                            &a_entry_filemode) ||
             log_tree_entry(odb,
                            b_tree_exists ? &b_tree_oid : NULL,
                            path, size, &b_entry_exists, &b_entry_oid,
                            &b_entry_filemode))
      goto clean;
    if (! a_entry_exists && ! b_entry_exists) {
      result = 0;
      goto clean;
    }
    if (a_entry_exists && b_entry_exists &&
        a_entry_filemode == b_entry_filemode &&
        git_oid_equal(&a_entry_oid, &b_entry_oid)) {
      result = 0;
      goto clean;
    }
    if (! slash) {
      result = 1;
      goto clean;
    }
    a_tree_exists = a_entry_exists &&
      a_entry_filemode == GIT_FILEMODE_TREE;
    b_tree_exists = b_entry_exists &&
      b_entry_filemode == GIT_FILEMODE_TREE;
    if (a_tree_exists)
      git_oid_cpy(&a_tree_oid, &a_entry_oid);
    if (b_tree_exists)
      git_oid_cpy(&b_tree_oid, &b_entry_oid);
    path = slash + 1;
    first = false;
  }
 clean:
  if (result >= 0 && b_root_object_dest) {
    *b_root_object_dest = b_root_owned;
    b_root_owned = NULL;
  }
  git_odb_object_free(a_root_owned);
  git_odb_object_free(b_root_owned);
  return result;
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
  map = &tmp->tag.data.td_map;
  tag_init_psym(map->key + 0, &g_sym_author_email);
  tag_init_psym(map->key + 1, &g_sym_message);
  tag_init_psym(map->key + 2, &g_sym_author_name);
  tag_init_psym(map->key + 3, &g_sym_parents);
  tag_init_psym(map->key + 4, &g_sym_hash);
  tag_init_psym(map->key + 5, &g_sym_date);
  p_list parents = NULL;
  git_oid_tostr(buf, sizeof(buf), git_commit_id(commit));
  if (! tag_init_str_1_alloc(map->value + 1,
                             git_commit_message(commit) ?
                             git_commit_message(commit) : "") ||
      ! tag_init_str_1_alloc(map->value + 4, buf))
    goto ko;
  tag_init_plist(map->value + 3, NULL);
  if ((count = git_commit_parentcount(commit)) > 1) {
    for (i = 0; i < count; ++i) {
      git_oid_tostr(buf, sizeof(buf),
                    git_commit_parent_id(commit, i));
      parents = list_new(parents);
      if (! parents)
        goto ko;
      map->value[3].data.td_plist = parents;
      if (! tag_init_str_1_alloc(&parents->tag, buf))
        goto ko;
    }
  }
  if ((sig = git_commit_author(commit)) != NULL) {
    tag_init_s32(map->value + 5, sig->when.time +
                 sig->when.offset * 60);
    if (! tag_init_str_1_alloc(map->value + 2,
                               sig->name ? sig->name : "") ||
        ! tag_init_str_1_alloc(map->value + 0,
                               sig->email ? sig->email : ""))
      goto ko;
  }
  else {
    tag_init(map->value + 0);
    tag_init(map->value + 2);
    tag_init(map->value + 5);
  }
  *log_tail = tmp;
  return &(*log_tail)->next.data.td_plist;
 ko:
  list_delete_all(tmp);
  return NULL;
}
