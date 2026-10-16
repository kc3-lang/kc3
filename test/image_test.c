#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "test.h"

#if HAVE_LIBJPEG && HAVE_LIBPNG

#include <jpeglib.h>
#include <jerror.h>
#include <png.h>
#include "../image/types.h"

#if HAVE_GLESV2
#define IMAGE_JPEG_ROUTES 4
#else
#define IMAGE_JPEG_ROUTES 2
#endif

static void *g_image_allocs[32];
static uw g_image_live;
static uw g_image_buffers;
static uw g_image_pngs;
static uw g_image_infos;
static uw g_image_alloc_calls;
static uw g_image_fail_alloc;
static uw g_image_write_calls;
static uw g_image_fail_write;
static bool g_image_short_write;
static bool g_image_fail_buffer;
static bool g_image_fail_png;
static bool g_image_fail_info;
static u8 g_image_fail_stage;
static bool g_image_bad_free;
static FILE *g_image_jpeg_file;
static uw g_image_files;
static uw g_image_jpegs;
static uw g_image_jpeg_rows;
static u8 g_image_jpeg_fail_stage;
static u8 g_image_jpeg_suspend;
static u8 g_image_jpeg_layout;

static void image_jpeg_raise (j_decompress_ptr info)
{
  info->err->msg_code = JERR_BAD_LENGTH;
  info->err->error_exit((j_common_ptr) info);
}

static void image_jpeg_create (j_decompress_ptr info, int version,
                               size_t size)
{
  if (g_image_jpeg_fail_stage == 1)
    image_jpeg_raise(info);
  jpeg_CreateDecompress(info, version, size);
  g_image_jpegs++;
  if (g_image_jpeg_fail_stage == 2)
    image_jpeg_raise(info);
}

static void image_jpeg_destroy (j_decompress_ptr info)
{
  if (info->mem)
    g_image_jpegs--;
  jpeg_destroy_decompress(info);
}

static int image_jpeg_header (j_decompress_ptr info, boolean required)
{
  if (g_image_jpeg_fail_stage == 3)
    image_jpeg_raise(info);
  if (g_image_jpeg_suspend == 1)
    return JPEG_SUSPENDED;
  return jpeg_read_header(info, required);
}

static boolean image_jpeg_start (j_decompress_ptr info)
{
  boolean result;
  if (g_image_jpeg_fail_stage == 4)
    image_jpeg_raise(info);
  if (g_image_jpeg_suspend == 2)
    return FALSE;
  result = jpeg_start_decompress(info);
  switch (g_image_jpeg_layout) {
  case 1: info->output_width = 0; break;
  case 2: info->output_height = 0; break;
  case 3: info->output_components = 0; break;
  case 4: info->output_components = 5; break;
  case 5:
    info->output_width = info->output_height = (JDIMENSION) -1;
    info->output_components = 4;
  }
  return result;
}

static JDIMENSION image_jpeg_scanlines (j_decompress_ptr info,
                                       JSAMPARRAY rows,
                                       JDIMENSION count)
{
  JDIMENSION result;
  if (g_image_jpeg_fail_stage == 5 ||
      (g_image_jpeg_fail_stage == 6 && info->output_scanline > 0))
    image_jpeg_raise(info);
  if (g_image_jpeg_suspend == 3)
    return 0;
  result = jpeg_read_scanlines(info, rows, count);
  g_image_jpeg_rows += result;
  return result;
}

static boolean image_jpeg_finish (j_decompress_ptr info)
{
  if (g_image_jpeg_fail_stage == 7)
    image_jpeg_raise(info);
  if (g_image_jpeg_suspend == 4)
    return FALSE;
  return jpeg_finish_decompress(info);
}

static void * image_alloc (uw size)
{
  void *data;
  uw i;
  g_image_alloc_calls++;
  if (g_image_alloc_calls == g_image_fail_alloc)
    return NULL;
  if (! (data = alloc(size)))
    return NULL;
  for (i = 0; i < sizeof(g_image_allocs) / sizeof(*g_image_allocs);
       i++)
    if (! g_image_allocs[i]) {
      g_image_allocs[i] = data;
      g_image_live++;
      return data;
    }
  alloc_free(data);
  return NULL;
}

static void image_free (void *data)
{
  uw i;
  if (! data)
    return;
  for (i = 0; i < sizeof(g_image_allocs) / sizeof(*g_image_allocs);
       i++)
    if (g_image_allocs[i] == data) {
      g_image_allocs[i] = NULL;
      g_image_live--;
      alloc_free(data);
      return;
    }
  g_image_bad_free = true;
}

