#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "test.h"

#if HAVE_LIBJPEG && HAVE_LIBPNG

#include <jpeglib.h>
#include <png.h>
#include "../image/types.h"

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
}

static s_image * image_test_resize (s_image *src, s_image *dest)
{
  (void) src;
  (void) dest;
  return NULL;
}

#define image_avir_resize_8 image_test_resize
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
  return 0;
}

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
#endif
}
