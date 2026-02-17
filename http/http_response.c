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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "http_response.h"

s_http_response * http_response_buf_parse (s_http_response *response,
                                           s_buf *buf, bool parse_body)
{
  uw      chunk_size = 0;
  s_str   chunk_size_str = {0};
  const s_str chunked_str = STR("chunked");
  p_list  chunks = NULL;
  p_list *chunks_tail;
  sw          content_length = -1;
  const s_str content_length_str = STR("content-length");
  s_tag *key = NULL;
  s_list **l = NULL;
  sw r = 0;
  s_buf_save save;
  s_str str;
  s_http_response tmp = {0};
  const s_str *transfer_encoding = NULL;
  const s_str  transfer_encoding_str = STR("transfer-encoding");
  s_tuple *tuple = NULL;
  s_tag *value = NULL;
  assert(response);
  assert(buf);
  buf_save_init(buf, &save);
  if (buf_read_until_1_into_str(buf, " ", &tmp.protocol) <= 0)
    goto clean;
  if (buf_parse_u16(buf, &tmp.code) <= 0)
    goto restore;
  if (tmp.code < 100 || tmp.code > 999) {
    err_puts("http_response_buf_parse: invalid response code");
    goto restore;
  }
  if ((r = buf_read_1(buf, " ")) <= 0)
    goto restore;
  if (buf_read_until_1_into_str(buf, "\r\n", &tmp.message) <= 0)
    goto restore;
  l = &tmp.headers;
  while (1) {
    if ((r = buf_read_1(buf, "\r\n")) < 0)
      goto restore;
    if (r > 0)
      break;
    *l = list_new_ptuple(2, NULL);
    if (! *l)
      goto restore;
    tuple = (*l)->tag.data.ptuple;
    key = tuple->tag;
    value = tuple->tag + 1;
    key->type = TAG_STR;
    if (buf_read_until_1_into_str(buf, ":", &key->data.str) <= 0)
      goto restore;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    value->type = TAG_STR;
    if (buf_read_until_1_into_str(buf, "\r\n", &value->data.str) <= 0)
      goto restore;
    if (! str_init_to_lower(&str, &key->data.str))
      goto restore;
    if (! compare_str(&content_length_str, &str))
      sw_init_str(&content_length, &value->data.str);
    else if (! compare_str(&transfer_encoding_str, &str))
      transfer_encoding = &value->data.str;
    str_clean(&str);
    l = &(*l)->next.data.plist;
  }
  if (! parse_body)
    goto ok;
  tmp.body.type = TAG_STR;
  if (transfer_encoding && ! compare_str(transfer_encoding, &chunked_str)) {
    chunks = NULL;
    chunks_tail = &chunks;
    while (1) {
      if (buf_read_until_1_into_str(buf, "\r\n", &chunk_size_str) <= 0) {
        err_puts("http_response_buf_parse: chunk_size_str");
        goto ko;
      }
      if (! uw_init_str_hexadecimal(&chunk_size, &chunk_size_str)) {
        err_puts("http_response_buf_parse: uw_init_str_hex");
        goto ko;
      }
      if (! chunk_size) {
        if (buf_read_1(buf, "\r\n") <= 0)
          goto ko;
        if (! str_init_concatenate_list(&tmp.body.data.str, chunks))
          goto ko;
        list_delete_all(chunks);
        goto ok;
      }
      if (! (*chunks_tail = list_new_str_alloc(chunk_size, NULL)))
        goto ko;
      if (! buf_read(buf, chunk_size, &(*chunks_tail)->tag.data.str)) {
        err_puts("http_response_buf_parse: buf_read");
        goto ko;
      }
      if (buf_read_1(buf, "\r\n") <= 0)
        goto ko;
      chunks_tail = &(*chunks_tail)->next.data.plist;
    }
  }
  if (content_length < 0)
    goto restore;
  if (! buf_read(buf, content_length, &tmp.body.data.str)) {
    err_puts("http_response_buf_parse: buf_read");
    goto restore;
  }
 ok:
  buf_save_clean(buf, &save);
  *response = tmp;
  return response;
 ko:
  list_delete_all(chunks);
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return NULL;
}

