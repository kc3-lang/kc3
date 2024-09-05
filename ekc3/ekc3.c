/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <libkc3/kc3.h>
#include "ekc3.h"
#include "html.h"

s_list *** ekc3_append_and_empty_buf (s_list ***tail, s_buf *buf)
{
  s_str str;
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(buf);
  if (! buf_read_to_str(buf, &str)) {
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
  *tail = &(**tail)->next.data.list;
  return tail;
}

s_list *** ekc3_append_block (s_list ***tail, s_block *block)
{
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(block);
  **tail = list_new(NULL);
  if (! **tail)
    return NULL;
  (**tail)->tag.type = TAG_BLOCK;
  (**tail)->tag.data.block = *block;
  *tail = &(**tail)->next.data.list;
  return tail;
}

s_list *** ekc3_append_silent_block (s_list ***tail, s_block *block)
{
  assert(tail);
  assert(*tail);
  assert(! **tail);
  assert(block);
  if (! ekc3_append_sym(tail, sym_1("silent")))
    return NULL;
  if (! ekc3_append_block(tail, block))
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
  *tail = &(**tail)->next.data.list;
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
  (**tail)->tag.type = TAG_SYM;
  (**tail)->tag.data.sym = sym;
  *tail = &(**tail)->next.data.list;
  return tail;
}

sw ekc3_buf_parse (s_buf *buf, p_ekc3 *dest)
{
  s_block block;
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
      r = ekc3_buf_parse_kc3_block(buf, &block);
      if (r <= 0) {
        err_puts("ekc3_buf_parse: ekc3_buf_parse_kc3_block");
        assert(! "ekc3_buf_parse: ekc3_buf_parse_kc3_block");
        goto restore;
      }
      if (! ekc3_append_block(&tail, &block)) {
        err_puts("ekc3_buf_parse: ekc3_append_block");
        assert(! "ekc3_buf_parse: ekc3_append_block");
        goto ko;
      }
      continue;
    }
    r = ekc3_buf_peek_kc3_silent_block(buf);
    if (r < 0) {
      err_puts("ekc3_buf_parse: ekc3_buf_peek_kc3_silent_block");
      assert(! "ekc3_buf_parse: ekc3_buf_peek_kc3_silent_block");
      goto ko;
    }
    if (r > 0) {
      if (! ekc3_append_and_empty_buf(&tail, &str_buf)) {
        err_puts("ekc3_buf_parse: ekc3_append_and_empty_buf 2");
        assert(! "ekc3_buf_parse: ekc3_append_and_empty_buf 2");
        goto ko;
      }
      r = ekc3_buf_parse_kc3_silent_block(buf, &block);
      if (r <= 0) {
        err_puts("ekc3_buf_parse: ekc3_buf_parse_kc3_silent_block");
        assert(! "ekc3_buf_parse: ekc3_buf_parse_kc3_silent_block");
        goto restore;
      }
      if (! ekc3_append_silent_block(&tail, &block)) {
        err_puts("ekc3_buf_parse: ekc3_append_silent_block");
        assert(! "ekc3_buf_parse: ekc3_append_silent_block");
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

sw ekc3_buf_parse_kc3_block (s_buf *buf, s_block *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_list *list;
  s_list **tail;
  s_block tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  list = NULL;
  tail = &list;
  while (1) {
    r = buf_ignore_spaces(buf);
    if (r < 0) {
      err_puts("ekc3_buf_parse_kc3_block: buf_ignore_spaces 1");
      assert(! "ekc3_buf_parse_kc3_block: buf_ignore_spaces 1");
      goto clean;
    }
    /*
    if (! r) {
      err_puts("ekc3_buf_parse_kc3_block: buf_ignore_spaces 2");
      assert(! "ekc3_buf_parse_kc3_block: buf_ignore_spaces 2");
      r = -1;
      goto clean;
    }
    */
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
      err_puts("ekc3_buf_parse_kc3_block: buf_parse_tag < 0");
      assert(! "ekc3_buf_parse_kc3_block: buf_parse_tag < 0");
      goto clean;
    }
    if (! r) {
      *tail = list_delete(*tail);
      err_puts("ekc3_buf_parse_kc3_block: buf_parse_tag = 0");
      assert(! "ekc3_buf_parse_kc3_block: buf_parse_tag = 0");
      break;
    }
    result += r;
    tail = &(*tail)->next.data.list;
  }
  if (! block_init_from_list(&tmp, (const s_list * const*) &list)) {
    list_delete_all(list);
    err_puts("ekc3_buf_parse_kc3_block: block_init_from_list");
    assert(! "ekc3_buf_parse_kc3_block: block_init_from_list");
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

sw ekc3_buf_parse_kc3_silent_block (s_buf *buf, s_block *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  r = buf_ignore_spaces(buf);
  if (r < 0) {
    err_puts("ekc3_buf_parse_kc3_silent_block: buf_ignore_spaces 1");
    assert(! "ekc3_buf_parse_kc3_silent_block: buf_ignore_spaces 1");
    goto clean;
  }
  r = buf_read_1(buf, "<%");
  if (r < 0) {
    err_puts("ekc3_buf_parse_kc3_silent_block: buf_read_1 < 0");
    assert(! "ekc3_buf_parse_kc3_silent_block: buf_read_1 < 0");
    goto restore;
  }
  if (! r) {
    err_puts("ekc3_buf_parse_kc3_silent_block: buf_read_1 = 0");
    assert(! "ekc3_buf_parse_kc3_silent_block: buf_read_1 = 0");
    goto restore;
  }
  result += r;
  r = buf_ignore_spaces(buf);
  if (r <= 0) {
    err_puts("ekc3_buf_parse_kc3_silent_block: buf_ignore_spaces 2");
    assert(! "ekc3_buf_parse_kc3_silent_block: buf_ignore_spaces 2");
    goto restore;
  }
  result += r;
  r = ekc3_buf_parse_kc3_block(buf, dest);
  if (r <= 0) {
    err_puts("ekc3_buf_parse_kc3_silent_block: buf_parse_kc3_block");
    assert(! "ekc3_buf_parse_kc3_silent_block: buf_parse_kc3_block");
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

sw ekc3_buf_peek_kc3_silent_block (s_buf *buf)
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

bool ekc3_eval_silent_block (const s_block *block)
{
  uw i = 0;
  s_tag result = {0};
  assert(block);
  while (i < block->count) {
    if (! eval_tag(block->tag + i, &result))
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

s_str * ekc3_inspect_block (const s_block *block, s_str *dest)
{
  uw i = 0;
  s_tag result = {0};
  const s_sym *type;
  assert(block);
  while (i < block->count) {
    tag_clean(&result);
    if (! eval_tag(block->tag + i, &result))
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

sw ekc3_render (const p_ekc3 *ekc3)
{
  const s_list *l;
  sw r;
  sw result = 0;
  assert(ekc3);
  l = *ekc3;
  while (l) {
    if (l->tag.type == TAG_SYM &&
        l->tag.data.sym == sym_1("silent")) {
      l = list_next(l);
      if (! l ||
          l->tag.type != TAG_BLOCK) {
        err_puts("ekc3_render: :silent without a block");
        assert(! "ekc3_render: :silent without a block");
        return -1;
      }
      if (! ekc3_eval_silent_block(&l->tag.data.block)) {
        err_puts("ekc3_render: ekc3_eval_silent_block");
        assert(! "ekc3_render: ekc3_eval_silent_block");
        return -1;
      }
    }
    else {
      if (! (r = ekc3_render_tag(&l->tag)))
        return r;
      result += r;
    }
    l = list_next(l);
  }
  return result;
}

sw ekc3_render_buf (s_buf *in)
{
  p_ekc3 ekc3;
  sw r;
  assert(in);
  ekc3_init(&ekc3);
  r = ekc3_buf_parse(in, &ekc3);
  if (r < 0) {
    err_puts("ekc3_render_buf: ekc3_buf_parse");
    assert(! "ekc3_render_buf: ekc3_buf_parse");
    return r;
  }
  r = ekc3_render(&ekc3);
  ekc3_clean(&ekc3);
  return r;
}

s_str * ekc3_render_file (const s_str *path, s_str *dest)
{
  s_buf buf;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  FILE *fp;
  s_buf in;
  char  in_data[BUF_SIZE];
  sw r;
  s_str tmp;
  buf_init(&in, false, BUF_SIZE, in_data);
  fp = file_open(path->ptr.pchar, "rb");
  if (! fp)
    return -1;
  if (! buf_file_open_r(&in, fp)) {
    fclose(fp);
    return -1;
  }
  file_dir = frame_get_w(&g_kc3_env.global_frame, &g_sym___DIR__);
  file_dir_save = *file_dir;
  file_dirname(path, &file_dir->data.str);
  file_path = frame_get_w(&g_kc3_env.global_frame, &g_sym___FILE__);
  file_path_save = *file_path;
  file_path->data.str = *path;
  if (! ekc3_render_buf(&in, &tmp))
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  buf_file_close(&in);
  fclose(fp);
  return r;
}

sw ekc3_render_raw_block (const s_block *block)
{
  uw i;
  sw r;
  s_tag result = {0};
  assert(block);
  i = 1;
  while (i < block->count) {
    tag_clean(&result);
    if (! eval_tag(block->tag + i, &result))
      return -1;
    i++;
  }
  switch (result.type) {
  case TAG_STR:
    r = io_write_str(&result.data.str);
    break;
  default:
    r = io_inspect_tag(&result);
  }
  tag_clean(&result);
  return r;
}

sw ekc3_render_tag (const s_tag *tag)
{
  const s_block *block;
  s_str escaped = {0};
  s_str in = {0};
  sw r;
  switch(tag->type) {
  case TAG_BLOCK:
    block = &tag->data.block;
    if (block->count > 1 &&
        block->tag->type == TAG_IDENT &&
        block->tag->data.ident.sym == sym_1("raw")) {
      if ((r = ekc3_render_raw_block(block)) < 0) {
        err_puts("ekc3_render_tag: ekc3_render_raw_block");
        assert(! "ekc3_render_tag: ekc3_render_raw_block");
      }
      return r;
    }
    if (! ekc3_inspect_block(&tag->data.block, &in)) {
      err_puts("ekc3_render_tag: ekc3_render_block_to_str");
      assert(! "ekc3_render_tag: ekc3_render_block_to_str");
      return -1;
    }
    if (! html_escape(&in, &escaped)) {
      str_clean(&in);
      err_puts("ekc3_render_tag: html_escape");
      assert(! "ekc3_render_tag: html_escape");
      return -1;
    }
    str_clean(&in);
    if ((r = io_write_str(&escaped)) < 0) {
      err_puts("ekc3_render_tag: io_write_str 1");
      assert(! "ekc3_render_tag: io_write_str 1");
    }
    str_clean(&escaped);
    return r;
  case TAG_STR:
    if ((r = io_write_str(&tag->data.str)) < 0) {
      err_puts("ekc3_render_tag: io_write_str 2");
      assert(! "ekc3_render_tag: io_write_str 2");
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
