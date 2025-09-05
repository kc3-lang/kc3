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
#include "socket/types.h"
#include "tls/types.h"
#include <tls.h>
#include <unistd.h>
#include "../libkc3/kc3.h"

p_tls kc3_tls_server(void)
{
    p_tls ctx = tls_server();

    if (!ctx) {
        err_write_1("kc3_tls_server: tls_server: ");
        err_puts(tls_error(ctx));
        assert(! "kc3_tls_server: tls_server");
    }
    return ctx;
}

bool kc3_tls_server_accept_socket (p_tls ctx, p_tls *client_ctx,
                                   t_socket client_fd)
{
  if (tls_accept_socket(ctx, client_ctx, client_fd)) {
    err_write_1("kc3_tls_server_accept_socket: "
                "tls_accept_socket");
    assert(! "kc3_tls_accept_socket: tls_accept_socket");
    return false;
  }
  return true;
}
