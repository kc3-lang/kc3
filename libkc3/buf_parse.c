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
#include "pcall.h"
#include "pcallable.h"
#include "pointer.h"
#include "pvar.h"
#include "ratio.h"
#include "securelevel.h"
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
      (tmp.dimension_count &&
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
  if (! dest->dimension_count) {
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
  address = alloc(tmp.dimension_count * sizeof(sw));
  if (! address)
    return -1;
  tmp.count = 1;
  i = 0;
  while (i < tmp.dimension_count) {
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
    err_inspect_sw_decimal(r);
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
      if (dimension == tmp.dimension_count - 1) {
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
          err_puts("buf_parse_array_data_rec: "
                   "buf_parse_array_data_rec");
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
    err_inspect_uw(dimension);
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
    dest->dimension_count = 0;
    r = result;
    goto clean;
  }
  tmp = *dest;
  tmp.dimension_count = 1;
  while (1) {
    if ((r = buf_read_1(buf, "{")) < 0)
      goto restore;
    if (! r)
      break;
    result += r;
    tmp.dimension_count++;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  tmp.dimensions = alloc(tmp.dimension_count *
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
  if (! sym_type_size(tmp.element_type, &size))
    return -1;
  if (! size) {
    err_puts("buf_parse_array_dimensions: zero item size");
    assert(! "buf_parse_array_dimensions: zero item size");
    return -1;
  }
  address = alloc(tmp.dimension_count * sizeof(sw));
  if (! address)
    return -1;
  tmp.dimensions[tmp.dimension_count - 1].item_size = size;
  if ((r = buf_parse_array_dimensions_rec(buf, &tmp, address,
                                          0)) < 0) {
    err_write_1("buf_parse_array_dimensions:"
                " buf_parse_array_dimensions_rec: ");
    err_inspect_sw_decimal(r);
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
    if (dimension == dest->dimension_count - 1) {
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
    if (dimension < tmp.dimension_count - 1)
      tmp.dimensions[dimension].item_size =
        tmp.dimensions[dimension + 1].count *
        tmp.dimensions[dimension + 1].item_size;
  }
  else if (tmp.dimensions[dimension].count != address[dimension]) {
    err_write_1("buf_parse_array_dimensions_rec: dimension mismatch: ");
    err_inspect_uw(dimension);
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

/*
sw buf_parse_do_block_inner (s_buf *buf, bool short_form,
                             s_do_block *do_block,
                             f_buf_parse_do_block_end f_end)
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
      i = &(*i)->next.data.plist;
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
      err_inspect_sw_decimal(buf->line);
      err_puts(": missing separator: ");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "buf_parse_do_block_inner: missing separator");
      goto restore;
    }
    if (! r) {
      if ((r = buf_read_1(buf, ";")) <= 0) {
        err_write_1("buf_parse_do_block_inner: line ");
        err_inspect_sw_decimal(buf->line);
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
*/

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
      args = &(*args)->next.data.plist;
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
  if (! ops_get_tag(ops, next_ident.sym, 2, &next_op_tag)) {
    r = 0;
    goto restore;
  }
  if (false) {
    err_write_1("buf_parse_call_op_rec: op = ");
    err_inspect_tag(&next_op_tag);
    err_write_1("\n");
  }
  next_op = next_op_tag.data.pstruct->data;
  while (r > 0 && next_op->precedence >= min_precedence) {
    tag_clean(&next_op_tag);
    next_op_tag = (s_tag) {0};
    if ((r = buf_parse_ident(buf, &next_ident)) <= 0)
      goto restore;
    result += r;
    if (! ops_get_tag(ops, next_ident.sym, 2, &next_op_tag)) {
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
      if (! (left->data.pcall = alloc(sizeof(s_call)))) {
        r = -1;
        goto restore;
      }
      *left->data.pcall = tmp3;
      left->type = TAG_PCALL;
      *right = (s_tag) {0};
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
    if (! ops_get_tag(ops, next_ident.sym, 2, &next_op_tag) &&
        ! ops_get_tag(ops, next_ident.sym, 1, &next_op_tag))
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
        call_clean(&tmp2);
        goto ok;
      }
      result += r;
      *right = (s_tag) {0};
      if (! (right->data.pcall = alloc(sizeof(s_call)))) {
        r = -1;
        goto restore;
      }
      *right->data.pcall = tmp2;
      right->type = TAG_PCALL;
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
      if (r <= 0 || ! (ops_get_tag(ops, next_ident.sym, 2, &next_op_tag)))
        goto ok;
      next_op = next_op_tag.data.pstruct->data;
    }
  }
 ok:
  result -= (buf->rpos - save.rpos);
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
  if (securelevel(0)) {
    err_puts("buf_parse_cfn: cannot parse Cfn with securelevel"
             " > 0");
    abort();
  }
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
  if ((r = buf_parse_plist_paren(buf, &arg_types)) <= 0)
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
  sw result_inner;
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
  result_inner = result;
  while (1) {
    buf_save_update(buf, &save);
    while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
           character_is_space(c) &&
           c != '\n') {
      csize = r;
      if ((r = buf_ignore(buf, csize)) <= 0)
        break;
      result_inner += csize;
    }
    if ((r = buf_parse_comment(buf)) <= 0)
      break;
    result_inner += r;
    result = result_inner;
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
      err_inspect_sym(tmp.type);
      err_write_1(" != ");
      err_inspect_sym(type);
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
      err_inspect_sw_decimal(digit);
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

sw buf_parse_do_block (s_buf *buf, s_do_block *do_block)
{
  f_buf_parse_end f_end = buf_parse_do_block_inner_end;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_do_block tmp = {0};
  assert(buf);
  assert(do_block);
  buf_save_init(buf, &save);
  if ((r = buf_read_sym(buf, &g_sym_do)) < 0)
    goto clean;
  if (! r) {
    if ((r = buf_read_1(buf, "{")) <= 0)
      goto clean;
    f_end = buf_parse_do_block_inner_end_short;
    tmp.short_form = true;
  }
  result += r;
  if ((r = buf_parse_comments(buf)) < 0) {
    err_puts("buf_parse_do_block: buf_parse_comments");
    assert(! "buf_parse_do_block: buf_parse_comments");
    goto restore;
  }
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0) {
    err_puts("buf_parse_do_block: buf_ignore_spaces");
    assert(! "buf_parse_do_block: buf_ignore_spaces");
    goto restore;
  }
  result += r;
  if ((r = buf_parse_do_block_inner(buf, f_end, &tmp)) < 0) {
    err_puts("buf_parse_do_block: buf_parse_do_block_inner < 0");
    assert(! "buf_parse_do_block: buf_parse_do_block_inner < 0");
    goto restore;
  }
  if (! r)
    goto restore;
  result += r;
  *do_block = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_do_block_inner (s_buf *buf, f_buf_parse_end f_end,
                             s_do_block *do_block)
{
  bool end;
  s_list **i;
  sw j;
  s_list *k;
  sw l;
  p_list list = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag tag;
  s_do_block tmp;
  i = &list;
  *i = NULL;
  buf_save_init(buf, &save);
  while (1) {
    if ((r = f_end(buf, &end)) < 0)
      goto restore;
    result += r;
    if (end)
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
      i = &(*i)->next.data.plist;
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
    if ((r = f_end(buf, &end)) < 0)
      goto restore;
    result += r;
    if (end)
      goto ok;
    if ((r = buf_read_1(buf, "\n")) < 0) {
      err_write_1("buf_parse_do_block_inner: line ");
      err_inspect_sw_decimal(buf->line);
      err_puts(": missing separator: ");
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "buf_parse_do_block_inner: missing separator");
      goto restore;
    }
    if (! r) {
      if ((r = buf_read_1(buf, ";")) <= 0) {
        err_write_1("buf_parse_do_block_inner: line ");
        err_inspect_sw_decimal(buf->line);
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
  j = list_length(list);
  if (! do_block_init(&tmp, j)) {
    r = -1;
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
  tmp.short_form = do_block->short_form;
  *do_block = tmp;
  r = result;
  goto clean;
}

sw buf_parse_do_block_inner_end (s_buf *buf, bool *dest)
{
  sw r;
  if ((r = buf_read_sym(buf, &g_sym_end)) < 0) {
    err_puts("buf_parse_do_block_inner_end: buf_read_sym :end");
    err_inspect_buf(buf);
    err_write_1("\n");
    assert(! "buf_parse_do_block_inner_end: buf_read_sym :end");
  }
  *dest = (r > 0) ? true : false;
  return r;
}

sw buf_parse_do_block_inner_end_short (s_buf *buf, bool *dest)
{
  sw r;
  if ((r = buf_read_1(buf, "}")) < 0) {
    err_puts("buf_parse_do_block_inner_end_short: buf_read_1 \"}\"");
    err_inspect_buf(buf);
    err_write_1("\n");
    assert(! "buf_parse_do_block_inner_end_short: buf_read_1 \"}\"");
  }
  *dest = (r > 0) ? true : false;
  return r;
}

sw buf_parse_f32 (s_buf *buf, f32 *dest)
{
  character c;
  u8 digit;
  s32 exp = 0;
  s8 exp_sign = 1;
  uw i;
  sw r;
  sw result = 0;
  s_buf_save save;
  s8 sign = 1;
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
  if ((r = buf_read_1(buf, "-")) > 0) {
    sign = -1;
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
    tmp += (f32) digit / i;
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
    tmp *= pow(10, exp_sign * exp);
  }
  if ((r = buf_read_1(buf, "f")) < 0)
    goto restore;
  result += r;
  if ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
      ! sym_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
  tmp *= sign;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_f64 (s_buf *buf, f64 *dest)
{
  u8 digit;
  f64 exp = 0;
  f64 exp_sign = 1;
  uw i;
  sw r;
  sw result = 0;
  s_buf_save save;
  f64 sign = 1;
  f64 tmp = 0;
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
  if ((r = buf_read_1(buf, "-")) > 0) {
    sign = -1;
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
  tmp *= sign;
  *dest = tmp;
  r = result;
  goto clean;
  restore:
  buf_save_restore_rpos(buf, &save);
  clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_f80 (s_buf *buf, f80 *dest) {
  u8 digit;
  f80 exp;
  f80 exp_sign = 1;
  sw r;
  sw result = 0;
  s_buf_save save;
  f80 sign = 1;
  f80 tmp = 0;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(F80)")) < 0)
    goto restore;
  result += r;
  if (r > 0) {
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_read_1(buf, "-")) > 0) {
    sign = -1;
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
  exp = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp += (f80) digit / exp;
    exp *= 10;
  }
  exp = 0;
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
  tmp *= sign;
  *dest = tmp;
  r = result;
  goto clean;
  restore:
  buf_save_restore_rpos(buf, &save);
  clean:
  buf_save_clean(buf, &save);
  return r;
}

#if HAVE_FLOAT128

sw buf_parse_f128 (s_buf *buf, f128 *dest) {
  u8 digit;
  f128 exp;
  f128 exp_sign = 1;
  sw r;
  sw result = 0;
  s_buf_save save;
  f128 sign = 1;
  f128 tmp = 0;
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
  if ((r = buf_read_1(buf, "-")) > 0) {
    sign = -1;
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
  exp = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp += (f128) digit / exp;
    exp *= 10;
  }
  exp = 0;
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
  tmp *= sign;
  *dest = tmp;
  r = result;
  goto clean;
  restore:
  buf_save_restore_rpos(buf, &save);
  clean:
  buf_save_clean(buf, &save);
  return r;
}

#endif

sw buf_parse_fact (s_buf *buf, s_fact *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_fact tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if (! (tmp.subject = tag_new()) ||
      ! (tmp.predicate = tag_new()) ||
      ! (tmp.object = tag_new())) {
    r = -1;
    goto clean;
  }
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_static_tag(buf, tmp.subject)) <= 0)
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
  if ((r = buf_parse_static_tag(buf, tmp.predicate)) <= 0)
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
  if ((r = buf_parse_static_tag(buf, tmp.object)) <= 0)
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
  fact_clean_all(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fact_w (s_buf *buf, s_fact_w *dest)
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
  s_fn_clause **tail;
  s_fn tmp = {0};
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
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if (securelevel(0) > 1) {
    err_puts("buf_parse_fn: cannot parse Fn with securelevel > 1");
    abort();
  }
  fn_init(&tmp, ident.module);
  tail = &tmp.clauses;
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
      tail = &(*tail)->next;
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
  buf_save_update(buf, &save);
  if ((r = buf_ignore_spaces_but_newline(buf)) < 0 ||
      (r1 = buf_parse_plist(buf, &frame_list)) <= 0)
    goto ok;
  result += r + r1;
  if (! frame_list)
    goto ok;
  tmp.frame = frame_new(NULL);
  l = frame_list;
  while (l) {
    if (! frame_binding_new(tmp.frame,
                            l->tag.data.tuple.tag[0].data.psym,
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
    tail = &(*tail)->next.data.plist;
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

sw buf_parse_ident_cast (s_buf *buf, s_ident *dest)
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
  if (r) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_ident(buf, &tmp)) < 0)
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

sw buf_parse_ident (s_buf *buf, s_ident *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_ident tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_module_name(buf, &tmp.module)) < 0)
    goto restore;
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
    i = &(*i)->next.data.plist;
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

sw buf_parse_integer_decimal (s_buf *buf, bool negative,
                              s_integer *dest)
{
  sw r;
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  if ((r = buf_parse_integer_unsigned_dec(buf, &tmp)) <= 0)
    return r;
  if (negative) {
    integer_neg(&tmp, &tmp2);
    integer_clean(&tmp);
    tmp = tmp2;
  }
  *dest = tmp;
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

sw buf_parse_plist (s_buf *buf, p_list *plist)
{
  p_list *i;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(plist);
  buf_save_init(buf, &save);
  i = plist;
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
    *plist = NULL;
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
      i = &(*i)->next.data.plist;
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
    err_puts("buf_parse_list: invalid list:");
    err_inspect_buf(buf);
    assert(! "buf_parse_list: invalid list");
    r = -1;
    goto restore;
  }
 restore:
  list_delete_all(*plist);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_plist_paren (s_buf *buf, p_list *dest)
{
  p_list tmp = NULL;
  p_list *tail;
  sw r;
  sw result = 0;
  s_buf_save save;
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
    goto ok;
  }
  tail = &tmp;
  while (1) {
    *tail = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*tail)->tag)) <= 0)
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
      goto ok;
    }
    if ((r = buf_read_1(buf, ",")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      tail = &(*tail)->next.data.plist;
      continue;
    }
    if ((r = buf_read_1(buf, "|")) <= 0)
      goto restore;
    result += r;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_parse_tag(buf, &(*tail)->next)) <= 0)
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
    goto ok;
  }
 ok:
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  list_delete_all(tmp);
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
  if ((r = buf_parse_sym_str(buf, &str)) < 0) {
    err_puts("buf_parse_list_tag: buf_parse_sym_str");
    assert(! "buf_parse_list_tag: buf_parse_sym_str");
    goto clean;
  }
  if (r > 0) {
    result += r;
    if ((r = buf_read_1(buf, ":") <= 0)) {
      str_clean(&str);
      goto tag;
    }
    result += r;
    if ((r = buf_parse_comments(buf)) < 0) {
      str_clean(&str);
      goto tag;
    }
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0) {
      str_clean(&str);
      goto tag;
    }
    result += r;
    if ((r = buf_parse_tag(buf, &value)) <= 0) {
      str_clean(&str);
      goto tag;
    }
    result += r;
    if (! tag_init_tuple(&tmp, 2)) {
      str_clean(&str);
      tag_clean(&value);
      return -2;
    }
    key.type = TAG_PSYM;
    key.data.psym = str_to_sym(&str);
    str_clean(&str);
    tmp.data.tuple.tag[0] = key;
    tmp.data.tuple.tag[1] = value;
    goto ok;
  }
 tag:
  result = 0;
  buf_save_restore_rpos(buf, &save);
  if ((r = buf_parse_tag(buf, &tmp)) <= 0) {
    err_puts("buf_parse_list_tag: buf_parse_tag");
    err_inspect_buf(buf);
    err_write_1("\n");
    assert(! "buf_parse_list_tag: buf_parse_tag");
    if (! r)
      r = -1;
    goto clean;
  }
  result += r;
 ok:
  *dest = tmp;
  r = result;
  goto clean;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_map (s_buf *buf, s_map *dest)
{
  s_list  *keys;
  s_list **keys_end;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_list  *values;
  s_list **values_end;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "%{")) <= 0)
    goto clean;
  result += r;
  keys = NULL;
  keys_end = &keys;
  values = NULL;
  values_end = &values;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "}")) < 0)
    goto restore;
  result += r;
  while (r == 0) {
    *keys_end = list_new(NULL);
    if ((r = buf_parse_map_key(buf, &(*keys_end)->tag)) <= 0)
      goto restore;
    result += r;
    keys_end = &(*keys_end)->next.data.plist;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    *values_end = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*values_end)->tag)) <= 0)
      goto restore;
    result += r;
    values_end = &(*values_end)->next.data.plist;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "}")) < 0)
      goto restore;
    result += r;
    if (r == 0) {
      if ((r = buf_read_1(buf, ",")) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      r = 0;
    }
  }
  if (! map_init_from_lists(dest, keys, values)) {
    r = 0;
    goto restore;
  }
  r = result;
  list_delete_all(keys);
  list_delete_all(values);
  goto clean;
 restore:
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_map_key (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_map_key_str(buf, dest)) ||
      (r = buf_parse_map_key_sym(buf, dest)) ||
      (r = buf_parse_map_key_tag(buf, dest)))
    return r;
  return 0;
}

