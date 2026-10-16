#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "test.h"

#if HAVE_GLESV2 && HAVE_FREETYPE2

#include "../gl/freetype2.h"
#include "../gl/gles2.h"
#include "../gl/types.h"

static FT_FaceRec g_text_face;
static FT_SizeRec g_text_size;
static FT_GlyphSlotRec g_text_glyph;
static FT_GlyphSlotRec g_text_glyphs[128];
static u8 g_text_bitmaps[128][64];
static bool g_text_real_font;
static FT_Pos g_text_kerning;
static bool g_text_fail_kerning;
static uw g_text_loads;
static uw g_text_fail_load;
static bool g_text_change_bitmap;
static bool g_text_fail_alloc;
static bool g_text_fail_str_copy;
static bool g_text_fail_texture;
static uw g_text_textures;
static uw g_text_alloc_size;
static u8 *g_text_alloc;
static bool g_text_guard_error;
static GLint g_text_max_size;
static GLenum g_text_gl_error;
static bool g_text_fail_upload;
static bool g_text_fail_mipmap;
static GLuint g_text_bound;
static uw g_text_uploads;
static GLsizei g_text_width;
static GLsizei g_text_height;
static u8 g_text_pixels[1024 * 1024];

static GLenum text_get_error (void)
{
  GLenum result = g_text_gl_error;
  g_text_gl_error = GL_NO_ERROR;
  return result;
}

static void text_get_integer (GLenum name, GLint *value)
{
  (void) name;
  *value = g_text_max_size;
}

static void text_gen_textures (GLsizei n, GLuint *textures)
{
  while (n-- > 0) {
    if (g_text_fail_texture) {
      *textures++ = 0;
      g_text_gl_error = GL_OUT_OF_MEMORY;
    }
    else {
      *textures++ = 1;
      g_text_textures++;
    }
  }
}

static void text_delete_textures (GLsizei n, const GLuint *textures)
{
  while (n-- > 0)
    if (*textures++) {
      if (! g_text_textures)
        g_text_guard_error = true;
      else
        g_text_textures--;
    }
}

static s_str * text_str_copy (s_str *dest, const s_str *src)
{
  if (g_text_fail_str_copy)
    return NULL;
  return str_init_copy(dest, src);
}

static void text_bind_texture (GLenum target, GLuint texture)
{
  (void) target;
  g_text_bound = texture;
}

static void text_tex_image (GLenum target, GLint level, GLint format,
                            GLsizei width, GLsizei height, GLint border,
                            GLenum pixel_format, GLenum type,
                            const void *pixels)
{
  u64 size;
  (void) target;
  (void) level;
  (void) format;
  (void) border;
  (void) pixel_format;
  (void) type;
  if (g_text_fail_upload) {
    g_text_gl_error = GL_OUT_OF_MEMORY;
    return;
  }
  size = (u64) width * height * 4;
  if (width <= 0 || height <= 0 || size > sizeof(g_text_pixels) ||
      size != g_text_alloc_size || ! pixels) {
    g_text_guard_error = true;
    return;
  }
  g_text_width = width;
  g_text_height = height;
  g_text_uploads++;
  memcpy(g_text_pixels, pixels, size);
}

static void text_generate_mipmap (GLenum target)
{
  (void) target;
  if (g_text_fail_mipmap)
    g_text_gl_error = GL_INVALID_OPERATION;
}

static void * text_alloc (uw size)
{
  g_text_alloc_size = size;
  if (g_text_fail_alloc || ! size || size > sizeof(g_text_pixels))
    return NULL;
  if (! (g_text_alloc = alloc(size + 128)))
    return NULL;
  memset(g_text_alloc, 0xA5, size + 128);
  memset(g_text_alloc + 64, 0, size);
  return g_text_alloc + 64;
}

static void text_free (void *data)
{
  uw i;
  if (! data)
    return;
  if (data != g_text_alloc + 64) {
    g_text_guard_error = true;
    return;
  }
  for (i = 0; i < 64; i++)
    if (g_text_alloc[i] != 0xA5 ||
        g_text_alloc[64 + g_text_alloc_size + i] != 0xA5)
      g_text_guard_error = true;
  alloc_free(g_text_alloc);
  g_text_alloc = NULL;
}

