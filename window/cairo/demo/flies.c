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
#include <math.h>
#include "../../../libkc3/kc3.h"
#include "../cairo_font.h"
#include "../cairo_sprite.h"
#include "../window_cairo.h"
#include "window_cairo_demo.h"
#include "flies.h"

#define BOARD_SIZE    25
#define FLY_TIME_MAX 200

typedef enum {
  BOARD_ITEM_SPACE    = 0,
  BOARD_ITEM_BLOCK    = 1,
  BOARD_ITEM_FLY      = 2,
  BOARD_ITEM_DEAD_FLY = 3
} e_board_item_type;

static const u8  g_board_item_space    = BOARD_ITEM_SPACE;
static const u8  g_board_item_block    = BOARD_ITEM_BLOCK;
static const u8  g_board_item_fly      = BOARD_ITEM_FLY;
static const u8  g_board_item_dead_fly = BOARD_ITEM_DEAD_FLY;
s_cairo_sprite   g_sprite_dead_fly     = {0};
s_cairo_sprite   g_sprite_fly          = {0};
static const f64 g_xy_ratio            = 0.6;

static void fly_init (s_map *map)
{
  uw address[2] = { BOARD_SIZE / 2,
                    0 };
  s_array *board;
  uw *in;
  f64 *t;
  board = &map->value[0].data.array;
  in    = &map->value[1].data.uw;
  t     = &map->value[3].data.f64;
  array_data_set(board, address, (void *) &g_board_item_fly);
  *t = 0.0;
  (*in)++;
}

bool flies_load (s_sequence *seq)
{
  uw address[2];
  s_array *board;
  uw i;
  uw j;
  s_map *map;
  tag_map(&seq->tag, 4);
  map = &seq->tag.data.map;
  tag_init_psym(  map->key  + 0, sym_1("board"));
  tag_init_array(map->value + 0, sym_1("U8[]"),
                 2, (uw[]) {BOARD_SIZE, BOARD_SIZE});
  tag_init_psym(   map->key + 1, sym_1("in"));
  tag_init_uw(   map->value + 1, 0);
  tag_init_psym(   map->key + 2, sym_1("out"));
  tag_init_uw(   map->value + 2, 0);
  tag_init_psym(   map->key + 3, sym_1("t"));
  tag_init_uw(   map->value + 3, 0);
  board = &map->value[0].data.array;
  array_allocate(board);
  i = 0;
  while (i < BOARD_SIZE) {
    address[0] = i;
    j = 0;
    while (j < BOARD_SIZE) {
      address[1] = j;
      array_data_set(board, address, (void *) &g_board_item_space);
      j++;
    }
    i++;
  }
  i = 0;
  while (i < BOARD_SIZE) {
    address[0] = i;
    address[1] = 0;
    array_data_set(board, address, (void *) &g_board_item_block);
    address[1] = BOARD_SIZE - 1;
    array_data_set(board, address, (void *) &g_board_item_block);
    address[0] = 0;
    address[1] = i;
    array_data_set(board, address, (void *) &g_board_item_block);
    address[0] = BOARD_SIZE - 1;
    array_data_set(board, address, (void *) &g_board_item_block);
    i++;
  }
  address[0] = BOARD_SIZE / 2;
  address[1] = 0;
  array_data_set(board, address, (void *) &g_board_item_space);
  address[1] = BOARD_SIZE - 1;
  array_data_set(board, address, (void *) &g_board_item_space);
  address[1] = BOARD_SIZE / 2;
  i = 1;
  while (i <= BOARD_SIZE / 2) {
    address[0] = i;
    array_data_set(board, address, (void *) &g_board_item_block);
    i++;
  }
  address[0] = BOARD_SIZE / 2;
  j = BOARD_SIZE / 4;
  while (j < BOARD_SIZE / 2) {
    address[1] = j;
    array_data_set(board, address, (void *) &g_board_item_block);
    j++;
  }
  address[1] = BOARD_SIZE * 3 / 4;
  i = BOARD_SIZE / 4;
  while (i < BOARD_SIZE - 1) {
    address[0] = i;
    array_data_set(board, address, (void *) &g_board_item_block);
    i++;
  }
  fly_init(map);
  return true;
}

