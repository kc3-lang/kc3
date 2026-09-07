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
#include "../libkc3/assert.h"
#include "../libkc3/buf.h"
#include "../libkc3/buf_file.h"
#include "../libkc3/buf_parse.h"
#include "../libkc3/character.h"
#include "../libkc3/file.h"
#include "../libkc3/str.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "to_c.h"

sw to_c_buf (s_to_c *context, s_buf *in)
{
  sw r;
  sw result = 0;
  s_tag tag = {0};
  assert(context);
  assert(in);
  while (1) {
    if ((r = buf_ignore_spaces(in)) < 0)
      break;
    if ((r = buf_parse_comments(in)) < 0)
      break;
    if ((r = buf_parse_tag(in, &tag)) < 0)
      break;
    if (! r)
      continue;
    if ((r = to_c_tag(context, &tag)) < 0) {
      tag_clean(&tag);
      return r;
    }
    result += r;
    tag_clean(&tag);
  }
  return result;
}

void to_c_clean (s_to_c *context)
{
  assert(context);
  if (context->c_buf.user_ptr)
    buf_file_close(&context->c_buf);
  buf_clean(&context->c_buf);
  if (context->c_fp)
    fclose(context->c_fp);
  context->c_buf = (s_buf) {0};
  context->c_fp = NULL;
  str_clean(&context->c_path);
  if (context->h_buf.user_ptr)
    buf_file_close(&context->h_buf);
  buf_clean(&context->h_buf);
  if (context->h_fp)
    fclose(context->h_fp);
  context->h_buf = (s_buf) {0};
  context->h_fp = NULL;
  str_clean(&context->h_ifndef);
  str_clean(&context->h_path);
  str_clean(&context->name);
}

sw to_c_defmodule_do_block (s_to_c *context, const s_do_block *do_block)
{
  uw i;
  sw r;
  sw result = 0;
  assert(context);
  assert(do_block);
  i = 0;
  while (i < do_block->count) {
    if ((r = to_c_defmodule_tag(context, do_block->tag + i)) < 0)
      return r;
    result += r;
    i++;
  }
  return result;
}

s_str * to_c_defmodule_sym_to_str (const s_sym *sym, s_str *dest)
{
  char a[1024];
  character c;
  s_str in = {0};
  s_buf out;
  sw r;
  in.size = sym->str.size;
  in.ptr = sym->str.ptr;
  buf_init(&out, false, sizeof(a), a);
  while ((r = str_read_character_utf8(&in, &c)) > 0) {
    if (c == '.')
      c = '_';
    else
      c = character_to_upper(c);
    if ((r = buf_write_character_utf8(&out, c)) <= 0)
      return NULL;
  }
  if ((r = buf_read_to_str(&out, dest)) <= 0)
    return NULL;
  return dest;
}

sw to_c_defmodule_tag (s_to_c *context, s_tag *tag)
{
  assert(context);
  assert(tag);
  err_puts("to_c_defmodule_tag: warning: not implemented");
  return 0;
}

sw to_c_file (s_to_c *context, s_str *in)
{
  s_buf buf = {0};
  FILE *fp = NULL;
  sw r;
  assert(context);
  assert(in);
  if (! (fp = file_open(in, "rb")))
    return -1;
  if (! buf_init_alloc(&buf, BUF_SIZE)) {
    fclose(fp);
    return -1;
  }
  if (! buf_file_open_r(&buf, fp)) {
    buf_clean(&buf);
    fclose(fp);
    return -1;
  }
  r = to_c_buf(context, &buf);
  buf_file_close(&buf);
  buf_clean(&buf);
  fclose(fp);
  return r;
}

s_to_c * to_c_init (s_to_c *context)
{
  *context = (s_to_c) {0};
  return context;
}

