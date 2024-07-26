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
#include "socket.h"

s_http_request * http_request_buf_parse (s_http_request *req, s_buf *buf)
{
  s_str line;
  sw r;
  s_buf_save save;
  s_list **tail;
  s_http_request tmp = {0};
  assert(req);
  assert(buf);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "GET ")) < 0)
    goto restore;
  if (r > 0)
    tmp.method = sym_1("get");
  if (! tmp.method) {
    err_puts("http_request_buf_parse: no method");
    goto restore;
  }  
  if (! buf_read_until_1_into_str(buf, " ", &tmp.url)) {
    err_puts("http_request_buf_parse: invalid URL");
    goto restore;
  }
  if (! buf_read_until_1_into_str(buf, "\r\n", &tmp.protocol)) {
    err_puts("http_request_buf_parse: invalid protocol");
    goto restore;
  }
  tail = &tmp.headers;
  while (1) {
    if (! buf_read_until_1_into_str(buf, "\r\n", &line)) {
      err_puts("http_request_buf_parse: invalid header");
      goto restore;
    }
    if (line.size == 0)
      break;
    *tail = list_new(NULL);
    (*tail)->tag.type = TAG_STR;
    (*tail)->tag.data.str = line;
    tail = &(*tail)->next.data.list;
  }
  buf_save_clean(buf, &save);
  *req = tmp;
  return req;
 restore:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return NULL;
}
