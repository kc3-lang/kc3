/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../libtommath/tommath.h"
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "buf_save.h"
#include "call.h"
#include "cfn.h"
#include "character.h"
#include "env.h"
#include "fn.h"
#include "fn_clause.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "operator.h"
#include "str.h"
#include "struct.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"

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
  if (! (address = calloc(tmp.dimension, sizeof(sw)))) {
    err_puts("buf_parse_array_data: out of memory: address");
    return -1;
  }
  tmp.count = 1;
  i = 0;
  while (i < tmp.dimension) {
    tmp.count *= tmp.dimensions[i].count;
    i++;
  }
  tmp.size = tmp.dimensions[0].count * tmp.dimensions[0].item_size;
  if (! (tmp.tags = calloc(tmp.count, sizeof(s_tag)))) {
    free(address);
    err_puts("buf_parse_array_data: out of memory: tags");
    return -1;
  }
  tag = tmp.tags;
  if ((r = buf_parse_array_data_rec(buf, &tmp, address, &tag,
                                    0)) <= 0) {
    err_write_1("buf_parse_array_data: buf_parse_array_data_rec: ");
    err_inspect_sw(&r);
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
  if (! (tmp.dimensions = calloc(tmp.dimension,
                                 sizeof(s_array_dimension)))) {
    err_puts("tmp.dimensions: failed to allocate memory");
    return -1;
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
  address = calloc(tmp.dimension, sizeof(sw));
  tmp.dimensions[tmp.dimension - 1].item_size = size;
  if ((r = buf_parse_array_dimensions_rec(buf, &tmp, address,
                                          0)) < 0) {
    err_write_1("buf_parse_array_dimensions:"
                " buf_parse_array_dimensions_rec: ");
    err_inspect_sw(&r);
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
      if ((r = buf_parse_tag(buf, &tag)) < 0) {
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
  uw d;
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
  if ((r = buf_parse_tag_primary(buf, &tmp.arguments->tag)) <= 0)
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
    if ((r = buf_parse_uw(buf, &d)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "]")) <= 0)
      goto restore;
    result += r;
    *addr_last = list_new(NULL);
    tag_init_uw(&(*addr_last)->tag, d);
    addr_last = &(*addr_last)->next.data.list;
    address++;
  }
  if (! address) {
    goto restore;
  }
  arg_addr->type = TAG_ARRAY;
  if (! list_to_array(addr, &g_sym_Uw_brackets, &arg_addr->data.array))
    goto restore;
  ident_init(&tmp.ident, NULL, &g_sym__brackets);
  if (! operator_resolve(&tmp.ident, 2, &tmp.ident)) {
    err_puts("buf_parse_brackets: could not resolve operator []");
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  r = 0;
  buf_save_restore_rpos(buf, &save);
  call_clean(&tmp);
 clean:
  list_delete_all(addr);
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call (s_buf *buf, s_call *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_call tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  call_init(&tmp);
  if ((r = buf_parse_ident(buf, &tmp.ident)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_call_args_paren(buf, &tmp)) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
  call_clean(&tmp);
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
  s_ident next_op;
  s_call tmp;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  call_init_op(&tmp);
  if ((r = buf_parse_tag_primary(buf, &tmp.arguments->tag)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_ident_peek(buf, &next_op)) <= 0)
    goto restore;
  if (! operator_resolve(&next_op, 2, &next_op) ||
      operator_precedence(&next_op) < 0) {
    r = 0;
    goto restore;
  }
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
  s_ident next_op;
  s8 next_op_precedence;
  s_ident op;
  s8 op_precedence;
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
  if ((r = buf_parse_ident_peek(buf, &next_op)) <= 0)
    goto restore;
  if (! operator_resolve(&next_op, 2, &next_op) ||
      (op_precedence = operator_precedence(&next_op)) < 0) {
    r = 0;
    goto restore;
  }
  while (r > 0 && op_precedence >= min_precedence) {
    if ((r = buf_parse_ident(buf, &next_op)) <= 0)
      goto restore;
    result += r;
    if (! operator_resolve(&next_op, 2, &next_op))
      goto restore;
    op = next_op;
    tmp.ident = op;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_parse_tag_primary(buf, right)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
      break;
    result += r;
    if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
      break;
    if (r > 0 && c == '\n')
      break;
    r = buf_parse_ident_peek(buf, &next_op);
    if (r <= 0)
      break;
    if (! operator_resolve(&next_op, 2, &next_op) &&
        ! operator_resolve(&next_op, 1, &next_op))
      break;
    next_op_precedence = operator_precedence(&next_op);
    while (r > 0 && operator_arity(&next_op) == 2 &&
           (next_op_precedence > op_precedence ||
            (operator_is_right_associative(&next_op) &&
             next_op_precedence == op_precedence))) {
      call_init_op(&tmp2);
      tmp2.arguments->tag = *right;
      if ((r = buf_parse_call_op_rec(buf, &tmp2,
                                     (next_op_precedence > op_precedence) ?
                                     op_precedence + 1 :
                                     op_precedence)) <= 0) {
        tmp2.arguments->tag.type = TAG_VOID;
        call_clean(&tmp2);
        break;
      }
      result += r;
      tag_init_call(right);
      right->data.call = tmp2;
      if ((r = buf_ignore_spaces_but_newline(buf)) < 0)
        break;
      result += r;
      if ((r = buf_peek_character_utf8(buf, &c)) <= 0)
        break;
      if (r > 0 && c == '\n') {
        r = -1;
        break;
      }
      r = buf_parse_ident_peek(buf, &next_op);
      if (r > 0 &&
          (! operator_resolve(&next_op, 2, &next_op) ||
           (next_op_precedence = operator_precedence(&next_op)) < 0)) {
        r = 0;
        break;
      }
    }
    if (r <= 0)
      break;
    call_init_op(&tmp3);
    tmp3.ident = op;
    tmp3.arguments->tag = *left;
    list_next(tmp3.arguments)->tag = *right;
    tag_init_call(left);
    left->data.call = tmp3;
  }
  call_clean(dest);
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
  call_clean(&tmp);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_op_unary (s_buf *buf, s_call *dest)
{
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
  if (! operator_resolve(&tmp.ident, 1, &tmp.ident))
    goto restore;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_tag_primary(buf, &tmp.arguments->tag)) <= 0)
    goto restore;
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
  call_clean(&tmp);
  r = 0;
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_call_paren (s_buf *buf, s_call *dest)
{
  sw r;
  sw result = 0;
  s_call tmp;
  s_buf_save save;
  call_init_op_unary(&tmp);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto restore;
  result += r;
  ident_init_1(&tmp.ident, "()");
  if (! operator_resolve(&tmp.ident, 1, &tmp.ident)) {
    assert(! "buf_parse_call_paren: could not resolve operator ()");
    r = -1;
    goto restore;
  }
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
  call_init_op_unary(&tmp);
  ident_init(&tmp.ident, module, &g_sym_cast);
  if ((r = buf_parse_tag(buf, &tmp.arguments->tag)) <= 0)
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
  s_str name_str;
  const s_sym *name_sym;
  sw r;
  sw result = 0;
  const s_sym *result_type;
  s_buf_save save;
  s_cfn tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "cfn")) <= 0)
    goto clean;
  result += r;
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
  cfn_prep_cif(&tmp);
  cfn_link(&tmp);
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
  if ((r = buf_read_1(buf, "#")) <= 0)
    return r;
  result += r;
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         c != '\n') {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      return r;
    result += csize;
  }
  while (1) {
    buf_save_init(buf, &save);
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto clean;
    r1 = r;
    if ((r = buf_read_1(buf, "#")) <= 0)
      goto restore;
    result += r1 + r;
    while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
           c != '\n') {
      csize = r;
      if ((r = buf_ignore(buf, csize)) < 0)
        goto clean;
      result += csize;
    }
    if (r < 0)
      goto clean;
    buf_save_clean(buf, &save);
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
  sw r1 = 0;
  s_buf_save save;
  assert(buf);
  buf_save_init(buf, &save);
  while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
         character_is_space(c) &&
         c != '\n') {
    csize = r;
    if ((r = buf_ignore(buf, csize)) < 0)
      goto clean;
    r1 += csize;
  }
  if ((r = buf_parse_comment(buf)) <= 0)
    goto restore;
  r = r + r1;
  goto clean;
 restore:
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
      err_inspect_sw(&digit);
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
    tmp *= pow(10, exp_sign * exp);
  }
  if ((r = buf_read_1(buf, "f")) <= 0)
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
      if (r > 0) {
        result += r;
      }
      while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
        exp = exp * 10 + digit;
        result += r;
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