s_to_c * to_c_open (s_to_c *context, const s_str *path)
{
  u32 i;
  assert(context);
  assert(path);
  if (! path->size)
    return NULL;
  if (! str_init_alloc(&context->c_path, path->size + 2))
    goto ko;
  memcpy(context->c_path.free.p_pchar, path->ptr.p_pchar, path->size);
  memcpy(context->c_path.free.p_pchar + path->size, ".c", 3);
  if (! (context->c_fp = file_open(&context->c_path, "wb")))
    goto ko;
  if (! buf_init_alloc(&context->c_buf, BUF_SIZE) ||
      ! buf_file_open_w(&context->c_buf, context->c_fp))
    goto ko;
  if (! str_init_alloc(&context->h_path, path->size + 2))
    goto ko;
  memcpy(context->h_path.free.p_pchar, path->ptr.p_pchar, path->size);
  memcpy(context->h_path.free.p_pchar + path->size, ".h", 3);
  if (! (context->h_fp = file_open(&context->h_path, "wb")))
    goto ko;
  if (! buf_init_alloc(&context->h_buf, BUF_SIZE) ||
      ! buf_file_open_w(&context->h_buf, context->h_fp))
    goto ko;
  i = path->size;
  while (i > 0 && path->ptr.p_pchar[i - 1] != '/')
    i--;
  if (! str_init_alloc_copy(&context->name, path->size - i,
                            path->ptr.p_pchar + i))
    goto ko;
  return context;
 ko:
  to_c_clean(context);
  return NULL;
}

sw to_c_tag (s_to_c *context, s_tag *tag)
{
  assert(context);
  assert(tag);
  switch (tag->type) {
  case TAG_PCALL:
    return to_c_tag_call(context, tag->data.td_pcall);
  default:
    err_write_1("to_c_tag: ignored tag: ");
    err_inspect_tag(tag);
    err_write_1("\n");
    return 0;
  }
}

sw to_c_tag_call (s_to_c *context, s_call *call)
{
  s_tag *tag_do_block;
  s_tag *tag_sym;
  assert(context);
  assert(call);
  if ((call->ident.module == NULL ||
       call->ident.module == &g_sym_KC3) &&
      call->ident.sym == &g_sym_defmodule &&
      call->arguments &&
      (tag_sym = &call->arguments->tag) &&
      tag_sym->type == TAG_PSYM &&
      call->arguments->next.type == TAG_PLIST &&
      call->arguments->next.data.td_plist &&
      (tag_do_block = &call->arguments->next.data.td_plist->tag) &&
      tag_do_block->type == TAG_DO_BLOCK)
    return to_c_tag_call_defmodule(context, tag_sym->data.td_psym,
                                   &tag_do_block->data.td_do_block);
  return 0;
}

sw to_c_tag_call_defmodule (s_to_c *context,
                            const s_sym *sym,
                            const s_do_block *do_block)
{
  sw r;
  sw result = 0;
  assert(context);
  assert(sym);
  assert(do_block);
  if (! to_c_defmodule_sym_to_str(sym, &context->h_ifndef))
    return -1;
  if ((r = buf_write_1(&context->h_buf,
                       "/* This file was generated from KC3. */\n"
                       "#ifndef KC3_TO_C_DEFMODULE_")) <= 0)
    return r;
  result += r;
  if ((r = buf_write_str(&context->h_buf, &context->h_ifndef)) <= 0)
    return r;
  
  if ((r = buf_write_1(&context->h_buf, "_H\n\n")) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(&context->c_buf,
                       "/* This file was generated from KC3. */\n"
                       "#include \"")) <= 0)
    return r;
  result += r;
  if ((r = buf_write_str(&context->c_buf, &context->name)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(&context->c_buf, ".h\"\n\n")) <= 0)
    return r;
  result += r;
  if ((r = to_c_defmodule_do_block(context, do_block)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(&context->h_buf, "#endif /* KC3_TO_C_DEFMODULE_")) <= 0)
    return r;
  result += r;
  if ((r = buf_write_str(&context->h_buf, &context->h_ifndef)) <= 0)
    return r;
  if ((r = buf_write_1(&context->h_buf, "_H */\n")) <= 0)
    return r;
  result += r;
  return result;
}
