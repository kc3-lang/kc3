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
#ifndef LIBKC3_SOCKET_FACTS_H
#define LIBKC3_SOCKET_FACTS_H

#include "types.h"

/* Stack-allocation compatible functions. */
void             socket_facts_clean (s_socket_facts *sf);
s_socket_facts * socket_facts_init (s_socket_facts *sf,
                                    const s_str *host,
                                    const s_str *service);

/* KC3 wrappers. */
bool * kc3_socket_facts_accept (s_facts *facts, t_socket *server,
                                bool *dest);
bool * kc3_socket_facts_close (s_facts *facts, bool *dest);
bool * kc3_socket_facts_open (s_facts *facts, const s_str *host,
                              const s_str *service, bool *dest);

/* Facts replication API. */
s_facts * socket_facts_accept (s_facts *facts, t_socket *server);
s_facts * socket_facts_close (s_facts *facts);
void *    socket_facts_listen_thread (void *arg);
s_facts * socket_facts_open (s_facts *facts, const s_str *host,
                             const s_str *service);

/* Log hooks for replication. */
void socket_facts_hook (void *context, e_fact_action action,
                        const s_fact *fact);
void socket_facts_listener_hook (void *context, e_fact_action action,
                                 const s_fact *fact);

#endif /* LIBKC3_SOCKET_FACTS_H */