sw buf_parse_map_key_str (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_str(buf, &str)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_read_1(buf, ":")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      dest->type = TAG_PSYM;
      dest->data.psym = str_to_sym(&str);
      str_clean(&str);
      goto ok;
    }
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "=>")) <= 0)
      goto restore;
    result += r;
    dest->type = TAG_STR;
    dest->data.str = str;
    goto ok;
  }
 ok:
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_map_key_sym (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_sym_str(buf, &str)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_read_1(buf, ":")) <= 0)
      goto restore;
    result += r;
    dest->type = TAG_PSYM;
    dest->data.psym = str_to_sym(&str);
    str_clean(&str);
  }
  r = result;
  goto clean;
 restore:
  str_clean(&str);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_map_key_tag (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag tag;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_tag(buf, &tag)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "=>")) <= 0)
    goto restore;
  result += r;
  *dest = tag;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_match (s_buf *buf, p_call *pcall)
{
  p_list arg = NULL;
  s_do_block *do_block = NULL;
  s_map map = {0};
  sw r;
  sw result = 0;
  s_buf_save save;
  p_call tmp = NULL;
  p_list  keys = NULL;
  p_list *keys_tail = &keys;
  p_list  values = NULL;
  p_list *values_tail = &values;
  assert(buf);
  assert(pcall);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "match")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if (! pcall_init(&tmp) ||
      ! (arg = list_new(NULL)) ||
      ! (tmp->arguments = list_new(arg)))
    goto ko;
  tmp->ident.module = &g_sym_KC3;
  tmp->ident.sym = &g_sym_match;
  if ((r = buf_parse_tag(buf, &tmp->arguments->tag)) <= 0) {
    pcall_clean(&tmp);
    goto restore;
  }
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "do")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  while (1) {
    if (! (*keys_tail = list_new(NULL)) ||
        (r = buf_parse_tag(buf, &(*keys_tail)->tag)) <= 0 ||
        ! (result += r) ||
        (r = buf_ignore_spaces(buf)) < 0 ||
        ! (result += r) ||
        ((r = buf_read_1(buf, "->")) <= 0 &&
         (r = buf_read_1(buf, "→")) <= 0) ||
        ! (result += r) ||
        (r = buf_ignore_spaces(buf)) < 0 ||
        ! (result += r) ||
        ! (*values_tail = list_new(NULL)) ||
        ! (do_block = &(*values_tail)->tag.data.do_block) ||
        (r = buf_parse_do_block_inner(buf, buf_parse_match_end,
                                      do_block)) <= 0)
      goto ko;
    result += r;
    (*values_tail)->tag.type = TAG_DO_BLOCK;
    keys_tail = &(*keys_tail)->next.data.plist;
    values_tail = &(*values_tail)->next.data.plist;
    if (buf->rpos > 3 &&
        buf->ptr.pchar[buf->rpos - 3] == 'e' &&
        buf->ptr.pchar[buf->rpos - 2] == 'n' &&
        buf->ptr.pchar[buf->rpos - 1] == 'd')
      break;
  }
  if (! map_init_from_lists(&map, keys, values))
    goto ko;
  arg->tag.type = TAG_MAP;
  arg->tag.data.map = map;
  *pcall = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
 ko:
  list_delete_all(values);
  list_delete_all(keys);
  pcall_clean(&tmp);
  r = -1;
  goto clean;
}

