#include <string.h>
#include <tls.h>
#include "../libkc3/buf.h"
#include "../libkc3/buf_save.h"
#include "../libkc3/socket.h"
#include "../libkc3/timespec.h"
#include "test.h"

typedef struct tls_buf_sink {
  char data[64];
  uw size;
  uw chunk;
  uw calls;
  uw fail_at;
  uw mode;
  uw waits;
  bool wait_write;
  bool pending;
  bool bad_retry;
  char pending_data[64];
  size_t pending_size;
} s_tls_buf_sink;

static s_tls_buf_sink *g_tls_buf_sink;

static ssize_t tls_buf_write_test (struct tls *ctx, const void *data,
                                   size_t size)
{
  s_tls_buf_sink *sink = (s_tls_buf_sink *) ctx;
  if (++sink->calls > 64 || ! sink->chunk)
    return -1;
  if (sink->pending) {
    if (size != sink->pending_size ||
        memcmp(data, sink->pending_data, size)) {
      sink->bad_retry = true;
      return -1;
    }
    sink->pending = false;
  }
  if (sink->calls == sink->fail_at) {
    if (sink->mode == 0)
      return -1;
    if (sink->mode == 1)
      return 0;
    if (size > sizeof(sink->pending_data))
      return -1;
    memcpy(sink->pending_data, data, size);
    sink->pending_size = size;
    sink->pending = true;
    return sink->mode == 3 ? TLS_WANT_POLLIN : TLS_WANT_POLLOUT;
  }
  if (size > sink->chunk)
    size = sink->chunk;
  if (size > sizeof(sink->data) - sink->size)
    return -1;
  memcpy(sink->data + sink->size, data, size);
  sink->size += size;
  return size;
}

static bool tls_buf_timeout_test (const s_timespec *deadline)
{
  (void) deadline;
  return g_tls_buf_sink->mode == 2 && g_tls_buf_sink->fail_at &&
    g_tls_buf_sink->calls + 1 == g_tls_buf_sink->fail_at;
}

static bool tls_buf_wait_test (t_socket socket, bool write,
                               const s_timespec *deadline)
{
  (void) socket;
  (void) deadline;
  g_tls_buf_sink->waits++;
  g_tls_buf_sink->wait_write = write;
  return false;
}

static ssize_t tls_buf_read_test (struct tls *ctx, void *data,
                                  size_t size)
{
  (void) ctx;
  (void) data;
  (void) size;
  return -1;
}

static const char * tls_buf_error_test (struct tls *ctx)
{
  (void) ctx;
  return "tls_buf test transport error";
}

#define tls_write tls_buf_write_test
#define tls_read tls_buf_read_test
#define tls_error tls_buf_error_test
#define timespec_timeout_expired tls_buf_timeout_test
#define socket_wait tls_buf_wait_test
#define tls_buf_close tls_buf_test_close
#define tls_buf_open_r tls_buf_test_open_r
#define tls_buf_open_r_refill tls_buf_test_open_r_refill
#define tls_buf_open_w tls_buf_test_open_w
#define tls_buf_open_w_flush tls_buf_test_open_w_flush
#include "../libkc3/tls_buf.c"
#undef tls_write
#undef tls_read
#undef tls_error
#undef timespec_timeout_expired
#undef socket_wait
#undef tls_buf_close
#undef tls_buf_open_r
#undef tls_buf_open_r_refill
#undef tls_buf_open_w
#undef tls_buf_open_w_flush

