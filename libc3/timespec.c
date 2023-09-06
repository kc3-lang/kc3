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
#include "tag.h"
#include "timespec.h"

s_tag * timespec_to_tag (const struct timespec *time, s_tag *dest)
{
  s_tag tmp;
  tag_init_tuple(&tmp, 2);
  tag_init_s64(&tmp.data.tuple.tag[0], time->tv_sec);
  tag_init_s64(&tmp.data.tuple.tag[1], time->tv_nsec);
  *dest = tmp;
  return dest;
}
