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
#include <unistd.h>
#include <tls.h>
#include "kc3.h"
#include "socket.h"
#include "socket_buf.h"
#include "tls_buf.h"
#include "tls_server.h"

void kc3_tls_server_clean (s_tls_server *tls_server)
{
  s_tls_buf *tls_buf;
  buf_flush(tls_server->socket_buf->buf_rw.w);
  tls_buf = tls_server->socket_buf->buf_rw.w->user_ptr;
  tls_close(tls_buf->ctx);
  tls_buf_close(tls_server->socket_buf->buf_rw.r);
  tls_buf_close(tls_server->socket_buf->buf_rw.w);
  buf_rw_clean(&tls_server->socket_buf->buf_rw);
  close(tls_server->socket_buf->sockfd);
  free(tls_server->socket_buf);
}

s_tls_server * kc3_tls_server_init_accept (s_tls_server *tls_server,
                                           p_socket socket,
                                           p_tls *ctx)
{
  sw r;
  p_tls tmp_ctx = NULL;
  assert(tls_server);
  assert(ctx);
  assert(*ctx);
  *tls_server = (s_tls_server) {0};
  tls_server->socket_buf = alloc(sizeof(s_socket_buf));
  if (! tls_server->socket_buf)
    return NULL;
  if (! socket_buf_init_accept(tls_server->socket_buf, socket)) {
    free(tls_server->socket_buf);
    return NULL;
  }
  if (false)
    err_puts("kc3_tls_server_init_accept: accept: OK");
  if (tls_accept_socket(*ctx, &tmp_ctx, tls_server->socket_buf->sockfd)) {
    err_write_1("kc3_tls_server_init_accept: tls_accept_socket: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_server_init_accept: tls_accept_socket");
    return NULL;
  }
  if (false)
    err_puts("kc3_tls_server_init_accept: tls_accept_socket: OK");
  if (! (tls_server->socket_buf->buf_rw.r = buf_new_alloc(BUF_SIZE))) {
    err_puts("kc3_tls_server_init_accept: buf_new_alloc r");
    assert(! "kc3_tls_server_init_accept: buf_new_alloc r");
    return NULL;
  }
  if (! (tls_server->socket_buf->buf_rw.w = buf_new_alloc(BUF_SIZE))) {
    err_puts("kc3_tls_server_init_accept: buf_new_alloc w");
    assert(! "kc3_tls_server_init_accept: buf_new_alloc w");
    buf_delete(tls_server->socket_buf->buf_rw.r);
    return NULL;
  }
  if (! tls_buf_open_r(tls_server->socket_buf->buf_rw.r, tmp_ctx)) {
    err_puts("kc3_tls_server_init_accept: tls_buf_open_r");
    assert(! "kc3_tls_server_init_accept: tls_buf_open_r");
    buf_delete(tls_server->socket_buf->buf_rw.w);
    buf_delete(tls_server->socket_buf->buf_rw.r);
    return NULL;
  }
  if (! tls_buf_open_w(tls_server->socket_buf->buf_rw.w, tmp_ctx)) {
    err_puts("kc3_tls_server_init_accept: tls_buf_open_w");
    assert(! "kc3_tls_server_init_accept: tls_buf_open_w");
    tls_buf_close(tls_server->socket_buf->buf_rw.r);
    buf_delete(tls_server->socket_buf->buf_rw.w);
    buf_delete(tls_server->socket_buf->buf_rw.r);
    return NULL;
  }
  while ((r = tls_handshake(tmp_ctx)) == TLS_WANT_POLLIN ||
         r == TLS_WANT_POLLOUT)
    ;
  if (r) {
    err_write_1("kc3_tls_server_init_accept: tls_handshake: ");
    err_puts(tls_error(tmp_ctx));
    assert(! "kc3_tls_server_init_accept: tls_handshake");
    tls_buf_close(tls_server->socket_buf->buf_rw.w);
    tls_buf_close(tls_server->socket_buf->buf_rw.r);
    buf_delete(tls_server->socket_buf->buf_rw.w);
    buf_delete(tls_server->socket_buf->buf_rw.r);
    return NULL;
  }
  return tls_server;
}
