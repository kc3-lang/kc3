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
#ifndef LIBKC3_WINDOW_EGL_ANDROID_H
#define LIBKC3_WINDOW_EGL_ANDROID_H

#include <android/native_window.h>
#include <android/input.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include "../window_egl.h"

typedef struct android_app *p_android_app;

bool window_egl_android_run (s_window_egl *window);

/* Android input handling */
int32_t window_egl_android_handle_input (p_android_app app,
                                         AInputEvent *event);
void window_egl_android_handle_cmd (p_android_app app, int32_t cmd);

#endif /* LIBKC3_WINDOW_EGL_ANDROID_H */