static FT_UInt text_char_index (FT_Face face, FT_ULong c)
{
  if (g_text_real_font)
    return FT_Get_Char_Index(face, c);
  return c < 128 ? c : '?';
}

static FT_Error text_load_glyph (FT_Face face, FT_UInt index,
                                FT_Int32 flags)
{
  g_text_loads++;
  if (g_text_loads == g_text_fail_load)
    return 1;
  if (g_text_real_font)
    return FT_Load_Glyph(face, index, flags);
  g_text_glyph = g_text_glyphs[index];
  if (g_text_change_bitmap && g_text_loads > 1)
    g_text_glyph.bitmap_left += 20;
  face->glyph = &g_text_glyph;
  return 0;
}

static FT_Error text_get_kerning (FT_Face face, FT_UInt left,
                                 FT_UInt right, FT_UInt mode,
                                 FT_Vector *delta)
{
  if (g_text_fail_kerning)
    return 1;
  if (g_text_real_font)
    return FT_Get_Kerning(face, left, right, mode, delta);
  delta->x = g_text_kerning;
  delta->y = 0;
  return 0;
}

static void text_reset (void)
{
  uw i;
  memset(&g_text_face, 0, sizeof(g_text_face));
  memset(&g_text_size, 0, sizeof(g_text_size));
  memset(g_text_glyphs, 0, sizeof(g_text_glyphs));
  memset(g_text_pixels, 0, sizeof(g_text_pixels));
  memset(g_text_bitmaps, 64, sizeof(g_text_bitmaps));
  g_text_face.size = &g_text_size;
  g_text_face.ascender = 6 * 64;
  g_text_face.descender = -2 * 64;
  g_text_size.metrics.y_scale = 65536;
  g_text_size.metrics.ascender = 6 * 64;
  g_text_size.metrics.descender = -2 * 64;
  g_text_size.metrics.height = 8 * 64;
  for (i = 0; i < 128; i++) {
    g_text_glyphs[i].advance.x = 4 * 64;
    g_text_glyphs[i].metrics.horiAdvance = 4 * 64;
    g_text_glyphs[i].bitmap_top = 6;
    g_text_glyphs[i].bitmap.width = 4;
    g_text_glyphs[i].bitmap.rows = 2;
    g_text_glyphs[i].bitmap.pitch = 4;
    g_text_glyphs[i].bitmap.buffer = g_text_bitmaps[i];
    g_text_glyphs[i].bitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
    g_text_glyphs[i].bitmap.num_grays = 256;
  }
  g_text_glyphs[' '].bitmap.width = 0;
  g_text_glyphs[' '].bitmap.rows = 0;
  g_text_glyphs[' '].bitmap.buffer = NULL;
  g_text_real_font = false;
  g_text_kerning = 0;
  g_text_fail_kerning = false;
  g_text_loads = 0;
  g_text_fail_load = 0;
  g_text_change_bitmap = false;
  g_text_fail_alloc = false;
  g_text_fail_str_copy = false;
  g_text_fail_texture = false;
  g_text_textures = 0;
  g_text_alloc_size = 0;
  g_text_alloc = NULL;
  g_text_guard_error = false;
  g_text_max_size = 4096;
  g_text_gl_error = GL_NO_ERROR;
  g_text_fail_upload = false;
  g_text_fail_mipmap = false;
  g_text_bound = 0;
  g_text_uploads = 0;
  g_text_width = 0;
  g_text_height = 0;
  (void) text_get_integer;
  (void) text_get_error;
}

