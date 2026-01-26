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
/**
 * @file crypt.h
 * @brief Password hashing
 *
 * Structure to manipulate password hashes.
 */
#ifndef LIBKC3_CRYPT_H
#define LIBKC3_CRYPT_H

#include "types.h"

bool *  crypt_check_password (const s_str *pass, const s_str *hash,
                              bool *dest);
s_str * crypt_hash_password (const s_str *pass, s_str *dest);
s_str * crypt_sha512 (const s_str *key, const s_str *setting,
                      s_str *dest);

#endif /* LIBKC3_CRYPT_H */