sw buf_parse_match_end (s_buf *buf, bool *dest)
{
  sw r;
  s_buf_save save;
  s_tag tag = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  *dest = false;
  if ((r = buf_read_1(buf, "end")) < 0)
    goto clean;
  if (r) {
    *dest = true;
    goto clean;
  }
  if ((r = buf_parse_tag(buf, &tag)) <= 0)
    goto clean;
  tag_clean(&tag);
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  if ((r = buf_read_1(buf, "->")) <= 0 &&
      (r = buf_read_1(buf, "→")) <= 0)
    goto restore;
  *dest = true;
  r = 0;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_module_name (s_buf *buf, p_sym *dest)
{
  sw r;
  sw r1;
  sw result = 0;
  s_buf_save save;
  s_str str;
  p_sym tmp;
  s_buf tmp_buf = {0};
  buf_save_init(buf, &save);
  if ((r = buf_parse_module_name_sym_ignore(buf)) <= 0)
    goto clean;
  result += r;
  while (1) {
    if ((r = buf_read_1(buf, ".")) <= 0)
      break;
    if ((r1 = buf_parse_module_name_sym_ignore(buf)) <= 0) {
      if (buf->rpos < (uw) r) {
        err_puts("buf_parse_module_name: invalid rpos");
        assert(! "buf_parse_module_name: invalid rpos");
        abort();
      }
      buf->rpos -= r;
      break;
    }
    result += r + r1;
  }
  if ((uw) result != buf->rpos - save.rpos) {
    err_puts("buf_parse_module_name: invalid result");
    assert(! "buf_parse_module_name: invalid result");
    r = -1;
    goto restore;
  }
  if (! str_init_alloc(&str, result)) {
    r = -1;
    goto restore;
  }
  memcpy(str.free.p, buf->ptr.pchar + save.rpos, str.size);
  tmp = str_to_sym(&str);
  str_clean(&str);
  if (! tmp) {
    r = -1;
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  buf_clean(&tmp_buf);
  return r;
}

sw buf_parse_module_name_sym_ignore (s_buf *buf)
{
  character c;
  sw r;
  sw result = 0;
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    return r;
  if (! character_is_uppercase(c))
    return 0;
  result += r;
  if ((r = buf_ignore(buf, r)) <= 0)
    return -1;
  while (1) {
    if ((r = buf_peek_character_utf8(buf, &c)) <= 0 ||
        ! character_is_alphanum(c))
      break;
    result += r;
    if ((r = buf_ignore(buf, r)) <= 0)
      return -1;
  }
  return result;
}

sw buf_parse_new_tag (s_buf *buf, s_tag **dest)
{
  sw r;
  s_tag *tag = tag_new();
  if ((r = buf_parse_tag(buf, tag)) < 0) {
    tag_delete(tag);
    *dest = NULL;
    return r;
  }
  *dest = tag;
  return r;
}

sw buf_parse_paren_sym (s_buf *buf, const s_sym **dest)
{
  uw b;
  sw r;
  sw result = 0;
  s_buf_save save;
  const s_sym *tmp;
  s_buf        tmp_buf;
  char         tmp_buf_data[64] = {0};
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_sym(buf, &tmp)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_peek_1(buf, "[")) < 0)
    goto restore;
  if (r > 0) {
    while (1) {
      if ((r = buf_read_1(buf, "[")) < 0)
        goto restore;
      if (! r)
        break;
      result += r;
      buf_init(&tmp_buf, false, sizeof(tmp_buf_data),
              tmp_buf_data);
      buf_inspect_sym(&tmp_buf, tmp);
      buf_write_1(&tmp_buf, "[");
      if ((r = buf_parse_uw(buf, &b)) <= 0)
        goto restore;
      result += r;
      buf_inspect_uw(&tmp_buf, b);
      if ((r = buf_read_1(buf, "]")) <= 0)
        goto restore;
      result += r;
      buf_write_1(&tmp_buf, "]");
    }
    buf_write_u8(&tmp_buf, 0);
    tmp = sym_1(tmp_buf.ptr.pchar);
  }
  if ((r = buf_read_1(buf, ")")) <= 0)
    goto restore;
  result += r;
  r = result;
  *dest = tmp;
  goto clean;
 restore:
  r = 0;
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_pcall (s_buf *buf, p_call *dest)
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
    goto restore;
  }
  result += r;
  if ((r = buf_parse_call_args_paren(buf, &tmp)) <= 0)
    goto restore;
  result += r;
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  **dest = tmp;
  r = result;
  goto clean;
 restore:
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_pcall_access (s_buf *buf, p_call *dest)
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
    k = &(*k)->next.data.plist;
    if (r <= 0)
      goto restore;
    result += r;
    if ((r = buf_peek_1(buf, ".")) < 0)
      goto restore;
    if (r == 0)
      break;
  }
  list_next(tmp.arguments)->tag.type = TAG_PLIST;
  list_next(tmp.arguments)->tag.data.plist = key;
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  **dest = tmp;
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

