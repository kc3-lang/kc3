/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <libkc3/assert.h>
#include "application.h"

void kc3_gtk4_application_delete (GtkApplication **app)
{
  g_object_unref(*app);
}

s32 kc3_gtk4_application_run (GtkApplication **app)
{
  return g_application_run(G_APPLICATION(*app), 1,
                           g_kc3_env->argv);
}

GtkApplication ** kc3_gtk4_application_new (const s_str *name,
                                            const s_str *id,
                                            GtkApplication **dest)
{
  GtkApplication *tmp;
  assert(name);
  g_set_application_name(name->ptr.pchar);
  if (! (tmp = gtk_application_new(id->ptr.pchar,
                                   G_APPLICATION_DEFAULT_FLAGS)))
    return NULL;
  *dest = tmp;
  return dest;
}
