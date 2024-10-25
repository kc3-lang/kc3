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
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include "assert.h"
#include "crypt.h"
#include "str.h"

bool * crypt_check_password (const s_str *pass, const s_str *hash,
                             bool *dest)
{
  sw e;
  assert(pass);
  assert(hash);
  assert(dest);
  if (crypt_checkpass(pass->ptr.pchar, hash->ptr.pchar)) {
    e = errno;
    err_write_1("crypt_check_password: ");
    err_write_1(strerror(e));
    err_write_1("\n");
    *dest = false;
  }
  else
    *dest = true;
  return dest;
}

s_str * crypt_hash_password (const s_str *pass, s_str *dest)
{
  sw e;
  char hash[_PASSWORD_LEN] = {0};
  assert(pass);
  assert(dest);
  if (crypt_newhash(pass->ptr.pchar, "bcrypt,a", hash,
                    sizeof(hash))) {
    e = errno;
    err_write_1("crypt_hash_password: ");
    err_write_1(strerror(e));
    err_write_1("\n");
    return NULL;
  }
  return str_init_copy_1(dest, hash);
}