sw buf_parse_pcall_brackets (s_buf *buf, p_call *dest)
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
  call_init(&tmp);
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
    addr_last = &(*addr_last)->next.data.plist;
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
  arg_addr->type = TAG_PLIST;
  arg_addr->data.plist = addr;
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  **dest = tmp;
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

sw buf_parse_pcall_cast (s_buf *buf, p_call *dest)
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
  if (sym_is_pointer_type(module, NULL))
    module = &g_sym_Pointer;
  ident_init(&tmp.ident, module, &g_sym_cast);
  tag_init_psym(&tmp.arguments->tag, module);
  if ((r = buf_parse_tag_primary
       (buf, &list_next(tmp.arguments)->tag)) <= 0)
    goto clean;
  result += r;
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  **dest = tmp;
  r = result;
  goto clean;
 restore:
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_pcall_if (s_buf *buf, p_call *dest)
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
  call_init(&tmp);
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
  args_last = &(*args_last)->next.data.plist;
  *args_last = list_new(NULL);
  then = &(*args_last)->tag;
  if ((r = buf_parse_if_then(buf, then, &has_else)) < 0)
    goto restore;
  result += r;
  args_last = &(*args_last)->next.data.plist;
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
    else_->data.do_block = (s_do_block) {0};
    if ((r = buf_parse_do_block_inner(buf, buf_parse_do_block_inner_end,
                                      &else_->data.do_block)) <= 0)
      goto restore;
    result += r;
  }
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  **dest = tmp;
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

sw buf_parse_pcall_op (s_buf *buf, p_call *dest)
{
  s_ident ident;
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp = {0};
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
  if (! ops_get_tag(ops, ident.sym, 2, &op_tag))
    goto restore;
  tag_clean(&op_tag);
  if ((r = buf_parse_call_op_rec(buf, &tmp, 0)) <= 0)
    goto restore;
  result += r;
  if (! (*dest = alloc(sizeof(s_call))))
    goto restore;
  **dest = tmp;
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

sw buf_parse_pcall_op_unary (s_buf *buf, p_call *dest)
{
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp = {0};
  assert(buf);
  assert(dest);
  call_init_op_unary(&tmp);
  buf_save_init(buf, &save);
  if ((r = buf_parse_ident(buf, &tmp.ident)) <= 0)
    goto restore;
  result += r;
  ops = env_global()->ops;
  if (! ops_get_tag(ops, tmp.ident.sym, 1, &op_tag)) {
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
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto clean;
  }
  **dest = tmp;
  return result;
 restore:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  r = 0;
 clean:
  call_clean(&tmp);
  return r;
}

sw buf_parse_pcall_paren (s_buf *buf, p_call *dest)
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
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  **dest = tmp;
  r = result;
  goto clean;
 restore:
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_pcallable (s_buf *buf, p_callable *dest)
{
  s_cfn cfn = {0};
  s_fn fn = {0};
  sw r;
  p_callable tmp = NULL;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_cfn(buf, &cfn)) > 0) {
    if (securelevel(0)) {
      err_puts("buf_parse_pcallable: cannot parse Cfn with securelevel"
               " > 0");
      abort();
    }
    if (! (tmp = callable_new())) {
      cfn_clean(&cfn);
      return -1;
    }
    tmp->type = CALLABLE_CFN;
    tmp->data.cfn = cfn;
    *dest = tmp;
  }
  else if ((r = buf_parse_fn(buf, &fn)) > 0) {
    if (securelevel(0) > 1) {
      err_puts("buf_parse_pcallable: cannot parse Fn with securelevel"
               " > 1");
      abort();
    }
    if (! (tmp = callable_new())) {
      fn_clean(&fn);
      return -1;
    }
    tmp->type = CALLABLE_FN;
    tmp->data.fn = fn;
    *dest = tmp;
  }
  else
    return 0;
  return r;
}

// TODO: no alloc before parsing, like plist
sw buf_parse_pcomplex (s_buf *buf, s_complex **c)
{
  sw r;
  s_complex *tmp;
  tmp = complex_new();
  if ((r = buf_parse_complex(buf, tmp)) <= 0) {
    free(tmp);
    return r;
  }
  *c = tmp;
  return r;
}

// TODO: no alloc before parsing, like plist
sw buf_parse_pcow (s_buf *buf, s_cow **c)
{
  sw r;
  s_cow *tmp;
  tmp = alloc(sizeof(s_cow));
  if ((r = buf_parse_cow(buf, tmp)) <= 0) {
    free(tmp);
    return r;
  }
  *c = tmp;
  return r;
}