static char tls_buf_partial_error_test (uw progress, uw mode,
                                       bool saved, bool close_retry)
{
  s_buf buf;
  const char data[] = {'A', 0, 'B', 'C', 'D', (char) 0xFF, 'E', 'F'};
  uw done = saved ? 4 : sizeof(data);
  s_buf_save inner;
  s_buf_save outer;
  s_tls_buf_sink sink = {0};
  sink.chunk = 1;
  sink.fail_at = progress + 1;
  sink.mode = mode;
  g_tls_buf_sink = &sink;
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_write(&buf, data, 4), 4);
  if (saved)
    TEST_EQ(buf_save_init(&buf, &outer), &buf);
  TEST_EQ(buf_write(&buf, data + 4, 2), 2);
  if (saved)
    TEST_EQ(buf_save_init(&buf, &inner), &buf);
  TEST_EQ(buf_write(&buf, data + 6, 2), 2);
  TEST_EQ(tls_buf_test_open_w(&buf, (p_tls) &sink), &buf);
  ((s_tls_buf *) buf.user_ptr)->deadline.tv_sec = 1;
  TEST_EQ(buf_flush(&buf), -1);
  TEST_EQ(sink.size, progress);
  TEST_MEMCMP(sink.data, data, progress);
  TEST_EQ(sink.calls, mode == 2 ? progress : progress + 1);
  TEST_EQ(sink.waits, mode >= 3 ? 1 : 0);
  TEST_EQ(sink.wait_write, mode == 4);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, sizeof(data) - progress);
  TEST_MEMCMP(buf.ptr.p_pchar, data + progress, buf.wpos);
  if (saved) {
    TEST_EQ(outer.wpos, 4 - progress);
    TEST_EQ(inner.wpos, 6 - progress);
    TEST_EQ(outer.rpos, 0);
    TEST_EQ(inner.rpos, 0);
    TEST_EQ(buf.save, &inner);
    TEST_EQ(inner.next, &outer);
  }
  sink.fail_at = 0;
  if (close_retry)
    tls_buf_test_close(&buf);
  else
    TEST_EQ(buf_flush(&buf), done - progress);
  TEST_EQ(sink.bad_retry, false);
  TEST_EQ(sink.pending, false);
  TEST_EQ(sink.size, done);
  TEST_MEMCMP(sink.data, data, done);
  TEST_EQ(buf.wpos, sizeof(data) - done);
  TEST_MEMCMP(buf.ptr.p_pchar, data + done, buf.wpos);
  if (saved) {
    TEST_EQ(outer.wpos, 0);
    TEST_EQ(inner.wpos, 2);
    TEST_EQ(buf_save_restore_wpos(&buf, &inner), &buf);
    TEST_EQ(buf.wpos, 2);
    TEST_EQ(buf_save_clean(&buf, &inner), &buf);
    TEST_EQ(buf_save_clean(&buf, &outer), &buf);
    if (! close_retry) {
      TEST_EQ(buf_flush(&buf), 2);
      TEST_EQ(sink.size, 6);
      TEST_MEMCMP(sink.data, data, 6);
      TEST_EQ(buf.wpos, 0);
    }
  }
  if (! close_retry)
    tls_buf_test_close(&buf);
  TEST_EQ(buf.user_ptr, NULL);
  TEST_EQ(buf.flush, NULL);
  buf_clean(&buf);
  g_tls_buf_sink = NULL;
  return 0;
}

static char tls_buf_saved_tail_test (const char *data, uw size,
                                     uw prefix, uw chunk)
{
  s_buf buf;
  uw calls;
  s_buf_save save;
  s_tls_buf_sink sink = {0};
  sink.chunk = chunk;
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_write(&buf, data, prefix), prefix);
  TEST_EQ(buf_save_init(&buf, &save), &buf);
  TEST_EQ(buf_write(&buf, data + prefix, size - prefix), size - prefix);
  TEST_EQ(tls_buf_test_open_w(&buf, (p_tls) &sink), &buf);
  TEST_EQ(buf_flush(&buf), prefix ? prefix : buf.size - size);
  TEST_EQ(sink.size, prefix);
  TEST_MEMCMP(sink.data, data, prefix);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(buf.wpos, size - prefix);
  TEST_EQ(save.rpos, 0);
  TEST_EQ(save.wpos, 0);
  TEST_EQ(buf.save, &save);
  TEST_MEMCMP(buf.ptr.p_pchar, data + prefix, size - prefix);
  calls = sink.calls;
  TEST_EQ(buf_flush(&buf), buf.size - buf.wpos);
  TEST_EQ(sink.calls, calls);
  TEST_EQ(buf_save_clean(&buf, &save), &buf);
  TEST_EQ(buf_flush(&buf), size > prefix ? size - prefix : buf.size);
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(sink.size, size);
  TEST_MEMCMP(sink.data, data, size);
  calls = sink.calls;
  tls_buf_test_close(&buf);
  TEST_EQ(sink.calls, calls);
  TEST_EQ(buf.user_ptr, NULL);
  TEST_EQ(buf.flush, NULL);
  buf_clean(&buf);
  return 0;
}

static char tls_buf_nested_saves_test (bool restore)
{
  s_buf buf;
  s_buf_save inner;
  s_buf_save outer;
  s_tls_buf_sink sink = {0};
  sink.chunk = 1;
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_write(&buf, "AB", 2), 2);
  TEST_EQ(buf_save_init(&buf, &outer), &buf);
  TEST_EQ(buf_write(&buf, "CD", 2), 2);
  TEST_EQ(buf_save_init(&buf, &inner), &buf);
  TEST_EQ(buf_write(&buf, "EF", 2), 2);
  TEST_EQ(tls_buf_test_open_w(&buf, (p_tls) &sink), &buf);
  TEST_EQ(buf_flush(&buf), 2);
  TEST_EQ(sink.size, 2);
  TEST_MEMCMP(sink.data, "AB", 2);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(buf.rpos, 0);
  TEST_MEMCMP(buf.ptr.p_pchar, "CDEF", 4);
  TEST_EQ(outer.wpos, 0);
  TEST_EQ(inner.wpos, 2);
  TEST_EQ(buf.save, &inner);
  TEST_EQ(inner.next, &outer);
  if (restore) {
    TEST_EQ(buf_save_restore_wpos(&buf, &inner), &buf);
    TEST_EQ(buf.wpos, 2);
    TEST_MEMCMP(buf.ptr.p_pchar, "CD", 2);
  }
  TEST_EQ(buf_save_clean(&buf, &inner), &buf);
  TEST_EQ(buf.save, &outer);
  TEST_EQ(buf_flush(&buf), buf.size - buf.wpos);
  TEST_EQ(sink.size, 2);
  TEST_EQ(sink.calls, 2);
  TEST_EQ(buf_save_clean(&buf, &outer), &buf);
  TEST_EQ(buf_flush(&buf), restore ? 2 : 4);
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(sink.size, restore ? 4 : 6);
  TEST_MEMCMP(sink.data, "ABCDEF", sink.size);
  tls_buf_test_close(&buf);
  buf_clean(&buf);
  return 0;
}

