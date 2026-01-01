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
#ifndef HTTP_TYPES_H
#define HTTP_TYPES_H

#include <pthread.h>
#ifdef WIN64
# include <ws2tcpip.h>
#else
# ifdef WIN32
#  include <winsock2.h>
# else
#  include <sys/socket.h>
# endif
#endif

#include "../libkc3/types.h"

/* 1 */
typedef s64                           t_socket;
typedef struct socket_buf             s_socket_buf;
typedef struct socket_facts           s_socket_facts;
typedef struct socket_facts_listener  s_socket_facts_listener;
typedef struct http_request           s_http_request;
typedef struct http_response          s_http_response;

/* 2 */
typedef t_socket *p_socket;

/* Struct 1 */
struct socket_buf {
  struct sockaddr *addr;
  u32              addr_len;
  s_str            addr_str;
  s64 sockfd;
  s_buf_rw buf_rw;
};

struct http_request {
  const s_sym *method;
  s_str url;
  s_str protocol;
  s_list *headers;
};

struct http_response {
  s_str protocol;
  u16 code;
  s_str message;
  s_list *headers;
  s_str body;
};

struct socket_facts {
  s_env          *env;
  s_facts        *facts;
  s_socket_buf    socket;
  s_marshall      marshall;
  s_marshall_read marshall_read;
  pthread_t       thread;
};

struct socket_facts_listener {
  s_env          *env;
  s_facts        *facts;
  s_socket_buf    client;
  s_marshall      marshall;
  s_marshall_read marshall_read;
  pthread_t       thread;
};

#endif /* HTTP_TYPES_H */
