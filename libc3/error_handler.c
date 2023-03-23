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
#include <stdlib.h>
#include "error_handler.h"

void error_handler_clean (s_error_handler *error_handler)
{
  assert(error_handler);
  (void) error_handler;  
}

s_error_handler * error_handler_delete (s_error_handler *error_handler)
{
  s_error_handler *next;
  assert(error_handler);
  next = error_handler->next;
  error_handler_clean(error_handler);
  free(error_handler);
  return next;
}

void error_handler_delete_all (s_error_handler *error_handler)
{
  while (error_handler)
    error_handler = error_handler_delete(error_handler);
}

s_error_handler *
error_handler_init (s_error_handler *error_handler,
                    s_error_handler *next)
{
  assert(error_handler);
  error_handler->next = next;
  return error_handler;
}

s_error_handler * error_handler_new (s_error_handler *next)
{
  s_error_handler *error_handler;
  if (! (error_handler = malloc(sizeof(s_error_handler))))
    errx(1, "error_handler_new: out of memory");
  return error_handler_init(error_handler, next);
}
