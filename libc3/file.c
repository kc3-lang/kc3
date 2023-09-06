/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <err.h>
#include <sys/stat.h>
#include "file.h"
#include "timespec.h"

s_tag * file_mtime (const s_str *path, s_tag *dest)
{
  struct stat sb;
  assert(path);
  assert(dest);
  if (stat(path->ptr.ps8, &sb)) {
    warn("file_mtime: %s", path->ptr.ps8);
    return NULL;
  }
  return timespec_to_tag(&sb.st_mtim, dest);
}