#define FT_Get_Char_Index text_char_index
#define FT_Load_Glyph text_load_glyph
#define FT_Get_Kerning text_get_kerning
#define glGetError text_get_error
#define glGetIntegerv text_get_integer
#define glGenTextures text_gen_textures
#define glDeleteTextures text_delete_textures
#define glBindTexture text_bind_texture
#define glTexImage2D text_tex_image
#define glGenerateMipmap text_generate_mipmap
#define alloc text_alloc
#define alloc_free text_free
#define str_init_copy text_str_copy
#define gl_text_clean text_clean_gl
#define gl_text_init text_init_gl
#define gl_text_init_1 text_init_1_gl
#define gl_text_init_str text_init_str_gl
#define gl_text_render_to_texture text_render_gl
#define gl_text_set_font text_set_font_gl
#define gl_text_set_text text_set_text_gl
#define gl_text_set_text_1 text_set_text_1_gl
#define gl_text_set_text_buf text_set_text_buf_gl
#define gl_text_update text_update_gl
#define gl_text_update_1 text_update_1_gl
#define gl_text_update_buf text_update_buf_gl

#include "../gl/gl_text.c"

static void text_fixture (s_gl_text *text, s_gl_font *font,
                           const char *str)
{
  *font = (s_gl_font) {.ft_face = &g_text_face, .point_per_pixel = 1};
  *text = (s_gl_text) {.font = font, .texture = 1};
  str_init_1(&text->str, NULL, str);
}

static u8 text_alpha (uw x, uw y)
{
  return g_text_pixels[((g_text_height - 1 - y) * g_text_width + x) * 4
                        + 3];
}

static int text_child (char (*test)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(30);
    _exit(test());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

static char text_overhang (void)
{
  s_gl_font font;
  s_gl_text text;
  text_reset();
  text_fixture(&text, &font, "a");
  g_text_glyphs['a'].bitmap.width = 8;
  g_text_glyphs['a'].bitmap.pitch = 8;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(g_text_guard_error, false);
  TEST_EQ(text.pix_w, 8);
  TEST_EQ(text.pix_h, 8);
  TEST_EQ(text_alpha(7, 0), 64);
  TEST_EQ(g_text_alloc, NULL);
  return 0;
}

static char text_zero_advance (void)
{
  s_gl_font font;
  s_gl_text text;
  text_reset();
  text_fixture(&text, &font, "a");
  g_text_glyphs['a'].advance.x = 0;
  g_text_glyphs['a'].metrics.horiAdvance = 0;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(g_text_guard_error, false);
  TEST_EQ(text.pix_w, 4);
  TEST_EQ(text_alpha(3, 0), 64);
  return 0;
}

static char text_bearings (void)
{
  s_gl_font font;
  s_gl_text text;
  text_reset();
  text_fixture(&text, &font, "ab");
  g_text_glyphs['a'].bitmap_left = -3;
  g_text_glyphs['a'].bitmap_top = 9;
  g_text_glyphs['b'].bitmap_top = -3;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(g_text_guard_error, false);
  TEST_EQ(text.pix_w, 11);
  TEST_EQ(text.pix_h, 14);
  TEST_EQ(text_alpha(0, 0), 64);
  TEST_EQ(text_alpha(7, 12), 64);
  return 0;
}

static char text_pitch (void)
{
  s_gl_font font;
  s_gl_text text;
  int sign;
  for (sign = -1; sign <= 1; sign += 2) {
    text_reset();
    text_fixture(&text, &font, "a");
    g_text_glyphs['a'].bitmap.pitch = sign * 6;
    memset(g_text_bitmaps['a'], 17, 4);
    memset(g_text_bitmaps['a'] + 4, 255, 2);
    memset(g_text_bitmaps['a'] + 6, 91, 4);
    TEST_ASSERT(gl_text_render_to_texture(&text));
    TEST_EQ(g_text_guard_error, false);
    TEST_EQ(text_alpha(0, 0), sign > 0 ? 17 : 91);
    TEST_EQ(text_alpha(3, 1), sign > 0 ? 91 : 17);
  }
  return 0;
}

static char text_kerning_overlap (void)
{
  s_gl_font font;
  s_gl_text text;
  text_reset();
  text_fixture(&text, &font, "ab");
  g_text_kerning = -6 * 64;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text.pix_w, 6);
  TEST_EQ(text_alpha(0, 0), 64);
  TEST_EQ(text_alpha(2, 0), 112);
  memset(g_text_bitmaps['b'], 0, sizeof(g_text_bitmaps['b']));
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text_alpha(2, 0), 64);
  TEST_EQ(g_text_guard_error, false);
  return 0;
}

