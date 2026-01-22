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
#ifndef LIBKC3_TLS_FACTS_H
#define LIBKC3_TLS_FACTS_H

#include "types.h"

/* Stack-allocation compatible functions. */
void          tls_facts_clean (s_tls_facts *tf);
s_tls_facts * tls_facts_init (s_tls_facts *tf, p_tls *ctx,
                              const s_str *host, const s_str *service);

/* KC3 wrappers. */
bool * kc3_tls_facts_accept (s_facts *facts, t_socket *server,
                             p_tls *ctx, const s_str *secret, bool *dest);
s_tls_facts_acceptor ** kc3_tls_facts_acceptor_loop (s_facts *facts,
                             t_socket *server, p_tls *ctx,
                             const s_str *secret,
                             s_tls_facts_acceptor **dest);
void kc3_tls_facts_acceptor_loop_join (s_tls_facts_acceptor **acceptor);
bool * kc3_tls_facts_close (s_facts *facts, bool *dest);
bool * kc3_tls_facts_open (s_facts *facts, p_tls *ctx,
                           const s_str *host, const s_str *service,
                           const s_str *secret, bool *dest);

/* Facts replication API. */
s_facts * tls_facts_accept (s_facts *facts, t_socket *server,
                            p_tls *ctx, const s_str *secret);
s_tls_facts_acceptor * tls_facts_acceptor_loop (s_facts *facts,
                            t_socket *server, p_tls *ctx,
                            const s_str *secret);
void      tls_facts_acceptor_loop_join (s_tls_facts_acceptor *acceptor);
void *    tls_facts_acceptor_loop_thread (void *arg);
s_facts * tls_facts_close (s_facts *facts);
void *    tls_facts_listen_thread (void *arg);
s_facts * tls_facts_open (s_facts *facts, p_tls *ctx,
                          const s_str *host, const s_str *service,
                          const s_str *secret);

/* HMAC authentication. */
bool tls_facts_auth_challenge (s_buf *w, s_buf *r, const s_str *secret);
bool tls_facts_auth_response (s_buf *w, s_buf *r, const s_str *secret);

/* Log hooks for replication. */
void tls_facts_hook (void *context, e_fact_action action,
                     const s_fact *fact);
void tls_facts_listener_hook (void *context, e_fact_action action,
                              const s_fact *fact);

#endif /* LIBKC3_TLS_FACTS_H */
