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
#include "../libkc3/s32.h"
#include "../libkc3/sym.h"
#include "widget.h"

GtkWidget ** kc3_gtk4_widget_set_halign (GtkWidget **widget,
                                         const s_sym * const *align)
{
  GtkAlign a = 0;
  if (*align == sym_1("fill"))
    a = GTK_ALIGN_FILL;
  else if (*align == sym_1("start"))
    a = GTK_ALIGN_START;
  else if (*align == sym_1("end"))
    a = GTK_ALIGN_END;
  else if (*align == sym_1("center"))
    a = GTK_ALIGN_CENTER;
#if GTK_VERSION_MINOR >= 12
  else if (*align == sym_1("baseline_fill"))
    a = GTK_ALIGN_BASELINE_FILL;
#endif
  else if (*align == sym_1("baseline"))
    //a = GTK_ALIGN_BASELINE;
    a = GTK_ALIGN_CENTER + 1;
#if GTK_VERSION_MINOR >= 12
  else if (*align == sym_1("baseline_center"))
    a = GTK_ALIGN_BASELINE_CENTER;
#endif
  else {
    err_puts("kc3_gtk4_widget_set_halign: invalid alignement");
    assert(! "kc3_gtk4_widget_set_halign: invalid alignement");
    return NULL;
  }
  gtk_widget_set_halign(*widget, a);
  return widget;
}

void kc3_gtk4_widget_set_hexpand (GtkWidget **widget,
                                  bool expand)
{
  gtk_widget_set_hexpand(*widget, expand);
}

GtkWidget ** kc3_gtk4_widget_set_size_request (GtkWidget **widget,
                                               const s_tag *width,
                                               const s_tag *height)
{
  s32 h;
  const s_sym *sym_S32 = &g_sym_S32;
  s32 w;
  if (! s32_init_cast(&w, &sym_S32, width)) {
    err_puts("kc3_gtk4_widget_set_size_request: invalid width");
    return NULL;
  }
  if (! s32_init_cast(&h, &sym_S32, height)) {
    err_puts("kc3_gtk4_widget_set_size_request: invalid height");
    return NULL;
  }
  gtk_widget_set_size_request(*widget, w, h);
  return widget;
}

void kc3_gtk4_widget_set_vexpand (GtkWidget **widget,
                                  bool expand)
{
  gtk_widget_set_vexpand(*widget, expand);
}
