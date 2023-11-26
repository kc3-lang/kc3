/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <math.h>
#include <libc3/c3.h>
#include "../window_cairo.h"
#include "flies.h"

#define BOARD_SIZE 25

static const character g_block =  0x2588;
static const character g_fly   = 0x1FAB0;
static const character g_space =     ' ';

bool flies_load (s_sequence *seq,
                 s_window_cairo *window)
{
  uw address[2];
  s_array *board;
  uw i;
  uw j;
  s_map *map;
  (void) window;
  tag_map(&seq->tag, 1);
  map = &seq->tag.data.map;
  tag_init_sym_1( map->keys  + 0, "board");
  tag_init_array(map->values + 0, sym_1("Character"),
                 2, (uw[]) {BOARD_SIZE, BOARD_SIZE});
  board = &map->values[0].data.array;
  i = 0;
  while (i < BOARD_SIZE) {
    address[0] = i;
    j = 0;
    while (j < BOARD_SIZE) {
      address[1] = j;
      array_data_set(board, address, &g_space);
      j++;
    }
    i++;
  }
  i = 0;
  while (i < BOARD_SIZE) {
    address[0] = i;
    address[1] = 0;
    array_data_set(board, address, &g_block);
    address[1] = BOARD_SIZE - 1;
    array_data_set(board, address, &g_block);
    address[0] = 0;
    address[1] = i;
    array_data_set(board, address, &g_block);
    address[0] = BOARD_SIZE - 1;
    array_data_set(board, address, &g_block);
    i++;
  }
  address[0] = BOARD_SIZE / 2;
  address[1] = 0;
  array_data_set(board, address, &g_space);
  address[1] = BOARD_SIZE - 1;
  array_data_set(board, address, &g_space);
  address[1] = BOARD_SIZE / 2;
  i = 1;
  while (i < BOARD_SIZE * 3 / 4) {
    address[0] = i;
    array_data_set(board, address, &g_block);
    i++;
  }
  address[0] = BOARD_SIZE * 3 / 4 - 1;
  j = BOARD_SIZE / 4;
  while (j < BOARD_SIZE / 2 - 1) {
    address[1] = j;
    array_data_set(board, address, &g_block);
    j++;
  }
  address[1] = BOARD_SIZE * 3 / 4;
  i = BOARD_SIZE / 4;
  while (i < BOARD_SIZE - 1) {
    address[0] = i;
    array_data_set(board, address, &g_block);
    i++;
  }
  return true;
}

bool flies_render (s_sequence *seq, s_window_cairo *window,
                   cairo_t *cr)
{
  uw address[2];
  s_array *board;
  s_buf buf;
  character *c;
  uw i;
  s8 line[BOARD_SIZE * 4 + 1];
  s_map *map;
  cairo_text_extents_t te;
  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  cairo_set_font_size(cr, window->h / BOARD_SIZE);
  cairo_select_font_face(cr, "Courier New",
                         CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  i = 0;
  while (i < BOARD_SIZE) {
    line[i] = ' ';
    i++;
  }
  line[i] = 0;
  cairo_text_extents(cr, line, &te);
  /* io_inspect(&seq->tag); */
  if (seq->tag.type == TAG_MAP) {
    map = &seq->tag.data.map;
    if (map->count > 0 && map->values[0].type == TAG_ARRAY) {
      board = &map->values[0].data.array;
      address[1] = 0;
      while (address[1] < BOARD_SIZE) {
        address[0] = 0;
        buf_init(&buf, false, sizeof(line), line);
        while (address[0] < BOARD_SIZE) {
          c = (character *) array_data(board, address);
          buf_write_character_utf8(&buf, *c);
          address[0]++;
        }
        buf_write_u8(&buf, 0);
        
        address[1]++;
      }
      address[0] = BOARD_SIZE / 2;
      address[1] = 0;
      array_data_set(board, address, &g_fly);
    }
  }
  return true;
}
