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
#include <errno.h>
#include <string.h>
#include <libkc3/kc3.h>
#include "config.h"

#if HAVE_WINEDITLINE
# include "buf_wineditline.h"
#else
# include "buf_linenoise.h"
#endif

#define BUFSZ 0x10000

bool  g_client = false;
s_str g_host = {0};
s_str g_port = {0};
bool  g_server = false;

sw  buf_ignore_character (s_buf *buf);
int ikc3_arg_client (s_env *env, int *argc, char ***argv);
int ikc3_arg_dump (s_env *env, int *argc, char ***argv);
int ikc3_arg_load (s_env *env, int *argc, char ***argv);
int ikc3_arg_server (s_env *env, int *argc, char ***argv);
sw  ikc3_run (void);
int usage (char *argv0);

sw buf_ignore_character (s_buf *buf)
{
  u8 b;
  character c = 0;
  sw csize;
  sw r;
  if ((r = buf_peek_character_utf8(buf, &c)) > 0)
    csize = r;
  else if ((r = buf_peek_u8(buf, &b)) > 0)
    csize = 1;
  else
    return r;
  if ((r = buf_ignore(buf, csize)) < 0)
    return r;
  return csize;
}

int ikc3_arg_client (s_env *env, int *argc, char ***argv)
{
  if (g_server) {
    err_puts("ikc3_arg_client: --client and --server are mutually"
             " exclusive");
    usage(env->argv[0]);
    return -1;
  }
  if (g_client) {
    err_puts("ikc3_arg_client: --client can only be set once");
    usage(env->argv[0]);
    return -1;
  }
  if (*argc < 3 || ! (*argv)[0] || ! (*argv)[1] || ! (*argv)[2]) {
    err_puts("ikc3_arg_client: --client invalid argument");
    usage(env->argv[0]);
    return -1;
  }
  str_init_1(&g_host, NULL, (*argv)[1]);
  str_init_1(&g_port, NULL, (*argv)[2]);
  g_client = true;
  *argc -= 3;
  *argv += 3;
  return 3;
}

int ikc3_arg_dump (s_env *env, int *argc, char ***argv)
{
  s_str path = {0};
  if (*argc < 2 || ! (*argv)[0] || ! (*argv)[1]) {
    err_puts("ikc3_arg_dump: invalid argument");
    usage(env->argv[0]);
    return -1;
  }
  str_init_1(&path, NULL, (*argv)[1]);
  if (! env_dump(env, &path)) {
    str_clean(&path);
    return -1;
  }
  str_clean(&path);
  *argc -= 2;
  *argv += 2;
  return 2;
}

int ikc3_arg_load (s_env *env, int *argc, char ***argv)
{
  sw     e = 0;
  FILE *fp = 0;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  s_str path = {0};
  sw r;
  if (*argc < 2) {
    err_puts("ikc3_arg_load: --load without an argument");
    assert(! "ikc3_arg_load: --load without an argument");
    return -1;
    }
  if (env->trace) {
    err_write_1("ikc3_arg_load: ");
    err_write_1((*argv)[1]);
    err_write_1("\n");
  }
  str_init_1(&path, NULL, (*argv)[1]);
  fp = file_open(&path, "rb");
  if (! fp) {
    e = errno;
    err_write_1("ikc3: ");
    err_inspect_str(&path);
    err_write_1(": ");
    err_write_1(strerror(e));
    err_write_1("\n");
    assert(! "ikc3: fopen");
    return 1;
  }
  str_clean(&path);
  if (! buf_file_open_r(env->in, fp)) {
    return -1;
  }
  file_dir = frame_get_w(env->global_frame, &g_sym___DIR__);
  file_dir_save = *file_dir;
  file_path = frame_get_w(env->global_frame, &g_sym___FILE__);
  file_path_save = *file_path;
  tag_init_str_1(file_path, NULL, (*argv)[1]);
  file_dir->type = TAG_STR;
  if (! file_dirname(&file_path->data.str, &file_dir->data.str)) {
    buf_file_close(env->in);
    return -1;
  }
  r = ikc3_run();
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  buf_file_close(env->in);
  fclose(fp);
  if (r)
    return r;
  *argc -= 2;
  *argv += 2;
  return 0;
}

