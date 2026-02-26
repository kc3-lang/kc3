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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>
#include "../../../libkc3/kc3.h"
#include "window_cairo_xcb.h"

bool window_cairo_run (s_window_cairo *window)
{
  return window_cairo_xcb_run(window);
}

bool window_cairo_xcb_event (s_window_cairo *window,
                             xcb_connection_t *conn,
                             xcb_screen_t *screen,
                             xcb_visualtype_t *visual,
                             xcb_pixmap_t *pixmap,
                             cairo_surface_t **surface,
                             xcb_window_t xcb_window,
                             xcb_intern_atom_reply_t *delete_reply,
                             xcb_generic_event_t *event,
                             struct xkb_state *xkb_state)
{
  xcb_button_press_event_t     *event_button;
  xcb_configure_notify_event_t *event_config;
  xcb_key_press_event_t        *event_key;
  xcb_motion_notify_event_t    *event_motion;
  switch (event->response_type & ~0x80) {
  case XCB_BUTTON_PRESS:
    event_button = (xcb_button_press_event_t *) event;
    if (! window->button(window, event_button->detail,
                         event_button->event_x,
                         window->h - event_button->event_y))
      goto ko;
    break;
  case XCB_EXPOSE:
    if (! window->render(window))
      goto ko;
    xcb_gcontext_t gc = xcb_generate_id(conn);
    u32 gc_mask = XCB_GC_GRAPHICS_EXPOSURES;
    u32 gc_values[1] = {0};
    xcb_create_gc(conn, gc, xcb_window, gc_mask, gc_values);
    xcb_copy_area(conn, *pixmap, xcb_window, gc,
                  0, 0, 0, 0, window->w, window->h);
    xcb_flush(conn);
    break;
  case XCB_CONFIGURE_NOTIFY:
    event_config = (xcb_configure_notify_event_t *) event;
    if (! window->resize(window, event_config->width,
                         event_config->height))
      goto ko;
    window->w = event_config->width;
    window->h = event_config->height;
    cairo_destroy(window->cr);
    cairo_surface_destroy(*surface);
    xcb_free_pixmap(conn, *pixmap);
    *pixmap = xcb_generate_id(conn);
    xcb_create_pixmap(conn, screen->root_depth, *pixmap, xcb_window,
                      window->w, window->h);
    *surface = cairo_xcb_surface_create(conn, *pixmap, visual,
                                        window->w, window->h);
    window->cr = cairo_create(*surface);
    break;
  case XCB_KEY_PRESS:
    event_key = (xcb_key_press_event_t *) event;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(xkb_state,
                                                 event_key->detail);
    if (! window->key(window, sym))
      goto ko;
    break;
  case XCB_MOTION_NOTIFY:
    event_motion = (xcb_motion_notify_event_t *) event;
    if (! window->motion(window, event_motion->event_x,
                         event_motion->event_y))
      goto ko;
    break;
  case XCB_CLIENT_MESSAGE:
    if((*(xcb_client_message_event_t *) event).data.data32[0] ==
       (*delete_reply).atom) {
      printf("window close\n");
      // callback ?
      g_kc3_exit_code = 0;
      goto ko;
    }
    break;
  default:
    printf("event type %d\n", event->response_type & ~0x80);
  }
  alloc_free(event);
  return true;
 ko:
  alloc_free(event);
  return false;
}

bool window_cairo_xcb_run (s_window_cairo *window)
{
  xcb_connection_t *conn;
  xcb_intern_atom_cookie_t cookie;
  xcb_intern_atom_cookie_t cookie2;
  xcb_generic_event_t *event;
  xcb_pixmap_t pixmap;
  bool r;
  xcb_intern_atom_reply_t* protocols_reply;
  xcb_intern_atom_reply_t* delete_reply;
  xcb_screen_t *screen;
  xcb_visualtype_t *screen_visual;
  s_timespec sleep;
  cairo_surface_t *surface;
  u32 value_mask;
  u32 *value_list;
  xcb_window_t xcb_window;
  struct xkb_context *xkb_ctx;
  int32_t xkb_device_id;
  struct xkb_keymap *xkb_keymap;
  struct xkb_state *xkb_state;
  conn = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(conn)) {
    fprintf(stderr, "Error opening display.\n");
    return false;
  }
  xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!xkb_ctx) {
    fprintf(stderr, "Failed to create XKB context\n");
    return false;
  }
  xkb_x11_setup_xkb_extension(conn, 1, 0, 0, NULL, NULL, NULL, NULL);
  xkb_device_id = xkb_x11_get_core_keyboard_device_id(conn);
  if (xkb_device_id == -1) {
    fprintf(stderr, "Failed to get XKB device ID\n");
    return false;
  }
  xkb_keymap =
    xkb_x11_keymap_new_from_device(xkb_ctx, conn, xkb_device_id,
                                   XKB_KEYMAP_COMPILE_NO_FLAGS);
  xkb_state = xkb_x11_state_new_from_device(xkb_keymap, conn,
                                            xkb_device_id);
  screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
  screen_visual = xcb_screen_visual_type(screen);
  xcb_window = xcb_generate_id(conn);
  value_mask = XCB_CW_EVENT_MASK;
  value_list = (u32[]) {
    XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_EXPOSURE |
    XCB_EVENT_MASK_KEY_PRESS |
    XCB_EVENT_MASK_POINTER_MOTION |
    XCB_EVENT_MASK_STRUCTURE_NOTIFY
  };
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, xcb_window,
                    screen->root, window->x, window->y,
                    window->w, window->h, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual,
                    value_mask, value_list);
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xcb_window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                      strlen(window->title), window->title);
  cookie = xcb_intern_atom(conn, 1, 12, "WM_PROTOCOLS");
  protocols_reply = xcb_intern_atom_reply(conn, cookie, 0);
  cookie2 = xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");
  delete_reply = xcb_intern_atom_reply(conn, cookie2, 0);
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xcb_window,
                      (*protocols_reply).atom, 4, 32, 1,
                      &(*delete_reply).atom);
  xcb_map_window(conn, xcb_window);
  pixmap = xcb_generate_id(conn);
  xcb_create_pixmap(conn, screen->root_depth, pixmap, xcb_window,
                    window->w, window->h);
  surface = cairo_xcb_surface_create(conn, pixmap, screen_visual,
                                     window->w, window->h);
  window->cr = cairo_create(surface);
  xcb_flush(conn);
  if (! (r = window->load(window)))
    goto clean;
  while (1) {
    if ((event = xcb_poll_for_event(conn))) {
      if (! (r = window_cairo_xcb_event(window, conn, screen,
                                        screen_visual, &pixmap,
                                        &surface, xcb_window,
                                        delete_reply,
                                        event, xkb_state)))
        goto clean;
    }
    else {
      sleep.tv_sec = 0;
      sleep.tv_nsec = 1000000000 / 120;
      nanosleep(&sleep, NULL);
      xcb_expose_event_t event = {0};
      event.response_type = XCB_EXPOSE;
      event.window = xcb_window;
      event.x = 0;
      event.y = 0;
      event.width = window->w;
      event.height = window->h;
      event.count = 0;
      xcb_send_event(conn, false, xcb_window, XCB_EVENT_MASK_EXPOSURE,
                     (const char *) &event);
      xcb_flush(conn);
    }
  }
 clean:
  cairo_surface_destroy(surface);
  cairo_destroy(window->cr);
  xkb_state_unref(xkb_state);
  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_ctx);
  xcb_disconnect(conn);
  return r;
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
