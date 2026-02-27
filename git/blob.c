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
#include "../libkc3/kc3.h"
#include <string.h>
#include <git2.h>
#include "blob.h"

s_str * kc3_git_blob_to_str (git_repository **repo, const s_str *hash,
                             s_str *dest)
{
  git_blob *blob = NULL;
  git_oid oid = {0};
  const u8 *raw;
  u64 size;
  s_str tmp = {0};
  assert(repo);
  assert(*repo);
  assert(hash);
  assert(dest);
  if (git_oid_fromstr(&oid, hash->ptr.p_pchar)) {
    err_puts("kc3_git_blob_to_str: invalid hash");
    return NULL;
  }
  if (git_blob_lookup(&blob, *repo, &oid)) {
    err_puts("kc3_git_blob_to_str: blob not found in repo");
    return NULL;
  }
  size = git_blob_rawsize(blob);
  if (! size) {
    git_blob_free(blob);
    return str_init_empty(dest);
  }
  raw = git_blob_rawcontent(blob);
  if (! raw) {
    err_puts("kc3_git_blob_to_str: git_blob_rawcontent");
    git_blob_free(blob);
    return NULL;
  }
  if (! str_init_alloc(&tmp, size)) {
    git_blob_free(blob);
    return NULL;
  }
  memcpy(tmp.free.p_pvoid, raw, size);
  git_blob_free(blob);
  *dest = tmp;
  return dest;
}
