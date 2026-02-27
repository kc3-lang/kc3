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
#include <unistd.h>
#include "ekc3.h"
#include "html.h"

s_tag * ekc3_parse_template (s_buf *input, s_tag *dest)
{
  s_buf buf = {0};
  character c;
  e_ekc3_state state;
  s_tag tmp = {0};
  s_buf token_buf = {0};
  u8    token_position;
  s_str token_str = {0};
  if (! input || ! dest) {
    err_puts("ekc3_parse_template_from_stream: invalid arguments");
    return NULL;
  }
  if (! buf_init_alloc(&buf, BUF_SIZE)) {
    err_puts("ekc3_parse_template_from_stream: buf_init_alloc result");
    return NULL;
  }
  if (! buf_init_alloc(&token_buf, BUF_SIZE)) {
    err_puts("ekc3_parse_template_from_stream: buf_init_alloc token");
    buf_clean(&buf);
    return NULL;
  }
  state = EKC3_STATE_RAW;
  token_position = 0;
  while (true) {
    if (buf_read_character_utf8(input, &c) <= 0) {
      switch (state) {
      case EKC3_STATE_RAW:
        if (buf_write_character_utf8(&buf, '\n') <= 0 ||
            buf_write_1(&buf, "EKC3.verbose ") <= 0 ||
            buf_read_to_str(&token_buf, &token_str) <= 0)
          goto error;
        if (buf_inspect_str(&buf, &token_str) <= 0) {
          str_clean(&token_str);
          goto error;
        }
        str_clean(&token_str);
        break;
      case EKC3_STATE_VERBOSE:
        if (buf_write_1(&buf, "\nEKC3.verbose ") <= 0 ||
            buf_read_to_str(&token_buf, &token_str) <= 0)
          goto error;
        if (buf_write_str(&buf, &token_str) <= 0) {
          str_clean(&token_str);
          goto error;
        }
        str_clean(&token_str);
        break;
      case EKC3_STATE_SILENT:
        if (buf_read_to_str(&token_buf, &token_str) <= 0)
          goto error;
        if (buf_write_str(&buf, &token_str) <= 0) {
          str_clean(&token_str);
          goto error;
        }
        str_clean(&token_str);
        break;
      }
      break;
    }
    switch (state) {
    case EKC3_STATE_RAW:
      switch (token_position) {
      case 0:
        if (c == '<') {
          token_position = 1;
        } else {
          if (buf_write_character_utf8(&token_buf, c) <= 0) {
            goto error;
          }
        }
        break;
      case 1:
        if (c == '%') {
          token_position = 2;
        } else {
          token_position = 0;
          if (buf_write_character_utf8(&token_buf, '<') <= 0 ||
              buf_write_character_utf8(&token_buf, c) <= 0) {
            goto error;
          }
        }
        break;
      case 2:
        if (buf_write_character_utf8(&buf, '\n') <= 0 ||
            buf_write_1(&buf, "EKC3.verbose ") <= 0)
          goto error;
        if (buf_read_to_str(&token_buf, &token_str) <= 0)
          goto error;
        if (buf_inspect_str(&buf, &token_str) <= 0) {
          str_clean(&token_str);
          goto error;
        }
        str_clean(&token_str);
        buf_empty(&token_buf);
        if (c == '=') {
          state = EKC3_STATE_VERBOSE;
          token_position = 0;
        } else {
          state = EKC3_STATE_SILENT;
          token_position = 0;
          if (buf_write_character_utf8(&token_buf, c) <= 0) {
            goto error;
          }
        }
        break;
      }
      break;
    case EKC3_STATE_SILENT:
    case EKC3_STATE_VERBOSE:
      switch (token_position) {
      case 0:
        if (c == '%')
          token_position = 1;
        else if (buf_write_character_utf8(&token_buf, c) <= 0)
          goto error;
        break;
      case 1:
        if (c == '>') {
          if (buf_read_to_str(&token_buf, &token_str) <= 0)
            goto error;
          if (state == EKC3_STATE_VERBOSE &&
              buf_write_1(&buf, "\nEKC3.verbose ") <= 0) {
            str_clean(&token_str);
            goto error;
          }
          if (buf_write_str(&buf, &token_str) <= 0) {
            str_clean(&token_str);
            goto error;
          }
          str_clean(&token_str);
          state = EKC3_STATE_RAW;
          token_position = 0;
          buf_empty(&token_buf);
        }
        else {
          if (buf_write_character_utf8(&token_buf, '%') <= 0 ||
              buf_write_character_utf8(&token_buf, c) <= 0)
            goto error;
          token_position = 0;
        }
        break;
      }
      break;
    }
  }
  if (buf_parse_tag(&buf, &tmp) <= 0) {
    err_puts("ekc3_parse_template: buf_parse_tag");
    err_inspect_buf(&buf);
    assert(! "ekc3_parse_template: buf_parse_tag");
    goto error;
  }
  buf_clean(&token_buf);
  buf_clean(&buf);
  *dest = tmp;
  return dest;
 error:
  buf_clean(&token_buf);
  buf_clean(&buf);
  return NULL;
}

