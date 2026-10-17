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
#include <string.h>
#include "../libkc3/assert.h"
#include "../libkc3/callable.h"
#include "../libkc3/eval.h"
#include "../libkc3/list.h"
#include "../libkc3/pcallable.h"
#include "../libkc3/str.h"
#include "../libkc3/tag.h"
#include "../libkc3/tuple.h"
#include "signal.h"

void kc3_gtk4_signal_callback (GObject *object, s_tuple *tuple)
{
  s_list *args = NULL;
  s_callable *callable;
  s_tag tag = {0};
  s_list *tmp;
  if (tuple->count != 2 ||
      tuple->tag[0].type != TAG_PCALLABLE) {
    err_puts("kc3_gtk4_signal_callback: invalid tuple");
    assert(! "kc3_gtk4_signal_callback: invalid tuple");
    abort();
  }
  callable = tuple->tag->data.td_pcallable;
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

static void kc3_gtk4_signal_data_clean (gpointer data, GClosure *closure)
{
  (void) closure;
  tuple_delete(data);
}

void kc3_gtk4_signal_connect (GObject **object,
                              const s_str *detailed_signal,
                              s_callable **callback,
                              s_tag *data)
{
  s_tag data_copy = {0};
  GQuark detail;
  s_str name;
  GSignalQuery query = {0};
  guint signal_id;
  s_tuple *tuple;
  if (! object || ! G_IS_OBJECT(*object) || ! detailed_signal ||
      ! callback || ! *callback || (*callback)->type == CALLABLE_VOID ||
      ! data) {
    err_puts("kc3_gtk4_signal_connect: invalid argument");
    return;
  }
  if (! detailed_signal->size || detailed_signal->size >= S32_MAX ||
      ! detailed_signal->ptr.p_pchar ||
      memchr(detailed_signal->ptr.p_pchar, 0, detailed_signal->size)) {
    err_puts("kc3_gtk4_signal_connect: invalid signal name");
    return;
  }
  if (! str_init_copy(&name, detailed_signal))
    return;
  if (! g_signal_parse_name(name.ptr.p_pchar, G_OBJECT_TYPE(*object),
                            &signal_id, &detail, TRUE)) {
    err_puts("kc3_gtk4_signal_connect: unknown signal");
    goto clean;
  }
  g_signal_query(signal_id, &query);
  if (! query.signal_id || query.n_params ||
      query.return_type != G_TYPE_NONE ||
      (detail && ! (query.signal_flags & G_SIGNAL_DETAILED))) {
    err_puts("kc3_gtk4_signal_connect: unsupported signal signature"
             " or detail (requires no parameters and no return value)");
    goto clean;
  }
  if (! (tuple = tuple_new(2))) {
    err_puts("kc3_gtk4_signal_connect: tuple_new");
    goto clean;
  }
  if (! pcallable_init_copy(&tuple->tag->data.td_pcallable, callback)) {
    err_puts("kc3_gtk4_signal_connect: pcallable_init_copy");
    goto clean_tuple;
  }
  tuple->tag->type = TAG_PCALLABLE;
  if (! tag_init_copy(&data_copy, data)) {
    err_puts("kc3_gtk4_signal_connect: tag_init_copy");
    goto clean_tuple;
  }
  tuple->tag[1] = data_copy;
  g_signal_connect_data(*object, name.ptr.p_pchar,
                        G_CALLBACK(kc3_gtk4_signal_callback), tuple,
                        kc3_gtk4_signal_data_clean, 0);
  goto clean;
 clean_tuple:
  tuple_delete(tuple);
 clean:
  str_clean(&name);
}