void tls_buf_test (void);
TEST_CASE_PROTOTYPE(tls_buf_flush_saved_tail);
TEST_CASE_PROTOTYPE(tls_buf_flush_boundaries);
TEST_CASE_PROTOTYPE(tls_buf_flush_nested_saves);
TEST_CASE_PROTOTYPE(tls_buf_flush_no_save);
TEST_CASE_PROTOTYPE(tls_buf_partial_retry);
TEST_CASE_PROTOTYPE(tls_buf_partial_close);
TEST_CASE_PROTOTYPE(tls_buf_partial_saves);

void tls_buf_test (void)
{
  TEST_CASE_RUN(tls_buf_flush_saved_tail);
  TEST_CASE_RUN(tls_buf_flush_boundaries);
  TEST_CASE_RUN(tls_buf_flush_nested_saves);
  TEST_CASE_RUN(tls_buf_flush_no_save);
  TEST_CASE_RUN(tls_buf_partial_retry);
  TEST_CASE_RUN(tls_buf_partial_close);
  TEST_CASE_RUN(tls_buf_partial_saves);
}

TEST_CASE(tls_buf_partial_retry)
{
  uw mode;
  uw progress;
  for (mode = 0; mode < 5; mode++)
    for (progress = 0; progress < 4; progress++)
      TEST_EQ(tls_buf_partial_error_test(progress, mode, false, false), 0);
}
TEST_CASE_END(tls_buf_partial_retry)

TEST_CASE(tls_buf_partial_close)
{
  uw mode;
  uw progress;
  for (mode = 0; mode < 5; mode++)
    for (progress = 0; progress < 4; progress++)
      TEST_EQ(tls_buf_partial_error_test(progress, mode, false, true), 0);
}
TEST_CASE_END(tls_buf_partial_close)

TEST_CASE(tls_buf_partial_saves)
{
  uw mode;
  uw progress;
  for (mode = 0; mode < 5; mode++)
    for (progress = 0; progress < 4; progress++) {
      TEST_EQ(tls_buf_partial_error_test(progress, mode, true, false), 0);
      TEST_EQ(tls_buf_partial_error_test(progress, mode, true, true), 0);
    }
}
TEST_CASE_END(tls_buf_partial_saves)

TEST_CASE(tls_buf_flush_saved_tail)
{
  TEST_EQ(tls_buf_saved_tail_test("AB", 2, 1, 2), 0);
}
TEST_CASE_END(tls_buf_flush_saved_tail)

TEST_CASE(tls_buf_flush_boundaries)
{
  uw chunk;
  const char data[] = {'A', 0, 'B', (char) 0xFF, 'C', '\n', 'D'};
  uw prefix;
  for (chunk = 1; chunk <= sizeof(data); chunk++)
    for (prefix = 0; prefix <= sizeof(data); prefix++)
      TEST_EQ(tls_buf_saved_tail_test(data, sizeof(data), prefix,
                                      chunk), 0);
}
TEST_CASE_END(tls_buf_flush_boundaries)

TEST_CASE(tls_buf_flush_nested_saves)
{
  TEST_EQ(tls_buf_nested_saves_test(false), 0);
  TEST_EQ(tls_buf_nested_saves_test(true), 0);
}
TEST_CASE_END(tls_buf_flush_nested_saves)

TEST_CASE(tls_buf_flush_no_save)
{
  s_buf buf;
  s_tls_buf_sink sink = {0};
  sink.chunk = 1;
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(tls_buf_test_open_w(&buf, (p_tls) &sink), &buf);
  TEST_EQ(buf_flush(&buf), 16);
  TEST_EQ(sink.calls, 0);
  TEST_EQ(buf_write(&buf, "ABCD", 4), 4);
  TEST_EQ(buf_flush(&buf), 4);
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(sink.size, 4);
  TEST_EQ(sink.calls, 4);
  TEST_MEMCMP(sink.data, "ABCD", 4);
  TEST_EQ(buf_flush(&buf), 16);
  TEST_EQ(sink.calls, 4);
  tls_buf_test_close(&buf);
  buf_clean(&buf);
}
TEST_CASE_END(tls_buf_flush_no_save)
