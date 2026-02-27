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
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../libkc3/kc3.h"

const char *g_env_argv0_default = PROG;
const char *g_env_argv0_dir_default = PREFIX;

static s_counter *g_httpd_server_thread_stop = NULL;

static void httpd_signal (int s);

static void httpd_signal (int s)
{
  if (g_httpd_server_thread_stop) {
    g_httpd_server_thread_stop->count.type = TAG_U8;
    g_httpd_server_thread_stop->count.data.td_u8 = 1;
  }
  else
    exit(s);
}

int main (int argc, char **argv)
{
  s_call call = {0};
  bool    daemonize = true;
  s_ident daemonize_ident;
  s_tag   daemonize_value;
  const s_str dump_path = STR("kc3.dump");
  s32 e;
  s_env *env;
  char  err_buf[64] = {0};
  s64   err_fd = -1;
  s_str err_str = {0};
  s_str init = STR("./config/init.kc3");
  char  log_buf[64] = {0};
  s64   log_fd = -1;
  s_str log_str = {0};
  const s_sym *module = NULL;
  char *p;
  s32 r = 1;
  s_ident server_thread_stop = {0};
  s32 skip = 0;
  time_t t;
  s_tag tmp = {0};
  const struct tm *utc = NULL;
  kc3_init(NULL, &argc, &argv);
  env = env_global();
  while (argc > 0 && argv[0] && argv[0][0] == '-') {
    skip = 1;
    p = argv[0] + 1;
    while (*p) {
      switch (*p) {
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
    strftime(log_buf, sizeof(log_buf) - 1,
             "log/kc3_httpd_%Y-%m-%d_%H:%M:%S.log",
             utc);
    str_init_1(&log_str, NULL, log_buf);
    if (! file_open_w(&log_str, &log_fd)) {
      str_clean(&log_str);
      kc3_clean(NULL);
      return 1;
    }
    buf_file_close(env->out);
    dup2(log_fd, 1);
    buf_fd_open_w(env->out, 1);
    strftime(err_buf, sizeof(err_buf) - 1,
             "log/kc3_httpd_%Y-%m-%d_%H:%M:%S.error.log",
             utc);
    str_init_1(&err_str, NULL, err_buf);
    if (! file_open_w(&err_str, &err_fd)) {
      str_clean(&err_str);
      kc3_clean(NULL);
      return 1;
    }
    buf_file_close(env->err);
    dup2(err_fd, 2);
    buf_fd_open_w(env->err, 2);
    buf_file_close(env->in);
    close(0);
  }
  ident_init(&daemonize_ident, &g_sym_KC3, sym_1("daemonize"));
  tag_init_bool(&daemonize_value, daemonize);
  env_def(env, &daemonize_ident, &daemonize_value);
  io_puts("KC3 HTTPd loading, please wait...");
#if ! (defined(WIN32) || defined(WIN64))
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    err_puts("http_event_base_new: signal: SIGPIPE");
    assert(! "http_event_base_new: signal: SIGPIPE");
    kc3_clean(NULL);
    return 1;
  }
  if (signal(SIGINT, httpd_signal) == SIG_ERR) {
    err_puts("http_event_base_new: signal: SIGINT");
    assert(! "http_event_base_new: signal: SIGINT");
    kc3_clean(NULL);
    return 1;
  }
  if (signal(SIGTERM, httpd_signal) == SIG_ERR) {
    err_puts("http_event_base_new: signal: SIGTERM");
    assert(! "http_event_base_new: signal: SIGTERM");
    kc3_clean(NULL);
    return 1;
  }
#endif
  module = sym_1("HTTPd");
  // TODO: env_module_is_loaded
  if (! file_access(&dump_path, &g_sym_r)) {
    if (! module_load(module)) {
      kc3_clean(NULL);
      return 1;
    }
    call_init(&call);
    call.ident.module = module;
    call.ident.sym = sym_1("load_app");
    if (! eval_call(&call, &tmp)) {
      err_puts("kc3_httpd: eval_call");
      assert(! "kc3_httpd: eval_call");
      call_clean(&call);
      kc3_clean(NULL);
      return 1;
    }
    call_clean(&call);
    tag_clean(&tmp);
    if (! env_dump(env, &dump_path)) {
      err_puts("kc3_httpd: env_dump");
      assert("kc3_httpd: env_dump");
      kc3_clean(NULL);
      return 1;
    }
    kc3_clean(NULL);
    return 0;
  }
  if (file_access(&init, &g_sym_r)) {
    if (! env_load(env, &init)) {
      kc3_clean(NULL);
      return 1;
    }
  }
  securelevel(2);
  server_thread_stop.module = sym_1("HTTPd");
  server_thread_stop.sym = sym_1("server_thread_stop");
  if (! ident_get(&server_thread_stop, &tmp) ||
      tmp.type != TAG_POINTER ||
      tmp.data.td_pointer.target_type != &g_sym_Counter ||
      ! (g_httpd_server_thread_stop =
         tmp.data.td_pointer.ptr.p_pvoid)) {
    err_puts("httpd_sigint: ident_get: HTTPd.server_thread_stop");
    assert(! "httpd_sigint: ident_get: HTTPd.server_thread_stop");
    abort();
  }
  call_init(&call);
  call.ident.module = module;
  call.ident.sym = sym_1("main");
  // TODO: better argument parsing
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
    r = tmp.data.td_u8;
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
