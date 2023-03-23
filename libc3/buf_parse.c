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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../libtommath/tommath.h"
#include "c3.h"

sw buf_parse_cfn_arg_types (s_buf *buf, s_list **dest);

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
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
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
  s_call tmp2;
  s_call tmp3;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  call_init_op(&tmp);
  bzero(&tmp2, sizeof(s_call));
  bzero(&tmp3, sizeof(s_call));
  left = &tmp.arguments->tag;
  right = &list_next(tmp.arguments)->tag;
  tag_copy(&dest->arguments->tag, left);
  if ((r = buf_parse_ident_peek(buf, &next_op)) <= 0)
    goto restore;
  if ((op_precedence = operator_precedence(&next_op)) < 0) {
    r = 0;
    goto restore;
  }
  while (r > 0 && op_precedence >= min_precedence) {
    if ((r = buf_parse_ident(buf, &next_op)) <= 0)
      goto restore;
    result += r;
    op = next_op;
    tmp.ident = op;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_parse_tag_primary(buf, right)) <= 0)
      goto restore;
    result += r;
    if ((r = buf_ignore_newline(buf)) < 0)
      break;
    if (r > 0) {
      result += r;
      break;
    }
    if ((r = buf_ignore_spaces(buf)) < 0)
      break;
    result += r;
    r = buf_parse_ident_peek(buf, &next_op);
    if (r <= 0)
      break;
    next_op_precedence = operator_precedence(&next_op);
    while (r > 0 && (next_op_precedence >= op_precedence ||
                     (operator_is_right_associative(&next_op) &&
                      next_op_precedence == op_precedence))) {
      call_init_op(&tmp2);
      tmp2.arguments->tag = *right;
      if ((r = buf_parse_call_op_rec(buf, &tmp2, (next_op_precedence > op_precedence) ? op_precedence + 1 : op_precedence)) <= 0) {
        tmp2.arguments->tag.type.type = TAG_VOID;
        call_clean(&tmp2);
        break;
      }
      result += r;
      tag_init_call(right, &tmp2);
      if ((r = buf_ignore_spaces(buf)) < 0)
        break;
      result += r;
      r = buf_parse_ident_peek(buf, &next_op);
      if (r > 0)
        next_op_precedence = operator_precedence(&next_op);
    }
    if (r <= 0 || (op_precedence = next_op_precedence) < min_precedence)
      break;
    call_init_op(&tmp3);
    tmp3.ident = op;
    tmp3.arguments->tag = *left;
    list_next(tmp3.arguments)->tag = *right;
    tag_init_call(left, &tmp3);
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

