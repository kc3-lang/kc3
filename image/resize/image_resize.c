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

static int usage (const char *prog);

int main (int argc, char **argv)
{
  s_image dest;
  uw h = 0;
  s_str input = {0};
  s_str output = {0};
  s32 r = 1;
  s_image src;
  uw w = 0;
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  if (argc != 4)
    return usage(PROG);
  uw_init_1(&w, argv[0]);
  uw_init_1(&h, argv[1]);
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
    image_clean(&dest);
    goto clean;
  }
  if (! image_init_alloc(&dest, w, h, src.components,
                         src.pixel_size)) {
    err_puts(PROG ": image_init_alloc");
    goto clean;
  }
  if (! image_resize_to_fill(&src, &dest)) {
    err_puts(PROG ": image_resize_to_fill");
    image_clean(&src);
    image_clean(&dest);
    r = 1;
  }
  image_clean(&src);
  if (! image_to_png_file(&dest, &output)) {
    err_puts(PROG ": image_to_png_file");
    image_clean(&dest);
    goto clean;
  }
  image_clean(&dest);
  r = 0;
 clean:
  kc3_clean(NULL);
  return r;
}

int usage (const char *prog)
{
  fprintf(stderr, "Usage: %s WIDTH HEIGHT INPUT OUTPUT\n", prog);
  return 1;
}