sw buf_parse_pointer (s_buf *buf, s_pointer *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  const s_sym *module_name;
  const s_sym *pointer_type;
  s_buf        pointer_type_buf = {0};
  s_pointer tmp = {0};
  uw u;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_module_name(buf, &module_name)) <= 0)
    goto restore;
  result += r;
  if (false) {
    err_write_1("buf_parse_pointer: module_name = ");
    err_inspect_sym(module_name);
    err_write_1("\n");
  }
  if ((r = buf_read_1(buf, "*")) <= 0)
    goto restore;
  result += r;
  if (! buf_init_alloc(&pointer_type_buf, BUF_SIZE)) {
    r = -1;
    goto restore;
  }
  if ((r = buf_inspect_sym(&pointer_type_buf, module_name)) <= 0 ||
      (r = buf_write_1(&pointer_type_buf, "*")) <= 0) {
    r = -1;
    buf_clean(&pointer_type_buf);
    goto restore;
  }
  while (1) {
    if ((r = buf_read_1(buf, "*")) < 0) {
      buf_clean(&pointer_type_buf);
      goto restore;
    }
    if (! r)
      break;
    if ((r = buf_write_1(&pointer_type_buf, "*")) <= 0) {
      r = -1;
      buf_clean(&pointer_type_buf);
      goto restore;
    }
  }
  if ((r = buf_read_to_sym(&pointer_type_buf, &pointer_type)) <= 0) {
    r = -1;
    buf_clean(&pointer_type_buf);
    goto restore;
  }
  buf_clean(&pointer_type_buf);
  if ((r = buf_read_1(buf, ")")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_uw(buf, &u)) <= 0)
    goto restore;
  result += r;
  if (false) {
    err_write_1("buf_parse_pointer: ");
    err_inspect_sym(pointer_type);
    err_write_1(" = 0x");
    err_inspect_uw_hexadecimal(u);
    err_write_1("\n");
  }
  if (! pointer_init(&tmp, pointer_type, NULL, (void *) u)) {
    r = -1;
    goto restore;
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

// TODO: no alloc before parsing, like plist
sw buf_parse_pstruct (s_buf *buf, p_struct *dest)
{
  sw r;
  p_struct tmp;
  if (! (tmp = struct_new(NULL)))
    return -1;
  if ((r = buf_parse_struct(buf, tmp)) <= 0) {
    free(tmp);
    return r;
  }
  *dest = tmp;
  return r;
}

sw buf_parse_ptag (s_buf *buf, p_tag *dest)
{
  (void) buf;
  (void) dest;
  assert(! "buf_parse_ptag: not implemented");
  err_puts("buf_parse_ptag: not implemented");
  return -1;
}

sw buf_parse_ptr (s_buf *buf, u_ptr_w *dest)
{
  s_integer i = {0};
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(Ptr)")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_integer(buf, &i)) <= 0)
    goto restore;
  result += r;
  if (! integer_is_zero(&i)) {
    err_write_1("buf_parse_ptr: non-null Ptr 0x");
    err_inspect_integer_hexadecimal(&i);
    err_write_1("\n");
    assert("buf_parse_ptr: non-null Ptr");
  }
  integer_clean(&i);
  dest->p = NULL;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_ptr_free (s_buf *buf, u_ptr_w *dest)
{
  s_integer i = {0};
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(PtrFree)")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_integer(buf, &i)) <= 0)
    goto restore;
  result += r;
  if (! integer_is_zero(&i)) {
    integer_clean(&i);
    goto restore;
  }
  integer_clean(&i);
  dest->p = NULL;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_pvar (s_buf *buf, p_var *dest)
{
  character c;
  uw i;
  sw r;
  sw result = 0;
  s_buf_save save;
  p_var tmp = NULL;
  const s_sym *type = NULL;
  assert(buf);
  (void) dest;
  buf_save_init(buf, &save);
  type = &g_sym_Tag;
  if ((r = buf_parse_paren_sym(buf, &type)) < 0)
    goto clean;
  result += r;
  if (r) {
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_read_1(buf, "?")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "0x")) < 0)
    goto ok;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_uw_hexadecimal(buf, &i)) <= 0)
      goto restore;
    result += r;
    tmp = (p_var) i;
    if (buf_peek_character_utf8(buf, &c) > 0 &&
        ! ident_character_is_reserved(c)) {
      r = 0;
      goto restore;
    }
    goto ok;
  }
  else {
    if (! pvar_init(&tmp, type)) {
      r = -1;
      goto restore;
    }
  }
 ok:
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_quote (s_buf *buf, s_quote *dest)
{
  s_quote quote;
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "quote")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  quote.tag = tag_new();
  if (! quote.tag) {
    r = -2;
    goto restore;
  }
  if ((r = buf_parse_tag(buf, quote.tag)) <= 0) {
    err_puts("buf_parse_quote: buf_parse_tag");
    err_inspect_buf(buf);
    err_write_1("\n");
    assert(! "buf_parse_quote: buf_parse_tag");
    free(quote.tag);
    goto restore;
  }
  result += r;
  *dest = quote;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_ratio (s_buf *buf, s_ratio *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_ratio tmp = {0};
  buf_save_init(buf, &save);
  if ((r = buf_parse_integer(buf, &tmp.numerator)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_read_1(buf, "/")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_integer(buf, &tmp.denominator)) <= 0)
    goto restore;
  result += r;
  if (! integer_is_positive(&tmp.denominator)) {
    r = -1;
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  ratio_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_special_operator (s_buf *buf, p_call *dest)
{
  bool b;
  s_list **args_last;
  s8 arity;
  sw i;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  call_init(&tmp);
  if ((r = buf_parse_ident(buf, &tmp.ident)) <= 0)
    goto restore;
  result += r;
  if (! ident_is_special_operator(&tmp.ident, &b)) {
    err_write_1("buf_parse_special_operator: ");
    err_inspect_ident(&tmp.ident);
    err_puts(": ident_is_special_operator");
    r = -1;
    goto restore;
  }
  if (! b) {
    r = 0;
    goto restore;
  }
  if ((arity = special_operator_arity(&tmp.ident)) < 0) {
    err_write_1("buf_parse_special_operator: ");
    err_inspect_ident(&tmp.ident);
    err_puts(": arity not found");
    assert(! "buf_parse_special_operator: arity not found");
    r = -1;
    goto restore;
  }
  args_last = &tmp.arguments;
  i = 0;
  while (i < arity) {
    if ((r = buf_parse_comments(buf)) < 0) {
      err_write_1("buf_parse_special_operator: ");
      err_inspect_ident(&tmp.ident);
      err_puts(": buf_parse_comments");
      assert(! "buf_parse_special_operator: buf_parse_comments");
      goto restore;
    }
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0) {
      err_write_1("buf_parse_special_operator: ");
      err_inspect_ident(&tmp.ident);
      err_puts(": buf_ignore_spaces < 0");
      assert(! "buf_parse_special_operator: buf_ignore_spaces < 0");
      goto restore;
    }
    if (! r)
      goto restore;
    result += r;
    *args_last = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*args_last)->tag)) < 0) {
      err_write_1("buf_parse_special_operator: ");
      err_inspect_ident(&tmp.ident);
      err_puts(": buf_parse_tag < 0");
      assert(! "buf_parse_special_operator: buf_parse_tag < 0");
      goto restore;
    }
    if (! r) {
      err_write_1("buf_parse_special_operator: ");
      err_inspect_ident(&tmp.ident);
      err_puts(": buf_parse_tag = 0");
      assert(! "buf_parse_special_operator: buf_parse_tag = 0");
      r = -1;
      goto restore;
    }
    result += r;
    args_last = &(*args_last)->next.data.plist;
    i++;
  }
  if (! (*dest = alloc(sizeof(s_call)))) {
    r = -1;
    goto restore;
  }
  r = result;
  **dest = tmp;
  goto clean;
 restore:
  call_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_static_tag (s_buf *buf, s_tag *tag)
{
  sw r;
  s_buf_save save;
  s_str str;
  s_tag tmp;
  assert(buf);
  assert(tag);
  buf_save_init(buf, &save);
  if ((r = buf_parse_tag(buf, &tmp)) <= 0)
    return r;
  if (tmp.type == TAG_PCALL &&
      (! tmp.data.pcall->ident.module ||
       tmp.data.pcall->ident.module == &g_sym_KC3) &&
      tmp.data.pcall->ident.sym == &g_sym_str &&
      tmp.data.pcall->arguments &&
      tmp.data.pcall->arguments->tag.type == TAG_PLIST) {
    tag_void(&tmp);
    buf_save_restore_rpos(buf, &save);
    if ((r = buf_parse_str(buf, &str)) <= 0)
      goto clean;
    tmp.type = TAG_STR;
    tmp.data.str = str;
  }
  *tag = tmp;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_str (s_buf *buf, s_str *dest)
{
  u8 b;
  character c;
  char *end = "\"";
  char *end1 = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "\"")) <= 0)
    goto save_clean;
  result += r;
  buf_init_alloc(&tmp, STR_MAX);
  if ((r = buf_read_1(buf, "\"\"\n")) < 0 ||
      (! r && (r = buf_read_1(buf, "\"\"")) < 0))
    goto restore;
  if (r > 0) {
    result += r;
    end = "\"\"\"";
    if (r == 3)
      end1 = "\n\"\"\"";
  }
  while (1) {
    if ((r = buf_read_1(buf, end)) < 0 ||
        (! r && end1 && (r = buf_read_1(buf, end1)) < 0))
      goto restore;
    if (r > 0) {
      result += r;
      break;
    }
    if ((r = buf_parse_str_character(buf, &c)) <= 0) {
      if ((r = buf_parse_str_u8(buf, &b)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_write_u8(&tmp, b)) < 0)
        goto restore;
      continue;
    }
    result += r;
    if ((r = buf_write_character_utf8(&tmp, c)) < 0)
      goto restore;
  }
  buf_read_to_str(&tmp, dest);
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_clean(&tmp);
 save_clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_str_character (s_buf *buf, character *dest)
{
  character c;
  sw csize;
  sw r;
  sw r1 = 0;
  sw r2 = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_peek_1(buf, "\\x")) < 0)
    goto clean;
  if (r > 0) {
    r = 0;
    goto clean;
  }
  if ((r = buf_read_1(buf, "\\")) < 0)
    goto clean;
  if (r > 0) {
    if ((r1 = buf_read_character_utf8(buf, &c)) <= 0) {
      r = r1;
      goto restore;
    }
    switch (c) {
    case '0': c = 0; break;
    case 'U': case 'u':
      if ((r2 = buf_parse_str_character_unicode(buf, &c)) < 0) {
        r = r2;
        goto restore;
      }
      break;
    case 'n': c = '\n'; break;
    case 'r': c = '\r'; break;
    case 's': c = ' '; break;
    case 't': c = '\t'; break;
    case 'v': c = '\v'; break;
    default: ;
    }
    *dest = c;
    r = r + r1 + r2;
    goto clean;
  }
  if ((r = buf_peek_character_utf8(buf, &c)) < 0)
    goto clean;
  if (c == '"') {
    r = 0;
    goto clean;
  }
  csize = r;
  if ((r = buf_ignore(buf, csize)) < 0)
    goto clean;
  *dest = c;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_str_character_unicode (s_buf *buf, character *dest)
{
  sw r;
  sw result = 0;
  u64 tmp;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "+")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_u64_hex(buf, &tmp)) <= 0) {
    buf_save_restore_rpos(buf, &save);
    goto clean;
  }
  *dest = (character) tmp;
  result += r;
  r = result;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_str_eval (s_buf *buf, s_tag *dest)
{
  s_str in;
  sw r;
  sw result;
  s_buf_save save;
  buf_save_init(buf, &save);
  r = buf_parse_str(buf, &in);
  if (r <= 0)
    goto clean;
  result = r;
  if (! str_parse_eval(&in, dest)) {
    r = -2;
    goto restore;
  }
  r = result;
  goto clean_in;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean_in:
  str_clean(&in);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_str_u8 (s_buf *buf, u8 *dest)
{
  u8 digit[3];
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "\\")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_read_1(buf, "x")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_digit_hex(buf, &digit[0])) < 0)
        goto restore;
      if (r == 0)
        goto read_u8;
      result += r;
      if ((r = buf_parse_digit_hex(buf, &digit[1])) < 0)
        goto restore;
      if (r == 0)
        goto read_u8;
      result += r;
      *dest = digit[0] * 16 + digit[1];
      r = result;
      goto clean;
    }
    if ((r = buf_parse_digit_oct(buf, &digit[0])) < 0)
      goto restore;
    if (r == 0)
      goto read_u8;
    result += r;
    if ((r = buf_parse_digit_oct(buf, &digit[1])) < 0)
      goto restore;
    if (r == 0)
      goto read_u8;
    result += r;
    if ((r = buf_parse_digit_oct(buf, &digit[2])) < 0)
      goto restore;
    if (r == 0)
      goto read_u8;
    result += r;
    *dest = digit[0] * 64 + digit[1] * 8 + digit[2];
    r = result;
    goto clean;
  }
 read_u8:
  r = buf_read_u8(buf, dest);
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_struct (s_buf *buf, s_struct *dest)
{
  s_list  *keys = NULL;
  s_list **keys_end;
  const s_sym *module;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_struct tmp = {0};
  s_list  *values = NULL;
  s_list **values_end;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "%")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_module_name(buf, &module)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto restore;
  result += r;
  keys = NULL;
  keys_end = &keys;
  values = NULL;
  values_end = &values;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "}")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if (! struct_init(&tmp, module)) {
      err_write_1("buf_parse_struct: struct_init(");
      err_inspect_sym(module);
      err_puts(")");
      assert(! "buf_parse_struct: struct init");
      r = -2;
      goto clean;
    }
    goto ok;
  }
  while (1) {
    *keys_end = list_new(NULL);
    if ((r = buf_parse_map_key(buf, &(*keys_end)->tag)) <= 0)
      goto restore;
    result += r;
    keys_end = &(*keys_end)->next.data.plist;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    *values_end = list_new(NULL);
    if ((r = buf_parse_tag(buf, &(*values_end)->tag)) <= 0)
      goto restore;
    result += r;
    values_end = &(*values_end)->next.data.plist;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "}")) < 0)
      goto restore;
    result += r;
    if (r > 0)
      break;
    if ((r = buf_read_1(buf, ",")) <= 0)
      goto restore;
    result += r;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    r = 0;
  }
  if (! struct_init_from_lists(&tmp, module, keys, values)) {
    err_write_1("buf_parse_struct: struct_init_from_lists ");
    err_inspect_sym(module);
    err_puts(".");
    r = -2;
    goto clean;
  }
 ok:
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_sym (s_buf *buf, const s_sym **dest)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  char t[SYM_MAX];
  s_buf tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_peek_1(buf, ":\"")) < 0)
    goto clean;
  if (r > 0) {
    if ((r = buf_read_1(buf, ":")) <= 0)
      goto clean;
    result += r;
    if ((r = buf_parse_str(buf, &str)) <= 0)
      goto restore;
    *dest = str_to_sym(&str);
    str_clean(&str);
    result += r;
    r = result;
    goto clean;
  }
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    goto clean;
  if (c == ':' || character_is_uppercase(c)) {
    csize = r;
    buf_init(&tmp, false, sizeof(t), t);
    if (c == ':') {
      if ((r = buf_ignore(buf, csize)) < 0)
        goto clean;
      if ((r = buf_peek_character_utf8(buf, &c)) <= 0 ||
          sym_character_is_reserved(c))
        goto restore;
    }
    else {
      if ((r = buf_xfer(&tmp, buf, csize)) < 0)
        goto clean;
    }
    result += csize;
    while (1) {
      if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
        break;
      if (c == '[') {
        if ((r = buf_peek_next_character_utf8(buf, &c)) <= 0)
          break;
        if (c != ']')
          break;
        csize = r;
        if ((r = buf_xfer(&tmp, buf, csize)) < 0)
          goto restore;
        result += r;
        break;
      }
      else if (c == '.') {
        csize = r;
        if ((r = buf_peek_next_character_utf8(buf, &c)) <= 0)
          break;
        if (! character_is_uppercase(c))
          break;
        r += csize;
      }
      else if (sym_character_is_reserved(c))
        break;
      csize = r;
      if ((r = buf_xfer(&tmp, buf, csize)) < 0)
        goto restore;
      result += csize;
    }
    buf_read_to_str(&tmp, &str);
    *dest = str_to_sym(&str);
    str_clean(&str);
    r = result;
    goto clean;
  }
  if (r > 0)
    r = 0;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_sym_str (s_buf *buf, s_str *str)
{
  character c = 0;
  sw csize;
  sw r;
  sw result = 0;
  s_buf_save save;
  char t[SYM_MAX];
  s_buf tmp;
  buf_save_init(buf, &save);
  buf_init(&tmp, false, sizeof(t), t);
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         ! sym_character_is_reserved(c)) {
    csize = r;
    if ((r = buf_xfer(&tmp, buf, csize)) < 0)
      goto restore;
    result += csize;
  }
  buf_read_to_str(&tmp, str);
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag (s_buf *buf, s_tag *dest)
{
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_comments(buf)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    goto restore;
  switch (c) {
  case '{':
    if ((r = buf_parse_tag_pcall_op(buf, dest)) ||
        (r = buf_parse_tag_tuple(buf, dest)) ||
        (r = buf_parse_tag_do_block(buf, dest)))
      goto end;
    goto restore;
  default:
    if ((r = buf_parse_tag_pcall_op(buf, dest)) ||
        (r = buf_parse_tag_primary(buf, dest)))
      goto end;
  }
  goto restore;
 end:
  if (r < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_comments(buf)) > 0)
      result += r;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag_array (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_array(buf, &dest->data.array)) > 0)
    dest->type = TAG_ARRAY;
  return r;
}