static char text_multiline_empty (void)
{
  s_gl_font font;
  s_gl_text text;
  uw i;
  text_reset();
  text_fixture(&text, &font, "a\nb");
  g_text_kerning = -6 * 64;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text.pix_w, 4);
  TEST_EQ(text.pix_h, 16);
  TEST_EQ(text_alpha(0, 0), 64);
  TEST_EQ(text_alpha(0, 8), 64);
  str_init_1(&text.str, NULL, " ");
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text.pix_w, 4);
  TEST_EQ(text.pix_h, 8);
  for (i = 3; i < g_text_alloc_size; i += 4)
    TEST_EQ(g_text_pixels[i], 0);
  str_init_1(&text.str, NULL, "\n");
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text.pix_w, 0);
  TEST_EQ(text.pix_h, 16);
  TEST_EQ(g_text_width, 1);
  str_init_1(&text.str, NULL, "");
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text.pix_w, 0);
  TEST_EQ(text.pix_h, 0);
  TEST_EQ(g_text_width, 1);
  TEST_EQ(g_text_height, 1);
  TEST_EQ(g_text_pixels[3], 0);
  TEST_EQ(g_text_guard_error, false);
  return 0;
}

static char text_bitmap_modes (void)
{
  s_gl_font font;
  s_gl_text text;
  text_reset();
  text_fixture(&text, &font, "a");
  g_text_glyphs['a'].bitmap.width = 9;
  g_text_glyphs['a'].bitmap.pitch = 2;
  g_text_glyphs['a'].bitmap.pixel_mode = FT_PIXEL_MODE_MONO;
  g_text_bitmaps['a'][0] = 0x81;
  g_text_bitmaps['a'][1] = 0x80;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text.pix_w, 9);
  TEST_EQ(text_alpha(0, 0), 255);
  TEST_EQ(text_alpha(1, 0), 0);
  TEST_EQ(text_alpha(7, 0), 255);
  TEST_EQ(text_alpha(8, 0), 255);
  text_reset();
  text_fixture(&text, &font, "a");
  g_text_glyphs['a'].bitmap.num_grays = 4;
  g_text_bitmaps['a'][0] = 1;
  g_text_bitmaps['a'][1] = 2;
  g_text_bitmaps['a'][2] = 3;
  g_text_bitmaps['a'][3] = 255;
  TEST_ASSERT(gl_text_render_to_texture(&text));
  TEST_EQ(text_alpha(0, 0), 85);
  TEST_EQ(text_alpha(1, 0), 170);
  TEST_EQ(text_alpha(2, 0), 255);
  TEST_EQ(text_alpha(3, 0), 255);
  TEST_EQ(g_text_guard_error, false);
  return 0;
}

static char text_invalid_bitmap (void)
{
  uw i;
  s_gl_font font;
  s_gl_text text;
  for (i = 0; i < 7; i++) {
    text_reset();
    text_fixture(&text, &font, "a");
    switch (i) {
    case 0: g_text_glyphs['a'].bitmap.buffer = NULL; break;
    case 1: g_text_glyphs['a'].bitmap.pitch = 3; break;
    case 2: g_text_glyphs['a'].bitmap.pitch = -3; break;
    case 3: g_text_glyphs['a'].bitmap.num_grays = 1; break;
    case 4: g_text_glyphs['a'].bitmap.num_grays = 257; break;
    case 5: g_text_glyphs['a'].bitmap.pixel_mode = FT_PIXEL_MODE_LCD;
      break;
    case 6:
      g_text_glyphs['a'].bitmap.rows = U32_MAX;
      g_text_glyphs['a'].bitmap.pitch = INT_MIN;
      break;
    }
    TEST_ASSERT(! gl_text_render_to_texture(&text));
    TEST_EQ(g_text_uploads, 0);
    TEST_EQ(g_text_alloc_size, 0);
    TEST_EQ(g_text_guard_error, false);
  }
  return 0;
}

