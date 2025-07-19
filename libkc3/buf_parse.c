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
#include <string.h>
#include "../libtommath/tommath.h"
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "buf_parse_uw.h"
#include "buf_save.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "character.h"
#include "complex.h"
#include "cow.h"
#include "data.h"
#include "do_block.h"
#include "env.h"
#include "fact.h"
#include "fn.h"
#include "fn_clause.h"
#include "frame.h"
#include "ident.h"
#include "inspect.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "op.h"
#include "ops.h"
#include "pvar.h"
#include "ratio.h"
#include "special_operator.h"
#include "str.h"
#include "struct.h"
#include "sym.h"
#include "tag.h"
#include "time.h"
#include "tuple.h"
#include "u16.h"

sw buf_parse_array_data_rec (s_buf *buf, s_array *dest, uw *address,
                             s_tag **tag, uw dimension);
sw buf_parse_array_dimensions_rec (s_buf *buf, s_array *dest,
                                   uw *address,uw dimension);
sw buf_peek_array_dimension_count (s_buf *buf, s_array *dest);

sw buf_parse_array (s_buf *buf, s_array *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_array tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_paren_sym(buf, &tmp.array_type)) <= 0)
    goto clean;
  result += r;
  if (! sym_is_array_type(tmp.array_type)) {
    r = 0;
    goto restore;
  }
  tmp.element_type = sym_array_type(tmp.array_type);
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_peek_array_dimension_count(buf, &tmp)) <= 0 ||
      (tmp.dimension &&
       (r = buf_peek_array_dimensions(buf, &tmp)) <= 0) ||
      (r = buf_parse_array_data(buf, &tmp)) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  array_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_array_data (s_buf *buf, s_array *dest)
{
  uw *address = NULL;
  uw i;
  sw r = 0;
  sw result = 0;
  s_tag *tag;
  s_array tmp;
  assert(buf);
  assert(dest);
  if (! dest->dimension) {
    if ((r = buf_read_1(buf, "{")) <= 0)
      return r;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      return r;
    result += r;
    if ((r = buf_read_1(buf, "}")) <= 0)
      return r;
    result += r;
    return result;
  }
  tmp = *dest;
  address = alloc(tmp.dimension * sizeof(sw));
  if (! address)
    return -1;
  tmp.count = 1;
  i = 0;
  while (i < tmp.dimension) {
    tmp.count *= tmp.dimensions[i].count;
    i++;
  }
  tmp.size = tmp.dimensions[0].count * tmp.dimensions[0].item_size;
  tmp.tags = alloc(tmp.count * sizeof(s_tag));
  if (! tmp.tags) {
    free(address);
    return -1;
  }
  tag = tmp.tags;
  if ((r = buf_parse_array_data_rec(buf, &tmp, address, &tag,
                                    0)) <= 0) {
    err_write_1("buf_parse_array_data: buf_parse_array_data_rec: ");
    err_inspect_sw_decimal(&r);
    err_write_1("\n");
    goto restore;
  }
  *dest = tmp;
  goto clean;
 restore:
  free(tmp.tags);
 clean:
  free(address);
  return r;
}

sw buf_parse_array_data_rec (s_buf *buf, s_array *dest, uw *address,
                             s_tag **tag, uw dimension)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_array tmp;
  assert(buf);
  assert(dest);
  tmp = *dest;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0) {
    err_puts("buf_parse_array_data_rec: {");
    goto clean;
  }
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0) {
    err_puts("buf_parse_array_data_rec: 1");
    goto restore;
  }
  result += r;
  if (address) {
    address[dimension] = 0;
    while (1) {
      if (dimension == tmp.dimension - 1) {
        if ((r = buf_parse_tag(buf, *tag)) < 0) {
          err_puts("buf_parse_array_data_rec: parse");
          goto clean;
        }
        result += r;
        (*tag)++;
      }
      else {
        if ((r = buf_parse_array_data_rec(buf, &tmp, address, tag,
                                          dimension + 1)) <= 0) {
          err_puts("buf_parse_array_data_rec: buf_parse_array_data_rec");
          goto restore;
        }
        result += r;
      }
      address[dimension]++;
      if ((r = buf_ignore_spaces(buf)) < 0) {
        err_puts("buf_parse_array_data_rec: 2");
        goto restore;
      }
      result += r;
      if ((r = buf_read_1(buf, ",")) < 0) {
        err_puts("buf_parse_array_data_rec: 3");
        goto restore;
      }
      result += r;
      if (! r)
        break;
      if ((r = buf_ignore_spaces(buf)) < 0) {
        err_puts("buf_parse_array_data_rec: 4");
        goto restore;
      }
      result += r;
    }
    if ((r = buf_ignore_spaces(buf)) < 0) {
      err_puts("buf_parse_array_data_rec: 4");
      goto restore;
    }
    result += r;
  }
  if ((r = buf_read_1(buf, "}")) <= 0) {
    err_puts("buf_parse_array_data_rec: }");
    goto restore;
  }
  result += r;
  if (tmp.dimensions[dimension].count != address[dimension]) {
    err_write_1("buf_parse_array_dimensions_rec: dimension mismatch: ");
    err_inspect_uw(&dimension);
    err_write_1("\n");
    assert(! "buf_parse_array_dimensions_rec: dimension mismatch");
    r = -1;
    goto restore;
  }
  r = result;
  goto clean;
 restore:
  err_puts("buf_parse_array_data_rec: restore");
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_array_dimension_count (s_buf *buf, s_array *dest)
{
  sw r = 1;
  sw result = 0;
  s_buf_save save;
  s_array tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "}")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    dest->dimension = 0;
    r = result;
    goto clean;
  }
  tmp = *dest;
  tmp.dimension = 1;
  while (1) {
    if ((r = buf_read_1(buf, "{")) < 0)
      goto restore;
    if (! r)
      break;
    result += r;
    tmp.dimension++;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  tmp.dimensions = alloc(tmp.dimension *
                         sizeof(s_array_dimension));
  if (! tmp.dimensions)
    return -1;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_array_dimensions (s_buf *buf, s_array *dest)
{
  uw *address;
  sw r;
  uw size;
  s_array tmp;
  assert(buf);
  assert(dest);
  tmp = *dest;
  if (! sym_type_size(&tmp.element_type, &size))
    return -1;
  if (! size) {
    err_puts("buf_parse_array_dimensions: zero item size");
    assert(! "buf_parse_array_dimensions: zero item size");
    return -1;
  }
  address = alloc(tmp.dimension * sizeof(sw));
  if (! address)
    return -1;
  tmp.dimensions[tmp.dimension - 1].item_size = size;
  if ((r = buf_parse_array_dimensions_rec(buf, &tmp, address,
                                          0)) < 0) {
    err_write_1("buf_parse_array_dimensions:"
                " buf_parse_array_dimensions_rec: ");
    err_inspect_sw_decimal(&r);
    err_write_1("\n");
    goto clean;
  }
  *dest = tmp;
 clean:
  free(address);
  return r;
}

sw buf_parse_array_dimensions_rec (s_buf *buf, s_array *dest,
                                   uw *address, uw dimension)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag tag;
  s_array tmp;
  assert(buf);
  assert(dest);
  assert(address);
  tmp = *dest;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0) {
    err_puts("buf_parse_array_dimensions_rec: {");
    goto clean;
  }
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0) {
    err_puts("buf_parse_array_dimensions_rec: 1");
    goto restore;
  }
  result += r;
  address[dimension] = 0;
  while (1) {
    if (dimension == dest->dimension - 1) {
      if ((r = buf_parse_tag(buf, &tag)) <= 0) {
        err_puts("buf_parse_array_dimensions_rec: buf_parse_tag");
        goto clean;
      }
      if (r) {
        result += r;
        tag_clean(&tag);
      }
    }
    else {
      if ((r = buf_parse_array_dimensions_rec(buf, &tmp, address,
                                              dimension + 1)) <= 0) {
        err_puts("buf_parse_array_dimensions_rec:"
                 " buf_parse_array_dimensions_rec");
        goto restore;
      }
      result += r;
    }
    address[dimension]++;
    if ((r = buf_ignore_spaces(buf)) < 0) {
      err_puts("buf_parse_array_dimensions_rec: 2");
      goto restore;
    }
    result += r;
    if ((r = buf_read_1(buf, ",")) < 0) {
      err_puts("buf_parse_array_dimensions_rec: 3");
      goto restore;
    }
    result += r;
    if (! r)
      break;
    if ((r = buf_ignore_spaces(buf)) < 0) {
      err_puts("buf_parse_array_dimensions_rec: 4");
      goto restore;
    }
    result += r;
  }
  if ((r = buf_read_1(buf, "}")) <= 0) {
    err_puts("buf_parse_array_dimensions_rec: }");
    goto restore;
  }
  result += r;
  if (! tmp.dimensions[dimension].count) {
    tmp.dimensions[dimension].count = address[dimension];
    if (dimension < tmp.dimension - 1)
      tmp.dimensions[dimension].item_size = tmp.dimensions[dimension + 1].count * tmp.dimensions[dimension + 1].item_size;
  }
  else if (tmp.dimensions[dimension].count != address[dimension]) {
    err_write_1("buf_parse_array_dimensions_rec: dimension mismatch: ");
    err_inspect_uw(&dimension);
    err_write_1("\n");
    assert(! "buf_parse_array_dimensions_rec: dimension mismatch");
    r = -1;
    goto restore;
  }
  r = result;
  goto clean;
 restore:
  err_puts("buf_parse_array_dimensions_rec: restore");
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_do_block (s_buf *buf, s_do_block *do_block)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  bool short_form = false;
  assert(buf);
  assert(do_block);
  buf_save_init(buf, &save);
  if ((r = buf_read_sym(buf, &g_sym_do)) < 0)
    goto clean;
  if (! r) {
    short_form = true;
    if ((r = buf_read_1(buf, "{")) <= 0)
      goto clean;
  }
  result += r;
  if ((r = buf_parse_comments(buf)) < 0) {
    err_puts("buf_parse_do_block: buf_parse_comments");
    assert(! "buf_parse_do_block: buf_parse_comments");
    goto restore;
  }
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0) {
    err_puts("buf_parse_do_block: buf_ignore_spaces");
    assert(! "buf_parse_do_block: buf_ignore_spaces");
    goto restore;
  }
  result += r;
  if ((r = buf_parse_do_block_inner(buf, short_form, do_block)) < 0) {
    err_puts("buf_parse_do_block: buf_parse_do_block_inner < 0");
    assert(! "buf_parse_do_block: buf_parse_do_block_inner < 0");
    goto restore;
  }
  if (! r)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_do_block_inner (s_buf *buf, bool short_form,
                             s_do_block *do_block)
{
  s_list **i;
  sw j;
  s_list *k;
  sw l;
  s_list *list = 0;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag tag;
  s_do_block tmp;
  i = &list;
  *i = NULL;
  buf_save_init(buf, &save);
  while (1) {
    if (short_form) {
      if ((r = buf_read_1(buf, "}")) < 0) {
        err_puts("buf_parse_do_block_inner: buf_read_1 \"}\"");
        err_inspect_buf(buf);
        err_write_1("\n");
        assert(! "buf_parse_do_block_inner: buf_read_1 \"}\"");
        goto restore;
      }
    }
    else if ((r = buf_read_sym(buf, &g_sym_end)) < 0) {
      err_puts("buf_parse_do_block_inner: buf_read_sym :end");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "buf_parse_do_block_inner: buf_read_sym :end");
      goto restore;
    }
    if (r > 0)
      goto ok;
    if ((r = buf_parse_tag(buf, &tag)) < 0) {
      err_puts("buf_parse_do_block_inner: buf_parse_tag");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "buf_parse_do_block_inner: buf_parse_tag");
      goto restore;
    }
    result += r;
    if (r > 0) {
      *i = list_new(NULL);
      if (! *i) {
        err_puts("buf_parse_do_block_inner: list_new");
        assert(! "buf_parse_do_block_inner: list_new");
        goto restore;
      }
      (*i)->tag = tag;
      i = &(*i)->next.data.list;
    }
    if ((r = buf_parse_comments(buf)) < 0) {
      err_puts("buf_parse_do_block_inner: buf_parse_comments 1");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "buf_parse_do_block_inner: buf_parse_comments 1");
      goto restore;
    }
    result += r;
    if ((r = buf_ignore_spaces_but_newline(buf)) < 0) {
      err_puts("buf_parse_do_block_inner:"
               " buf_ignore_spaces_but_newline 1");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(!("buf_parse_do_block_inner:"
               " buf_ignore_spaces_but_newline 1"));
      goto restore;
    }
    result += r;
    if (short_form) {
      if ((r = buf_read_1(buf, "}")) < 0) {
        err_puts("buf_parse_do_block_inner: buf_read_1 \"}\" < 0");
        assert(! "buf_parse_do_block_inner: buf_read_1 \"}\" < 0");
        goto restore;
      }
    }
    else if ((r = buf_read_sym(buf, &g_sym_end)) < 0) {
      err_puts("buf_parse_do_block_inner: buf_read_sym :end < 0");
      assert(! "buf_parse_do_block_inner: buf_read_sym :end < 0");
      goto restore;
    }
    if (r > 0)
      goto ok;
    if ((r = buf_read_1(buf, "\n")) < 0) {
      err_write_1("buf_parse_do_block_inner: line ");
      err_inspect_sw_decimal(&buf->line);
      err_puts(": missing separator: ");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "buf_parse_do_block_inner: missing separator");
      goto restore;
    }
    if (! r) {
      if ((r = buf_read_1(buf, ";")) <= 0) {
        err_write_1("buf_parse_do_block_inner: line ");
        err_inspect_sw_decimal(&buf->line);
        err_puts(": missing separator: ");
        err_inspect_buf(buf);
        err_write_1("\n");
        assert(! "buf_parse_do_block_inner: missing separator");
        goto restore;
      }
    }
    result += r;
    if ((r = buf_parse_comments(buf)) < 0) {
      err_puts("buf_parse_do_block_inner: buf_parse_comments 2");
      assert(! "buf_parse_do_block_inner: buf_parse_comments 2");
      goto restore;
    }
    result += r;
    if ((r = buf_ignore_spaces_but_newline(buf)) < 0) {
      err_puts("buf_parse_do_block_inner:"
               " buf_ignore_spaces_but_newline 2");
      assert(!("buf_parse_do_block_inner:"
               " buf_ignore_spaces_but_newline 2"));
      goto restore;
    }
    result += r;
  }
  r = 0;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  if (list)
    list_delete_all(list);
  buf_save_clean(buf, &save);
  return r;
 ok:
  result += r;
  j = list_length(list);
  if (! do_block_init(&tmp, j)) {
    r = -2;
    goto clean;
  }
  k = list;
  l = 0;
  while (l < j) {
    tmp.tag[l] = k->tag;
    tag_init_void(&k->tag);
    k = list_next(k);
    l++;
  }
  tmp.short_form = short_form;
  *do_block = tmp;
  r = result;
  goto clean;
}