static s_buf * image_buffer_new (uw size)
{
  s_buf *buf;
  char *data;
  if (g_image_fail_buffer || ! (data = image_alloc(size + 1)))
    return NULL;
  if (! (buf = buf_new(true, size, data))) {
    image_free(data);
    return NULL;
  }
  g_image_buffers++;
  return buf;
}

static void image_buffer_delete (s_buf *buf)
{
  if (buf->free) {
    image_free(buf->ptr.p_pvoid);
    buf->free = false;
  }
  g_image_buffers--;
  buf_delete(buf);
}

static sw image_buffer_write (s_buf *buf, const void *data, uw size)
{
  g_image_write_calls++;
  if (g_image_write_calls == g_image_fail_write) {
    if (g_image_short_write)
      return size ? buf_write(buf, data, size - 1) : -1;
    return -7;
  }
  return buf_write(buf, data, size);
}

static png_structp image_png_create (png_const_charp version,
                                     png_voidp error,
                                     png_error_ptr error_fn,
                                     png_error_ptr warning_fn)
{
  png_structp png;
  if (g_image_fail_png)
    return NULL;
  png = png_create_write_struct(version, error, error_fn, warning_fn);
  if (png)
    g_image_pngs++;
  return png;
}

static png_infop image_png_info (png_const_structrp png)
{
  png_infop info;
  if (g_image_fail_info)
    return NULL;
  info = png_create_info_struct(png);
  if (info)
    g_image_infos++;
  return info;
}

static void image_png_destroy (png_structpp png, png_infopp info)
{
  if (png && *png)
    g_image_pngs--;
  if (info && *info)
    g_image_infos--;
  png_destroy_write_struct(png, info);
}

static void image_png_write_info (png_structrp png, png_const_inforp info)
{
  if (g_image_fail_stage == 1)
    png_error(png, "test info failure");
  png_write_info(png, info);
}

static void image_png_write_row (png_structrp png, png_const_bytep row)
{
  if (g_image_fail_stage == 2)
    png_error(png, "test row failure");
  png_write_row(png, row);
}

static void image_png_write_end (png_structrp png, png_inforp info)
{
  if (g_image_fail_stage == 3)
    png_error(png, "test end failure");
  png_write_end(png, info);
}

static void image_reset (void)
{
  g_image_alloc_calls = 0;
  g_image_fail_alloc = 0;
  g_image_write_calls = 0;
  g_image_fail_write = 0;
  g_image_short_write = false;
  g_image_fail_buffer = false;
  g_image_fail_png = false;
  g_image_fail_info = false;
  g_image_fail_stage = 0;
  g_image_jpeg_rows = 0;
  g_image_jpeg_fail_stage = 0;
  g_image_jpeg_suspend = 0;
  g_image_jpeg_layout = 0;
}

static FILE * image_open (const char *path, const char *mode)
{
  int fd;
  FILE *fp;
  (void) path;
  if (! g_image_jpeg_file ||
      (fd = dup(fileno(g_image_jpeg_file))) < 0)
    return NULL;
  if (! (fp = fdopen(fd, mode))) {
    close(fd);
    return NULL;
  }
  rewind(fp);
  g_image_files++;
  return fp;
}

static int image_close (FILE *fp)
{
  g_image_files--;
  return fclose(fp);
}

static s_image * image_test_resize (s_image *src, s_image *dest)
{
  (void) src;
  (void) dest;
  return NULL;
}

#define image_avir_resize_8 image_test_resize
#define fopen image_open
#define fclose image_close
#define alloc image_alloc
#define alloc_free image_free
#define buf_new_alloc image_buffer_new
#define buf_delete image_buffer_delete
#define buf_write image_buffer_write
#define png_create_write_struct image_png_create
#define png_create_info_struct image_png_info
#define png_destroy_write_struct image_png_destroy
#define png_write_info image_png_write_info
#define png_write_row image_png_write_row
#define png_write_end image_png_write_end
#define jpeg_CreateDecompress image_jpeg_create
#define jpeg_destroy_decompress image_jpeg_destroy
#define jpeg_read_header image_jpeg_header
#define jpeg_start_decompress image_jpeg_start
#define jpeg_read_scanlines image_jpeg_scanlines
#define jpeg_finish_decompress image_jpeg_finish
#define image_clean image_test_clean
#define image_delete image_test_delete
#define image_init image_test_init
#define image_init_alloc image_test_init_alloc
#define image_init_file image_test_init_file
#define image_init_jpeg image_test_init_jpeg
#define image_init_png image_test_init_png
#define image_new_alloc image_test_new_alloc
#define image_resize_to_fill image_test_resize_to_fill
#define image_to_jpeg_file image_test_to_jpeg_file
#define image_to_png_file image_test_to_png_file
#define image_to_png_str image_test_to_png_str
#define kc3_image_delete kc3_image_test_delete
#define kc3_image_new_alloc kc3_image_test_new_alloc
#define kc3_image_new_resize_to_fill_file \
  kc3_image_test_new_resize_to_fill_file
