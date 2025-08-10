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
  character c = 0;
  const char *p = NULL;
  e_embed_state state = EMBED_STATE_RAW;
  s_str str = {0};
  p_list *tail = NULL;
  p_list template = NULL;
  s_tag tmp = {0};
  s_buf token_buf = {0};
  uw token_position = 0;
  if (! input || ! dest) {
    err_puts("embed_parse_template: NULL argument");
    return NULL;
  }
  tail = &template;
  if (! (*tail = list_new_str_1(NULL, "fn (buf) do", NULL))) {
    buf_clean(&token_buf);
    return NULL;
  }
  tail = &(*tail)->next.data.plist;
  if (! buf_init_alloc(&token_buf, BUF_SIZE)) {
    err_puts("embed_parse_template: buf_init_alloc failed");
    return NULL;
  }
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
          if (! (*tail = list_new_str_1(NULL, "\nEKC3.raw_str ", NULL))) {
            list_delete_all(template);
            buf_clean(&token_buf);
            return NULL;
          }
          tail = &(*tail)->next.data.plist;
          if (! buf_read_to_str(&token_buf, &str)) {
            list_delete_all(template);
            buf_clean(&token_buf);
            return NULL;
          }
          if (! (*tail = list_new_str_inspect_str(&str, NULL))) {
            list_delete_all(template);
            str_clean(&str);
            buf_clean(&token_buf);
            return NULL;
          }
          str_clean(&str);
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
            p = (state == EMBED_STATE_VERBOSE) ? "\nEKC3.verbose " :
              "\n";
            if (! (*tail = list_new_str_1(NULL, p, NULL))) {
              list_delete_all(template);
              buf_clean(&token_buf);
              return NULL;
            }
            tail = &(*tail)->next.data.plist;
            if (! buf_read_to_str(&token_buf, &str)) {
              list_delete_all(template);
              buf_clean(&token_buf);
              return NULL;
            }
            if (! (*tail = list_new_str(str.free.pchar, str.size,
                                        str.ptr.pchar, NULL))) {
              list_delete_all(template);
              buf_clean(&token_buf);
              return NULL;
            }
            tail = &(*tail)->next.data.plist;
            buf_empty(&token_buf);
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
    switch (state) {
    case EMBED_STATE_RAW:
      if (! (*tail = list_new_str_1(NULL, "\nEKC3.raw_str ", NULL))) {
        list_delete_all(template);
        buf_clean(&token_buf);
        return NULL;
      }
      tail = &(*tail)->next.data.plist;
      break;
    case EMBED_STATE_SILENT:
    case EMBED_STATE_VERBOSE:
      err_puts("embed_parse_template: unterminated code section");
      assert(! "embed_parse_template: unterminated code section");
      list_delete_all(template);
      buf_clean(&token_buf);
      return NULL;
    }
    if (! buf_read_to_str(&token_buf, &str)) {
      list_delete_all(template);
      buf_clean(&token_buf);
      return NULL;
    }
    if (! (*tail = list_new_str_inspect_str(&str, NULL))) {
      list_delete_all(template);
      str_clean(&str);
      buf_clean(&token_buf);
      return NULL;
    }
    str_clean(&str);
    tail = &(*tail)->next.data.plist;
  }
  buf_clean(&token_buf);
  if (! (*tail = list_new_str_1(NULL, "\nend", NULL))) {
    list_delete_all(template);
    return NULL;
  }
  tail = &(*tail)->next.data.plist;
  if (! str_init_concatenate_list(&str, template)) {
    list_delete_all(template);
    return NULL;
  }
  list_delete_all(template);
  tag_init_from_str(&tmp, &str);
  str_clean(&str);
  *dest = tmp;
  return dest;
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

s_tag * embed_parse_template_file (const s_str *path, s_tag *dest)
{
  s_buf buf = {0};
  FILE *fp = NULL;
  s_tag tmp = {0};
  if (! path || ! dest) {
    err_puts("embed_parse_template_file: invalid argument");
    assert(! "embed_parse_template_file: invalid argument");
    return NULL;
  }
  if (! buf_init_alloc(&buf, BUF_SIZE))
    return NULL;
  if (! (fp = file_open(path->ptr.pchar, "r"))) {
    buf_clean(&buf);
    return NULL;
  }
  if (! buf_file_open_r(&buf, fp)) {
    fclose(fp);
    buf_clean(&buf);
  }
  if (! embed_parse_template(&buf, &tmp)) {
    buf_file_close(&buf);
    buf_clean(&buf);
    return NULL;
  }
  buf_file_close(&buf);
  buf_clean(&buf);
  *dest = tmp;
  return dest;
}

s_tag * embed_parse_template_str (const s_str *input, s_tag *dest)
{
  s_buf buf = {0};
  if (! input || ! dest) {
    err_puts("embed_parse_template_str: invalid argument");
    assert(! "embed_parse_template_str: invalid argument");
    return NULL;
  }
  if (! buf_init_str_const(&buf, input)) {
    err_puts("embed_parse_template_str: buf_init_str");
    assert(! "embed_parse_template_str: buf_init_str");
    return NULL;
  }
  return embed_parse_template(&buf, dest);
}

