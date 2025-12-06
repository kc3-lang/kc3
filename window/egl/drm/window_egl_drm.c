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
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../../libkc3/kc3.h"
#include "../../../gl/gl.h"
#include "window_egl_drm.h"

/*
#ifndef EGL_PLATFORM_GBM_KHR
#define EGL_PLATFORM_GBM_KHR 0x31D7
#endif
*/

static void drm_context_clean (s_drm_context *ctx);
static bool drm_context_init (s_drm_context *ctx, s_window_egl *window);
static drmModeConnector * find_connector (int drm_fd,
                                          drmModeRes *resources);
static drmModeEncoder *   find_encoder (int drm_fd,
                                        drmModeConnector *connector);
static u32  keycode_to_keysym(u32 keycode);
static void libinput_close_restricted (int fd, void *user_data);
static int  libinput_open_restricted (const char *path, int flags,
                                      void *user_data);

static const struct libinput_interface libinput_interface = {
  .open_restricted  = libinput_open_restricted,
  .close_restricted = libinput_close_restricted,
};

static void drm_context_clean (s_drm_context *ctx)
{
  if (ctx->libinput)
    libinput_unref(ctx->libinput);
  if (ctx->previous_fb)
    drmModeRmFB(ctx->drm_fd, ctx->previous_fb);
  if (ctx->previous_bo)
    gbm_surface_release_buffer(ctx->gbm_surface, ctx->previous_bo);
  if (ctx->gbm_surface)
    gbm_surface_destroy(ctx->gbm_surface);
  if (ctx->gbm_device)
    gbm_device_destroy(ctx->gbm_device);
  if (ctx->crtc) {
    drmModeSetCrtc(ctx->drm_fd, ctx->crtc->crtc_id,
                   ctx->crtc->buffer_id, ctx->crtc->x, ctx->crtc->y,
                   &ctx->connector_id, 1, &ctx->crtc->mode);
    drmModeFreeCrtc(ctx->crtc);
  }
  if (ctx->connector)
    drmModeFreeConnector(ctx->connector);
  if (ctx->drm_fd >= 0)
    close(ctx->drm_fd);
}

static bool drm_context_init (s_drm_context *ctx, s_window_egl *window)
{
  drmModeRes *resources;
  drmModeEncoder *encoder;
  const char *drm_device = "/dev/dri/card0";
  ctx->drm_fd = open(drm_device, O_RDWR);
  if (ctx->drm_fd < 0) {
    err_write_1("drm_context_init: open: ");
    err_puts(drm_device);
    return false;
  }
  resources = drmModeGetResources(ctx->drm_fd);
  if (!resources) {
    err_puts("drm_context_init: drmModeGetResources");
    goto ko_fd;
  }
  ctx->connector = find_connector(ctx->drm_fd, resources);
  if (!ctx->connector) {
    err_puts("drm_context_init: no connected connector");
    goto ko_resources;
  }
  ctx->connector_id = ctx->connector->connector_id;
  drmModeObjectProperties *props;
  props = drmModeObjectGetProperties(ctx->drm_fd, ctx->connector_id,
                                     DRM_MODE_OBJECT_CONNECTOR);
  if (props) {
    u32 i;
    for (i = 0; i < props->count_props; i++) {
      drmModePropertyRes *prop = drmModeGetProperty(ctx->drm_fd,
                                                     props->props[i]);
      if (prop) {
        if (strcmp(prop->name, "max_bpc") == 0) {
          err_write_1("Connector max_bpc: ");
          err_inspect_u64_decimal(props->prop_values[i]);
          err_write_1("\n");
        }
        if (strcmp(prop->name, "HDR_OUTPUT_METADATA") == 0) {
          err_puts("Connector supports HDR_OUTPUT_METADATA");
        }
        drmModeFreeProperty(prop);
      }
    }
    drmModeFreeObjectProperties(props);
  }
  encoder = find_encoder(ctx->drm_fd, ctx->connector);
  if (!encoder) {
    err_puts("drm_context_init: no encoder");
    goto ko_connector;
  }
  ctx->crtc = drmModeGetCrtc(ctx->drm_fd, encoder->crtc_id);
  ctx->crtc_id = encoder->crtc_id;
  drmModeFreeEncoder(encoder);
  ctx->mode = &ctx->connector->modes[0];
  window->w = ctx->mode->hdisplay;
  window->h = ctx->mode->vdisplay;
  ctx->gbm_device = gbm_create_device(ctx->drm_fd);
  if (!ctx->gbm_device) {
    err_puts("drm_context_init: gbm_create_device");
    goto ko_crtc;
  }
  ctx->gbm_surface = gbm_surface_create(ctx->gbm_device,
                                        ctx->mode->hdisplay,
                                        ctx->mode->vdisplay,
                                        GBM_FORMAT_ARGB8888,
                                        GBM_BO_USE_SCANOUT |
                                        GBM_BO_USE_RENDERING);
  if (!ctx->gbm_surface) {
    err_puts("drm_context_init: gbm_surface_create");
    goto ko_gbm_device;
  }
  drmModeFreeResources(resources);
  ctx->previous_bo = NULL;
  ctx->previous_fb = 0;
  /* Initialize libinput */
  ctx->libinput = libinput_path_create_context(&libinput_interface,
                                                NULL);
  if (!ctx->libinput) {
    err_puts("drm_context_init: libinput_path_create_context");
    goto ko_gbm_surface;
  }
  ctx->libinput_fd = -1;
  /* Try to add keyboard device */
  if (libinput_path_add_device(ctx->libinput, "/dev/wskbd")) {
    ctx->libinput_fd = libinput_get_fd(ctx->libinput);
    err_puts("drm_context_init: keyboard initialized on /dev/wskbd");
  }
  else {
    err_puts("drm_context_init: libinput_path_add_device");
  }
  return true;
 ko_gbm_surface:
  gbm_surface_destroy(ctx->gbm_surface);
 ko_gbm_device:
  gbm_device_destroy(ctx->gbm_device);
 ko_crtc:
  drmModeFreeCrtc(ctx->crtc);
 ko_connector:
  drmModeFreeConnector(ctx->connector);
 ko_resources:
  drmModeFreeResources(resources);
 ko_fd:
  close(ctx->drm_fd);
  return false;
}

