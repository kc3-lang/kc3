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
#include "kc3_glib.h"

bool g_kc3_g_main_stop = false;

/*
static void kc3_g_signal_callback (GObject *object,
                                   s_callable *callback);
*/

bool * kc3_g_main_context_iteration (bool *dest)
{
  *dest = g_main_context_iteration(NULL, TRUE);
  return dest;
}

void kc3_g_main (void)
{
  while (! g_kc3_g_main_stop)
    g_main_context_iteration(NULL, TRUE);
}

/*
static void kc3_g_signal_callback (GObject *object,
                                   s_callable *callback)
{
  s_list *arguments;
  s_tag tag;
  if (! (arguments = list_new_ptr(object, NULL)))
    return;
  eval_callable_call(callback, arguments, &tag);
  tag_clean(&tag);
  list_delete_all(arguments);
}

void kc3_g_signal_connect (GObject **instance, const s_str *signal,
			   p_callable *callback)
{
  assert(instance);
  assert(signal);
  assert(callback);
  g_signal_connect(*instance, signal->ptr.pchar,
		   G_CALLBACK(kc3_g_signal_callback),
		   *callback);
}
*/
