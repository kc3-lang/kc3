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
#include <string.h>
#include <sys/socket.h>
#include <tls.h>
#include "../libkc3/alloc.h"
#include "../libkc3/assert.h"
#include "../libkc3/buf.h"
#include "../libkc3/buf_parse.h"
#include "../libkc3/env.h"
#include "../libkc3/env_fork.h"
#include "../libkc3/fact.h"
#include "../libkc3/facts.h"
#include "../libkc3/io.h"
#include "../libkc3/log.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/sha256.h"
#include "../libkc3/str.h"
#include "../socket/socket.h"
#include "tls_buf.h"
#include "tls_client.h"
#include "tls_facts.h"
#include "tls_server.h"

#define TLS_FACTS_CHALLENGE_SIZE 32

bool * kc3_tls_facts_close (s_facts *facts, bool *dest)
{
  assert(dest);
  *dest = tls_facts_close(facts) ? true : false;
  return dest;
}

bool * kc3_tls_facts_accept (s_facts *facts, t_socket *server,
                             p_tls *ctx, const s_str *secret, bool *dest)
{
  assert(dest);
  *dest = tls_facts_accept(facts, server, ctx, secret) ? true : false;
  return dest;
}

s_tls_facts_acceptor ** kc3_tls_facts_acceptor_loop (s_facts *facts,
                                    t_socket *server, p_tls *ctx,
                                    const s_str *secret,
                                    s_tls_facts_acceptor **dest)
{
  s_tls_facts_acceptor *tmp;
  assert(dest);
  tmp = tls_facts_acceptor_loop(facts, server, ctx, secret);
  if (! tmp)
    return NULL;
  *dest = tmp;
  return dest;
}

void kc3_tls_facts_acceptor_loop_join (s_tls_facts_acceptor **acceptor)
{
  assert(acceptor);
  tls_facts_acceptor_loop_join(*acceptor);
  *acceptor = NULL;
}