sw buf_parse_fact (s_buf *buf, s_fact_w *dest)
{
  s_tag *object = NULL;
  s_tag *predicate = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag *subject = NULL;
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
  if ((r = buf_parse_new_tag(buf, &subject)) <= 0)
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
  if ((r = buf_parse_new_tag(buf, &predicate)) <= 0)
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
  if ((r = buf_parse_new_tag(buf, &object)) <= 0)
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
  result += r;
  dest->subject = subject;
  dest->predicate = predicate;
  dest->object = object;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
  if (subject)
    tag_delete(subject);
  if (predicate)
    tag_delete(predicate);
  if (object)
    tag_delete(object);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fn (s_buf *buf, s_fn *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_fn tmp;
  s_fn_clause **tail;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "fn")) <= 0)
    goto clean;
  result += r;
  fn_init(&tmp);
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
 ok:
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
  s_buf_save save;
  s_fn_clause tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  fn_clause_init(&tmp, NULL);
  if ((r = buf_parse_fn_pattern(buf, &tmp.pattern)) <= 0) {
    err_puts("buf_parse_fn: invalid pattern");
    goto restore;
  }
  result += r;
  tmp.arity = list_length(tmp.pattern);
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_fn_algo(buf, &tmp.algo)) <= 0) {
    buf_inspect_fn_clause(&g_c3_env.err, &tmp);
    buf_flush(&g_c3_env.err);
    err_puts("buf_parse_fn: invalid program");
    goto restore;
  }
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  fn_clause_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fn_algo (s_buf *buf, s_list **dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_list **tail;
  s_tag tag;
  s_list *tmp = NULL;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  tail = &tmp;
  while (1) {
    if ((r = buf_parse_tag(buf, &tag)) < 0)
      goto restore;
    if (! r)
      break;
    result += r;
    *tail = list_new(NULL);
    (*tail)->tag = tag;
    tail = &(*tail)->next.data.list;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_read_1(buf, "}")) <= 0)
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

