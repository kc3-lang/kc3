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
#include "alloc.h"
#include "assert.h"
#include "binding.h"
#include "list.h"
#include "tag.h"

void binding_delete (s_binding *binding)
{
  assert(binding);
  tag_clean(&binding->value);
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

const s_tag * binding_get (const s_binding *binding, const s_sym *name)
{
  while (binding) {
    if (binding->name == name)
      return &binding->value;
    binding = binding->next;
  }
  return NULL;
}

s_binding * binding_init (s_binding *binding, const s_sym *name,
                          const s_tag *value, s_binding *next)
{
  s_binding tmp = {0};
  assert(binding);
  if (! tag_init_copy(&tmp.value, value))
    return NULL;
  tmp.name = name;
  tmp.next = next;
  *binding = tmp;
  return binding;
}

s_binding * binding_new (const s_sym *name, const s_tag *value,
                         s_binding *next)
{
  s_binding *binding;
  binding = alloc(sizeof(s_binding));
  if (! binding)
    return NULL;
  if (! binding_init(binding, name, value, next)) {
    free(binding);
    return NULL;
  }
  return binding;
}
