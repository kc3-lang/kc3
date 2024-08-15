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
#include "http.h"
#include <string.h>

s_tag * http_request_buf_parse (s_tag *req, s_buf *buf)
{
  s_str line;
  sw r;
  s_buf_save save;
  s_list **tail;
  s_tag tmp = {0};
  s_http_request tmp_req = {0};
  assert(req);
  assert(buf);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "GET ")) < 0)
    goto restore;
  if (r > 0)
    tmp_req.method = sym_1("get");
  if ((r = buf_read_1(buf, "POST ")) < 0)
    goto restore;
  if (r > 0)
    tmp_req.method = sym_1("post");
  if ((r = buf_read_1(buf, "PUT ")) < 0)
    goto restore;
  if (r > 0)
    tmp_req.method = sym_1("put");
  if ((r = buf_read_1(buf, "HEAD ")) < 0)
    goto restore;
  if (r > 0)
    tmp_req.method = sym_1("head");
  if (! tmp_req.method)
    goto restore;
  if (false) {
    err_write_1("http_request_buf_parse: method: ");
    err_inspect_sym(&tmp_req.method);
    err_write_1("\n");
  }
  if (! buf_read_until_1_into_str(buf, " ", &tmp_req.url)) {
    err_puts("http_request_buf_parse: invalid URL");
    goto restore;
  }
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
    tail = &(*tail)->next.data.list;
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

sw http_request_buf_write (s_http_request *req, s_buf *buf)
{
  s_tag *key;
  s_list *list;
  sw r;
  sw result = 0;
  s_str str;
  const s_sym *type = &g_sym_Str;
  s_tag *value;
  assert(req);
  assert(buf);
  if (! req->method->str.size) {
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
  if (! list_is_alist((const s_list * const *) &req->headers)) {
    err_puts("http_request_buf_write: invalid headers: not an AList");
    return -1;
  }
  if (! str_init_to_upper(&str, &req->method->str))
    return -1;
  if ((r = buf_write_str(buf, &str)) < 0)
    return r;
  result += r;
  str_clean(&str);
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
