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
#if defined(WIN32) || defined(WIN64)
# include <windows.h>
# include <wincrypt.h>
#else
# include <unistd.h>
# include <stdlib.h>
#endif
#include <tls.h>
#include <libkc3/kc3.h>
#include "tls.h"

s_str * kc3_tls_get_ca_cert_path (s_str *dest)
{
  uw i;
  static const s_str paths[] = {
    STR("/etc/certs/ca-certificates.crt"),
    STR("/etc/pki/tls/certs/ca-bundle.crt"),
    STR("/etc/ssl/cert.pem"),
    STR("/etc/ssl/certs/ca-certificates.crt"),
    STR("/usr/local/share/certs/ca-root-nss.crt"),
    STR("/usr/ssl/certs/ca-bundle.crt"),
  };
  const char *ssl_cert_file = getenv("SSL_CERT_FILE");
  if (ssl_cert_file && access(ssl_cert_file, R_OK) == 0)
    return str_init_1_alloc(dest, ssl_cert_file);
  i = 0;
  while (i < sizeof(paths) / sizeof(*paths)) {
    if (access(paths[i].ptr.pchar, R_OK) == 0) {
      *dest = paths[i];
      return dest;
    }
    i++;
  }
#if defined(WIN32) || defined(WIN64)
  err_puts("kc3_tls_get_ca_cert_path: not implemented");
  assert(! "kc3_tls_get_ca_cert_path: not implemented");
#else
  err_puts("kc3_tls_get_ca_cert_path: not found");
  assert(! "kc3_tls_get_ca_cert_path: not found");
#endif
  return NULL;
}

s_pointer * kc3_tls_config_new (s_pointer *dest)
{
  s_pointer tmp = {0};
  tmp.pointer_type = sym_1("KC3.Config*");
  tmp.target_type = sym_1("KC3.Config");
  if (! (tmp.ptr.p = tls_config_new()))
    return NULL;
  *dest = tmp;
  return dest;
}