sw buf_parse_tag_do_block (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_do_block(buf, &dest->data.do_block)) > 0)
    dest->type = TAG_DO_BLOCK;
  return r;
}

sw buf_parse_tag_bool (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_bool(buf, &dest->data.bool_)) > 0)
    dest->type = TAG_BOOL;
  return r;
}

sw buf_parse_tag_character (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_character(buf, &dest->data.character)) > 0)
    dest->type = TAG_CHARACTER;
  return r;
}

sw buf_parse_tag_complex (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcomplex(buf, &dest->data.pcomplex)) > 0)
    dest->type = TAG_PCOMPLEX;
  return r;
}

sw buf_parse_tag_cow (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcow(buf, &dest->data.pcow)) > 0)
    dest->type = TAG_PCOW;
  return r;
}

sw buf_parse_tag_f32 (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_f32(buf, &dest->data.f32)) > 0)
    dest->type = TAG_F32;
  return r;
}

sw buf_parse_tag_f64 (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_f64(buf, &dest->data.f64)) > 0)
    dest->type = TAG_F64;
  return r;
}

sw buf_parse_tag_f80 (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_f80(buf, &dest->data.f80)) > 0)
    dest->type = TAG_F80;
  return r;
}

#if HAVE_FLOAT128

sw buf_parse_tag_f128 (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_f128(buf, &dest->data.f128)) > 0)
    dest->type = TAG_F128;
  return r;
}

