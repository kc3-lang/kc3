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
#include "binding.h"
#include "compare.h"
#include "list.h"
#include "tag.h"

s_binding * binding_delete (s_binding *binding)
{
  s_binding *next;
  assert(binding);
  tag_clean(&binding->value);
  next = binding->next;
  free(binding);
  return next;
}

void binding_delete_all (s_binding *binding)
{
  s_binding *b;
  b = binding;
  while (b)
    b = binding_delete(b);
}

s_binding ** binding_find (s_binding **binding, const s_sym *name)
{
  s_binding **b;
  b = binding;
  while (*b) {
    if (! compare_sym(name, (*b)->name))
      return b;
    b = &(*b)->next;
  }
  return NULL;
}

s_tag * binding_get (s_binding *binding, const s_sym *name)
{
  s_binding *b;
  b = binding;
  while (b) {
    if (b->name == name)
      return &b->value;
    b = b->next;
  }
  return NULL;
}

s_tag * binding_get_w (s_binding *binding, const s_sym *name)
{
  s_binding *b;
  b = binding;
  while (b) {
    if (b->name == name)
      return &b->value;
    b = b->next;
  }
  return NULL;
}

s_binding * binding_init (s_binding *binding, const s_sym *name,
                          s_binding *next)
{
  s_binding tmp = {0};
  assert(binding);
  tag_init_void(&tmp.value);
  tmp.name = name;
  tmp.next = next;
  *binding = tmp;
  return binding;
}

s_binding * binding_new (const s_sym *name, s_binding *next)
{
  s_binding *binding;
  binding = alloc(sizeof(s_binding));
  if (! binding)
    return NULL;
  if (! binding_init(binding, name, next)) {
    free(binding);
    return NULL;
  }
  return binding;
}

s_binding * binding_new_copy (s_binding *src)
{
  s_binding **b;
  s_binding  *binding;
  s_binding *s;
  binding = NULL;
  b = &binding;
  s = src;
  while (s) {
    if (! (*b = binding_new(s->name, NULL)))
      goto clean;
    if (! tag_init_copy(&(*b)->value, &s->value))
      goto clean;
    b = &(*b)->next;
    s = s->next;
  }
  return binding;
 clean:
  binding_delete_all(binding);
  return NULL;
}