static char text_limits (void)
{
  uw i;
  s64 metric = (s64) S32_MAX * 64;
  s_gl_font font;
  s_gl_text text;
  if (metric > LONG_MAX)
    metric = LONG_MAX;
  for (i = 0; i < 6; i++) {
    text_reset();
    text_fixture(&text, &font, "a");
    switch (i) {
    case 0: g_text_max_size = 3; break;
    case 1: g_text_max_size = 0; break;
    case 2:
      g_text_glyphs['a'].bitmap_left = INT_MAX;
      break;
    case 3:
      g_text_size.metrics.ascender = metric;
      g_text_size.metrics.descender = 0;
      str_init_1(&text.str, NULL, " ");
      g_text_glyphs[' '].advance.x = metric;
      g_text_max_size = S32_MAX;
      break;
    case 4: font.ft_face = NULL; break;
    case 5: g_text_face.size = NULL; break;
    }
    TEST_ASSERT(! gl_text_render_to_texture(&text));
    TEST_EQ(g_text_uploads, 0);
    TEST_EQ(g_text_alloc_size, 0);
  }
  return 0;
}

static char text_failure_cleanup (void)
{
  uw i;
  s_gl_font font;
  s_gl_text text;
  for (i = 0; i < 7; i++) {
    text_reset();
    text_fixture(&text, &font, "a");
    text.pix_w = 37;
    text.pix_h = 41;
    switch (i) {
    case 0: g_text_fail_alloc = true; break;
    case 1: g_text_fail_load = 1; break;
    case 2: g_text_fail_load = 2; break;
    case 3: g_text_change_bitmap = true; break;
    case 4:
      str_init_1(&text.str, NULL, "ab");
      g_text_fail_kerning = true;
      break;
    case 5: str_init_1(&text.str, NULL, "\xC3"); break;
    case 6: str_init_1(&text.str, NULL, "\xFF"); break;
    }
    TEST_ASSERT(! gl_text_render_to_texture(&text));
    TEST_EQ(g_text_uploads, 0);
    TEST_EQ(g_text_alloc, NULL);
    TEST_EQ(g_text_guard_error, false);
    TEST_EQ(text.pix_w, 37);
    TEST_EQ(text.pix_h, 41);
  }
  return 0;
}

static char text_gl_failures (void)
{
  uw i;
  s_gl_font font;
  s_gl_text text;
  for (i = 0; i < 2; i++) {
    text_reset();
    text_fixture(&text, &font, "a");
    text.pix_w = 37;
    text.pix_h = 41;
    g_text_fail_upload = i == 0;
    g_text_fail_mipmap = i == 1;
    TEST_ASSERT(! gl_text_render_to_texture(&text));
    TEST_EQ(g_text_alloc, NULL);
    TEST_EQ(g_text_guard_error, false);
    TEST_EQ(g_text_bound, 0);
    TEST_EQ(text.pix_w, 37);
    TEST_EQ(text.pix_h, 41);
  }
  return 0;
}

static char text_update_failure (void)
{
  s_buf buf;
  s_gl_font font;
  s_gl_text text;
  text_reset();
  text_fixture(&text, &font, "");
  g_text_fail_alloc = true;
  TEST_ASSERT(! gl_text_update_1(&text, "a"));
  buf_init_1_const(&buf, "b");
  TEST_ASSERT(! gl_text_update_buf(&text, &buf));
  str_clean(&text.str);
  buf_clean(&buf);
  TEST_EQ(g_text_uploads, 0);
  TEST_EQ(g_text_alloc, NULL);
  return 0;
}

static char text_real_fonts (void)
{
  const char *paths[] = {
    "../fonts/CourierPrime/fonts/ttf/CourierPrime-Regular.ttf",
    "../fonts/CourierPrime/fonts/ttf/CourierPrime-Italic.ttf"
  };
  const char *strings[] = {"AV fi j", "Àg_ é\nj", " ", ""};
  FT_Face face;
  FT_Library library;
  uw i;
  uw j;
  uw size;
  s_gl_font font;
  s_gl_text text;
  TEST_EQ(FT_Init_FreeType(&library), 0);
  for (i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
    TEST_EQ(FT_New_Face(library, paths[i], 0, &face), 0);
    for (size = 12; size <= 48; size *= 2) {
      TEST_EQ(FT_Set_Pixel_Sizes(face, 0, size), 0);
      for (j = 0; j < sizeof(strings) / sizeof(strings[0]); j++) {
        text_reset();
        text_fixture(&text, &font, strings[j]);
        g_text_real_font = true;
        font.ft_face = face;
        TEST_ASSERT(gl_text_render_to_texture(&text));
        TEST_EQ(g_text_guard_error, false);
        TEST_EQ(g_text_alloc, NULL);
        TEST_EQ(g_text_uploads, 1);
      }
    }
    TEST_EQ(FT_Done_Face(face), 0);
  }
  TEST_EQ(FT_Done_FreeType(library), 0);
  return 0;
}

