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
#include <sys/socket.h>
#include <tls.h>
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
#include "../socket/socket.h"
#include "tls_buf.h"
#include "tls_client.h"
#include "tls_facts.h"
#include "tls_server.h"

bool * kc3_tls_facts_close (s_facts *facts, bool *dest)
{
  assert(dest);
  *dest = tls_facts_close(facts) ? true : false;
  return dest;
}

bool * kc3_tls_facts_accept (s_facts *facts, t_socket *server,
                             p_tls *ctx, bool *dest)
{
  assert(dest);
  *dest = tls_facts_accept(facts, server, ctx) ? true : false;
  return dest;
}

bool * kc3_tls_facts_open (s_facts *facts, p_tls *ctx,
                           const s_str *host, const s_str *service,
                           bool *dest)
{
  assert(dest);
  *dest = tls_facts_open(facts, ctx, host, service) ? true : false;
  return dest;
}

void tls_facts_clean (s_tls_facts *tf)
{
  assert(tf);
  marshall_clean(&tf->marshall);
  kc3_tls_client_clean(&tf->tls_client);
}

s_facts * tls_facts_close (s_facts *facts)
{
  s_log_hook *hook;
  s_tls_facts_listener *listener;
  s_log_hook *next;
  s_tls_facts *tf;
  pthread_t thread;
  assert(facts);
  if (! facts->log)
    return facts;
  hook = facts->log->hooks;
  while (hook) {
    next = hook->next;
    if (hook->f == tls_facts_hook) {
      tf = hook->context;
      tf->running = false;
      tf->hook = NULL;
      thread = tf->thread;
      shutdown(tf->tls_client.socket_buf.sockfd, SHUT_RDWR);
      socket_close(&tf->tls_client.socket_buf.sockfd);
      log_hook_remove(facts->log, hook);
      pthread_join(thread, NULL);
    }
    else if (hook->f == tls_facts_listener_hook) {
      listener = hook->context;
      listener->running = false;
      listener->hook = NULL;
      thread = listener->thread;
      shutdown(listener->tls_server.socket_buf.sockfd, SHUT_RDWR);
      socket_close(&listener->tls_server.socket_buf.sockfd);
      log_hook_remove(facts->log, hook);
      pthread_join(thread, NULL);
    }
    hook = next;
  }
  return facts;
}

void tls_facts_hook (void *context, e_fact_action action,
                     const s_fact *fact)
{
  s_tls_facts *tf;
  assert(context);
  assert(fact);
  tf = context;
  marshall_u8(&tf->marshall, false, action);
  marshall_fact(&tf->marshall, false, fact);
  marshall_to_buf(&tf->marshall, tf->tls_client.socket_buf.buf_rw.w);
}

void tls_facts_listener_hook (void *context, e_fact_action action,
                              const s_fact *fact)
{
  s_tls_facts_listener *listener;
  assert(context);
  assert(fact);
  listener = context;
  marshall_u8(&listener->marshall, false, action);
  marshall_fact(&listener->marshall, false, fact);
  marshall_to_buf(&listener->marshall,
                  listener->tls_server.socket_buf.buf_rw.w);
}

void * tls_facts_open_thread (void *arg)
{
  u8 action;
  bool b;
  s_fact fact = {0};
  s_marshall_read *mr;
  s_tls_facts *tf;
  tf = arg;
  env_global_set(tf->env);
  mr = &tf->marshall_read;
  tf->running = true;
  while (tf->running) {
    if (! marshall_read_header(mr))
      break;
    if (! marshall_read_chunk(mr)) {
      err_puts("tls_facts_open_thread: marshall_read_chunk");
      break;
    }
    if (! marshall_read_u8(mr, false, &action)) {
      err_puts("tls_facts_open_thread: marshall_read_u8");
      break;
    }
    if (! marshall_read_fact(mr, false, &fact)) {
      err_puts("tls_facts_open_thread: marshall_read_fact");
      break;
    }
    switch (action) {
    case FACT_ACTION_ADD:
      facts_add_fact(tf->facts, &fact);
      break;
    case FACT_ACTION_REMOVE:
      facts_remove_fact(tf->facts, &fact, &b);
      break;
    default:
      break;
    }
    fact_clean_all(&fact);
    marshall_read_reset_chunk(mr);
  }
  if (tf->hook)
    log_hook_remove(tf->facts->log, tf->hook);
  marshall_read_clean(mr);
  env_fork_delete(tf->env);
  tls_facts_clean(tf);
  free(tf);
  return NULL;
}

