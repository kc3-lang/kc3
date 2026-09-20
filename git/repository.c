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
#include <sys/stat.h>
#include <git2.h>
#include <git2/sys/commit_graph.h>
#include <git2/sys/errors.h>
#include <git2/sys/repository.h>
#include "git.h"
#include "repository.h"

#define REPOSITORY_ODB_CACHE_MAX 1024

typedef struct repository_graph_state s_repository_graph_state;
typedef struct repository_odb_cache_entry s_repository_odb_cache_entry;

struct repository_graph_state {
  dev_t device;
  ino_t inode;
  off_t size;
  time_t mtime;
  bool exists;
};

struct repository_odb_cache_entry {
  git_odb *odb;
  char objects_path[PATH_MAX];
  s_repository_graph_state graph_state;
  u64 used;
};

static s_repository_odb_cache_entry
  g_repository_odb_cache[REPOSITORY_ODB_CACHE_MAX] = {0};
static pthread_mutex_t g_repository_odb_cache_mutex =
  PTHREAD_MUTEX_INITIALIZER;
static u64 g_repository_odb_cache_clock = 0;

static void repository_commit_graph_init (git_odb *odb,
                                          const char *objects_path);
static void repository_graph_state_init
  (s_repository_graph_state *state, const char *objects_path);
static bool repository_graph_state_equal
  (const s_repository_graph_state *a,
   const s_repository_graph_state *b);
static void repository_odb_cache_attach (git_repository *repo);
static void repository_odb_cache_clean (void);
static void repository_odb_cache_lock (void);
static void repository_odb_cache_unlock (void);

static void repository_commit_graph_init (git_odb *odb,
                                          const char *objects_path)
{
#if LIBGIT2_VERSION_CHECK(1, 2, 0)
  git_commit_graph *graph = NULL;
# if LIBGIT2_VERSION_CHECK(2, 0, 0) || defined(GIT_EXPERIMENTAL_SHA256)
  git_commit_graph_open_options options =
    GIT_COMMIT_GRAPH_OPEN_OPTIONS_INIT;
# endif
  if (
# if LIBGIT2_VERSION_CHECK(2, 0, 0) || defined(GIT_EXPERIMENTAL_SHA256)
      git_commit_graph_open(&graph, objects_path, &options) ||
# else
      git_commit_graph_open(&graph, objects_path) ||
# endif
      git_odb_set_commit_graph(odb, graph))
    goto clean;
  graph = NULL;
 clean:
  git_error_clear();
  git_commit_graph_free(graph);
#else
  (void) odb;
  (void) objects_path;
#endif
}

static void repository_graph_state_init
  (s_repository_graph_state *state, const char *objects_path)
{
  char path[PATH_MAX];
  struct stat st;
  *state = (s_repository_graph_state) {0};
  if (snprintf(path, sizeof(path),
               "%s/info/commit-graphs/commit-graph-chain",
               objects_path) >= (int) sizeof(path) ||
      stat(path, &st)) {
    if (snprintf(path, sizeof(path), "%s/info/commit-graph",
                 objects_path) >= (int) sizeof(path) ||
        stat(path, &st))
      return;
  }
  state->device = st.st_dev;
  state->inode = st.st_ino;
  state->size = st.st_size;
  state->mtime = st.st_mtime;
  state->exists = true;
}

static bool repository_graph_state_equal
  (const s_repository_graph_state *a,
   const s_repository_graph_state *b)
{
  return a->exists == b->exists &&
    (! a->exists ||
     (a->device == b->device &&
      a->inode == b->inode &&
      a->size == b->size &&
      a->mtime == b->mtime));
}

static void repository_odb_cache_attach (git_repository *repo)
{
  git_odb *candidate = NULL;
  s_repository_odb_cache_entry *entry;
  uw i;
  uw oldest = 0;
  git_odb *old_odb = NULL;
  git_buf objects = GIT_BUF_INIT;
  s_repository_graph_state state;
  if (git_repository_item_path(&objects, repo,
                               GIT_REPOSITORY_ITEM_OBJECTS) ||
      ! objects.ptr || strlen(objects.ptr) >= PATH_MAX)
    goto clean;
  repository_graph_state_init(&state, objects.ptr);
  repository_odb_cache_lock();
  i = 0;
  while (i < REPOSITORY_ODB_CACHE_MAX) {
    entry = g_repository_odb_cache + i;
    if (entry->odb && ! strcmp(entry->objects_path, objects.ptr) &&
        repository_graph_state_equal(&entry->graph_state, &state)) {
      entry->used = ++g_repository_odb_cache_clock;
      if (git_repository_set_odb(repo, entry->odb))
        git_error_clear();
      repository_odb_cache_unlock();
      goto clean;
    }
    i++;
  }
  repository_odb_cache_unlock();
  if (git_repository_odb(&candidate, repo))
    goto clean;
  repository_commit_graph_init(candidate, objects.ptr);
  repository_odb_cache_lock();
  i = 0;
  while (i < REPOSITORY_ODB_CACHE_MAX) {
    entry = g_repository_odb_cache + i;
    if (entry->odb && ! strcmp(entry->objects_path, objects.ptr) &&
        repository_graph_state_equal(&entry->graph_state, &state)) {
      entry->used = ++g_repository_odb_cache_clock;
      if (git_repository_set_odb(repo, entry->odb))
        git_error_clear();
      repository_odb_cache_unlock();
      goto clean;
    }
    if (! entry->odb) {
      oldest = i;
      break;
    }
    if (entry->used < g_repository_odb_cache[oldest].used)
      oldest = i;
    i++;
  }
  entry = g_repository_odb_cache + oldest;
  old_odb = entry->odb;
  *entry = (s_repository_odb_cache_entry) {0};
  entry->odb = candidate;
  candidate = NULL;
  memcpy(entry->objects_path, objects.ptr, strlen(objects.ptr) + 1);
  entry->graph_state = state;
  entry->used = ++g_repository_odb_cache_clock;
  repository_odb_cache_unlock();
  git_odb_free(old_odb);
 clean:
  git_error_clear();
  git_odb_free(candidate);
  git_buf_dispose(&objects);
}

