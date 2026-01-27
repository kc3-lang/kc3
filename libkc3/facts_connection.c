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
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <tls.h>
#include <unistd.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_rw.h"
#include "compare.h"
#include "env.h"
#include "env_fork.h"
#include "fact.h"
#include "facts.h"
#include "facts_connection.h"
#include "facts_cursor.h"
#include "io.h"
#include "marshall.h"
#include "marshall_read.h"
#include "rwlock.h"
#include "sha512.h"
#include "str.h"
#include "tag.h"
#include "tls_buf.h"

#define FACTS_CONNECTION_AUTH_CHALLENGE_SIZE 32

static bool    facts_connection_auth (s_facts_connection *conn,
                                      bool is_server);
static s_facts_connection *
facts_connection_find_by_addr (s_facts *facts, const s_str *addr);
static s_str * facts_connection_get_addr (s64 sockfd, s_str *dest);
static bool    facts_connection_sync (s_facts_connection *conn,
                                      uw remote_next_id);
void *         facts_connection_thread (void *arg);

static bool facts_connection_auth (s_facts_connection *conn, bool is_server)
{
  u8 challenge[FACTS_CONNECTION_AUTH_CHALLENGE_SIZE];
  s_str challenge_str;
  u8 expected_hmac[SHA512_DIGEST_LENGTH];
  u8 received_hmac[SHA512_DIGEST_LENGTH];
  s_facts *facts;
  uw i;
  s_marshall *m;
  s_marshall_read *mr;
  assert(conn);
  facts = conn->facts;
  m = &conn->marshall;
  mr = &conn->marshall_read;
  if (is_server) {
    arc4random_buf(challenge, FACTS_CONNECTION_AUTH_CHALLENGE_SIZE);
    i = 0;
    while (i < FACTS_CONNECTION_AUTH_CHALLENGE_SIZE) {
      if (! marshall_u8(m, false, challenge[i]))
        return false;
      i++;
    }
    if (! marshall_to_buf(m, conn->buf_rw.w))
      return false;
    if (! marshall_read_header(mr))
      return false;
    if (! marshall_read_chunk(mr))
      return false;
    i = 0;
    while (i < SHA512_DIGEST_LENGTH) {
      if (! marshall_read_u8(mr, false, &received_hmac[i]))
        return false;
      i++;
    }
    marshall_read_reset_chunk(mr);
    str_init(&challenge_str, NULL, FACTS_CONNECTION_AUTH_CHALLENGE_SIZE,
             (const char *) challenge);
    sha512_hmac(&facts->secret, &challenge_str, expected_hmac);
    i = 0;
    while (i < SHA512_DIGEST_LENGTH) {
      if (received_hmac[i] != expected_hmac[i]) {
        err_puts("facts_connection_auth: HMAC verification failed");
        return false;
      }
      i++;
    }
  }
  else {
    if (! marshall_read_header(mr))
      return false;
    if (! marshall_read_chunk(mr))
      return false;
    i = 0;
    while (i < FACTS_CONNECTION_AUTH_CHALLENGE_SIZE) {
      if (! marshall_read_u8(mr, false, &challenge[i]))
        return false;
      i++;
    }
    marshall_read_reset_chunk(mr);
    str_init(&challenge_str, NULL, FACTS_CONNECTION_AUTH_CHALLENGE_SIZE,
             (const char *) challenge);
    sha512_hmac(&facts->secret, &challenge_str, expected_hmac);
    i = 0;
    while (i < SHA512_DIGEST_LENGTH) {
      if (! marshall_u8(m, false, expected_hmac[i]))
        return false;
      i++;
    }
    if (! marshall_to_buf(m, conn->buf_rw.w))
      return false;
  }
  return true;
}

bool facts_broadcast_add (s_facts *facts, const s_fact *fact)
{
  s_facts_connection *conn;
  assert(facts);
  assert(fact);
  conn = facts->connections;
  while (conn) {
    if (conn->running && conn->is_master) {
      marshall_u8(&conn->marshall, false, FACT_ACTION_ADD);
      marshall_fact(&conn->marshall, false, fact);
      marshall_to_buf(&conn->marshall, conn->buf_rw.w);
    }
    conn = conn->next;
  }
  return true;
}