static char text_string_failures (void)
{
  s_gl_font font;
  s_gl_text saved;
  s_gl_text text;
  s_str str;
  text_reset();
  text_fixture(&text, &font, "");
  TEST_ASSERT(str_init_copy_1(&text.str, "old"));
  saved = text;
  str_init_1(&str, NULL, "new");
  g_text_fail_str_copy = true;
  TEST_ASSERT(! gl_text_set_text(&text, &str));
  TEST_EQ(memcmp(&text, &saved, sizeof(text)), 0);
  TEST_EQ(memcmp(text.str.ptr.p_pchar, "old", 3), 0);
  str_clean(&text.str);
  text_fixture(&text, &font, "old");
  saved = text;
  TEST_EQ(gl_text_init_str(&text, &font, &str), NULL);
  TEST_EQ(memcmp(&text, &saved, sizeof(text)), 0);
  TEST_EQ(g_text_textures, 0);
  g_text_fail_str_copy = false;
  g_text_fail_texture = true;
  TEST_EQ(gl_text_init(&text, &font), NULL);
  TEST_EQ(memcmp(&text, &saved, sizeof(text)), 0);
  TEST_EQ(g_text_textures, 0);
  TEST_EQ(g_text_gl_error, GL_NO_ERROR);
  g_text_fail_texture = false;
  TEST_EQ(gl_text_init_str(&text, &font, &str), &text);
  TEST_EQ(g_text_textures, 1);
  gl_text_clean(&text);
  TEST_EQ(g_text_textures, 0);
  TEST_EQ(g_text_guard_error, false);
  return 0;
}

#define TEXT_CASE(name)                       \
  TEST_CASE(gl_text_ ## name)                 \
  {                                          \
    TEST_EQ(text_child(text_ ## name), 0);    \
  }                                          \
  TEST_CASE_END(gl_text_ ## name)

TEXT_CASE(overhang)
TEXT_CASE(zero_advance)
TEXT_CASE(bearings)
TEXT_CASE(pitch)
TEXT_CASE(kerning_overlap)
TEXT_CASE(multiline_empty)
TEXT_CASE(bitmap_modes)
TEXT_CASE(invalid_bitmap)
TEXT_CASE(limits)
TEXT_CASE(failure_cleanup)
TEXT_CASE(gl_failures)
TEXT_CASE(update_failure)
TEXT_CASE(real_fonts)
TEXT_CASE(string_failures)

#endif

void gl_text_test (void);

void gl_text_test (void)
{
#if HAVE_GLESV2 && HAVE_FREETYPE2
  TEST_CASE_RUN(gl_text_overhang);
  TEST_CASE_RUN(gl_text_zero_advance);
  TEST_CASE_RUN(gl_text_bearings);
  TEST_CASE_RUN(gl_text_pitch);
  TEST_CASE_RUN(gl_text_kerning_overlap);
  TEST_CASE_RUN(gl_text_multiline_empty);
  TEST_CASE_RUN(gl_text_bitmap_modes);
  TEST_CASE_RUN(gl_text_invalid_bitmap);
  TEST_CASE_RUN(gl_text_limits);
  TEST_CASE_RUN(gl_text_failure_cleanup);
  TEST_CASE_RUN(gl_text_gl_failures);
  TEST_CASE_RUN(gl_text_update_failure);
  TEST_CASE_RUN(gl_text_real_fonts);
  TEST_CASE_RUN(gl_text_string_failures);
#endif
}