sw buf_parse_fn_pattern (s_buf *buf, s_list **dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_tag tag;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) < 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, ")")) < 0)
    goto restore;
  if (r) {
    result += r;
    goto ok;
  }
  while (1) {
    if ((r = buf_parse_tag(buf, &tag)) <= 0)
      goto restore;
    result += r;
    *tail = list_new(NULL);
    (*tail)->tag = tag;
    tail = &(*tail)->next.data.list;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, ",")) < 0)
      goto restore;
    result += r;
    if (! r) {
      if ((r = buf_read_1(buf, ")")) < 0)
        goto restore;
      if (r) {
        result += r;
        goto ok;
      }
    }
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
 ok:
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
  list_delete_all(tmp);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_ident (s_buf *buf, s_ident *dest)
{
  character c;
  sw csize;
  const s_sym *module = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  char t[IDENT_MAX];
  s_buf tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_module_name(buf, &module)) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_read_1(buf, ".")) <= 0)
      goto restore;
    result += r;
  }
  if ((r = buf_peek_1(buf, "_\"")) < 0)
    goto restore;
  if (r > 0) {
    if ((r = buf_read_1(buf, "_")) < 0)
      goto restore;
    result += r;
    if ((r = buf_parse_str(buf, &str)) < 0)
      goto restore;
    result += r;
    str_to_ident(&str, dest);
    dest->module = module;
    str_clean(&str);
    r = result;
    goto clean;
  }
  if ((r = buf_peek_character_utf8(buf, &c)) < 0)
    goto restore;
  if (r > 0 && ! ident_first_character_is_reserved(c)) {
    csize = r;
    buf_init(&tmp, false, sizeof(t), t);
    if ((r = buf_xfer(&tmp, buf, csize)) < 0)
      goto restore;
    result += csize;
    while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
           ! ident_character_is_reserved(c)) {
      csize = r;
      if ((r = buf_xfer(&tmp, buf, csize)) != csize)
        goto restore;
      result += csize;
    }
    buf_read_to_str(&tmp, &str);
    str_to_ident(&str, dest);
    dest->module = module;
    str_clean(&str);
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