sw http_response_buf_write (const s_http_response *response,
                            s_buf *buf, bool send_body)
{
  sw    content_length = -1;
  s_str content_length_str = {0};
  s_tag default_messages = {0};
  s32 e;
  sw end;
  t_fd fd;
  s_ident ident = {0};
  s_buf *in;
  s_tag *key = NULL;
  const s_list *l = NULL;
  const s_map *map;
  s_str protocol = {0};
  sw r = 0;
  sw result = 0;
  sw s;
  sw size;
  sw start;
  s_str str;
  s_tag tag_code = {0};
  s_tag tag_message = {0};
  s_buf tmp;
  const s_sym *type;
  s_tag *value = NULL;
  sw w = 0;
  assert(response);
  if (! buf)
    return -1;
  if (! response->protocol.size)
    str_init_1(&protocol, NULL, "HTTP/1.1");
  else
    protocol = response->protocol;
  if ((r = buf_write_str(buf, &protocol)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  tag_code.type = TAG_U16;
  tag_code.data.u16 = response->code;
  if (! tag_code.data.u16)
    tag_code.data.u16 = 200;
  if (tag_code.data.u16 < 100 || tag_code.data.u16 > 999) {
    err_puts("http_response_buf_write: invalid response code");
    return -1;
  }
  if ((r = buf_inspect_u16_decimal(buf, tag_code.data.u16)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if (! response->message.size) {
    ident_init(&ident, sym_1("HTTP.Response"), sym_1("default_messages"));
    ident_get(&ident, &default_messages);
    if (! tag_is_alist(&default_messages)) {
      err_puts("http_response_buf_write: invalid default_messages:"
               " not an AList");
      tag_clean(&default_messages);
      return -1;
    }
    if (alist_get(default_messages.data.plist,
                  &tag_code, &tag_message)) {
      if (tag_message.type != TAG_STR) {
        err_puts("http_response_buf_write: invalid default message:"
                 " not a Str");
        tag_clean(&tag_message);
        tag_clean(&default_messages);
        return -1;
      }
    }
  }
  else {
    tag_message.type = TAG_STR;
    tag_message.data.str = response->message;
    tag_message.data.str.free.p = NULL;
  }
  if ((r = buf_write_str(buf, &tag_message.data.str)) < 0) {
    tag_clean(&tag_message);
    tag_clean(&default_messages);
    return r;
  }
  result += r;
  tag_clean(&tag_message);
  tag_clean(&default_messages);
  if ((r = buf_write_1(buf, "\r\n")) < 0)
    return r;
  result += r;
  str_init_1(&content_length_str, NULL, "Content-Length");
  l = response->headers;
  while (l) {
    if (l->tag.type != TAG_PTUPLE ||
        l->tag.data.ptuple->count != 2) {
      err_puts("http_response_buf_write: invalid header: not a Tuple");
      return -1;
    }
    key = l->tag.data.ptuple->tag;
    value = key + 1;
    if (key->type != TAG_STR || value->type != TAG_STR) {
      err_puts("http_response_buf_write: invalid header: not a Str");
      return -1;
    }
    if (! compare_str(&content_length_str, &key->data.str))
      sw_init_str(&content_length, &value->data.str);
    if ((r = buf_write_str(buf, &key->data.str)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ": ")) < 0)
      return r;
    result += r;
    if ((r = buf_write_str(buf, &value->data.str)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, "\r\n")) < 0)
      return r;
    result += r;
    l = list_next(l);
  }
  if (content_length < 0 &&
      response->body.type == TAG_STR) {
    if ((r = buf_write_str(buf, &content_length_str)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ": ")) < 0)
      return r;
    result += r;
    r = buf_inspect_u32_decimal(buf, response->body.data.str.size);
    if (r < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, "\r\n")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1(buf, "\r\n")) < 0)
    return r;
  result += r;
  if (send_body) {
    if (! tag_type(&response->body, &type))
      return -1;
    if (type == &g_sym_Str) {
      if ((r = buf_write_str(buf, &response->body.data.str)) < 0)
        return r;
      result += r;
    }
    else if (type == &g_sym_Buf) {
      in = response->body.data.pstruct->data;
      while (buf_refill(in, in->size) > 0) {
        err_inspect_buf(in);
        if (buf_read_to_str(in, &str) <= 0)
          return -1;
        err_inspect_str(&str);
        if ((r = buf_write(buf, str.ptr.pchar, str.size)) <= 0)
          return r;
        result += r;
        str_clean(&str);
      }
    }
    else if (type == &g_sym_S32) {
      buf_init_alloc(&tmp, BUF_SIZE);
      fd = response->body.data.s32;
      while (1) {
        if ((r = read(fd, tmp.ptr.p, tmp.size)) < 0) {
          e = errno;
          err_write_1("http_response_buf_write: ");
          err_inspect_s32(fd);
          err_write_1(": ");
          err_puts(strerror(e));
          return r;
        }
        if (! r)
          break;
        tmp.rpos = 0;
        tmp.wpos = r;
        while (tmp.rpos < (uw) r) {
          if ((w = buf_write(buf, tmp.ptr.ps8 + tmp.rpos,
                             r - tmp.rpos)) <= 0)
            return w;
          result += w;
          tmp.rpos += w;
        }
      }
      close(fd);
    }
    else if (type == &g_sym_Map &&
             (map = &response->body.data.map) &&
             map->count == 3 &&
             map->key[1].data.psym == sym_1("fd") &&
             map->value[1].type == TAG_S32 &&
             map->key[2].data.psym == sym_1("start") &&
             map->value[2].type == TAG_SW &&
             map->key[0].data.psym == sym_1("end_") &&
             map->value[0].type == TAG_SW) {
      fd    = map->value[1].data.s32;
      start = map->value[2].data.sw;
      end   = map->value[0].data.sw;
      if (start < 0)
        start = content_length + start + 1;
      if (end < 0)
        end = content_length + end + 1;
      if (content_length < 0 ||
          start > end ||
          start > content_length) {
        err_puts("http_response_buf_write:"
                 " 416 Requested Range Not Satisfiable");
        assert(!("http_response_buf_write:"
                 " 416 Requested Range Not Satisfiable"));
        return -1;
      }
      size = end - start;
      if (lseek(fd, start, SEEK_SET) < 0) {
        e = errno;
        err_write_1("http_response_buf_write: lseek ");
        err_inspect_s32(fd);
        err_write_1(": ");
        err_puts(strerror(e));
        return -1;
      }
      if (size) {
        buf_init_alloc(&tmp, BUF_SIZE);
        while (1) {
          s = tmp.size;
          if (size < s)
            s = size;
          if ((r = read(fd, tmp.ptr.p, s)) < 0) {
            e = errno;
            err_write_1("http_response_buf_write: read ");
            err_inspect_s32(fd);
            err_write_1(": ");
            err_puts(strerror(e));
            return r;
          }
          if (! r)
            break;
          size -= r;
          tmp.rpos = 0;
          tmp.wpos = r;
          while (tmp.rpos < (uw) r) {
            if ((w = buf_write(buf, tmp.ptr.ps8 + tmp.rpos,
                               r - tmp.rpos)) <= 0)
              return w;
            result += w;
            tmp.rpos += w;
          }
        }
      }
      close(fd);
    }
    else {
      err_write_1("http_response_buf_write: unknown body type: ");
      err_inspect_sym(type);
      err_write_1("\n");
      return -1;
    }
  }
  if ((r = buf_flush(buf)) < 0)
    return r;
  return result;
}