#endif

sw buf_parse_tag_ident (s_buf *buf, s_tag *dest)
{
  s_env *env;
  sw r;
  s_tag *tag;
  assert(buf);
  assert(dest);
  env = env_global();
  r = buf_parse_ident_cast(buf, &dest->data.ident);
  if (r > 0) {
    if (! dest->data.ident.module &&
        (tag = frame_get_w(env->read_time_frame,
                           dest->data.ident.sym)))
      tag_init_copy(dest, tag);
    else
      dest->type = TAG_IDENT;
  }
  return r;
}

sw buf_parse_tag_ident_sym (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_ident_sym(buf, &dest->data.psym)) > 0)
    dest->type = TAG_PSYM;
  return r;
}

sw buf_parse_tag_integer (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_integer(buf, &dest->data.integer)) > 0)
    dest->type = TAG_INTEGER;
  return r;
}

sw buf_parse_tag_map (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_map(buf, &dest->data.map)) > 0)
    dest->type = TAG_MAP;
  return r;
}

sw buf_parse_tag_match (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_match(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_module_name (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_module_name(buf, &dest->data.psym)) > 0)
    dest->type = TAG_PSYM;
  return r;
}

sw buf_parse_tag_number (s_buf *buf, s_tag *dest)
{
  void *data = NULL;
  s_tag i = {0};
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_tag tmp = {0};
  const s_sym *type = NULL;
  assert(buf);
  assert(dest);
#if HAVE_FLOAT128
  if ((r = buf_parse_tag_f128(buf, dest)) > 0)
    return r;
#endif
  if ((r = buf_parse_tag_f80(buf, dest)) > 0)
    return r;
  if ((r = buf_parse_tag_f64(buf, dest)) > 0)
    return r;
  if ((r = buf_parse_tag_f32(buf, dest)) > 0)
    return r;
  if ((r = buf_parse_tag_ratio(buf, dest)) > 0)
    return r;
  buf_save_init(buf, &save);
  if ((r = buf_parse_paren_sym(buf, &type)) > 0) {
    if (! sym_type_is_integer(type)) {
      r = 0;
      goto restore;
    }
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_tag_integer(buf, &i)) <= 0)
    goto restore;
  result += r;
  if (type) {
    sym_to_tag_type(type, &tmp.type);
    if (! tag_to_pointer(&tmp, type, &data)) {
      r = -1;
      goto restore;
    }
    if (! data_init_cast(data, &type, &i)) {
      r = -1;
      goto restore;
    }
    tag_clean(&i);
  }
  else {
    tag_integer_reduce(&i, &tmp);
    tag_clean(&i);
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  tag_clean(&i);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag_pcall (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_access (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_access(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_brackets (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_brackets(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_cast (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_cast(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_if (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_if(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_op (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_op(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_paren (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_paren(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcall_op_unary (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcall_op_unary(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_pcallable (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pcallable(buf, &dest->data.pcallable)) > 0)
    dest->type = TAG_PCALLABLE;
  return r;
}

sw buf_parse_tag_plist (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_plist(buf, &dest->data.plist)) > 0)
    dest->type = TAG_PLIST;
  return r;
}

sw buf_parse_tag_pointer (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pointer(buf, &dest->data.pointer)) > 0)
    dest->type = TAG_POINTER;
  return r;
}

sw buf_parse_tag_primary (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_comments(buf)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_tag_match(buf, dest)) ||
      (r = buf_parse_tag_special_operator(buf, dest)) ||
      (r = buf_parse_tag_pcall_brackets(buf, dest)) ||
      (r = buf_parse_tag_primary_2(buf, dest)))
    goto end;
  goto restore;
 end:
  if (r < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_comments(buf)) > 0)
      result += r;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag_primary_2 (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_comments(buf)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_tag_pcall_access(buf, dest)) != 0 ||
      (r = buf_parse_tag_primary_3(buf, dest)) != 0)
    goto end;
  goto restore;
 end:
  if (r < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_comments(buf)) > 0)
      result += r;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag_primary_3 (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_comments(buf)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_parse_tag_pcall_op_unary(buf, dest)) != 0 ||
      (r = buf_parse_tag_primary_4(buf, dest)) != 0)
    goto end;
  goto restore;
 end:
  if (r < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_comments(buf)) > 0)
      result += r;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag_primary_4 (s_buf *buf, s_tag *dest)
{
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_comments(buf)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
    goto restore;
  switch (c) {
  case '(':
    if ((r = buf_parse_tag_pvar(buf, dest)) ||
        (r = buf_parse_tag_cow(buf, dest)) ||
        (r = buf_parse_tag_number(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)) ||
        (r = buf_parse_tag_pointer(buf, dest)) ||
        (r = buf_parse_tag_ptr(buf, dest)) ||
        (r = buf_parse_tag_ptr_free(buf, dest)) ||
        (r = buf_parse_tag_array(buf, dest)) ||
        (r = buf_parse_tag_pcall_cast(buf, dest)) ||
        (r = buf_parse_tag_pcall_paren(buf, dest)))
      goto end;
    goto restore;
  case '?':
    if ((r = buf_parse_tag_pvar(buf, dest)))
      goto end;
    goto restore;
  case 'v':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_void(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    if ((r = buf_parse_tag_number(buf, dest)))
      goto end;
    goto restore;
  case 'c':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_cow(buf, dest)) ||
        (r = buf_parse_tag_pcallable(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case 'd':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_do_block(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case 'u':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_unquote(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case 'i':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_pcall_if(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case 'q':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_quote(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case 'f':
  case 'm':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_pcallable(buf, dest)) ||
        (r = buf_parse_tag_bool(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case 't':
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_bool(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)))
      goto end;
    goto restore;
  case '\'':
    if ((r = buf_parse_tag_character(buf, dest)))
      goto end;
    goto restore;
  case '%':
    if ((r = buf_parse_tag_map(buf, dest)) ||
        (r = buf_parse_tag_time(buf, dest)) ||
        (r = buf_parse_tag_pstruct(buf, dest)))
      goto end;
    goto restore;
  case '"':
    if ((r = buf_parse_tag_str(buf, dest)))
      goto end;
    goto restore;
  case '{':
    if ((r = buf_parse_tag_tuple(buf, dest)))
      goto end;
    goto restore;
  case '[':
    if ((r = buf_parse_tag_plist(buf, dest)))
      goto end;
    goto restore;
  case ':':
    if ((r = buf_parse_tag_sym(buf, dest)))
      goto end;
    goto restore;
  default:
    if ((r = buf_parse_tag_special_operator(buf, dest)) ||
        (r = buf_parse_tag_pcallable(buf, dest)) ||
        (r = buf_parse_tag_pcall(buf, dest)) ||
        (r = buf_parse_tag_ident(buf, dest)) ||
        (r = buf_parse_tag_sym(buf, dest)))
      goto end;
    goto restore;
  }
 end:
  if (r < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_comments(buf)) > 0)
      result += r;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tag_pstruct (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pstruct(buf, &dest->data.pstruct)) > 0)
    dest->type = TAG_PSTRUCT;
  return r;
}

sw buf_parse_tag_ptr (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_ptr(buf, &dest->data.ptr)) > 0)
    dest->type = TAG_PTR;
  return r;
}

sw buf_parse_tag_ptr_free (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_ptr_free(buf, &dest->data.ptr_free)) > 0)
    dest->type = TAG_PTR_FREE;
  return r;
}

sw buf_parse_tag_pvar (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_pvar(buf, &dest->data.pvar)) > 0)
    dest->type = TAG_PVAR;
  return r;
}

sw buf_parse_tag_quote (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_quote(buf, &dest->data.quote)) > 0)
    dest->type = TAG_QUOTE;
  return r;
}

