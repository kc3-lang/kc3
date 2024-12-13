#include "button.h"

GtkWidget ** kc3_gtk4_button_new (GtkWidget ** dest )
{
  GtkWidget * tmp;
  if (! (tmp = gtk_button_new()))
    return NULL;
  *dest = tmp;
  return dest;
}
      
