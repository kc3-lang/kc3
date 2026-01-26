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
#include "../../../libkc3/kc3.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/looper.h>
#include "native_app_glue/android_native_app_glue.h"
#include "window_egl_android.h"

static u32  android_keycode_to_kc3 (int32_t android_keycode);
static bool window_egl_android_setup (s_window_egl_android *window,
                                      ANativeWindow *native_window);

static u32 android_keycode_to_kc3 (int32_t android_keycode)
{
  switch (android_keycode) {
  case AKEYCODE_BACK:       return 0xff1b; // Escape
  case AKEYCODE_ENTER:      return 0xff0d; // Return
  case AKEYCODE_SPACE:      return 0x0020; // Space
  case AKEYCODE_A:          return 0x0061; // 'a'
  case AKEYCODE_B:          return 0x0062; // 'b'
  case AKEYCODE_C:          return 0x0063; // 'c'
  case AKEYCODE_D:          return 0x0064; // 'd'
  case AKEYCODE_E:          return 0x0065; // 'e'
  case AKEYCODE_F:          return 0x0066; // 'f'
  case AKEYCODE_G:          return 0x0067; // 'g'
  case AKEYCODE_H:          return 0x0068; // 'h'
  case AKEYCODE_I:          return 0x0069; // 'i'
  case AKEYCODE_J:          return 0x006a; // 'j'
  case AKEYCODE_K:          return 0x006b; // 'k'
  case AKEYCODE_L:          return 0x006c; // 'l'
  case AKEYCODE_M:          return 0x006d; // 'm'
  case AKEYCODE_N:          return 0x006e; // 'n'
  case AKEYCODE_O:          return 0x006f; // 'o'
  case AKEYCODE_P:          return 0x0070; // 'p'
  case AKEYCODE_Q:          return 0x0071; // 'q'
  case AKEYCODE_R:          return 0x0072; // 'r'
  case AKEYCODE_S:          return 0x0073; // 's'
  case AKEYCODE_T:          return 0x0074; // 't'
  case AKEYCODE_U:          return 0x0075; // 'u'
  case AKEYCODE_V:          return 0x0076; // 'v'
  case AKEYCODE_W:          return 0x0077; // 'w'
  case AKEYCODE_X:          return 0x0078; // 'x'
  case AKEYCODE_Y:          return 0x0079; // 'y'
  case AKEYCODE_Z:          return 0x007a; // 'z'
  default:                  return android_keycode;
  }
}

