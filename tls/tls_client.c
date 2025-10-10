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
#include <unistd.h>
#include <tls.h>
#include "../libkc3/kc3.h"
#include "../socket/socket.h"
#include "tls_buf.h"
#include "tls_client.h"

void kc3_tls_client_clean (s_tls_client *tls_client)
{
  s_tls_buf *tls_buf;
  buf_flush(tls_client->socket_buf.buf_rw.w);
  tls_buf = tls_client->socket_buf.buf_rw.r->user_ptr;
  tls_close(tls_buf->ctx);
  tls_buf_close(tls_client->socket_buf.buf_rw.r);
  tls_buf_close(tls_client->socket_buf.buf_rw.w);
  buf_rw_clean(&tls_client->socket_buf.buf_rw);

  // TODO: check if tests leak without that
  close(tls_client->socket_buf.sockfd);
}

s_tls_client * kc3_tls_client_init_connect (s_tls_client *tls_client,
                                            p_tls *ctx, s_str *host,
                                            s_str *port)
{
  sw r;
  s_tls_client tmp = {0};
  assert(tls_client);
  assert(ctx);
  assert(*ctx);
  assert(host);
  assert(port);
  if (! socket_init_connect(&tmp.socket_buf.sockfd, host, port)) {
    err_puts("kc3_tls_client_init_connect: socket_init_connect");
    assert(! "kc3_tls_client_init_connect: socket_init_connect");
    return NULL;
  }
  if (tls_connect_socket(*ctx, tmp.socket_buf.sockfd, host->ptr.pchar)) {
    err_write_1("kc3_tls_client_init_connect: tls_connect_socket: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_client_init_connect: tls_connect_socket");
    return NULL;
  }
  if (! (tmp.socket_buf.buf_rw.r = buf_new_alloc(BUF_SIZE))) {
    err_puts("kc3_tls_client_init_connect: buf_new_alloc r");
    assert(! "kc3_tls_client_init_connect: buf_new_alloc r");
    return NULL;
  }
  if (! (tmp.socket_buf.buf_rw.w = buf_new_alloc(BUF_SIZE))) {
    err_puts("kc3_tls_client_init_connect: buf_new_alloc w");
    assert(! "kc3_tls_client_init_connect: buf_new_alloc w");
    buf_delete(tmp.socket_buf.buf_rw.r);
    return NULL;
  }
  while ((r = tls_handshake(*ctx)) == TLS_WANT_POLLIN ||
         r == TLS_WANT_POLLOUT)
    ;
  if (r) {
    err_write_1("kc3_tls_client_init_connect: tls_handshake: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_client_init_connect: tls_handshake");
    tls_buf_close(tmp.socket_buf.buf_rw.w);
    tls_buf_close(tmp.socket_buf.buf_rw.r);
    buf_delete(tmp.socket_buf.buf_rw.w);
    buf_delete(tmp.socket_buf.buf_rw.r);
    return NULL;
  }
  if (! tls_buf_open_r(tmp.socket_buf.buf_rw.r, *ctx)) {
    err_puts("kc3_tls_client_init_connect: tls_buf_open_r");
    assert(! "kc3_tls_client_init_connect: tls_buf_open_r");
    buf_delete(tmp.socket_buf.buf_rw.w);
    buf_delete(tmp.socket_buf.buf_rw.r);
    return NULL;
  }
  if (! tls_buf_open_w(tmp.socket_buf.buf_rw.w, *ctx)) {
    err_puts("kc3_tls_client_init_connect: ");
    assert(! "kc3_tls_client_init_connect: ");
    tls_buf_close(tmp.socket_buf.buf_rw.r);
    buf_delete(tmp.socket_buf.buf_rw.w);
    buf_delete(tmp.socket_buf.buf_rw.r);
    return NULL;
  }
  *tls_client = tmp;
  return tls_client;
}
