/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <libkc3/kc3.h>
#include "config.h"

int main (int argc, char **argv)
{
  bool    daemonize = true;
  s_ident daemonize_ident;
  s_tag   daemonize_value;
  s32 e;
  char  log_buf[64] = {0};
  s32   log_fd = -1;
  s_str log_str = {0};
  s_call call = {0};
  const s_sym *module = NULL;
  char *p;
  s32 r = 1;
  s32 skip = 0;
  s_str str;
  time_t t;
  s_tag tmp = {0};
  const struct tm *utc = NULL;
  kc3_init(NULL, &argc, &argv);
  g_kc3_env->trace = true;
  while (argc > 0 && argv[0] && argv[0][0] == '-') {
    skip = 1;
    p = argv[0] + 1;
    while (*p) {
      switch (*p) {
      case 'D':
        if (argc <= skip || ! argv[skip]) {
          err_puts("kc3_httpd: -D without an argument");
          assert(! "kc3_httpd: -D without an argument");
          kc3_clean(NULL);
          return 1;
        }
        str_init_1(&str, NULL, argv[1]);
        if (! facts_open_file(g_kc3_env->facts, &str)) {
          err_puts("kc3_httpd: -D: facts_open_file");
          assert(! "kc3_httpd: -D: facts_open_file");
          kc3_clean(NULL);
          return 1;
        }
        skip++;
        break;
      case 'd':
        daemonize = false;
        break;
      case 'C':
        if (argc <= skip || ! argv[skip]) {
          err_puts("kc3_httpd: -C without an argument");
          assert(! "kc3_httpd: -C without an argument");
          kc3_clean(NULL);
          return 1;
        }
        if (chdir(argv[skip])) {
          e = errno;
          err_write_1("kc3_httpd: chdir: ");
          err_write_1(argv[1]);
          err_write_1(": ");
          err_puts(strerror(e));
          assert(! "kc3_httpd: chdir");
          kc3_clean(NULL);
          return 1;
        }
        skip++;
        break;
      default:
        err_write_1("kc3_httpd: unknown flag: -");
        err_write_u8(*p);
        err_write_1("\n");
        assert(! "kc3_httpd: unknown flag");
        kc3_clean(NULL);
        return 1;
      }
      p++;
    }
    argc -= skip;
    argv += skip;
  }
  if (daemonize) {
    if ((t = time(NULL)) == -1) {
      kc3_clean(NULL);
      return 1;
    }
    if (! (utc = gmtime(&t))) {
      kc3_clean(NULL);
      return 1;
    }
    strftime(log_buf, sizeof(log_buf) - 1, "log/kc3_httpd_%Y-%m-%d_%H:%M:%S.log",
             utc);
    str_init_1(&log_str, NULL, log_buf);
    if (! file_open_w(&log_str, &log_fd)) {
      str_clean(&log_str);
      kc3_clean(NULL);
      return 1;
    }
    buf_file_close(g_kc3_env->out);
    dup2(log_fd, 1);
    buf_fd_open_w(g_kc3_env->out, 1);
    buf_file_close(g_kc3_env->err);
    dup2(log_fd, 2);
    buf_fd_open_w(g_kc3_env->err, 2);
    buf_file_close(g_kc3_env->in);
    close(0);
  }
  ident_init(&daemonize_ident, &g_sym_KC3, sym_1("daemonize"));
  tag_init_bool(&daemonize_value, daemonize);
  env_def(g_kc3_env, &daemonize_ident, &daemonize_value);
  io_puts("KC3 HTTPd loading, please wait...");
#ifndef WIN32
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    err_puts("http_event_base_new: signal");
    assert(! "http_event_base_new: signal");
    kc3_clean(NULL);
    return 1;
  }
#endif
  module = sym_1("HTTPd");
  if (! module_load(module)) {
    kc3_clean(NULL);
    return 1;
  }
  call_init(&call);
  call.ident.module = module;
  call.ident.sym = sym_1("main");
  //FIXME
  if (argc >= 2)
    call.arguments = list_new_str_1
      (NULL, argv[0], list_new_str_1
       (NULL, argv[1], NULL));
  else if (argc == 1)
    call.arguments = list_new_str_1(NULL, argv[0], NULL);
  else
    call.arguments = NULL;
  if (! eval_call(&call, &tmp)) {
    err_puts("kc3_httpd: eval_call");
    goto clean;
  }
  switch (tmp.type) {
  case TAG_U8:
    r = tmp.data.u8;
    break;
  default:
    err_write_1("invalid return type from main: ");
    err_inspect_tag(&tmp);
    err_write_1("\n");
  }
 clean:
  tag_clean(&tmp);
  call_clean(&call);
  kc3_clean(NULL);
  return r;
}
