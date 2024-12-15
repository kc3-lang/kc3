#include "button.h"

GtkWidget ** kc3_gtk4_button_new (GtkWidget ** dest )
{
  GtkWidget * tmp;
  if (! (tmp = gtk_button_new()))
    return NULL;
  *dest = tmp;
  return dest;
}
      
GtkWidget ** kc3_gtk4_button_new_with_label (GtkWidget **dest,
                                             s_str *label)
{
  GtkWidget * tmp;
  if (! (tmp = gtk_button_new_with_label(label->ptr.pchar)))
    return NULL;
  *dest = tmp;
  return dest;
}