static bool drm_fb_from_bo (s_drm_context *ctx, struct gbm_bo *bo,
                            u32 *fb_id)
{
  u32 width, height, stride, handle;
  int ret;
  width = gbm_bo_get_width(bo);
  height = gbm_bo_get_height(bo);
  stride = gbm_bo_get_stride(bo);
  handle = gbm_bo_get_handle(bo).u32;
  ret = drmModeAddFB(ctx->drm_fd, width, height, 24, 32, stride,
                     handle, fb_id);
  if (ret) {
    err_puts("drm_fb_from_bo: drmModeAddFB failed");
    return false;
  }
  return true;
}

static bool drm_swap_buffers (s_drm_context *ctx)
{
  struct gbm_bo *bo;
  u32 fb_id;
  int ret;
  bo = gbm_surface_lock_front_buffer(ctx->gbm_surface);
  if (!bo) {
    err_puts("drm_swap_buffers: gbm_surface_lock_front_buffer failed");
    return false;
  }
  if (!drm_fb_from_bo(ctx, bo, &fb_id)) {
    gbm_surface_release_buffer(ctx->gbm_surface, bo);
    return false;
  }
  ret = drmModeSetCrtc(ctx->drm_fd, ctx->crtc_id, fb_id, 0, 0,
                       &ctx->connector_id, 1, ctx->mode);
  if (ret) {
    err_puts("drm_swap_buffers: drmModeSetCrtc failed");
    drmModeRmFB(ctx->drm_fd, fb_id);
    gbm_surface_release_buffer(ctx->gbm_surface, bo);
    return false;
  }
  if (ctx->previous_fb)
    drmModeRmFB(ctx->drm_fd, ctx->previous_fb);
  if (ctx->previous_bo)
    gbm_surface_release_buffer(ctx->gbm_surface, ctx->previous_bo);
  ctx->previous_bo = bo;
  ctx->previous_fb = fb_id;
  return true;
}

static drmModeConnector * find_connector (int drm_fd,
                                          drmModeRes *resources)
{
  drmModeConnector *connector;
  u32 i;
  for (i = 0; i < (u32) resources->count_connectors; i++) {
    connector = drmModeGetConnector(drm_fd, resources->connectors[i]);
    if (connector->connection == DRM_MODE_CONNECTED)
      return connector;
    drmModeFreeConnector(connector);
  }
  return NULL;
}

static drmModeEncoder * find_encoder (int drm_fd,
                                      drmModeConnector *connector)
{
  if (connector->encoder_id)
    return drmModeGetEncoder(drm_fd, connector->encoder_id);
  return NULL;
}

// Map Linux input keycodes to X11 keysyms
static u32 keycode_to_keysym(u32 keycode)
{
  // Common keycodes from linux/input-event-codes.h
  switch (keycode) {
  case 1:   return 0xff1b; // KEY_ESC -> XK_Escape
  case 16:  return 'q';    // KEY_Q
  case 105: return 0xff51; // KEY_LEFT -> XK_Left
  case 106: return 0xff53; // KEY_RIGHT -> XK_Right
  case 103: return 0xff52; // KEY_UP -> XK_Up
  case 108: return 0xff54; // KEY_Down -> XK_Down
  default:
    if (keycode >= 2 && keycode <= 11)
      return '0' + ((keycode + 8) % 10); // Number keys
    if (keycode >= 16 && keycode <= 25)
      return 'a' + (keycode - 16); // Q-P
    if (keycode >= 30 && keycode <= 38)
      return 'a' + (keycode - 30 + 10); // A-L
    if (keycode >= 44 && keycode <= 50)
      return 'a' + (keycode - 44 + 19); // Z-M
    return 0;
  }
}

