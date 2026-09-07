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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../libkc3/kc3.h"

static sw usage (const char *argv0);

static sw kc3txt_buf_inspect_log (s_buf *out, uw id, u8 action, s_fact *fact)
{
  static const char *action_str[] = {" ADD ", " REMOVE ", " REPLACE "};
  sw r;
  sw result = 0;
  assert(out);
  if ((r = buf_inspect_uw_decimal(out, id)) <= 0)
    return r;
  result += r;
  if (action >= 3) {
    err_puts("kc3txt_buf_inspect_log: unknown action");
    return -1;
  }
  if ((r = buf_write_1(out, action_str[action])) <= 0)
    return r;
  result += r;
  if ((r = buf_inspect_fact(out, fact)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(out, "\n")) <= 0)
    return r;
  result += r;
  return result;
}

static sw kc3txt_from_dump (s_env *env,
                            const char *file_path)
{
  char b[BUF_SIZE];
  s_buf out;
  char dump_dir[1024] = ".";
  const char *slash;
  slash = strrchr(file_path, '/');
  if (slash) {
    size_t len = slash - file_path;
    if (len >= sizeof(dump_dir))
      len = sizeof(dump_dir) - 1;
    memcpy(dump_dir, file_path, len);
    dump_dir[len] = 0;
  }
  char db_path[1100];
  snprintf(db_path, sizeof(db_path), "%s/db", dump_dir);
  if (access(db_path, F_OK) == 0) {
    char log_path[2200];
    snprintf(log_path, sizeof(log_path), "%s/app.facts.bin.facts", db_path);
    if (slash && chdir(dump_dir) != 0) {
      fprintf(stderr, "%s: cannot chdir to %s\n", PROG, dump_dir);
      return 1;
    }
  }
  buf_init(&out, false, sizeof(b), b);
  buf_file_open_w(&out, stdout);
  facts_dump(env->facts, &out);
  buf_flush(&out);
  buf_file_close(&out);
  buf_clean(&out);
  kc3_clean(NULL);
  return 0;
}

int main (int argc, char **argv)
{
  u8 action;
  char b[BUF_SIZE];
  s_env *env = NULL;
  s_fact fact;
  const char *file_path;
  uw file_path_len;
  uw id;
  s_buf out;
  s_str path;
  g_env_argv0_default = PROG;
  g_env_argv0_dir_default = PREFIX;
  if (argc < 2)
    return usage(PROG);
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  env = env_global();
  file_path = argv[0];
  file_path_len = strlen(file_path);
  if (file_path_len >= 5 && ! strncmp(file_path + file_path_len - 5, ".dump", 5))
    return kc3txt_from_dump(env, file_path);
  else {
    str_init_1(&path, NULL, argv[0]);
    s_marshall_read mr = {0};
    sw r;
    if ( ! marshall_read_init_file(&mr, &path)) {
      fprintf(stderr, "Failed to open file: %s\n", argv[0]);
      str_clean(&path);
      return 1;
    };
    buf_init(&out, false, sizeof(b), b);
    buf_file_open_w(&out, stdout);
    while (1) {
      while ((r = buf_peek_1(mr.buf, "KC3MARSH")) == 0) {
        if (buf_peek_1(mr.buf, "_KC3UW_") <= 0) goto done;
        if (! marshall_read_uw(&mr, false, &id) ||
            ! marshall_read_u8(&mr, false, &action) ||
            ! marshall_read_fact(&mr, false, &fact))
          goto done;
        kc3txt_buf_inspect_log(&out, id, action, &fact);
        fact_clean_all(&fact);
      }
      if (r > 0) {
        marshall_read_chunk_file(&mr);
        continue;
      }
      break;
    }
  done:
    buf_flush(&out);
    buf_file_close(&out);
    marshall_read_clean(&mr);
    str_clean(&path);
  }
  kc3_clean(NULL);
  return 0;
}

static sw usage (const char *argv0)
{
  fprintf(stderr, "Usage: %s FILE\n", argv0);
  return 1;
}
