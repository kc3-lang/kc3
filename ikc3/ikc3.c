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
#include <signal.h>
#include <string.h>
#ifdef HAVE_PTHREAD
# include <pthread.h>
#endif
#include "../libkc3/kc3.h"
#include "../socket/socket.h"
#include "../socket/socket_buf.h"
#include "config.h"

#if HAVE_WINEDITLINE
# include "buf_wineditline.h"
#else
# include "buf_linenoise.h"
#endif

#define BUFSZ 0x10000

const char  *g_argv0_default = PROG;
const char  *g_argv0_dir_default = PREFIX;
bool         g_client = false;
s_str        g_host = {0};
s_str        g_port = {0};
bool         g_server = false;
s_buf       *g_server_env_err = NULL;
s_buf       *g_server_env_in  = NULL;
s_buf       *g_server_env_out = NULL;
t_socket     g_server_socket = -1;
s_socket_buf g_socket_buf = {0};
bool         g_tls = false;

sw  buf_ignore_character (s_buf *buf);

// TODO: use positive return value to increment argc and argv
// TODO: remove one reference level (*)
int    ikc3_arg_client (s_env *env, int *argc, char ***argv);
int    ikc3_arg_dump (s_env *env, int *argc, char ***argv);
int    ikc3_arg_load (s_env *env, int *argc, char ***argv);
int    ikc3_arg_server (s_env *env, int *argc, char ***argv);
int    ikc3_arg_tls (s_env *env, int *argc, char ***argv);
void   ikc3_buf_editline_close (s_buf *buf);
void   ikc3_buf_editline_open_r (s_buf *buf);
void   ikc3_client_clean (void);
int    ikc3_client_init (void);
sw     ikc3_run (void);
void   ikc3_server_clean (s_env *env);
int    ikc3_server_init (s_env *env);
int    usage (char *argv0);

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
  if (! env_module_load(env, &g_sym_RPC_Response)) {
    err_puts("ikc3_arg_client: failed to load RPC.Response module");
    assert(! "ikc3_arg_client: failed to load RPC.Response module");
    return -1;
  }
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
  if (! env_module_load(env, &g_sym_RPC)) {
    err_puts("ikc3_arg_client: failed to load RPC module");
    assert(! "ikc3_arg_client: failed to load RPC module");
    return -1;
  }
  if (! env_module_load(env, &g_sym_RPC_Response)) {
    err_puts("ikc3_arg_client: failed to load RPC.Response module");
    assert(! "ikc3_arg_client: failed to load RPC.Response module");
    return -1;
  }
  *argc -= 3;
  *argv += 3;
  return 3;
}

int ikc3_arg_tls (s_env *env, int *argc, char ***argv)
{
  (void) env;
  g_tls = true;
  *argc -= 1;
  *argv += 1;
  return 1;
}

void ikc3_buf_editline_close (s_buf *buf)
{
#if HAVE_WINEDITLINE
  buf_wineditline_close(buf, ".ikc3_history");
#else
  buf_linenoise_close(buf, ".ikc3_history");
#endif
}

void ikc3_buf_editline_open_r (s_buf *buf)
{
#if HAVE_WINEDITLINE
  buf_wineditline_open_r(buf, "ikc3> ", ".ikc3_history");
#else
  buf_linenoise_open_r(buf, "ikc3> ", ".ikc3_history");
#endif
}

void ikc3_client_clean (void)
{
  socket_buf_close(&g_socket_buf);
}

int ikc3_client_init (void)
{
  if (! socket_buf_init_connect(&g_socket_buf, &g_host, &g_port)) {
    err_write_1("ikc3: unable to connect to ");
    err_inspect_str(&g_host);
    err_write_1(" ");
    err_inspect_str(&g_port);
    err_write_1("\n");
    return 1;
  }
  if (true) {
    io_write_1("ikc3: connected to ");
    io_inspect_str(&g_host);
    io_write_1(" ");
    io_inspect_str(&g_port);
    io_write_1("\n");
  }
  return 0;
}

int ikc3_client_init_tls (void)
{
  // TODO: TLS init
  // TODO: replace ikc3_client_init with TLS.Client init code in tests
  //       for TLS connection option (--tls)
  return -1;
}

