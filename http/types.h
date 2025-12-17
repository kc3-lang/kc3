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
#ifndef LIBKC3_HTTP_TYPES_H
#define LIBKC3_HTTP_TYPES_H

#ifdef WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif

#include "../libkc3/types.h"

/* 1 */
typedef s32                  t_socket;
typedef struct socket_buf    s_socket_buf;
typedef struct http_request  s_http_request;
typedef struct http_response s_http_response;

/* 2 */
typedef t_socket *p_socket;

/* Struct 1 */
struct socket_buf {
  struct sockaddr *addr;
  u32              addr_len;
  s_str            addr_str;
  t_socket sockfd;
  s_buf_rw buf_rw;
};

struct http_request {
  s_time  time;
  s_tag   method;
  s_str   url;
  s_tag   body;
  s_str   protocol;
  s_str   client_addr;
  s_list *headers;
  s_list *cookies;
  s_map   params;
};

struct http_response {
  s_str   protocol;
  u16     code;
  s_str   message;
  s_list *headers;
  s_tag   body;
};

#endif /* LIBKC3_HTTP_TYPES_H */
