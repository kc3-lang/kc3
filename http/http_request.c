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
#include <string.h>
#include <libkc3/kc3.h>
#include "http.h"
#include "http_request.h"
#include "url.h"

s_tag * http_request_buf_parse (s_tag *req, s_buf *buf)
{
  bool b;
  s_tag  body;
  s_str *body_str;
  s_str boundary;
  const s_str content_length_str = {{NULL}, 14, {"Content-Length"}};
  uw          content_length_uw = 0;
  s_str      *content_type = NULL;
  const s_str content_type_str = {{NULL}, 12, {"Content-Type"}};
  const s_str cookie_str = {{NULL}, 6, {"Cookie"}};
  s_str *key;
  s_str line;
  s_tag method_key = {0};
  s_tag method_value = {0};
  static const s_str multipart_form_data =
    {{NULL}, 30, {"multipart/form-data; boundary="}};
  s_buf_save save;
  s_list **tail;
  s_tag tmp = {0};
  s_http_request tmp_req = {0};
  s_str       url;
  static const s_str urlencoded =
    {{NULL}, 33, {"application/x-www-form-urlencoded"}};
  s_str *value;
  assert(req);
  assert(buf);
  if (method_key.type == TAG_VOID)
    tag_init_str_1(&method_key, NULL, "_method");
  buf_save_init(buf, &save);
  if (! http_request_buf_parse_method(buf, &tmp_req.method))
    goto restore;
  if (false) {
    err_write_1("http_request_buf_parse: method: ");
    err_inspect_tag(&tmp_req.method);
    err_write_1("\n");
  }
  if (! buf_read_until_1_into_str(buf, " ", &url)) {
    err_puts("http_request_buf_parse: invalid URL");
    goto restore;
  }
  if (false) {
    err_write_1("http_request_buf_parse: url: ");
    err_inspect_str(&url);
    err_write_1("\n");
  }
  url_unescape(&url, &tmp_req.url);
  str_clean(&url);
  if (false) {
    err_write_1("http_request_buf_parse: url: ");
    err_inspect_str(&tmp_req.url);
    err_write_1("\n");
  }
  if (! buf_read_until_1_into_str(buf, "\r\n", &tmp_req.protocol)) {
    err_puts("http_request_buf_parse: invalid protocol");
    goto restore;
  }
  if (false) {
    err_write_1("http_request_buf_parse: protocol: ");
    err_inspect_str(&tmp_req.protocol);
    err_write_1("\n");
  }
  tail = &tmp_req.headers;
  while (1) {
    if (! buf_read_until_1_into_str(buf, "\r\n", &line)) {
      err_puts("http_request_buf_parse: invalid header");
      goto restore;
    }
    if (line.size == 0)
      break;
    *tail = list_new(NULL);
    (*tail)->tag.type = TAG_STR;
    if (! http_header_split(&line, &(*tail)->tag))
      goto restore;
    key   = &(*tail)->tag.data.tuple.tag[0].data.str;
    value = &(*tail)->tag.data.tuple.tag[1].data.str;
    if (! compare_str_case_insensitive(&content_length_str, key)) {
      if (! uw_init_str(&content_length_uw, value))
        goto restore;
      tag_clean((*tail)->tag.data.tuple.tag + 1);
      tag_init_uw((*tail)->tag.data.tuple.tag + 1, content_length_uw);
    }
    else if (! compare_str_case_insensitive(&content_type_str, key))
      content_type = value;
    else if (! compare_str_case_insensitive(&cookie_str, key))
      http_request_cookie_add(&tmp_req, value);
    tail = &(*tail)->next.data.list;
  }
    if (true) {
      err_write_1("http_request_buf_parse: content_type ");
      err_inspect_str(content_type);
      err_write_1("\n");
    }
  if (content_type &&
      str_starts_with_case_insensitive(content_type,
                                       &multipart_form_data,
                                       &b) && b) {
    if (! str_init_slice(&boundary, content_type,
                         multipart_form_data.size, -1))
      goto restore;
    if (true) {
      err_write_1("http_request_buf_parse: boundary ");
      err_inspect_str(&boundary);
      err_write_1("\n");
    }
  }
  else if (content_type &&
           content_length_uw) {
      tmp_req.body.type = TAG_STR;
      body_str = &tmp_req.body.data.str;
    if (! buf_read(buf, content_length_uw, body_str))
      goto restore;
    if (false) {
      err_inspect_str(body_str);
      err_write_1("\n");
    }
    if (! compare_str_case_insensitive(content_type, &urlencoded)) {
      if (! url_www_form_decode(body_str, &body))
        goto restore;
      if (false) {
        err_write_1("http_request_buf_parse: body: ");
        err_inspect_tag(&body);
        err_write_1("\n");
      }
      if (alist_get(body.data.list,
                    &method_key, &method_value)) {
        http_request_method_from_str(&method_value.data.str,
                                     &tmp_req.method);
        tag_clean(&method_value);
      }
      str_clean(body_str);
      tmp_req.body = body;
    }
  }
  if (! tag_init_struct(&tmp, sym_1("HTTP.Request")))
    goto restore;
  if (! struct_allocate(&tmp.data.struct_)) {
    tag_void(&tmp);
    goto restore;
  }
  *((s_http_request *) tmp.data.struct_.data) = tmp_req;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  *req = tmp;
  return req;
}