static void repository_odb_cache_clean (void)
{
  git_odb *odb[REPOSITORY_ODB_CACHE_MAX] = {0};
  uw i = 0;
  repository_odb_cache_lock();
  while (i < REPOSITORY_ODB_CACHE_MAX) {
    odb[i] = g_repository_odb_cache[i].odb;
    g_repository_odb_cache[i] = (s_repository_odb_cache_entry) {0};
    i++;
  }
  g_repository_odb_cache_clock = 0;
  repository_odb_cache_unlock();
  i = 0;
  while (i < REPOSITORY_ODB_CACHE_MAX) {
    git_odb_free(odb[i]);
    i++;
  }
}

static void repository_odb_cache_lock (void)
{
  if (pthread_mutex_lock(&g_repository_odb_cache_mutex)) {
    err_puts("repository_odb_cache_lock: pthread_mutex_lock");
    abort();
  }
}

static void repository_odb_cache_unlock (void)
{
  if (pthread_mutex_unlock(&g_repository_odb_cache_mutex)) {
    err_puts("repository_odb_cache_unlock: pthread_mutex_unlock");
    abort();
  }
}

s32 kc3_git_init (void)
{
  s32 result = git_libgit2_init();
  if (result > 0)
    git_libgit2_opts(GIT_OPT_SET_CACHE_OBJECT_LIMIT, GIT_OBJECT_TREE,
                     1024 * 1024);
  return result;
}

s32 kc3_git_shutdown (void)
{
  repository_odb_cache_clean();
  return git_libgit2_shutdown();
}

void kc3_git_repository_free (git_repository **repo)
{
  if (repo && *repo) {
    git_repository_free(*repo);
    *repo = NULL;
  }
}

git_reference ** kc3_git_repository_head (git_repository **repo,
                                          git_reference **dest)
{
  const git_error *e;
  git_reference *tmp = NULL;
  if (git_repository_head(&tmp, *repo)) {
    if (false) {
      e = git_error_last();
      err_write_1("kc3_git_repository_head: ");
      err_puts(e->message);
    }
    *dest = NULL;
  }
  else
    *dest = tmp;
  return dest;
}

git_repository ** kc3_git_repository_init (git_repository **repo,
                                           s_str *path)
{
  const git_error *e;
  git_repository *tmp;
  if (git_repository_init(&tmp, path->ptr.p_pchar, false)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_init: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
    return NULL;
  }
  repository_odb_cache_attach(tmp);
  *repo = tmp;
  return repo;
}

git_repository ** kc3_git_repository_init_bare (git_repository **repo,
                                                s_str *path)
{
  const git_error *e;
  git_repository *tmp;
  if (git_repository_init(&tmp, path->ptr.p_pchar, true)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_init_bare: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
    return NULL;
  }
  repository_odb_cache_attach(tmp);
  *repo = tmp;
  return repo;
}

git_repository ** kc3_git_repository_open (git_repository **repo,
                                           s_str *path)
{
  const git_error *e;
  git_repository *tmp;
  if (git_repository_open(&tmp, path->ptr.p_pchar)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_open: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
    return NULL;
  }
  repository_odb_cache_attach(tmp);
  *repo = tmp;
  return repo;
}

git_repository ** kc3_git_repository_open_bare (git_repository **repo,
                                                s_str *path)
{
  const git_error *e;
  git_repository *tmp;
  if (git_repository_open_bare(&tmp, path->ptr.p_pchar)) {
    e = git_error_last();
    err_write_1("kc3_git_repository_open_bare: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(e->message);
    return NULL;
  }
  repository_odb_cache_attach(tmp);
  *repo = tmp;
  return repo;
}