s_tag * ekc3_parse_template_1 (const char *input, s_tag *dest)
{
  s_buf input_buf;
  uw len;
  assert(input);
  assert(dest);
  if (! input || ! dest) {
    err_puts("ekc3_parse_template_1: invalid arguments");
    assert(! "ekc3_parse_template_1: invalid arguments");
    return NULL;
  }
  len = strlen(input);
  if (! buf_init_const(&input_buf, len, input)) {
    err_puts("ekc3_parse_template_1: buf_init_const");
    assert(! "ekc3_parse_template_1: buf_init_const");
    return NULL;
  }
  input_buf.wpos = len;
  return ekc3_parse_template(&input_buf, dest);
}

s_tag * ekc3_parse_template_str (const s_str *input, s_tag *dest)
{
  s_buf input_buf;
  assert(input);
  assert(dest);
  if (! input || ! dest) {
    err_puts("ekc3_parse_template_str: invalid arguments");
    assert(! "ekc3_parse_template_str: invalid arguments");
    return NULL;
  }
  if (! buf_init_const(&input_buf, input->size, input->ptr.p_pchar)) {
    err_puts("ekc3_parse_template_str: buf_init_const");
    assert(! "ekc3_parse_template_str: buf_init_const");
    return NULL;
  }
  input_buf.wpos = input->size;
  return ekc3_parse_template(&input_buf, dest);
}

