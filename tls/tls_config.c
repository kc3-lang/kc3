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
#include "tls.h"

s_str * kc3_tls_config_error (p_tls_config *config, s_str *dest)
{
  assert(config);
  assert(*config);
  assert(dest);
  return str_init_1_alloc(dest, tls_config_error(*config));
}

void kc3_tls_config_free (p_tls_config *config)
{
  assert(config);
  assert(*config);
  tls_config_free(*config);
}

p_tls_config * kc3_tls_config_new (p_tls_config *dest)
{
  p_tls_config tmp = {0};
  if (! (tmp = tls_config_new())) {
    ERROR("tls_config_new");
    return NULL;
  }
  *dest = tmp;
  return dest;
}

p_tls_config * kc3_tls_config_set_ca_file (p_tls_config *config,
                                           s_str *path,
                                           p_tls_config *dest)
{
  assert(config);
  assert(*config);
  assert(path);
  assert(path->size);
  assert(path->ptr.pchar);
  if (tls_config_set_ca_file(*config, path->ptr.pchar)) {
    err_write_1("kc3_tls_config_set_ca_file: tls_config_set_ca_file: ");
    err_write_1(tls_config_error(*config));
    err_write_1("\n");
    assert(! "kc3_tls_config_set_ca_file: tls_config_set_ca_file");
    return NULL;
  }
  if (false) {
    err_write_1("kc3_tls_config_set_ca_file: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  if (! dest)
    return config;
  *dest = *config;
  return dest;
}

p_tls_config * kc3_tls_config_set_cert_file (p_tls_config *config,
                                             const s_str *file_path,
                                             p_tls_config *dest)
{
  assert(config);
  assert(*config);
  assert(file_path);
  if (tls_config_set_cert_file(*config, file_path->ptr.pchar)) {
    err_write_1("kc3_tls_config_set_cert_file: "
                "tls_config_set_cert_file: ");
    err_puts(tls_config_error(*config));
    assert(! "kc3_tls_config_set_cert_file: tls_config_set_cert_file");
    return NULL;
  }
  if (false) {
    err_write_1("kc3_tls_config_set_cert_file: ");
    err_inspect_str(file_path);
    err_write_1("\n");
  }
  if (! dest)
    return config;
  *dest = *config;
  return dest;
}

p_tls_config * kc3_tls_config_set_key_file (p_tls_config *config,
                                            const s_str *key_file_path,
                                            p_tls_config *dest)
{
  assert(config);
  assert(*config);
  assert(key_file_path);
  if (tls_config_set_key_file(*config, key_file_path->ptr.pchar)) {
    err_write_1("kc3_tls_config_set_key_file: "
                "tls_config_set_key_file: ");
    err_puts(tls_config_error(*config));
    assert(! "kc3_tls_config_set_key_file: tls_config_set_key_file: ");
    return NULL;
  }
  if (false) {
    err_write_1("kc3_tls_config_set_key_file: ");
    err_inspect_str(key_file_path);
    err_write_1("\n");
  }
  if (! dest)
    return config;
  *dest = *config;
  return dest;
}