void http_response_clean (s_http_response *res)
{
  str_clean(&res->protocol);
  str_clean(&res->message);
  list_delete_all(res->headers);
  tag_clean(&res->body);
}

s_tag * http_response_find_header (const s_http_response *res,
                                   const s_str *key)
{
  s_list *h;
  assert(res);
  assert(key);
  h = res->headers;
  while (h) {
    if (h->tag.type != TAG_PTUPLE ||
        h->tag.data.ptuple->count != 2 ||
        h->tag.data.ptuple->tag->type != TAG_STR) {
      err_write_1("http_response_find_header: invalid header: ");
      err_inspect_tag(&h->tag);
      err_write_1("\n");
      return NULL;
    }
    if (! compare_str_case_insensitive(&h->tag.data.ptuple->tag->data.str,
                                       key))
      return h->tag.data.ptuple->tag + 1;
    h = list_next(h);
  }
  return NULL;
}

s_http_response * http_response_init_copy (s_http_response *res,
                                           s_http_response *src)
{
  s_http_response tmp = {0};
  if (! str_init_copy(&tmp.protocol, &src->protocol))
    return NULL;
  tmp.code = src->code;
  if (! str_init_copy(&tmp.message, &src->message))
    goto clean;
  if (! plist_init_copy(&tmp.headers, &src->headers))
    goto clean;
  if (! tag_init_copy(&tmp.body, &src->body))
    goto clean;
  *res = tmp;
  return res;
 clean:
  http_response_clean(&tmp);
  return NULL;
}

s_http_response * http_response_set_header (s_http_response *res,
                                            const s_str *key,
                                            const s_str *value,
                                            s_http_response *dest)
{
  s_tag *header;
  s_http_response tmp = {0};
  if (! http_response_init_copy(&tmp, res))
    return NULL;
  if ((header = http_response_find_header(&tmp, key))) {
    tag_clean(header);
    tag_init_str_copy(header, value);
  }
  else {
    if (! (tmp.headers = list_new_ptuple(2, tmp.headers)))
      goto clean;
    tag_init_str_copy(tmp.headers->tag.data.ptuple->tag, key);
    tag_init_str_copy(tmp.headers->tag.data.ptuple->tag + 1, value);
  }
  *dest = tmp;
  return dest;
 clean:
  http_response_clean(&tmp);
  return NULL;
}