#define kc3_image_to_png_file kc3_image_test_to_png_file
#define kc3_image_to_png_str kc3_image_test_to_png_str

#include "../image/image.c"

#undef alloc
#undef alloc_free
#undef buf_new_alloc
#undef buf_delete
#undef buf_write
#undef png_create_write_struct
#undef png_create_info_struct
#undef png_destroy_write_struct
#undef png_write_info
#undef png_write_row
#undef png_write_end
#undef fopen
#undef fclose
#undef jpeg_CreateDecompress
#undef jpeg_destroy_decompress
#undef jpeg_read_header
#undef jpeg_start_decompress
#undef jpeg_read_scanlines
#undef jpeg_finish_decompress

#if HAVE_GLESV2

#include "../gl/gles2.h"

static uw g_image_textures;
static uw g_image_uploads;
static uw g_image_strings;
static u8 g_image_texture[16 * 16 * 4];
static GLsizei g_image_texture_w;
static GLsizei g_image_texture_h;

static GLenum image_gl_error (void)
{
  return GL_NO_ERROR;
}

static const char * image_gl_error_string (s32 error)
{
  (void) error;
  return "test GL error";
}

static void image_gl_gen (GLsizei count, GLuint *textures)
{
  while (count-- > 0)
    *textures++ = ++g_image_textures;
}

static void image_gl_delete (GLsizei count, const GLuint *textures)
{
  while (count-- > 0)
    if (*textures++)
      g_image_textures--;
}

static void image_gl_bind (GLenum target, GLuint texture)
{
  (void) target;
  (void) texture;
}

static void image_gl_target (GLenum target)
{
  (void) target;
}

static void image_gl_parameter (GLenum target, GLenum name, GLint value)
{
  (void) target;
  (void) name;
  (void) value;
}

static void image_gl_upload (GLenum target, GLint level, GLint internal,
                             GLsizei w, GLsizei h, GLint border,
                             GLenum format, GLenum type, const void *data)
{
  (void) target;
  (void) level;
  (void) internal;
  (void) border;
  (void) format;
  (void) type;
  g_image_uploads++;
  g_image_texture_w = w;
  g_image_texture_h = h;
  if (w > 0 && h > 0 && (uw) w * h * 4 <= sizeof(g_image_texture))
    memcpy(g_image_texture, data, (uw) w * h * 4);
}

static s_str * image_path_copy (s_str *dest, const char *path)
{
  if (! str_init_copy_1(dest, path))
    return NULL;
  g_image_strings++;
  return dest;
}

static void image_path_clean (s_str *path)
{
  if (path->free.p_pvoid)
    g_image_strings--;
  str_clean(path);
}

static s_str * image_path_search (const s_str *path, const s_sym *mode,
                                  s_str *dest)
{
  (void) mode;
  return image_path_copy(dest, path->ptr.p_pchar);
}

#define alloc image_alloc
#define alloc_free image_free
#define fopen image_open
#define fclose image_close
#define str_init_copy_1 image_path_copy
#define str_clean image_path_clean
#define file_search image_path_search
#define glGetError image_gl_error
#define glGenTextures image_gl_gen
#define glDeleteTextures image_gl_delete
#define glBindTexture image_gl_bind
#define glActiveTexture image_gl_target
#define glGenerateMipmap image_gl_target
#define glTexParameteri image_gl_parameter
#define glTexImage2D image_gl_upload
#define gl_error_string image_gl_error_string
#define gl_sprite_clean image_sprite_clean
#define gl_sprite_init image_sprite_init
#define gl_sprite_init_jpeg image_sprite_init_jpeg
#define gl_sprite_init_png image_sprite_init_png
#define gl_sprite_texture image_sprite_texture

