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
#include <string.h>
#include "../libkc3/kc3.h"
#include "buf_readline.h"

#define BUFSZ 0x10000

int usage (char *argv0);

const char *g_argv0_default = PROG;
const char *g_argv0_dir_default = PREFIX;

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

sw buf_xfer_spaces (s_buf *out, s_buf *in)
{
  character c;
  sw csize;
  sw r;
  sw size = 0;
  assert(in);
  assert(out);
  while ((r = buf_peek_character_utf8(in, &c)) > 0 &&
         c < UCD_MAX &&
         g_ucd[c].flags & (UCD_OTHER_CONTROL | UCD_SEPARATOR_SPACE)) {
    csize = r;
    if ((r = buf_xfer(out, in, csize)) != csize)
      return -1;
    size += csize;
    if ((r = buf_flush(out)) < 0)
      return -1;
  }
  if (r < 0)
    return r;
  return size;
}

sw kc3s_run (void)
{
  s_env *env;
  s_tag input;
  sw r;
  s_tag result;
  env = env_global();
  while (1) {
    if ((r = buf_parse_comments(env->in)) < 0)
      break;
    if ((r = buf_ignore_spaces(env->in)) < 0)
      break;
    if ((r = buf_parse_tag(env->in, &input)) < 0)
      break;
    if (r > 0) {
      if (! eval_tag(&input, &result)) {
        tag_clean(&input);
        continue;
      }
      tag_clean(&input);
      tag_clean(&result);
    }
    else
      if ((r = buf_ignore_character(env->in)) <= 0)
        break;
  }
  return 0;
}

int main (int argc, char **argv)
{
  s_env *env;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  FILE *fp;
  char  in_data[BUF_SIZE];
  s_buf in_original;
  s_str path = {0};
  sw r;
  if (argc < 1)
    return usage(argv[0]);
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  env = env_global();
  in_original = *env->in;
  buf_init(env->in, false, sizeof(in_data), in_data);
  while (argc) {
    if (! strcmp("--load", *argv) ||
        ! strcmp("-l", *argv)) {
      if (argc < 2) {
        err_write_1(env->argv[0]);
        err_write_1(": ");
        err_write_1(*argv);
        err_write_1(" without an argument\n");
        assert(! "env_init: -l or --load without an argument");
        r = 1;
        goto clean;
      }
      str_init_1(&path, NULL, argv[1]);
      fp = file_open(&path, "rb");
      if (! fp) {
        r = -1;
        str_clean(&path);
        goto clean;
      }
      str_clean(&path);
      if (! buf_file_open_r(env->in, fp)) {
        r = -1;
        goto clean;
      }
      file_dir = frame_get_w(env->global_frame, &g_sym___DIR__);
      file_dir_save = *file_dir;
      file_path = frame_get_w(env->global_frame, &g_sym___FILE__);
      file_path_save = *file_path;
      tag_init_str_1(file_path, NULL, argv[1]);
      file_dir->type = TAG_STR;
      if (! file_dirname(&file_path->data.str, &file_dir->data.str)) {
        buf_file_close(env->in);
        r = -1;
        goto clean;
      }
      r = kc3s_run();
      *file_dir = file_dir_save;
      *file_path = file_path_save;
      buf_file_close(env->in);
      fclose(fp);
      if (r)
        goto clean;
      argc -= 2;
      argv += 2;
    }
    else if (argc > 1 && argv[0] && argv[1] &&
             ! strcmp(argv[0], "--dump")) {
      str_init_1(&path, NULL, argv[1]);
      if (! env_dump(env, &path)) {
        r = 1;
        str_clean(&path);
        goto clean;
      }
      str_clean(&path);
      argc -= 2;
      argv += 2;
    }
    else if (argc == 1 && ! strcmp("--quit", *argv)) {
      r = 0;
      goto clean;
    }
    else
      break;
  }
  *env->in = in_original;
  r = kc3s_run();
 clean:
  *env->in = in_original;
  kc3_clean(NULL);
  return r;
}

int usage (char *argv0)
{
  printf("Usage: %s\n", argv0);
  return 1;
}
