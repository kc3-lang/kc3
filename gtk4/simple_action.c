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
#include "simple_action.h"

static s_action_callback *g_action_callbacks = NULL;

void kc3_gtk4_simple_action_callback (GSimpleAction *action)
{
  s_list *args;
  s_callable *callable;
  s_action_callback *cb;
  s_tag tag;
  s_list *tmp;
  cb = g_action_callbacks;
  while (cb && cb->action != action)
    cb = cb->next;
  if (! cb) {
    err_puts("kc3_gtk4_simple_action_callback:"
             " action not found in callbacks");
    assert(!("kc3_gtk4_simple_action_callback:"
             " action not found in callbacks"));
    abort();
  }
  callable = cb->callable;
  if (! callable ||
      callable->type == CALLABLE_VOID) {
    err_puts("kc3_gtk4_signal_callback: invalid callable");
    assert(! "kc3_gtk4_signal_callback: invalid callable");
    abort();
  }
  if (! (tmp = list_new_tag_copy(&cb->tag, NULL))) {
    err_puts("kc3_gtk4_signal_callback: list_new_tag_copy");
    assert(! "kc3_gtk4_signal_callback: list_new_tag_copy");
    abort();
  }
  args = tmp;
  if (! (tmp = list_new_ptr(action, args))) {
    err_puts("kc3_gtk4_signal_callback: list_new_tag_copy");
    assert(! "kc3_gtk4_signal_callback: list_new_tag_copy");
    abort();
  }
  args = tmp;  
  if (! eval_callable_call(callable, args, &tag)) {
    err_puts("kc3_gtk4_signal_callback: eval_callable_call");
    assert(! "kc3_gtk4_signal_callback: eval_callable_call");
    abort();
  }
  tag_clean(&tag);
  list_delete_all(args);

}

GSimpleAction ** kc3_gtk4_simple_action_new (GSimpleAction **dest,
                                             const s_str *name,
                                             p_callable *callable,
                                             s_tag *data)
{
  s_action_callback *cb;
  GSimpleAction *tmp;
  if (! (cb = alloc(sizeof(s_action_callback))))
    return NULL;
  if (! (tmp = g_simple_action_new(name->ptr.pchar, NULL)))
    return NULL;
  cb->action = tmp;
  cb->next = g_action_callbacks;
  g_action_callbacks = cb;
  if (! (pcallable_init_copy(&cb->callable, callable))) {
    err_puts("kc3_gtk4_simple_action_new: pcallable_init_copy");
    assert(! "kc3_gtk4_simple_action_new: pcallable_init_copy");
    return NULL;
  }
  if (! (tag_init_copy(&cb->tag, data))) {
    err_puts("kc3_gtk4_simple_action_new: tag_init_copy");
    assert(! "kc3_gtk4_simple_action_new: tag_init_copy");
    return NULL;
  }
  g_signal_connect(tmp, "activate",
                   G_CALLBACK(kc3_gtk4_simple_action_callback),
                   NULL);
  *dest = tmp;
  return dest;
}