sw buf_parse_bool (s_buf *buf, bool *p)
{
  character c;
  sw r;
  s_buf_save save;
  bool value = 2;
  assert(buf);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "true")) < 0)
    goto clean;
  else if (r > 0)
    value = true;
  else {
    if ((r = buf_read_1(buf, "false")) < 0)
      goto clean;
    else if (r > 0)
      value = false;
  }
  if (r == 0)
    goto clean;
  if (buf_peek_character_utf8(buf, &c) > 0 &&
      ! ident_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
  assert(value == true || value == false);
  *p = value;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_brackets (s_buf *buf, s_call *dest)
{
  s_tag *arg_addr;
  uw address = 0;
  s_list *addr = NULL;
  s_list **addr_last = &addr;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  tmp.arguments = list_new(list_new(NULL));
  arg_addr = &(list_next(tmp.arguments)->tag);
  if ((r = buf_parse_tag_ident(buf, &tmp.arguments->tag)) <= 0)
    goto restore;
  result += r;
  while (1) {
    if ((r = buf_read_1(buf, "[")) < 0)
      goto restore;
    if (! r)
      break;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    *addr_last = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*addr_last)->tag)) <= 0)
      goto restore;
    result += r;
    addr_last = &(*addr_last)->next.data.list;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "]")) <= 0)
      goto restore;
    result += r;
    address++;
  }
  if (! address) {
    goto restore;
  }
  tmp.ident.module = &g_sym_KC3;
  tmp.ident.sym = &g_sym__brackets;
  arg_addr->type = TAG_LIST;
  arg_addr->data.list = addr;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  r = 0;
  buf_save_restore_rpos(buf, &save);
  call_clean(&tmp);
  list_delete_all(addr);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call (s_buf *buf, s_call *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  call_init(&tmp);
  if ((r = buf_parse_ident(buf, &tmp.ident)) <= 0) {
    call_clean(&tmp);
    goto clean;
  }
  result += r;
  if ((r = buf_parse_call_args_paren(buf, &tmp)) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_access (s_buf *buf, s_call *dest)
{
  s_list **k;
  s_list  *key = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if (! call_init_op(&tmp))
    return -1;
  tmp.ident.module = &g_sym_KC3;
  tmp.ident.sym = &g_sym_access;
  r = buf_parse_tag_ident(buf, &tmp.arguments->tag);
  if (r <= 0)
    goto restore;
  result += r;
  k = &key;
  while (1) {
    if ((r = buf_read_1(buf, ".")) <= 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    *k = list_new(NULL);
    r = buf_parse_tag_ident_sym(buf, &(*k)->tag);
    k = &(*k)->next.data.list;
    if (r <= 0)
      goto restore;
    result += r;
    if ((r = buf_peek_1(buf, ".")) < 0)
      goto restore;
    if (r == 0)
      break;
  }
  list_next(tmp.arguments)->tag.type = TAG_LIST;
  list_next(tmp.arguments)->tag.data.list = key;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  list_delete_all(key);
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_args_paren (s_buf *buf, s_call *dest)
{
  s_list **args;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag tag;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  args = &dest->arguments;
  *args = NULL;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, ")")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    r = result;
    goto clean;
  }
  while (1) {
    if ((r = buf_parse_tag(buf, &tag)) <= 0)
      goto restore;
    result += r;
    *args = list_new(NULL);
    (*args)->tag = tag;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, ")")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      r = result;
      goto clean;
    }
    if ((r = buf_read_1(buf, ",")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      args = &(*args)->next.data.list;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      continue;
    }
    if ((r = buf_read_1(buf, ",|")) < 0) /* apply */
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_parse_tag(buf, &(*args)->next)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, ")")) <= 0)
        goto restore;
      result += r;
      r = result;
      goto clean;
    }
    goto restore;
  }
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_op (s_buf *buf, s_call *dest)
{
  s_ident ident;
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if (! call_init_op(&tmp)) {
    r = -1;
    goto clean;
  }
  if ((r = buf_parse_tag_primary(buf, &tmp.arguments->tag)) <= 0) {
    call_clean(&tmp);
    goto clean;
  }
  result += r;
  if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_peek_ident(buf, &ident)) <= 0)
    goto restore;
  ops = env_global()->ops;
  if (! ops_get(ops, ident.sym, 2, &op_tag)) {
    r = 0;
    goto restore;
  }
  tag_clean(&op_tag);
  if ((r = buf_parse_call_op_rec(buf, &tmp, 0)) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  call_clean(&tmp);
  r = 0;
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_op_rec (s_buf *buf, s_call *dest, u8 min_precedence)
{
  character c;
  s_tag *left;
  bool merge_left = false;
  s_ident next_ident;
  s_tag   next_op_tag = {0};
  s_op   *next_op;
  s_op  *op;
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  s_tag *right;
  s_buf_save save;
  s_call tmp;
  s_call tmp2 = {0};
  s_call tmp3 = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  call_init_op(&tmp);
  left = &tmp.arguments->tag;
  right = &list_next(tmp.arguments)->tag;
  tag_init_copy(left, &dest->arguments->tag);
  if ((r = buf_peek_ident(buf, &next_ident)) <= 0)
    goto restore;
  // FIXME: acquire read-only lock for ops
  ops = env_global()->ops;
  if (! ops_get(ops, next_ident.sym, 2, &next_op_tag)) {
    r = 0;
    goto restore;
  }
  next_op = next_op_tag.data.pstruct->data;
  while (r > 0 && next_op->precedence >= min_precedence) {
    tag_clean(&next_op_tag);
    next_op_tag = (s_tag) {0};
    if ((r = buf_parse_ident(buf, &next_ident)) <= 0)
      goto restore;
    result += r;
    if (! ops_get(ops, next_ident.sym, 2, &next_op_tag)) {
      r = 0;
      goto restore;
    }
    next_op = next_op_tag.data.pstruct->data;
    if (merge_left) {
      call_init_op(&tmp3);
      tmp3.ident.module = NULL;
      tmp3.ident.sym = op->sym;
      tmp3.arguments->tag = *left;
      list_next(tmp3.arguments)->tag = *right;
      tag_init_call(left);
      left->data.call = tmp3;
    }
    else
      merge_left = true;
    tag_clean(&op_tag);
    op_tag = (s_tag) {0};
    if (! tag_init_copy(&op_tag, &next_op_tag)) {
      r = -1;
      goto restore;
    }
    op = op_tag.data.pstruct->data;
    tmp.ident.module = NULL;
    tmp.ident.sym = op->sym;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_parse_tag_primary(buf, right)) <= 0)
      goto restore;
    result += r;
    buf_save_update(buf, &save);
    if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
      break;
    result += r;
    if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
      break;
    if (r > 0 && c == '\n')
      break;
    tag_clean(&next_op_tag);
    next_op_tag = (s_tag) {0};
    r = buf_peek_ident(buf, &next_ident);
    if (r <= 0)
      break;
    if (! ops_get(ops, next_ident.sym, 2, &next_op_tag) &&
        ! ops_get(ops, next_ident.sym, 1, &next_op_tag))
      break;
    next_op = next_op_tag.data.pstruct->data;
    while ((next_op->arity == 2 &&
            next_op->precedence > op->precedence) ||
           (next_op->associativity == OP_ASSOCIATIVITY_RIGHT &&
            next_op->precedence == op->precedence)) {
      call_init_op(&tmp2);
      tmp2.arguments->tag = *right;
      if ((r = buf_parse_call_op_rec
           (buf, &tmp2, (next_op->precedence > op->precedence) ?
            op->precedence + 1 : op->precedence)) <= 0) {
        tmp2.arguments->tag.type = TAG_VOID;
        call_clean(&tmp2);
        goto ok;
      }
      result += r;
      tag_init_call(right);
      right->data.call = tmp2;
      buf_save_update(buf, &save);
      if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
        goto ok;
      result += r;
      if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
        goto ok;
      if (r > 0 && c == '\n')
        goto ok;
      tag_clean(&next_op_tag);
      next_op_tag = (s_tag) {0};
      r = buf_peek_ident(buf, &next_ident);
      if (r <= 0 || ! (ops_get(ops, next_ident.sym, 2, &next_op_tag)))
        goto ok;
      next_op = next_op_tag.data.pstruct->data;
    }
  }
 ok:
  buf_save_restore_rpos(buf, &save);
  tag_clean(&next_op_tag);
  tag_clean(&op_tag);
  call_clean(dest);
  *dest = tmp;
  buf_save_clean(buf, &save);
  return result;
 restore:
  buf_save_restore_rpos(buf, &save);
  tag_clean(&next_op_tag);
  tag_clean(&op_tag);
  call_clean(&tmp);
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_op_unary (s_buf *buf, s_call *dest)
{
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp;
  assert(buf);
  assert(dest);
  call_init_op_unary(&tmp);
  buf_save_init(buf, &save);
  if ((r = buf_parse_ident(buf, &tmp.ident)) <= 0)
    goto restore;
  result += r;
  ops = env_global()->ops;
  if (! ops_get(ops, tmp.ident.sym, 1, &op_tag)) {
    if (false) {
      err_write_1("buf_parse_call_op_unary: ");
      err_inspect_ident(&tmp.ident);
      err_puts(": operator_resolve");
    }
    goto restore;
  }
  tag_clean(&op_tag);
  buf_save_clean(buf, &save);
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto clean;
  result += r;
  if ((r = buf_parse_tag(buf, &tmp.arguments->tag)) <= 0)
    goto clean;
  result += r;
  *dest = tmp;
  return result;
 restore:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  r = 0;
 clean:
  call_clean(&tmp);
  return r;
}

sw buf_parse_call_paren (s_buf *buf, s_call *dest)
{
  sw r;
  sw result = 0;
  s_call tmp = {0};
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto restore;
  result += r;
  call_init_op_unary(&tmp);
  tmp.ident.sym = &g_sym__paren;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_tag(buf, &tmp.arguments->tag)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, ")")) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_callable (s_buf *buf, p_callable *dest)
{
  s_cfn cfn = {0};
  s_fn fn = {0};
  sw r;
  p_callable tmp = NULL;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_cfn(buf, &cfn)) > 0) {
    if (! (tmp = callable_new())) {
      cfn_clean(&cfn);
      return -1;
    }
    tmp->type = CALLABLE_CFN;
    tmp->data.cfn = cfn;
    *dest = tmp;
  }
  else if ((r = buf_parse_fn(buf, &fn)) > 0) {
    if (! (tmp = callable_new())) {
      fn_clean(&fn);
      return -1;
    }
    tmp->type = CALLABLE_FN;
    tmp->data.fn = fn;
    *dest = tmp;
  }
  return r;
}

sw buf_parse_cast (s_buf *buf, s_call *dest)
{
  const s_sym *module = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp;
  buf_save_init(buf, &save);
  if ((r = buf_parse_paren_sym(buf, &module)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  call_init_op(&tmp);
  ident_init(&tmp.ident, module, &g_sym_cast);
  tag_init_sym(&tmp.arguments->tag, module);
  if ((r = buf_parse_tag_primary(buf,
                                 &list_next(tmp.arguments)->tag)) <= 0)
    goto clean;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_cfn (s_buf *buf, s_cfn *dest)
{
  s_list *arg_types = NULL;
  bool macro = false;
  s_str name_str;
  const s_sym *name_sym;
  sw r;
  sw result = 0;
  const s_sym *result_type;
  s_buf_save save;
  s_cfn tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "cfn")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_read_1(buf, "_macro")) < 0)
    goto restore;
  if (r) {
    result += r;
    macro = true;
  }
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_sym(buf, &result_type)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_str(buf, &name_str)) <= 0)
    goto restore;
  result += r;
  if (! (name_sym = str_to_sym(&name_str)))
    goto restore;
  str_clean(&name_str);
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_list_paren(buf, &arg_types)) <= 0)
    goto restore;
  result += r;
  cfn_init(&tmp, name_sym, arg_types, result_type);
  tmp.macro = macro;
  // FIXME: implement env_eval_cfn
  /*
  cfn_prep_cif(&tmp);
  cfn_link(&tmp);
  */
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_character (s_buf *buf, character *dest)
{
  character c;
  sw r;
  sw r1;
  sw r2;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "'")) <= 0)
    goto clean;
  if ((r1 = buf_parse_str_character(buf, &c)) <= 0) {
    r = r1;
    goto restore;
  }
  if ((r2 = buf_read_1(buf, "'")) <= 0) {
    r = r2;
    goto restore;
  }
  *dest = c;
  r = r + r1 + r2;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_comment (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw r1;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "#")) <= 0)
    goto clean;
  result += r;
  while (1) {
    if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
      goto restore;
    csize = r;
    if ((r = buf_ignore(buf, csize)) <= 0)
      goto restore;
    result += csize;
    if (c == '\n')
      break;
  }
  while (1) {
    buf_save_update(buf, &save);
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto clean;
    r1 = r;
    if ((r = buf_read_1(buf, "#")) <= 0)
      goto restore;
    result += r1 + r;
    while (1) {
      r = buf_peek_character_utf8(buf, &c);
      if (r <= 0)
        break;
      csize = r;
      if ((r = buf_ignore(buf, csize)) < 0)
        goto clean;
      result += csize;
      if (c == '\n')
        break;
    }
    if (r < 0)
      goto clean;
  }
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return result;
}

sw buf_parse_comments (s_buf *buf)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  buf_save_init(buf, &save);
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c) &&
         c != '\n') {
    csize = r;
    if ((r = buf_ignore(buf, csize)) <= 0)
      goto restore;
    result += csize;
  }
  if ((r = buf_parse_comment(buf)) <= 0)
    goto restore;
  result += r;
  while (1) {
    buf_save_update(buf, &save);
    while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
           character_is_space(c) &&
           c != '\n') {
      csize = r;
      if ((r = buf_ignore(buf, csize)) <= 0)
        break;
      result += csize;
    }
    if ((r = buf_parse_comment(buf)) <= 0)
      break;
    result += r;
  }
  if (r <= 0)
    buf_save_restore_rpos(buf, &save);
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_complex (s_buf *buf, s_complex *c)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_complex tmp;
  assert(buf);
  assert(c);
  buf_save_init(buf, &save);
  if ((r = buf_parse_tag(buf, &tmp.x)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "+i")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_tag(buf, &tmp.y)) <= 0)
    goto clean;
  result += r;
  *c = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_cow (s_buf *buf, s_cow *cow)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_cow tmp = {0};
  const s_sym *type;
  assert(buf);
  assert(cow);
  buf_save_init(buf, &save);
  type = &g_sym_Tag;
  if ((r = buf_parse_paren_sym(buf, &type)) < 0)
    goto clean;
  if (r) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_read_1(buf, "cow")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if (! cow_init(&tmp, type))
    goto restore;
  if ((r = buf_parse_tag(buf, cow_read_write(&tmp))) <= 0)
    goto restore;
  result += r;
  if (tmp.type != &g_sym_Tag) {
    if (! tag_type(cow_read_write(&tmp), &type)) {
      r = -1;
      goto restore;
    }
    if (tmp.type != type) {
      err_write_1("buf_parse_cow: type mismatch: ");
      err_inspect_sym(&tmp.type);
      err_write_1(" != ");
      err_inspect_sym(&type);
      assert(! "buf_parse_cow: type mismatch");
      r = -1;
      goto restore;
    }
  }
  cow_freeze(&tmp);
  *cow = tmp;
  r = result;
  goto clean;
 restore:
  cow_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_digit (s_buf *buf, const s_str *base, u8 *dest)
{
  character c;
  sw digit;
  sw r;
  assert(buf);
  assert(base);
  assert(dest);
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    return r;
  if ((digit = str_character_position(base, c)) >= 0) {
    if (digit > 255) {
      err_write_1("buf_parse_digit: digit overflow: ");
      err_inspect_sw_decimal(&digit);
      err_write_1("\n");
      assert(! "buf_parse_digit: digit overflow");
      return -1;
    }
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      return r;
    *dest = digit;
    return r;
  }
  return 0;
}

sw buf_parse_digit_hex (s_buf *buf, u8 *dest)
{
  character c;
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    return r;
  if (c >= '0' && c <= '9')
    *dest = c - '0';
  else if (c >= 'a' && c <= 'f')
    *dest = c - 'a' + 10;
  else if (c >= 'A' && c <= 'F')
    *dest = c - 'A' + 10;
  else
    return 0;
  r = buf_ignore(buf, r);
  return r;
}

sw buf_parse_digit_oct (s_buf *buf, u8 *dest)
{
  character c;
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    return r;
  if (c >= '0' && c <= '7')
    *dest = c - '0';
  else
    return 0;
  r = buf_ignore(buf, r);
  return r;
}

sw buf_parse_digit_bin (s_buf *buf, u8 *dest)
{
  character c;
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    return r;
  if (c == '0' || c == '1')
    *dest = c - '0';
  else
    return 0;
  r = buf_ignore(buf, r);
  return r;
}

sw buf_parse_digit_dec (s_buf *buf, u8 *dest)
{
  character c;
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    return r;
  if (c < '0' || c > '9')
    return 0;
  *dest = c - '0';
  r = buf_ignore(buf, r);
  return r;
}

sw buf_parse_f32 (s_buf *buf, f32 *dest)
{
  character c;
  u8 digit;
  f64 exp = 0;
  f64 exp_sign = 1;
  uw i;
  sw r;
  sw result = 0;
  s_buf_save save;
  f64 tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(F32)")) < 0)
    goto restore;
  result += r;
  if (r > 0) {
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto restore;
  result += r;
  tmp = digit;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp = tmp * 10 + digit;
  }
  if (r < 0 ||
      (r = buf_read_1(buf, ".")) <= 0)
    goto restore;
  result += r;
  i = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp += (f64) digit / i;
    i *= 10;
    result += r;
  }
  if ((r = buf_read_1(buf, "e")) > 0) {
    result += r;
    if ((r = buf_read_1(buf, "-")) > 0) {
      exp_sign = -1;
      result += r;
    }
    else if ((r = buf_read_1(buf, "+")) > 0) {
      result += r;
    }
    while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
      exp = exp * 10 + digit;
      result += r;
    }
    tmp *= powf(10, exp_sign * exp);
  }
  if ((r = buf_read_1(buf, "f")) < 0)
    goto restore;
  result += r;
  if ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
      ! sym_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
  *dest = (f32) tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_f64 (s_buf *buf, f64 *dest) {
  sw r;
  sw result = 0;
  u8 digit;
  s_buf_save save;
  f64 tmp = 0;
  s64 exp = 0;
  s8  exp_sign = 1;
  uw i;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(F64)")) < 0)
    goto restore;
  result += r;
  if (r > 0) {
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto restore;
  tmp = digit;
  result += r;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp = tmp * 10 + digit;
    result += r;
  }
  if (r < 0 ||
      (r = buf_read_1(buf, ".")) <= 0)
    goto restore;
  result += r;
  i = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp += (f64) digit / i;
    i *= 10;
  }
  if ((r = buf_read_1(buf, "e")) > 0) {
    result += r;
    if ((r = buf_read_1(buf, "-")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      exp_sign = -1;
    }
    else {
      r = buf_read_1(buf, "+");
      if (r < 0)
        goto restore;
      result += r;
      while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
        result += r;
        exp = exp * 10 + digit;
      }
    }
    tmp *= pow(10, exp_sign * exp);
  }
  *dest = tmp;
  r = result;
  goto clean;
  restore:
  buf_save_restore_rpos(buf, &save);
  clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_f128 (s_buf *buf, f128 *dest) {
  sw r;
  sw result = 0;
  u8 digit;
  s_buf_save save;
  f128 tmp = 0;
  s64 exp = 0;
  s8  exp_sign = 1;
  uw i;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(F128)")) < 0)
    goto restore;
  result += r;
  if (r > 0) {
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto restore;
  tmp = digit;
  result += r;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp = tmp * 10 + digit;
    result += r;
  }
  if (r < 0 ||
      (r = buf_read_1(buf, ".")) <= 0)
    goto restore;
  result += r;
  i = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp += (f128) digit / i;
    i *= 10;
  }
  if ((r = buf_read_1(buf, "e")) > 0) {
    result += r;
    if ((r = buf_read_1(buf, "-")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      exp_sign = -1;
    }
    else {
      r = buf_read_1(buf, "+");
      if (r < 0)
        goto restore;
      result += r;
      while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
        result += r;
        exp = exp * 10 + digit;
      }
    }
    tmp *= powl(10, exp_sign * exp);
  }
  *dest = tmp;
  r = result;
  goto clean;
  restore:
  buf_save_restore_rpos(buf, &save);
  clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fact (s_buf *buf, s_fact_w *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_fact_w tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_static_tag(buf, &tmp.subject)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, ",")) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_static_tag(buf, &tmp.predicate)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, ",")) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_static_tag(buf, &tmp.object)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "}")) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  fact_w_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fn (s_buf *buf, s_fn *dest)
{
  s_list *frame_list = NULL;
  s_ident ident;
  s_list *l = NULL;
  sw r;
  sw r1;
  sw result = 0;
  s_buf_save save;
  s_fn tmp = {0};
  s_fn_clause **tail;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_ident(buf, &ident)) <= 0)
    goto clean;
  result += r;
  if (ident.sym != &g_sym_fn &&
      ident.sym != &g_sym_macro) {
    r = 0;
    goto restore;
  }
  fn_init(&tmp, ident.module);
  tail = &tmp.clauses;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "{")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
    while (1) {
      *tail = fn_clause_new(NULL);
      if ((r = buf_parse_fn_clause(buf, *tail)) <= 0)
        goto restore;
      result += r;
      tail = &(*tail)->next_clause;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, "}")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        goto ok;
      }
    }
  }
  else {
    tmp.clauses = fn_clause_new(NULL);
    if ((r = buf_parse_fn_clause(buf, tmp.clauses)) <= 0)
      goto restore;
    result += r;
  }
  buf_save_clean(buf, &save);
  buf_save_init(buf, &save);
  if ((r = buf_ignore_spaces_but_newline(buf)) <= 0 ||
      (r1 = buf_parse_list(buf, &frame_list)) <= 0)
    goto ok;
  result += r + r1;
  if (! frame_list)
    goto ok;
  tmp.frame = frame_new(NULL);
  l = frame_list;
  while (l) {
    if (! frame_binding_new(tmp.frame,
                            l->tag.data.tuple.tag[0].data.sym,
                            l->tag.data.tuple.tag + 1))
      goto restore;
    l = list_next(l);
  }
  list_delete_all(frame_list);
 ok:
  tmp.macro = ident.sym == &g_sym_macro;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  fn_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fn_clause (s_buf *buf, s_fn_clause *dest)
{
  sw r;
  sw result = 0;
  s_fn_clause tmp;
  assert(buf);
  assert(dest);
  fn_clause_init(&tmp, NULL);
  if ((r = buf_parse_fn_pattern(buf, &tmp.pattern)) <= 0) {
    err_puts("buf_parse_fn_clause: invalid pattern");
    err_inspect_buf(buf);
    err_write_1("\n");
    goto clean;
  }
  result += r;
  tmp.arity = list_length(tmp.pattern);
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto clean;
  result += r;
  if ((r = buf_parse_do_block(buf, &tmp.algo)) <= 0) {
    err_puts("buf_parse_fn_clause: invalid program");
    err_inspect_buf(buf);
    err_write_1("\n");
    goto clean;
  }
  result += r;
  *dest = tmp;
  return result;
 clean:
  fn_clause_clean(&tmp);
  return r;
}