s_tls_facts * tls_facts_init (s_tls_facts *tf, p_tls *ctx,
                              const s_str *host, const s_str *service)
{
  s_tls_facts tmp = {0};
  assert(tf);
  assert(ctx);
  assert(host);
  assert(service);
  if (! kc3_tls_client_init_connect(&tmp.tls_client, ctx, (s_str *) host,
                                    (s_str *) service))
    return NULL;
  tmp.ctx = *ctx;
  if (! marshall_init(&tmp.marshall)) {
    kc3_tls_client_clean(&tmp.tls_client);
    return NULL;
  }
  *tf = tmp;
  return tf;
}

s_facts * tls_facts_accept (s_facts *facts, t_socket *server, p_tls *ctx)
{
  s_tls_facts_listener *listener;
  assert(facts);
  assert(server);
  assert(ctx);
  listener = alloc(sizeof(s_tls_facts_listener));
  if (! listener)
    return NULL;
  listener->env = env_fork_new(env_global());
  if (! listener->env) {
    free(listener);
    return NULL;
  }
  listener->facts = facts;
  listener->ctx = *ctx;
  if (! kc3_tls_server_init_accept(&listener->tls_server, server, ctx)) {
    err_puts("tls_facts_accept: kc3_tls_server_init_accept");
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (! marshall_read_init_buf(&listener->marshall_read,
                               listener->tls_server.socket_buf.buf_rw.r)) {
    err_puts("tls_facts_accept: marshall_read_init_buf");
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (! marshall_init(&listener->marshall)) {
    err_puts("tls_facts_accept: marshall_init");
    marshall_read_clean(&listener->marshall_read);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (pthread_create(&listener->thread, NULL,
                     tls_facts_listen_thread, listener)) {
    err_puts("tls_facts_accept: pthread_create");
    marshall_clean(&listener->marshall);
    marshall_read_clean(&listener->marshall_read);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  return facts;
}

void * tls_facts_listen_thread (void *arg)
{
  u8 action;
  bool b;
  s_fact fact = {0};
  s_tls_facts_listener *listener;
  s_marshall_read *mr;
  listener = arg;
  env_global_set(listener->env);
  mr = &listener->marshall_read;
  if (! listener->facts->log) {
    err_puts("tls_facts_listen_thread: listener facts log is NULL");
    return NULL;
  }
  listener->hook = log_hook_add(listener->facts->log,
                                tls_facts_listener_hook, listener);
  if (! listener->hook) {
    err_puts("tls_facts_listen_thread: log_hook_add");
    return NULL;
  }
  listener->running = true;
  while (listener->running) {
    if (! marshall_read_header(mr))
      break;
    if (! marshall_read_chunk(mr)) {
      err_puts("tls_facts_listen_thread: marshall_read_chunk");
      break;
    }
    if (! marshall_read_u8(mr, false, &action)) {
      err_puts("tls_facts_listen_thread: marshall_read_u8");
      break;
    }
    if (! marshall_read_fact(mr, false, &fact)) {
      err_puts("tls_facts_listen_thread: marshall_read_fact");
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
      err_write_1("tls_facts_listen_thread: invalid action: ");
      err_inspect_u8_decimal(action);
      err_write_1("\n");
      break;
    }
    fact_clean_all(&fact);
    marshall_read_reset_chunk(mr);
  }
  if (listener->hook)
    log_hook_remove(listener->facts->log, listener->hook);
  marshall_read_clean(mr);
  marshall_clean(&listener->marshall);
  kc3_tls_server_clean(&listener->tls_server);
  env_fork_delete(listener->env);
  free(listener);
  return NULL;
}

s_facts * tls_facts_open (s_facts *facts, p_tls *ctx,
                          const s_str *host, const s_str *service)
{
  s_tls_facts *tf;
  assert(facts);
  assert(ctx);
  assert(host);
  assert(service);
  if (! facts->log)
    return NULL;
  tf = alloc(sizeof(s_tls_facts));
  if (! tf)
    return NULL;
  if (! tls_facts_init(tf, ctx, host, service)) {
    free(tf);
    return NULL;
  }
  tf->facts = facts;
  tf->env = env_fork_new(env_global());
  if (! tf->env) {
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  if (! marshall_read_init_buf(&tf->marshall_read,
                               tf->tls_client.socket_buf.buf_rw.r)) {
    env_fork_delete(tf->env);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  if (pthread_create(&tf->thread, NULL,
                     tls_facts_open_thread, tf)) {
    marshall_read_clean(&tf->marshall_read);
    env_fork_delete(tf->env);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  tf->hook = log_hook_add(facts->log, tls_facts_hook, tf);
  if (! tf->hook) {
    marshall_read_clean(&tf->marshall_read);
    env_fork_delete(tf->env);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  return facts;
}
