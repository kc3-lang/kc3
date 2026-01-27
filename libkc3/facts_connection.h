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

/* Stack-allocation compatible functions, call facts_connection_clean
   after use. */
void                 facts_connection_clean (s_facts_connection *conn);
s_facts_connection * facts_connection_init (s_facts_connection *conn,
                                            s_facts *facts,
                                            s64 sockfd,
                                            p_tls tls);

/* Heap allocation functions, call facts_connection_delete after use. */
s_facts_connection * facts_connection_new (s_facts *facts, s64 sockfd,
                                           p_tls tls);
void                 facts_connection_delete (s_facts_connection *conn);

/* Operators. */
s_facts_connection * facts_connection_add (s_facts *facts, s64 sockfd,
                                            p_tls tls, bool is_server);
bool                 facts_connection_remove (s_facts *facts,
                                              s_facts_connection *conn);
void                 facts_connections_close_all (s_facts *facts);

/* Send to master. */
s_facts_connection * facts_connection_get_master (s_facts *facts);

#endif /* LIBKC3_FACTS_CONNECTION_H */
