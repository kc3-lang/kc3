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
#include "application.h"

#define G_APPLICATION_DEFAULT_FLAGS 0

void kc3_gtk4_application_delete (GtkApplication **app)
{
  g_object_unref(*app);
}

GtkApplication ** kc3_gtk4_application_new (GtkApplication **dest,
                                            const s_str *name,
                                            const s_str *id)
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

s32 kc3_gtk4_application_run (GtkApplication **app)
{
  return g_application_run(G_APPLICATION(*app), 1,
                           env_global()->argv);
}

void kc3_gtk4_application_set_accel_for_action (GtkApplication **app,
                                                const s_str *action,
                                                const s_str *accel)
{
  gtk_application_set_accels_for_action(GTK_APPLICATION(*app),
                                        action->ptr.pchar,
                                        (const char *[]) {
                                          accel->ptr.pchar,
                                          NULL
                                        });
}

void kc3_gtk4_application_set_menubar (GtkApplication **app,
                                       GMenuModel **menu)
{
  assert(app);
  assert(*app);
  assert(menu);
  assert(*menu);
  gtk_application_set_menubar(GTK_APPLICATION(*app),
                              G_MENU_MODEL(*menu));
}
