/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <strings.h>
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
  bzero(arg, sizeof(s_arg));
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

s_arg * arg_new (s_arg *next)
{
  s_arg *arg;
  if (! (arg = malloc(sizeof(s_arg))))
    errx(1, "arg_new: out of memory");
  return arg_init(arg, next);
}
