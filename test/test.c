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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

#define TARGET_NAME_MAX 32
#define TARGETS_MAX     1000

#define TEST_KO_MAX     10

long         g_test_assert_count = 0;
long         g_test_assert_ko = 0;
long         g_test_assert_ok = 0;
const char  *g_test_case_name = NULL;
const char  *g_test_context = NULL;
long         g_test_count = 0;
long         g_test_ko = 0;
long         g_test_last_ok = 0;
long         g_test_ok = 0;
const char  *g_test_targets_default[] = {NULL};
char         g_test_targets_env[TARGETS_MAX * TARGET_NAME_MAX];
char        *g_test_targets_env_v[TARGETS_MAX + 1];
const char **g_test_targets = {NULL};

static char * test_strsep (char **p, const char *delim);

void test_clean (void)
{
}

void test_context (const char *context)
{
  g_test_context = context;
}

int test_file_compare (const char *path_a, const char *path_b)
{
  FILE *fp_a = NULL;
  FILE *fp_b = NULL;
  char buf_a[1024];
  char buf_b[1024];
  unsigned lineno = 0;
  if (path_a == path_b)
    goto ok;
  if (! (fp_a = fopen(path_a, "rb")))
    goto ko;
  if (! (fp_b = fopen(path_b, "rb")))
    goto ko;
  while (1) {
    lineno++;
    if (! fgets(buf_a, sizeof(buf_a), fp_a)) {
      if (fgets(buf_b, sizeof(buf_b), fp_b))
        goto ko;
      goto ok;
    }
    if (! fgets(buf_b, sizeof(buf_b), fp_b))
      goto ko;
    if (strncmp(buf_a, buf_b, sizeof(buf_a)))
      goto ko;
  }
  /* never reached */
 ko:
  fclose(fp_a);
  fclose(fp_b);
  fprintf(stderr, "\n%sFiles %s and %s differ line %u.%s",
          TEST_COLOR_KO,
          path_a, path_b, lineno,
          TEST_COLOR_RESET);
  test_ko();
  return 1;
 ok:
  fclose(fp_a);
  fclose(fp_b);
  test_ok();
  return 0;
}

void test_init (char *argv0, int *argc, char ***argv)
{
  const char **t;
  if ((*argv)[(*argc)] != NULL)
    err(1, "argv[argc] != NULL");
  g_test_context = NULL;
  g_test_count = 0;
  g_test_ko = 0;
  g_test_last_ok = 0;
  g_test_ok = 0;
  g_test_targets = g_test_targets_default;
  if (*argc > 1) {
    g_test_targets = (const char **) (*argv) + 1;
  }
  else {
    const char *env_target;
    env_target = getenv("KC3_TEST");
    if (env_target) {
      char **ap;
      size_t len = 0;
      char *target;
      len = strlen(env_target);
      target = g_test_targets_env;
      if (len + 1 > sizeof(g_test_targets_env))
        err(1, "too many targets");
      memcpy(target, env_target, len + 1);
      ap = g_test_targets_env_v;
      while (ap < g_test_targets_env_v + TARGETS_MAX &&
             (*ap = test_strsep(&target, " \t")) != NULL &&
             **ap)
        ap++;
      *ap = NULL;
      g_test_targets = (const char **) g_test_targets_env_v;
    }
  }
  printf("%s", argv0);
  t = g_test_targets;
  while (*t) {
    printf(" %s", *t);
    t++;
  }
  fflush(stdout);
}

void test_ko (void)
{
  g_test_assert_count++;
  g_test_assert_ko++;
  g_test_count++;
  g_test_ko++;
  fprintf(stderr, "%sF%s", TEST_COLOR_KO, TEST_COLOR_RESET);
  if (g_test_context) {
    fprintf(stderr, "\n%s%s%s", TEST_COLOR_KO, g_test_context,
            TEST_COLOR_RESET);
    g_test_context = NULL;
  }
  g_test_last_ok = 0;
  if (g_test_ko >= TEST_KO_MAX) {
    fprintf(stderr, "\nMaximum number of errors reached, exiting.\n");
    exit(1);
  }
}

void test_ok (void)
{
  g_test_count++;
  g_test_ok++;
  fprintf(stderr, "%s.%s", TEST_COLOR_OK, TEST_COLOR_RESET);
  g_test_last_ok = 1;
}

void test_summary (void)
{
  fprintf(stderr, "\n");
  printf("Total %ld tests. "
         "%sOK %ld (%.1f%%)%s. "
         "%sKO %ld (%.1f%%)%s.\n",
         g_test_count,
         TEST_COLOR_OK,
         g_test_ok,
         g_test_ok * 100.0f / g_test_count,
         TEST_COLOR_RESET,
         TEST_COLOR_KO,
         g_test_ko,
         g_test_ko * 100.0f / g_test_count,
         TEST_COLOR_RESET);
}

static char * test_strsep (char **p, const char *delim)
{
  char *d;
  char *start;
  if (! p || ! *p || ! delim)
    return NULL;
  d = start = *p;
  while (*d && ! strchr(delim, *d))
    d++;
  if (*d) {
    *d = 0;
    *p = d + 1;
  }
  else
    *p = d;
  return start;
}

int test_target (const char *target)
{
  const char **i;
  i = g_test_targets;
  if (! *i)
    return 1;
  while (*i) {
    if (strcmp(target, *i) == 0)
      return 1;
    i++;
  }
  return 0;
}
