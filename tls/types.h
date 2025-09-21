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

#include "../libkc3/types.h"
#include "../socket/types.h"

typedef struct tls        *p_tls;
typedef struct tls_buf     s_tls_buf;
typedef struct tls_client  s_tls_client;
typedef struct tls_config *p_tls_config;
typedef struct tls_server  s_tls_server;

struct tls_buf {
  p_tls ctx;
  t_socket socket;
};

struct tls_client {
  s_buf_rw buf_rw;
  t_socket socket;
};

struct tls_server {
  s_buf_rw buf_rw;
  t_socket socket;
};

#endif