bool * kc3_tls_facts_open (s_facts *facts, p_tls *ctx,
                           const s_str *host, const s_str *service,
                           const s_str *secret, bool *dest)
{
  assert(dest);
  *dest = tls_facts_open(facts, ctx, host, service, secret) ? true : false;
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
  s_tls_buf *tls_buf;
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
      buf_flush(tf->tls_client.socket_buf.buf_rw.w);
      tls_buf = tf->tls_client.socket_buf.buf_rw.r->user_ptr;
      if (tls_buf && tls_buf->ctx)
        tls_close(tls_buf->ctx);
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
      buf_flush(listener->tls_server.socket_buf.buf_rw.w);
      tls_buf = listener->tls_server.socket_buf.buf_rw.r->user_ptr;
      if (tls_buf && tls_buf->ctx)
        tls_close(tls_buf->ctx);
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
  str_clean(&tf->secret);
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

s_facts * tls_facts_accept (s_facts *facts, t_socket *server, p_tls *ctx,
                            const s_str *secret)
{
  s_tls_facts_listener *listener;
  assert(facts);
  assert(server);
  assert(ctx);
  assert(secret);
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
  if (! str_init_copy(&listener->secret, secret)) {
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (! kc3_tls_server_init_accept(&listener->tls_server, server, ctx)) {
    str_clean(&listener->secret);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (! tls_facts_auth_challenge(listener->tls_server.socket_buf.buf_rw.w,
                                 listener->tls_server.socket_buf.buf_rw.r,
                                 &listener->secret)) {
    err_puts("tls_facts_accept: authentication failed");
    str_clean(&listener->secret);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (! marshall_read_init_buf(&listener->marshall_read,
                               listener->tls_server.socket_buf.buf_rw.r)) {
    err_puts("tls_facts_accept: marshall_read_init_buf");
    str_clean(&listener->secret);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  if (! marshall_init(&listener->marshall)) {
    err_puts("tls_facts_accept: marshall_init");
    marshall_read_clean(&listener->marshall_read);
    str_clean(&listener->secret);
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
    str_clean(&listener->secret);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  return facts;
}

s_tls_facts_acceptor * tls_facts_acceptor_loop (s_facts *facts,
                                                t_socket *server,
                                                p_tls *ctx,
                                                const s_str *secret)
{
  s_tls_facts_acceptor *acceptor;
  assert(facts);
  assert(server);
  assert(ctx);
  assert(secret);
  if (! facts->log)
    return NULL;
  acceptor = alloc(sizeof(s_tls_facts_acceptor));
  if (! acceptor)
    return NULL;
  acceptor->env = env_fork_new(env_global());
  if (! acceptor->env) {
    free(acceptor);
    return NULL;
  }
  acceptor->facts = facts;
  acceptor->ctx = *ctx;
  if (! str_init_copy(&acceptor->secret, secret)) {
    env_fork_delete(acceptor->env);
    free(acceptor);
    return NULL;
  }
  acceptor->server = *server;
  acceptor->running = true;
  if (pthread_create(&acceptor->thread, NULL,
                     tls_facts_acceptor_loop_thread, acceptor)) {
    err_puts("tls_facts_acceptor_loop: pthread_create");
    str_clean(&acceptor->secret);
    env_fork_delete(acceptor->env);
    free(acceptor);
    return NULL;
  }
  return acceptor;
}

void tls_facts_acceptor_loop_join (s_tls_facts_acceptor *acceptor)
{
  assert(acceptor);
  acceptor->running = false;
  shutdown(acceptor->server, SHUT_RDWR);
  socket_close(&acceptor->server);
  pthread_join(acceptor->thread, NULL);
}

void * tls_facts_acceptor_loop_thread (void *arg)
{
  s_tls_facts_acceptor *acceptor;
  acceptor = arg;
  env_global_set(acceptor->env);
  while (acceptor->running) {
    if (! tls_facts_accept(acceptor->facts, &acceptor->server,
                           &acceptor->ctx, &acceptor->secret)) {
      break;
    }
  }
  str_clean(&acceptor->secret);
  env_fork_delete(acceptor->env);
  free(acceptor);
  return NULL;
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
    marshall_read_clean(mr);
    marshall_clean(&listener->marshall);
    str_clean(&listener->secret);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
    return NULL;
  }
  listener->hook = log_hook_add(listener->facts->log,
                                tls_facts_listener_hook, listener);
  if (! listener->hook) {
    err_puts("tls_facts_listen_thread: log_hook_add");
    marshall_read_clean(mr);
    marshall_clean(&listener->marshall);
    str_clean(&listener->secret);
    kc3_tls_server_clean(&listener->tls_server);
    env_fork_delete(listener->env);
    free(listener);
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
  str_clean(&listener->secret);
  kc3_tls_server_clean(&listener->tls_server);
  env_fork_delete(listener->env);
  free(listener);
  return NULL;
}

s_facts * tls_facts_open (s_facts *facts, p_tls *ctx,
                          const s_str *host, const s_str *service,
                          const s_str *secret)
{
  s_tls_facts *tf;
  assert(facts);
  assert(ctx);
  assert(host);
  assert(service);
  assert(secret);
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
  if (! str_init_copy(&tf->secret, secret)) {
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  if (! tls_facts_auth_response(tf->tls_client.socket_buf.buf_rw.w,
                                tf->tls_client.socket_buf.buf_rw.r,
                                &tf->secret)) {
    err_puts("tls_facts_open: authentication failed");
    str_clean(&tf->secret);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  tf->env = env_fork_new(env_global());
  if (! tf->env) {
    str_clean(&tf->secret);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  if (! marshall_read_init_buf(&tf->marshall_read,
                               tf->tls_client.socket_buf.buf_rw.r)) {
    env_fork_delete(tf->env);
    str_clean(&tf->secret);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  if (pthread_create(&tf->thread, NULL,
                     tls_facts_open_thread, tf)) {
    marshall_read_clean(&tf->marshall_read);
    env_fork_delete(tf->env);
    str_clean(&tf->secret);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  tf->hook = log_hook_add(facts->log, tls_facts_hook, tf);
  if (! tf->hook) {
    tf->running = false;
    shutdown(tf->tls_client.socket_buf.sockfd, SHUT_RDWR);
    socket_close(&tf->tls_client.socket_buf.sockfd);
    pthread_join(tf->thread, NULL);
    marshall_read_clean(&tf->marshall_read);
    env_fork_delete(tf->env);
    str_clean(&tf->secret);
    tls_facts_clean(tf);
    free(tf);
    return NULL;
  }
  return facts;
}

bool tls_facts_auth_challenge (s_buf *w, s_buf *r, const s_str *secret)
{
  s_str challenge;
  s_str expected;
  s_str expected_b64;
  s_str response;
  bool result = false;
  uw response_len;
  if (! str_init_random_base64_uw(&challenge, TLS_FACTS_CHALLENGE_SIZE))
    return false;
  if (buf_write_str(w, &challenge) < 0) {
    str_clean(&challenge);
    return false;
  }
  if (buf_write_u8(w, '\n') < 0) {
    str_clean(&challenge);
    return false;
  }
  if (buf_flush(w) < 0) {
    str_clean(&challenge);
    return false;
  }
  if (! sha256_hmac_str(secret, &challenge, &expected)) {
    str_clean(&challenge);
    return false;
  }
  str_clean(&challenge);
  if (! str_init_base64url(&expected_b64, expected.ptr.p, expected.size)) {
    str_clean(&expected);
    return false;
  }
  str_clean(&expected);
  response_len = expected_b64.size;
  if (! buf_read(r, response_len, &response)) {
    str_clean(&expected_b64);
    return false;
  }
  if (response.size == expected_b64.size &&
      ! memcmp(response.ptr.p, expected_b64.ptr.p, response.size))
    result = true;
  str_clean(&response);
  str_clean(&expected_b64);
  return result;
}

bool tls_facts_auth_response (s_buf *w, s_buf *r, const s_str *secret)
{
  s_str challenge;
  s_str hmac;
  s_str hmac_b64;
  if (buf_read_until_1_into_str(r, "\n", &challenge) <= 0)
    return false;
  if (! sha256_hmac_str(secret, &challenge, &hmac)) {
    str_clean(&challenge);
    return false;
  }
  str_clean(&challenge);
  if (! str_init_base64url(&hmac_b64, hmac.ptr.p, hmac.size)) {
    str_clean(&hmac);
    return false;
  }
  str_clean(&hmac);
  if (buf_write_str(w, &hmac_b64) < 0) {
    str_clean(&hmac_b64);
    return false;
  }
  str_clean(&hmac_b64);
  if (buf_flush(w) < 0)
    return false;
  return true;
}
