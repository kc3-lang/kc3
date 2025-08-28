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
#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <unistd.h>
#include <stdlib.h>
#endif
#include <tls.h>
#include <libkc3/kc3.h>
#include "tls.h"

const char * kc3_tls_get_ca_cert_path (void)
{
  static const char* ca_cert_paths[] = {
    "/etc/ssl/certs/ca-certificates.crt",
    "/etc/pki/tls/certs/ca-bundle.crt",
    "/etc/ssl/cert.pem",
    "/usr/local/share/certs/ca-root-nss.crt",
    "/etc/certs/ca-certificates.crt",
    NULL
  };
  const char *cert_file = getenv("SSL_CERT_FILE");
  if (cert_file && access(cert_file, R_OK) == 0) {
    return cert_file;
  }
  for (int i = 0; ca_cert_paths[i]; i++) {
    if (access(ca_cert_paths[i], R_OK) == 0) {
      return ca_cert_paths[i];
    }
  }
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