sw buf_parse_cfn (s_buf *buf, s_cfn *dest)
{
  s_list *arg_types = NULL;
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
  if ((r = buf_parse_str(buf, &tmp.name)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_cfn_arg_types(buf, &arg_types)) <= 0)
    goto restore;
  result += r;
  tmp.arity = list_length(arg_types);
  cfn_set_type(&tmp, arg_types, result_type);
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

sw buf_parse_cfn_arg_types (s_buf *buf, s_list **dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_list **tail;
  s_list *tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  tail = &tmp;
  while (1) {
    *tail = list_new(NULL);
    if ((r = buf_parse_tag_sym(buf, &(*tail)->tag)) <= 0)
      goto restore;
    result += r;
    tail = &(*tail)->next.data.list;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_1(buf, ",")) < 0)
      goto restore;
    if (! r)
      break;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_read_1(buf, ")")) <= 0)
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
      return r;
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
  tag_delete(subject);
  tag_delete(predicate);
  tag_delete(object);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fn (s_buf *buf, s_fn **dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_fn *tmp = NULL;
  s_fn **tail = &tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "fn")) <= 0)
    goto clean;
  result += r;
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
      *tail = fn_new(NULL);
      if ((r = buf_parse_fn_clause(buf, *tail)) <= 0)
        goto restore;
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
    tmp = fn_new(NULL);
    if ((r = buf_parse_fn_clause(buf, tmp)) <= 0)
      goto restore;
    result += r;
  }
 ok:
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  fn_delete_all(tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_fn_clause (s_buf *buf, s_fn *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  s_fn tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  fn_init(&tmp, NULL);
  if ((r = buf_parse_fn_pattern(buf, &tmp.pattern)) <= 0) {
    warnx("buf_parse_fn: invalid pattern");
    goto restore;
  }
  result += r;
  tmp.arity = list_length(tmp.pattern);
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  if ((r = buf_parse_fn_algo(buf, &tmp.algo)) <= 0) {
    buf_inspect_fn(&g_c3_env.err, &tmp);
    buf_flush(&g_c3_env.err);
    warnx("buf_parse_fn: invalid program");
    goto restore;
  }
  result += r;
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
    if ((r = buf_read_1(buf, ";")) < 0)
      goto restore;
    if (! r)
      break;
    result += r;
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
      result += r;
      break;
    }
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
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
  const s_sym *module_name = NULL;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  s8 t[IDENT_MAX];
  s_buf tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_parse_module_name(buf, &module_name)) < 0)
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
    dest->module_name = module_name;
    str_clean(&str);
    r = result;
    goto clean;
  }
  r = 0;
  goto clean;
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
    if ((r = buf_parse_integer_unsigned_bin(buf, dest)) <= 0)
      goto restore;
    result += r;
    if (negative)
      integer_neg(dest, dest);
    r = result;
    goto clean;
  }
  if ((r = buf_read_1(buf, "0o")) < 0)
    goto clean;
  if (r == 0 && (r = buf_read_1(buf, "0O")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_integer_unsigned_oct(buf, dest)) <= 0)
      goto restore;
    result += r;
    if (negative)
      integer_neg(dest, dest);
    r = result;
    goto clean;
  }
  if ((r = buf_read_1(buf, "0x")) < 0)
    goto clean;
  if (r == 0 && (r = buf_read_1(buf, "0X")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_integer_unsigned_hex(buf, dest)) <= 0)
      goto restore;
    result += r;
    if (negative)
      integer_neg(dest, dest);
    r = result;
    goto clean;
  }
  if ((r = buf_parse_integer_unsigned_dec(buf, dest)) <= 0) {
    r = 0;
    goto restore;
  }
  result += r;
  if (negative)
    integer_neg(dest, dest);
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
    if ((r = buf_parse_tag(buf, &(*i)->tag)) <= 0)
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
  result += r;
  if ((r = buf_inspect_sym(&tmp, sym)) < 0)
    goto clean;
  save.rpos = buf->rpos;
  while ((r = buf_read_1(buf, ".")) > 0 &&
         (r = buf_parse_sym(buf, &sym)) > 0) {
    result += r + 1;
    save.rpos = buf->rpos;
    if ((r = buf_write_1(&tmp, ".")) < 0 ||
        (r = buf_inspect_sym(&tmp, sym)) < 0)
      goto clean;
  }
  if ((r = buf_peek_1(buf, ".")) <= 0)
    buf_save_restore_rpos(buf, &save);
  buf_read_to_str(&tmp, &str);
  *dest = str_to_sym(&str);
  str_clean(&str);
  r = result;
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
  s8 *end = "\"";
  s8 *end1 = NULL;
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

sw buf_parse_sym (s_buf *buf, const s_sym **dest)
{
  character c;
  sw csize;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  s8 t[SYM_MAX];
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
    while ((r = buf_peek_character_utf8(buf, &c)) > 0 &&
           ! sym_character_is_reserved(c)) {
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

sw buf_parse_tag_bool (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_bool(buf, &dest->data.bool)) > 0)
    dest->type.type = TAG_BOOL;
  return r;
}

sw buf_parse_tag_call (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_call(buf, &dest->data.call)) > 0)
    dest->type.type = TAG_CALL;
  return r;
}

sw buf_parse_tag_call_op (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_call_op(buf, &dest->data.call)) > 0)
    dest->type.type = TAG_CALL;
  return r;
}

sw buf_parse_tag_cfn (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_cfn(buf, &dest->data.cfn)) > 0)
    dest->type.type = TAG_CFN;
  return r;
}

sw buf_parse_tag_character (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_character(buf, &dest->data.character)) > 0)
    dest->type.type = TAG_CHARACTER;
  return r;
}

sw buf_parse_tag_fn (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_fn(buf, &dest->data.fn)) > 0)
    dest->type.type = TAG_FN;
  return r;
}

sw buf_parse_tag_ident (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_ident(buf, &dest->data.ident)) > 0)
    dest->type.type = TAG_IDENT;
  return r;
}

sw buf_parse_tag_integer (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_integer(buf, &dest->data.integer)) > 0)
    dest->type.type = TAG_INTEGER;
  return r;
}

