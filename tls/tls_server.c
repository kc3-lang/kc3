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
#include <tls.h>
#include "../libkc3/kc3.h"
#include "../socket/socket.h"
#include "tls_buf.h"
#include "tls_server.h"

void kc3_tls_server_clean (s_tls_server *tls_server)
{
  tls_buf_close(tls_server->buf_rw.r);
  tls_buf_close(tls_server->buf_rw.w);
  buf_rw_clean(&tls_server->buf_rw);
  close(tls_server->socket);
}

s_tls_server * kc3_tls_server_init_accept (s_tls_server *tls_server,
                                           p_socket socket,
                                           p_tls *ctx)
{
  s_tls_server tmp = {0};
  assert(tls_server);
  assert(ctx);
  assert(*ctx);
  if (! socket_init_accept(&tmp.socket, socket)) {
    err_puts("kc3_tls_server_init_accept: socket_init_accept");
    assert(! "kc3_tls_server_init_accept: socket_init_accept");
    return NULL;
  }
  if (tls_accept_socket(*ctx, &tmp.ctx, *socket)) {
    err_write_1("kc3_tls_server_init_accept: tls_accept_socket: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_server_init_accept: tls_accept_socket");
    return NULL;
  }
  if (! (tmp.buf_rw.r = buf_new_alloc(BUF_SIZE)))
    return NULL;
  if (! (tmp.buf_rw.w = buf_new_alloc(BUF_SIZE))) {
    buf_delete(tmp.buf_rw.r);
    return NULL;
  }
  if (! tls_buf_open_r(tmp.buf_rw.r, *ctx)) {
    buf_delete(tmp.buf_rw.w);
    buf_delete(tmp.buf_rw.r);
    return NULL;
  }
  if (! tls_buf_open_w(tmp.buf_rw.w, *ctx)) {
    tls_buf_close(tmp.buf_rw.r);
    buf_delete(tmp.buf_rw.w);
    buf_delete(tmp.buf_rw.r);
    return NULL;
  }
  *tls_server = tmp;
  return tls_server;
}