bool facts_broadcast_remove (s_facts *facts, const s_fact *fact)
{
  s_facts_connection *conn;
  assert(facts);
  assert(fact);
  conn = facts->connections;
  while (conn) {
    if (conn->running && conn->is_master) {
      marshall_u8(&conn->marshall, false, FACT_ACTION_REMOVE);
      marshall_fact(&conn->marshall, false, fact);
      marshall_to_buf(&conn->marshall, conn->buf_rw.w);
    }
    conn = conn->next;
  }
  return true;
}

s_facts_connection * facts_connect (s_facts *facts,
                                    const s_str *host,
                                    const s_str *service,
                                    p_tls_config config)
{
  struct addrinfo  hints = {0};
  struct addrinfo *res;
  struct addrinfo *res0;
  s64              sockfd;
  p_tls            tls;
  assert(facts);
  assert(host);
  assert(service);
  assert(config);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(host->ptr.pchar, service->ptr.pchar, &hints, &res0)) {
    err_write_1("facts_connect: getaddrinfo: ");
    err_write_1(host->ptr.pchar);
    err_write_1(":");
    err_write_1(service->ptr.pchar);
    err_write_1(": ");
    err_puts(strerror(errno));
    return NULL;
  }
  sockfd = -1;
  res = res0;
  while (res) {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
      res = res->ai_next;
      continue;
    }
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      close(sockfd);
      sockfd = -1;
      res = res->ai_next;
      continue;
    }
    break;
  }
  freeaddrinfo(res0);
  if (sockfd < 0) {
    err_write_1("facts_connect: connect: ");
    err_write_1(host->ptr.pchar);
    err_write_1(":");
    err_write_1(service->ptr.pchar);
    err_write_1(": ");
    err_puts(strerror(errno));
    return NULL;
  }
  tls = tls_client();
  if (! tls) {
    err_puts("facts_connect: tls_client");
    close(sockfd);
    return NULL;
  }
  if (tls_configure(tls, config) < 0) {
    err_write_1("facts_connect: tls_configure: ");
    err_puts(tls_error(tls));
    tls_free(tls);
    close(sockfd);
    return NULL;
  }
  if (tls_connect_socket(tls, sockfd, host->ptr.pchar) < 0) {
    err_write_1("facts_connect: tls_connect_socket: ");
    err_puts(tls_error(tls));
    tls_free(tls);
    close(sockfd);
    return NULL;
  }
  if (tls_handshake(tls) < 0) {
    err_write_1("facts_connect: tls_handshake: ");
    err_puts(tls_error(tls));
    tls_free(tls);
    close(sockfd);
    return NULL;
  }
  return facts_connection_add(facts, sockfd, tls, false);
}

