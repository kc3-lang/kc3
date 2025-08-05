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
#include <string.h>
#include <libkc3/kc3.h>
#include "embed.h"

s_tag * embed_parse_template (s_buf *input, s_tag *dest)
{
  character c;
  static s_ident verbose = {0};
  e_embed_state state = EMBED_STATE_RAW;
  s_str str;
  p_list *tail;
  p_list template = NULL;
  s_buf token_buf;
  uw token_position = 0;
  if (! input || ! dest) {
    err_puts("embed_parse_template: NULL argument");
    return NULL;
  }
  if (! buf_init_alloc(&token_buf, BUF_SIZE)) {
    err_puts("embed_parse_template: buf_init_alloc failed");
    return NULL;
  }
  if (! verbose.sym) {
    ident_init(&verbose, sym_1("EKC3"), sym_1("verbose"));
  }
  tail = &template;
  while (buf_read_character_utf8(input, &c) > 0) {
    switch (state) {
    case EMBED_STATE_RAW:
      switch (token_position) {
      case 0:
        if (c == '<')
          token_position = 1;
        else
          buf_write_character_utf8(&token_buf, c);
        break;
      case 1:
        if (c == '%')
          token_position = 2;
        else {
          token_position = 0;
          buf_write_character_utf8(&token_buf, '<');
          buf_write_character_utf8(&token_buf, c);
        }
        break;
      case 2:
        if (token_buf.wpos > 0) {
          if (! buf_read_to_str(&token_buf, &str)) {
            buf_clean(&token_buf);
            return NULL;
          }
          if (! (*tail = list_new_str_copy(&str, NULL))) {
            str_clean(&str);
            buf_clean(&token_buf);
            return NULL;
          }
          tail = &(*tail)->next.data.plist;
          buf_empty(&token_buf);
        }
        if (c == '=') {
          state = EMBED_STATE_VERBOSE;
          token_position = 0;
        } else {
          state = EMBED_STATE_SILENT;
          token_position = 0;
          buf_write_character_utf8(&token_buf, c);
        }
        break;
      }
      break;
    case EMBED_STATE_SILENT:
    case EMBED_STATE_VERBOSE:
      switch (token_position) {
      case 0:
        if (c == '%')
          token_position = 1;
        else
          buf_write_character_utf8(&token_buf, c);
        break;
      case 1:
        if (c == '>') {
          if (token_buf.wpos > 0) {
            if (! buf_read_to_str(&token_buf, &str)) {
              buf_clean(&token_buf);
              return NULL;
            }
            if (! (*tail = list_new_str_copy(&str, NULL))) {
              str_clean(&str);
              buf_clean(&token_buf);
              return NULL;
            }
            tail = &(*tail)->next.data.plist;
            buf_empty(&token_buf);
          }
          if (state == EMBED_STATE_VERBOSE) {
            if (! (*tail = list_new_ident(&verbose, NULL))) {
              buf_clean(&token_buf);
              return NULL;
            }
            tail = &(*tail)->next.data.plist;
          }
          state = EMBED_STATE_RAW;
          token_position = 0;
        }
        else {
          buf_write_character_utf8(&token_buf, '%');
          buf_write_character_utf8(&token_buf, c);
          token_position = 0;
        }
        break;
      }
      break;
    }
  }
  if (token_buf.wpos > 0) {
    if (! buf_read_to_str(&token_buf, &str)) {
      buf_clean(&token_buf);
      return NULL;
    }
    template = list_new_str_copy(&str, template);
    if (! template) {
      str_clean(&str);
      buf_clean(&token_buf);
      return NULL;
    }
  }
  buf_clean(&token_buf);
  return tag_init_plist(dest, template);
}

s_tag * embed_parse_template_1 (const char *input, s_tag *dest)
{
  s_buf input_buf;
  if (! input || ! dest) {
    err_puts("embed_parse_template_1: NULL argument");
    return NULL;
  }
  if (! buf_init_const(&input_buf, strlen(input), input)) {
    err_puts("embed_parse_template_1: buf_init_const failed");
    return NULL;
  }
  return embed_parse_template(&input_buf, dest);
}