sw buf_parse_fn_pattern (s_buf *buf, s_list **dest)
{
  sw r;
  sw result = 0;
  s_tag tag;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(buf);
  assert(dest);
  if ((r = buf_read_1(buf, "(")) < 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto clean;
  result += r;
  if ((r = buf_read_1(buf, ")")) < 0)
    goto clean;
  if (r) {
    result += r;
    goto ok;
  }
  while (1) {
    if ((r = buf_parse_tag_primary_2(buf, &tag)) <= 0)
      goto clean;
    result += r;
    *tail = list_new(NULL);
    (*tail)->tag = tag;
    tail = &(*tail)->next.data.list;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto clean;
    result += r;
    if ((r = buf_read_1(buf, ",")) < 0)
      goto clean;
    result += r;
    if (! r) {
      if ((r = buf_read_1(buf, ")")) < 0)
        goto clean;
      if (r) {
        result += r;
        goto ok;
      }
    }
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto clean;
    result += r;
  }
 ok:
  *dest = tmp;
  return result;
 clean:
  list_delete_all(tmp);
  return r;
}

sw buf_parse_ident (s_buf *buf, s_ident *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_ident tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(Ident)")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  r = buf_parse_module_name(buf, &tmp.module);
  if (r < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_read_1(buf, ".")) <= 0)
      goto restore;
    result += r;
  }
  r = buf_parse_ident_sym(buf, &tmp.sym);
  if (r <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_ident_sym (s_buf *buf, const s_sym **dest)
{
  s_buf buf_tmp;
  character c;
  sw csize;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  char t[IDENT_MAX];
  const s_sym *tmp = NULL;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_peek_1(buf, "_\"")) < 0)
    goto restore;
  if (r > 0) {
    if ((r = buf_read_1(buf, "_")) < 0)
      goto restore;
    result += r;
    if ((r = buf_parse_str(buf, &str)) < 0)
      goto restore;
    result += r;
    tmp = str_to_sym(&str);
    str_clean(&str);
    if (! tmp) {
      r = -1;
      goto restore;
    }
    *dest = tmp;
    r = result;
    goto clean;
  }
  if ((r = buf_peek_character_utf8(buf, &c)) < 0)
    goto restore;
  if (r > 0 && ! ident_first_character_is_reserved(c)) {
    csize = r;
    buf_init(&buf_tmp, false, sizeof(t), t);
    if ((r = buf_xfer(&buf_tmp, buf, csize)) < 0)
      goto restore;
    result += csize;
    while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
           ! ident_character_is_reserved(c)) {
      csize = r;
      if ((r = buf_xfer(&buf_tmp, buf, csize)) != csize)
        goto restore;
      result += csize;
    }
    buf_read_to_str(&buf_tmp, &str);
    tmp = str_to_sym(&str);
    str_clean(&str);
    if (! tmp) {
      r = 0;
      goto restore;
    }
    *dest = tmp;
    r = result;
    goto clean;
  }
  r = 0;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_if (s_buf *buf, s_call *dest)
{
  s_list **args_last;
  s_tag *condition;
  s_tag *else_;
  bool has_else;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag *then;
  s_call tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_sym(buf, &g_sym_if)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  tmp.ident.module = &g_sym_KC3;
  tmp.ident.sym = &g_sym_if_then_else;
  args_last = &tmp.arguments;
  *args_last = list_new(NULL);
  condition = &(*args_last)->tag;
  if ((r = buf_parse_tag(buf, condition)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  args_last = &(*args_last)->next.data.list;
  *args_last = list_new(NULL);
  then = &(*args_last)->tag;
  if ((r = buf_parse_if_then(buf, then, &has_else)) < 0)
    goto restore;
  result += r;
  args_last = &(*args_last)->next.data.list;
  *args_last = list_new(NULL);
  else_ = &(*args_last)->tag;
  if (has_else) {
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    else_->type = TAG_DO_BLOCK;
    if ((r = buf_parse_do_block_inner(buf, false,
                                      &else_->data.do_block)) <= 0)
      goto restore;
    result += r;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
  call_clean(&tmp);
  r = -1;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_if_then (s_buf *buf, s_tag *dest, bool *has_else)
{
  s_list **i;
  sw j;
  s_list *k;
  sw l;
  s_list *list = 0;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_do_block tmp;
  buf_save_init(buf, &save);
  if ((r = buf_read_sym(buf, &g_sym_then)) < 0)
    goto restore;
  result += r;
  if (! r) {
    if ((r = buf_read_sym(buf, &g_sym_do)) < 0)
      goto restore;
    result += r;
  }
  i = &list;
  *i = NULL;
  while (1) {
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    *i = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*i)->tag)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_sym(buf, &g_sym_end)) < 0)
      goto restore;
    if (! r && (r = buf_read_1(buf, "}")) < 0)
      goto restore;
    if (r > 0) {
      *has_else = false;
      goto ok;
    }
    if ((r = buf_read_sym(buf, &g_sym_else)) < 0)
      goto restore;
    if (r > 0) {
      *has_else = true;
      goto ok;
    }
    if ((r = buf_read_1(buf, "\n")) < 0 ||
        (! r && (r = buf_read_1(buf, ";")) <= 0))
      goto restore;
    result += r;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_sym(buf, &g_sym_end)) < 0)
      goto restore;
    if (! r && (r = buf_read_1(buf, "}")) < 0)
      goto restore;
    if (r > 0) {
      *has_else = false;
      goto ok;
    }
    if ((r = buf_read_sym(buf, &g_sym_else)) < 0)
      goto restore;
    if (r > 0) {
      *has_else = true;
      goto ok;
    }
    i = &(*i)->next.data.list;
  }
  r = 0;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  if (list)
    list_delete_all(list);
  return r;
 ok:
  result += r;
  j = list_length(list);
  if (! do_block_init(&tmp, j)) {
    r = -2;
    goto restore;
  }
  k = list;
  l = 0;
  while (l < j) {
    tmp.tag[l] = k->tag;
    tag_init_void(&k->tag);
    k = list_next(k);
    l++;
  }
  dest->type = TAG_DO_BLOCK;
  dest->data.do_block = tmp;
  r = result;
  goto clean;

}