sw buf_parse_ident_peek (s_buf *buf, s_ident *dest)
{
  sw r;
  s_buf_save save;
  buf_save_init(buf, &save);
  r = buf_parse_ident(buf, dest);
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return r;
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
  int result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto clean;
  result += r;
  integer_init_zero(dest);
  if (mp_add_d(&dest->mp_int, digit, &dest->mp_int) != MP_OKAY)
    goto error;
  while ((r = buf_read_1(buf, "_")) >= 0 &&
         (result += r,
          (r = buf_parse_digit_dec(buf, &digit)) > 0)) {
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
  buf_save_init(buf, &save);
  i = list;
  if ((r = buf_read_1(buf, "[")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_read_1(buf, "]")) < 0)
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
    if ((r = buf_parse_list_tag(buf, &(*i)->tag)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_parse_comments(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, "]")) < 0)
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
      if ((r = buf_read_1(buf, "]")) <= 0)
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

sw buf_parse_list_paren (s_buf *buf, s_list **list)
{
  s_list **i;
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  i = list;
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
  if ((r = buf_parse_sym_str(buf, &str)) < 0)
    goto clean;
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
    key.type = TAG_SYM;
    key.data.sym = str_to_sym(&str);
    str_clean(&str);
    tmp.data.tuple.tag[0] = key;
    tmp.data.tuple.tag[1] = value;
    goto ok;
  }
 tag:
  result = 0;
  buf_save_restore_rpos(buf, &save);
  if ((r = buf_parse_tag(buf, &tmp)) <= 0)
    goto clean;
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
    keys_end = &(*keys_end)->next.data.list;
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
    values_end = &(*values_end)->next.data.list;
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
      dest->type = TAG_SYM;
      dest->data.sym = str_to_sym(&str);
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
    dest->type = TAG_SYM;
    dest->data.sym = str_to_sym(&str);
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

sw buf_parse_module_name (s_buf *buf, const s_sym **dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  const s_sym *sym;
  s_buf tmp;
  buf_save_init(buf, &save);
  buf_init_alloc(&tmp, SYM_MAX);
  if ((r = buf_parse_sym(buf, &sym)) <= 0)
    goto clean;
  if (! sym_is_module(sym)) {
    r = 0;
    goto restore;
  }
  result += r;
  if ((r = buf_inspect_sym(&tmp, &sym)) < 0)
    goto clean;
  save.rpos = buf->rpos;
  while ((r = buf_read_1(buf, ".")) > 0 &&
         (r = buf_parse_sym(buf, &sym)) > 0 &&
         sym_is_module(sym)) {
    result += r + 1;
    save.rpos = buf->rpos;
    if ((r = buf_write_1(&tmp, ".")) < 0 ||
        (r = buf_inspect_sym(&tmp, &sym)) < 0)
      goto clean;
  }
  buf_save_restore_rpos(buf, &save);
  buf_read_to_str(&tmp, &str);
  *dest = str_to_sym(&str);
  str_clean(&str);
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  buf_clean(&tmp);
  return r;
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
  sw r;
  sw result = 0;
  s_buf_save save;
  const s_sym *tmp;
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
  (void) buf;
  (void) dest;
  assert(! "buf_parse_ptr: not implemented");
  err_puts("buf_parse_ptr: not implemented");
  return -1;
}

sw buf_parse_ptr_free (s_buf *buf, u_ptr_w *dest)
{
  (void) buf;
  (void) dest;
  assert(! "buf_parse_ptr_free: not implemented");
  err_puts("buf_parse_ptr_free: not implemented");
  return -1;
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
  if ((r = buf_parse_tag(buf, quote.tag)) <= 0)
    goto restore;
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
  character c;
  uw end;
  bool eval = false;
  s_str in;
  s_buf in_buf = {0};
  s_str out;
  sw r;
  sw result;
  s_buf_save save;
  uw start;
  buf_save_init(buf, &save);
  r = buf_parse_str(buf, &in);
  if (r <= 0)
    goto clean;
  result = r;
  buf_init_str(&in_buf, false, &in);
  start = 0;
  while (1) {
    if (! eval) {
      end = in_buf.rpos;
      r = buf_read_1(&in_buf, "#{");
      if (r < 0)
        goto restore;
      if (r > 0) {
        buf_slice_to_str(&in_buf, start, end, &out);
        eval = true;
      }
    }
    r = buf_read_character_utf8(&in_buf, &c);
    
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
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
  read_u8:
    buf_save_restore_rpos(buf, &save);
  }
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
      err_write_1("buf_parse_struct: struct_init ");
      err_inspect_sym(&module);
      err_puts(".");
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
    keys_end = &(*keys_end)->next.data.list;
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
    values_end = &(*values_end)->next.data.list;
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
    err_inspect_sym(&module);
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
  if ((r = buf_parse_tag_call_op(buf, dest)) != 0 ||
      (r = buf_parse_tag_brackets(buf, dest)) != 0 ||
      (r = buf_parse_tag_primary(buf, dest)) != 0)
    goto end;
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

sw buf_parse_tag_bool (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_bool(buf, &dest->data.bool)) > 0)
    dest->type = TAG_BOOL;
  return r;
}

sw buf_parse_tag_brackets (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_brackets(buf, &dest->data.call)) > 0)
    dest->type = TAG_CALL;
  return r;
}

sw buf_parse_tag_call (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_call(buf, &dest->data.call)) > 0)
    dest->type = TAG_CALL;
  return r;
}

sw buf_parse_tag_call_op (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_call_op(buf, &dest->data.call)) > 0)
    dest->type = TAG_CALL;
  return r;
}