sw ikc3_run (void)
{
  s_env *env;
  s_buf *env_err;
  s_buf err_buf;
  s_tag input;
  sw r;
  s_tag *response;
  s_tag result = {0};
  s_tag rpc_tag = {0};
  env = env_global();
  assert(env);
  env_err = env->err;
  if (g_server) {
    if (! buf_init_alloc(&err_buf, BUF_SIZE))
      return 1;
    env->err = &err_buf;
  }
  while (1) {
    if ((r = buf_ignore_spaces(env->in)) < 0 ||
        (r = buf_parse_comments(env->in)) < 0) {
      r = 0;
      goto clean;
    }
    if ((r = buf_parse_tag(env->in, &input)) > 0) {
      tag_init(&result);
      if (g_client) {
        s_str input_str;
        s_list *args;
        tag_init_pcall(&rpc_tag);
        rpc_tag.data.pcall->ident.module = &g_sym_RPC;
        rpc_tag.data.pcall->ident.sym = &g_sym_request;
        if (! inspect_tag(&input, &input_str)) {
          tag_clean(&input);
          r = 1;
          goto clean;
        }
        args = list_new(NULL);
        if (! args) {
          tag_clean(&input);
          str_clean(&input_str);
          r = 1;
          goto clean;
        }
        args->tag.type = TAG_STR;
        args->tag.data.str = input_str;
        rpc_tag.data.pcall->arguments = args;
        if (buf_inspect_tag(g_socket_buf.buf_rw.w, &rpc_tag) <= 0 ||
            buf_write_1(g_socket_buf.buf_rw.w, "\n") <= 0 ||
            buf_flush(g_socket_buf.buf_rw.w) < 0) {
          tag_clean(&input);
          tag_clean(&rpc_tag);
          r = 1;
          goto clean;
        }
        tag_clean(&rpc_tag);
        if (buf_parse_tag(g_socket_buf.buf_rw.r, &result) <= 0) {
          tag_clean(&input);
          r = 1;
          goto clean;
        }
        if (buf_read_1(g_socket_buf.buf_rw.r, "\n") <= 0) {
          tag_clean(&result);
          tag_clean(&input);
          r = 1;
          goto clean;
        }
        if (result.type != TAG_PSTRUCT ||
            ! result.data.pstruct ||
            result.data.pstruct->pstruct_type->module !=
            &g_sym_RPC_Response ||
            ! (response = result.data.pstruct->tag) ||
            response[0].type != TAG_STR ||
            response[1].type != TAG_STR) {
          err_puts("ikc3_run: invalid RPC response");
          assert(! "ikc3_run: invalid RPC response");
          tag_clean(&result);
          tag_clean(&input);
          r = 1;
          goto clean;
        }
        buf_write_str(env->err, &response[0].data.str);
        buf_write_str(env->out, &response[1].data.str);
        buf_inspect_tag(env->out, &response[2]);
      }
      else if (! eval_tag(&input, &result)) {
        tag_clean(&input);
        if (g_server) {
          s_rpc_response response = {0};
          if (buf_read_to_str(&err_buf, &response.err) < 0) {
            r = 1;
            goto clean;
          }
          str_init_empty(&response.out);
          tag_init_void(&response.result);
          if (! tag_init_pstruct_copy_data(&result, &g_sym_RPC_Response,
                                           &response)) {
            str_clean(&response.err);
            r = 1;
            goto clean;
          }
          if (buf_inspect_tag(env->out, &result) < 0) {
            tag_clean(&result);
            r = 1;
            goto clean;
          }
          tag_clean(&result);
        }
        // XXX not secure (--pedantic)
        goto next;
      }
      tag_clean(&input);
      if (! g_client) {
        if (buf_inspect_tag(env->out, &result) < 0) {
          tag_clean(&result);
          r = 0;
          goto clean;
        }
      }
      tag_clean(&result);
    }
    if (r < 0 ||
        (r == 0 &&
         (r = buf_ignore_character(env->in)) <= 0)) {
      r = 0;
      goto clean;
    }
  next:
    if ((r = buf_write_1(env->out, "\n")) < 0) {
      r = 1;
      goto clean;
    }
    if ((r = buf_flush(env->out)) < 0) {
      r = 1;
      goto clean;
    }
  }
 clean:
  if (g_server) {
    env->err = env_err;
    buf_clean(&err_buf);
  }
  return r;
}

