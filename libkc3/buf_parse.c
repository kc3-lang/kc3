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
  if ((r = buf_parse_sym_str(buf,ELF          >                    ��         @     @  ��L�    L3$UH��ASAWAVAUATH��xI��I��W�L�}�A)GA)G�A)G�A)G�A)G�I�G    H��`����    L��L���    I��H����   L�m�L���    ��tpL���    H�E�L���    I��H��x^H�u�L���    H��~HH�}� tH�u�L���    H��~0H�u�L���    H��~M�I�H�u��   L���H�M���%E1��I���M��H�}��    H��`���L���    H��`���L���    L��H��xA\A]A^A_A[]L3$L;    t
�������������L�    L3$UH��ASAWAVAUATSH��  I��I��W�)E�)�p���)�`���)�P���H�u��    H�5    L���    I��H����  L�}�L���    H���t  I��H�u�L���    H���\  H��L���    H���H  I��H�5    L���    H���-  M�L�L�H����   H�5    L���    H���  I��L������M����   �@   L��1�H��P����    L��H�u��    L��H�5    �    L��H�u��    H����   I��L��H�u��    L��H�5    �    H����   I�M�L��H��H�H��L��H�5    �    L��H�5    �    I��H���N����EL������L��1��    I�0�    H�E�H�5    L���    H��~I��I�H�E�H�M�H��H�u�L���    E1�H�u�L���    L��H��  [A\A]A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVH��(I��I��H�u��    L��L���    I��H��~H�u�L���    H�u�L���    L��H��(A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVH��(I��I��H�u��    L��L���    I��H��~H�u�L���    H�u�L���    L��H��(A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVAUATSH��pI��H�~ ��   I��H��h����   H���H�H�XH�<�    �    I������H���  I��H��x����   H��tH��H�
H��H��H��H��H��H��u�H��h���H�PH�H�U�Hk�p�    H�E�H����   H��H�M�H�H��h���L��L��E1��    H�E�H��~dH��h����   L���H��yH�5    L���    I��H��~nL���    I��H��xH�5    L���    H��~zM�I�M���?L���    �5H�=    �    H�}��    H�=    �    H���    L���    L�e�L��H��p[A\A]A^A_A[]L3$L;    t����������������I������L�    L3$UH��ASAWAVAUATSH��   M��I��I��H��L�E�H�� ����   �H�H��x���H���    H�5    H���    I��H���  L�}�H���    I��H���  M�M���T  K��    H�� ���H�@H��H�E�I�EH�E�L�m�H�]�L9m�u2H��L��L�u�I�6�    I��H����  L�e�M��I�pI��H�]��-H��H�� ���L��H�M�L�E��    I��H���\  L�e�M��K��H���    I��H��H��xwL��H�5    �    I��H��xqH�E�L�L�H��H�H��M��L���    I��M��tbM����   I�L�m�L���/���H�=    �    ��   H�=    �    �   H�=    �    I���H�=    �    M��L���   M��xJI�L�m�L��H�5    H���    I��H��~ H��0���L��H��H�K;�u%M��   H�=    �WH�=    �    L��M���HH�=    �    H�}��    H�=    �    I�������H�=    �    H�]��'H�=    �    H�=    �    H��x���H���    H��x���H���    L��H�İ   [A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    L���    I��H����   tL���    I��H����   M��E1�H�u�L���    H��~c�]�I�t$L��H�u��    I�Ǹ   ��{uGM��GueL��H�u��    I�Ǹ%   M��,uJL��H�u��    I�Ǹ   M��u/�   I���%M��~_A�$M�L���    1�H��HO�I��M��H�u�L���    H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t�������u�L��L���    H��tI��y��E1����L�    L3$UH��ASAWAVAUATSH��   I��I��H��P����    H�5    L���    I��H��~LL���    I��H��x*M�H�5    L���    I��H��xtfM�I�D$    �M��H��P���L���    M��H��P���L���    L��H�Ā   [A\A]A^A_A[]L3$L;    t�����������������I�$AD$H)E�AD$8)E�AD$()E�AD$)�p���H�5    L���    I��H���a���H�]�L�e�I�D$H�E�A�   �   M��t<L���    H��xzI��M�I�L��H�5    �    I��H��H��H��H��y�����H���    H��tFH�M�H�U�H�L�aH�A(�p���(M�(U�(]�H�E�X0P H �����I������I�������������L�    L3$UH��ASAWAVAUATH��hI��I��H��p����   �H�H�}�H�u��    I������H����   H�}� tbH��x���H���    H����   I��H�E�H��p���H�NH�VH��H�D�L��L��1��    H�E�H��x"H��p����   L���H��/H�=    �    �-H�=    �    H�}��    H�=    �    L���    L�m�L��H��hA\A]A^A_A[]L3$L;    t	������������L�    L3$UH��ASAWAVAUATSH��  I��H�U�H��H�M�H��0����   H�u��H�H�u�H���    H�5    H���    I��H����   H���    I��H����   M�H�E�J��    I�EH�E�L������H�]�H�E�H�@H��H��L9�uL���    I��H���n  L���    � H��0���H�U�H�M��    I��H���P  M��H�E�J��H���    I��H��H��xuL��H�5    �    I��H��xuM�L�L�M��t{H�}��    I��H����   I�M��H�]�L�������A���H�=    �    �  H�=    �    M����   H�=    �    I��L����   H�=    �    M���   H�5    H�}��    I��H����   I�H��@���L��H��H�4H�}�J��H����   H9�H�]�t}H�=    �    H�}��    H�=    �    I�������;H�=    �    M���&H�=    �%���H�=    �    �H�=    �    H�]�H�=    �    H�u�H���    H�u�H���    L��H��  [A\A]A^A_A[]L3$L;    t�������H�H�H�M�H��8���H��H��H��H9�H�]�s�H��H�TH�TH�T���L�    L3$UH��ASAWAVAUATSH��0H��I��H�u��    H�5    L���    I��H����   tE1��H�5    L���    I��A�H��~|L���    I��H��x9H�]�L���    H��H��~1A��L��H�U��    H��x(txM�I�I�M���3H�=    M���H�=    I���
H�=    I���    H�u�L���    H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t
�����������E1����L�    L3$UH��ASAWAVAUATSH��0I��H�u��    H�u�L���    H��~GI��1�L�m̋}��    ��t4�}�
t.L��L���    H����   L�L��L���    I��H����1�L���    I��H��~wI�L�m�M��L��H�u��    L��L���    H��M��H��~5�}��    ��t)�}�
t#L��H���    H��~I�L��L���    H����L���    N�$8H����I��L�e�L��L���    L��L���    L��H��0[A\A]A^A_A[]L3$L;    t���������L�    L3$UH��ASAWAVAUATSH���   H�U�A��I��H�E�H�E�H�     H��p����    1�L�� ���L�u�D�}�L��E��tH�5    �    I��H��y��  H�5    �    I��H����  M���w  L��L���    I��H���e  t01��    H�U�H�H���x  �   H��L���H�H�H��H�U�L���    I��H���+  L���    I��H���?  E��tH�}�H�5    �    I��H��y!�#  H�}�H�5    �    I��H���2  I�M�M�M���  L�u�L��H�5    �    I��H��x^L�� ���uL��H�5    �    I��H��~>L���    H��H���`  L���    I��H���]  M�L�L�L�u�D�}�����H�=    �    I�~�    H�=    �    L���    H�=    �    M���}  M���fH�=    �>  H�=    �    L���    H�=    �    M���D  H�=    �    L�}�L���    H�=    �    M��M���  L��L�e�L���    I��H�}�H���    H����   L�e�M��~5E1�L�m�H�}�L�   L���H�L���    L���    I��I��pI��u�L��E��E�E�H�M�H�E�H�AI��L�u�L�e��   H�=    �    I���sH�=    �    M���^I������L�u��hH�=    �H�=    �    �>H�=    �)H�=    �    L���    H�=    �    �H�=    �    M��L�u�H��p���L���    L�e�M��tL���    H��p���L���    L��H���   [A\A]A^A_A[]L3$L;    t���������������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    H�5    L���    I��H��xXA�t#H�u�L���    H��~�}��    ��t'E�,$�0H�5    L���    I��H��xE1�M��u��H�u�L���    E1�H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t��������L�    L3$UH��ASAWAVAUATSH��pH�u�I��H�E�    W�)E�)E�H��`����    1��    H���    I��H�E�H���    H�E�L�}�L��L���    H�E�H���0  H�5    H�}��    H���  H��L�e�1�H�E�H�E�H����   H���    H����   I��1��    I�$H�}�H���    H����   I��M�$$H�}��    H����   I��H�}�H�5    �    H����   I��H]�L�L�L�H��H�H��H�]�H�}�H�5    �    H��H�M�H���L����TH�}� tMH�    H�E�H�    H�E�H�M��   H�E�H�AH�E�H�M�(E�H�U�BH�H�JH�}�L�}��+H��`���L�u�L���    H�}��    H�}��    E1�L��H��`����    L��H��p[A\A]A^A_A[]L3$L;    t�����L�    L3$UH��ASAWAVAUATPI��I���    I��M�nL��L���    I��H��~1I�}  t	A�'   �!I��$�   I�v�    H��t�L��H���    L��H��A\A]A^A_A[]L3$L;    t���������������L�    L3$UH��ASAWAVAUATH��HI��I��W�L�e�A)D$A)$H�u��    L���    L��L���    I��H��~*H�u�L���    I��H��~!M�(E�(M�AOA�H�}��    �H�u�L���    M��H�u�L���    L��H��HA\A]A^A_A[]L3$L;    t������L�    L3$UH��ASAWAVAUATH��8I��I��W�)E�H�u��    H�5    L���    I��H����   tL���    I��H��x\M��E1�H�u�L���    I��H��xTtH�5    L���    H��~/M�I�M��H�u�L���    I��H��~M�(E�A�M���I��H�u�L���    H�u�L���    L��H��8A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVAUATSH��   I��I��H�u��    H�5    L���    I��H���!  I�D$    L���    I��H����  L���    H����  I��M�I�H�5    L���    I��H����  tM���  H�� ���L���    I��H����  H�� ���1��    I�$�   H��H���H�L���    H��H����  L���    H���U  M�I�I�L��H�5    �    H���4  uvL��H�5    �    H��H���f  taM�$$L���    I��H����   L���    H����   I��L�I�I�L��H�� ���H���    I��H���.����   I���   H�5    L���    H��H����   L���    I��H����   L���    H��xxH�E�I�4$H��pL���    I��H����   L���    H��xKH�E�L���    H��x:H�E�H�5    L���    H��~"L�Lm�I�Lm�Lm�M�I�M���M���I��H�u�L���    H�u�L���    L��H�İ   [A\A]A^A_A[]L3$L;    t�����������������I���M�����L�    L3$UH��ASAWAVAUATSH��`I��I��H�E�    W�L�u�A)FA)H��p����    L���    H����   H�    H�E�H�    H�E�H�u�L���    I��H����   L�e�H�]�L��H�5    �    I��H����   L���    I��H����   L�u�1��    I��H�H�pL���    L��I��H����   A�#   H�I��H�5    �    H��xnL}�M�M�H��H��M���m���H�}��    �    L�u�H�}��    L�p(E�(M�H�E� H�FI�������L1��M���M���M��I���I��M��H�}��    H�}��    H��p���L���    H��p���L���    L��H��`[A\A]A^A_A[]L3$L;    t������������������L�    L3$UH��ASAVI��H���    H��~A�#   A^A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATH���   I��I��W�)�p���)�`���)�P���)�@���)�0���)� ���)����H�u��    H�}��    H����   H�u�L���    I��H����   L���    H��x{I��H�u�L���    H��~g�    H�xpH�u�H������   �    H��tDH������    H�u�L��1��    H��~%M�I�E�M�AOAM���,I�������#H�}��    H�u�L���    E1��	H�}��    H�u�L���    L��H���   A\A]A^A_A[]L3$L;    t��������L�    L3$UH��ASAWAVAUATSH�� I��I��H�u��    L���    I��H����   tL���    I��H��~mM��E1�I�\$L��H���    I��H��~!A�$   M�L���    1�H��HO�I��;u-L��H���    I��H���uL��L���    I��H����M��H�u�L���    H�u�L���    L��H�� [A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVATH�� I��I��L�e�L���    L��L���    I��L��L���    L��L���    L��H�� A\A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVAUATSH���  ��I��I��W�)E�)�p���)�`���)�P���)�@���)�0���)� ���)�p���)�`���)�P���)�@���)�0���)� ���)����)�����)�����)����)� ���H�������    L������L���    M�L���    H�E�I�t$L���    H�u�L���    H����  �    H�xpH�u�H�� ���H�}��   �    H����  L������H��0���H� 8X
sUE1�H������L���    H�� ����    H������    L������L���    ����������AOA�w  �]�L������I�GH������H�E�H��H�������L�� ���L�e�E1�L���    W�)E�)�p���)�`���)�P���)�@���)�0���)� ���L��L���    L��I��H���N  H�u�H�}��   �    H����  H�بH��u|H�� ����    Hǅ ���    H�E�H� H�����H������   H��L������L���H�H���    �   H��H�u��H�L���    (� ���(����H������ HL�����L���    W�)�p���)�`���)�P���)�@���)�0���)� ���)����L��H�� ����    H���Z  H�� ���H� Hǅ����    H�E�H� H������L���    I��H���/  L��H�u��    H���m  M�M�I�L��H�������    L���    H����  I��M�L��H�u��    H��������}�
�����L�� ���L���    W�)E�)�p���)�`���)�P���)�@���)�0���)� ���L��H�u��    H���<���L��L�}�H�}�L���   �    H��u!H�}�L���   H�� ����    H�������H��0���L�e�L�8A�uA�G
H�M�:A
wA�G
A��@  H�M�:A
�3  H������H���    H�������   L���H�H�E��@
A:G
 ��L��H���    H����  I��M�L���    (�����(�����H������ HL��H�������    L���    H���5  I��M�L��H�u��    H�������}�
����L�� ���L���    W�)E�)�p���)�`���)�P���)�@���)�0���)� ���L��H�u��    H�������H�u�H�}��   L���    H�������H��0�������1�:E�L�� ���L�e�������r���I���E1�H������L���    H�� ����    H������    H�������    H������L���    L��H���  [A\A]A^A_A[]L3$L;    t���������M�������H������H�G�     �    �����M���g���I�������[���M���S�����L�    L3$UH��ASAWAVAUATH��   I��I��W�)E�)�p���)�`���)�P���)�@���)�0���)� ���L�e�L���    H�u�L���    L��L���    H��~xI���    H�xpH�u�H�� ����   �    H��tRH�� ����    H�u�L���    L���    I��H��xGH�u�L���    H��~nM�I�E�M�ANA�&L�u�L��L���    L��L���    E1�H�}��    L��H�ĸ   A\A]A^A_A[]L3$L;    t�������������I������L�    L3$UH��ASAWAVAUATSH��PH��I��W�)E�)E�H�u��    H�5    L���    I��H����   L�e�L���    H�    I�D$L���    I��H��x`H�]�H�u�L���    H��H��~ML���    I��H��xBH�5    L���    H��~3M�M�I�I�(E�(M�H�E�H M���'M���I���M���I��H�}��    H�u�L���    H�u�L���    L��H��P[A\A]A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVATH��   I��I��W�H��@���)FP)F@)F0)F )F)H�F`    )E�)E�)E��    H��~+I���    H��t_�    H��H��H��@����   �H��<H�u�L���    I��H��~K�    H��t1�    (E�(M�(U�@HP(I��H��@����    �	H�}��    I������L��H�Ġ   A\A^A_A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATSH���   H��I��1�H�E�H��P���W�)�@���)�0���)� ���)����)� ���)�����H��X����    H�5    L���    I��H���  H�5    L���    I��H���  �E�L���    I��H����   H�u�L���    H����   H�E�L���    H����   H�E�H�]�H��x���L���    H��H����   H��x����    H����   H�E�H��x����    L���    H��~oH�E�H�u�L���    H��~ZM�M�L�e�Le�I�M�Le�I�H�U�H�M�L������L��H�u��    �E�A��   H�}�L���H�L���!L���L���H���L��H��X���L���    H��X���L���    H��H���   [A\A]A^A_A[]L3$L;    t	������������L�    L3$UH��ASAWAVAUATSH��   I��I��H�E�    W�)E�)E�)E�H��P����    H��p���L���    I��H���(  H��x���H;    tH;    tE1���  H�E�H��p���H�}��    L���    I��H����   M�H�5    L���    I��H����  ��   L���    H���N  L�}�L�}�M�I�1��    I�L��H���    I��H��~MM�?L���    H��H���H  L��H�5    �    H��H���2  I��(M�I�H��t�L�L�}���   M���  1��    H�E�L��H���    H����   L�H��L��P���L��L���    L��L���    L���    H����   I��H�u�L���    H��~lH��L�H�L�e�M��tcH�M�1��    H�E�L��H�}�I�T$H�rH��H��pH���    H��tfL���    I��H��u�H���    H�E�H�M��I���?H�E�H���H�E�H;    �E�(E�(M�AO AG(E�AI��� I���I��H�}��    H��P���L���    H��P���L���    L��H�Ā   [A\A]A^A_A[]L3$L;    t���������������L�    L3$UH��ASAWAVAUATSH��PI��I��L�e�I�$    H�u��    L��L���    I��H����   L���    I��H��xfL�}�H�]�H���    L�}�H�    H��L���    H�{L���    H�{�    L��H���    H��~5M�I�E�M�H�E�H �H�}��    H�u�L���    M���I��H�u�L���    L��H��P[A\A]A^A_A[]L3$L;    t���������������L�    L3$UH��ASAWAVAUATSH���  I��I��H�u��    H�5    L���    H����  tSH�5    L���    I��H����  H�u�L���    I��H���}  H�]�H���    I�$H���    M��l  H�u�L���    I��H���T  �}̃�:t�    ���(  H�� ���H�� ����   1��    �}�:u>L��L���    H����   H�u�L���    I��H����   �}��    ����   �H�� ���L��L���    H����   H�u�L���    H��~pH�؋}̃�.t��[��   �    ��t%�QL��H�u��    H��~@I�ǋ}��    ��t1L�H�� ���L��H���    H��xJI�L��H�u��    H��H���H�� ���L�}�L���    L���    I�$L���    �I���E1��I��H�u�L���    M��H�u�L���    L��H���  [A\A]A^A_A[]L3$L;    t����H�u�L���    H���s����}�]�i���H�� ���L��H���    I��H�������M��C�����L�    L3$UH��ASAWAVAUATSH���   I��I��H�u��    L�-    L��L���    I��H���l  H������   �    H�5    L���    H��H���%  ��   L�}�I�1�H��A��H�    HD�H�M�L�-    L��L���    H��H����   A��H����D�uL��H�u��    H��H����   H����   L��H�u��    H��~H�؋u�H������    �$L��H�u��    H��H��~x�u�H������    H��x`I�L��L���    H��H���l����GH�5    L���    H��H��x0����L�}�E1�1�H�E�� ���L�H�����H�u��    �H��H�u�L���    H������    I��H�u�L���    L��H���   [A\A]A^A_A[]L3$L;    t�����L�    L3$UH��ASAWAVAUATSH��@H��I��H�u��    H�5    L���    I��H��~dL���    I��H��x8L���    H��x0M�I�H�5    L���    I��H��xt\M�H�    �M���I��H�;�    H�u�L���    H�u�L���    L��H��@[A\A]A^A_A[]L3$L;    t��������H�    1��    H�L��H���    I��H��~�I��H�]�L���    H��H���j  L���    H���T  M�I�I�L��H�5    �    H���3  u|L��H�5    �    H��H���  tgM�?L���    I��H���  L���    H����   I��L�I�I�1��    I�L��H���    I��H��H�]��A��������I������H�5    L���    H��H����   L���    I��H����   L���    H��xqH�E�I�7H��pL���    I��H��~lL���    H��xIH�E�L���    H��x8H�E�H�5    L���    H��~ L�Lm�I�Lm�Lm�M�I�M���(���I���I���M��H�]������M������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    H�5    L���    I��H��~NH�u�L���    I��H��~&H�5    L���    H��~�M�A�$M�I�M���M���I��H�u�L���    H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t������������������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    H�5    L���    H��xt:E1��I��H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t�������H�5    L���    I��H��x@H�u�L��t?�    I��H���  �M�1���rb��mz��0��   ��U��   ��   M���o����    I��H���^����}�"�L���L��L���    I��H���=����E�A�$�1�����t0��std��tu{�E�	   �	   �k��ntZ��ruc�E�   �   �S��ut��vuK�E�   �   �;H�u�L���    H��x;�M��'�E�    1���E�    �    ��E�
   �
   1�A�$M�I�����I��H�u�L���    ������L�    L3$UH��ASAWAVAUATSH��0I��H�u��    H�5    L���    H����   I��L�e�L��L���    H����   I��L��H���    H��~M�}�
u�L�e�H�]�L��L���    L���    H��~gI��L��H�5    �    H��~?M�I�M��L��H���    I��H��~L��L���    H��x$M�}�
u�I��M��y��H�u�L���    �E1�H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t������������������L�    L3$UH��ASAWAVAUATSH��  I��I��H�u��    H������L���    I��H����   L���    H��H��~eH�5    L���    I��H��~SL�e�L���    I��H��~DH��0���L���    H��~rL�M�I�I�H��������   H�}��    M���I���M���M��H�u�L���    H�u�L���    L��H��  [A\A]A^A_A[]L3$L;    t����I������L�    L3$UH��ASAWAVAUATSH��`I��I��W�)E�)E�H��x����    H�    H�u�H�L���    I��H���  tL���    I��H����   M��E1�H�5    L���    H��H����   L���    I��H����   H�u�H�}��    H����   H�}��    L��H���    H��~~H�M�H;    ��   H�E�H�}��    H�u�H���    H��t<H�E�H;E�H�E���   H�=    �    H�}��    H�=    �    H�}��    I�������I���M���I��H�}��    H��x���L���    H��x���L���    L��H��`[A\A]A^A_A[]L3$L;    t
�����������L�L�H��H�H��L�e�L���    A($A(L$AAOI�����L�    L3$UH��ASAWAVATH��I��I��I��H�u��    H��~a�u�L���    H�E�H��x5I��H=   r.H�=    �    H�}��    H�=    �    H�������1��H�u�L���    H��~E�&H��A\A^A_A[]L3$L;    t�����������������L�    L3$UH��ASAWAVPI��I��H�u��    H��~1�M�qвЃ�
r�q�����r
�q��Ƀ�w1 �A�L��H���    H��A^A_A[]L3$L;    t���������1�����L�    L3$UH��ASAWAVPI��I��H�u��    H��~"�M�у����0u���A�L��H���    �1�H��A^A_A[]L3$L;    t����������L�    L3$UH��ASAWAVPI��I��H�u��    H��~"�M�у����0u���A�L��H���    �1�H��A^A_A[]L3$L;    t����������L�    L3$UH��ASAWAVPI��I��H�u��    H��~ �M�Qƃ��s1�����A�L��H���    H��A^A_A[]L3$L;    t	������������L�    L3$UH��ASAWAVAUATSH��PI��I��H�u��    H�5    L���    I��H���b  tL���    I��H���I  M��E1�H�u�L���    H��H��~eL�H�u���*��E�L���    I��H��~JL�e��M�L��E�W��*��Y    �X��M�L��L���    �M�I��H����I����   �M��M�M����   H�5    L���    I��H����   I�H�u�L���    H����   �
   L�m��]�I��E�W��*��	���������fHn�fb    f\    f(�f��X��������^��X��]�H�H��L��L���    �]�H����JM��H�u�L���    H�u�L���    L��H��P[A\A]A^A_A[]L3$L;    t	�����������]��]�H�5    L���    H����   I��M�H�5    L���    H��~
