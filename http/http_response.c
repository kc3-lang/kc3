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
#include "socket.h"

s_http_response * http_response_buf_write (s_http_response *response,
                                           s_buf *buf)
{
  sw    content_length = -1;
  s_str content_length_str = {0};
  s_tag default_messages = {0};
  s_ident ident = {0};
  s_tag *key = NULL;
  const s_list *l = NULL;
  s_tag message = {0};
  s_str protocol = {0};
  sw r = 0;
  s_tag tag_code = {0};
  s_tag *value = NULL;
  assert(response);
  assert(buf);
  if (! response->protocol.size)
    str_init_1(&protocol, NULL, "HTTP/1.1");
  else
    protocol = response->protocol;
  if ((r = buf_write_str(buf, &protocol)) < 0)
    return NULL;
  if ((r = buf_write_1(buf, " ")) < 0)
    return NULL;
  tag_code.type = TAG_U16;
  tag_code.data.u16 = response->code;
  if (! tag_code.data.u16)
    tag_code.data.u16 = 200;
  if (tag_code.data.u16 < 100 || tag_code.data.u16 > 999) {
    err_puts("http_response_buf_write: invalid response code");
    return NULL;
  }
  if ((r = buf_inspect_u16_decimal(buf, &tag_code.data.u16)) < 0)
    return NULL;
  if ((r = buf_write_1(buf, " ")) < 0)
    return NULL;
  if (! response->message.size) {
    ident_init(&ident, sym_1("HTTP.Response"), sym_1("default_messages"));
    ident_get(&ident, &default_messages);
    if (! tag_is_alist(&default_messages)) {
      err_puts("http_response_buf_write: invalid default_messages:"
               " not an AList");
      return NULL;
    }
    if (alist_get((const s_list * const *) &default_messages.data.list,
                  &tag_code, &message)) {
      if (message.type != TAG_STR) {
        err_puts("http_response_buf_write: invalid default message:"
                 " not a Str");
        return NULL;
      }
    }
  }
  else {
    message.type = TAG_STR;
    message.data.str = response->message;
  }
  if ((r= buf_write_str(buf, &message.data.str)) < 0)
    return NULL;
  if ((r = buf_write_1(buf, "\r\n")) < 0)
    return NULL;
  str_init_1(&content_length_str, NULL, "Content-Length");
  l = response->headers;
  while (l) {
    if (l->tag.type != TAG_TUPLE ||
        l->tag.data.tuple.count != 2) {
      err_puts("http_response_buf_write: invalid header: not a Tuple");
      return NULL;
    }
    key = l->tag.data.tuple.tag;
    value = key + 1;
    if (key->type != TAG_STR || value->type != TAG_STR) {
      err_puts("http_response_buf_write: invalid header: not a Str");
      return NULL;
    }
    if (! compare_str(&content_length_str, &key->data.str))
      sw_init_1(&content_length, value->data.str.ptr.pchar);
    if ((r = buf_write_str(buf, &key->data.str)) < 0)
      return NULL;
    if ((r = buf_write_1(buf, ": ")) < 0)
      return NULL;
    if ((r = buf_write_str(buf, &value->data.str)) < 0)
      return NULL;
    if ((r = buf_write_1(buf, "\r\n")) < 0)
      return NULL;
    l = list_next(l);
  }
  if ((r = buf_write_1(buf, "\r\n")) < 0)
    return NULL;
  if ((r = buf_write_str(buf, &response->body)) < 0)
    return NULL;
  return response;
}
