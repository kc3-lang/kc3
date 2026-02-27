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
#include <string.h>
#include "../libkc3/kc3.h"
#include "embed.h"

s_tag * embed_parse_template (s_buf *input, s_tag *dest)
{
  s_buf_save input_save = {0};
  character c = 0;
  sw level = 0;
 const char *p = NULL;
  sw r;
  e_embed_state state = EMBED_STATE_RAW;
  s_str str = {0};
  p_list *tail = NULL;
  p_list template = NULL;
  s_tag tmp = {0};
  s_buf token_buf = {0};
  s8    token_position = 0;
  if (! input || ! dest) {
    err_puts("embed_parse_template: NULL argument");
    return NULL;
  }
  tail = &template;
  if (! (*tail = list_new_str_1(NULL, "fn () do str([", NULL))) {
    buf_clean(&token_buf);
    return NULL;
  }
  tail = &(*tail)->next.data.td_plist;
  if (! buf_init_alloc(&token_buf, BUF_SIZE)) {
    err_puts("embed_parse_template: buf_init_alloc failed");
    return NULL;
  }
  while (buf_read_character_utf8(input, &c) > 0) {
    switch (state) {
    case EMBED_STATE_VOID:
      err_puts("embed_parse_template: EMBED_STATE_VOID 1");
      assert(! "embed_parse_template: EMBED_STATE_VOID 1");
      return NULL;
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
          p = ",\n  ";
          if (! level) {
            level++;
            p++;
          }
          if (! (*tail = list_new_str_1(NULL, p, NULL))) {
            list_delete_all(template);
            buf_clean(&token_buf);
            return NULL;
          }
          tail = &(*tail)->next.data.td_plist;
          if (buf_read_to_str(&token_buf, &str) <= 0) {
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
          tail = &(*tail)->next.data.td_plist;
          buf_empty(&token_buf);
        }
        if (c == '=') {
          token_position = 0;
          buf_save_init(input, &input_save);
          if (buf_ignore_spaces(input) < 0 ||
              (r = buf_read_1(input, "raw")) < 0 ||
              (r && (r = buf_ignore_spaces(input)) < 0)) {
            buf_save_clean(input, &input_save);
            list_delete_all(template);
            buf_clean(&token_buf);
            return NULL;
          }
          if (r)
            state = EMBED_STATE_VERBOSE_RAW;
          else {
            state = EMBED_STATE_VERBOSE;
            buf_save_restore_rpos(input, &input_save);
          }
          buf_save_clean(input, &input_save);
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
    case EMBED_STATE_VERBOSE_RAW:
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
            switch (state) {
            case EMBED_STATE_SILENT:      p = ",\n  do ";         break;
            case EMBED_STATE_VERBOSE:     p = ",\n  HTML.escape(";
                                                                  break;
            case EMBED_STATE_VERBOSE_RAW: p = ",\n (Str) ";       break;
            case EMBED_STATE_RAW:
            case EMBED_STATE_VOID:
              err_puts("embed_parse_template: invalid state");
              assert(! "embed_parse_template: invalid state");
              abort();
            }
            if (! level) {
              level++;
              p++;
            }
            if (! (*tail = list_new_str_1(NULL, p, NULL))) {
              list_delete_all(template);
              buf_clean(&token_buf);
              return NULL;
            }
            tail = &(*tail)->next.data.td_plist;
            if (buf_read_to_str(&token_buf, &str) <= 0) {
              list_delete_all(template);
              buf_clean(&token_buf);
              return NULL;
            }
            if (! (*tail = list_new_str(str.free.p_pchar, str.size,
                                        str.ptr.p_pchar, NULL))) {
              list_delete_all(template);
              buf_clean(&token_buf);
              return NULL;
            }
            tail = &(*tail)->next.data.td_plist;
            buf_empty(&token_buf);
            switch (state) {
            case EMBED_STATE_SILENT:      p = "\n    \"\"\n  end";
                                                            break;
            case EMBED_STATE_VERBOSE:     p = ")";          break;
            case EMBED_STATE_VERBOSE_RAW: p = NULL;         break;
            case EMBED_STATE_RAW:
            case EMBED_STATE_VOID:
              err_puts("embed_parse_template: EMBED_STATE_VOID 2");
              assert(! "embed_parse_template: EMBED_STATE_VOID 2");
              return NULL;
            }
            if (p) {
              if (! (*tail = list_new_str_1(NULL, p, NULL))) {
                list_delete_all(template);
                buf_clean(&token_buf);
                return NULL;
              }
              tail = &(*tail)->next.data.td_plist;
            }
          }
          level++;
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
      p = ",\n  ";
      if (! level)
        p++;
      if (! (*tail = list_new_str_1(NULL, p, NULL))) {
        list_delete_all(template);
        buf_clean(&token_buf);
        return NULL;
      }
      tail = &(*tail)->next.data.td_plist;
      break;
    case EMBED_STATE_SILENT:
    case EMBED_STATE_VERBOSE:
    case EMBED_STATE_VERBOSE_RAW:
      err_puts("embed_parse_template: unterminated code section");
      assert(! "embed_parse_template: unterminated code section");
      list_delete_all(template);
      buf_clean(&token_buf);
      return NULL;
    case EMBED_STATE_VOID:
      err_puts("embed_parse_template: EMBED_STATE_VOID 3");
      assert(! "embed_parse_template: EMBED_STATE_VOID 3");
      return NULL;
    }
    if (buf_read_to_str(&token_buf, &str) <= 0) {
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
    tail = &(*tail)->next.data.td_plist;
  }
  buf_clean(&token_buf);
  if (! (*tail = list_new_str_1(NULL, " ])\nend", NULL))) {
    list_delete_all(template);
    return NULL;
  }
  tail = &(*tail)->next.data.td_plist;
  if (! str_init_concatenate_list(&str, template)) {
    list_delete_all(template);
    return NULL;
  }
  list_delete_all(template);
  if (! tag_init_from_str(&tmp, &str)) {
    tmp = (s_tag) {0};
    tmp.type = TAG_STR;
    tmp.data.td_str = str;
  }
  else
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
  s_env *env;
  FILE *fp = NULL;
  s_tag tmp = {0};
  if (! path || ! dest) {
    err_puts("embed_parse_template_file: invalid argument");
    assert(! "embed_parse_template_file: invalid argument");
    return NULL;
  }
  env = env_global();
  if (env && env->trace) {
    err_write_1("embed_parse_template_file: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  if (! buf_init_alloc(&buf, BUF_SIZE))
    goto ko;
  if (! (fp = file_open(path, "r"))) {
    buf_clean(&buf);
    goto ko;
  }
  if (! buf_file_open_r(&buf, fp)) {
    fclose(fp);
    buf_clean(&buf);
    goto ko;
  }
  if (! embed_parse_template(&buf, &tmp)) {
    buf_file_close(&buf);
    fclose(fp);
    buf_clean(&buf);
    goto ko;
  }
  buf_file_close(&buf);
  fclose(fp);
  buf_clean(&buf);
  if (env && env->trace) {
    err_write_1("embed_parse_template_file: ");
    err_inspect_str(path);
    err_write_1(": OK\n");
  }
  *dest = tmp;
  return dest;
 ko:
  if (env && env->trace) {
    err_write_1("embed_parse_template_file: ");
    err_inspect_str(path);
    err_write_1(": ERROR\n");
  }
  return NULL;
}

s_tag * embed_parse_template_file_or_abort (const s_str *path, s_tag *dest)
{
  if (! embed_parse_template_file(path, dest))
    abort();
  return dest;
}

s_tag * embed_parse_template_str (const s_str *input, s_tag *dest)
{
  s_buf buf = {0};
  s_tag tmp = {0};
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
  if (! embed_parse_template(&buf, &tmp)) {
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  *dest = tmp;
  return dest;
}