sw buf_parse_tag_call_paren (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_call_paren(buf, &dest->data.call)) > 0)
    dest->type = TAG_CALL;
  return r;
}

sw buf_parse_tag_call_op_unary (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_call_op_unary(buf, &dest->data.call)) > 0)
    dest->type = TAG_CALL;
  return r;
}

sw buf_parse_tag_cast (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_cast(buf, &dest->data.call)) > 0)
    dest->type = TAG_CALL;
  return r;
}

sw buf_parse_tag_cfn (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_cfn(buf, &dest->data.cfn)) > 0)
    dest->type = TAG_CFN;
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

sw buf_parse_tag_fn (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_fn(buf, &dest->data.fn)) > 0)
    dest->type = TAG_FN;
  return r;
}

sw buf_parse_tag_ident (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_ident(buf, &dest->data.ident)) > 0)
    dest->type = TAG_IDENT;
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

sw buf_parse_tag_list (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_list(buf, &dest->data.list)) > 0)
    dest->type = TAG_LIST;
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

sw buf_parse_tag_module_name (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_module_name(buf, &dest->data.sym)) > 0)
    dest->type = TAG_SYM;
  return r;
}

sw buf_parse_tag_number (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  r = buf_parse_tag_f32(buf, dest);
  if (r > 0)
    return r;
  r = buf_parse_tag_f64(buf, dest);
  if (r > 0)
    return r;
  r = buf_parse_tag_integer(buf, dest);
  if (r > 0) {
    tag_integer_reduce(dest);
    return r;
  }
  return 0;
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
  if ((r = buf_parse_tag_void(buf, dest)) != 0 ||
      (r = buf_parse_tag_number(buf, dest)) != 0 ||
      (r = buf_parse_tag_array(buf, dest)) != 0 ||
      (r = buf_parse_tag_cast(buf, dest)) != 0 ||
      (r = buf_parse_tag_call(buf, dest)) != 0 ||
      (r = buf_parse_tag_call_paren(buf, dest)) != 0 ||
      (r = buf_parse_tag_call_op_unary(buf, dest)) != 0 ||
      (r = buf_parse_tag_bool(buf, dest)) != 0 ||
      (r = buf_parse_tag_character(buf, dest)) != 0 ||
      (r = buf_parse_tag_map(buf, dest)) != 0 ||
      (r = buf_parse_tag_str(buf, dest)) != 0 ||
      (r = buf_parse_tag_tuple(buf, dest)) != 0 ||
      (r = buf_parse_tag_quote(buf, dest)) != 0 ||
      (r = buf_parse_tag_cfn(buf, dest)) != 0 ||
      (r = buf_parse_tag_fn(buf, dest)) != 0 ||
      (r = buf_parse_tag_struct(buf, dest)) != 0 ||
      (r = buf_parse_tag_ident(buf, dest)) != 0 ||
      (r = buf_parse_tag_list(buf, dest)) != 0 ||
      (r = buf_parse_tag_sym(buf, dest)) != 0)
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

sw buf_parse_tag_quote (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_quote(buf, &dest->data.quote)) > 0)
    dest->type = TAG_QUOTE;
  return r;
}

sw buf_parse_tag_str (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_str(buf, &dest->data.str)) > 0)
    dest->type = TAG_STR;
  return r;
}

sw buf_parse_tag_struct (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_struct(buf, &dest->data.struct_)) > 0)
    dest->type = TAG_STRUCT;
  return r;
}

sw buf_parse_tag_sym (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_sym(buf, &dest->data.sym)) > 0)
    dest->type = TAG_SYM;
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

sw buf_parse_tag_void (s_buf *buf, s_tag *dest)
{
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_parse_void(buf, NULL)) > 0)
    dest->type = TAG_VOID;
  return r;
}

sw buf_parse_tuple (s_buf *buf, s_tuple *tuple)
{
  s_list **i;
  s_list *list = 0;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(tuple);
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
      tuple_init(tuple, i);
      j = list;
      k = 0;
      while (i--) {
	tuple->tag[k] = j->tag;
	tag_init_void(&j->tag);
	j = list_next(j);
        k++;
      }
      r = result;
      goto clean;
    }
    if ((r = buf_read_1(buf, ",")) <= 0)
      goto restore;
    result += r;
    i = &(*i)->next.data.list;
  }
  result += r;
  if ((r = buf_read_1(buf, "}")) <= 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
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

sw buf_parse_var (s_buf *buf, void *dest)
{
  character c;
  sw r;
  s_buf_save save;
  assert(buf);
  (void) dest;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "?")) <= 0)
    goto clean;
  if (buf_peek_character_utf8(buf, &c) > 0 &&
      ! ident_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
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
