/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>
#include "../../../libkc3/kc3.h"
#include "../../../gl/gl.h"
#include "window_egl_xcb.h"

#ifndef EGL_PLATFORM_XCB_EXT
#define EGL_PLATFORM_XCB_EXT 0x31DC
#endif

bool window_egl_run (s_window_egl *window)
{
  return window_egl_xcb_run(window);
}

static bool window_egl_xcb_setup (s_window_egl *window,
                                  xcb_connection_t *conn,
                                  xcb_screen_t *screen,
                                  xcb_window_t xcb_window)
{
  EGLint major, minor;
  EGLConfig config;
  EGLint    config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_DEPTH_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_NONE
  };
  EGLint context_attribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MINOR_VERSION, 0,
    EGL_NONE
  };
  EGLint gl_w, gl_h;
  EGLint num_configs;
  (void) screen;
  window->egl_display = eglGetPlatformDisplay(EGL_PLATFORM_XCB_EXT,
                                              conn, NULL);
  if (window->egl_display == EGL_NO_DISPLAY) {
    err_puts("window_egl_xcb_setup: eglGetDisplay failed");
    return false;
  }
  if (! eglInitialize(window->egl_display, &major, &minor)) {
    err_puts("window_egl_xcb_setup: eglInitialize failed");
    return false;
  }
  if (! eglChooseConfig(window->egl_display, config_attribs, &config, 1,
                        &num_configs) || num_configs != 1) {
    err_puts("window_egl_xcb_setup: eglChooseConfig failed");
    return false;
  }
  window->egl_config = config;
  eglBindAPI(EGL_OPENGL_ES_API);
  window->egl_surface = eglCreatePlatformWindowSurface
    (window->egl_display, config, &xcb_window, NULL);
  if (window->egl_surface == EGL_NO_SURFACE) {
    err_puts("window_egl_xcb_setup: eglCreateWindowSurface failed");
    return false;
  }
  window->egl_context = eglCreateContext(window->egl_display, config,
                                         EGL_NO_CONTEXT, context_attribs);
  if (window->egl_context == EGL_NO_CONTEXT) {
    err_puts("window_egl_xcb_setup: eglCreateContext failed");
    return false;
  }
  if (!eglMakeCurrent(window->egl_display, window->egl_surface,
                      window->egl_surface, window->egl_context)) {
    err_puts("window_egl_xcb_setup: eglMakeCurrent failed");
    return false;
  }
  const char *gl_version = (const char*) glGetString(GL_VERSION);
  if (gl_version) {
    err_write_1("OpenGL Version: ");
    err_puts(gl_version);
  }
  eglQuerySurface(window->egl_display, window->egl_surface,
                  EGL_WIDTH, &gl_w);
  eglQuerySurface(window->egl_display, window->egl_surface,
                  EGL_HEIGHT, &gl_h);
  window->pixel_w = gl_w;
  window->pixel_h = gl_h;
  return true;
}

bool window_egl_xcb_event (s_window_egl *window,
                           xcb_connection_t *conn,
                           xcb_screen_t *screen,
                           xcb_window_t xcb_window,
                           xcb_intern_atom_reply_t *delete_reply,
                           xcb_generic_event_t *event,
                           struct xkb_state *xkb_state)
{
  xcb_button_press_event_t     *event_button;
  (void) conn;
  (void) screen;
  (void) xcb_window;
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
    eglSwapBuffers(window->egl_display, window->egl_surface);
    break;
  case XCB_CONFIGURE_NOTIFY:
    event_config = (xcb_configure_notify_event_t *) event;
    window->w = event_config->width;
    window->h = event_config->height;
    {
      EGLint gl_w, gl_h;
      eglQuerySurface(window->egl_display, window->egl_surface,
                      EGL_WIDTH, &gl_w);
      eglQuerySurface(window->egl_display, window->egl_surface,
                      EGL_HEIGHT, &gl_h);
      window->pixel_w = gl_w;
      window->pixel_h = gl_h;
    }
    if (! window->resize(window, window->pixel_w, window->pixel_h))
      goto ko;
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
      g_kc3_exit_code = 0;
      goto ko;
    }
    break;
  default:
    break;
  }
  return true;
 ko:
  return false;
}

