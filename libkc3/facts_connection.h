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
#ifndef LIBKC3_FACTS_CONNECTION_H
#define LIBKC3_FACTS_CONNECTION_H

#include "types.h"

/* Stack-allocation compatible functions. */
void                facts_connection_clean (s_facts_connection *conn);
s_facts_connection * facts_connection_init (s_facts_connection *conn,
                                            s_facts *facts,
                                            s64 sockfd,
                                            p_tls tls);

/* Heap allocation. */
s_facts_connection * facts_connection_new (s_facts *facts, s64 sockfd,
                                            p_tls tls);
void                 facts_connection_delete (s_facts_connection *conn);

/* Connection list management on s_facts. */
s_facts_connection * facts_accept (s_facts *facts, s64 server_fd,
                                    p_tls server_tls);
s_facts_acceptor *   facts_acceptor_loop (s_facts *facts, s64 server,
                                           p_tls tls);
void                 facts_acceptor_loop_join (s_facts_acceptor *acceptor);
void *               facts_acceptor_loop_thread (void *arg);
s_facts_connection * facts_connect (s_facts *facts,
                                    const s_str *host,
                                    const s_str *service,
                                    p_tls_config config);
s_facts_connection * facts_connection_add (s_facts *facts, s64 sockfd,
                                            p_tls tls, bool is_server);
bool                 facts_connection_remove (s_facts *facts,
                                              s_facts_connection *conn);
void                 facts_connections_close_all (s_facts *facts);

/* KC3 wrappers. */
bool *               kc3_facts_accept (s_facts **facts, p_socket server_fd,
                                        p_tls *tls, bool *dest);
s_facts_acceptor **  kc3_facts_acceptor_loop (s_facts **facts, p_socket server,
                                               p_tls *tls,
                                               s_facts_acceptor **dest);
void                 kc3_facts_acceptor_loop_join (s_facts_acceptor **acceptor);
void                 kc3_facts_close_all (s_facts **facts);
bool *               kc3_facts_connect (s_facts **facts, const s_str *host,
                                         const s_str *service,
                                         p_tls_config *config, bool *dest);

/* Broadcasting to all connections. */
bool facts_broadcast_add (s_facts *facts, const s_fact *fact);
bool facts_broadcast_remove (s_facts *facts, const s_fact *fact);

/* Send to master. */
s_facts_connection * facts_connection_get_master (s_facts *facts);

#endif /* LIBKC3_FACTS_CONNECTION_H */