�    �#H�5    L���    H��1�H��HO��    �E�I�H�u�L���    fW�H��~8L�e�I��E�W��*��Y    �X��M�L��L���    �M�H����E��Y�W��Z��    �    �Z��M��Y��M�M��H�5    L���    I��H�������H�u�L���    H��~�}��    ��tM�W��ZE��A����E1��y�����L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    H�5    L���    I��H���K  tL���    H��H����   L��1�H�u�L���    I��H��~eH�u���*��E�I�L���    I��H��~RH�]��M�M��E�W��*��Y    �X��M�L��H���    �M�I��H����M���  I���  �M��M�M����  H�5    L���    I��H���w  M�H�u�L���    H��~z�
   L�m��]�I��E�W��*��	���������fHn�fb    f\    f(�f��X��������^��X��]�H�H��L��L���    �]�H�����]��]�H�5    L���    H��~`I��M�H�5    L���    I��H����   tIM�H������1�H��W����������H*��    �    �M��Y�f(���E��A�kH�5    L���    I��H��xHM�H�u�L���    �   H��~�1�L�m�I�H���M�H�AL��L���    H���   �d���H�u�L���    H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVAUATSH��pI��I��H�u��    H�5    L���    I��H���  tL���    H��H��x}L��1�H�u�L���    I��H��~ZH�u��f�E��E��}�I�L���    I��H��~FH�]��m�M��E�f�E��    �E��}�L��H���    �m�I��H����M���  I���  �m��}�M���u  H�5    L���    I��H���Z  M�H�u�L���    H��L�}�~b�
   L�=    L�m��m�I��E����f�E�H�]�1�H�����m�A�����������}����}�H�H��L��L���    �m�H�����m��}�H�5    L���    H��~WI��M�H�5    L���    I��H����   t@M�H������1�L�}�H��H�]��m��    �<$�|$�    �m����L�}��m�A�?�oH�5    L���    I��H��xLM�H�u�L���    �   H��~�1�L�m�L�}�I�H���M�H�AL��L���    H���   �m���H�u�L���    H�u�L���    L��H��p[A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVAUATSH��   I��I��H�������`  1��    H��8���L���    H�5    L���    I��H���M  L���    I��H���  L���    H��H���  H������L���    H����  H��X���L���    H����  H��`���L���    H����  H��h���H�5    L���    H����  H��p���L���    H���~  H��x���L���    H���f  H�E�H��@���L���    H���J  H�E�L���    H���5  H�E�L���    H���   H�E�H�5    L���    H���  H�E�L���    H����   H�E�L���    H����   H�E�H������L���    H����   H�E�L���    H����   H�E�L���    H����   H�E�H�5    L���    H��~|M�H�X���L�H��`���H�h���H�p���H�x���H�H�U�HU�HU�HU�H��H�H��H�M�HM�HM�HM�HM�H�L�e�I�I�H�������`  L���    �(I���M���I��H�������    H��8���L���    H��8���L���    L��H��   [A\A]A^A_A[]L3$L;    t�����������L�    L3$UH��ASAWAVATH��   I��I��H�u��    H��0���L���    I��H����   ��0�����   H��@���H����H;    ����ujH��H���H;    uZH��P���H��tN�8uIH��0����    H�u�L���    H�u�L���    I��H��~/H��@����@�"   H�M�H�HE� H��0����   L���H�H�u�L���    L��H�İ   A\A^A_A[]L3$L;    t������L�    L3$UH��ASAWAVAUATH��8I��I��H�}�1��    H�u�L���    I��H��~VH�}��    H�E�L���    I��H��xBH�u�L���    H��~6M�I�E�M�U�AT$ AL$A$M���:H�=    �M���#H�=    I���    L���    H�=    �    H�}��    L��H��8A\A]A^A_A[]L3$L;    t
�������������L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    H�5    L���    I��H���  L���    I��H��x<L���    H��x>M�I�H�5    L���    I��H��x0t:M�I�E     ��  H�=    M���  H�=    I���  H�=    �  I�E     1��    I�E L��H���    H��H�=    H���  L�m�L�m�I������H����   L���    I��H����   L���    H����   L�L�H��H�H��L��H�5    �    H����   ��   L��H�5    �    I��H����   ��   H�E�H� H�E�L���    I��H����  L���    I��H���  L��L�m�I��H�M�I�1��    L�m�I�E L��H���    H��H������H�=    I���I  I���E  H�=    �5  M��L�m�H�=    �&  H�=    �H�=    I���
  H�I���  H�=    ��   H�5    L���    I��H����   �  L���    I��H���  L���    H���  I�I�M�H�E�H�0H��pL���    I��H����   ��   L���    H��H����   L���    H����   M�H��H�H��L�H�5    L���    H����   ��   H��H�H��I���=H�=    M���H�=    �
H�=    M��L�m��    I�} �    H�u�L���    H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t���H�=    �H�=    �H�=    ����H�=    �H�=    ����H�=    �#���H�=    �V���H�=    �J���H�=    �P�����L�    L3$UH��ASAWAVAUATSH��   I��I��H�E�    H�5    �    H��H���!  L���    I��H��x-I�H�5    L���    H��H����   tL�1�I���   1�L����   H��@���L���    H��H����   L�u�L�}�L��@���L�e�1��    I��   H��L���H�M�?L���    I��H����   L��H�5    �    H����   L�L�H��H�H��H��L��@���L�e�uL��H�5    �    H��xfuwL���    I��H��x^I��I�L��L���    H��H���V����A1��    H��H�Đ   [A\A]A^A_A[]L3$L;    t	����������H���L���L��H�}��H��H�H��H�E�L�u�������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    L���    I��H��xvtL���    I��H��~UM��E1�I�t$L���    I��H��~!A�$   M�L���    1�H��HO�I��&uL��L���    I��H����M��H�u�L���    H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATSH���   I��I��H��������   1��    H�u�L���    H�u�L���    I��H���  H�}��    ����   L�������   L���    H�u�L���    H��xzL�m�I�FHH�E�L�-    H������L��L���    H��~fL��H�u��    H��~UI��H�}��    ��tEI�FHH�E�H��L���    H��xM�I��H��H�u��    H��y�I���OE1�H�u�L���    �>H�u�L���    H������L�m�L���    L���    I��L���    M��tQH�E�L� H�u�L���    H�������    L��H���   [A\A]A^A_A[]L3$L;    t���������������I�������^�����L�    L3$UH��ASAWAVAUATSH���  H��I��H�u��    H�5    L���    H���f  t^H�5    L���    I��H���L  H�u�L���    I��H��x]H�]�L�e�L���    H��L���    H��tEM�H�E�H��  H�u�L���    I��H����   t�}��    ��tE1���   M����   I��������   L������H�������   L��1��    L��L��L���    H����   H�]�H�u�L���    H��~CH��L������L�e̋}��    ��u)L��L��H���    H9�uNI�L��L���    H��H���H������L�e�L���    L���    I��L���    M���)���H�E�L�(�I��H�u�L���    H�u�L���    L��H���  [A\A]A^A_A[]L3$L;    t������������������L�    L3$UH��ASAWAVAUATSH��   I��I��W�)E�)E�H��X����    H�5    L���    H��H����  L��x���L���    H���Z  I��L���    H���F  I��L���    H�E�H���.  H�    H�E�H�    H�E�1��    I��H�E�L��H���    H�E�H����   L���    H�E�H����   L���    H����   1�H�E��    I���   H�U�L��H���    H�M�H����   I�Le�Le�Le�I�I�M�M���   1��    I���   �}� tSI��L���    H��x[I��L���    H��xKH��A�E    I��L��1�L���    H��~*M�H��H�H��L�I��(E�(M�H��x���H �$H��X���L���    H�}��    I�������I��H��X���L���    L��H�Ā   [A\A]A^A_A[]L3$L;    t
�������������L�    L3$UH��ASAWAVAUATSH��pI��I��I��H��`����    H�5    L���    I��H���,  uH�5    L���    I��H���  L�m�H�E�    L���    I��H����  L�}�L�u�H�E�H�E�L�e�L���    H��H����  1��    H�M�H�L��H���    I��H����  L���    I��H����  L�u�L���    I��H����  L��H�5    �    I��H���`  Lm�I�M�Lm�M�M���T  H�}�H�5    �    I��H���*  �2  L�e�L��H�5    �    H��H���  ��q  L��H�5    �    H��H����   uL��H�5    �    H��H����   L���    I��H����   L���    I��H����   L��H�5    �    I��H����   L�M�I�M����   H�}�H�5    �    I��H��xX��   L�e�L��H�5    �    H��H��xL��   L�}�H�E�H� H��H�E�L���    I��H��H���'����E1��L��L�e��L���1��oL��L�u�I��H��`���L���    H��`���L���    M��tL���    L��H��p[A\A]A^A_A[]L3$L;    t������1�M���I���I��M���H�E��L�u�L���    I��H�}�H���    H��tmL�u�M��~:1�L�u�H�}�H߹   L���H�L���    L���    I��H��H��pH��I��u�M�H�M��   E�AH�E�H�A L�e�L�u�����I������L�e��������L�    L3$UH��ASAWAVAUATSH��`I��I��H��x����    H�5    L���    I��H����   H�5    L���    I��H���Z  ttH�u�L���    H��H���=  M�M��t4L�m�L�}�L�}�L��L���    L���    I�GI�EAL�}�A)E I�H�E�I�G(E�A��   M����   H�5    L���    I��H����   �k���H�5    L���    I��H����   tYH�u�L���    H��H����   M�M��t�L�}�L�}�L�m�L��L���    L���    I�EI�GAE A)L�}��I���H�5    L���    I��H��xEu�H�5    L���    I��H��x,tgH�u�L���    H��H���u���I��H��x���L���    H��x���L���    L��H��`[A\A]A^A_A[]L3$L;    t��������������H�5    L���    I��H��x��|���H�u�L���    H��~:I��M��t*H�]�H�u�H���    H���    H�M�H�AH�C)M��K���E1��D�����L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    H�u�L���    H��H��~<L���    �u�L��L���    ��tPL���    H�u�L���    H��H������H��H�u�L���    H��H��0[A\A]A^A_A[]L3$L;    t���L�-    L��L���    H��xSL��H�u��    I��H��~B�   L��L���    ���m���H��Hc�H��I��u�L��L���    L����t��F���I�ǅۉ�IN��W�����L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    H�u�L���    H��H��~<L���    �u�L��L���    ��tPL���    H�u�L���    H��H������H��H�u�L���    H��H��0[A\A]A^A_A[]L3$L;    t���L�-    L��L���    H��xSL��H�u��    I��H��~B�   L��L���    ���m���H��Hc�H��I��u�L��L���    L����t��F���I�ǅۉ�IN��W�����L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    H�u�L���    H��H��~<L���    �u�L��L���    ��tPL���    H�u�L���    H��H������H��H�u�L���    H��H��0[A\A]A^A_A[]L3$L;    t���L�-    L��L���    H��xSL��H�u��    I��H��~B�   L��L���    ���m���H��Hc�H��I��u�L��L���    L����t��F���I�ǅۉ�IN��W�����L�    L3$UH��ASAWAVAUATSH��`I��I��1�H�E�W�)E�H�E�)E�H��p����    H�u�L���    I��H����   L�}��u�H�}��    H����   H�5    L���    H����   I��H�]�L�}�M�L��H�u��    H��~oI�ž
   H��L���    ��u7�u�L��H���    ��u$M�L���    L��H�5    �    I��H��y��$H�}��    H��p���L���    I�������M��H�E�H�M�H�A(E�H��p���L���    L��H��`[A\A]A^A_A[]L3$L;    t	������������L�    L3$UH��ASAWAVAUATSH��   I��I��H�u��    H�u�L���    I��H����   ��   H�5    L���    H����   L���    H����   H��L���    H����   I��H������L���    H��~}H�E�H�� ����   �    H���  L�Lm�I�L�e�L���    H��L���    H��8���� #   H�XH��8���H��pH�������   �H�M���:H�=    �    �lH�}��    H�u�L���    H�� ���L���    I��H��~H�� ����   L���H��,H�=    �    L���    H�=    �    1�I��H�I	�H�u�L���    L��H��   [A\A]A^A_A[]L3$L;    t������������������H�}��    H�������    I���������L�    L3$UH��ASAWAVAUATSH���  H�u�I��L�e�A�$    H�u��    H�����E1�H������   1��    L��L���    H��~CI��E1�H������}��    ��u*H��L��L���    H��x)M�L��H�u��    I��H���H�����H�u��    �I��H�u�L���    M��H�u�L���    L��H���  [A\A]A^A_A[]L3$L;    t��������������L�    L3$UH��ASAWAVAUATSH��pI��I��H��h����    H�5    L���    I��H����   1�H�]�H�]�L���    I��H��xpL�m�L���    I��H��xdH�5    L���    H��x]M�I�M�H��t\1�E1�H�}�L��H���    H����  L���    H���    �   M����  E1���  1�E1�M����  1�E1�I���  1��    H�E�L��H���    I��H����  H�]�L�u�L�e�M�6L���    H���3  H�E�L���    H���  H�E�1��    H�L��H���    H����   H�E�L�u�H�L���    M��I��H����   H�]�L���    H��H����   L��H�5    �    H����   M�L}�L}�L}�I�I�M�H����   L�u�L��H�5    �    I��H����   L���    I��H����   L���    H��xzH�m��H�]�H��M�I�M�1��    H�M�H�L��H���    I��H��M��L�u�������<E1��?I���2M���*I���%I��L�e��L�u�H�]�M��L�e�����M���I��M��L�u�H�]�L���    H���    H��h���L���    M��H��h���L���    L��H��p[A\A]A^A_A[]L3$L;    t������������������L�    L3$UH��ASAWAVPI��I���    H��uL��L���    H��t&H��A^A_A[]L3$L;    t��������������L��L��H��A^A_A[]�    ��L�    L3$UH��ASAWAVAUATSH��@I��I��H�u��    H�u�L���    I��H����   t@H�5    L���    H����   t+I�A�$#   L�m�L���    I�D$L���    �E1��zL���    I��H����   L���    H��H����   H�5    L���    H��~/M�H��H�H��L�A�$"   E�AD$H�E�I�D$ I���I��H�u�L���    H�u�L���    L��H��@[A\A]A^A_A[]L3$L;    t	����������M���I�����L�    L3$UH��ASAWAVAUATH��8I��I��H�u��    H�u�L���    I��H��x\t=H�5    L���    I��H��~+M�A�$#   L�m�L���    I�D$L���    �E1��H�}��    H�u�L���    M��H�u�L���    L��H��8A\A]A^A_A[]L3$L;    t�������L�    L3$UH��ASAWAVAUATSH��   I��I��H�u��    H��@���L���    I��H��~gL���    I��H��xJL���    H��H��x?H�5    L���    H��~0M�H��H�H��L�H��@����   L���H�I���M���I���I��H�u�L���    H�u�L���    L��H�Đ   [A\A]A^A_A[]L3$L;    t�����L�    L3$UH��ASAWAVATI��I���    I��L��H���    I��H��yL���    E1�M�>L��A\A^A_A[]L3$L;    t���������������L�    L3$UH��ASAWAVATI��I���    I��L��H���    I��H��~M�>�L���    L��A\A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVATI��I���    �    I��L��H���    I��H��~M�>�L���    L��A\A^A_A[]L3$L;    t�����������L�    L3$UH��ASAWAVATI��I��1��    H��tI��L��H���    I��H��~M�&�I�������L���    L��A\A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVAUATSH��   I��I��W�)�`���)�P���)�@���Hǅp���    H�� ����    H�5    L���    H��H����  H�u�L���    I��H����   H�5    L���    H����   H�E�H�E�    L���    I��H����   L���    H����   I�H�M�L�L�H�H�M�H�5    L���    H��H��x��   H�u�H��@����    H����  L�m�H]�E1�E1�H��p���H�M�H�A0(�@���(�P���(�`���Q I��  E1�E1�L���E1�E1�H���E1�L���E1�E1�H�� ���L���    �  L�m�E1�1��    I��H�E�L��H���    H��H��~�L�}�L�}�H�E�H�E�M�?L���    I��H����  L���    H����  H�E�1��    L�m�I�E L��H���    H���o  H�E�L�}�M��M�m L���    I��H���Q  L�m�L���    I��H���>  L��H�5    �    H���  H]�L�H]�H]�M�I�I�H���  L��H�5    �    H��H����   L���    H����   I��L���    H����   L�}�I��H�m��L�I�I�L�m�1��    I�L��H���    H��H��������   H�=    �    H�}��    H�=    �    H��H������H��E1�E1�L���    L���    H�� ���L���    H��H�İ   [A\A]A^A_A[]L3$L;    t�����H���L���L��L�m�L�}������H�u�L�m�H��@���L�}�L��L���    H��tL���Y���H�=    �    H�}��    H�=    �    H��H������H���A�����L�    L3$UH��ASPH�=    �    H������H��A[]L3$L;    t�����L�    L3$UH��ASAWAVAUATSH��@I��I��W�)E�H�E�    H�u��    H�5    L���    H��H��~kL���    I��H��xcH�u�L���    I��H��~LI�H�}��    ��u!H�=    �    H�}��    H�=    �    M�H�}��    I�$    �I���M��H�u�L���    H�u�L���    L��H��@[A\A]A^A_A[]L3$L;    t����������������L�    L3$UH��ASAWAVAUATSH��@I��I��W�)E�H�E�    H�u��    H�5    L���    I��H��~@L���    H��H��x5H�u�L���    I��H��~$H�}��    ��tL�I�I�    �M���I��H�u�L���    H�u�L���    L��H��@[A\A]A^A_A[]L3$L;    t�����������L�    L3$UH��ASAWAVAUATH��HI��I��H�E�    H�u��    H�    H�u�H�L���    I��H����   tL���    I��H����   M��E1�H�5    L���    I��H����   M�H�5    L���    H��xKtNI��H�u�L���    H��~VM�I�L�m�L�m�H�u�L���    H��~1�}��    ��u%E1��2M���-E1��H�u�H�}��    H��tL�m�M�,$�I���I������H�u�L���    H�u�L���    L��H��HA\A]A^A_A[]L3$L;    t���������L�    L3$UH��ASAWAVAUATSH�� I��I��H�u��    H�5    L���    I��H����   L���    H��H��~2�    H��t-I��L��H���    H��~#L�H��H�H��M�,$I���EI���4I�������+H�=    I���    L���    H�=    �    L���    H�u�L���    H�u�L���    L��H�� [A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVAUATSH��PI��I��W�L�}�A)G A)GA)H�u��    L��L���    I��H����   H�5    L���    I��H��~IH�u�L���    H��H��~:H�}��    ��t2M�I�(E�(M�(U�AT$ AL$A$M���&M���I���I������H�}��    H�u�L���    H�u�L���    L��H��P[A\A]A^A_A[]L3$L;    t�������������������L�    L3$UH��ASAWAVAUATSH��`I��I��W�L�}�A)GA)H��p����    L��L���    I��H����  H�}�H�u��    H����   �}� �i  H�}��    ����   L�e�t|H�]���H�E�E1�L���    I��H����   M�L���    I��H����   ��   M�1��    H�L��H���    I��H����   �e  M�H�H��I��L9e�u�(E�(M�H�E�H ��   H�=    �    H�}��    H�=    �H�=    �    H�}��    H�=    �    I�������   H�=    �    H�}��    H�=    �vH�=    �    H�}��    H�=    �XM��t2H�=    �    H�}��    H�=    �    �    H�=    �    E1��$H�=    �    H�}��    H�=    �    M��H�}��    H��p���L���    H��p���L���    L��H��`[A\A]A^A_A[]L3$L;    t������������H�=    �    H�}��    H�=    �������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    H�5    L���    I��H���  ��   H�5    L���    I��H����   tRM�H�u�L���    I��H����   ��   M�H�u�L���    I��H��x~��   M�E���E�A��   H�u�L���    I��H��xMt\M�H�u�L���    I��H��x7tCM�H�u�L���    I��H��xt*M�EՊM����� �M�A��+M��H�u�L���    �H�u�L���    L��L���    I��H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t�����������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    H�5    L���    I��H��~/H�u�L���    I��H��~�E�A�$M��H�u�L���    M��H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    H�u�L���    I��H��~FL�e�1�L�m�E1�M�H��H���]�H��H�H��L��L���    I��H���M��~
H�E�H�M��H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t�����������L�    L3$UH��ASAWAVATH��@I��I��H�u��    H�u�L���    I��H��~,H�}�L���    ��uH�u�L���    I������H�}��    H�u�L���    L��H��@A\A^A_A[]L3$L;    t�����������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�%   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�	   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�
   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�#   A^A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATH��  I��I��1�H�E�W�)E�)E�)�p���)�`���)�P���)�@���)�0���)E�)E�)� ���)����)� ���)�����)�����)�����)�����H�E�L�nL���    H��~I��A�
   �  L��L���    H��~I��A�	   �j  L��L���    H��~I��A�   �K  L��L���    H��~I��A�   �,  H�u�L���    H�u�L���    H��~-I��H�}��    ����   L���    I��H����   M��E1�H��@���L���    I��H��~ǅ0���   M�H�}�H����   L������L���    H�u�H�U�L���    I��������t>H�}�H�u�H��0����    H��t%H��0����    H�������   L���?E1��M��H��0����    H�u�L���    M���L��0���L���    �   L��L���H�H�u�L���    L��H��  A\A]A^A_A[]L3$L;    t	������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATH��(I��I��H�u��    L���    I��H��xvtL���    I��H��~UM��E1�I�t$L���    I��H��~!A�$   M�L���    1�H��HO�I��&uL��L���    I��H����M��H�u�L���    H�u�L���    L��H��(A\A]A^A_A[]L3$L;    t�����������������L�    L3$UH��ASAWAVAUATSH��0I��I��H�u��    L���    I��H���b  tL���    I��H��~cM��E1�H�u�L���    H��~B�Ẽ�9J��'��   �HЃ�
��  L��L���    I��H����  M���  ��  I����  M����  ��h~l��s��   ��u��  ��t�i  ��u��  I�\$L��H���    I�Ÿ   M����  ��  L��H���    I��H����  �&   �  ��b��   ��:�G  ��?�U  ��[�>  I�t$L���    I�Ÿ   �G  ��"�c  ��%�j  ��'�	  I�t$L���    I�Ÿ   �  ��i��  ��mth��q��   I�\$L��H���    I�Ÿ   M����  ��  L��H���    I��H����  �!   �  ��c��  ��d�  ��fuyI�\$L��H���    I�Ÿ   M����  �X  L��H���    I��H��}�  ��v�  ��{u,I�t$L���    I�Ÿ%   �5  ��(�y  ��-�����I�\$L��H���    I�Ÿ   M���  ��  L��H���    I��H����  �   ��   L��L���    ����I�\$L��H���    I�Ÿ   M����   ��  L��H���    I�Ÿ$   M����   �e  L��H���    I�Ÿ   �mI�\$L��H���    I�Ÿ   M��Y�-  L��H���    I��H���Y  �   �5I�t$L���    I�Ÿ#   �I�t$L���    I�Ÿ    M����  A�$M�L���    1�H��HO�I���  I�\$L��H���    I�Ÿ   M�����  L��H���    I��H����  �   �I�\$L��H���    I�Ÿ   M����V  L��H���    I��H����   �   �[���I�\$L��H���    I�Ÿ   M���:����  L���    I��H��~A1�����I�\$L��H���    I�Ÿ   M���������  L��H���    I��H����  L��H���    I��H���  �   ����L��L���    I��H�������I�\$L��H���    I�Ÿ    M��������X  L��H���    I�Ÿ   M���b����6  L��L���    I��H���(���L��H���    I��H����   �   �$�����   L��H���    I��H���A�����   L��L���    I��H�������L��H���    I��H����   �#   �������   L��H���    I��H������������uL��L���    �l���umL��H���    I��H���N���uTL��H���    I��H��~
�   �]���u5L��H���    I��H���~���uL��H���    I��H���e���uE1�H�u�L���    H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t���������������L�    L3$UH��ASAVI��H���    H��~A�    A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI���    H��~A�    A^A[]L3$L;    t�����L�    L3$UH��ASAVI��H���    H��~A�&   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�!   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�$   A^A[]L3$L;    t�����������������L�    L3$UH��ASAVI��H���    H��~A�   A^A[]L3$L;    t�����������������UH��]�    ��L�    L3$UH��ASAVI��H���    H��~A�#   A^A[]L3$L;    t�����������������L�    L3$UH��ASAWAVPI��I���    H��~&H��A^A_A[]L3$L;    t��������������L��L��H��A^A_A[]�    ��L�    L3$UH��ASAWAVAUATSH��PI��I��H�u��    H�5    L���    I��H����   H�E�    L���    I��H����   L�}�L�}�L���    H����   H�E�L�e�1��    I�L��H���    H��H����   L���    M��I��H����   L���    H����   Lm�Lm�M�I�I�L��H�5    �    H��M��xKu`L��H�5    �    I��H��~cM�M�?I��L���    I��H���?����FH�u�L���    �g1��8I���/I���*M���I��M���I��H�]�H���    H��}_E1��M��H�]�H�u�L���    M��H�u�L���    H��tH���    M��L��H��P[A\A]A^A_A[]L3$L;    t��������I��L�u�M�H�}�H���    E1�I��H�}�L��   L���H�L���    L���    I��I��pI��u�E�H�E� L�u��^�����L�    L3$UH��ASAWAVAUATSH��@I��I��H�u��    H�5    L���    I��H����   L���    I��H����   L���    H��H����   �    H��t~L��H��H�E��    H��~xH�E�L���    H��xbH�E�L���    H��xQH�E�H�5    L���    H����   M�I�Lm�L�}�L}�I�M�H�E�I�$�0M���I���I�������I���H�}�I���    H�u�L���    H�u�L���    L��H��@[A\A]A^A_A[]L3$L;    t������������������I�����L�    L3$UH��ASAWAVH��(I��H�u��    H�5    L���    I��H��~,H�u�L���    H��~�}��    ��uH�u�L���    E1�H�u�L���    L��H��(A^A_A[]L3$L;    t���������������L�    L3$UH��ASAWAVAUATSH��`I��I��W�)E�H�E�    H��x����    H�5    L���    I��H����  L���    I��H��xLL���    H���}  M�I�H�5    L���    I��H���_  t M�H�E�I�G(E�A�]  M���=  H�5    L���    I��H���"  L�}�M����   L���    I��H���R  L���    H����  H�E�L��H�u��    H��H���*  L�}�L���    I��H���  L���    H����  M�Le�Le�L�L�I��I�L��H�5    �    I��H���y  ��  L��H�5    �    I��H���X  t1L���    I��H����  L���    H���.  M�I�M�M��L��H�5    �    I��H���
  ��   L���    H��H���F  L���    I��H���-  L��H�u��    H����   H�E�L�}�L���    I��H����   L���    H����   M�H]�H]�L�I�M��I�L��H�5    �    I��H��xl��   L��H�5    �    I��H��xOt-L���    I��H����   L���    H��x)M�I�M�M��L��H�5    �    I��H��������I��H��x���L���    H�}��    H��x���L���    L��H��`[A\A]A^A_A[]L3$L;    t	����������M���I���M�H�E�H�M�H�A(E����L�    L3$UH��ASAWAVAUATSH��PI��I��W�)E�H�E�    H�u��    H�5    L���    I��H����  H�}��    H����  H�}�1��    H�}�H��p1��    L���    I��H��xLL���    H���t  M�I�H�5    L���    I��H���V  t M�H�E�I�G(E�A�Q  M���4  H�5    L���    I��H���  L�}�M����   L���    H��H���N  L���    I��H���5  H�u�L���    H����  L�}�I��L���    H��H���
  L���    H����  M�M�Le�I�M��I�L��H�5    �    I��H���u  ��  L��H�5    �    I��H���T  t1L���    I��H����  L���    H���*  M�I�M�M��L��H�5    �    I��H���  ��   L���    H��H���B  L���    I��H���)  H�u�H��pL���    H����   L�}�L���    I��H����   L���    H����   M�H]�L�L�I��I�L��H�5    �    I��H��xl��   L��H�5    �    I��H��xOt-L���    I��H����   L���    H��x)M�I�M�M��L��H�5    �    I��H��������I��H�u�L���    H�}��    H�u�L���    L��H��P[A\A]A^A_A[]L3$L;    t����������������M���I���M�H�E�H�M�H�A(E����L�    L3$UH��ASAWAVAUATSH��0H�u�I��H�u��    H�u�L���    I��H��~-E1�L�m�1�L�K���M�L�<AL��L���    I��H����1�E1�M��xH�E�L�8I���H�u�L���    H�u�L���    L��H��0[A\A]A^A_A[]L3$L;    t���������������             GNU   �                $@      �      �?      $@      �_  0C  0E              0C      0E  0C  0E              0C      0E   A   A{ } buf_parse_array_data: buf_parse_array_data_rec:  
 buf_parse_array_data_rec: { buf_parse_array_data_rec: 1 buf_parse_array_data_rec: parse buf_parse_array_data_rec: buf_parse_array_data_rec buf_parse_array_data_rec: 2 , buf_parse_array_data_rec: 3 buf_parse_array_data_rec: 4 buf_parse_array_data_rec: } buf_parse_array_dimensions_rec: dimension mismatch:  buf_parse_array_data_rec: restore buf_parse_array_dimensions: zero item size buf_parse_array_dimensions: buf_parse_array_dimensions_rec:  buf_parse_array_dimensions_rec: { buf_parse_array_dimensions_rec: 1 buf_parse_array_dimensions_rec: buf_parse_tag buf_parse_array_dimensions_rec: buf_parse_array_dimensions_rec buf_parse_array_dimensions_rec: 2 buf_parse_array_dimensions_rec: 3 buf_parse_array_dimensions_rec: 4 buf_parse_array_dimensions_rec: } buf_parse_array_dimensions_rec: restore buf_parse_do_block: buf_parse_comments buf_parse_do_block: buf_ignore_spaces buf_parse_do_block: buf_parse_do_block_inner < 0 buf_parse_do_block_inner: buf_read_1 "}" buf_parse_do_block_inner: buf_read_sym :end buf_parse_do_block_inner: buf_parse_tag buf_parse_do_block_inner: list_new buf_parse_do_block_inner: buf_parse_comments 1 buf_parse_do_block_inner: buf_ignore_spaces_but_newline 1 buf_parse_do_block_inner: buf_read_1 "}" < 0 buf_parse_do_block_inner: buf_read_sym :end < 0 buf_parse_do_block_inner: line  : missing separator:  ; buf_parse_do_block_inner: buf_parse_comments 2 buf_parse_do_block_inner: buf_ignore_spaces_but_newline 2 true false [ ] . ( ) ,| cfn _macro ' # +i cow buf_parse_cow: type mismatch:   !=  buf_parse_digit: digit overflow:  (F32) e - + f (F64) (F128) buf_parse_fn_clause: invalid pattern buf_parse_fn_clause: invalid program (Ident) _" _ 0b 0B 0o 0O 0x 0X buf_parse_list: buf_parse_comments 1 buf_parse_list: buf_ignore_spaces 1 buf_parse_list: buf_read_1 "]" 1 buf_parse_list: buf_parse_list_tag < 0 buf_parse_list: buf_parse_list_tag = 0 buf_parse_list: buf_parse_comments 2 buf_parse_list: buf_ignore_spaces 2 buf_parse_list: buf_read_1 "]" 2 buf_parse_list: buf_read_1 "," buf_parse_list: buf_parse_comments 3 buf_parse_list: buf_ignore_spaces 3 | buf_parse_list: buf_read_1 "|" buf_parse_list: buf_parse_comments 4 buf_parse_list: buf_ignore_spaces 4 buf_parse_list: buf_parse_tag < 0 buf_parse_list: buf_parse_tag = 0 buf_parse_list: buf_parse_comments 5 buf_parse_list: buf_ignore_spaces 5 buf_parse_list: buf_read_1 "]" < 0 buf_parse_list: buf_read_1 "]" = 0 buf_parse_list: invalid list buf_parse_list_tag: buf_parse_sym_str : buf_parse_list_tag: buf_parse_tag %{ => buf_parse_ptag: not implemented (Ptr) buf_parse_ptr: non-null Ptr 0x (PtrFree) ? quote buf_parse_quote: buf_parse_tag / buf_parse_special_operator:  : ident_is_special_operator : special operator not found : buf_parse_comments : buf_ignore_spaces < 0 : missing argument : buf_parse_tag < 0 : buf_parse_tag = 0 " ""
 "" """ 
""" \x \ x % buf_parse_struct: struct_init( buf_parse_struct: struct_init_from_lists  :" %Time{ tv_sec: tv_nsec: unquote( void                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                zR x�  (          P   P�CN�����  (   H       @   P�CR������ $   t       �    P�CJ���H   $   �       �    P�CJ���H   ,   �       �   P�CO������q](   �       �   P�CR�������,          ^   P�CO������
T,   P      �   P�CR�������^ (   �          P�CN������   ,   �      �   P�CR������wT,   �      %   P�CO�������X (         P   P�CO������(   8      �   P�CR�������(   d      �    P�CN������   (   �         P�CO�������(   �      �    P�CK�����h   (   �      �    P�CN������   (            P�CN������   ,   @      �   P�CR������m^(   p      �   P�CO�������    �      P    P�CD��]  (   �      p   P�CQ�����9  (   �         P�CO������� $         �    P�CL����F ,   @         P�CR������V,   p      U   P�CQ�����Z  (   �      @   P�CO������$   �          P�CO����� (   �          P�CR�������(          �   P�CR�������(   L          P�CO������� ,   x      �   P�CR������
Q(   �      �   P�CR�������,   �      �   P�CO�������U (         �    P�CN������   ,   0      �   P�CN�����KT   (   `      @   P�CO������,   �         P�CR�������Q ,   �      �   P�CO������vX$   �      �    P�CL����� (         �    P�CG���PV   $   @      p    P�CG���A   $   h      p    P�CG���A       �      p    P�CG���,   �      5   P�CO�������W(   �      �   P�CO�������(         �   P�CO������p(   <      �   P�CR�������$   h          P�CO����� (   �          P�CN������   ,   �      �   P�CO������^P,   �      �   P�CR������dW(   	      �    P�CN������   ,   H	      �   P�CR������o\(   x	          P�CR�������(   �	      0   P�CR�������,   �	      �   P�CO�������S,    
      �   P�CO�������[,   0
      *   P�CO������~P ,   `
      *   P�CO������~P ,   �
      *   P�CO������~P (   �
      p   P�CO������7,   �
      �   P�CR�������_(            P�CR������� (   H         P�CO�������(   t      v    P�CG���k[Q,   �      J   P�CO������W(   �      �    P�CN������   (   �      �    P�CR������� $   (      p    P�CH����{  $   P      p    P�CH����z  $   x      p    P�CH����  $   �      �    P�CH����J ,   �         P�CR������YR   �      @    P�CC�Z(            P�CO������� (   D      �    P�CO������� (   p      `   P�CN�����+  (   �         P�CO������� (   �          P�CO������� ,   �      �   P�CO������%Y(   $      �   P�CN�����I  (   P      �    P�CN�����s   (   |      �    P�CO������� $   �      �    P�CL����k     �      P    P�CD��]      �      P    P�CD��]            P    P�CD��]      <      P    P�CD��]      `      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]            P    P�CD��]      8      P    P�CD��]      \      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]            P    P�CD��]      4      P    P�CD��]      X      P    P�CD��]      |      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]  (         p   P�CQ�����5      8      P    P�CD��]      \      P    P�CD��]  (   �      �    P�CN������   (   �      �   P�CO�������    �      P    P�CD��]      �      P    P�CD��]             P    P�CD��]      D      @    P�CD��Y      h      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]      �      P    P�CD��]     �          E�CA             P    P�CD��]  (   <      f    P�CG���[[Q,   h      .   P�CO�������U,   �      e   P�CO������_$   �      �    P�CJ���i   ,   �      r   P�CO�����