sw buf_parse_integer (s_buf *buf, s_integer *dest)
{
  sw r;
  sw result;
  s_buf_save save;
  e_bool negative;
  s_integer tmp;
  s_integer tmp2;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "-")) < 0)
    goto clean;
  result = r;
  negative = r ? true : false;
  if ((r = buf_read_1(buf, "0b")) < 0)
    goto restore;
  if ((r == 0) && (r = buf_read_1(buf, "0B")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_integer_unsigned_bin(buf, &tmp)) <= 0)
      goto restore;
    result += r;
    if (negative) {
      integer_neg(&tmp, &tmp2);
      integer_clean(&tmp);
      tmp = tmp2;
    }
    *dest = tmp;
    r = result;
    goto clean;
  }
  if ((r = buf_read_1(buf, "0o")) < 0)
    goto clean;
  if (r == 0 && (r = buf_read_1(buf, "0O")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_integer_unsigned_oct(buf, &tmp)) <= 0)
      goto restore;
    result += r;
    if (negative) {
      integer_neg(&tmp, &tmp2);
      integer_clean(&tmp);
      tmp = tmp2;
    }
    *dest = tmp;
    r = result;
    goto clean;
  }
  if ((r = buf_read_1(buf, "0x")) < 0)
    goto clean;
  if (r == 0 && (r = buf_read_1(buf, "0X")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_integer_unsigned_hex(buf, &tmp)) <= 0)
      goto restore;
    result += r;
    if (negative) {
      integer_neg(&tmp, &tmp2);
      integer_clean(&tmp);
      tmp = tmp2;
    }
    *dest = tmp;
    r = result;
    goto clean;
  }
  if ((r = buf_parse_integer_unsigned_dec(buf, &tmp)) <= 0) {
    r = 0;
    goto restore;
  }
  result += r;
  if (negative) {
    integer_neg(&tmp, &tmp2);
    integer_clean(&tmp);
    tmp = tmp2;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_integer_unsigned_bin (s_buf *buf, s_integer *dest)
{
  const mp_digit radix = 2;
  sw r;
  u8 digit;
  int result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_parse_digit_bin(buf, &digit)) <= 0)
    goto clean;
  result += r;
  integer_init_zero(dest);
  if (mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
    goto error;
  while ((r = buf_read_1(buf, "_")) >= 0 &&
         (r = buf_parse_digit_bin(buf, &digit)) > 0) {
    result += r;
    if (mp_mul_d(&dest->mp_int, radix, &dest->mp_int) != MP_OKAY ||
        mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
      goto error;
  }
  if (result > 0)
    r = result;
  goto clean;
 error:
  r = -1;
  integer_clean(dest);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_integer_unsigned_dec (s_buf *buf, s_integer *dest)
{
  const mp_digit radix = 10;
  sw r;
  u8 digit;
  sw result = 0;
  s_buf_save save;
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  buf_save_init(buf, &save);
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto clean;
  result += r;
  if (! integer_init_u8(&tmp, digit))
    goto error;
  while ((r = buf_read_1(buf, "_")) >= 0 &&
         (result += r,
          (r = buf_parse_digit_dec(buf, &digit)) > 0)) {
    result += r;
    if (mp_mul_d(&tmp.mp_int, radix, &tmp2.mp_int) != MP_OKAY ||
        mp_add_d(&tmp2.mp_int, digit, &tmp.mp_int) != MP_OKAY)
      goto error;
    integer_clean(&tmp2);
  }
  *dest = tmp;
  r = result;
  goto clean;
 error:
  r = -1;
  integer_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_integer_unsigned_hex (s_buf *buf, s_integer *dest)
{
  const mp_digit radix = 16;
  sw r;
  u8 digit;
  int result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_parse_digit_hex(buf, &digit)) <= 0)
    goto clean;
  result += r;
  integer_init_zero(dest);
  if (mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
    goto error;
  while ((r = buf_read_1(buf, "_")) >= 0 &&
         (r = buf_parse_digit_hex(buf, &digit)) > 0) {
    result += r;
    if (mp_mul_d(&dest->mp_int, radix, &dest->mp_int) != MP_OKAY ||
        mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
      goto error;
  }
  if (result > 0)
    r = result;
  goto clean;
 error:
  r = -1;
  integer_clean(dest);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_integer_unsigned_oct (s_buf *buf, s_integer *dest)
{
  const mp_digit radix = 8;
  sw r;
  u8 digit;
  int result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_parse_digit_oct(buf, &digit)) <= 0)
    goto clean;
  result += r;
  integer_init_zero(dest);
  if (mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
    goto error;
  while ((r = buf_read_1(buf, "_")) >= 0 &&
         (r = buf_parse_digit_oct(buf, &digit)) > 0) {
    result += r;
    if (mp_mul_d(&dest->mp_int, radix, &dest->mp_int) != MP_OKAY ||
        mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
      goto error;
  }
  if (result > 0)
    r = result;
  goto clean;
 error:
  r = -1;
  integer_clean(dest);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_list (s_buf *buf, s_list **list)
{
  s_list **i;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(list);
  buf_save_init(buf, &save);
  i = list;
  if ((r = buf_read_1(buf, "[")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0) {
    err_puts("buf_parse_list: buf_parse_comments 1");
    assert(! "buf_parse_list: buf_parse_comments 1");
    goto restore;
  }
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0) {
    err_puts("buf_parse_list: buf_ignore_spaces 1");
    assert(! "buf_parse_list: buf_ignore_spaces 1");
    goto restore;
  }
  result += r;
  if ((r = buf_read_1(buf, "]")) < 0) {
    err_puts("buf_parse_list: buf_read_1 \"]\" 1");
    assert(! "buf_parse_list: buf_read_1 \"]\" 1");
    goto restore;
  }
  if (r > 0) {
    result += r;
    *list = NULL;
    r = result;
    goto clean;
  }
  *i = NULL;
  while (1) {
    *i = list_new(NULL);
    if ((r = buf_parse_list_tag(buf, &(*i)->tag)) < 0) {
      err_puts("buf_parse_list: buf_parse_list_tag < 0");
      assert(! "buf_parse_list: buf_parse_list_tag < 0");
      goto restore;
    }
    if (! r) {
      err_puts("buf_parse_list: buf_parse_list_tag = 0");
      assert(! "buf_parse_list: buf_parse_list_tag = 0");
      r = -1;
      goto restore;
    }
    result += r;
    if ((r = buf_parse_comments(buf)) < 0) {
      err_puts("buf_parse_list: buf_parse_comments 2");
      assert(! "buf_parse_list: buf_parse_comments 2");
      goto restore;
    }
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0) {
      err_puts("buf_parse_list: buf_ignore_spaces 2");
      assert(! "buf_parse_list: buf_ignore_spaces 2");
      goto restore;
    }
    result += r;
    if ((r = buf_read_1(buf, "]")) < 0) {
      err_puts("buf_parse_list: buf_read_1 \"]\" 2");
      assert(! "buf_parse_list: buf_read_1 \"]\" 2");
      goto restore;
    }
    if (r > 0) {
      result += r;
      r = result;
      goto clean;
    }
    if ((r = buf_read_1(buf, ",")) < 0) {
      err_puts("buf_parse_list: buf_read_1 \",\"");
      assert(! "buf_parse_list: buf_read_1 \",\"");
      goto restore;
    }
    if (r > 0) {
      result += r;
      i = &(*i)->next.data.list;
      if ((r = buf_parse_comments(buf)) < 0) {
        err_puts("buf_parse_list: buf_parse_comments 3");
        assert(! "buf_parse_list: buf_parse_comments 3");
        goto restore;
      }
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0) {
        err_puts("buf_parse_list: buf_ignore_spaces 3");
        assert(! "buf_parse_list: buf_ignore_spaces 3");
        goto restore;
      }
      result += r;
      continue;
    }
    if ((r = buf_read_1(buf, "|")) < 0) {
      err_puts("buf_parse_list: buf_read_1 \"|\"");
      assert(! "buf_parse_list: buf_read_1 \"|\"");
      goto restore;
    }
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0) {
        err_puts("buf_parse_list: buf_parse_comments 4");
        assert(! "buf_parse_list: buf_parse_comments 4");
        goto restore;
      }
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0) {
        err_puts("buf_parse_list: buf_ignore_spaces 4");
        assert(! "buf_parse_list: buf_ignore_spaces 4");
        goto restore;
      }
      result += r;
      if ((r = buf_parse_tag(buf, &(*i)->next)) < 0) {
        err_puts("buf_parse_list: buf_parse_tag < 0");
        assert(! "buf_parse_list: buf_parse_tag < 0");
        goto restore;
      }
      if (! r) {
        err_puts("buf_parse_list: buf_parse_tag = 0");
        assert(! "buf_parse_list: buf_parse_tag = 0");
        r = -1;
        goto restore;
      }
      result += r;
      if ((r = buf_parse_comments(buf)) < 0) {
        err_puts("buf_parse_list: buf_parse_comments 5");
        assert(! "buf_parse_list: buf_parse_comments 5");
        goto restore;
      }
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0) {
        err_puts("buf_parse_list: buf_ignore_spaces 5");
        assert(! "buf_parse_list: buf_ignore_spaces 5");
        goto restore;
      }
      result += r;
      if ((r = buf_read_1(buf, "]")) < 0) {
        err_puts("buf_parse_list: buf_read_1 \"]\" < 0");
        assert(! "buf_parse_list: buf_read_1 \"]\" < 0");
        goto restore;
      }
      if (! r) {
        err_puts("buf_parse_list: buf_read_1 \"]\" = 0");
        assert(! "buf_parse_list: buf_read_1 \"]\" = 0");
        r = -1;
        goto restore;
      }
      result += r;
      r = result;
      goto clean;
    }
    err_puts("buf_parse_list: invalid list");
    assert(! "buf_parse_list: invalid list");
    r = -1;
    goto restore;
  }
 restore:
  list_delete_all(*list);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_list_paren (s_buf *buf, s_list **list)
{
  s_list **i;
  sw r;
  sw result = 0;
  s_buf_save save;
  i = list;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, ")")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    *list = NULL;
    r = result;
    goto clean;
  }
  *i = NULL;
  while (1) {
    *i = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*i)->tag)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, ")")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      r = result;
      goto clean;
    }
    if ((r = buf_read_1(buf, ",")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      i = &(*i)->next.data.list;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      continue;
    }
    if ((r = buf_read_1(buf, "|")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_parse_tag(buf, &(*i)->next)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, ")")) <= 0)
        goto restore;
      result += r;
      r = result;
      goto clean;
    }
    goto restore;
  }
 restore:
  list_delete_all(*list);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_list_tag (s_buf *buf, s_tag *dest)
{
  s_tag key;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  s_tag tmp;
  s_tag value;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_sym_str(buf,ELF          >                    í         @     @  óúL‹    L3$UH‰åASAWAVAUATHƒìxI‰ôI‰şWÀL}¸A)GA)GøA)GèA)GØA)GÈIÇG    Hµ`ÿÿÿè    L‰÷L‰şè    I‰ÇH…À¥   L‹m¸L‰ïè    „ÀtpL‰ïè    H‰EÀL‰÷è    I‰ÅH…Àx^Hu€L‰÷è    H…À~HHƒ}ˆ tHu€L‰÷è    H…À~0Hu€L‰÷è    H…À~MıIÅHu€¹   L‰çóH¥M‰ïë%E1ÿëI‰ÇëM‰ïH}€è    Hµ`ÿÿÿL‰÷è    Hµ`ÿÿÿL‰÷è    L‰øHƒÄxA\A]A^A_A[]L3$L;    t
ÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì  I‰÷I‰şWÀ)E€)…pÿÿÿ)…`ÿÿÿ)…PÿÿÿHu˜è    H5    L‰÷è    I‰ÄH…À˜  L‰}ÀL‰÷è    H…Àˆt  I‰ÅHuÈL‰÷è    H…À\  H‹ØL‰÷è    H…ÀˆH  I‰ÇH5    L‰÷è    H…Àˆ-  MåLûLëH…À„ô   H5    L‰÷è    H…Àˆ  I‰ÄL­ÀşÿÿM…ä„«   º@   L‰ï1öHPÿÿÿè    L‰ïHuÈè    L‰ïH5    è    L‰÷Hu¸è    H…Àª   I‰ÇL‰ïHu¸è    L‰÷H5    è    H…Àƒ   IÜMüL‰ãH‡ØHØH‡ØL‰ïH5    è    L‰÷H5    è    I‰ÄH…À‰NÿÿÿëEL½ÀşÿÿL‰ÿ1öè    I‹0è    H‰EÈH5    L‰÷è    H…À~I‰ÄIÜH‹EÈH‹MÀH‰ëHu˜L‰÷è    E1äHu˜L‰÷è    L‰àHÄ  [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVHƒì(I‰÷I‰şHuÈè    L‰÷L‰şè    I‰ÇH…À~HuÈL‰÷è    HuÈL‰÷è    L‰øHƒÄ(A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVHƒì(I‰÷I‰şHuÈè    L‰÷L‰şè    I‰ÇH…À~HuÈL‰÷è    HuÈL‰÷è    L‰øHƒÄ(A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìpI‰ÿHƒ~ „Ë   I‰õH…hÿÿÿ¹   H‰ÇóH¥H‹XH<İ    è    IÇÄÿÿÿÿH…À„  I‰ÆH‹…xÿÿÿ¹   H…ÛtH‹ĞH¯
H‡ĞHƒÀH‡ĞH‡ØHÿÈH‡ØuçH‰hÿÿÿH‹PH¯H‰UHkùpè    H‰E˜H…À„‚   H‹ØHMÀH‰HµhÿÿÿL‰ÿL‰òE1Àè    H‰EÈH…À~dHµhÿÿÿ¹   L‰ïóH¥ëyH5    L‰ÿè    I‰ÄH…À~nL‰ÿè    I‰ÆH…ÀxH5    L‰ÿè    H…À~zMæIÆM‰ôë?L‰÷è    ë5H=    è    H}Èè    H=    è    H‰ßè    L‰÷è    L‹eÈL‰àHƒÄp[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃI‰ÄëËóúL‹    L3$UH‰åASAWAVAUATSHì°   M‰ÅI‰ÏI‰ÖH‰ûL‰E˜H½ ÿÿÿ¹   óH¥HµxÿÿÿH‰ßè    H5    H‰ßè    I‰ÄH…À  L‰}¸H‰ßè    I‰ÇH…Àˆ  MçM…ö„T  KÇî    H… ÿÿÿH‹@HÿÈH‰E IEH‰E¨L‰mÀH‰]ÈL9m u2H‰ßL‰óL‹u¸I‹6è    I‰ÄH…Àˆ„  L‰e°M‰üIƒpI‰ŞH‹]Èë-H‰ßHµ ÿÿÿL‰òH‹M¸L‹E¨è    I‰ÄH…À\  L‰e°M‰üKÿîH‰ßè    I‰ßH‹ØH…ÀxwL‰ÿH5    è    I‰ÅH…ÀxqH‹E°LàLëH‡ØHØH‡ØM‰üL‰ÿè    I‰ÇM…ítbM…ÿˆ¨   IßL‹mÀL‰ãé/ÿÿÿH=    è    éÿ   H=    è    é†   H=    è    I‰ÜëH=    è    M‰ìL‰ûé«   M…ÿxJIßL‹mÀL‰ãH5    H‰ßè    I‰ÄH…À~ H‹…0ÿÿÿL‰éHÁáH‹K;îu%Müé€   H=    ëWH=    è    L‰ãM‰üëHH=    è    H}˜è    H=    è    IÇÄÿÿÿÿëH=    è    H‹]Èë'H=    è    H=    è    HµxÿÿÿH‰ßè    HµxÿÿÿH‰ßè    L‰àHÄ°   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰ôI‰şHu è    L‰÷è    I‰ÇH…ÀˆÈ   tL‰÷è    I‰ÅH…À£   MıëE1íHuÌL‰÷è    H…À~c‹]ÌIt$L‰÷H‰uÀè    I‰Ç¸   ƒû{uGM…ÿGueL‰÷H‹uÀè    I‰Ç¸%   M…ÿ,uJL‰÷H‹uÀè    I‰Ç¸   M…ÿu/é‚   I‰Çë%M…ÿ~_A‰$MïL‰÷è    1ÉH…ÀHOÈIÏëM‰ïHu L‰÷è    Hu L‰÷è    L‰øHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÃu¿L‰÷L‰æè    H…ÀtI‰Çyë¨E1ÿë£óúL‹    L3$UH‰åASAWAVAUATSHì€   I‰ôI‰şHµPÿÿÿè    H5    L‰÷è    I‰ÅH…À~LL‰÷è    I‰ÇH…Àx*MïH5    L‰÷è    I‰ÅH…ÀxtfMïIÇD$    ëM‰ıHµPÿÿÿL‰÷è    M‰ïHµPÿÿÿL‰÷è    L‰øHÄ€   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃI‹$AD$H)E AD$8)EAD$()E€AD$)…pÿÿÿH5    L‰÷è    I‰ÅH…ÀˆaÿÿÿH‰]ÀL‰e¸ID$H‰EÈA¼   »   M…ít<L‰÷è    H…ÀxzIÿÄMïIÇL‰÷H5    è    I‰ÅH‡ØHƒÀH‡ØH…ÀyÄéÿÿÿH‰ßè    H…ÀtFH‹M¸H‹UÀH‰L‰aH‰A(…pÿÿÿ(M€(U(] H‹EÈX0P H éËşÿÿI‰Åé±şÿÿIÇÇÿÿÿÿéÆşÿÿóúL‹    L3$UH‰åASAWAVAUATHƒìhI‰öI‰üH½pÿÿÿ¹   óH¥H}°HuĞè    IÇÅÿÿÿÿH…À„¤   Hƒ}Ğ tbH‹½xÿÿÿHÁçè    H…À„„   I‰ÇH‹EĞHµpÿÿÿH‹NH‹VHÁáH‰DøL‰çL‰ú1Éè    H‰EÈH…Àx"Hµpÿÿÿ¹   L‰÷óH¥ë/H=    è    ë-H=    è    H}Èè    H=    è    L‰ÿè    L‹mÈL‰èHƒÄhA\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì  I‰ÍH‰UÀH‰ûH‰M¸H½0ÿÿÿ¹   H‰u¨óH¥HuˆH‰ßè    H5    H‰ßè    I‰ÄH…Àõ   H‰ßè    I‰ÇH…Àˆò   MçH‹EÀJÇè    IEH‰E°LµÀşÿÿH‰]ÈH‹E¨H‹@HÿÈH‰ßL9èuL‰öè    I‰ÄH…Àn  L‰÷è    ë Hµ0ÿÿÿH‹UÀH‹M°è    I‰ÄH…ÀP  M‰îH‹EÀJÿèH‰ßè    I‰İH‹ØH…ÀxuL‰ïH5    è    I‰ÅH…ÀxuMüLëLãM…ít{H‹}Èè    I‰ÇH…ÀˆÛ   IßM‰õH‹]ÈLµÀşÿÿéAÿÿÿH=    è    é  H=    è    M‰üéÛ   H=    è    I‰ÜL‰ëéÄ   H=    è    M‰ìé¬   H5    H‹}Èè    I‰ÄH…À„   IÜH‹…@ÿÿÿL‰ñHÁáH‹4H‹}ÀJ‹÷H…ö„¼   H9ÖH‹]Èt}H=    è    H}¸è    H=    è    IÇÄÿÿÿÿë;H=    è    M‰üë&H=    é%ÿÿÿH=    è    ëH=    è    H‹]ÈH=    è    HuˆH‰ßè    HuˆH‰ßè    L‰àHÄ  [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÃHÁH‰H‹M¸H‹•8ÿÿÿH‡ĞHÿÈH‡ĞH9ÑH‹]Ès§HÁáH‹TH¯TH‰Të‘óúL‹    L3$UH‰åASAWAVAUATSHƒì0H‰óI‰şHu¨è    H‹5    L‰÷è    I‰ÇH…Àˆ   tE1äëH5    L‰÷è    I‰ÇA´H…À~|L‰÷è    I‰ÅH…Àx9H‰]ÈL‰÷è    H‹ØH…À~1A¶ôL‰÷H‹UÈè    H…Àx(txMıIİIÅM‰ïë3H=    M‰ïëH=    I‰ßë
H=    I‰Çè    Hu¨L‰÷è    Hu¨L‰÷è    L‰øHƒÄ0[A\A]A^A_A[]L3$L;    t
ÌÌÌÌÌÌÌÌÌÌÃE1ÿë¸óúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰şHu¨è    HuÌL‰÷è    H…À~GI‰Ä1ÛLmÌ‹}Ìè    „Àt4ƒ}Ì
t.L‰÷L‰æè    H…À   LãL‰÷L‰îè    I‰ÄH…ÀÄë1ÛL‰÷è    I‰ÇH…À~wIßLmÌM‰üL‰÷Hu¨è    L‰÷L‰îè    H‹ØM‰çH…À~5‹}Ìè    „Àt)ƒ}Ì
t#L‰÷H‰Şè    H…À~IßL‰÷L‰îè    H‹ØëÆL‰÷è    N$8H…À˜ëI‰ÇLe¨L‰÷L‰æè    L‰÷L‰æè    L‰øHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHìĞ   H‰U¨A‰÷I‰şHE°H‰EÀHÇ     Hµpÿÿÿè    1ÛL¥ ÿÿÿL‰uÈD‰}¼L‰÷E„ÿtH5    è    I‰ÅH…ÀyéÀ  H‹5    è    I‰ÅH…ÀˆÈ  M…í…w  L‰÷L‰æè    I‰ÅH…Àˆe  t01ÿè    H‹UÀH‰H…À„x  ¹   H‰ÇL‰æóH¥H‹Hƒê€H‰UÀL‰÷è    I‰ÄH…Àˆ+  L‰÷è    I‰ÆH…Àˆ?  E„ÿtH‹}ÈH5    è    I‰ÇH…Ày!é#  H‹}ÈH‹5    è    I‰ÇH…Àˆ2  IİMåMõM…ÿ…  L‹uÈL‰÷H5    è    I‰ÇH…Àx^L¥ ÿÿÿuL‰÷H5    è    I‰ÇH…À~>L‰÷è    H‹ØH…Àˆ`  L‰÷è    I‰ÆH…Àˆ]  MïLóLûL‹uÈD‹}¼é„şÿÿH=    è    I~è    H=    è    L‰÷è    H=    è    M‰ıé}  M‰ïëfH=    é>  H=    è    L‰÷è    H=    è    M‰åéD  H=    è    L‹}ÈL‰ÿè    H=    è    M‰õM‰şé  L‰ëL‹e°L‰çè    I‰ÆH}H‰Æè    H…À„Œ   L‰eÀM…ö~5E1äL‹mÀH‹}˜Lç¹   L‰îóH¥L‰ïè    L‰ïè    I‰ÅIƒÄpIÿÎuÒLû‹E¼ˆE EH‹M¨H‹E H‰AI‰İL‹uÈL‹eÀé—   H=    è    I‰İësH=    è    M‰õë^IÇÅşÿÿÿL‹uÈëhH=    ëH=    è    ë>H=    ë)H=    è    L‰÷è    H=    è    ëH=    è    M‰ıL‹uÈHµpÿÿÿL‰÷è    L‹e°M…ätL‰çè    HµpÿÿÿL‰÷è    L‰èHÄĞ   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰ôI‰şHu°è    H5    L‰÷è    I‰ÇH…ÀxXAµt#HuÔL‰÷è    H…À~‹}Ôè    „Àt'Eˆ,$ë0H5    L‰÷è    I‰ÇH…ÀxE1íM…ÿu¾ëHu°L‰÷è    E1ÿHu°L‰÷è    L‰øHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìpH‰u€I‰ÿHÇE¸    WÀ)E )EHµ`ÿÿÿè    1ÿè    H‰Çè    I‰ÆH‰E H‰Çè    H‰EˆL‰}ÈL‰ÿL‰öè    H‰EÀH…À0  H5    H‹}Èè    H…Àˆ  H‹ØLe¸1ÀH‰E°H‹EÈH…Û„©   H‰Çè    H…Àˆì   I‰Å1ÿè    I‰$H‹}ÈH‰Æè    H…ÀÉ   I‰ÇM‹$$H‹}Èè    H…Àˆ°   I‰ÆH‹}ÈH5    è    H…À”   Iƒì€H]ÀLëLóLûH‡ØHØH‡ØH‰]ÀH‹}ÈH5    è    H‹ØHÿM°H…À‰LÿÿÿëTHƒ}° tMH‹    H‰EH‹    H‰E˜H‹MˆÇ   H‹E¸H‰AH‹EH‹M˜(E H‹U€BH‰H‰JH‹}ÈL‹}Àë+Hµ`ÿÿÿL‹uÈL‰÷è    H}è    H‹}¸è    E1ÿL‰÷Hµ`ÿÿÿè    L‰øHƒÄp[A\A]A^A_A[]L3$L;    tÌÌÃóúL‹    L3$UH‰åASAWAVAUATPI‰öI‰ÿè    I‰ÄMnL‰ÿL‰îè    I‰ÇH…À~1Iƒ}  t	AÇ'   ë!I‹¼$    I‹vè    H…ÀtáL‰÷H‰Æè    L‰øHƒÄA\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒìHI‰÷I‰şWÀLe°A)D$A)$Huè    L‰çè    L‰÷L‰æè    I‰ÄH…À~*Hu°L‰÷è    I‰ÅH…À~!Mì(E°(MÀAOAëH}°è    ëHuL‰÷è    M‰ìHuL‰÷è    L‰àHƒÄHA\A]A^A_A[]L3$L;    tÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒì8I‰÷I‰şWÀ)EÀHu è    H5    L‰÷è    I‰ÄH…Àˆ‚   tL‰÷è    I‰ÅH…Àx\MåëE1íHuÀL‰÷è    I‰ÄH…ÀxTtH5    L‰÷è    H…À~/MìIÄM‰åHuÈL‰÷è    I‰ÄH…À~Mì(EÀAëM‰ìëI‰ÄHu L‰÷è    Hu L‰÷è    L‰àHƒÄ8A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì°   I‰ôI‰şHu˜è    H5    L‰÷è    I‰ÇH…À!  IÇD$    L‰÷è    I‰ÅH…Àˆğ  L‰÷è    H…Àˆä  IƒÄMıIÅH5    L‰÷è    I‰ÇH…ÀˆÂ  tMïéÄ  Hµ ÿÿÿL‰÷è    I‰ÇH…À  H ÿÿÿ1ÿè    I‰$¹   H‰ÇH‰ŞóH¥L‰÷è    H‹ØH…Àˆµ  L‰÷è    H…ÀˆU  MïIßIÇL‰÷H5    è    H…Àˆ4  uvL‰÷H5    è    H‹ØH…Àˆf  taM‹$$L‰÷è    I‰ÅH…Àˆø   L‰÷è    H…Àˆì   Iƒì€LûIÅIİL‰÷H ÿÿÿH‰Şè    I‰ÇH…À.ÿÿÿé¿   IÇéÃ   H5    L‰÷è    H‹ØH…Àè   L‰÷è    I‰ÅH…Àˆ€   L‰÷è    H…ÀxxH‰E¸I‹4$HƒÆpL‰÷è    I‰ÄH…À¬   L‰÷è    H…ÀxKH‰EÀL‰÷è    H…Àx:H‰EÈH5    L‰÷è    H…À~"LûLm¸IİLmÀLmÈMåIÅM‰ïëM‰ïëI‰ÇHu˜L‰÷è    Hu˜L‰÷è    L‰øHÄ°   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃI‰ßë¯M‰çëªóúL‹    L3$UH‰åASAWAVAUATSHƒì`I‰ôI‰ıHÇEÈ    WÀLu A)FA)Hµpÿÿÿè    L‰÷è    H…À„ş   H‹    H‰E H‹    H‰E¨H‹u°L‰ïè    I‰ÇH…ÀÙ   L‰e˜H]ÈL‰ïH5    è    I‰ÆH…Àº   L‰ïè    I‰ÄH…Àˆ«   L‰u1ÿè    I‰ÆH‰HpL‰ïè    L‰ïI‰ÅH…À„   AÇ#   H‹I‰şH5    è    H…ÀxnL}MçMïHƒë€H…ÀM‰õ…mÿÿÿH‹}°è    Ç    L‹uÈH‹}°è    L‰p(E (M°H‹E˜ HëFIÇÇÿÿÿÿëL1ÿëM‰÷ëM‰çëM‰ïI‰ıëI‰ÇM‰õH‹}Èè    H} è    HµpÿÿÿL‰ïè    HµpÿÿÿL‰ïè    L‰øHƒÄ`[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ#   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHìÈ   I‰÷I‰şWÀ)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿ)…ÿÿÿHuˆè    H}¸è    H…À„—   H‹uÈL‰÷è    I‰ÄH…À¢   L‰÷è    H…Àx{I‰ÅHu¨L‰÷è    H…À~gè    H‹xpH‹u°Hÿÿÿº   è    H…ÀtDH½ÿÿÿè    Hu¸L‰÷1Òè    H…À~%MåIÅE¸MÈAOAM‰ìë,IÇÄÿÿÿÿë#H}¸è    HuˆL‰÷è    E1äë	H}¸è    HuˆL‰÷è    L‰àHÄÈ   A\A]A^A_A[]L3$L;    tÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì I‰ôI‰şHu°è    L‰÷è    I‰ÇH…Àˆ   tL‰÷è    I‰ÅH…À~mMıëE1íI\$L‰÷H‰Şè    I‰ÇH…À~!AÇ$   MïL‰÷è    1ÉH…ÀHOÈIÏë;u-L‰÷H‰Şè    I‰ÇH…ÀÊuL‰÷L‰æè    I‰ÇH…À½ëM‰ïHu°L‰÷è    Hu°L‰÷è    L‰øHƒÄ [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATHƒì I‰öI‰ÿLeÀL‰æè    L‰ÿL‰öè    I‰ÆL‰ÿL‰æè    L‰ÿL‰æè    L‰ğHƒÄ A\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHìÀ  ‰ÓI‰ôI‰şWÀ)E€)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿ)…pşÿÿ)…`şÿÿ)…Pşÿÿ)…@şÿÿ)…0şÿÿ)… şÿÿ)…şÿÿ)…Ğşÿÿ)…Àşÿÿ)…ÿÿÿ)… ÿÿÿHµ€şÿÿè    L½ şÿÿL‰ÿè    M‹L‰ÿè    H‰EÀI‹t$L‰ÿè    Hu L‰÷è    H…ÀÒ  è    H‹xpH‹u¨H ÿÿÿH‰}¸º   è    H…À„¬  L‰¥èşÿÿH‹…0ÿÿÿH‹ 8X
sUE1íHµ€şÿÿL‰÷è    H½ ÿÿÿè    H½şÿÿè    L‹½èşÿÿL‰ÿè    … şÿÿ°şÿÿAOAéw  ‰]œL‰½ğşÿÿIGH‰…øşÿÿH‹EÀHƒÀH‰…àşÿÿ³L½ ÿÿÿLe E1íL‰ÿè    WÀ)E€)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿL‰÷L‰æè    L‰ùI‰ÇH…ÀN  H‹u¨H‹}¸º   è    H…À„Ÿ  H‡Ø¨H‡Øu|H½ ÿÿÿè    HÇ… ÿÿÿ    H‹E°H‹ H‰…ÿÿÿH‹…ÿÿÿ¹   H‰ÇL‹¥ğşÿÿL‰æóH¥H‰Çè    ¹   H‰ÇH‹uÀóH¥L‰çè    (… ÿÿÿ(ÿÿÿH‹…øşÿÿ HL¥şÿÿL‰çè    WÀ)…pşÿÿ)…`şÿÿ)…Pşÿÿ)…@şÿÿ)…0şÿÿ)… şÿÿ)…şÿÿL‰çHµ ÿÿÿè    H…À„Z  H‹… şÿÿH‹ HÇ… şÿÿ    H‰E°H‹ H‰…¨şÿÿL‰÷è    I‰ÄH…Àˆ/  L‰÷H‹uÀè    H…Àm  MïMçIÇL‰÷Hµ€şÿÿè    L‰÷è    H…Àˆº  I‰ÅMıL‰÷HuÌè    H…À›ıÿÿƒ}Ì
„‘ıÿÿL½ ÿÿÿL‰ÿè    WÀ)E€)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿL‰÷Hu è    H…À<ıÿÿL‰ùL‹}¨H‹}¸L‰şº   è    H…Àu!H‹}¸L‰şº   H ÿÿÿè    H…À„şüÿÿH‹…0ÿÿÿL‹eÀL‹8A€uAŠG
H‹M°:A
wAŠG
A€…@  H‹M°:A
…3  HÀşÿÿH‰ßè    H‹½Ğşÿÿ¹   L‰æóH¥H‹E°Š@
A:G
 ¶ĞL‰÷H‰Şè    H…À‡  I‰ÇMïL‰çè    (…Àşÿÿ(ĞşÿÿH‹…àşÿÿ HL‰÷Hµ€şÿÿè    L‰÷è    H…Àˆ5  I‰ÅMıL‰÷HuÌè    H…Àüÿÿƒ}Ì
„üÿÿL½ ÿÿÿL‰ÿè    WÀ)E€)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿL‰÷Hu è    H…À·ûÿÿH‹u¨H‹}¸º   L‰ùè    H…À„™ûÿÿH‹…0ÿÿÿéšşÿÿ1Û:EœL½ ÿÿÿLe ƒıûÿÿérûÿÿI‰ÅëE1íHµ€şÿÿL‰÷è    H½ ÿÿÿè    H½şÿÿè    H½ şÿÿè    Hµ€şÿÿL‰÷è    L‰èHÄÀ  [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃM‰ıéôúÿÿH½ÀşÿÿH‹GÇ     è    éÙúÿÿM‰ıégÿÿÿIÇÅÿÿÿÿé[ÿÿÿM‰åéSÿÿÿóúL‹    L3$UH‰åASAWAVAUATHì¸   I‰öI‰ÿWÀ)E€)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿLe¸L‰çè    Hu˜L‰ÿè    L‰ÿL‰æè    H…À~xI‰Äè    H‹xpH‹uÀH ÿÿÿº   è    H…ÀtRH½ ÿÿÿè    Hu˜L‰ÿè    L‰ÿè    I‰ÅH…ÀxGH‹uÈL‰ÿè    H…À~nMåIÅE¸MÈANAë&Lu˜L‰ÿL‰öè    L‰ÿL‰öè    E1íH}¸è    L‰èHÄ¸   A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃI‰ÅëÃóúL‹    L3$UH‰åASAWAVAUATSHƒìPH‰óI‰şWÀ)E°)E Hu€è    H5    L‰÷è    I‰ÇH…Àš   Le L‰çè    H‹    I‰D$L‰÷è    I‰ÅH…Àx`H‰]ÈH‹u°L‰÷è    H‹ØH…À~ML‰÷è    I‰ÄH…ÀxBH5    L‰÷è    H…À~3MıMåIÅIİ(E (M°H‹EÈH M‰ïë'M‰ïëI‰ßëM‰çëI‰ÇH} è    Hu€L‰÷è    Hu€L‰÷è    L‰øHƒÄP[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATHì    I‰öI‰üWÀHµ@ÿÿÿ)FP)F@)F0)F )F)HÇF`    )EĞ)EÀ)E°è    H…À~+I‰Çè    H…Àt_Ç    H‰ÇHƒÇHµ@ÿÿÿ¹   óH¥ë<Hu°L‰çè    I‰ÇH…À~Kè    H…Àt1Ç    (E°(MÀ(UĞ@HP(I‰ëH½@ÿÿÿè    ë	H}°è    IÇÇÿÿÿÿL‰øHÄ    A\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHìà   H‰óI‰ş1ÀH‰EÀH‰…PÿÿÿWÀ)…@ÿÿÿ)…0ÿÿÿ)… ÿÿÿ)…ÿÿÿ)… ÿÿÿ)…ğşÿÿHµXÿÿÿè    H5    L‰÷è    I‰ÅH…À  H5    L‰÷è    I‰ÇH…Àˆ  •EÏL‰÷è    I‰ÄH…Àó   HuL‰÷è    H…ÀÙ   H‰E˜L‰÷è    H…ÀÄ   H‰E¨H‰] HµxÿÿÿL‰÷è    H‹ØH…À©   H½xÿÿÿè    H…À„”   H‰E°H½xÿÿÿè    L‰÷è    H…À~oH‰E¸HuÀL‰÷è    H…À~ZMïMçL‹e˜Le¨IÜMüLe¸IÄH‹UÀH‹ML½ğşÿÿL‰ÿH‹u°è    ŠEÏAˆ¹   H‹} L‰şóH¥L‰ãë!L‰ëëL‰ûëH‹ØëL‰ãHµXÿÿÿL‰÷è    HµXÿÿÿL‰÷è    H‰ØHÄà   [A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì€   I‰÷I‰şHÇE¸    WÀ)E )E)E€HµPÿÿÿè    HµpÿÿÿL‰÷è    I‰ÅH…À(  H‹…xÿÿÿH;    tH;    tE1íéï  H‰EÈH‹µpÿÿÿH}€è    L‰÷è    I‰ÄH…À­   MìH5    L‰÷è    I‰ÅH…Àˆ©  „‘   L‰÷è    H…ÀN  L‰}ÀL}˜MåIÅ1ÿè    I‰L‰÷H‰Æè    I‰ÄH…À~MM‹?L‰÷è    H‹ØH…ÀˆH  L‰÷H5    è    H‰ÁH…Àˆ2  IƒÇ(MåIİH…Ét¢LéL‹}Àéä   M‰åé  1ÿè    H‰E˜L‰÷H‰Æè    H…À¯   LàH‹ØL¥PÿÿÿL‰÷L‰æè    L‰÷L‰æè    L‰÷è    H…À€   I‰ÅHu¸L‰÷è    H…À~lH‰ÙLéHÁL‹e¸M…ätcH‰MÀ1ÿè    H‰E¨L‰ãH‹}¨I‹T$H‹rH‡ĞHƒÀpH‡Ğè    H…ÀtfL‰çè    I‰ÄH…ÀuÏH‰ßè    H‹EÈH‹MÀëI‰Åë?H‹EÈH‰ÙëH‹EÈH;    ”E€(E(M AO AG(E€AI‰Íë I‰İëI‰ÍH}€è    HµPÿÿÿL‰÷è    HµPÿÿÿL‰÷è    L‰èHÄ€   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìPI‰÷I‰şLeÀIÇ$    Hu€è    L‰÷L‰æè    I‰ÄH…À“   L‰÷è    I‰ÅH…ÀxfL‰}ÈH] H‰ßè    L‹}ÀH‹    H‰ßL‰şè    H‹{L‰şè    H‹{è    L‰÷H‰Æè    H…À~5MåIÅE M°H‹EÈH ëH} è    Hu€L‰÷è    M‰ìëI‰ÄHu€L‰÷è    L‰àHƒÄP[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHìĞ  I‰ôI‰şHuè    H5    L‰÷è    H…Àˆ¥  tSH5    L‰÷è    I‰ÅH…À¤  Hu°L‰÷è    I‰ÇH…À}  H]°H‰ßè    I‰$H‰ßè    Mıél  HuÌL‰÷è    I‰ÅH…ÀT  ‹}Ìƒÿ:tè    „À„(  H½ ÿÿÿH ûÿÿº   1öè    ƒ}Ì:u>L‰÷L‰îè    H…Àˆï   HuÌL‰÷è    I‰ÇH…Àä   ‹}Ìè    „À…Ô   ëH½ ÿÿÿL‰öL‰êè    H…Àˆª   HuÌL‰÷è    H…À~pH‹Ø‹}Ìƒÿ.tƒÿ[„Ô   è    „Àt%ëQL‰÷HuÌè    H…À~@I‰Ç‹}Ìè    „Àt1LûH½ ÿÿÿL‰öH‰Úè    H…ÀxJIİL‰÷HuÌè    H‹ØH…À“H½ ÿÿÿL}°L‰şè    L‰ÿè    I‰$L‰ÿè    ëI‰ÅëE1íëI‰ÇHuL‰÷è    M‰ıHuL‰÷è    L‰èHÄĞ  [A\A]A^A_A[]L3$L;    tÌÌÌÃHuÌL‰÷è    H…Àsÿÿÿƒ}Ì]…iÿÿÿH½ ÿÿÿL‰öH‹Ğè    I‰ÇH…ÀˆÿÿÿMıéCÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHìĞ   I‰÷I‰şHu˜è    L-    L‰÷L‰îè    I‰ÄH…Àl  H½ÿÿÿ¾   è    H5    L‰÷è    H‹ØH…Àˆ%  „Ø   L‰}ÀIÜ1ÉHƒûA”ÇH    HDÈH‰M¸L-    L‰÷L‰îè    H‹ØH…Àˆá   A€÷H…Û•ÀDøuL‰÷H‹u¸è    H‹ØH…Àˆº   H…Û…™   L‰÷HuÈè    H…À~H‹Ø‹uÈH½ÿÿÿè    ë$L‰÷HuÏè    H‹ØH…À~x¶uÏH½ÿÿÿè    H…Àx`IÜL‰÷L‰îè    H‹ØH…À‰lÿÿÿëGH5    L‰÷è    H‹ØH…Àx0…ÿÿÿL‰}ÀE1ÿ1ÀH‰E¸é ÿÿÿLãH½ÿÿÿH‹uÀè    ëH‹ØHu˜L‰÷è    H½ÿÿÿè    I‰ÜHu˜L‰÷è    L‰àHÄĞ   [A\A]A^A_A[]L3$L;    tÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì@H‰óI‰şHuè    H5    L‰÷è    I‰ÄH…À~dL‰÷è    I‰ÅH…Àx8L‰÷è    H…Àx0MåIÅH5    L‰÷è    I‰ÄH…Àxt\MìHÇ    ëM‰ìëI‰ÄH‹;è    HuL‰÷è    HuL‰÷è    L‰àHƒÄ@[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÃHÇ    1ÿè    H‰L‰÷H‰Æè    I‰ÄH…À~”I‰ßH‰]ÈL‰÷è    H‹ØH…Àˆj  L‰÷è    H…ÀˆT  MìIÜIÄL‰÷H5    è    H…Àˆ3  u|L‰÷H5    è    H‹ØH…Àˆ  tgM‹?L‰÷è    I‰ÅH…Àˆ  L‰÷è    H…Àˆì   Iƒï€LãIÅIİ1ÿè    I‰L‰÷H‰Æè    I‰ÄH…ÀH‹]ÈAÿÿÿéÉşÿÿIÄéÕşÿÿH5    L‰÷è    H‹ØH…À—   L‰÷è    I‰ÅH…Àˆˆ   L‰÷è    H…ÀxqH‰E°I‹7HƒÆpL‰÷è    I‰ÇH…À~lL‰÷è    H…ÀxIH‰E¸L‰÷è    H…Àx8H‰EÀH5    L‰÷è    H…À~ LãLm°IİLm¸LmÀMıIÅM‰ìé(şÿÿI‰ÄëI‰ÜëM‰ìH‹]ÈéşıÿÿM‰üëòóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰ôI‰şHu°è    H5    L‰÷è    I‰ÇH…À~NHuÔL‰÷è    I‰ÅH…À~&H5    L‰÷è    H…À~‹MÔA‰$MıIÅM‰ïëM‰ïëI‰ÇHu°L‰÷è    Hu°L‰÷è    L‰øHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰ôI‰şHu°è    H5    L‰÷è    H…Àxt:E1ÿëI‰ÇHu°L‰÷è    L‰øHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÃH5    L‰÷è    I‰ÅH…Àx@HuÔL‰÷t?è    I‰ÇH…À  ‹MÔ1Àƒùrbƒùmzƒù0„·   ƒùU„˜   éĞ   M‰ïéoÿÿÿè    I‰ÇH…Àˆ^ÿÿÿƒ}Ô"„LÿÿÿL‰÷L‰şè    I‰ÇH…Àˆ=ÿÿÿ‹EÔA‰$é1ÿÿÿƒùt0ƒùstdƒùtu{ÇEÔ	   ¹	   ëkƒùntZƒùrucÇEÔ   ¹   ëSƒùutƒùvuKÇEÔ   ¹   ë;HuÔL‰÷è    H…Àx;‹MÔë'ÇEÔ    1ÉëÇEÔ    ¹    ëÇEÔ
   ¹
   1ÀA‰$MïIÇé˜şÿÿI‰ÇHu°L‰÷è    é„şÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰şHu¨è    H5    L‰÷è    H…À»   I‰ÇLeÌL‰÷L‰æè    H…À’   I‰ÅL‰÷H‰Æè    H…À~Mïƒ}Ì
uĞLe¨H]ÌL‰÷L‰æè    L‰÷è    H…À~gI‰ÅL‰÷H5    è    H…À~?MıIÅM‰ïL‰÷H‰Şè    I‰ÅH…À~L‰÷L‰îè    H…Àx$Mïƒ}Ì
uÔI‰ÅM…íy”ëHu¨L‰÷è    ëE1ÿHu¨L‰÷è    L‰øHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì  I‰ôI‰şHu¨è    HµÀşÿÿL‰÷è    I‰ÇH…À   L‰÷è    H‹ØH…À~eH5    L‰÷è    I‰ÅH…À~SL‰eÈL‰÷è    I‰ÄH…À~DHµ0ÿÿÿL‰÷è    H…À~rLûMåIİIÅHµÀşÿÿºà   H‹}Èè    M‰ïëI‰ßëM‰ïëM‰çHu¨L‰÷è    Hu¨L‰÷è    L‰øHÄ  [A\A]A^A_A[]L3$L;    tÌÌÌÃI‰ÇëÈóúL‹    L3$UH‰åASAWAVAUATSHƒì`I‰÷I‰şWÀ)E°)E Hµxÿÿÿè    H‹    HuÈH‰L‰÷è    I‰ÄH…Àˆ  tL‰÷è    I‰ÅH…Àß   MåëE1íH5    L‰÷è    H‹ØH…À·   L‰÷è    I‰ÄH…À°   H‹uÈH} è    H…À„š   H} è    L‰÷H‰Æè    H…À~~H‹M H;    „Â   H‰E˜H} è    HuÈH‰Çè    H…Àt<H‹E H;EÈH‹E˜„’   H=    è    H} è    H=    è    H}Èè    IÇÄÿÿÿÿëI‰ÜëM‰ìëI‰ÄH} è    HµxÿÿÿL‰÷è    HµxÿÿÿL‰÷è    L‰àHƒÄ`[A\A]A^A_A[]L3$L;    t
ÌÌÌÌÌÌÌÌÌÌÃLëLãH‡ØHØH‡ØLe L‰çè    A($A(L$AAOI‰Üë’óúL‹    L3$UH‰åASAWAVATHƒìI‰ÖI‰ôI‰ÿHuÜè    H…À~a‹uÜL‰çè    H‰EĞH…Àx5I‰ÄH=   r.H=    è    H}Ğè    H=    è    HÇÀÿÿÿÿë1ÀëHuÜL‰ÿè    H…À~Eˆ&HƒÄA\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVPI‰÷I‰şHuäè    H…À~1‹MäqĞ²Ğƒş
rqŸ²©ƒşr
q¿²Éƒşw1 ÑAˆL‰÷H‰Æè    HƒÄA^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃ1ÀëÛóúL‹    L3$UH‰åASAWAVPI‰÷I‰şHuäè    H…À~"‹Mä‹Ñƒâøƒú0u€ÁĞAˆL‰÷H‰Æè    ë1ÀHƒÄA^A_A[]L3$L;    tÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVPI‰÷I‰şHuäè    H…À~"‹Mä‹Ñƒâşƒú0u€ÁĞAˆL‰÷H‰Æè    ë1ÀHƒÄA^A_A[]L3$L;    tÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVPI‰÷I‰şHuäè    H…À~ ‹MäQÆƒúös1Àë€ÁĞAˆL‰÷H‰Æè    HƒÄA^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìPI‰÷I‰şHuˆè    H5    L‰÷è    I‰ÅH…Àˆb  tL‰÷è    I‰ÄH…ÀˆI  MìëE1äHuÏL‰÷è    H‹ØH…À~eLãHuÏ¶ò*ÀòEÀL‰÷è    I‰ÅH…À~JLeÏòMÀLë¶EÏWÀò*ÀòY    òXÈòMÀL‰÷L‰æè    òMÀI‰ÅH…ÀÉëI‰İéÃ   òMÀòMÀM…íˆ°   H5    L‰÷è    I‰ÄH…À’   IÜHuÏL‰÷è    H…À¿   »
   LmÏò]ÀIÄ¶EÏWÀò*Àë	ÌÌÌÌÌÌÌÌÌfHnËfb    f\    f(ÑfÑòXÑëÌÌÌÌÌò^ÂòXØò]ÀHÛH›L‰÷L‰îè    ò]ÀH…À–ëJM‰åHuˆL‰÷è    HuˆL‰÷è    L‰èHƒÄP[A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃò]Àò]ÀH5    L‰÷è    H…ÀÎ   I‰ÅMåH5    L‰÷è    H…À~
