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
#include <fcntl.h>
#include "fd.h"

bool * fd_set_blocking (s32 fd, bool blocking, bool *result)
{
  s32 flags;
  flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    return NULL;
  if (! blocking)
    flags |= O_NONBLOCK;
  else
    flags &= ~ (s32) O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) == -1)
    return NULL;
  *result = true;
  return result;
}
