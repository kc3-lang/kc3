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
/** @file types.h
 *  @brief Module KC3.Window.EGL.DRM
 *
 *  Types for DRM/GBM backend for EGL windows.
 */
#ifndef LIBKC3_WINDOW_EGL_DRM_TYPES_H
#define LIBKC3_WINDOW_EGL_DRM_TYPES_H

#include <config.h>
#include <gbm.h>
#include <libinput.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "../../../libkc3/types.h"
#include "../types.h"

typedef struct drm_context s_drm_context;

struct drm_context {
  int drm_fd;
  struct gbm_device *gbm_device;
  struct gbm_surface *gbm_surface;
  drmModeModeInfo *mode;
  drmModeConnector *connector;
  drmModeCrtc *crtc;
  u32 connector_id;
  u32 crtc_id;
  struct gbm_bo *previous_bo;
  u32 previous_fb;
  struct libinput *libinput;
  int libinput_fd;
};

#endif /* LIBKC3_WINDOW_EGL_DRM_TYPES_H */
