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
#include "../libkc3/assert.h"
#include "../libkc3/callable.h"
#include "../libkc3/eval.h"
#include "../libkc3/list.h"
#include "../libkc3/pcallable.h"
#include "../libkc3/tag.h"
#include "../libkc3/tuple.h"
#include "signal.h"

void kc3_gtk4_signal_callback (GObject *object, s_tuple *tuple)
{
  s_list *args = NULL;
  s_callable *callable;
  s_tag tag;
  s_list *tmp;
  if (tuple->count != 2 ||
      tuple->tag[0].type != TAG_PCALLABLE) {
    err_puts("kc3_gtk4_signal_callback: invalid tuple");
    assert(! "kc3_gtk4_signal_callback: invalid tuple");
    abort();
  }
  callable = tuple->tag->data.pcallable;
  if (! callable ||
      callable->type == CALLABLE_VOID) {
    err_puts("kc3_gtk4_signal_callback: invalid callable");
    assert(! "kc3_gtk4_signal_callback: invalid callable");
    abort();
  }
  if (! (tmp = list_new_tag_copy(tuple->tag + 1, NULL))) {
    err_puts("kc3_gtk4_signal_callback: list_new_tag_copy");
    assert(! "kc3_gtk4_signal_callback: list_new_tag_copy");
    abort();
  }
  args = tmp;
  if (! (tmp = list_new_ptr(object, args))) {
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

void kc3_gtk4_signal_connect (GObject **object,
                              const s_str *detailed_signal,
                              s_callable **callback,
                              s_tag *data)
{
  s_tuple *tuple;
  if (! (tuple = tuple_new(2))) {
    err_puts("kc3_gtk4_signal_connect: tuple_new");
    assert(! "kc3_gtk4_signal_connect: tuple_new");
    abort();
  }
  tuple->tag->type = TAG_PCALLABLE;
  if (! pcallable_init_copy(&tuple->tag->data.pcallable, callback)) {
    err_puts("kc3_gtk4_signal_connect: pcallable_init_copy");
    assert(! "kc3_gtk4_signal_connect: pcallable_init_copy");
    abort();
  }
  if (! tag_init_copy(tuple->tag + 1, data)) {
    err_puts("kc3_gtk4_signal_connect: tag_init_copy");
    assert(! "kc3_gtk4_signal_connect: tag_init_copy");
    abort();
  }
  g_signal_connect(*object, detailed_signal->ptr.pchar,
                   G_CALLBACK(kc3_gtk4_signal_callback), tuple);
}