int ikc3_arg_server (s_env *env, int *argc, char ***argv)
{
  if (g_client) {
    err_puts("ikc3_arg_server: --client and --server are mutually"
             " exclusive");
    usage(env->argv[0]);
    return -1;
  }
  if (g_server) {
    err_puts("ikc3_arg_server: --server can only be set once");
    usage(env->argv[0]);
    return -1;
  }
  if (*argc < 3 || ! (*argv)[0] || ! (*argv)[1] || ! (*argv)[2]) {
    err_puts("ikc3_arg_server: --server invalid argument");
    usage(env->argv[0]);
    return -1;
  }
  str_init_1(&g_host, NULL, (*argv)[1]);
  str_init_1(&g_port, NULL, (*argv)[2]);
  g_server = true;
  *argc -= 3;
  *argv += 3;
  return 3;
}

sw ikc3_run (void)
{
  s_env *env;
  s_tag input;
  sw r;
  s_tag result;
  env = env_global();
  while (1) {
    r = buf_ignore_spaces(env->in);
    if (r < 0)
      return 0;
    r = buf_parse_comments(env->in);
    if (r < 0)
      return 0;
    r = buf_parse_tag(env->in, &input);
    if (r > 0) {
      if (! eval_tag(&input, &result)) {
        tag_clean(&input);
        continue;
      }
      if (buf_inspect_tag(env->out, &result) < 0) {
	tag_clean(&input);
	tag_clean(&result);
        return 0;
      }
      tag_clean(&input);
      tag_clean(&result);
    }
    if (r < 0 ||
        (r == 0 &&
         (r = buf_ignore_character(env->in)) <= 0))
      return 0;
    if ((r = buf_write_1(env->out, "\n")) < 0)
      return 0;
    if ((r = buf_flush(env->out)) < 0)
      return 0;
  }
  return 0;
}

int main (int argc, char **argv)
{
  s_env *env;
  s_buf in_original;
  sw r;
  if (argc < 1)
    return usage("ikc3");
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  env = env_global();
  in_original = *env->in;
  while (argc) {
    if (! strcmp("--client", *argv)) {
      if ((r = ikc3_arg_client(env, &argc, &argv)) < 0)
        goto clean;
    }
    else if (! strcmp(argv[0], "--dump")) {
      if ((r = ikc3_arg_dump(env, &argc, &argv)) < 0)
        goto clean;
    }
    else if (! strcmp("--load", *argv) ||
             ! strcmp("-l", *argv)) {
      if ((r = ikc3_arg_load(env, &argc, &argv)) < 0)
        goto clean;
    }
    else if (! strcmp("--server", *argv)) {
      if ((r = ikc3_arg_server(env, &argc, &argv)) < 0)
        goto clean;
    }
    else if (argc == 1 &&
             ! strcmp("--quit", *argv)) {
      r = 0;
      goto clean;
    }
    else
      break;
  }
  *env->in = in_original;
#if HAVE_WINEDITLINE
  buf_wineditline_open_r(env->in, "ikc3> ", ".ikc3_history");
#else
  buf_linenoise_open_r(env->in, "ikc3> ", ".ikc3_history");
#endif
  if (g_client) {
    // TODO: open env->in and env->out from socket_buf
  }
  if (g_server) {
    // TODO: open env->in and env->out from socket_buf
  }
  r = ikc3_run();
  if (g_client) {
    // TODO: restore env->in and env->out and close socket_buf
  }
  if (g_server) {
    // TODO: restore env->in and env->out and close socket_buf
  }
#if HAVE_WINEDITLINE
  buf_wineditline_close(env->in, ".ikc3_history");
#else
  buf_linenoise_close(env->in, ".ikc3_history");
#endif
 clean:
  *env->in = in_original;
  kc3_clean(NULL);
  return r;
}

int usage (char *argv0)
{
  printf("Usage: %s [--load FILE] [--client HOST PORT]\n"
         "  [--server HOST PORT] [--dump FILE] [--quit]\n",
         argv0);
  return 1;
}
