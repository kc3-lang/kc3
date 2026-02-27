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
#if defined(WIN32) || defined(WIN64)
# include <winsock2.h>
# include <windows.h>
# include <wincrypt.h>
#else
# include <unistd.h>
# include <stdlib.h>
#endif
#include <errno.h>
#include <string.h>
#include <tls.h>
#include "kc3.h"
#include "tls.h"

p_tls * kc3_tls_accept_socket (p_tls *ctx, p_tls *dest_client_ctx,
                               p_socket client_fd)
{
  p_tls tmp = NULL;
  assert(ctx);
  assert(*ctx);
  assert(dest_client_ctx);
  assert(client_fd);
  if (tls_accept_socket(*ctx, &tmp, *client_fd)) {
    err_write_1("kc3_tls_accept_socket: tls_accept_socket: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_accept_socket: tls_accept_socket");
    return NULL;
  }
  *dest_client_ctx = tmp;
  return dest_client_ctx;
}

p_tls * kc3_tls_configure (p_tls *ctx, p_tls_config *cfg, p_tls *dest)
{
  assert(ctx);
  assert(*ctx);
  assert(cfg);
  assert(*cfg);
  if (tls_configure(*ctx, *cfg)) {
    err_write_1("kc3_tls_configure: tls_configure: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_configure: tls_configure");
    return NULL;
  }
  if (! dest)
    return ctx;
  *dest = *ctx;
  return dest;
}

s_str * kc3_tls_ca_cert_path (s_str *dest)
{
  const char *default_path;
  uw i;
  const s_str paths[] = {
    STR("/etc/certs/ca-certificates.crt"),
    STR("/etc/pki/tls/certs/ca-bundle.crt"),
    STR("/etc/ssl/cert.pem"),
    STR("/etc/ssl/certs/ca-certificates.crt"),
    STR("/usr/share/certs/ca-root-nss.crt"),
    STR("/usr/local/share/certs/ca-root-nss.crt"),
    STR("/usr/ssl/certs/ca-bundle.crt"),
  };
  const char *ssl_cert_file;
  assert(dest);
  ssl_cert_file = getenv("SSL_CERT_FILE");
  if (ssl_cert_file && access(ssl_cert_file, R_OK) == 0)
    return str_init_1_alloc(dest, ssl_cert_file);
  default_path = tls_default_ca_cert_file();
  if (default_path) {
    if (access(default_path, R_OK)) {
      s_str default_path_str = {0};
      sw e = errno;
      str_init_1(&default_path_str, NULL, default_path);
      err_write_1("kc3_tls_ca_cert_path: ");
      err_inspect_str(&default_path_str);
      err_write_1(": access: ");
      err_puts(strerror(e));
      goto search;
    }
    return str_init_1_alloc(dest, default_path);
  }
 search:
  i = 0;
  while (i < sizeof(paths) / sizeof(*paths)) {
    if (access(paths[i].ptr.p_pchar, R_OK) == 0) {
      *dest = paths[i];
      return dest;
    }
    i++;
  }
#if defined(WIN32) || defined(WIN64)
  err_puts("kc3_tls_ca_cert_path: not implemented");
  assert(! "kc3_tls_ca_cert_path: not implemented");
#else
  err_puts("kc3_tls_ca_cert_path: not found");
  assert(! "kc3_tls_ca_cert_path: not found");
#endif
  return NULL;
}

p_tls * kc3_tls_client (p_tls *dest)
{
  p_tls tmp = NULL;
  assert(dest);
  if (! (tmp = tls_client())) {
    err_puts("kc3_tls_client: tls_client");
    assert(! "kc3_tls_client: tls_client");
    return NULL;
  }
  *dest = tmp;
  return dest;
}

p_tls * kc3_tls_close (p_tls *ctx, p_tls *dest)
{
  assert(ctx);
  assert(*ctx);
  assert(dest);
  if (tls_close(*ctx)) {
    err_write_1("kc3_tls_close: tls_close: ");
    err_puts(tls_error(*ctx));
    return NULL;
  }
  *dest = *ctx;
  return dest;
}

p_tls * kc3_tls_connect_socket (p_tls *ctx, p_socket sockfd,
                                const s_str *hostname,
                                p_tls *dest)
{
  assert(ctx);
  assert(sockfd);
  assert(hostname);
  assert(dest);
  if (tls_connect_socket(*ctx, *sockfd, hostname->ptr.p_pchar)) {
    err_write_1("kc3_tls_connect_socket: tls_connect_socket: ");
    err_puts(tls_error(*ctx));
    assert(! "kc3_tls_connect_socket: tls_connect_socket");
    return NULL;
  }
  *dest = *ctx;
  return dest;
}

void kc3_tls_free (p_tls *ctx)
{
  assert(ctx);
  assert(*ctx);
  tls_free(*ctx);
}

s_tag * kc3_tls_init (s_tag *dest)
{
  if (tls_init()) {
    err_puts("kc3_tls_init: tls_init");
    assert(! "kc3_tls_init: tls_init");
    return NULL;
  }
  return tag_init_psym(dest, sym_1("TLS"));
}

p_tls * kc3_tls_server (p_tls *dest)
{
  p_tls tmp;
  if (! (tmp = tls_server())) {
    err_write_1("kc3_tls_server: tls_server: ");
    err_puts(tls_error(tmp));
    assert(! "kc3_tls_server: tls_server");
    return NULL;
  }
  *dest = tmp;
  return dest;
}