#include "../gl/gl_sprite.c"

#undef alloc
#undef alloc_free
#undef fopen
#undef fclose
#undef str_init_copy_1
#undef str_clean
#undef file_search

#endif

static int image_child (char (*test)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(10);
    _exit(test());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

static void image_pixels (u8 *data, uw size)
{
  uw i;
  u32 state = 1;
  for (i = 0; i < size; i++) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    data[i] = state;
  }
}

static char image_no_leaks (void)
{
  TEST_EQ(g_image_live, 0);
  TEST_EQ(g_image_buffers, 0);
  TEST_EQ(g_image_pngs, 0);
  TEST_EQ(g_image_infos, 0);
  TEST_EQ(g_image_bad_free, false);
  TEST_EQ(g_image_files, 0);
  TEST_EQ(g_image_jpegs, 0);
#if HAVE_GLESV2
  TEST_EQ(g_image_strings, 0);
  TEST_EQ(g_image_textures, 0);
#endif
  return 0;
}

static char image_jpeg_bad_file (void)
{
  const u8 data[] = {0xFF, 0xD8, 0xFF, 0xD9, 0, 0, 0, 0};
  TEST_ASSERT((g_image_jpeg_file = tmpfile()));
  TEST_EQ(fwrite(data, 1, sizeof(data), g_image_jpeg_file), sizeof(data));
  TEST_EQ(fflush(g_image_jpeg_file), 0);
  rewind(g_image_jpeg_file);
  return 0;
}

