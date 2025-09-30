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
#include <android/native_window.h>
#include <android/looper.h>
#include <libkc3/kc3.h>
#include <xkbcommon/xkbcommon.h>
#include "window_egl_android.h"

bool window_egl_run (s_window_egl *window)
{
  return window_egl_android_run(window);
}

static bool window_egl_android_setup (s_window_egl *window,
                                      ANativeWindow *native_window)
{
  EGLint major, minor;
  EGLConfig config;
  EGLint    config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_DEPTH_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };
  EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };
  EGLint num_config;

  window->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (window->egl_display == EGL_NO_DISPLAY) {
    fprintf(stderr, "window_egl_android_setup: eglGetDisplay failed\n");
    return false;
  }

  if (!eglInitialize(window->egl_display, &major, &minor)) {
    fprintf(stderr, "window_egl_android_setup: eglInitialize failed\n");
    return false;
  }

  printf("EGL version: %d.%d\n", major, minor);

  if (!eglChooseConfig(window->egl_display, config_attribs, &config, 1,
                       &num_config)) {
    fprintf(stderr, "window_egl_android_setup: eglChooseConfig failed\n");
    return false;
  }

  if (num_config != 1) {
    fprintf(stderr, "window_egl_android_setup: no suitable EGL config found\n");
    return false;
  }

  window->egl_surface = eglCreateWindowSurface(window->egl_display, config,
                                               native_window, NULL);
  if (window->egl_surface == EGL_NO_SURFACE) {
    fprintf(stderr, "window_egl_android_setup: eglCreateWindowSurface failed\n");
    return false;
  }

  window->egl_context = eglCreateContext(window->egl_display, config,
                                         EGL_NO_CONTEXT, context_attribs);
  if (window->egl_context == EGL_NO_CONTEXT) {
    fprintf(stderr, "window_egl_android_setup: eglCreateContext failed\n");
    return false;
  }

  if (!eglMakeCurrent(window->egl_display, window->egl_surface,
                      window->egl_surface, window->egl_context)) {
    fprintf(stderr, "window_egl_android_setup: eglMakeCurrent failed\n");
    return false;
  }

  return true;
}

bool window_egl_android_run (s_window_egl *window)
{
  ANativeWindow *native_window;

  assert(window);

  native_window = window_egl_android_get_native_window(window);
  if (!native_window) {
    fprintf(stderr, "window_egl_android_run: failed to get native window\n");
    return false;
  }

  if (!window_egl_android_setup(window, native_window)) {
    fprintf(stderr, "window_egl_android_run: setup failed\n");
    return false;
  }

  // Main render loop would go here
  // For now, just return success

  return true;
}

ANativeWindow * window_egl_android_get_native_window (s_window_egl *window)
{
  // This would typically be passed from Java via JNI
  // For now, return NULL as placeholder
  (void) window;
  return NULL;
}