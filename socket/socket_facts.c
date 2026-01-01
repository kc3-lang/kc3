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
#include "../libkc3/buf.h"
#include "../libkc3/env.h"
#include "../libkc3/env_fork.h"
#include "../libkc3/fact.h"
#include "../libkc3/facts.h"
#include "../libkc3/io.h"
#include "../libkc3/log.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "socket.h"
#include "socket_buf.h"
#include "socket_facts.h"

bool * kc3_socket_facts_close (s_facts *facts, bool *dest)
{
  assert(dest);
  *dest = socket_facts_close(facts) ? true : false;
  return dest;
}

bool * kc3_socket_facts_listen (s_facts *facts, const s_str *host,
                                const s_str *service, bool *dest)
{
  assert(dest);
  *dest = socket_facts_listen(facts, host, service) ? true : false;
  return dest;
}

bool * kc3_socket_facts_open (s_facts *facts, const s_str *host,
                              const s_str *service, bool *dest)
{
  assert(dest);
  *dest = socket_facts_open(facts, host, service) ? true : false;
  return dest;
}

void socket_facts_clean (s_socket_facts *sf)
{
  assert(sf);
  marshall_clean(&sf->marshall);
  socket_buf_clean(&sf->socket);
}

s_facts * socket_facts_close (s_facts *facts)
{
  s_socket_facts *sf;
  assert(facts);
  if (! facts->log)
    return facts;
  sf = facts->log->hooks ? facts->log->hooks->context : NULL;
  if (sf) {
    log_hook_remove(facts->log, sf->hook);
    socket_facts_clean(sf);
    free(sf);
  }
  return facts;
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

void socket_facts_listener_hook (void *context, e_fact_action action,
                                 const s_fact *fact)
{
  s_socket_facts_listener *listener;
  assert(context);
  assert(fact);
  listener = context;
  marshall_u8(&listener->marshall, false, action);
  marshall_fact(&listener->marshall, false, fact);
  marshall_to_buf(&listener->marshall, listener->client.buf_rw.w);
}

void * socket_facts_open_thread (void *arg)
{
  u8 action;
  bool b;
  s_fact fact = {0};
  s_marshall_read *mr;
  s_socket_facts *sf;
  sf = arg;
  env_global_set(sf->env);
  err_puts("socket_facts_open_thread: entered");
  mr = &sf->marshall_read;
  err_puts("socket_facts_open_thread: starting loop");
  while (1) {
    err_puts("socket_facts_open_thread: reading header");
    if (! marshall_read_header(mr)) {
      err_puts("socket_facts_open_thread: header read failed");
      break;
    }
    err_puts("socket_facts_open_thread: calling marshall_read_chunk");
    if (! marshall_read_chunk(mr)) {
      err_puts("socket_facts_open_thread: marshall_read_chunk failed");
      break;
    }
    err_puts("socket_facts_open_thread: calling marshall_read_u8");
    if (! marshall_read_u8(mr, false, &action)) {
      err_puts("socket_facts_open_thread: marshall_read_u8");
      break;
    }
    if (! marshall_read_fact(mr, false, &fact)) {
      err_puts("socket_facts_open_thread: marshall_read_fact");
      break;
    }
    switch (action) {
    case FACT_ACTION_ADD:
      facts_add_fact(sf->facts, &fact);
      break;
    case FACT_ACTION_REMOVE:
      facts_remove_fact(sf->facts, &fact, &b);
      break;
    default:
      break;
    }
    fact_clean(&fact);
    marshall_read_reset_chunk(mr);
  }
  err_puts("socket_facts_open_thread: connection closed");
  return NULL;
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

s_facts * socket_facts_listen (s_facts *facts, const s_str *host,
                               const s_str *service)
{
  s_socket_facts_listener *listener;
  t_socket server;
  assert(facts);
  assert(host);
  assert(service);
  err_write_1("socket_facts_listen: calling socket_init_listen\n");
  if (! socket_init_listen(&server, host, service)) {
    err_write_1("socket_facts_listen: socket_init_listen failed\n");
    return NULL;
  }
  err_write_1("socket_facts_listen: socket_init_listen ok, fd = ");
  err_inspect_s64_decimal(server);
  err_write_1("\n");
  listener = alloc(sizeof(s_socket_facts_listener));
  if (! listener) {
    socket_close(&server);
    return NULL;
  }
  listener->env = env_fork_new(env_global());
  if (! listener->env) {
    socket_close(&server);
    free(listener);
    return NULL;
  }
  listener->facts = facts;
  err_write_1("socket_facts_listen: calling socket_buf_init_accept\n");
  if (! socket_buf_init_accept(&listener->client, &server)) {
    err_write_1("socket_facts_listen: socket_buf_init_accept failed\n");
    socket_close(&server);
    free(listener);
    return NULL;
  }
  err_write_1("socket_facts_listen: socket_buf_init_accept ok\n");
  socket_close(&server);
  err_write_1("socket_facts_listen: calling marshall_read_init_buf\n");
  if (! marshall_read_init_buf(&listener->marshall_read,
                               listener->client.buf_rw.r)) {
    err_write_1("socket_facts_listen: marshall_read_init_buf failed\n");
    socket_buf_clean(&listener->client);
    free(listener);
    return NULL;
  }
  if (! marshall_init(&listener->marshall)) {
    err_write_1("socket_facts_listen: marshall_init failed\n");
    marshall_read_clean(&listener->marshall_read);
    socket_buf_clean(&listener->client);
    free(listener);
    return NULL;
  }
  err_write_1("socket_facts_listen: calling pthread_create\n");
  if (pthread_create(&listener->thread, NULL,
                     socket_facts_listen_thread, listener)) {
    err_write_1("socket_facts_listen: pthread_create failed\n");
    marshall_clean(&listener->marshall);
    marshall_read_clean(&listener->marshall_read);
    socket_buf_clean(&listener->client);
    free(listener);
    return NULL;
  }
  pthread_detach(listener->thread);
  err_write_1("socket_facts_listen: success\n");
  return facts;
}

void * socket_facts_listen_thread (void *arg)
{
  u8 action;
  bool b;
  s_fact fact = {0};
  s_socket_facts_listener *listener;
  s_marshall_read *mr;
  listener = arg;
  env_global_set(listener->env);
  err_puts("socket_facts_listen_thread: entered");
  mr = &listener->marshall_read;
  if (listener->facts->log) {
    listener->hook = log_hook_add(listener->facts->log,
                                  socket_facts_listener_hook, listener);
    if (listener->hook)
      err_puts("socket_facts_listen_thread: log hook set up");
    else
      err_puts("socket_facts_listen_thread: log_hook_add failed");
  }
  err_puts("socket_facts_listen_thread: starting loop");
  while (1) {
    err_puts("socket_facts_listen_thread: reading header");
    if (! marshall_read_header(mr)) {
      err_puts("socket_facts_listen_thread: header read failed");
      break;
    }
    err_write_1("socket_facts_listen_thread: heap_size = ");
    err_inspect_uw_decimal(mr->heap_size);
    err_write_1(", buf_size = ");
    err_inspect_uw_decimal(mr->buf_size);
    err_write_1("\n");
    err_puts("socket_facts_listen_thread: calling marshall_read_chunk");
    if (! marshall_read_chunk(mr)) {
      err_puts("socket_facts_listen_thread: marshall_read_chunk failed");
      break;
    }
    err_puts("socket_facts_listen_thread: marshall_read_chunk ok");
    err_puts("socket_facts_listen_thread: calling marshall_read_u8");
    if (! marshall_read_u8(mr, false, &action)) {
      err_puts("socket_facts_listen_thread: marshall_read_u8");
      break;
    }
    if (! marshall_read_fact(mr, false, &fact)) {
      err_puts("socket_facts_listen_thread: marshall_read_fact");
      break;
    }
    switch (action) {
    case FACT_ACTION_ADD:
      facts_add_fact(listener->facts, &fact);
      break;
    case FACT_ACTION_REMOVE:
      facts_remove_fact(listener->facts, &fact, &b);
      break;
    default:
      err_write_1("socket_facts_listen_thread: invalid action: ");
      err_inspect_u8_decimal(action);
      err_write_1("\n");
      break;
    }
    fact_clean(&fact);
    marshall_read_reset_chunk(mr);
  }
  if (listener->hook)
    log_hook_remove(listener->facts->log, listener->hook);
  marshall_clean(&listener->marshall);
  socket_buf_clean(&listener->client);
  marshall_read_clean(mr);
  env_fork_delete(listener->env);
  free(listener);
  err_write_1("socket_facts_listen_thread: connection closed");
  return NULL;
}

s_facts * socket_facts_open (s_facts *facts, const s_str *host,
                             const s_str *service)
{
  s_socket_facts *sf;
  assert(facts);
  assert(host);
  assert(service);
  if (! facts->log)
    return NULL;
  sf = alloc(sizeof(s_socket_facts));
  if (! sf)
    return NULL;
  if (! socket_facts_init(sf, host, service)) {
    free(sf);
    return NULL;
  }
  sf->facts = facts;
  sf->env = env_fork_new(env_global());
  if (! sf->env) {
    socket_facts_clean(sf);
    free(sf);
    return NULL;
  }
  if (! marshall_read_init_buf(&sf->marshall_read,
                               sf->socket.buf_rw.r)) {
    env_fork_delete(sf->env);
    socket_facts_clean(sf);
    free(sf);
    return NULL;
  }
  if (pthread_create(&sf->thread, NULL,
                     socket_facts_open_thread, sf)) {
    marshall_read_clean(&sf->marshall_read);
    env_fork_delete(sf->env);
    socket_facts_clean(sf);
    free(sf);
    return NULL;
  }
  pthread_detach(sf->thread);
  sf->hook = log_hook_add(facts->log, socket_facts_hook, sf);
  if (! sf->hook) {
    marshall_read_clean(&sf->marshall_read);
    env_fork_delete(sf->env);
    socket_facts_clean(sf);
    free(sf);
    return NULL;
  }
  return facts;
}