/*
s_list *** ekc3_append_and_empty_buf (s_list ***tail, s_buf *buf)
{
  s_str str;
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(buf);
  if (buf_read_to_str(buf, &str) <= 0) {
    err_puts("ekc3_append_and_empty_buf: buf_read_to_str");
    assert(! "ekc3_append_and_empty_buf: buf_read_to_str");
    return NULL;
  }
  if (! str.size) {
    buf_empty(buf);
    return tail;
  }
  **tail = list_new(NULL);
  if (! **tail) {
    str_clean(&str);
    err_puts("ekc3_append_and_empty_buf: list_new");
    assert(! "ekc3_append_and_empty_buf: list_new");
    return NULL;
  }
  (**tail)->tag.type = TAG_STR;
  (**tail)->tag.data.str = str;
  buf_empty(buf);
  *tail = &(**tail)->next.data.plist;
  return tail;
}

s_list *** ekc3_append_do_block (s_list ***tail, s_do_block *do_block)
{
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(do_block);
  **tail = list_new(NULL);
  if (! **tail)
    return NULL;
  (**tail)->tag.type = TAG_DO_BLOCK;
  (**tail)->tag.data.do_block = *do_block;
  *tail = &(**tail)->next.data.plist;
  return tail;
}

s_list *** ekc3_append_silent_do_block (s_list ***tail,
                                        s_do_block *do_block)
{
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(do_block);
  if (! ekc3_append_sym(tail, sym_1("silent")))
    return NULL;
  if (! ekc3_append_do_block(tail, do_block))
    return NULL;
  return tail;
}

s_list *** ekc3_append_str (s_list ***tail, s_str *str)
{
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(str);
  **tail = list_new(NULL);
  if (! *tail)
    return NULL;
  (**tail)->tag.type = TAG_STR;
  (**tail)->tag.data.str = *str;
  *tail = &(**tail)->next.data.plist;
  return tail;
}

s_list *** ekc3_append_sym (s_list ***tail, const s_sym *sym)
{
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(sym);
  **tail = list_new(NULL);
  if (! **tail)
    return NULL;
  (**tail)->tag.type = TAG_PSYM;
  (**tail)->tag.data.psym = sym;
  *tail = &(**tail)->next.data.plist;
  return tail;
}

sw ekc3_buf_parse (s_buf *buf, p_ekc3 *dest)
{
  s_do_block do_block;
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf str_buf;
  s_list **tail;
  p_ekc3 tmp;
  assert(dest);
  assert(buf);
  if (! buf_init_alloc(&str_buf, BUF_SIZE)) {
    err_puts("ekc3_buf_parse: buf_init_alloc");
    assert(! "ekc3_buf_parse: buf_init_alloc");
    return -1;
  }
  ekc3_init(&tmp);
  tail = &tmp;
  buf_save_init(buf, &save);
  while (1) {
    r = buf_read_1(buf, "<%=");
    if (r < 0)
      break;
    if (r > 0) {
      result += r;
      if (! ekc3_append_and_empty_buf(&tail, &str_buf)) {
        err_puts("ekc3_buf_parse: ekc3_append_and_empty_buf 1");
        assert(! "ekc3_buf_parse: ekc3_append_and_empty_buf 1");
        goto ko;
      }
      r = ekc3_buf_parse_kc3_do_block(buf, &do_block);
      if (r <= 0) {
        err_puts("ekc3_buf_parse: ekc3_buf_parse_kc3_do_block");
        assert(! "ekc3_buf_parse: ekc3_buf_parse_kc3_do_block");
        goto restore;
      }
      if (! ekc3_append_do_block(&tail, &do_block)) {
        err_puts("ekc3_buf_parse: ekc3_append_do_block");
        assert(! "ekc3_buf_parse: ekc3_append_do_block");
        goto ko;
      }
      continue;
    }
    r = ekc3_buf_peek_kc3_silent_do_block(buf);
    if (r < 0) {
      err_puts("ekc3_buf_parse: ekc3_buf_peek_kc3_silent_do_block");
      assert(! "ekc3_buf_parse: ekc3_buf_peek_kc3_silent_do_block");
      goto ko;
    }
    if (r > 0) {
      if (! ekc3_append_and_empty_buf(&tail, &str_buf)) {
        err_puts("ekc3_buf_parse: ekc3_append_and_empty_buf 2");
        assert(! "ekc3_buf_parse: ekc3_append_and_empty_buf 2");
        goto ko;
      }
      r = ekc3_buf_parse_kc3_silent_do_block(buf, &do_block);
      if (r <= 0) {
        err_puts("ekc3_buf_parse: ekc3_buf_parse_kc3_silent_do_block");
        assert(! "ekc3_buf_parse: ekc3_buf_parse_kc3_silent_do_block");
        goto restore;
      }
      if (! ekc3_append_silent_do_block(&tail, &do_block)) {
        err_puts("ekc3_buf_parse: ekc3_append_silent_do_block");
        assert(! "ekc3_buf_parse: ekc3_append_silent_do_block");
        goto ko;
      }
      continue;
    }
    r = buf_read_character_utf8(buf, &c);
    if (r <= 0) {
      if (! ekc3_append_and_empty_buf(&tail, &str_buf)) {
        err_puts("ekc3_buf_parse: ekc3_append_and_empty_buf 3");
        assert(! "ekc3_buf_parse: ekc3_append_and_empty_buf 3");
        goto ko;
      }
      break;
    }
    result += r;
    r = buf_write_character_utf8(&str_buf, c);
    if (r <= 0) {
      err_puts("ekc3_buf_parse: buf_write_character_utf8 str_buf");
      assert(! "ekc3_buf_parse: buf_write_character_utf8 str_buf");
      goto restore;
    }
  }
  //err_inspect_list((const s_list * const *) &tmp);
  *dest = tmp;
  r = result;
  goto clean;
 ko:
  r = -1;
 restore:
  buf_save_restore_rpos(buf, &save);
  ekc3_clean(&tmp);
 clean:
  buf_clean(&str_buf);
  buf_save_clean(buf, &save);
  return r;
}

sw ekc3_buf_parse_kc3_do_block (s_buf *buf, s_do_block *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_list *list;
  s_list **tail;
  s_do_block tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  list = NULL;
  tail = &list;
  while (1) {
    r = buf_ignore_spaces(buf);
    if (r < 0) {
      err_puts("ekc3_buf_parse_kc3_do_block: buf_ignore_spaces 1");
      assert(! "ekc3_buf_parse_kc3_do_block: buf_ignore_spaces 1");
      goto clean;
    }
    r = buf_read_1(buf, "%>");
    if (r < 0)
      goto clean;
    if (r > 0) {
      result += r;
      break;
    }
    *tail = list_new(NULL);
    r = buf_parse_tag(buf, &(*tail)->tag);
    if (r < 0) {
      list_delete_all(list);
      err_puts("ekc3_buf_parse_kc3_do_block: buf_parse_tag < 0");
      assert(! "ekc3_buf_parse_kc3_do_block: buf_parse_tag < 0");
      goto clean;
    }
    if (! r) {
      *tail = list_delete(*tail);
      err_puts("ekc3_buf_parse_kc3_do_block: buf_parse_tag = 0");
      assert(! "ekc3_buf_parse_kc3_do_block: buf_parse_tag = 0");
      break;
    }
    result += r;
    tail = &(*tail)->next.data.plist;
  }
  if (! do_block_init_from_list(&tmp, &list)) {
    list_delete_all(list);
    err_puts("ekc3_buf_parse_kc3_do_block: do_block_init_from_list");
    assert(! "ekc3_buf_parse_kc3_do_block: do_block_init_from_list");
    r = -1;
    goto clean;
  }
  list_delete_all(list);
  *dest = tmp;
  r = result;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw ekc3_buf_parse_kc3_silent_do_block (s_buf *buf, s_do_block *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  r = buf_ignore_spaces(buf);
  if (r < 0) {
    err_puts("ekc3_buf_parse_kc3_silent_do_block: buf_ignore_spaces 1");
    assert(! "ekc3_buf_parse_kc3_silent_do_block: buf_ignore_spaces 1");
    goto clean;
  }
  r = buf_read_1(buf, "<%");
  if (r < 0) {
    err_puts("ekc3_buf_parse_kc3_silent_do_block: buf_read_1 < 0");
    assert(! "ekc3_buf_parse_kc3_silent_do_block: buf_read_1 < 0");
    goto restore;
  }
  if (! r) {
    err_puts("ekc3_buf_parse_kc3_silent_do_block: buf_read_1 = 0");
    assert(! "ekc3_buf_parse_kc3_silent_do_block: buf_read_1 = 0");
    goto restore;
  }
  result += r;
  r = buf_ignore_spaces(buf);
  if (r <= 0) {
    err_puts("ekc3_buf_parse_kc3_silent_do_block: buf_ignore_spaces 2");
    assert(! "ekc3_buf_parse_kc3_silent_do_block: buf_ignore_spaces 2");
    goto restore;
  }
  result += r;
  r = ekc3_buf_parse_kc3_do_block(buf, dest);
  if (r <= 0) {
    err_puts("ekc3_buf_parse_kc3_silent_do_block:"
             " buf_parse_kc3_do_block");
    assert(!("ekc3_buf_parse_kc3_silent_do_block:"
             " buf_parse_kc3_do_block"));
    goto restore;
  }
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw ekc3_buf_peek_kc3_silent_do_block (s_buf *buf)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  r = buf_ignore_spaces(buf);
  if (r < 0) {
    r = 0;
    goto clean;
  }
  result += r;
  r = buf_read_1(buf, "<%");
  if (r <= 0)
    goto restore;
  result += r;
  r = buf_ignore_spaces(buf);
  if (r <= 0)
    goto restore;
  result += r;
  r = result;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

void ekc3_clean (p_ekc3 *ekc3)
{
  list_delete_all(*ekc3);
}

bool ekc3_eval_silent_do_block (const s_do_block *do_block)
{
  uw i = 0;
  s_tag result = {0};
  assert(do_block);
  while (i < do_block->count) {
    if (! eval_tag(do_block->tag + i, &result))
      return false;
    tag_clean(&result);
    i++;
  }
  return true;
}

p_ekc3 * ekc3_init (p_ekc3 *ekc3)
{
  p_ekc3 tmp = {0};
  *ekc3 = tmp;
  return ekc3;
}

s_str * ekc3_inspect_do_block (const s_do_block *do_block, s_str *dest)
{
  uw i = 0;
  s_tag result = {0};
  const s_sym *type;
  assert(do_block);
  while (i < do_block->count) {
    tag_clean(&result);
    if (! eval_tag(do_block->tag + i, &result))
      return NULL;
    i++;
  }
  switch (result.type) {
  case TAG_STR:
    if (! str_init_copy(dest, &result.data.str))
      return NULL;
    break;
  default:
    type = &g_sym_Str;
    if (! str_init_cast(dest, &type, &result))
      return NULL;
    break;
  }
  tag_clean(&result);
  return dest;
}

s_tag * ekc3_load (const s_str *path, s_tag *dest)
{
  s_buf buf;
  s32 fd = -1;
  s_tag tmp = {0};
  if (! buf_init_alloc(&buf, BUF_SIZE))
    return NULL;
  if (! file_open_r(path, &fd))
    goto clean;
  if (! buf_fd_open_r(&buf, fd))
    goto clean;
  if (! ekc3_buf_parse(&buf, &tmp.data.plist))
    goto clean;
  tmp.type = TAG_PLIST;
  buf_clean(&buf);
  *dest = tmp;
  return dest;
 clean:
  if (fd > 0)
    close(fd);
  buf_clean(&buf);
  return NULL;
}

sw ekc3_render (s_buf *buf, const p_ekc3 *ekc3)
{
  const s_list *l;
  sw r;
  sw result = 0;
  assert(ekc3);
  l = *ekc3;
  while (l) {
    if (l->tag.type == TAG_PSYM &&
        l->tag.data.psym == sym_1("silent")) {
      l = list_next(l);
      if (! l ||
          l->tag.type != TAG_DO_BLOCK) {
        err_puts("ekc3_render: :silent without a do_block");
        assert(! "ekc3_render: :silent without a do_block");
        return -1;
      }
      if (! ekc3_eval_silent_do_block(&l->tag.data.do_block)) {
        err_puts("ekc3_render: ekc3_eval_silent_do_block");
        assert(! "ekc3_render: ekc3_eval_silent_do_block");
        return -1;
      }
    }
    else {
      if ((r = ekc3_render_tag(buf, &l->tag)) < 0) {
        err_puts("ekc3_render: ekc3_render_tag");
        assert(! "ekc3_render: ekc3_render_tag");
        return r;
      }
      result += r;
    }
    l = list_next(l);
  }
  return result;
}

s_str * ekc3_render_buf_to_str (s_buf *in, s_str *dest)
{
  p_ekc3 ekc3;
  sw r;
  assert(in);
  ekc3_init(&ekc3);
  r = ekc3_buf_parse(in, &ekc3);
  if (r < 0) {
    err_puts("ekc3_render_buf_to_str: ekc3_buf_parse");
    assert(! "ekc3_render_buf_to_str: ekc3_buf_parse");
    return NULL;
  }
  if (! ekc3_render_to_str(&ekc3, dest)) {
    ekc3_clean(&ekc3);
    return NULL;
  }
  ekc3_clean(&ekc3);
  return dest;
}

s_str * ekc3_render_file_to_str (const s_str *path, s_str *dest)
{
  s_env *env;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  FILE *fp;
  s_buf in;
  char  in_data[BUF_SIZE];
  s_str *result;
  env = env_global();
  buf_init(&in, false, BUF_SIZE, in_data);
  fp = file_open(path->ptr.p_pchar, "rb");
  if (! fp)
    return NULL;
  if (! buf_file_open_r(&in, fp)) {
    fclose(fp);
    return NULL;
  }
  file_dir = frame_get_w(env->global_frame, &g_sym___DIR__);
  file_dir_save = *file_dir;
  file_dirname(path, &file_dir->data.str);
  file_path = frame_get_w(env->global_frame, &g_sym___FILE__);
  file_path_save = *file_path;
  file_path->data.str = *path;
  result = ekc3_render_buf_to_str(&in, dest);
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  buf_file_close(&in);
  fclose(fp);
  return result;
}

sw ekc3_render_raw_do_block (s_buf *buf, const s_do_block *do_block)
{
  uw i;
  sw r;
  s_tag result = {0};
  assert(buf);
  assert(do_block);
  i = 1;
  while (i < do_block->count) {
    tag_clean(&result);
    if (! eval_tag(do_block->tag + i, &result))
      return -1;
    i++;
  }
  switch (result.type) {
  case TAG_STR:
    r = buf_write_str(buf, &result.data.str);
    break;
  case TAG_VOID:
    r = 0;
    break;
  default:
    r = buf_inspect_tag(buf, &result);
  }
  tag_clean(&result);
  return r;
}

sw ekc3_render_raw_do_block_size (s_pretty *pretty,
                                  const s_do_block *do_block)
{
  uw i;
  sw r;
  s_tag result = {0};
  assert(pretty);
  assert(do_block);
  i = 1;
  while (i < do_block->count) {
    tag_clean(&result);
    if (! eval_tag(do_block->tag + i, &result))
      return -1;
    i++;
  }
  switch (result.type) {
  case TAG_STR:
    r = buf_write_str_size(pretty, &result.data.str);
    break;
  case TAG_VOID:
    r = 0;
    break;
  default:
    r = buf_inspect_tag_size(pretty, &result);
  }
  tag_clean(&result);
  return r;
}

sw ekc3_render_size (s_pretty *pretty, const p_ekc3 *ekc3)
{
  const s_list *l;
  sw r;
  sw result = 0;
  assert(pretty);
  assert(ekc3);
  l = *ekc3;
  while (l) {
    if (l->tag.type == TAG_PSYM &&
        l->tag.data.psym == sym_1("silent")) {
      l = list_next(l);
      if (! l ||
          l->tag.type != TAG_DO_BLOCK) {
        err_puts("ekc3_render_size: :silent without a do_block");
        assert(! "ekc3_render_size: :silent without a do_block");
        return -1;
      }
      if (! ekc3_eval_silent_do_block(&l->tag.data.do_block)) {
        err_puts("ekc3_render_size: ekc3_eval_silent_do_block");
        assert(! "ekc3_render_size: ekc3_eval_silent_do_block");
        return -1;
      }
    }
    else {
      if ((r = ekc3_render_tag_size(pretty, &l->tag)) < 0) {
        err_puts("ekc3_render_size: ekc3_render_tag_size");
        assert(! "ekc3_render_size: ekc3_render_tag_size");
        return r;
      }
      result += r;
    }
    l = list_next(l);
  }
  return result;
}

sw ekc3_render_tag (s_buf *buf, const s_tag *tag)
{
  const s_do_block *do_block;
  s_str escaped = {0};
  s_str in = {0};
  sw r;
  switch(tag->type) {
  case TAG_DO_BLOCK:
    do_block = &tag->data.do_block;
    if (do_block->count > 1 &&
        do_block->tag->type == TAG_IDENT &&
        do_block->tag->data.ident.sym == sym_1("raw")) {
      if ((r = ekc3_render_raw_do_block(buf, do_block)) < 0) {
        err_puts("ekc3_render_tag: ekc3_render_raw_do_block");
        assert(! "ekc3_render_tag: ekc3_render_raw_do_block");
      }
      return r;
    }
    if (! ekc3_inspect_do_block(&tag->data.do_block, &in)) {
      err_puts("ekc3_render_tag: ekc3_inspect_do_block");
      assert(! "ekc3_render_tag: ekc3_inspect_do_block");
      return -1;
    }
    if (! html_escape(&in, &escaped)) {
      str_clean(&in);
      err_puts("ekc3_render_tag: html_escape");
      assert(! "ekc3_render_tag: html_escape");
      return -1;
    }
    str_clean(&in);
    if ((r = buf_write_str(buf, &escaped)) < 0) {
      err_puts("ekc3_render_tag: buf_write_str 1");
      assert(! "ekc3_render_tag: buf_write_str 1");
    }
    str_clean(&escaped);
    return r;
  case TAG_STR:
    if ((r = buf_write_str(buf, &tag->data.str)) < 0) {
      err_puts("ekc3_render_tag: buf_write_str 2");
      assert(! "ekc3_render_tag: buf_write_str 2");
    }
    return r;
  default:
    break;
  }
  err_write_1("ekc3_render_tag: cannot render ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(": ");
  err_inspect_tag(tag);
  err_write_1("\n");
  return -1;
}

sw ekc3_render_tag_size (s_pretty *pretty, const s_tag *tag)
{
  const s_do_block *do_block;
  s_str escaped = {0};
  s_str in = {0};
  sw r;
  switch(tag->type) {
  case TAG_DO_BLOCK:
    do_block = &tag->data.do_block;
    if (do_block->count > 1 &&
        do_block->tag->type == TAG_IDENT &&
        do_block->tag->data.ident.sym == sym_1("raw")) {
      if ((r = ekc3_render_raw_do_block_size(pretty, do_block)) < 0) {
        err_puts("ekc3_render_tag_size: ekc3_render_raw_do_block");
        assert(! "ekc3_render_tag_size: ekc3_render_raw_do_block");
      }
      return r;
    }
    if (! ekc3_inspect_do_block(&tag->data.do_block, &in)) {
      err_puts("ekc3_render_tag_size: ekc3_inspect_do_block");
      assert(! "ekc3_render_tag_size: ekc3_inspect_do_block");
      return -1;
    }
    if (! html_escape(&in, &escaped)) {
      str_clean(&in);
      err_puts("ekc3_render_tag_size: html_escape");
      assert(! "ekc3_render_tag_size: html_escape");
      return -1;
    }
    str_clean(&in);
    if ((r = buf_write_str_size(pretty, &escaped)) < 0) {
      err_puts("ekc3_render_tag_size: buf_write_str 1");
      assert(! "ekc3_render_tag_size: buf_write_str 1");
    }
    str_clean(&escaped);
    return r;
  case TAG_STR:
    if ((r = buf_write_str_size(pretty, &tag->data.str)) < 0) {
      err_puts("ekc3_render_tag_size: buf_write_str 2");
      assert(! "ekc3_render_tag_size: buf_write_str 2");
    }
    return r;
  default:
    break;
  }
  err_write_1("ekc3_render_tag_size: cannot render ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(": ");
  err_inspect_tag(tag);
  err_write_1("\n");
  return -1;
}

s_str * ekc3_render_to_str (const p_ekc3 *ekc3, s_str *dest)
{
  
  s_buf out;
  s_pretty pretty = {0};
  sw r;
  assert(ekc3);
  assert(dest);
  if ((r = ekc3_render_size(&pretty, ekc3)) < 0)
    return NULL;
  if (! buf_init_alloc(&out, r))
    return NULL;
  if ((r = ekc3_render(&out, ekc3)) < 0) {
    buf_clean(&out);
    return NULL;
  }
  buf_to_str(&out, dest);
  return dest;
}
*/