ò    ë#H5    L‰÷è    H‰Á1ÀH…ÉHOÁò    òE°IÅHuÏL‰÷è    fWÉH…À~8LeÏIÅ¶EÏWÀò*ÀòY    òXÈòM¨L‰÷L‰æè    òM¨H…ÀÌòE°òYÁWÉòZÈó    è    óZÀòMÀòYÈòMÀM‰ìH5    L‰÷è    I‰ÅH…Àˆ³şÿÿHu¼L‰÷è    H…À~‹}¼è    „ÀtMåWÀòZEÀóAéşÿÿE1íéyşÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰÷I‰şHu è    H5    L‰÷è    I‰ÄH…ÀˆK  tL‰÷è    H‹ØH…Àˆˆ   Lãë1ÛHuÏL‰÷è    I‰ÅH…À~eHuÏ¶ò*ÀòEÀIİL‰÷è    I‰ÄH…À~RH]ÏòMÀMå¶EÏWÀò*ÀòY    òXÈòMÀL‰÷H‰Şè    òMÀI‰ÄH…ÀÉëM‰ìé­  I‰Üé¥  òMÀòMÀM…äˆ’  H5    L‰÷è    I‰ÄH…Àw  MìHuÏL‰÷è    H…À~z»
   LmÏò]ÀIÄ¶EÏWÀò*Àë	ÌÌÌÌÌÌÌÌÌfHnËfb    f\    f(ÑfÑòXÑëÌÌÌÌÌò^ÂòXØò]ÀHÛH›L‰÷L‰îè    ò]ÀH…À–ëò]Àò]ÀH5    L‰÷è    H…À~`I‰ÅMåH5    L‰÷è    I‰ÄH…Àˆª   tIMìHÇÁÿÿÿÿ1ÛH¯ÙWÉëÌÌÌÌÌÌÌòH*Ëò    è    òMÀòYÈf(ÁëòEÀòAëkH5    L‰÷è    I‰ÄH…ÀxHMìHuÏL‰÷è    ¹   H…À~‘1ÛLmÏIÄH›¶MÏHAL‰÷L‰îè    H…Àá¹   édÿÿÿHu L‰÷è    Hu L‰÷è    L‰àHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìpI‰÷I‰şHu€è    H5    L‰÷è    I‰ÄH…Àˆ  tL‰÷è    H‹ØH…Àx}Lãë1ÛHuÏL‰÷è    I‰ÅH…À~ZHuÏ¶f‰E¾ßE¾Û}ÄIİL‰÷è    I‰ÄH…À~FH]ÏÛmÄMå¶EÏf‰EÀØ    ŞEÀÛ}ÄL‰÷H‰Şè    ÛmÄI‰ÄH…ÀÓëM‰ìéŒ  I‰Üé„  ÛmÄÛ}ÄM…äˆu  H5    L‰÷è    I‰ÄH…ÀZ  MìHuÏL‰÷è    H…ÀL‰}°~b»
   L=    LmÏÛmÄIÄ¶EÏëÌÌf‰EÂH‰] 1ÀH…Û˜Àßm AØ‡ëÌÌÌÌÌÌÌŞ}ÂŞÁÛ}ÄHÛH›L‰÷L‰îè    ÛmÄH…À³ëÛmÄÛ}ÄH5    L‰÷è    H…À~WI‰ÅMåH5    L‰÷è    I‰ÄH…Àˆ¥   t@MìHÇÁÿÿÿÿ1ÛL‹}°H¯ÙH‰]¨ßm¨Ù    Û<$Û|$è    ÛmÄŞÉëL‹}°ÛmÄAÛ?ëoH5    L‰÷è    I‰ÄH…ÀxLMìHuÏL‰÷è    ¹   H…À~š1ÛLmÏL‹}°IÄH›¶MÏHAL‰÷L‰îè    H…Àá¹   émÿÿÿHu€L‰÷è    Hu€L‰÷è    L‰àHƒÄp[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì   I‰÷I‰şH½Ğıÿÿº`  1öè    Hµ8ÿÿÿL‰÷è    H5    L‰÷è    I‰ÄH…ÀM  L‰÷è    I‰ÅH…Àˆ  L‰÷è    H‹ØH…Àˆ  HµĞıÿÿL‰÷è    H…Àå  H‰…XÿÿÿL‰÷è    H…ÀˆÍ  H‰…`ÿÿÿL‰÷è    H…Àˆµ  H‰…hÿÿÿH5    L‰÷è    H…Àˆ–  H‰…pÿÿÿL‰÷è    H…Àˆ~  H‰…xÿÿÿL‰÷è    H…Àˆf  H‰E€Hµ@şÿÿL‰÷è    H…ÀJ  H‰EˆL‰÷è    H…Àˆ5  H‰EL‰÷è    H…Àˆ   H‰E˜H5    L‰÷è    H…Àˆ  H‰E L‰÷è    H…Àˆï   H‰E¨L‰÷è    H…ÀˆÚ   H‰E°Hµ°şÿÿL‰÷è    H…À¾   H‰E¸L‰÷è    H…Àˆ©   H‰EÀL‰÷è    H…Àˆ”   H‰EÈH5    L‰÷è    H…À~|MåHXÿÿÿLëH‹`ÿÿÿHhÿÿÿHpÿÿÿHxÿÿÿHÙH‹U€HUˆHUHU˜H‡ÑHÑH‡ÑH‹M HM¨HM°HM¸HMÀHÑL‹eÈIÄIÌHµĞıÿÿº`  L‰ÿè    ë(I‰ÄëM‰ìëI‰ÜH½Ğıÿÿè    Hµ8ÿÿÿL‰÷è    Hµ8ÿÿÿL‰÷è    L‰àHÄ   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATHì°   I‰÷I‰şHu¨è    Hµ0ÿÿÿL‰÷è    I‰ÄH…À°   ƒ½0ÿÿÿ……   H‹…@ÿÿÿH…À•ÁH;    •À„ÁujH‹…HÿÿÿH;    uZH‹…PÿÿÿH…ÀtNƒ8uIH½0ÿÿÿè    Hu¨L‰÷è    HuÈL‰÷è    I‰ÄH…À~/H…@ÿÿÿÇ@ğ"   H‹MØH‰HEÈ Hµ0ÿÿÿ¹   L‰ÿóH¥Hu¨L‰÷è    L‰àHÄ°   A\A^A_A[]L3$L;    tÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒì8I‰ôI‰ÿH}¨1öè    Hu°L‰ÿè    I‰ÆH…À~VH‹}°è    H‰E¨L‰ÿè    I‰ÅH…ÀxBHu¸L‰ÿè    H…À~6MõIÅE¨M¸UÈAT$ AL$A$M‰îë:H=    ëM‰îë#H=    I‰Æè    L‰ÿè    H=    è    H}¨è    L‰ğHƒÄ8A\A]A^A_A[]L3$L;    t
ÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰õI‰şHu è    H5    L‰÷è    I‰ÇH…À  L‰÷è    I‰ÄH…Àx<L‰÷è    H…Àx>MüIÄH5    L‰÷è    I‰ÇH…Àx0t:MçIÇE     éÉ  H=    M‰çé   H=    I‰Çé‘  H=    é…  IÇE     1ÿè    I‰E L‰÷H‰Æè    H‹ØH=    H…Àˆ  L‰mÈL‰mÀIÇÇÿÿÿÿH…Û„÷   L‰÷è    I‰ÅH…Àˆï   L‰÷è    H…Àˆñ   LãLëH‡ØHØH‡ØL‰÷H5    è    H…ÀˆÓ   …Ü   L‰÷H5    è    I‰ÅH…ÀˆÌ   „Ò   H‹EÀH‹ H‰EÀL‰÷è    I‰ÄH…Àˆ‡  L‰÷è    I‰ÇH…Àˆ  L‰èL‹mÀIƒí€HØMüIÄ1ÿè    L‰mÀI‰E L‰÷H‰Æè    H‹ØH…À‰ÿÿÿH=    I‰ßéI  I‰ßéE  H=    é5  M‰ïL‹mÈH=    é&  H=    ëH=    I‰Çé
  HØI‰Çé  H=    éğ   H5    L‰÷è    I‰ÅH…ÀˆÎ   „  L‰÷è    I‰ÄH…Àˆ  L‰÷è    H…Àˆ  IİIÄMìH‹EÀH‹0HƒÆpL‰÷è    I‰ÅH…Àˆğ   „ó   L‰÷è    H‹ØH…Àˆë   L‰÷è    H…Àˆæ   MìH‡ØHØH‡ØLãH5    L‰÷è    H…ÀˆË   „Ñ   H‡ØHØH‡ØI‰ßë=H=    M‰çëH=    ë
H=    M‰ïL‹mÈè    I‹} è    Hu L‰÷è    Hu L‰÷è    L‰øHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÃH=    ëªH=    ë‰H=    éƒşÿÿH=    ë‰H=    é€ÿÿÿH=    é#şÿÿH=    éVşÿÿH=    éJşÿÿH=    éPÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHì   I‰öI‰üHÇEÈ    H5    è    H‹ØH…Àˆ!  L‰çè    I‰ÅH…Àx-IİH5    L‰çè    H‹ØH…Àˆó   tLë1ÀI‰éë   1ÿL‰ëéÜ   Hµ@ÿÿÿL‰çè    H‹ØH…À¿   L‰uÀL}ÈLµ@ÿÿÿL‰e¸1ÿè    I‰¹   H‰ÇL‰öóH¥M‹?L‰çè    I‰ÆH…Àˆ¶   L‰çH5    è    H…Àˆ™   LëLóH‡ØHØH‡ØH…ÀLµ@ÿÿÿL‹e¸uL‰çH5    è    H…ÀxfuwL‰çè    I‰ÅH…Àx^Iƒï€IİL‰çL‰öè    H‹ØH…ÀVÿÿÿëA1ÿè    H‰ØHÄ   [A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃH‹ØëL‰óëL‰ëH‹}Èë»H‡ØHØH‡ØH‹EÈL‹uÀé·şÿÿóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰ôI‰şHu¸è    L‰÷è    I‰ÇH…ÀxvtL‰÷è    I‰ÅH…À~UMıëE1íIt$L‰÷è    I‰ÇH…À~!AÇ$   MïL‰÷è    1ÉH…ÀHOÈIÏë&uL‰÷L‰æè    I‰ÇH…ÀÒëM‰ïHu¸L‰÷è    Hu¸L‰÷è    L‰øHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHìà   I‰õI‰şH½øşÿÿº   1öè    Hu L‰÷è    HuÈL‰÷è    I‰ÇH…À  H‹}Èè    „À„¤   L¥øşÿÿ¾   L‰çè    HuÈL‰çè    H…ÀxzL‰mÀI‹FHH‰E°L-    HøşÿÿL‰÷L‰îè    H…À~fL‰÷HuÈè    H…À~UI‰ÄH‹}Èè    „ÀtEI‹FHH‰E°H‰ßL‰îè    H…ÀxMçIÿÇH‰ßHuÈè    H…Ày I‰ÇëOE1ÿHu L‰÷è    ë>Hu L‰÷è    H½øşÿÿLmˆL‰îè    L‰ïè    I‰ÄL‰ïè    M…ätQH‹EÀL‰ Hu L‰÷è    H½øşÿÿè    L‰øHÄà   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃIÇÇÿÿÿÿé^ÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHìà  H‰óI‰şHuˆè    H5    L‰÷è    H…Àˆf  t^H5    L‰÷è    I‰ÇH…ÀˆL  Hu¨L‰÷è    I‰ÅH…Àx]H‰]ÀLe¨L‰çè    H‹ØL‰çè    H…ÛtEMïH‹EÀH‰é  HuÌL‰÷è    I‰ÇH…Àˆñ   t‹}Ìè    „ÀtE1ÿéÛ   M‰ïéÓ   IÇÇÿÿÿÿéÇ   L¥øşÿÿHğúÿÿº   L‰ç1öè    L‰çL‰öL‰úè    H…Àˆ   H‰]ÀHuÌL‰÷è    H…À~CH‹ØL­øşÿÿLeÌ‹}Ìè    „Àu)L‰ïL‰öH‰Úè    H9ØuNIßL‰÷L‰æè    H‹ØH…ÀËH½øşÿÿLe¨L‰æè    L‰çè    I‰ÅL‰çè    M…í„)ÿÿÿH‹EÀL‰(ëI‰ÇHuˆL‰÷è    HuˆL‰÷è    L‰øHÄà  [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì€   I‰÷I‰şWÀ)E°)E HµXÿÿÿè    H‹5    L‰÷è    H‹ØH…À“  L‰½xÿÿÿL‰÷è    H…ÀZ  I‰ÅL‰÷è    H…ÀˆF  I‰ÄL‰÷è    H‰E€H…Àˆ.  H‹    H‰E H‹    H‰E¨1ÿè    I‰ÇH‰E°L‰÷H‰Æè    H‰EˆH…Àò   L‰÷è    H‰EH…Àˆİ   L‰÷è    H…ÀˆÌ   1ÿH‰E˜è    I‰‡€   HUÏL‰÷H‰Æè    H‹M˜H…Àˆ   IİLe€LeˆLeIÌIÄMìM‹¿€   1ÿè    I‰‡€   €}Ï tSI‰ÅL‰÷è    H…Àx[I‰ÇL‰÷è    H…ÀxKH‹ØAÇE    IƒÅL‰÷1öL‰êè    H…À~*MçH‡ØHØH‡ØLûI‰Ü(E (M°H‹…xÿÿÿH ë$HµXÿÿÿL‰÷è    H} è    IÇÄÿÿÿÿëI‰ÜHµXÿÿÿL‰÷è    L‰àHÄ€   [A\A]A^A_A[]L3$L;    t
ÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìpI‰×I‰öI‰üHµ`ÿÿÿè    H‹5    L‰çè    I‰ÅH…Àˆ,  uH‹5    L‰çè    I‰ÅH…Àˆ  L‰m°HÇE¸    L‰çè    I‰ÅH…Àˆï  L‰} L‰u¨HE¸H‰EÀL‰eÈL‰çè    H‹ØH…Àˆá  1ÿè    H‹MÀH‰L‰çH‰Æè    I‰ÆH…À°  L‰çè    I‰ÇH…Àˆ¥  L‰u˜L‰çè    I‰ÆH…Àˆ„  L‰çH‹5    è    I‰ÄH…Àˆ`  Lm°IİMıLm˜MõM…ä…T  H‹}ÈH5    è    I‰ÄH…Àˆ*  …2  L‹eÈL‰çH‹5    è    H‹ØH…Àˆ  ±…q  L‰çH5    è    H‹ØH…Àˆ÷   uL‰çH5    è    H‹ØH…ÀÚ   L‰çè    I‰ÇH…ÀˆÃ   L‰çè    I‰ÆH…Àˆ¦   L‰çH‹5    è    I‰ÄH…Àˆ‚   LëM÷IßM…ä…İ   H‹}ÈH5    è    I‰ÄH…ÀxX…¿   L‹eÈL‰çH‹5    è    H‹ØH…ÀxL…ª   L‰}°H‹EÀH‹ Hƒè€H‰EÀL‰çè    I‰ÅH‹ØH…À‰'şÿÿëE1öëL‰ãL‹eÈëL‰óë1ÉëoL‰ûL‹u¸I‰İHµ`ÿÿÿL‰çè    Hµ`ÿÿÿL‰çè    M…ötL‰÷è    L‰èHƒÄp[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÃ1ÉM‰ıëI‰ÜëI‰ÜM‰ı±H‹E ˆL‹u¸L‰÷è    I‰ÇH}€H‰Æè    H…ÀtmL‰uÀM…ÿ~:1ÛL‹uÀH‹}ˆHß¹   L‰öóH¥L‰÷è    L‰÷è    I‰ÆH‡ØHƒÀpH‡ØIÿÏuÌMåH‹M¨Ç   E€AH‹EH‰A L‹eÈL‹uÀéÿÿÿIÇÅşÿÿÿL‹eÈéøşÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì`I‰÷I‰şHµxÿÿÿè    H5    L‰÷è    I‰ÅH…Àˆ‰   H5    L‰÷è    I‰ÄH…ÀˆZ  ttHu°L‰÷è    H‹ØH…À=  MìM…ít4Lm°L‰}ÈL}˜L‰ïL‰şè    L‰ïè    I‹GI‰EAL‹}ÈA)E IÜH‹EÀI‰G(E°Aéû   M‰ìéó   H5    L‰÷è    I‰ÄH…ÀˆÉ   …kÿÿÿH5    L‰÷è    I‰ÄH…Àˆ·   tYHu°L‰÷è    H‹ØH…À‹   MìM…ít‚L‰}ÈL}°Lm˜L‰ÿL‰îè    L‰ÿè    I‹EI‰GAE A)L‹}ÈéIÿÿÿH5    L‰÷è    I‰ÄH…ÀxEuH5    L‰÷è    I‰ÄH…Àx,tgHu°L‰÷è    H‹ØH…ÀuÿÿÿI‰ÜHµxÿÿÿL‰÷è    HµxÿÿÿL‰÷è    L‰àHƒÄ`[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃH5    L‰÷è    I‰ÄH…Àx¬…|ÿÿÿHu°L‰÷è    H…À~:I‰ÄM…ít*H]°Hu˜H‰ßè    H‰ßè    HM˜H‹AH‰C)MìéKşÿÿE1äéDÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰ôI‰şHu¨è    HuÏL‰÷è    H‹ØH…À~<L‰çè    ¶uÏL‰çL‰âè    …ÀtPL‰çè    Hu¨L‰÷è    H‡ØHÇÀÿÿÿÿH‡ØHu¨L‰÷è    H‰ØHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÃL-    L‰÷L‰îè    H…ÀxSL‰÷HuÏè    I‰ÇH…À~B¾   L‰çL‰âè    …À…mÿÿÿH‡ØHcØH‡ØIÇ¶uÏL‰çL‰âè    L‰û…Àt¢éFÿÿÿI‰Ç…Û‰ÛINßéWÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰ôI‰şHu¨è    HuÏL‰÷è    H‹ØH…À~<L‰çè    ¶uÏL‰çL‰âè    …ÀtPL‰çè    Hu¨L‰÷è    H‡ØHÇÀÿÿÿÿH‡ØHu¨L‰÷è    H‰ØHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÃL-    L‰÷L‰îè    H…ÀxSL‰÷HuÏè    I‰ÇH…À~B¾   L‰çL‰âè    …À…mÿÿÿH‡ØHcØH‡ØIÇ¶uÏL‰çL‰âè    L‰û…Àt¢éFÿÿÿI‰Ç…Û‰ÛINßéWÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰ôI‰şHu¨è    HuÏL‰÷è    H‹ØH…À~<L‰çè    ¶uÏL‰çL‰âè    …ÀtPL‰çè    Hu¨L‰÷è    H‡ØHÇÀÿÿÿÿH‡ØHu¨L‰÷è    H‰ØHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÃL-    L‰÷L‰îè    H…ÀxSL‰÷HuÏè    I‰ÇH…À~B¾   L‰çL‰âè    …À…mÿÿÿH‡ØHcØH‡ØIÇ¶uÏL‰çL‰âè    L‰û…Àt¢éFÿÿÿI‰Ç…Û‰ÛINßéWÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì`I‰÷I‰ş1ÀH‰EÀWÀ)E°H‰E )EHµpÿÿÿè    HuÏL‰÷è    I‰ÅH…ÀÖ   L‰}¨¶uÏH}°è    H…À„…   H5    L‰÷è    H…Àˆ‘   I‰ÄH]°L}MìL‰÷HuÏè    H…À~oI‰Å¾
   H‰ßL‰úè    …Àu7¶uÏL‰ÿH‰Úè    …Àu$MåL‰ÿè    L‰÷H5    è    I‰ÄH…Ày ë$H}°è    HµpÿÿÿL‰÷è    IÇÅÿÿÿÿëM‰åH‹EÀH‹M¨H‰A(E°HµpÿÿÿL‰÷è    L‰èHƒÄ`[A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì   I‰÷I‰şHuè    Hu°L‰÷è    I‰ÄH…ÀˆÉ   „Ú   H5    L‰÷è    H…À¹   L‰÷è    H…Àˆ¨   H‹ØL‰÷è    H…Àˆ”   I‰ÅHµ°şÿÿL‰÷è    H…À~}H‰EÈH½ ÿÿÿ¾   è    H…À„  LãLmÈIİLe°L‰çè    H‹ØL‰çè    H‹…8ÿÿÿÇ #   H‰XH‹½8ÿÿÿHƒÇpHµ°şÿÿ¹   óH¥M‰ìë:H=    è    ëlH}°è    HuL‰÷è    Hµ ÿÿÿL‰÷è    I‰ÄH…À~Hµ ÿÿÿ¹   L‰ÿóH¥ë,H=    è    L‰÷è    H=    è    1ÀIƒüHÀI	ÄHuL‰÷è    L‰àHÄ   [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃH}°è    H½°şÿÿè    IÇÄşÿÿÿë­óúL‹    L3$UH‰åASAWAVAUATSHìÀ  H‰uÀI‰şLeÌAÇ$    Hu è    H½ÿÿÿE1ÿHûÿÿº   1öè    L‰÷L‰æè    H…À~CI‰ÅE1ÿHÿÿÿ‹}Ìè    „Àu*H‰ßL‰öL‰êè    H…Àx)MïL‰÷HuÌè    I‰ÅH…ÀÊH½ÿÿÿH‹uÀè    ëI‰ÄHu L‰÷è    M‰çHu L‰÷è    L‰øHÄÀ  [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìpI‰õI‰üHµhÿÿÿè    H5    L‰çè    I‰ÆH…À‚   1ÛH‰]ÈH‰]°L‰çè    I‰ÇH…ÀxpL‰mˆL‰çè    I‰ÅH…ÀxdH5    L‰çè    H…Àx]M÷IÅMıH…Àt\1ÛE1öH‹}ˆL‰öH‰Úè    H…À„¯  L‰÷è    H‰ßè    é   M‰õéø  E1öéÎ  1ÛE1öM‰ïéÁ  1ÛE1öI‰Çé´  1ÿè    H‰EÈL‰çH‰Æè    I‰ÇH…ÀŠ  H]°LuÈL‰eÀM‹6L‰çè    H…Àˆ3  H‰EL‰çè    H…Àˆ  H‰E˜1ÿè    H‰L‰çH‰Æè    H…Àü   H‰E L‰u¸H‹L‰çè    M‰æI‰ÄH…Àˆß   H‰]¨L‰÷è    H‹ØH…ÀˆÌ   L‰÷H5    è    H…Àˆ¹   MïL}L}˜L} IÜIÄMüH…À…¡   L‹uÀL‰÷H5    è    I‰ÇH…À   L‰÷è    I‰ÅH…Àˆ‚   L‰÷è    H…ÀxzHƒm¸€H‹]¨Hƒë€MçIÅMı1ÿè    H‹M¸H‰L‰÷H‰Æè    I‰ÇH…ÀM‰ôL‹u¸Àşÿÿë<E1ÿë?I‰Çë2M‰çë*I‰ßë%I‰ÇL‹eÀëL‹uÈH‹]°M‰åL‹eÀéşÿÿM‰ïëI‰ÇM‰ôL‹uÈH‹]°L‰÷è    H‰ßè    HµhÿÿÿL‰çè    M‰ıHµhÿÿÿL‰çè    L‰èHƒÄp[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVPI‰öI‰ÿè    H…ÀuL‰ÿL‰öè    H…Àt&HƒÄA^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃL‰ÿL‰öHƒÄA^A_A[]é    óúL‹    L3$UH‰åASAWAVAUATSHƒì@I‰ôI‰şHu˜è    Hu¸L‰÷è    I‰ÇH…ÀˆÁ   t@H5    L‰÷è    H…Àˆ˜   t+IÇAÇ$#   Lm¸L‰ïè    I‰D$L‰ïè    ëE1ÿëzL‰÷è    I‰ÅH…Àˆœ   L‰÷è    H‹ØH…À   H5    L‰÷è    H…À~/MıH‡ØHØH‡ØLëAÇ$"   E¸AD$H‹EÈI‰D$ I‰ßëI‰ÇHu˜L‰÷è    Hu˜L‰÷è    L‰øHƒÄ@[A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃM‰ïë¹I‰ßë´óúL‹    L3$UH‰åASAWAVAUATHƒì8I‰ôI‰şHu è    HuÀL‰÷è    I‰ÇH…Àx\t=H5    L‰÷è    I‰ÅH…À~+MïAÇ$#   LmÀL‰ïè    I‰D$L‰ïè    ëE1ÿëH}Àè    Hu L‰÷è    M‰ïHu L‰÷è    L‰øHƒÄ8A\A]A^A_A[]L3$L;    tÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì   I‰ôI‰şHu°è    Hµ@ÿÿÿL‰÷è    I‰ÇH…À~gL‰÷è    I‰ÅH…ÀxJL‰÷è    H‹ØH…Àx?H5    L‰÷è    H…À~0MıH‡ØHØH‡ØLëHµ@ÿÿÿ¹   L‰çóH¥I‰ßëM‰ïëI‰ßëI‰ÇHu°L‰÷è    Hu°L‰÷è    L‰øHÄ   [A\A]A^A_A[]L3$L;    tÌÌÃóúL‹    L3$UH‰åASAWAVATI‰öI‰üè    I‰ÇL‰çH‰Æè    I‰ÄH…ÀyL‰ÿè    E1ÿM‰>L‰àA\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATI‰öI‰üè    I‰ÇL‰çH‰Æè    I‰ÄH…À~M‰>ëL‰ÿè    L‰àA\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATI‰öI‰ü¿    è    I‰ÇL‰çH‰Æè    I‰ÄH…À~M‰>ëL‰ÿè    L‰àA\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATI‰öI‰ÿ1ÿè    H…ÀtI‰ÄL‰ÿH‰Æè    I‰ÇH…À~M‰&ëIÇÇÿÿÿÿëL‰çè    L‰øA\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHì°   I‰õI‰şWÀ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿHÇ…pÿÿÿ    Hµ ÿÿÿè    H5    L‰÷è    H‹ØH…ÀÎ  HuÀL‰÷è    I‰ÄH…ÀÕ   H5    L‰÷è    H…ÀÈ   H‰EÈHÇE°    L‰÷è    I‰ÇH…Àˆ³   L‰÷è    H…Àˆ—   IÜH‹MÈLùLáHÁH‰MÈH5    L‰÷è    H‹ØH…Àx„“   H‹uÀH½@ÿÿÿè    H…À„õ  L‰m H]ÈE1íE1ÿH‹…pÿÿÿH‹M H‰A0(…@ÿÿÿ(Pÿÿÿ(•`ÿÿÿQ Iéç  E1íE1ÿL‰ãëE1íE1ÿH‹ØëE1íL‰ûëE1íE1ÿHµ ÿÿÿL‰÷è    é¯  L‰m E1í1ÿè    I‰ÇH‰E€L‰÷H‰Æè    H‹ØH…À~ÄL‰}¨L}€HE°H‰E¸M‹?L‰÷è    I‰ÄH…Àˆ¬  L‰÷è    H…Àˆ–  H‰Eˆ1ÿè    L‹m¸I‰E L‰÷H‰Æè    H…Ào  H‰EL‰}˜M‰çM‹m L‰÷è    I‰ÄH…ÀˆQ  L‰m¸L‰÷è    I‰ÅH…Àˆ>  L‰÷H5    è    H…Àˆ  H]ÈLûH]ˆH]MìIÄIÜH…À…  L‰÷H5    è    H‹ØH…Àí   L‰÷è    H…ÀˆÏ   I‰ÅL‰÷è    H…Àˆ»   L‹}˜Iƒï€Hƒm¸€LãIÅIİL‰mÈ1ÿè    I‰L‰÷H‰Æè    H‹ØH…ÀÂşÿÿéˆ   H=    è    H}Àè    H=    è    H‡ØHÇÀşÿÿÿH‡ØE1íE1ÿL‰ÿè    L‰ïè    Hµ ÿÿÿL‰÷è    H‰ØHÄ°   [A\A]A^A_A[]L3$L;    tÌÌÌÌÃH‹ØëL‰ãëL‰ëL‹m°L‹}¨éÜıÿÿH‹uÀL‹m°H½@ÿÿÿL‹}¨L‰úL‰éè    H…ÀtL‰ãéYıÿÿH=    è    H}Àè    H=    è    H‡ØHÇÀşÿÿÿH‡ØéAÿÿÿóúL‹    L3$UH‰åASPH=    è    HÇÀÿÿÿÿHƒÄA[]L3$L;    tÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì@I‰ôI‰şWÀ)E°HÇEÀ    Huè    H5    L‰÷è    H‹ØH…À~kL‰÷è    I‰ÇH…ÀxcHu°L‰÷è    I‰ÅH…À~LIßH}°è    „Àu!H=    è    H}°è    H=    è    MïH}°è    IÇ$    ëI‰ßëM‰ïHuL‰÷è    HuL‰÷è    L‰øHƒÄ@[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì@I‰÷I‰şWÀ)E°HÇEÀ    Huè    H5    L‰÷è    I‰ÅH…À~@L‰÷è    H‹ØH…Àx5Hu°L‰÷è    I‰ÄH…À~$H}°è    „ÀtLëIÜIÇ    ëM‰ìëI‰ÜHuL‰÷è    HuL‰÷è    L‰àHƒÄ@[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒìHI‰ôI‰şHÇEĞ    Hu˜è    H‹    HuÀH‰L‰÷è    I‰ÇH…ÀˆÔ   tL‰÷è    I‰ÅH…À€   MıëE1íH5    L‰÷è    I‰ÇH…À   MïH5    L‰÷è    H…ÀxKtNI‰ÅHu¸L‰÷è    H…À~VMïIÇL‹m¸L‰mĞHuÌL‰÷è    H…À~1‹}Ìè    „Àu%E1ÿë2M‰ïë-E1íëH‹uÀH}Ğè    H…ÀtL‹mĞM‰,$ëI‰ÇëIÇÇÿÿÿÿHu˜L‰÷è    Hu˜L‰÷è    L‰øHƒÄHA\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì I‰ôI‰şHu°è    H5    L‰÷è    I‰ÇH…À‡   L‰÷è    H‹ØH…À~2è    H…Àt-I‰ÅL‰÷H‰Æè    H…À~#LûH‡ØHØH‡ØM‰,$I‰ßëEI‰ßë4IÇÇşÿÿÿë+H=    I‰Çè    L‰÷è    H=    è    L‰ïè    Hu°L‰÷è    Hu°L‰÷è    L‰øHƒÄ [A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒìPI‰ôI‰şWÀL} A)G A)GA)Hu€è    L‰÷L‰şè    I‰ÇH…À†   H5    L‰÷è    I‰ÅH…À~IHu¸L‰÷è    H‹ØH…À~:H}¸è    „Àt2MıIİ(E (M°(UÀAT$ AL$A$M‰ïë&M‰ïëI‰ßëIÇÇÿÿÿÿH} è    Hu€L‰÷è    Hu€L‰÷è    L‰øHƒÄP[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì`I‰ôI‰şWÀL} A)GA)Hµpÿÿÿè    L‰÷L‰şè    I‰ÇH…ÀÊ  H} HuÏè    H…À„µ   €}Ï „i  H} è    „Àˆ¸   L‰e˜t|H]°¶ÀH‰EE1äL‰÷è    I‰ÅH…Àˆ½   MïL‰÷è    I‰ÅH…ÀˆÄ   „Ü   Mï1ÿè    H‰L‰÷H‰Æè    I‰ÅH…Àˆô   „e  MïH‹Hƒë€IÿÄL9eu’(E (M°H‹E˜H éÿ   H=    è    H} è    H=    ëH=    è    H} è    H=    è    IÇÇÿÿÿÿéœ   H=    è    H} è    H=    ëvH=    è    H} è    H=    ëXM…ät2H=    è    H} è    H=    è    è    H=    è    E1ÿë$H=    è    H} è    H=    è    M‰ïH} è    HµpÿÿÿL‰÷è    HµpÿÿÿL‰÷è    L‰øHƒÄ`[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÃH=    è    H} è    H=    éßşÿÿóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰÷I‰şHu°è    H5    L‰÷è    I‰ÄH…Àˆ  „í   H5    L‰÷è    I‰ÅH…Àˆµ   tRMåHuÕL‰÷è    I‰ÄH…Àˆ›   „£   MåHuÖL‰÷è    I‰ÄH…Àx~„†   MìŠEÕÀàEÖAˆéŒ   HuÕL‰÷è    I‰ÅH…ÀxMt\MåHuÖL‰÷è    I‰ÄH…Àx7tCMåHu×L‰÷è    I‰ÄH…Àxt*MìŠEÕŠMÖÀàÀá ÁM×Aˆë+M‰ìHu°L‰÷è    ëHu°L‰÷è    L‰÷L‰şè    I‰ÄHu°L‰÷è    L‰àHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰ôI‰şHu°è    H5    L‰÷è    I‰ÇH…À~/HuĞL‰÷è    I‰ÅH…À~‹EĞA‰$MïëHu°L‰÷è    M‰ïHu°L‰÷è    L‰øHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰ôI‰şHu è    HuÏL‰÷è    I‰ÇH…À~FL‰eÀ1ÛLmÏE1äMüH‰ØHÁà¶]ÏH‡ØHØH‡ØL‰÷L‰îè    I‰ÇH…ÀÖM…ä~
