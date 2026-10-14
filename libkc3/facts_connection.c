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
#include "ht.h"
#include "io.h"
#include "marshall.h"
#include "marshall_read.h"
#include "rwlock.h"
#include "sha512.h"
#include "str.h"
#include "tag.h"
#include "tls_buf.h"

#define FACTS_CONNECTION_AUTH_CHALLENGE_SIZE 32
#define FACTS_CONNECTION_PROTOCOL_VERSION 2

static bool    facts_connection_auth (s_facts_connection *conn,
                                      bool is_server);
static bool    facts_connection_auth_read (s_marshall_read *mr,
                                           u8 *dest, uw size);
static s_facts_connection *
               facts_connection_find_by_addr (s_facts *facts,
                                              const s_str *addr);
static s_str * facts_connection_get_addr (s64 sockfd, s_str *dest);
static int     facts_connection_compare_remove (const void *a,
                                                const void *b);
static void *  facts_connection_thread (void *arg);

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
    if (! facts_connection_auth_read(mr, received_hmac,
                                     SHA512_DIGEST_LENGTH))
      return false;
    str_init(&challenge_str, NULL, FACTS_CONNECTION_AUTH_CHALLENGE_SIZE,
             (const char *) challenge);
    sha512_hmac(&facts->secret, &challenge_str, expected_hmac);
    i = 0;
    u8 diff = 0;
    while (i < SHA512_DIGEST_LENGTH) {
      diff |= (received_hmac[i] ^ expected_hmac[i]);
      i++;
    }
    if (diff != 0) {
      err_puts("facts_connection_auth: HMAC verification failed");
      return false;
    }
  }
  else {
    if (! facts_connection_auth_read(mr, challenge,
                                     FACTS_CONNECTION_AUTH_CHALLENGE_SIZE))
      return false;
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

static bool facts_connection_auth_read (s_marshall_read *mr,
                                        u8 *dest, uw size)
{
  uw i;
  uw wire_size;
  if (size != FACTS_CONNECTION_AUTH_CHALLENGE_SIZE &&
      size != SHA512_DIGEST_LENGTH)
    return false;
  wire_size = size * (sizeof("_KC3U8_") - 1 + sizeof(u8));
  if (! marshall_read_header_limit(mr, 0, 0, wire_size) ||
      mr->heap_offset || mr->buf_size != wire_size) {
    err_puts("facts_connection_auth: invalid message header");
    return false;
  }
  i = 0;
  while (i < size) {
    if (buf_read_1(mr->buf, "_KC3U8_") <= 0 ||
        buf_read_u8(mr->buf, dest + i) <= 0) {
      err_puts("facts_connection_auth: invalid message body");
      return false;
    }
    i++;
  }
  marshall_read_chunk_reset(mr);
  return true;
}

s_facts_connection * facts_connection_add (s_facts *facts, s64 sockfd,
                                            p_tls tls, bool is_server)
{
  s_str addr;
  s_facts_connection *conn;
  uw remote_next_id;
  u8 remote_priority;
  u8 remote_version;
  bool sent;
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
    tls_close(tls);
    tls_free(tls);
    close(sockfd);
    return NULL;
  }
  conn->addr = addr;
  if (! facts_connection_auth(conn, is_server)) {
    err_puts("facts_connection_add: facts_connection_auth");
    facts_connection_delete(conn);
    return NULL;
  }
#if HAVE_PTHREAD
  if (! rwlock_r(&facts->rwlock)) {
    facts_connection_delete(conn);
    return NULL;
  }
#endif
  sent = marshall_u8(&conn->marshall, false,
                     FACTS_CONNECTION_PROTOCOL_VERSION) &&
    marshall_u8(&conn->marshall, false, facts->priority) &&
    marshall_uw(&conn->marshall, false, facts->next_id) &&
    marshall_to_buf(&conn->marshall, conn->buf_rw.w) >= 0;
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  if (! sent) {
    facts_connection_delete(conn);
    return NULL;
  }
  marshall_reset_ht(&conn->marshall);
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
  if (! marshall_read_u8(&conn->marshall_read, false, &remote_version) ||
      remote_version != FACTS_CONNECTION_PROTOCOL_VERSION ||
      buf_peek_1(conn->marshall_read.buf, "_KC3U8_") <= 0) {
    err_puts("facts_connection_add: incompatible protocol version");
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
  marshall_read_chunk_reset(&conn->marshall_read);
  if (facts->priority == remote_priority) {
    err_puts("facts_connection_add: equal priority");
    facts_connection_delete(conn);
    return NULL;
  }
  conn->is_master = (facts->priority < remote_priority);
  conn->env = env_fork_new(env_global());
  if (! conn->env) {
    err_puts("facts_connection_add: env_fork_new");
    facts_connection_delete(conn);
    return NULL;
  }
#if HAVE_PTHREAD
  if (! rwlock_w(&facts->rwlock)) {
    facts_connection_delete(conn);
    return NULL;
  }
#endif
  if (conn->is_master && ! facts_connection_sync(conn, remote_next_id)) {
    err_puts("facts_connection_add: facts_connection_sync");
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    facts_connection_delete(conn);
    return NULL;
  }
  conn->next = facts->connections;
  facts->connections = conn;
  conn->running = true;
  if (pthread_create(&conn->thread, NULL, facts_connection_thread, conn)) {
    err_puts("facts_connection_add: pthread_create");
    facts->connections = conn->next;
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    env_fork_delete(conn->env);
    conn->env = NULL;
    facts_connection_delete(conn);
    return NULL;
  }
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
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
    alloc_free(conn);
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
  if (! marshall_init(&conn->marshall, BUF_SIZE)) {
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
    alloc_free(conn);
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
      if (thread)
        pthread_join(thread, NULL);
      facts_connection_delete(conn);
      return true;
    }
    c = &(*c)->next;
  }
  return false;
}