void window_egl_android_handle_cmd (p_android_app app, int32_t cmd)
{
  s_window_egl_android *window = (s_window_egl_android *) app->userData;
  LOGI("handle_cmd: cmd=%d", cmd);
  switch (cmd) {
  case APP_CMD_INIT_WINDOW:
    LOGI("APP_CMD_INIT_WINDOW");
    if (app->window && window) {
      if (! window_egl_android_setup(window, app->window)) {
        LOGE("window_egl_android_setup failed");
        abort();
      }
      LOGI("Window setup complete");
    }
    return;
  case APP_CMD_TERM_WINDOW:
    if (window) {
      if (window->egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(window->egl_display, EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (window->egl_context != EGL_NO_CONTEXT) {
          eglDestroyContext(window->egl_display, window->egl_context);
        }
        if (window->egl_surface != EGL_NO_SURFACE) {
          eglDestroySurface(window->egl_display, window->egl_surface);
        }
        eglTerminate(window->egl_display);
      }
    }
    return;
  case APP_CMD_WINDOW_RESIZED:
    LOGI("APP_CMD_WINDOW_RESIZED");
    if (window) {
      int32_t w = ANativeWindow_getWidth(app->window);
      int32_t h = ANativeWindow_getHeight(app->window);
      LOGI("Resizing to %dx%d", w, h);
      window->w = w / 2;
      window->h = h / 2;
      window->pixel_w = w;
      window->pixel_h = h;
      if (window->resize && ! window->resize(window, window->w,
                                             window->h)) {
        LOGE("window->resize failed");
        abort();
      }
    }
    return;
  default:
    return;
  }
}

s32 window_egl_android_handle_input (p_android_app app,
                                     AInputEvent *event)
{
  static float touch_start_x = 0;
  static float touch_start_y = 0;
  static float pinch_start_dist = 0;
  s_window_egl_android *window = (s_window_egl_android *) app->userData;
  if (! window)
    return 0;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
    int32_t keycode = AKeyEvent_getKeyCode(event);
    int32_t action = AKeyEvent_getAction(event);
    if (action == AKEY_EVENT_ACTION_DOWN) {
      u32 kc3_keysym = android_keycode_to_kc3(keycode);
      if (window->key && ! window->key(window, kc3_keysym)) {
        return 1;
      }
    }
    return 1;
  }
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    int32_t action = AMotionEvent_getAction(event) &
      AMOTION_EVENT_ACTION_MASK;
    size_t pointer_count = AMotionEvent_getPointerCount(event);
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);
    int32_t screen_w = ANativeWindow_getWidth(app->window);
    int32_t edge_zone = screen_w / 10;
    if (pointer_count == 2) {
      float x0 = AMotionEvent_getX(event, 0);
      float y0 = AMotionEvent_getY(event, 0);
      float x1 = AMotionEvent_getX(event, 1);
      float y1 = AMotionEvent_getY(event, 1);
      float dx = x1 - x0;
      float dy = y1 - y0;
      float dist = sqrtf(dx * dx + dy * dy);
      if (action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
        pinch_start_dist = dist;
        LOGI("Pinch start: dist=%.1f", dist);
      }
      else if (action == AMOTION_EVENT_ACTION_MOVE) {
        float scale = dist / pinch_start_dist;
        if (scale > 1.1) {
          float cx = (x0 + x1) / 2;
          float cy = (y0 + y1) / 2;
          s64 logical_x = (s64) (cx / 2);
          s64 logical_y = window->h - (s64) (cy / 2);
          LOGI("Pinch zoom in at (%.1f, %.1f) -> (%lld, %lld)",
               cx, cy, (long long) logical_x, (long long) logical_y);
          if (window->button)
            window->button(window, 4, logical_x, logical_y);
          pinch_start_dist = dist;
        }
        else if (scale < 0.9) {
          float cx = (x0 + x1) / 2;
          float cy = (y0 + y1) / 2;
          s64 logical_x = (s64) (cx / 2);
          s64 logical_y = window->h - (s64) (cy / 2);
          LOGI("Pinch zoom out at (%.1f, %.1f) -> (%lld, %lld)",
               cx, cy, (long long) logical_x, (long long) logical_y);
          if (window->button &&
              ! window->button(window, 5, logical_x, logical_y)) {
            LOGI("window_egl_android_handle_input: window->button"
                 " -> false");
            exit(1);
          }
          pinch_start_dist = dist;
        }
      }
      return 1;
    }
    if (action == AMOTION_EVENT_ACTION_DOWN) {
      touch_start_x = x;
      touch_start_y = y;
      LOGI("Touch down: x=%.1f y=%.1f screen_w=%d edge_zone=%d",
           x, y, screen_w, edge_zone);
    }
    else if (action == AMOTION_EVENT_ACTION_UP) {
      float dx = x - touch_start_x;
      float dy = y - touch_start_y;
      float dist = sqrtf(dx * dx + dy * dy);
      bool is_edge_swipe = false;
      LOGI("Touch up: x=%.1f y=%.1f dx=%.1f dy=%.1f start_x=%.1f "
           "dist=%.1f", x, y, dx, dy, touch_start_x, dist);
      if (dist > 100 && touch_start_x < edge_zone && dx > 100 &&
          fabs(dx) > fabs(dy)) {
        LOGI("Left edge swipe right -> Left key");
        if (window->key)
          window->key(window, 0xff51);
        is_edge_swipe = true;
      }
      else if (dist > 100 && touch_start_x > screen_w - edge_zone &&
               dx < -100 && fabs(dx) > fabs(dy)) {
        LOGI("Right edge swipe left -> Right key");
        if (window->key)
          window->key(window, 0xff53);
        is_edge_swipe = true;
      }
      if (! is_edge_swipe) {
        u8 button = 1;
        s64 logical_x = (s64) (x / 2);
        s64 logical_y = window->h - (s64) (y / 2);
        if (window->button && ! window->button(window, button,
                                               logical_x, logical_y)) {
          LOGE("window_egl_android_handle_input: ! window->button");
          exit(1);
        }
      }
    }
  }
  return 0;
}

