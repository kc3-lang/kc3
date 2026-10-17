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
#include "../../libkc3/kc3.h"
#include <stdio.h>
#include "../image.h"
#include "../image_avir.h"

static bool dimension (const char *arg, uw *dest);
static int  usage (const char *prog);

int main (int argc, char **argv)
{
  s_image dest = {0};
  uw h = 0;
  s_str input = {0};
  s_str output = {0};
  s32 r = 1;
  s_image src = {0};
  uw w = 0;
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  if (argc != 4) {
    usage(PROG);
    goto clean;
  }
  if (! dimension(argv[0], &w) || ! dimension(argv[1], &h)) {
    err_puts(PROG ": invalid dimensions");
    goto clean;
  }
  str_init_1(&input, NULL, argv[2]);
  str_init_1(&output, NULL, argv[3]);
  if (! file_access(&input, &g_sym_r)) {
    err_write_1(PROG ": cannot open file for reading: ");
    err_write_str(&input);
    err_write_1("\n");
    goto clean;
  }
  if (! image_init_file(&src, &input)) {
    err_puts(PROG ": image_init_file");
    goto clean;
  }
  if (! src.pixel_size || w > (uw) SW_MAX / src.pixel_size ||
      h > (uw) SW_MAX / (w * src.pixel_size)) {
    err_puts(PROG ": image size overflow");
    goto clean;
  }
  if (! image_init_alloc(&dest, w, h, src.components,
                         src.pixel_size)) {
    err_puts(PROG ": image_init_alloc");
    goto clean;
  }
  if (! image_resize_to_fill(&src, &dest)) {
    err_puts(PROG ": image_resize_to_fill");
    goto clean;
  }
  if (! image_to_png_file(&dest, &output)) {
    err_puts(PROG ": image_to_png_file");
    goto clean;
  }
  r = 0;
 clean:
  image_clean(&dest);
  image_clean(&src);
  kc3_clean(NULL);
  return r;
}

static bool dimension (const char *arg, uw *dest)
{
  s_buf buf;
  bool valid;
  uw value = 0;
  buf_init_1_const(&buf, arg);
  valid = buf_parse_uw(&buf, &value) > 0 && buf.rpos == buf.wpos &&
    value > 0 && value <= S32_MAX;
  buf_clean(&buf);
  if (! valid)
    return false;
  *dest = value;
  return true;
}

int usage (const char *prog)
{
  fprintf(stderr, "Usage: %s WIDTH HEIGHT INPUT OUTPUT\n", prog);
  return 1;
}
