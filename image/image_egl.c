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
#include "../libkc3/kc3.h"
#include "../gl/gl.h"
#include "image_egl.h"
#include "image.h"

void image_egl_clean (s_image_egl *image)
{
  assert(image);
  if (image->egl_context != EGL_NO_CONTEXT)
    eglDestroyContext(image->egl_display, image->egl_context);
  if (image->egl_surface != EGL_NO_SURFACE)
    eglDestroySurface(image->egl_display, image->egl_surface);
  if (image->egl_display != EGL_NO_DISPLAY)
    eglTerminate(image->egl_display);
  image_clean(&image->image);
}

void image_egl_delete (s_image_egl *image)
{
  assert(image);
  image_egl_clean(image);
  free(image);
}

s_image_egl * image_egl_init (s_image_egl *image, uw w, uw h)
{
  EGLConfig config;
  EGLint    config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
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
  EGLint major;
  EGLint minor;
  EGLint num_configs;
  EGLint pbuffer_attribs[] = {
    EGL_WIDTH, (EGLint) w,
    EGL_HEIGHT, (EGLint) h,
    EGL_NONE
  };
  s_image_egl tmp = {0};
  if (! image_init_alloc(&tmp.image, w, h, 4, 4)) {
    err_puts("image_egl_init: image_init_alloc");
    return NULL;
  }
  tmp.egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (tmp.egl_display == EGL_NO_DISPLAY) {
    err_puts("image_egl_init: eglGetDisplay");
    return NULL;
  }
  if (! eglInitialize(tmp.egl_display, &major, &minor)) {
    err_puts("image_egl_init: eglInitialize");
    return NULL;
  }
  if (false) {
    err_write_1("EGL Version: ");
    err_inspect_s32_decimal(major);
    err_write_1(".");
    err_inspect_s32_decimal(minor);
    err_write_1("\n");
  }
  if (! eglBindAPI(EGL_OPENGL_ES_API)) {
    EGLint error = eglGetError();
    err_write_1("image_egl_init: eglBindAPI: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return NULL;
  }
  if (! eglChooseConfig(tmp.egl_display, config_attribs,
                        &config, 1, &num_configs)) {
    err_puts("image_egl_init: eglChooseConfig");
    return NULL;
  }
  if (num_configs == 0) {
    err_puts("image_egl_init: no EGL config");
    return NULL;
  }
  tmp.egl_config = config;
  tmp.egl_surface = eglCreatePbufferSurface
    (tmp.egl_display, config, pbuffer_attribs);
  if (tmp.egl_surface == EGL_NO_SURFACE) {
    EGLint error = eglGetError();
    err_write_1("image_egl_init: "
                "eglCreatePbufferSurface: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return NULL;
  }
  tmp.egl_context = eglCreateContext(tmp.egl_display,
                                     config,
                                     EGL_NO_CONTEXT,
                                     context_attribs);
  if (tmp.egl_context == EGL_NO_CONTEXT) {
    err_puts("image_egl_init: eglCreateContext");
    return NULL;
  }
  image_egl_make_context_current(&tmp);
  *image = tmp;
  return image;
}

void image_egl_make_context_current (s_image_egl *image)
{
  if (! eglMakeCurrent(image->egl_display, image->egl_surface,
                       image->egl_surface, image->egl_context)) {
    EGLint error = eglGetError();
    err_write_1("image_egl_make_context_current: "
                "eglMakeCurrent: 0x");
    err_inspect_s32_hexadecimal(error);
    err_write_1("\n");
    assert(! "image_egl_make_context_current: eglMakeCurrent");
  }
}

s_image_egl * image_egl_new (uw w, uw h)
{
  s_image_egl *image = NULL;
  if (! (image = alloc(sizeof(s_image_egl))))
    return NULL;
  if (! image_egl_init(image, w, h)) {
    free(image);
    return NULL;
  }
  return image;
}

void image_egl_read (s_image_egl *image)
{
  GLenum format;
  assert(image);
  switch (image->image.components) {
  case 1: format = GL_LUMINANCE; break;
  case 2: format = GL_LUMINANCE_ALPHA; break;
  case 3: format = GL_RGB; break;
  case 4: format = GL_RGBA; break;
  default:
    err_write_1("image_egl_read: invalid components: ");
    err_inspect_u8_decimal(image->image.components);
    err_write_1("\n");
    return;
  }
  if (image->image.components != image->image.pixel_size) {
    err_write_1("image_egl_read: invalid pixel size: ");
    err_inspect_u8_decimal(image->image.pixel_size);
    err_write_1("\n");
    return;
  }
  glReadPixels(0, 0, image->image.w, image->image.h, format,
               GL_UNSIGNED_BYTE, image->image.data);
}

s_image_egl *
image_egl_resize_to_fill_file (s_image_egl *image, s_str *path)
{
  s_gl_sprite sprite;
  if (! gl_sprite_init(&sprite, path->ptr.pchar, 1, 1, 1, 1)) {
    ERROR("gl_sprite_init");
    return NULL;
  }
  if (! image_egl_resize_to_fill_sprite(image, &sprite)) {
    ERROR("image_egl_resize_to_fill_sprite");
    gl_sprite_clean(&sprite);
    return NULL;
  }
  gl_sprite_clean(&sprite);
  return image;
}

s_image_egl *
image_egl_resize_to_fill_sprite (s_image_egl *image,
                                 s_gl_sprite *sprite)
{
  s_gl_ortho ortho;
  GLuint texture;
  f32 img_aspect;
  f32 sprite_aspect;
  f32 x, y, w, h;
  assert(image);
  assert(sprite);
  if (! sprite->pix_w || ! sprite->pix_h) {
    ERROR("invalid sprite");
    return NULL;
  }
  texture = gl_sprite_texture(sprite, 0);
  img_aspect = (f32) image->image.w / (f32) image->image.h;
  sprite_aspect = (f32) sprite->pix_w / (f32) sprite->pix_h;
  if (img_aspect > sprite_aspect) {
    w = (f32) image->image.w;
    h = w / sprite_aspect;
    x = 0.0f;
    y = ((f32) image->image.h - h) / 2.0f;
  }
  else {
    h = (f32) image->image.h;
    w = h * sprite_aspect;
    y = 0.0f;
    x = ((f32) image->image.w - w) / 2.0f;
  }
  if (! gl_ortho_init(&ortho)) {
    ERROR("gl_ortho_init");
    return NULL;
  }
  glViewport(0, 0, image->image.w, image->image.h);
  gl_ortho_resize(&ortho, 0.0f, (f32) image->image.w,
                  0.0f, (f32) image->image.h, -1.0f, 1.0f);
  mat4_translate(&ortho.view_matrix, 0, image->image.h, 0);
  mat4_scale(&ortho.view_matrix, 1, -1, 1);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gl_ortho_render(&ortho);
  gl_ortho_bind_texture(&ortho, texture);
  gl_ortho_color(&ortho, 1.0f, 1.0f, 1.0f, 1.0f);
  gl_ortho_rect(&ortho, x, y, w, h);
  gl_ortho_render_end(&ortho);
  glFlush();
  gl_ortho_clean(&ortho);
  image_egl_read(image);
  return image;
}

void kc3_image_egl_delete (s_image_egl **image)
{
  assert(image);
  assert(*image);
  image_egl_clean(*image);
  free(*image);
}

uw * kc3_image_egl_height (s_image_egl **image, uw *dest)
{
  assert(image);
  assert(*image);
  assert(dest);
  *dest = (*image)->image.h;
  return dest;
}

void kc3_image_egl_make_context_current (s_image_egl **image)
{
  image_egl_make_context_current(*image);
}

s_marshall ** kc3_image_egl_marshall (s_marshall **m, bool heap,
                                      s_image_egl **image)
{
  assert(m);
  assert(*m);
  assert(image);
  assert(*image);
  if (! marshall_uw(*m, heap, (*image)->image.w) ||
      ! marshall_uw(*m, heap, (*image)->image.h)) {
    err_puts("kc3_image_egl_marshall: marshall_uw");
    return NULL;
  }
  return m;
}

s_image_egl ** kc3_image_egl_marshall_read (s_marshall_read **mr,
                                            bool heap,
                                            s_image_egl **dest)
{
  uw h = 0;
  s_image_egl *tmp = NULL;
  uw w = 0;
  assert(mr);
  assert(*mr);
  assert(dest);
  if (! marshall_read_uw(*mr, heap, &w) ||
      ! marshall_read_uw(*mr, heap, &h))
    return NULL;
  if (! (tmp = image_egl_new(w, h)))
    return NULL;
  *dest = tmp;
  return dest;
}

s_image_egl ** kc3_image_egl_new (s_image_egl **image,
                                  s_tag *w_tag, s_tag *h_tag)
{
  uw h = 0;
  const s_sym *sym_Uw = &g_sym_Uw;
  s_image_egl *tmp = NULL;
  uw w = 0;
  if (! uw_init_cast(&w, &sym_Uw, w_tag))
    return NULL;
  if (! uw_init_cast(&h, &sym_Uw, h_tag))
    return NULL;
  if (! (tmp = alloc(sizeof(s_image_egl))))
    return NULL;
  if (! image_egl_init(tmp, w, h)) {
    free(tmp);
    return NULL;
  }
  *image = tmp;
  return image;
}

s_image_egl ** kc3_image_egl_resize_to_fill_file (s_image_egl **image,
                                                  s_str *path)
{
  if (! image_egl_resize_to_fill_file(*image, path))
    ERROR("image_egl_resize_to_fill_file");
  return image;
}

s_image_egl ** kc3_image_egl_read (s_image_egl **image)
{
  image_egl_read(*image);
  return image;
}

s_image_egl ** kc3_image_egl_to_png_file (s_image_egl **image,
                                          s_str *path)
{
  if (! image_to_png_file(&(*image)->image, path)) {
    ERROR("image_to_png_file");
    return NULL;
  }
  return image;
}

uw * kc3_image_egl_width (s_image_egl **image, uw *dest)
{
  assert(image);
  assert(*image);
  assert(dest);
  *dest = (*image)->image.w;
  return dest;
}