s_facts_connection * facts_connection_add (s_facts *facts, s64 sockfd,
                                            p_tls tls, bool is_server)
{
  s_str addr;
  s_facts_connection *conn;
  uw remote_next_id;
  u8 remote_priority;
  assert(facts);
  assert(tls);
  if (! facts_connection_get_addr(sockfd, &addr)) {
    err_puts("facts_connection_add: facts_connection_get_addr");
    tls_close(tls);
    tls_free(tls);
    close(sockfd);
    return NULL;
  }
  if (facts_connection_find_by_addr(facts, &addr)) {
    if (! facts->shutting_down)
      err_puts("facts_connection_add: duplicate connection");
    str_clean(&addr);
    tls_close(tls);
    tls_free(tls);
    close(sockfd);
    return NULL;
  }
  conn = facts_connection_new(facts, sockfd, tls);
  if (! conn) {
    str_clean(&addr);
    return NULL;
  }
  conn->addr = addr;
  if (! facts_connection_auth(conn, is_server)) {
    err_puts("facts_connection_add: facts_connection_auth");
    facts_connection_delete(conn);
    return NULL;
  }
  marshall_u8(&conn->marshall, false, facts->priority);
  marshall_uw(&conn->marshall, false, facts->next_id);
  marshall_to_buf(&conn->marshall, conn->buf_rw.w);
  if (! marshall_read_header(&conn->marshall_read)) {
    err_puts("facts_connection_add: marshall_read_header");
    facts_connection_delete(conn);
    return NULL;
  }
  if (! marshall_read_chunk(&conn->marshall_read)) {
    err_puts("facts_connection_add: marshall_read_chunk");
    facts_connection_delete(conn);
    return NULL;
  }
  if (! marshall_read_u8(&conn->marshall_read, false, &remote_priority)) {
    err_puts("facts_connection_add: marshall_read_u8");
    facts_connection_delete(conn);
    return NULL;
  }
  if (! marshall_read_uw(&conn->marshall_read, false, &remote_next_id)) {
    err_puts("facts_connection_add: marshall_read_uw");
    facts_connection_delete(conn);
    return NULL;
  }
  marshall_read_reset_chunk(&conn->marshall_read);
  if (facts->priority == remote_priority) {
    err_puts("facts_connection_add: equal priority");
    facts_connection_delete(conn);
    return NULL;
  }
  conn->is_master = (facts->priority < remote_priority);
  if (conn->is_master && ! facts_connection_sync(conn, remote_next_id)) {
    err_puts("facts_connection_add: facts_connection_sync");
    facts_connection_delete(conn);
    return NULL;
  }
  conn->env = env_fork_new(env_global());
  if (! conn->env) {
    err_puts("facts_connection_add: env_fork_new");
    facts_connection_delete(conn);
    return NULL;
  }
  conn->next = facts->connections;
  facts->connections = conn;
  conn->running = true;
  if (pthread_create(&conn->thread, NULL, facts_connection_thread, conn)) {
    err_puts("facts_connection_add: pthread_create");
    facts->connections = conn->next;
    env_fork_delete(conn->env);
    conn->env = NULL;
    facts_connection_delete(conn);
    return NULL;
  }
  return conn;
}

void facts_connection_clean (s_facts_connection *conn)
{
  assert(conn);
  if (conn->env) {
    env_fork_delete(conn->env);
    conn->env = NULL;
  }
  str_clean(&conn->addr);
  marshall_clean(&conn->marshall);
  marshall_read_clean(&conn->marshall_read);
  tls_buf_close(conn->buf_rw.w);
  tls_buf_close(conn->buf_rw.r);
  if (conn->tls) {
    tls_close(conn->tls);
    tls_free(conn->tls);
  }
  buf_rw_clean(&conn->buf_rw);
  if (conn->sockfd >= 0)
    close(conn->sockfd);
}

void facts_connection_delete (s_facts_connection *conn)
{
  if (conn) {
    facts_connection_clean(conn);
    free(conn);
  }
}

static s_facts_connection *
facts_connection_find_by_addr (s_facts *facts, const s_str *addr)
{
  s_facts_connection *conn;
  assert(facts);
  assert(addr);
  conn = facts->connections;
  while (conn) {
    if (! compare_str(&conn->addr, addr))
      return conn;
    conn = conn->next;
  }
  return NULL;
}

static s_str * facts_connection_get_addr (s64 sockfd, s_str *dest)
{
  char buf[INET6_ADDRSTRLEN];
  struct sockaddr_storage addr;
  socklen_t len;
  void *src;
  len = sizeof(addr);
  if (getpeername(sockfd, (struct sockaddr *) &addr, &len) < 0)
    return NULL;
  if (addr.ss_family == AF_INET)
    src = &((struct sockaddr_in *) &addr)->sin_addr;
  else if (addr.ss_family == AF_INET6)
    src = &((struct sockaddr_in6 *) &addr)->sin6_addr;
  else
    return NULL;
  if (! inet_ntop(addr.ss_family, src, buf, sizeof(buf)))
    return NULL;
  return str_init_1_alloc(dest, buf);
}

