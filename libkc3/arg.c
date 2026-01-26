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
#include "alloc.h"
#include "assert.h"
#include "arg.h"

s_arg * arg_delete (s_arg *arg)
{
  s_arg *next = NULL;
  if (arg) {
    next = arg->next;
    free(arg);
  }
  return next;
}

void arg_delete_all (s_arg *arg)
{
  while (arg)
    arg = arg_delete(arg);
}

s_arg * arg_init (s_arg *arg)
{
  assert(arg);
  *arg = (s_arg) {0};
  return arg;
}

uw arg_length (s_arg *arg)
{
  uw length = 0;
  while (arg) {
    length++;
    arg = arg->next;
  }
  return length;
}

s_arg * arg_new (void)
{
  s_arg *arg;
  arg = alloc(sizeof(s_arg));
  if (! arg)
    return NULL;
  if (! arg_init(arg)) {
    free(arg);
    return NULL;
  }
  return arg;
}
