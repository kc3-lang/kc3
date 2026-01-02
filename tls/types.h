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

#ifndef  LIBKC3_TLS_TYPES_H
#define  LIBKC3_TLS_TYPES_H

#include <pthread.h>
#include "../libkc3/types.h"
#include "../socket/types.h"

/* Struct types. */
typedef struct tls_buf              s_tls_buf;
typedef struct tls_client           s_tls_client;
typedef struct tls_facts            s_tls_facts;
typedef struct tls_facts_acceptor   s_tls_facts_acceptor;
typedef struct tls_facts_listener   s_tls_facts_listener;
typedef struct tls_server           s_tls_server;

/* Pointer types. */
typedef struct tls        *p_tls;
typedef struct tls_config *p_tls_config;

/* 1 */
struct tls_buf {
  p_tls ctx;
  t_socket socket;
};

struct tls_client {
  s_socket_buf socket_buf;
  s_str tls_version;
};

struct tls_server {
  s_socket_buf socket_buf;
  s_str tls_version;
};

/* 2 */
struct tls_facts {
  p_tls           ctx;
  s_env          *env;
  s_facts        *facts;
  s_log_hook     *hook;
  bool            running;
  s_tls_client    tls_client;
  s_marshall      marshall;
  s_marshall_read marshall_read;
  pthread_t       thread;
};

struct tls_facts_acceptor {
  p_tls           ctx;
  s_env          *env;
  s_facts        *facts;
  bool            running;
  t_socket        server;
  pthread_t       thread;
};

struct tls_facts_listener {
  p_tls           ctx;
  s_env          *env;
  s_facts        *facts;
  s_log_hook     *hook;
  bool            running;
  s_tls_server    tls_server;
  s_marshall      marshall;
  s_marshall_read marshall_read;
  pthread_t       thread;
};

#endif

