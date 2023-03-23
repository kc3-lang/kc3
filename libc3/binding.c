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
#include "binding.h"
#include "list.h"

void binding_delete (s_binding *binding)
{
  free(binding);
}

void binding_delete_all (s_binding *binding)
{
  s_binding *next;
  while (binding) {
    next = binding->next;
    binding_delete(binding);
    binding = next;
  }
}

const s_tag * binding_get (s_binding *binding, const s_sym *name)
{
  while (binding) {
    if (binding->name == name)
      return binding->value;
    binding = binding->next;
  }
  return NULL;
}

s_binding * binding_init (s_binding *binding, const s_sym *name,
                          const s_tag *value, s_binding *next)
{
  assert(binding);
  binding->name = name;
  binding->value = value;
  binding->next = next;
  return binding;
}

s_binding * binding_new (const s_sym *name, const s_tag *value,
                         s_binding *next)
{
  s_binding *binding;
  if (! (binding = malloc(sizeof(s_binding))))
    errx(1, "binding_new: out of memory");
  return binding_init(binding, name, value, next);
}