bool flies_render (s_sequence *seq)
{
  char a[BOARD_SIZE];
  uw address[2];
  s_array *board;
  f64 board_w;
  f64 board_h;
  f64 board_x;
  u8 *board_item;
  f64 board_item_w;
  f64 board_item_h;
  s_buf buf;
  cairo_t *cr;
  f64 dead_fly_scale;
  u8   direction;
  u8   direction_prev = 4;
  bool directions[9];
  uw  fly_address[2];
  uw *fly_in;
  uw  fly_prev_address[2];
  uw *fly_out;
  f64 fly_scale;
  uw *fly_time;
  uw i;
  uw j;
  s_map *map;
  uw r;
  uw random_bits = 0;
  cairo_text_extents_t te;
  f64 x;
  f64 y;
  s_window_cairo *window;
  window = seq->window;
  cr = window->cr;
  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  /* io_inspect(&seq->tag); */
  if (seq->tag.type == TAG_MAP) {
    map = &seq->tag.data.map;
    if (map->count == 4 &&
        map->value[0].type == TAG_ARRAY &&
        map->value[1].type == TAG_UW &&
        map->value[2].type == TAG_UW &&
        map->value[3].type == TAG_UW) {
      board    = &map->value[0].data.array;
      fly_in   = &map->value[1].data.uw;
      fly_out  = &map->value[2].data.uw;
      fly_time = &map->value[3].data.uw;
      board_item_h = (f64) (window->h - 60) / (BOARD_SIZE + 1);
      board_item_w = board_item_h * g_xy_ratio;
      board_w = board_item_w * BOARD_SIZE;
      board_h = board_item_h * BOARD_SIZE;
      board_x = (window->w - board_w) / 2.0;
      fly_scale = 2.0 * board_item_w / g_sprite_fly.w;
      dead_fly_scale = 2.0 * board_item_w / g_sprite_dead_fly.w;
      cairo_set_source_rgb(cr, 0.6, 0.7, 0.9);
      cairo_rectangle(cr, board_x, 0, board_w, board_h);
      cairo_fill(cr);
      cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
      cairo_set_font_size(cr, board_item_h);
      cairo_font_set(cr, &g_font_courier_new);
      buf_init(&buf, false, sizeof(a), a);
      buf_write_1(&buf, "In ");
      buf_inspect_uw_decimal(&buf, *fly_in);
      buf_write_u8(&buf, 0);
      cairo_text_extents(cr, buf.ptr.pchar, &te);
      y = board_h + board_item_h + te.height + te.y_bearing;
      x = board_x;
      cairo_move_to(cr, x, y);
      cairo_show_text(cr, buf.ptr.pchar);
      buf_init(&buf, false, sizeof(a), a);
      buf_write_1(&buf, "Out ");
      buf_inspect_uw_decimal(&buf, *fly_out);
      buf_write_u8(&buf, 0);
      x = board_x + board_item_w * (BOARD_SIZE / 2 + 1);
      cairo_move_to(cr, x, y);
      cairo_show_text(cr, buf.ptr.pchar);
      address[1] = 0;
      while (address[1] < BOARD_SIZE) {
        y = board_item_h * address[1];
        address[0] = 0;
        while (address[0] < BOARD_SIZE) {
          x = board_x + board_item_w * address[0];
          cairo_translate(cr, x, y);
          board_item = (u8 *) array_data(board, address);
          assert(board_item);
          switch (*board_item) {
          case BOARD_ITEM_SPACE:
            break;
          case BOARD_ITEM_BLOCK:
            cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
            cairo_rectangle(cr, 0, 0, board_item_w + 1.0,
                            board_item_h + 1.0);
            cairo_fill(cr);
            break;
          case BOARD_ITEM_FLY:
            cairo_translate(cr, -board_item_w / 2.0,
                            -board_item_h / 2.0);
            cairo_scale(cr, fly_scale, fly_scale);
            cairo_sprite_blit(&g_sprite_fly, 0,
                              cr, 0, 0);
            if (address[0] == BOARD_SIZE / 2 &&
                address[1] == BOARD_SIZE - 1) {
              array_data_set(board, address, (void *) &g_board_item_space);
              (*fly_out)++;
              fly_init(map);
              break;
            }
            fly_address[0] = address[0];
            fly_address[1] = address[1];
            i = 0;
            while (i < 3) {
              j = 0;
              while (j < 9) {
                directions[j] = true;
                j++;
              }
              while (directions[0] | directions[1] | directions[2] |
                     directions[3] | directions[4] | directions[5] |
                     directions[6] | directions[7] | directions[8]) {
                if (random_bits < 4) {
                  r = arc4random();
                  random_bits = 32;
                }
                direction = r % 16;
                r >>= 4;
                random_bits -= 4;
                if (direction >= 12)
                  direction = direction_prev;
                if (direction >= 9)
                  direction = (direction - 6) % 3 + 6;
                fly_prev_address[0] = fly_address[0];
                fly_prev_address[1] = fly_address[1];
                switch (direction) {
                case 0: fly_address[0]--; fly_address[1]--; break;
                case 1:                   fly_address[1]--; break;
                case 2: fly_address[0]++; fly_address[1]--; break;
                case 3: fly_address[0]--;                 ; break;
                case 4:                                   ; break;
                case 5: fly_address[0]++;                 ; break;
                case 6: fly_address[0]--; fly_address[1]++; break;
                case 7:                   fly_address[1]++; break;
                case 8: fly_address[0]++; fly_address[1]++; break;
                }
                if (fly_address[0] < BOARD_SIZE &&
                    fly_address[1] < BOARD_SIZE &&
                    (board_item = (u8 *) array_data(board,
                                                    fly_address)) &&
                    *board_item == g_board_item_space) {
                  array_data_set(board, fly_prev_address,
                                 (void *) &g_board_item_space);
                  array_data_set(board, fly_address,
                                 (void *) &g_board_item_fly);
                  direction_prev = direction;
                  break;
                }
                directions[direction] = false;
                fly_address[0] = fly_prev_address[0];
                fly_address[1] = fly_prev_address[1];
              }
              i++;
            }
            *fly_time += 1;
            if (*fly_time > FLY_TIME_MAX) {
              array_data_set(board, fly_address,
                             (void *) &g_board_item_dead_fly);
              fly_init(map);
            }
            break;
          case BOARD_ITEM_DEAD_FLY:
            cairo_translate(cr, -board_item_w / 2.0,
                            -board_item_h / 2.0);
            cairo_scale(cr, dead_fly_scale, dead_fly_scale);
            cairo_sprite_blit(&g_sprite_dead_fly, 0,
                              cr, 0, 0);
            break;
          }
          cairo_identity_matrix(cr);
          address[0]++;
        }
        address[1]++;
      }
    }
  }
  return true;
}

bool flies_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