static void libinput_close_restricted (int fd, void *user_data)
{
  (void) user_data;
  close(fd);
}

static int libinput_open_restricted (const char *path, int flags,
                                     void *user_data)
{
  int e;
  int fd = open(path, flags);
  (void) user_data;
  if (fd < 0) {
    e = errno;
    err_write_1("libinput_open_restricted: ");
    err_write_1(path);
    err_write_1(": ");
    err_puts(strerror(e));
  }
  return fd < 0 ? -errno : fd;
}

static bool window_egl_drm_setup (s_window_egl *window,
                                  s_drm_context *ctx)
{
  EGLint major, minor;
  EGLConfig config;
  EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
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
  window->egl_display = eglGetDisplay((EGLNativeDisplayType)
                                      ctx->gbm_device);
  if (window->egl_display == EGL_NO_DISPLAY) {
    err_puts("window_egl_drm_setup: eglGetDisplay");
    return false;
  }
  if (!eglInitialize(window->egl_display, &major, &minor)) {
    err_puts("window_egl_drm_setup: eglInitialize");
    return false;
  }
  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    EGLint error = eglGetError();
    err_write_1("window_egl_drm_setup: eglBindAPI(EGL_OPENGL_ES_API) failed: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return false;
  }
  if (!eglChooseConfig(window->egl_display, config_attribs, &config,
                       1, &num_configs)) {
    err_puts("window_egl_drm_setup: eglChooseConfig");
    return false;
  }
  if (num_configs == 0) {
    err_puts("window_egl_drm_setup: no matching EGL config found");
    return false;
  }
  EGLint visual_id, alpha_size;
  eglGetConfigAttrib(window->egl_display, config,
                     EGL_NATIVE_VISUAL_ID, &visual_id);
  eglGetConfigAttrib(window->egl_display, config,
                     EGL_ALPHA_SIZE, &alpha_size);
  err_write_1("EGL config visual ID: 0x");
  err_inspect_u32_hexadecimal((u32) visual_id);
  err_write_1(" alpha_size: ");
  err_inspect_s32_decimal((s32) alpha_size);
  err_write_1("\n");
  window->egl_config = config;
  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    EGLint error = eglGetError();
    err_write_1("window_egl_drm_setup: eglBindAPI(EGL_OPENGL_ES_API) failed: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return false;
  }
  window->egl_surface = eglCreateWindowSurface
    (window->egl_display, config,
     (EGLNativeWindowType) ctx->gbm_surface, NULL);
  if (window->egl_surface == EGL_NO_SURFACE) {
    EGLint error = eglGetError();
    err_write_1("window_egl_drm_setup: eglCreateWindowSurface failed: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return false;
  }
  window->egl_context = eglCreateContext(window->egl_display, config,
                                         EGL_NO_CONTEXT,
                                         context_attribs);
  if (window->egl_context == EGL_NO_CONTEXT) {
    err_puts("window_egl_drm_setup: eglCreateContext failed");
    return false;
  }
  if (!eglMakeCurrent(window->egl_display, window->egl_surface,
                      window->egl_surface, window->egl_context)) {
    err_puts("window_egl_drm_setup: eglMakeCurrent failed");
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

bool window_egl_run (s_window_egl *window)
{
  return window_egl_drm_run(window);
}

static bool drm_process_input (s_drm_context *ctx, s_window_egl *window)
{
  struct libinput_event *event;
  if (ctx->libinput_fd < 0)
    return true;
  libinput_dispatch(ctx->libinput);
  while ((event = libinput_get_event(ctx->libinput))) {
    enum libinput_event_type type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_KEYBOARD_KEY) {
      struct libinput_event_keyboard *kev =
        libinput_event_get_keyboard_event(event);
      enum libinput_key_state state =
        libinput_event_keyboard_get_key_state(kev);
      if (state == LIBINPUT_KEY_STATE_PRESSED) {
        u32 keycode = libinput_event_keyboard_get_key(kev);
        u32 keysym = keycode_to_keysym(keycode);
        if (keysym && window->key && !window->key(window, keysym)) {
          libinput_event_destroy(event);
          return false;
        }
      }
    }
    libinput_event_destroy(event);
  }
  return true;
}

bool window_egl_drm_run (s_window_egl *window)
{
  s_drm_context ctx;
  if (! drm_context_init(&ctx, window)) {
    err_puts("window_egl_drm_run: drm_context_init");
    return false;
  }
  if (! window_egl_drm_setup(window, &ctx)) {
    err_puts("window_egl_drm_run: window_egl_drm_setup");
    goto ko_drm;
  }
  if (!window->load(window))
    goto ko_drm;
  while (1) {
    if (!drm_process_input(&ctx, window))
      break;
    if (!window->render(window))
      break;
    eglSwapBuffers(window->egl_display, window->egl_surface);
    if (!drm_swap_buffers(&ctx))
      break;
  }
  drm_context_clean(&ctx);
  return true;
 ko_drm:
  drm_context_clean(&ctx);
  return false;
}
