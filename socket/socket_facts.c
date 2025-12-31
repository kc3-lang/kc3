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
#include "../libkc3/alloc.h"
#include "../libkc3/assert.h"
#include "../libkc3/facts.h"
#include "../libkc3/marshall.h"
#include "socket_buf.h"
#include "socket_facts.h"

void socket_facts_clean (s_socket_facts *sf)
{
  assert(sf);
  marshall_clean(&sf->marshall);
  socket_buf_clean(&sf->socket);
}

void socket_facts_close (s_facts *facts)
{
  s_socket_facts *sf;
  assert(facts);
  if (! facts->log)
    return;
  sf = facts->log->hook_context;
  if (sf) {
    socket_facts_clean(sf);
    free(sf);
    facts->log->hook_context = NULL;
    facts->log->hook = NULL;
  }
}

void socket_facts_hook (void *context, e_fact_action action,
                        const s_fact *fact)
{
  s_socket_facts *sf;
  assert(context);
  assert(fact);
  sf = context;
  marshall_u8(&sf->marshall, false, action);
  marshall_fact(&sf->marshall, false, fact);
  marshall_to_buf(&sf->marshall, sf->socket.buf_rw.w);
}

s_socket_facts * socket_facts_init (s_socket_facts *sf,
                                    const s_str *host,
                                    const s_str *service)
{
  s_socket_facts tmp = {0};
  assert(sf);
  assert(host);
  assert(service);
  if (! socket_buf_init_connect(&tmp.socket, host, service))
    return NULL;
  if (! marshall_init(&tmp.marshall)) {
    socket_buf_clean(&tmp.socket);
    return NULL;
  }
  *sf = tmp;
  return sf;
}

void socket_facts_open (s_facts *facts, const s_str *host,
                        const s_str *service)
{
  s_socket_facts *sf;
  assert(facts);
  assert(host);
  assert(service);
  if (! facts->log)
    return;
  sf = alloc(sizeof(s_socket_facts));
  if (! sf)
    return;
  if (! socket_facts_init(sf, host, service)) {
    free(sf);
    return;
  }
  facts->log->hook = socket_facts_hook;
  facts->log->hook_context = sf;
}