static char image_jpeg_fatal (void)
{
  u8 pixel = 0xA5;
  s_image image = {.w = 91, .h = 37, .components = 1,
                   .pixel_size = 1, .data = &pixel};
  const s_str path = STR("f19.jpeg");
  int fd;
  TEST_EQ(image_jpeg_bad_file(), 0);
  fd = fileno(g_image_jpeg_file);
  TEST_EQ(image_init_jpeg(&image, &path, g_image_jpeg_file), NULL);
  TEST_EQ(image.w, 91);
  TEST_EQ(image.h, 37);
  TEST_EQ(image.data, &pixel);
  TEST_EQ(pixel, 0xA5);
  TEST_ASSERT(fcntl(fd, F_GETFD) >= 0);
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

static char image_jpeg_dispatch (void)
{
  s_image image = {0};
  const s_str path = STR("f19.jpeg");
  TEST_EQ(image_jpeg_bad_file(), 0);
  TEST_EQ(image_init_file(&image, &path), NULL);
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

#if HAVE_GLESV2

static char image_jpeg_sprite (void)
{
  s_gl_sprite sprite = {0};
  TEST_EQ(image_jpeg_bad_file(), 0);
  TEST_EQ(gl_sprite_init_jpeg(&sprite, "f19.jpeg", 1, 1, 1, 1), NULL);
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(g_image_uploads, 0);
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

static char image_jpeg_sprite_dispatch (void)
{
  s_gl_sprite sprite = {0};
  TEST_EQ(image_jpeg_bad_file(), 0);
  TEST_EQ(gl_sprite_init(&sprite, "f19.jpeg", 1, 1, 1, 1), NULL);
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(g_image_uploads, 0);
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

#endif

static void image_jpeg_pixels (u8 *data, u8 components)
{
  uw x;
  uw y;
  for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++) {
      *data++ = 30 + y * 8;
      if (components >= 3) {
        *data++ = 70;
        *data++ = 190 - y * 6;
      }
      if (components == 4)
        *data++ = 0;
    }
}

static char image_jpeg_valid_file (u8 components, bool progressive)
{
  struct jpeg_compress_struct info = {0};
  struct jpeg_error_mgr error;
  u8 data[8 * 8 * 4];
  JSAMPROW row;
  int i;
  TEST_ASSERT((g_image_jpeg_file = tmpfile()));
  image_jpeg_pixels(data, components);
  info.err = jpeg_std_error(&error);
  jpeg_create_compress(&info);
  jpeg_stdio_dest(&info, g_image_jpeg_file);
  info.image_width = info.image_height = 8;
  info.input_components = components;
  info.in_color_space = components == 1 ? JCS_GRAYSCALE :
    components == 3 ? JCS_RGB : JCS_CMYK;
  jpeg_set_defaults(&info);
  jpeg_set_quality(&info, 100, TRUE);
  for (i = 0; i < info.num_components; i++)
    info.comp_info[i].h_samp_factor = info.comp_info[i].v_samp_factor = 1;
  if (progressive)
    jpeg_simple_progression(&info);
  jpeg_start_compress(&info, TRUE);
  while (info.next_scanline < info.image_height) {
    row = data + info.next_scanline * 8 * components;
    TEST_EQ(jpeg_write_scanlines(&info, &row, 1), 1);
  }
  jpeg_finish_compress(&info);
  jpeg_destroy_compress(&info);
  TEST_EQ(fflush(g_image_jpeg_file), 0);
  rewind(g_image_jpeg_file);
  return 0;
}

static char image_jpeg_failure (uw route)
{
  u8 pixel = 0xA5;
  s_image image = {.w = 91, .h = 37, .components = 1,
                   .pixel_size = 1, .data = &pixel};
  const s_str path = STR("f19.jpeg");
  int fd = fileno(g_image_jpeg_file);
  rewind(g_image_jpeg_file);
  if (route == 0)
    TEST_EQ(image_init_jpeg(&image, &path, g_image_jpeg_file), NULL);
  else if (route == 1)
    TEST_EQ(image_init_file(&image, &path), NULL);
#if HAVE_GLESV2
  else {
    s_gl_sprite sprite = {.pix_w = 91, .pix_h = 37};
    uw uploads = g_image_uploads;
    if (route == 2)
      TEST_EQ(gl_sprite_init_jpeg(&sprite, "f19.jpeg", 1, 1, 1, 1),
               NULL);
    else
      TEST_EQ(gl_sprite_init(&sprite, "f19.jpeg", 1, 1, 1, 1), NULL);
    TEST_EQ(sprite.pix_w, 91);
    TEST_EQ(sprite.pix_h, 37);
    TEST_EQ(sprite.texture, NULL);
    TEST_EQ(g_image_uploads, uploads);
  }
#endif
  TEST_EQ(image.w, 91);
  TEST_EQ(image.h, 37);
  TEST_EQ(image.components, 1);
  TEST_EQ(image.pixel_size, 1);
  TEST_EQ(image.data, &pixel);
  TEST_EQ(pixel, 0xA5);
  TEST_ASSERT(fcntl(fd, F_GETFD) >= 0);
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

static char image_jpeg_valid (void)
{
  s_image image;
  const s_str path = STR("f19.jpeg");
  const u8 components[] = {1, 3, 4};
  u8 pixels[8 * 8 * 4];
  uw i;
  uw j;
  uw progressive;
  for (i = 0; i < sizeof(components); i++)
    for (progressive = 0; progressive < 2; progressive++) {
      image_reset();
      TEST_EQ(image_jpeg_valid_file(components[i], progressive), 0);
      image_jpeg_pixels(pixels, components[i]);
      TEST_EQ(image_init_file(&image, &path), &image);
      TEST_EQ(image.w, 8);
      TEST_EQ(image.h, 8);
      TEST_EQ(image.components, components[i]);
      TEST_EQ(image.pixel_size, components[i]);
      for (j = 0; j < 8 * 8 * components[i]; j++)
        TEST_ASSERT(abs((int) image.data[j] - pixels[j]) <= 3);
      image_clean(&image);
      TEST_EQ(fclose(g_image_jpeg_file), 0);
      g_image_jpeg_file = NULL;
      TEST_EQ(image_no_leaks(), 0);
    }
  return 0;
}

static char image_jpeg_errors (void)
{
  uw route;
  u8 stage;
  TEST_EQ(image_jpeg_valid_file(3, false), 0);
  for (route = 0; route < IMAGE_JPEG_ROUTES; route++)
    for (stage = 1; stage <= 7; stage++) {
      image_reset();
      g_image_jpeg_fail_stage = stage;
      TEST_EQ(image_jpeg_failure(route), 0);
      if (stage == 6 || stage == 7)
        TEST_ASSERT(g_image_jpeg_rows > 0);
    }
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(image_jpeg_valid(), 0);
  return 0;
}

static char image_jpeg_alloc_failure (void)
{
  uw route;
  uw failure;
  TEST_EQ(image_jpeg_valid_file(3, false), 0);
  for (route = 0; route < IMAGE_JPEG_ROUTES; route++)
    for (failure = 1; failure <= 2; failure++) {
      image_reset();
      g_image_fail_alloc = failure;
      TEST_EQ(image_jpeg_failure(route), 0);
      TEST_EQ(g_image_alloc_calls, failure);
    }
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(image_jpeg_valid(), 0);
  return 0;
}

static char image_jpeg_no_progress (void)
{
  uw route;
  u8 stage;
  TEST_EQ(image_jpeg_valid_file(3, false), 0);
  for (route = 0; route < IMAGE_JPEG_ROUTES; route++)
    for (stage = 1; stage <= 4; stage++) {
      image_reset();
      g_image_jpeg_suspend = stage;
      TEST_EQ(image_jpeg_failure(route), 0);
    }
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  return 0;
}

static char image_jpeg_limits (void)
{
  uw route;
  u8 layout;
  TEST_EQ(image_jpeg_valid_file(3, false), 0);
  for (route = 0; route < IMAGE_JPEG_ROUTES; route++)
    for (layout = 1; layout <= 5; layout++) {
      image_reset();
      g_image_jpeg_layout = layout;
      TEST_EQ(image_jpeg_failure(route), 0);
      TEST_EQ(g_image_alloc_calls, 1);
    }
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  return 0;
}

static char image_jpeg_late_error (void)
{
  uw route;
  const u8 soi[] = {0xFF, 0xD8};
  TEST_EQ(image_jpeg_valid_file(3, false), 0);
  TEST_EQ(fseek(g_image_jpeg_file, -2, SEEK_END), 0);
  TEST_EQ(fwrite(soi, 1, sizeof(soi), g_image_jpeg_file), sizeof(soi));
  TEST_EQ(fflush(g_image_jpeg_file), 0);
  for (route = 0; route < IMAGE_JPEG_ROUTES; route++) {
    image_reset();
    TEST_EQ(image_jpeg_failure(route), 0);
    TEST_EQ(g_image_alloc_calls, 2);
    TEST_EQ(g_image_jpeg_rows, 8);
  }
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  return 0;
}

static char image_jpeg_warning (void)
{
  s_image image;
  const s_str path = STR("f19.jpeg");
  long length;
  image_reset();
  TEST_EQ(image_jpeg_valid_file(3, false), 0);
  TEST_EQ(fseek(g_image_jpeg_file, 0, SEEK_END), 0);
  TEST_ASSERT((length = ftell(g_image_jpeg_file)) > 2);
  TEST_EQ(ftruncate(fileno(g_image_jpeg_file), length - 2), 0);
  rewind(g_image_jpeg_file);
  TEST_EQ(image_init_jpeg(&image, &path, g_image_jpeg_file), &image);
  TEST_EQ(image.w, 8);
  TEST_EQ(image.h, 8);
  image_clean(&image);
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

#if HAVE_GLESV2

static char image_jpeg_sprite_valid (void)
{
  s_gl_sprite sprite;
  s_image image;
  const s_str path = STR("f19.jpeg");
  u8 pixels[8 * 8 * 3];
  uw x;
  uw y;
  image_reset();
  TEST_EQ(image_jpeg_valid_file(3, true), 0);
  TEST_EQ(image_init_file(&image, &path), &image);
  TEST_EQ(image.components, 3);
  memcpy(pixels, image.data, sizeof(pixels));
  image_clean(&image);
  TEST_EQ(gl_sprite_init(&sprite, "f19.jpeg", 1, 1, 1, 2), &sprite);
  TEST_EQ(sprite.total_w, 8);
  TEST_EQ(sprite.total_h, 8);
  TEST_EQ(sprite.pix_w, 8);
  TEST_EQ(sprite.pix_h, 8);
  TEST_EQ(sprite.pt_w, 16);
  TEST_EQ(sprite.pt_h, 16);
  TEST_EQ(sprite.frame_count, 1);
  TEST_EQ(g_image_uploads, 1);
  TEST_EQ(g_image_texture_w, 8);
  TEST_EQ(g_image_texture_h, 8);
  for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++) {
      TEST_MEM_EQ(g_image_texture + (y * 8 + x) * 4, 3,
                   pixels + ((7 - y) * 8 + x) * 3, 3);
      TEST_EQ(g_image_texture[(y * 8 + x) * 4 + 3], 255);
    }
  gl_sprite_clean(&sprite);
  TEST_EQ(fclose(g_image_jpeg_file), 0);
  g_image_jpeg_file = NULL;
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

#endif

static char image_png_roundtrip (uw w, uw h, u8 components)
{
  u8 data[128 * 128 * 4];
  u8 decoded[sizeof(data)];
  png_image png = {0};
  s_image src = {.w = w, .h = h, .components = components,
                .pixel_size = components, .data = data};
  const png_uint_32 formats[] = {PNG_FORMAT_GRAY, PNG_FORMAT_GA,
                                PNG_FORMAT_RGB, PNG_FORMAT_RGBA};
  s_str out;
  uw size = w * h * components;
  TEST_ASSERT(size <= sizeof(data));
  image_reset();
  image_pixels(data, size);
  TEST_EQ(image_to_png_str(&src, &out), &out);
  TEST_ASSERT(out.size > size);
  png.version = PNG_IMAGE_VERSION;
  TEST_ASSERT(png_image_begin_read_from_memory(&png, out.ptr.p_pvoid,
                                               out.size));
  TEST_EQ(png.width, w);
  TEST_EQ(png.height, h);
  png.format = formats[components - 1];
  TEST_ASSERT(png_image_finish_read(&png, NULL, decoded, 0, NULL));
  TEST_MEM_EQ(decoded, size, data, size);
  png_image_free(&png);
  image_free(out.free.p_pvoid);
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

static char image_png_small (void)
{
  TEST_EQ(image_png_roundtrip(2, 2, 4), 0);
  return 0;
}

static char image_png_formats (void)
{
  u8 components;
  for (components = 1; components <= 4; components++) {
    TEST_EQ(image_png_roundtrip(1, 1, components), 0);
    TEST_EQ(image_png_roundtrip(7, 3, components), 0);
    TEST_EQ(image_png_roundtrip(32, 32, components), 0);
    TEST_EQ(image_png_roundtrip(128, 128, components), 0);
  }
  return 0;
}

static char image_png_failure (void)
{
  u8 data[128 * 128 * 4];
  s_image src = {.w = 128, .h = 128, .components = 4,
                .pixel_size = 4, .data = data};
  s_str out = STR("unchanged");
  const s_str before = out;
  image_pixels(data, sizeof(data));
  TEST_EQ(image_to_png_str(&src, &out), NULL);
  TEST_EQ(out.free.p_pvoid, before.free.p_pvoid);
  TEST_EQ(out.ptr.p_pvoid, before.ptr.p_pvoid);
  TEST_EQ(out.size, before.size);
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

static char image_png_alloc_failure (void)
{
  uw allocations;
  uw i;
  TEST_EQ(image_png_roundtrip(128, 128, 4), 0);
  allocations = g_image_alloc_calls;
  TEST_ASSERT(allocations > 2);
  for (i = 1; i <= allocations; i++) {
    image_reset();
    g_image_fail_alloc = i;
    TEST_EQ(image_png_failure(), 0);
    TEST_EQ(g_image_alloc_calls, i);
  }
  image_reset();
  g_image_fail_buffer = true;
  TEST_EQ(image_png_failure(), 0);
  image_reset();
  g_image_fail_png = true;
  TEST_EQ(image_png_failure(), 0);
  image_reset();
  g_image_fail_info = true;
  TEST_EQ(image_png_failure(), 0);
  return 0;
}

static char image_png_write_failure (void)
{
  uw i;
  uw j;
  uw writes;
  TEST_EQ(image_png_roundtrip(128, 128, 4), 0);
  writes = g_image_write_calls;
  TEST_ASSERT(writes > 3);
  for (i = 1; i <= writes; i++)
    for (j = 0; j < 2; j++) {
      image_reset();
      g_image_fail_write = i;
      g_image_short_write = j != 0;
      TEST_EQ(image_png_failure(), 0);
      TEST_EQ(g_image_write_calls, i);
    }
  return 0;
}

static char image_png_library_failure (void)
{
  u8 stage;
  for (stage = 1; stage <= 3; stage++) {
    image_reset();
    g_image_fail_stage = stage;
    TEST_EQ(image_png_failure(), 0);
  }
  return 0;
}

static char image_png_invalid (void)
{
  u8 data[4] = {0};
  s_image src;
  s_str out;
  const s_str before = STR("unchanged");
  uw i;
  for (i = 0; i < 12; i++) {
    image_reset();
    src = (s_image) {.w = 1, .h = 1, .components = 4,
                     .pixel_size = 4, .data = data};
    out = before;
    switch (i) {
    case 0: src.w = 0; break;
    case 1: src.h = 0; break;
    case 2: src.components = 0; break;
    case 3: src.components = 5; break;
    case 4: src.pixel_size = 0; break;
    case 5: src.pixel_size = 3; break;
    case 6: src.pixel_size = 8; break;
    case 7: src.data = NULL; break;
    case 8: src.w = (uw) PNG_UINT_31_MAX + 1; break;
    case 9: src.h = (uw) PNG_UINT_31_MAX + 1; break;
    case 10: src.w = UW_MAX; break;
    case 11: src.w = src.h = PNG_UINT_31_MAX; break;
    }
    TEST_EQ(image_to_png_str(&src, &out), NULL);
    TEST_EQ(out.free.p_pvoid, before.free.p_pvoid);
    TEST_EQ(out.ptr.p_pvoid, before.ptr.p_pvoid);
    TEST_EQ(out.size, before.size);
    TEST_EQ(g_image_alloc_calls, 0);
    TEST_EQ(image_no_leaks(), 0);
  }
  TEST_EQ(image_to_png_str(NULL, &out), NULL);
  TEST_EQ(image_to_png_str(&src, NULL), NULL);
  return 0;
}

static char image_png_output_limit (void)
{
  s_buf *out;
  png_structp png;
  u8 data = 0;
  image_reset();
  TEST_ASSERT((out = image_buffer_new(16)));
  TEST_ASSERT((png = image_png_create(PNG_LIBPNG_VER_STRING, NULL,
                                      NULL, NULL)));
  png_set_write_fn(png, out, image_png_write_callback, NULL);
  if (! setjmp(png_jmpbuf(png))) {
    image_png_write_callback(png, &data, (png_size_t) SW_MAX + 1);
    TEST_ASSERT(false);
  }
  out->wpos = out->size = SW_MAX;
  if (! setjmp(png_jmpbuf(png))) {
    image_png_write_callback(png, &data, 1);
    TEST_ASSERT(false);
  }
  out->wpos = 0;
  out->size = 16;
  image_buffer_delete(out);
  image_png_destroy(&png, NULL);
  TEST_EQ(image_no_leaks(), 0);
  return 0;
}

#define IMAGE_CASE(name)                       \
  TEST_CASE(image_ ## name)                    \
  {                                           \
    TEST_EQ(image_child(image_ ## name), 0);    \
  }                                           \
  TEST_CASE_END(image_ ## name)

IMAGE_CASE(png_small)
IMAGE_CASE(png_formats)
IMAGE_CASE(png_alloc_failure)
IMAGE_CASE(png_write_failure)
IMAGE_CASE(png_library_failure)
IMAGE_CASE(png_invalid)
IMAGE_CASE(png_output_limit)
IMAGE_CASE(jpeg_fatal)
IMAGE_CASE(jpeg_dispatch)
IMAGE_CASE(jpeg_valid)
IMAGE_CASE(jpeg_errors)
IMAGE_CASE(jpeg_alloc_failure)
IMAGE_CASE(jpeg_no_progress)
IMAGE_CASE(jpeg_limits)
IMAGE_CASE(jpeg_late_error)
IMAGE_CASE(jpeg_warning)
#if HAVE_GLESV2
IMAGE_CASE(jpeg_sprite)
IMAGE_CASE(jpeg_sprite_dispatch)
IMAGE_CASE(jpeg_sprite_valid)
#endif

#endif

void image_test (void)
{
#if HAVE_LIBJPEG && HAVE_LIBPNG
  TEST_CASE_RUN(image_png_small);
  TEST_CASE_RUN(image_png_formats);
  TEST_CASE_RUN(image_png_alloc_failure);
  TEST_CASE_RUN(image_png_write_failure);
  TEST_CASE_RUN(image_png_library_failure);
  TEST_CASE_RUN(image_png_invalid);
  TEST_CASE_RUN(image_png_output_limit);
  TEST_CASE_RUN(image_jpeg_fatal);
  TEST_CASE_RUN(image_jpeg_dispatch);
  TEST_CASE_RUN(image_jpeg_valid);
  TEST_CASE_RUN(image_jpeg_errors);
  TEST_CASE_RUN(image_jpeg_alloc_failure);
  TEST_CASE_RUN(image_jpeg_no_progress);
  TEST_CASE_RUN(image_jpeg_limits);
  TEST_CASE_RUN(image_jpeg_late_error);
  TEST_CASE_RUN(image_jpeg_warning);
#if HAVE_GLESV2
  TEST_CASE_RUN(image_jpeg_sprite);
  TEST_CASE_RUN(image_jpeg_sprite_dispatch);
  TEST_CASE_RUN(image_jpeg_sprite_valid);
#endif
#endif
}