sw buf_parse_tag_ratio (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_ratio(buf, &dest->data.ratio)) > 0)
    dest->type = TAG_RATIO;
  return r;
}

sw buf_parse_tag_special_operator (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_special_operator(buf, &dest->data.pcall)) > 0)
    dest->type = TAG_PCALL;
  return r;
}

sw buf_parse_tag_str (s_buf *buf, s_tag *dest)
{
  assert(buf);
  assert(dest);
  return buf_parse_str_eval(buf, dest);
}

sw buf_parse_tag_sym (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_sym(buf, &dest->data.psym)) > 0)
    dest->type = TAG_PSYM;
  return r;
}

sw buf_parse_tag_time (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_time(buf, &dest->data.time)) > 0)
    dest->type = TAG_TIME;
  return r;
}

sw buf_parse_tag_tuple (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_tuple(buf, &dest->data.tuple)) > 0)
    dest->type = TAG_TUPLE;
  return r;
}

sw buf_parse_tag_unquote (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_unquote(buf, &dest->data.unquote)) > 0)
    dest->type = TAG_UNQUOTE;
  return r;
}

sw buf_parse_tag_void (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_void(buf, NULL)) > 0)
    dest->type = TAG_VOID;
  return r;
}

sw buf_parse_time (s_buf *buf, s_time *dest)
{
  sw r;
  if ((r = buf_parse_time_as_sw(buf, dest)) > 0)
    return r;
  return buf_parse_time_as_tags(buf, dest);
}

sw buf_parse_time_as_sw (s_buf *buf, s_time *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_time tmp = {0};
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "%Time{")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "}")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    *dest = tmp;
    r = result;
    goto clean;
  }
  while (1) {
    if ((r = buf_read_1(buf, "tv_sec:")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_parse_sw(buf, &tmp.tv_sec)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, "}")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        *dest = tmp;
        r = result;
        goto clean;
      }
      if ((r = buf_read_1(buf, ",")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        if ((r = buf_parse_comments(buf)) < 0)
          goto restore;
        result += r;
        if ((r = buf_ignore_spaces(buf)) < 0)
          goto restore;
        result += r;
        continue;
      }
    }
    if ((r = buf_read_1(buf, "tv_nsec:")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_parse_sw(buf, &tmp.tv_nsec)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, "}")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        *dest = tmp;
        r = result;
        goto clean;
      }
      if ((r = buf_read_1(buf, ",")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        if ((r = buf_parse_comments(buf)) < 0)
          goto restore;
        result += r;
        if ((r = buf_ignore_spaces(buf)) < 0)
          goto restore;
        result += r;
        continue;
      }
    }
    break;
  }
  r = -1;
 restore:
  buf_save_restore_rpos(buf, &save);
  time_clean(&tmp);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_time_as_tags (s_buf *buf, s_time *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_time tmp = {0};
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "%Time{")) <= 0)
    goto clean;
  result += r;
  if (! time_allocate(&tmp))
    goto restore;
  tag_init_sw(tmp.tag, 0);
  tag_init_sw(tmp.tag + 1, 0);
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "}")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    *dest = tmp;
    r = result;
    goto clean;
  }
  while (1) {
    if ((r = buf_read_1(buf, "tv_sec:")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_parse_tag(buf, tmp.tag)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, "}")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        *dest = tmp;
        r = result;
        goto clean;
      }
      if ((r = buf_read_1(buf, ",")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        if ((r = buf_parse_comments(buf)) < 0)
          goto restore;
        result += r;
        if ((r = buf_ignore_spaces(buf)) < 0)
          goto restore;
        result += r;
      }
    }
    if ((r = buf_read_1(buf, "tv_nsec:")) < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_parse_tag(buf, tmp.tag + 1)) <= 0)
        goto restore;
      result += r;
      if ((r = buf_parse_comments(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_ignore_spaces(buf)) < 0)
        goto restore;
      result += r;
      if ((r = buf_read_1(buf, "}")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        *dest = tmp;
        r = result;
        goto clean;
      }
      if ((r = buf_read_1(buf, ",")) < 0)
        goto restore;
      if (r > 0) {
        result += r;
        if ((r = buf_parse_comments(buf)) < 0)
          goto restore;
        result += r;
        if ((r = buf_ignore_spaces(buf)) < 0)
          goto restore;
        result += r;
      }
    }
  }
  r = -1;
 restore:
  buf_save_restore_rpos(buf, &save);
  time_clean(&tmp);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_tuple (s_buf *buf, s_tuple *dest)
{
  s_list **i;
  s_list *list = 0;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tuple tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  result += r;
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
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "}")) < 0)
      goto restore;
    if (r > 0) {
      sw i;
      s_list *j;
      sw k;
      result += r;
      i = list_length(list);
      if (i < 2) {
	r = 0;
	goto restore;
      }
      tuple_init(&tmp, i);
      j = list;
      k = 0;
      while (i--) {
	tmp.tag[k] = j->tag;
	tag_init_void(&j->tag);
	j = list_next(j);
        k++;
      }
      *dest = tmp;
      r = result;
      goto clean;
    }
    if ((r = buf_read_1(buf, ",")) <= 0)
      goto restore;
    result += r;
    i = &(*i)->next.data.plist;
  }
  r = -1;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  if (list)
    list_delete_all(list);
  return r;
}

sw buf_parse_u64_dec (s_buf *buf, u64 *dest)
{
 sw r;
  sw result = 0;
  u8 digit;
  s_buf_save save;
  u64 tmp = 0;
  buf_save_init(buf, &save);
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp = tmp * 10 + digit;
    result += r;
  }
  if (r < 0) {
    buf_save_restore_rpos(buf, &save);
    goto clean;
  }
  *dest = tmp;
  r = result;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_u64_hex (s_buf *buf, u64 *dest)
{
 sw r;
  sw result = 0;
  u8 digit;
  s_buf_save save;
  u64 tmp = 0;
  buf_save_init(buf, &save);
  while ((r = buf_parse_digit_hex(buf, &digit)) > 0) {
    tmp = tmp * 16 + digit;
    result += r;
  }
  if (result > 0) {
    *dest = tmp;
    r = result;
  }
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_unquote (s_buf *buf, s_unquote *dest)
{
  s_unquote unquote;
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "unquote")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  unquote.tag = tag_new();
  if (! unquote.tag) {
    r = -2;
    goto restore;
  }
  if ((r = buf_parse_tag(buf, unquote.tag)) <= 0) {
    free(unquote.tag);
    goto restore;
  }
  result += r;
  *dest = unquote;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_void (s_buf *buf, void *dest)
{
  character c;
  sw r;
  s_buf_save save;
  assert(buf);
  (void) dest;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "void")) <= 0)
    goto clean;
  if (buf_peek_character_utf8(buf, &c) > 0 &&
      ! ident_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_peek_array_dimension_count (s_buf *buf, s_array *dest)
{
  sw r;
  s_buf_save save;
  buf_save_init(buf, &save);
  r = buf_parse_array_dimension_count(buf, dest);
  if (r > 0)
    buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return r;
}

sw buf_peek_array_dimensions (s_buf *buf, s_array *dest)
{
  sw r;
  s_buf_save save;
  buf_save_init(buf, &save);
  r = buf_parse_array_dimensions(buf, dest);
  if (r > 0)
    buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return r;
}

sw buf_peek_ident (s_buf *buf, s_ident *dest)
{
  sw r;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  r = buf_parse_ident(buf, dest);
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return r;
}
