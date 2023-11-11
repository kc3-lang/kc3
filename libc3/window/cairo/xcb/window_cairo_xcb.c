/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <stdio.h>
#include <stdlib.h>
#include <cairo/cairo-xcb.h>
#include <libc3/c3.h>
#include <xcb/xcb.h>
#include "window_cairo_xcb.h"

bool window_cairo_run (s_window_cairo *window)
{
  return window_cairo_xcb_run(window);
}

bool window_cairo_xcb_run (s_window_cairo *window)
{
  xcb_configure_notify_event_t *configure_event;
  xcb_connection_t *conn;
  cairo_t *cr;
  xcb_screen_t *screen;
  xcb_visualtype_t *screen_visual;
  cairo_surface_t *surface;
  xcb_window_t xcb_window;
  xcb_generic_event_t *event;
  conn = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(conn)) {
    fprintf(stderr, "Error opening display.\n");
    exit(1);
  }
  screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
  screen_visual = xcb_screen_visual_type(screen);
  xcb_window = xcb_generate_id(conn);
  uint32_t value_mask = XCB_CW_EVENT_MASK;
  uint32_t value_list[1] = {XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, xcb_window, screen->root,
                    window->x, window->y, window->w, window->h, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual,
                    value_mask, value_list);
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xcb_window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, 12,
                      window->title);
  xcb_map_window(conn, xcb_window);
  xcb_flush(conn);
  surface = cairo_xcb_surface_create(conn, xcb_window, screen_visual, 800, 600);
  cr = cairo_create(surface);
  window->cr = cr;
  if (window->render(window, cr)) {
    cairo_surface_flush(surface);
    xcb_flush(conn);
    while ((event = xcb_wait_for_event(conn))) {
      switch (event->response_type & ~0x80) {
      case XCB_EXPOSE:
        if (! window->render(window, cr)) {
          free(event);
          goto exit_loop;
        }
        cairo_surface_flush(surface);
        break;
      case XCB_CONFIGURE_NOTIFY:
        configure_event = (xcb_configure_notify_event_t *) event;
        cairo_xcb_surface_set_size(surface, configure_event->width, configure_event->height);
        if (! window->resize(window, configure_event->width,
                             configure_event->height)) {
          free(event);
          goto exit_loop;
        }
        break;
      }
      free(event);
      xcb_flush(conn);
    }
  }
 exit_loop:
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  xcb_disconnect(conn);
  return true;
}

xcb_visualtype_t * xcb_screen_visual_type (xcb_screen_t *screen)
{
  xcb_depth_iterator_t depth_iter;
  xcb_visualtype_iterator_t visual_iter;
  depth_iter = xcb_screen_allowed_depths_iterator(screen);
  while (depth_iter.rem) {
    visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
    while (visual_iter.rem) {
      if (screen->root_visual == visual_iter.data->visual_id) {
        return visual_iter.data;
      }
      xcb_visualtype_next(&visual_iter);
    }
    xcb_depth_next(&depth_iter);
  }
  return NULL;
}