s_facts_connection * facts_connection_get_master (s_facts *facts)
{
  s_facts_connection *conn;
  assert(facts);
  conn = facts->connections;
  while (conn) {
    if (conn->running && ! conn->is_master)
      return conn;
    conn = conn->next;
  }
  return NULL;
}

s_facts_connection * facts_connection_init (s_facts_connection *conn,
                                            s_facts *facts,
                                            s64 sockfd,
                                            p_tls tls)
{
  assert(conn);
  assert(facts);
  assert(tls);
  *conn = (s_facts_connection) {0};
  conn->facts = facts;
  conn->sockfd = sockfd;
  conn->tls = tls;
  if (! buf_rw_init_alloc(&conn->buf_rw, 65536)) {
    err_puts("facts_connection_init: buf_rw_init_alloc");
    return NULL;
  }
  tls_buf_open_r(conn->buf_rw.r, tls);
  tls_buf_open_w(conn->buf_rw.w, tls);
  if (! marshall_init(&conn->marshall)) {
    err_puts("facts_connection_init: marshall_init");
    buf_rw_clean(&conn->buf_rw);
    return NULL;
  }
  if (! marshall_read_init_buf(&conn->marshall_read, conn->buf_rw.r)) {
    err_puts("facts_connection_init: marshall_read_init_buf");
    marshall_clean(&conn->marshall);
    buf_rw_clean(&conn->buf_rw);
    return NULL;
  }
  return conn;
}

s_facts_connection * facts_connection_new (s_facts *facts, s64 sockfd,
                                            p_tls tls)
{
  s_facts_connection *conn;
  conn = alloc(sizeof(s_facts_connection));
  if (! conn)
    return NULL;
  if (! facts_connection_init(conn, facts, sockfd, tls)) {
    free(conn);
    return NULL;
  }
  return conn;
}

bool facts_connection_remove (s_facts *facts, s_facts_connection *conn)
{
  s_facts_connection **c;
  pthread_t thread;
  assert(facts);
  assert(conn);
  signal(SIGPIPE, SIG_IGN);
  c = &facts->connections;
  while (*c) {
    if (*c == conn) {
      *c = conn->next;
      conn->running = false;
      shutdown(conn->sockfd, SHUT_RDWR);
      thread = conn->thread;
      pthread_join(thread, NULL);
      facts_connection_delete(conn);
      return true;
    }
    c = &(*c)->next;
  }
  return false;
}

static bool facts_connection_sync (s_facts_connection *conn,
                                   uw remote_next_id)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_facts *facts;
  s_facts_remove_log **log;
  s_facts_remove_log *tmp;
  s_fact remove_fact;
  bool result = false;
  s_fact start = {0};
  assert(conn);
  facts = conn->facts;
#if HAVE_PTHREAD
  if (! rwlock_r(&facts->rwlock))
    return false;
#endif
  if (facts->next_id <= remote_next_id) {
#if HAVE_PTHREAD
    rwlock_unlock_r(&facts->rwlock);
#endif
    return true;
  }
  start.subject = TAG_FIRST;
  start.predicate = TAG_FIRST;
  start.object = TAG_FIRST;
  start.id = remote_next_id;
  facts_cursor_init(facts, &cursor, facts->index, &start, NULL);
  while (facts_cursor_next(&cursor, &fact) && fact) {
    if (! marshall_u8(&conn->marshall, false, FACT_ACTION_ADD))
      goto clean;
    if (! marshall_fact(&conn->marshall, false, fact))
      goto clean;
    if (! marshall_to_buf(&conn->marshall, conn->buf_rw.w))
      goto clean;
  }
  log = &facts->remove_log;
  while (*log) {
    if ((*log)->fact.id >= remote_next_id) {
      remove_fact.subject = &(*log)->fact.subject;
      remove_fact.predicate = &(*log)->fact.predicate;
      remove_fact.object = &(*log)->fact.object;
      remove_fact.id = (*log)->fact.id;
      if (! marshall_u8(&conn->marshall, false, FACT_ACTION_REMOVE))
        goto clean;
      if (! marshall_fact(&conn->marshall, false, &remove_fact))
        goto clean;
      if (! marshall_to_buf(&conn->marshall, conn->buf_rw.w))
        goto clean;
      if (++(*log)->sync_count >= (*log)->target_count) {
        tmp = *log;
        *log = tmp->next;
        fact_w_clean(&tmp->fact);
        free(tmp);
        continue;
      }
    }
    log = &(*log)->next;
  }
  result = true;
 clean:
  facts_cursor_clean(&cursor);
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return result;
}