void window_egl_android_clean (s_window_egl_android *window)
{
  window_egl_clean((s_window_egl *) window);
}

s_window_egl_android *
window_egl_android_init (s_window_egl_android *window,
                         s64 x, s64 y, u64 w, u64 h,
                         const char *title,
                         u64 sequence_count)
{
  return (s_window_egl_android *)
    window_egl_init((s_window_egl *) window, x, y, w, h,
                    title, sequence_count);
}

bool window_egl_android_run (s_window_egl_android *window)
{
  int events;
  struct android_poll_source *source;
  assert(window);
  LOGI("window_egl_android_run starting");
  if (! window->app) {
    LOGE("no android_app");
    return false;
  }
  window->app->userData = window;
  window->app->onAppCmd = window_egl_android_handle_cmd;
  window->app->onInputEvent = window_egl_android_handle_input;
  LOGI("Entering event loop");
  while (1) {
    int timeout = (window->egl_display == EGL_NO_DISPLAY ||
                   window->egl_surface == EGL_NO_SURFACE) ? -1 : 0;
    if (ALooper_pollOnce(timeout, NULL, &events,
                         (void **) &source) >= 0) {
      if (source != NULL) {
        source->process(window->app, source);
      }
    }

    if (window->app->destroyRequested != 0) {
      if (window->unload)
        window->unload(window);
      return true;
    }

    if (window->egl_display != EGL_NO_DISPLAY &&
        window->egl_surface != EGL_NO_SURFACE) {
      if (window->render && ! window->render(window)) {
        break;
      }
      eglSwapBuffers(window->egl_display, window->egl_surface);
    }
  }
  if (window->unload)
    window->unload(window);
  return true;
}

static bool window_egl_android_setup (s_window_egl_android *window,
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
  LOGI("window_egl_android_setup starting");
  window->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (window->egl_display == EGL_NO_DISPLAY) {
    LOGE("eglGetDisplay failed");
    return false;
  }
  LOGI("eglGetDisplay ok");
  if (! eglInitialize(window->egl_display, &major, &minor)) {
    LOGE("eglInitialize failed");
    return false;
  }
  LOGI("EGL version: %d.%d", major, minor);
  if (! eglChooseConfig(window->egl_display, config_attribs, &config, 1,
                        &num_config)) {
    LOGE("eglChooseConfig failed");
    return false;
  }
  if (num_config != 1) {
    LOGE("no suitable EGL config");
    return false;
  }
  LOGI("eglChooseConfig ok");
  window->egl_surface = eglCreateWindowSurface(window->egl_display,
                                               config,
                                               (EGLNativeWindowType) native_window,
                                               NULL);
  if (window->egl_surface == EGL_NO_SURFACE) {
    LOGE("eglCreateWindowSurface failed");
    return false;
  }
  LOGI("eglCreateWindowSurface ok");
  window->egl_context = eglCreateContext(window->egl_display, config,
                                         EGL_NO_CONTEXT, context_attribs);
  if (window->egl_context == EGL_NO_CONTEXT) {
    LOGE("eglCreateContext failed");
    return false;
  }
  LOGI("eglCreateContext ok");
  if (!eglMakeCurrent(window->egl_display, window->egl_surface,
                      window->egl_surface, window->egl_context)) {
    LOGE("eglMakeCurrent failed");
    return false;
  }
  LOGI("eglMakeCurrent ok");
  {
    int32_t w = ANativeWindow_getWidth(native_window);
    int32_t h = ANativeWindow_getHeight(native_window);
    LOGI("Native window size: %dx%d", w, h);
    window->w = w / 2;
    window->h = h / 2;
    window->pixel_w = w;
    window->pixel_h = h;
  }
  if (window->load && ! window->load(window)) {
    LOGE("window->load failed");
    return false;
  }
  LOGI("window->load ok");
  if (window->resize &&
      ! window->resize(window, window->w, window->h)) {
    LOGE("window->resize failed");
    return false;
  }
  return true;
}

/*
bool window_egl_run (s_window_egl *window)
{
  return window_egl_android_run((s_window_egl_android *) window);
}
*/
