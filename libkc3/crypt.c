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
#include <sys/types.h>
#include <errno.h>
#if ! (defined(WIN32) || defined(WIN64))
#include <pwd.h>
#endif
#include <string.h>
#include <unistd.h>
#include "assert.h"
#include "crypt.h"
#include "str.h"
#include "tag.h"

#if HAVE_CRYPT_NEWHASH && 0

bool * crypt_check_password (const s_str *pass, const s_str *hash,
                             bool *dest)
{
  sw e;
  assert(pass);
  assert(hash);
  assert(dest);
  if (crypt_checkpass(pass->ptr.pchar, hash->ptr.pchar)) {
    if (errno != EACCES) {
      e = errno;
      err_write_1("crypt_check_password: ");
      err_write_1(strerror(e));
      err_write_1("\n");
    }
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

#else

/* SHA512 password hash */

bool * crypt_check_password (const s_str *pass, const s_str *hash,
                             bool *dest)
{
  s_str str;
  assert(pass);
  assert(hash);
  assert(dest);
  if (! crypt_sha512(pass, hash, &str)) {
    err_puts("crypt_check_password: crypt_sha512");
    assert(! "crypt_check_password: crypt_sha512");
    *dest = false;
  }
  else
    *dest = true;
  return dest;
}

s_str * crypt_hash_password (const s_str *pass, s_str *dest)
{
  s_str config;
  s_str hash;
  s_str prefix = {{NULL}, 17, {"$6$rounds=123456$"}};
  s_str salt;
  s_tag tag;
  tag_init_uw(&tag, 16);
  if (! str_init_random_base64(&salt, &tag))
    return NULL;
  if (! str_init_concatenate(&config, &prefix, &salt)) {
    str_clean(&salt);
    return NULL;
  }
  if (! crypt_sha512(pass, &config, &hash)) {
    str_clean(&config);
    str_clean(&salt);
    return NULL;
  }
  str_clean(&config);
  str_clean(&salt);
  *dest = hash;
  return dest;
}

#endif