void ikc3_server_clean (s_env *env)
{
  env->in  = g_server_env_in;
  env->out = g_server_env_out;
  env->err = g_server_env_err;
  socket_buf_close(&g_socket_buf);
  socket_close(&g_server_socket);
}

int ikc3_server_init (s_env *env)
{
  assert(env);
  if (! socket_init_listen(&g_server_socket, &g_host, &g_port)) {
    err_write_1("ikc3: failed to listen on ");
    err_inspect_str(&g_host);
    err_write_1(" ");
    err_inspect_str(&g_port);
    err_write_1("\n");
    return 1;
  }
  if (true) {
    io_write_1("ikc3: listening on ");
    io_inspect_str(&g_host);
    io_write_1(" ");
    io_inspect_str(&g_port);
    io_write_1("\n");
  }
  if (! socket_buf_init_accept(&g_socket_buf, &g_server_socket)) {
    err_puts("ikc3: ikc3_server_init: socket_buf_init_accept");
    socket_close(&g_server_socket);
    return 1;
  }
  if (true) {
    io_write_1("ikc3: connected to ");
    io_inspect_str(&g_socket_buf.addr_str);
    io_write_1("\n");
  }
  g_server_env_in = env->in;
  g_server_env_out = env->out;
  g_server_env_err = env->err;
  env->in = g_socket_buf.buf_rw.r;
  env->out = g_socket_buf.buf_rw.w;
  //env->err = g_socket_buf.buf_rw.w;
  return 0;
}

int ikc3_server_init_tls (s_env *env)
{
  assert(env);
  // TODO: TLS init
  if (! socket_init_listen(&g_server_socket, &g_host, &g_port)) {
    err_write_1("ikc3: failed to listen on ");
    err_inspect_str(&g_host);
    err_write_1(" ");
    err_inspect_str(&g_port);
    err_write_1("\n");
    return 1;
  }
  if (true) {
    io_write_1("ikc3: listening on ");
    io_inspect_str(&g_host);
    io_write_1(" ");
    io_inspect_str(&g_port);
    io_write_1("\n");
  }
  if (! socket_buf_init_accept(&g_socket_buf, &g_server_socket)) {
    err_puts("ikc3: ikc3_server_init: socket_buf_init_accept");
    socket_close(&g_server_socket);
    return 1;
  }
  // TODO: TLS server up to tls_handshake()
  if (true) {
    io_write_1("ikc3: connected to ");
    io_inspect_str(&g_socket_buf.addr_str);
    io_write_1("\n");
  }
  g_server_env_in = env->in;
  g_server_env_out = env->out;
  g_server_env_err = env->err;
  env->in = g_socket_buf.buf_rw.r;
  env->out = g_socket_buf.buf_rw.w;
  //env->err = g_socket_buf.buf_rw.w;
  return 0;
}

int main (int argc, char **argv)
{
  s_env *env = NULL;
  s_buf  in_original = {0};
  sw r = 0;
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
    else if (! strcmp("--tls", argv[0])) {
      if ((r = ikc3_arg_tls(env, &argc, &argv)) < 0)
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
  if (g_client && g_server) {
    err_puts("ikc3: --client and --server is not supported");
    return usage(env->argv[0]);
  }
  if (g_tls) {
    if (! g_client && ! g_server) {
      err_puts("ikc3: --tls without --client or --server");
      return usage(env->argv[0]);
    }
  }
  *env->in = in_original;
  if (g_server) {
    if (ikc3_server_init(env))
      goto clean;
    r = ikc3_run();
    goto clean;
  }
  ikc3_buf_editline_open_r(env->in);
  if (g_client) {
    if (ikc3_client_init())
      goto close;
  }
  r = ikc3_run();
  if (g_client)
    ikc3_client_clean();
  else if (g_server)
    ikc3_server_clean(env);
 close:
  ikc3_buf_editline_close(env->in);
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