bool window_egl_xcb_run (s_window_egl *window)
{
  xcb_connection_t *conn;
  xcb_screen_t *screen;
  xcb_window_t xcb_window;
  /* xcb_visualtype_t *visual; */
  xcb_generic_event_t *event;
  xcb_intern_atom_cookie_t protocols_cookie;
  xcb_intern_atom_reply_t *protocols_reply;
  xcb_intern_atom_cookie_t delete_cookie;
  xcb_intern_atom_reply_t *delete_reply;
  struct xkb_context *xkb_context;
  struct xkb_keymap *xkb_keymap;
  struct xkb_state *xkb_state;
  int32_t xkb_device_id;
  u32 value_mask;
  u32 value_list[2];
  conn = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(conn)) {
    err_puts("window_egl_xcb_run: xcb_connect");
    goto ko;
  }
  screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
  /* visual = xcb_screen_visual_type(screen); */
  xcb_window = xcb_generate_id(conn);
  value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  value_list[0] = screen->black_pixel;
  value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
                  XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_POINTER_MOTION |
                  XCB_EVENT_MASK_STRUCTURE_NOTIFY;
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, xcb_window,
                    screen->root, window->x, window->y, window->w,
                    window->h, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual, value_mask, value_list);
  protocols_cookie = xcb_intern_atom(conn, 1, 12, "WM_PROTOCOLS");
  delete_cookie = xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");
  protocols_reply = xcb_intern_atom_reply(conn, protocols_cookie, 0);
  delete_reply = xcb_intern_atom_reply(conn, delete_cookie, 0);
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xcb_window,
                      (*protocols_reply).atom, 4, 32, 1,
                      &(*delete_reply).atom);
  free(protocols_reply);
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xcb_window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                      strlen(window->title), window->title);

  if (!window_egl_xcb_setup(window, conn, screen, xcb_window)) {
    err_puts("window_egl_xcb_run: window_egl_xcb_setup failed");
    goto ko_conn;
  }
  xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (xkb_x11_setup_xkb_extension(conn, XKB_X11_MIN_MAJOR_XKB_VERSION,
                                  XKB_X11_MIN_MINOR_XKB_VERSION,
                                  0, NULL, NULL, NULL, NULL) != 1) {
    err_puts("window_egl_xcb_run: xkb_x11_setup_xkb_extension failed");
    goto ko_conn;
  }
  xkb_device_id = xkb_x11_get_core_keyboard_device_id(conn);
  if (xkb_device_id == -1) {
    err_puts("window_egl_xcb_run: xkb_x11_get_core_keyboard_device_id failed");
    goto ko_conn;
  }
  xkb_keymap = xkb_x11_keymap_new_from_device(xkb_context, conn,
                                              xkb_device_id,
                                              XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!xkb_keymap) {
    err_puts("window_egl_xcb_run: xkb_x11_keymap_new_from_device failed");
    goto ko_conn;
  }
  xkb_state = xkb_x11_state_new_from_device(xkb_keymap, conn, xkb_device_id);
  if (!xkb_state) {
    err_puts("window_egl_xcb_run: xkb_x11_state_new_from_device failed");
    goto ko_conn;
  }
  if (! window->load(window))
    goto ko_conn;
  xcb_map_window(conn, xcb_window);
  xcb_flush(conn);
  while (1) {
    while ((event = xcb_poll_for_event(conn))) {
      if (! window_egl_xcb_event(window, conn, screen, xcb_window,
                                 delete_reply, event, xkb_state)) {
        free(event);
        goto done;
      }
      free(event);
    }
    if (! window->render(window))
      break;
    eglSwapBuffers(window->egl_display, window->egl_surface);
    xcb_flush(conn);
  }
done:
  xkb_state_unref(xkb_state);
  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_context);
  free(delete_reply);
  xcb_disconnect(conn);
  return true;
 ko_conn:
  xcb_disconnect(conn);
 ko:
  return false;
}

xcb_visualtype_t * xcb_screen_visual_type (xcb_screen_t *screen)
{
  xcb_depth_iterator_t depth_iter;
  xcb_visualtype_iterator_t visual_iter;
  depth_iter = xcb_screen_allowed_depths_iterator(screen);
  for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
    visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
    for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
      if (screen->root_visual == visual_iter.data->visual_id)
        return visual_iter.data;
  }
  return NULL;
}