H‹EÀH‰M‰çHu L‰÷è    L‰øHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVATHƒì@I‰ôI‰şHu¨è    HuÈL‰÷è    I‰ÇH…À~,H}ÈL‰æè    „ÀuHu¨L‰÷è    IÇÇşÿÿÿH}Èè    Hu¨L‰÷è    L‰øHƒÄ@A\A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ%   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ	   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ
   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ#   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHì  I‰÷I‰ş1ÀH‰EÈWÀ)E)E€)…pÿÿÿ)…`ÿÿÿ)…Pÿÿÿ)…@ÿÿÿ)…0ÿÿÿ)E°)E )… ÿÿÿ)…ÿÿÿ)… ÿÿÿ)…ğşÿÿ)…àşÿÿ)…Ğşÿÿ)…ÀşÿÿH‰EĞLnL‰îè    H…À~I‰ÄAÇ
   é‰  L‰÷L‰îè    H…À~I‰ÄAÇ	   éj  L‰÷L‰îè    H…À~I‰ÄAÇ   éK  L‰÷L‰îè    H…À~I‰ÄAÇ   é,  Hu L‰÷è    HuĞL‰÷è    H…À~-I‰ÄH}Ğè    „À„­   L‰÷è    I‰ÅH…Àˆ¡   MåëE1íHµ@ÿÿÿL‰÷è    I‰ÄH…À~Ç…0ÿÿÿ   MìH‹}ĞH…ÿ„…   L­ÀşÿÿL‰îè    H‹uĞHUÈL‰ïè    IÇÅÿÿÿÿ„Àt>H‹}ÈHuĞH•0ÿÿÿè    H…Àt%H½0ÿÿÿè    HµÀşÿÿ¹   L‰ÿë?E1íëM‰åH½0ÿÿÿè    Hu L‰÷è    M‰ìëL­0ÿÿÿL‰ïè    ¹   L‰ÿL‰îóH¥Hu L‰÷è    L‰àHÄ  A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATHƒì(I‰ôI‰şHu¸è    L‰÷è    I‰ÇH…ÀxvtL‰÷è    I‰ÅH…À~UMıëE1íIt$L‰÷è    I‰ÇH…À~!AÇ$   MïL‰÷è    1ÉH…ÀHOÈIÏë&uL‰÷L‰æè    I‰ÇH…ÀÒëM‰ïHu¸L‰÷è    Hu¸L‰÷è    L‰øHƒÄ(A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì0I‰ôI‰şHu¨è    L‰÷è    I‰ÅH…Àˆb  tL‰÷è    I‰ÇH…À~cMïëE1ÿHuÌL‰÷è    H…À~B‹EÌƒø9Jƒø'ğ   HĞƒù
ƒö  L‰÷L‰æè    I‰ÅH…À„ô  M…í‰  éé  I‰Åéá  M‰ıéÙ  ƒøh~lƒøsÖ   ƒøu‚  ƒøt„i  ƒøu…ª  I\$L‰÷H‰Şè    I‰Å¸   M…íµ  …‰  L‰÷H‰Şè    I‰ÅH…Àµ  ¸&   é  ƒøbÉ   ƒø:„G  ƒø?„U  ƒø[…>  It$L‰÷è    I‰Å¸   éG  ƒø"„c  ƒø%„j  ƒø'…	  It$L‰÷è    I‰Å¸   é  ƒøi„õ  ƒømthƒøq…Ø   I\$L‰÷H‰Şè    I‰Å¸   M…íã  …·  L‰÷H‰Şè    I‰ÅH…Àã  ¸!   é¼  ƒøc„Ó  ƒød„  ƒøfuyI\$L‰÷H‰Şè    I‰Å¸   M…í„  …X  L‰÷H‰Şè    I‰ÅH…À}é  ƒøv„  ƒø{u,It$L‰÷è    I‰Å¸%   é5  ƒø(„y  ƒø-„øıÿÿI\$L‰÷H‰Şè    I‰Å¸   M…í  …ß  L‰÷H‰Şè    I‰ÅH…ÀÊ  ¸   éä   L‰÷L‰æè    é«ıÿÿI\$L‰÷H‰Şè    I‰Å¸   M…í³   …‡  L‰÷H‰Şè    I‰Å¸$   M…í‘   …e  L‰÷H‰Şè    I‰Å¸   ëmI\$L‰÷H‰Şè    I‰Å¸   M…íY…-  L‰÷H‰Şè    I‰ÅH…ÀY  ¸   ë5It$L‰÷è    I‰Å¸#   ëIt$L‰÷è    I‰Å¸    M…íÕ  A‰$MıL‰÷è    1ÉH…ÀHOÈIÍéÆ  I\$L‰÷H‰Şè    I‰Å¸   M…íÃ…—  L‰÷H‰Şè    I‰ÅH…Àİ  ¸   ëŸI\$L‰÷H‰Şè    I‰Å¸   M…í‚…V  L‰÷H‰Şè    I‰ÅH…À‚   ¸   é[ÿÿÿI\$L‰÷H‰Şè    I‰Å¸   M…í:ÿÿÿ…  L‰÷è    I‰ÅH…À~A1ÀéÿÿÿI\$L‰÷H‰Şè    I‰Å¸   M…íüşÿÿ…Ğ  L‰÷H‰Şè    I‰ÅH…À…·  L‰÷H‰Şè    I‰ÅH…À  ¸   é¼şÿÿL‰÷L‰æè    I‰ÅH…À…ˆûÿÿI\$L‰÷H‰Şè    I‰Å¸    M…í„şÿÿ…X  L‰÷H‰Şè    I‰Å¸   M…íbşÿÿ…6  L‰÷L‰æè    I‰ÅH…À…(ûÿÿL‰÷H‰Şè    I‰ÅH…À™   ¸   é$şÿÿ…ø   L‰÷H‰Şè    I‰ÅH…ÀAÿÿÿ…Û   L‰÷L‰æè    I‰ÅH…À…ÍúÿÿL‰÷H‰Şè    I‰ÅH…À¨   ¸#   éÉıÿÿ…   L‰÷H‰Şè    I‰ÅH…À¾üÿÿéÄşÿÿuL‰÷L‰æè    élúÿÿumL‰÷H‰Şè    I‰ÅH…ÀNÿÿÿuTL‰÷H‰Şè    I‰ÅH…À~
¸   é]ıÿÿu5L‰÷H‰Şè    I‰ÅH…À~şÿÿuL‰÷H‰Şè    I‰ÅH…ÀeşÿÿuE1íHu¨L‰÷è    Hu¨L‰÷è    L‰èHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ    A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öè    H…À~AÇ    A^A[]L3$L;    tÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ&   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ!   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ$   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúUH‰å]é    óúL‹    L3$UH‰åASAVI‰öHƒÆè    H…À~AÇ#   A^A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVPI‰öI‰ÿè    H…À~&HƒÄA^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÃL‰ÿL‰öHƒÄA^A_A[]é    óúL‹    L3$UH‰åASAWAVAUATSHƒìPI‰÷I‰şHu€è    H5    L‰÷è    I‰ÄH…Àç   HÇEÀ    L‰÷è    I‰ÅH…ÀˆÙ   L‰}¸L}ÀL‰÷è    H…ÀˆÄ   H‰E°L‰eÈ1ÿè    I‰L‰÷H‰Æè    H‹ØH…À    L‰÷è    M‰ôI‰ÆH…Àˆ   L‰çè    H…Àˆ‚   LmÈLm°MõIİIÅL‰çH5    è    H…ÀM‰æxKu`L‰÷H5    è    I‰ÄH…À~cMìM‹?Iƒï€L‰÷è    I‰ÅH…À‰?ÿÿÿëFHu€L‰÷è    ëg1Ûë8I‰Åë/I‰İë*M‰õëI‰ÅM‰æëI‰ÇH‹]ÀH‰ßè    Hƒø}_E1íëM‰åH‹]ÀHu€L‰÷è    M‰ïHu€L‰÷è    H…ÛtH‰ßè    M‰üL‰àHƒÄP[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÃI‰ÄL‰uÈMïH} H‰Æè    E1öI‰İH‹}¨L÷¹   L‰îóH¥L‰ïè    L‰ïè    I‰ÅIƒÆpIÿÌuÒE H‹E¸ L‹uÈé^ÿÿÿóúL‹    L3$UH‰åASAWAVAUATSHƒì@I‰ôI‰şHuè    H5    L‰÷è    I‰ÇH…ÀÖ   L‰÷è    I‰ÅH…Àˆ’   L‰÷è    H‹ØH…Àˆƒ   è    H…Àt~L‰÷H‰ÆH‰EÈè    H…À~xH‰E°L‰÷è    H…ÀxbH‰E¸L‰÷è    H…ÀxQH‰EÀH5    L‰÷è    H…À   MıIİLm°L‹}¸L}ÀIÇMïH‹EÈI‰$ë0M‰ïëI‰ßëIÇÇşÿÿÿëI‰ÇëH‹}ÈI‰Çè    HuL‰÷è    HuL‰÷è    L‰øHƒÄ@[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃI‰Çë½óúL‹    L3$UH‰åASAWAVHƒì(I‰şHuÀè    H5    L‰÷è    I‰ÇH…À~,HuäL‰÷è    H…À~‹}äè    „ÀuHuÀL‰÷è    E1ÿHuÀL‰÷è    L‰øHƒÄ(A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÃóúL‹    L3$UH‰åASAWAVAUATSHƒì`I‰÷I‰şWÀ)E HÇE°    Hµxÿÿÿè    H5    L‰÷è    I‰ÄH…À¹  L‰÷è    I‰ÅH…ÀxLL‰÷è    H…Àˆ}  MåIÅH5    L‰÷è    I‰ÄH…Àˆ_  t MìH‹E°I‰G(E Aé]  M‰ìé=  H5    L‰÷è    I‰ÄH…Àˆ"  L‰}˜M…ä„ğ   L‰÷è    I‰ÇH…ÀˆR  L‰÷è    H…Àˆí  H‰EÀL‰÷Hu è    H‹ØH…À*  L‰}ÈL‰÷è    I‰ÇH…Àˆ  L‰÷è    H…Àˆ¨  MìLeÈLeÀLûLãI‰İIÅL‰÷H5    è    I‰ÄH…Àˆy  …Î  L‰÷H5    è    I‰ÄH…ÀˆX  t1L‰÷è    I‰ÇH…Àˆ“  L‰÷è    H…Àˆ.  MìIÇMçM‰ıL‰÷H5    è    I‰ÄH…Àˆ
  „ä   L‰÷è    H‹ØH…ÀˆF  L‰÷è    I‰ÇH…Àˆ-  L‰÷Hu¨è    H…ÀÄ   H‰EÈL‰}ÀL‰÷è    I‰ÇH…Àˆü   L‰÷è    H…Àˆ—   MìH]ÀH]ÈLãIÇM‰ıIİL‰÷H5    è    I‰ÄH…Àxl…Á   L‰÷H5    è    I‰ÄH…ÀxOt-L‰÷è    I‰ÇH…ÀˆŠ   L‰÷è    H…Àx)MìIÇMçM‰ıL‰÷H5    è    I‰ÄH…À‰çıÿÿëI‰ÄHµxÿÿÿL‰÷è    H} è    HµxÿÿÿL‰÷è    L‰àHƒÄ`[A\A]A^A_A[]L3$L;    t	ÌÌÌÌÌÌÌÌÌÃM‰üëªI‰Üë¥MìH‹E°H‹M˜H‰A(E ë¥óúL‹    L3$UH‰åASAWAVAUATSHƒìPI‰÷I‰şWÀ)E°HÇEÀ    Hu€è    H5    L‰÷è    I‰ÄH…ÀÙ  H}°è    H…À„²  H‹}À1öè    H‹}ÀHƒÇp1öè    L‰÷è    I‰ÅH…ÀxLL‰÷è    H…Àˆt  MåIÅH5    L‰÷è    I‰ÄH…ÀˆV  t MìH‹EÀI‰G(E°AéQ  M‰ìé4  H5    L‰÷è    I‰ÄH…Àˆ  L‰} M…ä„ë   L‰÷è    H‹ØH…ÀˆN  L‰÷è    I‰ÇH…Àˆ5  H‹uÀL‰÷è    H…ÀÌ  L‰}¨I‰ßL‰÷è    H‹ØH…Àˆ
  L‰÷è    H…Àˆ   MìMüLe¨IÜM‰åIÅL‰÷H5    è    I‰ÄH…Àˆu  …Ê  L‰÷H5    è    I‰ÄH…ÀˆT  t1L‰÷è    I‰ÇH…Àˆ  L‰÷è    H…Àˆ*  MìIÇMçM‰ıL‰÷H5    è    I‰ÄH…Àˆ  „à   L‰÷è    H‹ØH…ÀˆB  L‰÷è    I‰ÇH…Àˆ)  H‹uÀHƒÆpL‰÷è    H…À¼   L‰}¨L‰÷è    I‰ÇH…Àˆø   L‰÷è    H…Àˆ“   MìH]¨LãLûI‰İIÅL‰÷H5    è    I‰ÄH…Àxl…Á   L‰÷H5    è    I‰ÄH…ÀxOt-L‰÷è    I‰ÇH…ÀˆŠ   L‰÷è    H…Àx)MìIÇMçM‰ıL‰÷H5    è    I‰ÄH…À‰ğıÿÿëI‰ÄHu€L‰÷è    H}°è    Hu€L‰÷è    L‰àHƒÄP[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃM‰üëªI‰Üë¥MìH‹EÀH‹M H‰A(E°ë¢óúL‹    L3$UH‰åASAWAVAUATSHƒì0H‰uÀI‰şHu è    HuÏL‰÷è    I‰ÄH…À~-E1ÿLmÏ1ÛLãK¿¶MÏL<AL‰÷L‰îè    I‰ÄH…ÀŞë1ÛE1ÿM…äxH‹EÀL‰8I‰ÜëHu L‰÷è    Hu L‰÷è    L‰àHƒÄ0[A\A]A^A_A[]L3$L;    tÌÌÌÌÌÌÌÌÌÌÌÌÌÌÃ             GNU   À                $@      ğ¿      ğ?      $@      €_  0C  0E              0C      0E  0C  0E              0C      0E   A   A{ } buf_parse_array_data: buf_parse_array_data_rec:  
 buf_parse_array_data_rec: { buf_parse_array_data_rec: 1 buf_parse_array_data_rec: parse buf_parse_array_data_rec: buf_parse_array_data_rec buf_parse_array_data_rec: 2 , buf_parse_array_data_rec: 3 buf_parse_array_data_rec: 4 buf_parse_array_data_rec: } buf_parse_array_dimensions_rec: dimension mismatch:  buf_parse_array_data_rec: restore buf_parse_array_dimensions: zero item size buf_parse_array_dimensions: buf_parse_array_dimensions_rec:  buf_parse_array_dimensions_rec: { buf_parse_array_dimensions_rec: 1 buf_parse_array_dimensions_rec: buf_parse_tag buf_parse_array_dimensions_rec: buf_parse_array_dimensions_rec buf_parse_array_dimensions_rec: 2 buf_parse_array_dimensions_rec: 3 buf_parse_array_dimensions_rec: 4 buf_parse_array_dimensions_rec: } buf_parse_array_dimensions_rec: restore buf_parse_do_block: buf_parse_comments buf_parse_do_block: buf_ignore_spaces buf_parse_do_block: buf_parse_do_block_inner < 0 buf_parse_do_block_inner: buf_read_1 "}" buf_parse_do_block_inner: buf_read_sym :end buf_parse_do_block_inner: buf_parse_tag buf_parse_do_block_inner: list_new buf_parse_do_block_inner: buf_parse_comments 1 buf_parse_do_block_inner: buf_ignore_spaces_but_newline 1 buf_parse_do_block_inner: buf_read_1 "}" < 0 buf_parse_do_block_inner: buf_read_sym :end < 0 buf_parse_do_block_inner: line  : missing separator:  ; buf_parse_do_block_inner: buf_parse_comments 2 buf_parse_do_block_inner: buf_ignore_spaces_but_newline 2 true false [ ] . ( ) ,| cfn _macro ' # +i cow buf_parse_cow: type mismatch:   !=  buf_parse_digit: digit overflow:  (F32) e - + f (F64) (F128) buf_parse_fn_clause: invalid pattern buf_parse_fn_clause: invalid program (Ident) _" _ 0b 0B 0o 0O 0x 0X buf_parse_list: buf_parse_comments 1 buf_parse_list: buf_ignore_spaces 1 buf_parse_list: buf_read_1 "]" 1 buf_parse_list: buf_parse_list_tag < 0 buf_parse_list: buf_parse_list_tag = 0 buf_parse_list: buf_parse_comments 2 buf_parse_list: buf_ignore_spaces 2 buf_parse_list: buf_read_1 "]" 2 buf_parse_list: buf_read_1 "," buf_parse_list: buf_parse_comments 3 buf_parse_list: buf_ignore_spaces 3 | buf_parse_list: buf_read_1 "|" buf_parse_list: buf_parse_comments 4 buf_parse_list: buf_ignore_spaces 4 buf_parse_list: buf_parse_tag < 0 buf_parse_list: buf_parse_tag = 0 buf_parse_list: buf_parse_comments 5 buf_parse_list: buf_ignore_spaces 5 buf_parse_list: buf_read_1 "]" < 0 buf_parse_list: buf_read_1 "]" = 0 buf_parse_list: invalid list buf_parse_list_tag: buf_parse_sym_str : buf_parse_list_tag: buf_parse_tag %{ => buf_parse_ptag: not implemented (Ptr) buf_parse_ptr: non-null Ptr 0x (PtrFree) ? quote buf_parse_quote: buf_parse_tag / buf_parse_special_operator:  : ident_is_special_operator : special operator not found : buf_parse_comments : buf_ignore_spaces < 0 : missing argument : buf_parse_tag < 0 : buf_parse_tag = 0 " ""
 "" """ 
""" \x \ x % buf_parse_struct: struct_init( buf_parse_struct: struct_init_from_lists  :" %Time{ tv_sec: tv_nsec: unquote( void                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                zR x  (          P   P†CNŒ‹  (   H       @   P†CRƒŒ‹ $   t       €    P†CJ‹H   $   œ       €    P†CJ‹H   ,   Ä       µ   P†COƒŒ‹q](   ô       à   P†CRƒŒ‹,          ^   P†COƒŒ‹
T,   P      æ   P†CRƒŒ‹^ (   €          P†CNŒ‹è   ,   ¬      é   P†CRƒŒ‹wT,   Ü      %   P†COƒŒ‹æX (         P   P†COƒŒ‹(   8      ğ   P†CRƒŒ‹±(   d      Ğ    P†CNŒ‹œ   (            P†COƒŒ‹Ş(   ¼           P†CKŒ‹h   (   è      Ğ    P†CNŒ‹   (            P†CNŒ‹Ñ   ,   @      º   P†CRƒŒ‹m^(   p      à   P†COƒŒ‹¡    œ      P    P†CD‹]  (   À      p   P†CQŒ‹9  (   ì         P†COƒŒ‹Ğ $         €    P†CLŒ‹F ,   @         P†CRƒŒ‹V,   p      U   P†CQŒ‹Z  (          @   P†COƒŒ‹$   Ì          P†COŒ‹â (   ô          P†CRƒŒ‹Ä(          Ğ   P†CRƒŒ‹‘(   L          P†COƒŒ‹ä ,   x      …   P†CRƒŒ‹
Q(   ¨      ğ   P†CRƒŒ‹»,   Ô      Ÿ   P†COƒŒ‹©U (         Ğ    P†CNŒ‹’   ,   0      Ê   P†CNŒ‹KT   (   `      @   P†COƒŒ‹,   Œ         P†CRƒŒ‹ÚQ ,   ¼      ä   P†COƒŒ‹vX$   ì      À    P†CLŒ‹… (         „    P†CG‹PV   $   @      p    P†CG‹A   $   h      p    P†CG‹A             p    P†CG‹,   ´      5   P†COƒŒ‹·W(   ä      à   P†COƒŒ‹ (         °   P†COƒŒ‹p(   <      ğ   P†CRƒŒ‹µ$   h          P†COŒ‹í (             P†CNŒ‹Ç   ,   ¼      ó   P†COƒŒ‹^P,   ì      É   P†CRƒŒ‹dW(   	      ğ    P†CNŒ‹³   ,   H	      ¼   P†CRƒŒ‹o\(   x	          P†CRƒŒ‹¾(   ¤	      0   P†CRƒŒ‹ó,   Ğ	      §   P†COƒŒ‹»S,    
      €   P†COƒŒ‹Ó[,   0
      *   P†COƒŒ‹~P ,   `
      *   P†COƒŒ‹~P ,   
      *   P†COƒŒ‹~P (   À
      p   P†COƒŒ‹7,   ì
      î   P†CRƒŒ‹Œ_(            P†CRƒŒ‹Ò (   H         P†COƒŒ‹Ñ(   t      v    P†CG‹k[Q,          J   P†COƒŒ‹W(   Ğ      Ğ    P†CNŒ‹   (   ü      ğ    P†CRƒŒ‹» $   (      p    P†CHŒ‹{  $   P      p    P†CHŒ‹z  $   x      p    P†CHŒ‹  $          €    P†CHŒ‹J ,   È         P†CRƒŒ‹YR   ø      @    P†CC‹Z(            P†COƒŒ‹Ó (   D      à    P†COƒŒ‹¨ (   p      `   P†CNŒ‹+  (   œ         P†COƒŒ‹Ğ (   È          P†COƒŒ‹à ,   ô         P†COƒŒ‹%Y(   $      €   P†CNŒ‹I  (   P      °    P†CNŒ‹s   (   |      À    P†COƒŒ‹ˆ $   ¨           P†CLŒ‹k     Ğ      P    P†CD‹]      ô      P    P†CD‹]            P    P†CD‹]      <      P    P†CD‹]      `      P    P†CD‹]      „      P    P†CD‹]      ¨      P    P†CD‹]      Ì      P    P†CD‹]      ğ      P    P†CD‹]            P    P†CD‹]      8      P    P†CD‹]      \      P    P†CD‹]      €      P    P†CD‹]      ¤      P    P†CD‹]      È      P    P†CD‹]      ì      P    P†CD‹]            P    P†CD‹]      4      P    P†CD‹]      X      P    P†CD‹]      |      P    P†CD‹]             P    P†CD‹]      Ä      P    P†CD‹]      è      P    P†CD‹]  (         p   P†CQŒ‹5      8      P    P†CD‹]      \      P    P†CD‹]  (   €      ğ    P†CNŒ‹³   (   ¬      à   P†COƒŒ‹¤    Ø      P    P†CD‹]      ü      P    P†CD‹]             P    P†CD‹]      D      @    P†CD‹Y      h      P    P†CD‹]      Œ      P    P†CD‹]      °      P    P†CD‹]      Ô      P    P†CD‹]     ø          E†CA             P    P†CD‹]  (   <      f    P†CG‹[[Q,   h      .   P†COƒŒ‹™U,   ˜      e   P†COƒŒ‹_$   È           P†CJ‹i   ,   ğ      r   P†COƒŒ