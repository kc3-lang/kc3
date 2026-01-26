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
#include "text_view.h"

s_str * kc3_gtk4_text_view_get_text (GtkTextView **text_view,
                                     s_str *dest)
{
  GtkTextBuffer *buffer;
  GtkTextIter end;
  char *s;
  GtkTextIter start;
  assert(text_view);
  assert(dest);
  if (! (buffer = gtk_text_view_get_buffer(*text_view))) {
    err_puts("kc3_gtk4_text_view_get_text: gtk_text_view_get_buffer");
    assert(! "kc3_gtk4_text_view_get_text: gtk_text_view_get_buffer");
    return NULL;
  }
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  if (! (s = gtk_text_buffer_get_text(buffer, &start, &end, false))) {
    err_puts("kc3_gtk4_text_view_get_text: gtk_text_buffer_get_text");
    assert(! "kc3_gtk4_text_view_get_text: gtk_text_buffer_get_text");
    return NULL;
  }
  return str_init_1_alloc(dest, s);
}

GtkWidget ** kc3_gtk4_text_view_new (GtkWidget **dest)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_text_view_new()))
    return NULL;
  *dest = tmp;
  return dest;
}

GtkWidget ** kc3_gtk4_text_view_new_with_buffer (GtkWidget **dest,
                                                 GtkTextBuffer **tb)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_text_view_new_with_buffer(*tb)))
    return NULL;
  *dest = tmp;
  return dest;
}

void kc3_gtk4_text_view_set_monospace (GtkTextView **text_view,
                                       bool monospace)
{
  assert(text_view);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(*text_view), monospace);
}

void kc3_gtk4_text_view_set_text (GtkTextView **text_view,
                                  s_str *text)
{
  GtkTextBuffer *buffer;
  assert(text_view);
  assert(text);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(*text_view));
  if (! buffer) {
    err_puts("kc3_gtk4_text_view_set_text: gtk_text_view_get_buffer");
    assert(! "kc3_gtk4_text_view_set_text: gtk_text_view_get_buffer");
    return;
  }
  gtk_text_buffer_set_text(buffer, text->ptr.pchar, text->size);
}
