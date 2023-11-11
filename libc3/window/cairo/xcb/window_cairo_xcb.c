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

bool window_cairo (sw x, sw y, sw w, sw h,
                   const s8 *title,
                   f_window_cairo_render render
                   f_window_cairo_resize resize)
{
  return window_cairo_xcb(x, y, w, h, title, render, resize);
}

bool window_cairo_xcb (sw x, sw y, sw w, sw h,
                       const s8 *title,
                       f_window_cairo_render render,
                       f_window_cairo_resize resize)
{
  xcb_connection_t *conn;
  cairo_t *cr;
  xcb_screen_t *screen;
  xcb_visualtype_t *screen_visual;
  cairo_surface_t *surface;
  xcb_window_t window;
  xcb_generic_event_t *event;
  conn = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(conn)) {
    fprintf(stderr, "Error opening display.\n");
    exit(1);
  }
  screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
  screen_visual = xcb_screen_visual_type(screen);
  window = xcb_generate_id(conn);
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, screen->root, x, y, w, h, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0, NULL);
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, 12, title);
  xcb_map_window(conn, window);
  xcb_flush(conn);
  surface = cairo_xcb_surface_create(conn, window, screen_visual, 800, 600);
  cr = cairo_create(surface);
  if (render(cr)) {
    cairo_surface_flush(surface);
    xcb_flush(conn);
    while ((event = xcb_wait_for_event(conn))) {
      if ((event->response_type & ~0x80) == XCB_EXPOSE) {
        if (! render(cr))
          break;
        cairo_surface_flush(surface);
        xcb_flush(conn);
      }
      free(event);
    }
  }
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  xcb_disconnect(conn);
  return 0;
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