static int facts_connection_compare_remove (const void *a, const void *b)
{
  const s_facts_remove_log * const *x = a;
  const s_facts_remove_log * const *y = b;
  return ((*x)->id > (*y)->id) - ((*x)->id < (*y)->id);
}

bool facts_connection_send (s_facts_connection *conn, u8 action,
                            const s_fact *fact)
{
  if (! marshall_u8(&conn->marshall, false, action) ||
      ! marshall_fact(&conn->marshall, false, fact) ||
      marshall_to_buf(&conn->marshall, conn->buf_rw.w) < 0) {
    conn->running = false;
    if (conn->sockfd >= 0)
      shutdown(conn->sockfd, SHUT_RDWR);
    return false;
  }
  marshall_reset_ht(&conn->marshall);
  return true;
}

bool facts_connection_sync (s_facts_connection *conn, uw remote_next_id)
{
  uw count = 0;
  s_facts_cursor cursor;
  s_fact *fact;
  s_facts *facts;
  uw i;
  s_facts_remove_log *log;
  s_facts_remove_log **removals = NULL;
  s_fact remove_fact;
  bool result = false;
  s_fact start = {0};
  assert(conn);
  facts = conn->facts;
#if HAVE_PTHREAD
  if (! rwlock_r(&facts->rwlock))
    return false;
#endif
  if (facts->log && facts->log->failed)
    goto unlock;
  if (facts->next_id <= remote_next_id) {
    result = facts->next_id == remote_next_id;
    goto unlock;
  }
  start.subject = TAG_FIRST;
  start.predicate = TAG_FIRST;
  start.object = TAG_FIRST;
  start.id = remote_next_id;
  for (log = facts->remove_log; log; log = log->next) {
    if (log->id >= remote_next_id) {
      if (count == UW_MAX / sizeof(*removals))
        goto unlock;
      count++;
    }
  }
  if (count) {
    if (! (removals = alloc(count * sizeof(*removals))))
      goto unlock;
    i = 0;
    for (log = facts->remove_log; log; log = log->next)
      if (log->id >= remote_next_id)
        removals[i++] = log;
    qsort(removals, count, sizeof(*removals),
          facts_connection_compare_remove);
  }
  if (! facts_cursor_init(facts, &cursor, facts->index, &start, NULL))
    goto unlock;
  if (! facts_cursor_next(&cursor, &fact))
    goto clean;
  i = 0;
  while (fact || i < count) {
    if (i < count && (! fact || removals[i]->id <= fact->id)) {
      log = removals[i++];
      remove_fact.subject = &log->fact.subject;
      remove_fact.predicate = &log->fact.predicate;
      remove_fact.object = &log->fact.object;
      remove_fact.id = log->id;
      if (! facts_connection_send(conn, FACT_ACTION_REMOVE, &remove_fact))
        goto clean;
    }
    else {
      if (! facts_connection_send(conn, FACT_ACTION_ADD, fact) ||
          ! facts_cursor_next(&cursor, &fact))
        goto clean;
    }
  }
  result = true;
 clean:
  facts_cursor_clean(&cursor);
 unlock:
  alloc_free(removals);
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return result;
}

bool facts_connection_apply (s_facts_connection *conn, u8 action,
                              s_fact *fact)
{
  s_facts *facts = conn->facts;
  uw next_id;
  bool removed;
  bool result = false;
  if (action != FACT_ACTION_ADD && action != FACT_ACTION_REMOVE)
    return false;
#if HAVE_PTHREAD
  if (! rwlock_w(&facts->rwlock))
    return false;
#endif
  next_id = facts->next_id;
  if (! conn->is_master) {
    if (fact->id == UW_MAX || facts->transaction)
      goto clean;
    if (fact->id < next_id) {
      result = true;
      goto clean;
    }
  }
  switch (action) {
  case FACT_ACTION_ADD:
    if (conn->is_master)
      result = facts_add_fact_local(facts, fact) != NULL;
    else
      result = facts_add_fact_id(facts, fact) != NULL;
    break;
  case FACT_ACTION_REMOVE:
    if (conn->is_master)
      result = facts_remove_fact_local(facts, fact, &removed) != NULL;
    else
      result = facts_remove_fact_id(facts, fact, &removed) != NULL;
    break;
  default:
    break;
  }
  if (! conn->is_master) {
    if (result)
      facts->next_id = fact->id + 1;
    else
      facts->next_id = next_id;
  }
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return result;
}

static void * facts_connection_thread (void *arg)
{
  u8 action;
  bool ok;
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
    ok = facts_connection_apply(conn, action, &fact);
    fact_clean_all(&fact);
    marshall_read_chunk_reset(mr);
    if (mr->ht.items)
      ht_empty(&mr->ht);
    if (! ok)
      break;
  }
  conn->running = false;
  shutdown(conn->sockfd, SHUT_RDWR);
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
    if (thread)
      pthread_join(thread, NULL);
    facts_connection_delete(conn);
    conn = next;
  }
  facts->connections = NULL;
}