void * facts_connection_thread (void *arg)
{
  u8 action;
  bool b;
  s_facts_connection *conn;
  s_fact fact = {0};
  s_marshall_read *mr;
  conn = arg;
  env_global_set(conn->env);
  mr = &conn->marshall_read;
  while (conn->running) {
    if (! marshall_read_header(mr))
      break;
    if (! marshall_read_chunk(mr)) {
      err_puts("facts_connection_thread: marshall_read_chunk");
      break;
    }
    if (! marshall_read_u8(mr, false, &action)) {
      err_puts("facts_connection_thread: marshall_read_u8");
      break;
    }
    if (! marshall_read_fact(mr, false, &fact)) {
      err_puts("facts_connection_thread: marshall_read_fact");
      break;
    }
    switch (action) {
    case FACT_ACTION_ADD:
      if (conn->is_master)
        facts_add_fact_local(conn->facts, &fact);
      else
        facts_add_fact_id(conn->facts, &fact);
      break;
    case FACT_ACTION_REMOVE:
      facts_remove_fact_local(conn->facts, &fact, &b);
      break;
    default:
      err_write_1("facts_connection_thread: invalid action: ");
      err_inspect_u8_decimal(action);
      err_write_1("\n");
      break;
    }
    fact_clean_all(&fact);
    marshall_read_reset_chunk(mr);
  }
  conn->running = false;
  return NULL;
}

void facts_connections_close_all (s_facts *facts)
{
  s_facts_connection *conn;
  s_facts_connection *next;
  pthread_t thread;
  assert(facts);
  signal(SIGPIPE, SIG_IGN);
  conn = facts->connections;
  while (conn) {
    next = conn->next;
    conn->running = false;
    shutdown(conn->sockfd, SHUT_RDWR);
    thread = conn->thread;
    pthread_join(thread, NULL);
    facts_connection_delete(conn);
    conn = next;
  }
  facts->connections = NULL;
}

void kc3_facts_close_all (s_facts **facts)
{
  assert(facts);
  facts_connections_close_all(*facts);
}

bool * kc3_facts_accept (s_facts **facts, p_socket server_fd, p_tls *tls,
                          bool *dest)
{
  assert(facts);
  assert(server_fd);
  assert(tls);
  assert(dest);
  *dest = facts_accept(*facts, *server_fd, *tls) ? true : false;
  return dest;
}

s_facts_acceptor ** kc3_facts_acceptor_loop (s_facts **facts, p_socket server,
                                              p_tls *tls,
                                              s_facts_acceptor **dest)
{
  s_facts_acceptor *tmp;
  assert(facts);
  assert(server);
  assert(tls);
  assert(dest);
  tmp = facts_acceptor_loop(*facts, *server, *tls);
  if (! tmp)
    return NULL;
  *dest = tmp;
  return dest;
}

void kc3_facts_acceptor_loop_join (s_facts_acceptor **acceptor)
{
  assert(acceptor);
  facts_acceptor_loop_join(*acceptor);
  *acceptor = NULL;
}

bool * kc3_facts_connect (s_facts **facts, const s_str *host,
                          const s_str *service, p_tls_config *config,
                          bool *dest)
{
  assert(facts);
  assert(host);
  assert(service);
  assert(config);
  assert(dest);
  *dest = facts_connect(*facts, host, service, *config) ? true : false;
  return dest;
}