s_tag * http_request_buf_parse_method (s_buf *buf, s_tag *dest)
{
  s_buf_save save;
  s_str str;
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if (! buf_read_until_1_into_str(buf, " ", &str)) {
    if (false)
      err_puts("http_request_buf_parse_method: buf_read_until_1_into_str");
    goto restore;
  }
  if (! http_request_method_from_str(&str, &tmp))
    goto restore;
  str_clean(&str);
  buf_save_clean(buf, &save);
  *dest = tmp;
  return dest;
 restore:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return NULL;
}

sw http_request_buf_write (s_http_request *req, s_buf *buf)
{
  s_tag *key;
  s_list *list;
  const s_str *method;
  sw r;
  sw result = 0;
  s_str str;
  const s_sym *type = &g_sym_Str;
  s_tag *value;
  assert(req);
  assert(buf);
  switch (req->method.type) {
  case TAG_SYM:
    method = &req->method.data.sym->str;
    break;
  case TAG_STR:
    method = &req->method.data.str;
    break;
  default:
    err_puts("http_request_buf_write: invalid method");
    assert(! "http_request_buf_write: invalid method");
    return -1;
  }
  if (! method->size) {
    err_puts("http_request_buf_write: invalid method: \"\"");
    return -1;
  }    
  if (! req->url.size) {
    err_puts("http_request_buf_write: invalid url: \"\"");
    return -1;
  }    
  if (! req->protocol.size) {
    err_puts("http_request_buf_write: invalid protocol: \"\"");
    return -1;
  }    
  if (! list_is_alist(req->headers)) {
    err_puts("http_request_buf_write: invalid headers: not an AList");
    return -1;
  }
  if ((r = buf_write_str(buf, method)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) <= 0)
    return r;
  result += r;
  if ((r = buf_write_str(buf, &req->url)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) <= 0)
    return r;
  result += r;
  if ((r = buf_write_str(buf, &req->protocol)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\r\n")) < 0)
    return r;
  result += r;
  list = req->headers;
  while (list) {
    key = list->tag.data.tuple.tag;
    value = list->tag.data.tuple.tag + 1;
    switch (key->type) {
    case TAG_STR:
      if ((r = buf_write_str(buf, &key->data.str)) <= 0)
        return r;
      result += r;
      break;
    case TAG_SYM:
      if ((r = buf_write_str(buf, &key->data.sym->str)) <= 0)
        return r;
      result += r;
      break;
    default:
      err_write_1("http_request_buf_write: invalid header key: ");
      err_inspect_tag(key);
      return -1;
    }
    if ((r = buf_write_1(buf, ": ")) <= 0)
      return r;
    result += r;
    if (! str_init_cast(&str, &type, value)) {
      err_write_1("http_request_buf_write: invalid header value: ");
      err_inspect_tag(value);
      return -1;
    }
    if ((r = buf_write_str(buf, &str)) <= 0) {
      str_clean(&str);
      return r;
    }
    result += r;
    str_clean(&str);
    if ((r = buf_write_1(buf, "\r\n")) <= 0)
      return r;
    result += r;
    list = list_next(list);
  }
  if ((r = buf_write_1(buf, "\r\n")) <= 0)
    return r;
  result += r;
  return result;
}

s_http_request * http_request_cookie_add (s_http_request *req,
                                          s_str *cookies)
{
  sw eq_pos;
  s_list *s;
  const s_str  separator = {{NULL}, 2, {"; "}};
  s_list *split;
  s_list **tail;
  s_tuple *tuple;
  tail = list_tail(&req->cookies);
  if (! str_split(cookies, &separator, &split))
    return NULL;
  s = split;
  while (s) {
    if (s->tag.type != TAG_STR) {
      err_puts("http_request_cookie_add: not a Str");
      assert(! "http_request_cookie_add: not a Str");
      goto ko;
    }
    if ((eq_pos = str_position_1(&s->tag.data.str, "=")) < 0)
      goto next;
    if (! (*tail = list_new_tuple(2, NULL))) {
      err_puts("http_request_cookie_add: list_new_tuple");
      assert(! "http_request_cookie_add: list_new_tuple");
      goto ko;
    }
    tuple = &(*tail)->tag.data.tuple;
    tuple->tag[0].type = TAG_STR;
    tuple->tag[1].type = TAG_STR;
    if (! str_init_slice(&tuple->tag[0].data.str, &s->tag.data.str,
                         0, eq_pos)) {
      err_puts("http_request_cookie_add: str_init_slice 1");
      assert(! "http_request_cookie_add: str_init_slice 1");
      goto ko;
    }
    if (! str_init_slice(&tuple->tag[1].data.str, &s->tag.data.str,
                         eq_pos + 1, -1)) {
      err_puts("http_request_cookie_add: str_init_slice 2");
      assert(! "http_request_cookie_add: str_init_slice 2");
      goto ko;
    }
  next:
    s = list_next(s);
  }
  list_delete_all(split);
  return req;
 ko:
  list_delete_all(split);
  return NULL;
}

s_tag * http_request_method_from_str (const s_str *str, s_tag *dest)
{
  const s_list *allowed_methods;
  s_tag allowed_methods_tag;
  bool b;
  s_ident ident;
  s_tag tmp = {0};
  assert(str);
  assert(dest);
  ident_init(&ident, sym_1("HTTP.Request"), sym_1("allowed_methods"));
  if (! ident_get(&ident, &allowed_methods_tag)) {
    err_puts("http_request_method_from_str: missing"
             " HTTP.Request.allowed_methods");
    assert(!("http_request_method_from_str: missing"
             " HTTP.Request.allowed_methods"));
    return NULL;
  }
  if (allowed_methods_tag.type != TAG_LIST ||
      ! allowed_methods_tag.data.list ||
      allowed_methods_tag.data.list->tag.type != TAG_SYM) {
    err_puts("http_request_method_from_str: invalid"
             " HTTP.Request.allowed_methods");
    assert(!("http_request_method_from_str: invalid"
             " HTTP.Request.allowed_methods"));
    goto clean;
  }
  allowed_methods = allowed_methods_tag.data.list;
  if (! (tmp.data.sym = sym_find(str))) {
    tmp.type = TAG_STR;
    if (! str_init_copy(&tmp.data.str, str))
      goto clean;
    tag_clean(&allowed_methods_tag);
    *dest = tmp;
    return dest;
  }
  tmp.type = TAG_SYM;
  if (! list_has(&allowed_methods, &tmp, &b)) {
    err_puts("http_request_method_from_str: list_has");
    goto clean;
  }
  if (! b) {
    tag_init_str(&tmp, NULL, tmp.data.sym->str.size,
                 tmp.data.sym->str.ptr.pchar);
  }
  tag_clean(&allowed_methods_tag);
  *dest = tmp;
  return dest;
 clean:
  tag_clean(&allowed_methods_tag);
  return NULL;
}
