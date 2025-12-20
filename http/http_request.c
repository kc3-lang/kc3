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
#include <string.h>
#include "../libkc3/kc3.h"
#include "http.h"
#include "http_request.h"
#include "url.h"

s_tag * http_request_buf_parse (s_tag *req, s_buf *buf)
{
  bool b;
  s_tag  body;
  s_str *body_str;
  s_str boundary = {0};
  s_str boundary_newline = {0};
  s_str boundary_tmp = {0};
  const s_str content_disposition_str = STR("Content-Disposition");
  const s_str content_length_str = STR("Content-Length");
  uw          content_length_uw = 0;
  s_str      *content_type = NULL;
  const s_str content_type_str = STR("Content-Type");
  const s_str cookie_str = STR("Cookie");
  const s_str dash = STR("--");
  s_env *env;
  s_tag filename = {0};
  s_buf header_buf = {0};
  s_str *key;
  s_str line;
  static s_tag method_key = {0};
  s_tag method_value = {0};
  static s_tag mode = {0};
  static const s_str multipart_form_data =
    STR("multipart/form-data; boundary=");
  s_list            *multipart_headers = NULL;
  s_tag              multipart_name = {0};
  s_str              multipart_value = {0};
  s_tag              multipart_value_tag = {0};
  const s_str newline = {{NULL}, 2, {"\r\n"}};
  s_tag path = {0};
  s_buf path_buf;
  s_str path_random;
  static s_tag   prefix = {0};
  static s_ident prefix_ident;
  s_list *           query_split;
  static const s_str query_separator = STR("?");
  sw r;
  static s_tag   random_len = {0};
  static s_ident random_len_ident;
  //s_buf_save save;
  s_tag size;
  static const s_sym *sym_Upload;
  s_list **tail;
  s_tag tmp = {0};
  s_http_request tmp_req = {0};
  s_tag upload = {0};
  s_str       url;
  static const s_str urlencoded =
    STR("application/x-www-form-urlencoded");
  s_str *value;
  assert(req);
  assert(buf);
  env = env_global();
  if (method_key.type == TAG_VOID) { // init static variables
    tag_init_str_1(&method_key, NULL, "_method");
    prefix_ident.module = sym_1("HTTP.Upload");
    prefix_ident.sym = sym_1("tmp_filename_prefix");
    if (! env_ident_get(env, &prefix_ident, &prefix) ||
        prefix.type != TAG_STR) {
      err_puts("http_request_buf_parse: invalid"
               " HTTP.Upload.tmp_filename_prefix");
      goto restore;
    }
    random_len_ident.module = sym_1("HTTP.Upload");
    random_len_ident.sym = sym_1("tmp_filename_random_length");
    if (! env_ident_get(env, &random_len_ident, &random_len) ||
        random_len.type != TAG_U8) {
      err_puts("http_request_buf_parse: invalid"
               " HTTP.Upload.tmp_filename_random_length");
      goto restore;
    }
    tag_init_u32(&mode, 0700);
    sym_Upload = sym_1("HTTP.Upload");
  }
  if (! http_request_buf_parse_method(buf, &tmp_req.method))
    goto restore;
  if (false) {
    err_write_1("http_request_buf_parse: method: ");
    err_inspect_tag(&tmp_req.method);
    err_write_1("\n");
  }
  time_init_now(&tmp_req.time);
  if (buf_read_until_1_into_str(buf, " ", &url) <= 0 ||
      url.size < 1 ||
      url.ptr.pchar[0] != '/') {
    err_puts("http_request_buf_parse: invalid URL");
    goto restore;
  }
  if (false) {
    err_write_1("http_request_buf_parse: url: ");
    err_inspect_str(&url);
    err_write_1("\n");
  }
  if (! str_split(&url, &query_separator, &query_split)) {
    err_puts("http_request_buf_parse: str_split(url, '?')");
    goto restore;
  }
  if (query_split->tag.type != TAG_STR) {
    err_puts("http_request_buf_parse: query_split first tag is"
	     " not a Str");
    goto restore;
  }
  str_clean(&url);
  url_unescape(&query_split->tag.data.str, &tmp_req.url);
  list_delete_all(query_split);
  if (false) {
    err_write_1("http_request_buf_parse: url: ");
    err_inspect_str(&tmp_req.url);
    err_write_1("\n");
  }
  if (buf_read_until_1_into_str(buf, "\r\n", &tmp_req.protocol) <= 0) {
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
    if (buf_read_until_1_into_str(buf, "\r\n", &line) <= 0) {
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
    tail = &(*tail)->next.data.plist;
  }
  if (content_type && content_length_uw) {
    if (false) {
      err_write_1("http_request_buf_parse: content_type ");
      err_inspect_str(content_type);
      err_write_1("\n");
      err_write_1("http_request_buf_parse: content_length_uw ");
      err_inspect_uw_decimal(content_length_uw);
      err_write_1("\n");
    }
    if (str_starts_with_case_insensitive(content_type,
                                         &multipart_form_data,
                                         &b) && b) {
      tag_init_plist(&tmp_req.body, NULL);
      if (! str_init_slice(&boundary_tmp, content_type,
                           multipart_form_data.size, -1))
        goto restore;
      if (! str_init_concatenate(&boundary, &dash, &boundary_tmp))
        goto restore;
      str_clean(&boundary_tmp);
      boundary_tmp = (s_str) {0};
      if (! str_init_concatenate(&boundary_newline, &newline, &boundary))
        goto restore;
      if (false) {
        err_write_1("http_request_buf_parse: boundary ");
        err_inspect_str(&boundary);
        err_write_1("\n");
      }
      if (buf_read_str(buf, &boundary) <= 0) {
        err_puts("http_request_buf_parse: invalid first multipart"
                 " boundary");
        goto restore;
      }
      while (1) {
        if ((r = buf_read_str(buf, &newline)) <= 0) {
          err_puts("http_request_buf_parse: missing newline");
          goto restore;
        }
        multipart_headers = NULL;
        tail = &multipart_headers;
        while (1) {
          if (buf_read_until_str_into_str(buf, &newline, &line) <= 0) {
            err_puts("http_request_buf_parse: invalid multipart"
                     " header");
            goto restore;
          }
          if (line.size == 0)
            break;
          *tail = list_new(NULL);
          (*tail)->tag.type = TAG_STR;
          if (! http_header_split(&line, &(*tail)->tag))
            goto restore;
          if ((*tail)->tag.type != TAG_TUPLE) {
            err_puts("http_request_buf_parse: http_header_split did not"
                     " return a Tuple");
            goto restore;
          }
          key   = &(*tail)->tag.data.tuple.tag[0].data.str;
          value = &(*tail)->tag.data.tuple.tag[1].data.str;
          if (! compare_str_case_insensitive
              (key, &content_disposition_str)) {
            buf_init_str_const(&header_buf, value);
            if (buf_read_1(&header_buf, "form-data; name=\"") <= 0) {
              err_puts("http_request_buf_parse: unknown content"
                       " disposition");
              goto restore;
            }
            multipart_name.type = TAG_STR;
            if (buf_read_until_1_into_str(&header_buf, "\"",
                                          &multipart_name.data.str)
                <= 0) {
              err_puts("http_request_buf_parse: invalid name");
              goto restore;
            }
            tag_clean(&filename);
            tag_init_str_empty(&filename);
            if (buf_read_1(&header_buf, "; filename=\"") > 0) {
              if (buf_read_until_1_into_str(&header_buf, "\"",
                                            &filename.data.str) <= 0 ||
                  str_character_position(&filename.data.str, '/')
                  >= 0) {
                err_puts("http_request_buf_parse: invalid filename");
                goto restore;
              }
            }
            if (header_buf.rpos != header_buf.wpos) {
              err_puts("http_request_buf_parse: failed to parse content"
                       " disposition header");
              goto restore;
            }
          } // if (! compare_str_case_insensitive(key, &content_disposition_str))
          tail = &(*tail)->next.data.plist;
        }
        if (false) {
          err_write_1("http_request_buf_parse: multipart headers = ");
          err_inspect_list(multipart_headers);
          err_write_1("\n");
          err_write_1("http_request_buf_parse: multipart name = ");
          err_inspect_str(&multipart_name.data.str);
          err_write_1("\n");
          err_write_1("http_request_buf_parse: filename = ");
          err_inspect_str(&filename.data.str);
          err_write_1("\n");
        }
        if (filename.data.str.size) {
          if (! buf_init_alloc(&path_buf, 4096)) {
            err_puts("http_request_buf_parse: buf_init_alloc(path_buf)");
            goto restore;
          }
          if (buf_write_str(&path_buf, &prefix.data.str) <= 0) {
            err_puts("http_request_buf_parse:"
                     " buf_write_str(path_buf, prefix)");
            goto restore;
          }
          if (! str_init_random_base64(&path_random, &random_len)) {
            err_puts("http_request_buf_parse: str_init_random_base64");
            goto restore;
          }
          if (buf_write_str(&path_buf, &path_random) <= 0) {
            err_puts("http_request_buf_parse:"
                     " buf_write_str(path_buf, path_random)");
            goto restore;
          }
          str_clean(&path_random);
          path_random = (s_str) {0};
          if (buf_write_1(&path_buf, "_") <= 0) {
            err_puts("http_request_buf_parse:"
                     " buf_write_1(path_buf, \"_\")");
            goto restore;
          }
          if (buf_write_str(&path_buf, &filename.data.str) <= 0) {
            err_puts("http_request_buf_parse:"
                     " buf_write_str(path_buf, filename)");
            goto restore;
          }
	  path.type = TAG_STR;
          if (buf_read_to_str(&path_buf, &path.data.str) <= 0) {
            err_puts("http_request_buf_parse:"
                     " buf_read_to_str(path_buf, path)");
            goto restore;
          }
          buf_clean(&path_buf);
          if (false) {
            err_write_1("http_request_buf_parse: path = ");
            err_inspect_str(&path.data.str);
            err_write_1("\n");
          }
          if (! file_ensure_parent_directory(&path.data.str, &mode)) {
	    err_puts("http_request_buf_parse,file_ensure_directory");
            goto restore;
	  }
	  size.type = TAG_SW;
          if ((size.data.sw = buf_read_until_str_into_file
	       (buf, &boundary_newline, &path.data.str)) < 0) {
            err_puts("http_request_buf_parse:"
                     " buf_read_until_str_into_file");
            goto restore;
          }
          if (true) {
            err_write_1("http_request_buf_parse: upload ");
            err_inspect_str(&path.data.str);
            err_write_1(" (");
            err_inspect_sw_decimal(size.data.sw);
            err_write_1(")\n");
          }
	  upload.type = TAG_PSTRUCT;
	  if (! pstruct_init(&upload.data.pstruct, sym_Upload))
	    goto restore;
	  if (! struct_allocate(upload.data.pstruct))
	    goto restore;
	  if (! struct_set(upload.data.pstruct, sym_1("filename"),
			   &filename))
	    goto restore;
	  if (! struct_set(upload.data.pstruct, sym_1("size"),
			   &size))
	    goto restore;
	  if (! struct_set(upload.data.pstruct, sym_1("tmp_path"),
			   &path))
	    goto restore;
          tmp_req.body.data.plist =
            list_new_tuple_2(&multipart_name, &upload,
                             tmp_req.body.data.plist);	  
        }
        else { // if (filename.data.str.size)
          if (buf_read_until_str_into_str(buf, &boundary_newline,
                                          &multipart_value) <= 0) {
            err_puts("http_request_buf_parse: failed to parse"
                     " multipart value");
            goto restore;
          }
          multipart_value_tag.type = TAG_STR;
          multipart_value_tag.data.str = multipart_value;
          tmp_req.body.data.plist =
            list_new_tuple_2(&multipart_name, &multipart_value_tag,
                             tmp_req.body.data.plist);
        } // else if (filename.data.str.size)
        list_delete_all(multipart_headers);
        if ((r = buf_read_str(buf, &dash)) < 0) {
          err_puts("http_request_buf_parse: buf_read_str(buf, dash)");
          goto restore;
        }
        if (r)
          break;
      } // while(1)
      if (buf_read_str(buf, &newline) <= 0) {
        err_puts("http_request_buf_parse: missing final newline");
        goto restore;
      }
    }
    else if (! compare_str_case_insensitive(content_type,
                                            &urlencoded)) {
      tmp_req.body.type = TAG_STR;
      body_str = &tmp_req.body.data.str;
      if (! buf_read(buf, content_length_uw, body_str))
        goto restore;
      if (false) {
        err_inspect_str(body_str);
        err_write_1("\n");
      }
      if (! url_www_form_decode(body_str, &body))
        goto restore;
      if (false) {
        err_write_1("http_request_buf_parse: body: ");
        err_inspect_tag(&body);
        err_write_1("\n");
      }
      if (alist_get(body.data.plist,
                    &method_key, &method_value)) {
        http_request_method_from_str(&method_value.data.str,
                                     &tmp_req.method);
        tag_clean(&method_value);
      }
      str_clean(body_str);
      tmp_req.body = body;
    }
  }
  if (! tag_init_pstruct(&tmp, sym_1("HTTP.Request")))
    goto restore;
  if (! struct_allocate(tmp.data.pstruct)) {
    tag_void(&tmp);
    goto restore;
  }
  *((s_http_request *) tmp.data.pstruct->data) = tmp_req;
  goto clean;
 restore:
  list_delete_all(multipart_headers);
  if (boundary_tmp.ptr.pchar != boundary.ptr.pchar)
    str_clean(&boundary_tmp);
  str_clean(&boundary);
  //buf_save_restore_rpos(buf, &save);
 clean:
  //buf_save_clean(buf, &save);
  *req = tmp;
  return req;
}

s_tag * http_request_buf_parse_method (s_buf *buf, s_tag *dest)
{
  //s_buf_save save;
  s_str str;
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  //buf_save_init(buf, &save);
  if (buf_read_until_1_into_str(buf, " ", &str) <= 0) {
    if (false)
      err_puts("http_request_buf_parse_method: buf_read_until_1_into_str");
    goto restore;
  }
  if (! http_request_method_from_str(&str, &tmp))
    goto restore;
  str_clean(&str);
  //buf_save_clean(buf, &save);
  *dest = tmp;
  return dest;
 restore:
  //buf_save_restore_rpos(buf, &save);
  //buf_save_clean(buf, &save);
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
  case TAG_PSYM:
    method = &req->method.data.psym->str;
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
    case TAG_PSYM:
      if ((r = buf_write_str(buf, &key->data.psym->str)) <= 0)
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

void http_request_clean (s_http_request *req)
{
  assert(req);
  time_clean(&req->time);
  tag_clean(&req->method);
  str_clean(&req->url);
  tag_clean(&req->body);
  str_clean(&req->protocol);
  str_clean(&req->client_addr);
  list_delete_all(req->headers);
  list_delete_all(req->cookies);
  map_clean(&req->params);
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
  tail = plist_tail(&req->cookies);
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
  if (allowed_methods_tag.type != TAG_PLIST ||
      ! allowed_methods_tag.data.plist ||
      allowed_methods_tag.data.plist->tag.type != TAG_PSYM) {
    err_puts("http_request_method_from_str: invalid"
             " HTTP.Request.allowed_methods");
    assert(!("http_request_method_from_str: invalid"
             " HTTP.Request.allowed_methods"));
    goto clean;
  }
  allowed_methods = allowed_methods_tag.data.plist;
  if (! (tmp.data.psym = sym_find(str))) {
    tmp.type = TAG_STR;
    if (! str_init_copy(&tmp.data.str, str))
      goto clean;
    tag_clean(&allowed_methods_tag);
    *dest = tmp;
    return dest;
  }
  tmp.type = TAG_PSYM;
  if (! list_has(allowed_methods, &tmp)) {
    tag_init_str(&tmp, NULL, tmp.data.psym->str.size,
                 tmp.data.psym->str.ptr.pchar);
  }
  tag_clean(&allowed_methods_tag);
  *dest = tmp;
  return dest;
 clean:
  tag_clean(&allowed_methods_tag);
  return NULL;
}