sw buf_parse_tag_list (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_list(buf, &dest->data.list)) > 0)
    dest->type.type = TAG_LIST;
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
  if ((r = buf_parse_tag_bool(buf, dest)) != 0 ||
      (r = buf_parse_tag_character(buf, dest)) != 0)
    goto end;
  if ((r = buf_parse_tag_integer(buf, dest)) != 0) {
    tag_integer_reduce(dest);
    goto end;
  }
  if ((r = buf_parse_tag_list(buf, dest)) != 0 ||
      (r = buf_parse_tag_str(buf, dest)) != 0 ||
      (r = buf_parse_tag_tuple(buf, dest)) != 0 ||
      (r = buf_parse_tag_quote(buf, dest)) != 0 ||
      (r = buf_parse_tag_cfn(buf, dest)) != 0 ||
      (r = buf_parse_tag_fn(buf, dest)) != 0 ||
      (r = buf_parse_tag_call(buf, dest)) != 0 ||
      (r = buf_parse_tag_ident(buf, dest)) != 0 ||
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
  if ((r = buf_parse_quote(buf, &dest->data.quote)) > 0)
    dest->type.type = TAG_QUOTE;
  return r;
}

sw buf_parse_tag_str (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_str(buf, &dest->data.str)) > 0)
    dest->type.type = TAG_STR;
  return r;
}

sw buf_parse_tag_sym (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_sym(buf, &dest->data.sym)) > 0)
    dest->type.type = TAG_SYM;
  return r;
}

sw buf_parse_tag_tuple (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_tuple(buf, &dest->data.tuple)) > 0)
    dest->type.type = TAG_TUPLE;
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

sw buf_parse_u32 (s_buf *buf, u32 *dest)
{
  sw r;
  sw result = 0;
  u8 digit;
  s_buf_save save;
  u32 tmp = 0;
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

sw buf_parse_u8 (s_buf *buf, u8 *dest)
{
  sw r;
  sw result = 0;
  u8 digit;
  s_buf_save save;
  u8 tmp = 0;
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

sw buf_parse_f32 (s_buf *buf, f32 *dest)
{
  sw r;
  sw result = 0;
  u8 digit;
  u8 exponent = 0;
  s_buf_save save;
  f32 tmp = 0;
  f32 frac = 0.1;
  f32 exp = 0;
  f32 exp_sign = 1;
  buf_save_init(buf, &save);
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp = tmp * 10 + digit;
    result += r;
  }
  if (r < 0) {
    buf_save_restore_rpos(buf, &save);
    goto clean;
  }
  if ((r = buf_peek_1(buf, ".")) > 0) {
    result += r;
    if ((r = buf_ignore(buf, 1)) < 0)
      goto restore;
    while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
      tmp += frac * digit;
      frac /= 10;
      result += r;
    }
  }
  if ((r = buf_peek_1(buf, "e")) > 0) {
    exponent = 1;
    result += r;
    if ((r = buf_ignore(buf, 1)) < 0)
      goto restore;
    if ((r = buf_peek_1(buf, "-")) > 0) {
      exp_sign = -1;
      result += r;
      if ((r = buf_ignore(buf, 1)) < 0)
        goto restore;
    }
    else if ((r = buf_peek_1(buf, "+")) > 0) {
      result += r;
      if ((r = buf_ignore(buf, 1)) < 0)
        goto restore;
    }
    while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
      exp = exp * 10 + digit;
      result += r;
    }
  }
  if (exponent) {
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

sw buf_parse_f64 (s_buf *buf, f64 *dest) {
  sw r;
  sw result = 0;
  u8 digit;
  u8 exponent = 0;
  s_buf_save save;
  f64 tmp = 0;
  f64 frac = 0.1;
  f64 exp = 0;
  f64 exp_sign = 1;
  buf_save_init(buf, &save);
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp = tmp * 10 + digit;
    result += r;
  }
  if (r < 0) {
    buf_save_restore_rpos(buf, &save);
    goto clean;
  }
  if ((r = buf_peek_1(buf, ".")) > 0) {
    result += r;
    if ((r = buf_ignore(buf, 1)) < 0)
      goto restore;
    while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
      tmp += frac * digit;
      frac /= 10;
      result += r;
    }
  }
  if ((r = buf_peek_1(buf, "e")) > 0) {
    exponent = 1;
    result += r;
    if ((r = buf_ignore(buf, 1)) < 0)
      goto restore;
    if ((r = buf_peek_1(buf, "-")) > 0) {
      exp_sign = -1;
      result += r;
      if ((r = buf_ignore(buf, 1)) < 0)
        goto restore;
    } else if ((r = buf_peek_1(buf, "+")) > 0) {
      result += r;
      if ((r = buf_ignore(buf, 1)) < 0)
        goto restore;
    }
    while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
      exp = exp * 10 + digit;
      result += r;
    }
  }
  if (exponent) {
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

