#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/buf.h"
#include "../libkc3/buf_fd.h"
#include "../libkc3/buf_save.h"
#include "test.h"

typedef struct buf_fd_sink {
  char data[64];
  uw size;
  uw calls;
  uw fail_at;
  uw sends;
  uw writes;
  uw polls;
  bool file;
  int error;
  int poll_result;
} s_buf_fd_sink;

static s_buf_fd_sink *g_buf_fd_sink;

static ssize_t buf_fd_sink_write (const void *data, size_t size)
{
  s_buf_fd_sink *sink = g_buf_fd_sink;
  if (++sink->calls == sink->fail_at) {
    errno = sink->error;
    return -1;
  }
  if (! size || sink->size == sizeof(sink->data)) {
    errno = EIO;
    return -1;
  }
  memcpy(sink->data + sink->size, data, 1);
  sink->size++;
  return 1;
}

static ssize_t buf_fd_send_test (int fd, const void *data, size_t size,
                                int flags)
{
  (void) fd;
  (void) flags;
  g_buf_fd_sink->sends++;
  if (g_buf_fd_sink->file) {
    errno = ENOTSOCK;
    return -1;
  }
  return buf_fd_sink_write(data, size);
}

static ssize_t buf_fd_write_test (int fd, const void *data, size_t size)
{
  (void) fd;
  g_buf_fd_sink->writes++;
  return buf_fd_sink_write(data, size);
}

static int buf_fd_poll_test (struct pollfd *fds, nfds_t count,
                            int timeout)
{
  (void) fds;
  (void) count;
  (void) timeout;
  g_buf_fd_sink->polls++;
  errno = EINTR;
  return g_buf_fd_sink->poll_result;
}

void buf_fd_test_close (s_buf *buf);
s_buf * buf_fd_test_open_r (s_buf *buf, t_fd fd);
s_buf * buf_fd_test_open_w (s_buf *buf, t_fd fd);

#define send buf_fd_send_test
#define write buf_fd_write_test
#define poll buf_fd_poll_test
#define buf_fd_close buf_fd_test_close
#define buf_fd_open_r buf_fd_test_open_r
#define buf_fd_open_r_refill buf_fd_test_open_r_refill
#define buf_fd_open_r_seek buf_fd_test_open_r_seek
#define buf_fd_open_r_tell buf_fd_test_open_r_tell
#define buf_fd_open_w buf_fd_test_open_w
#define buf_fd_open_w_flush buf_fd_test_open_w_flush
#define buf_fd_open_w_seek buf_fd_test_open_w_seek
#define buf_fd_open_w_tell buf_fd_test_open_w_tell
#include "../libkc3/buf_fd.c"
#undef send
#undef write
#undef poll
#undef buf_fd_close
#undef buf_fd_open_r
#undef buf_fd_open_r_refill
#undef buf_fd_open_r_seek
#undef buf_fd_open_r_tell
#undef buf_fd_open_w
#undef buf_fd_open_w_flush
#undef buf_fd_open_w_seek
#undef buf_fd_open_w_tell

static char buf_fd_partial_error_test (uw progress, uw mode,
                                      bool saved, bool close_retry)
{
  s_buf buf;
  const char data[] = {'A', 0, 'B', 'C', 'D', (char) 0xFF, 'E', 'F'};
  uw done = saved ? 4 : sizeof(data);
  s_buf_save inner;
  s_buf_save outer;
  s_buf_fd_sink sink = {0};
  sink.fail_at = progress + 1;
  sink.file = mode == 1;
  sink.error = mode < 2 ? EIO : EAGAIN;
  sink.poll_result = mode == 3 ? -1 : 0;
  g_buf_fd_sink = &sink;
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_write(&buf, data, 4), 4);
  if (saved)
    TEST_EQ(buf_save_init(&buf, &outer), &buf);
  TEST_EQ(buf_write(&buf, data + 4, 2), 2);
  if (saved)
    TEST_EQ(buf_save_init(&buf, &inner), &buf);
  TEST_EQ(buf_write(&buf, data + 6, 2), 2);
  TEST_EQ(buf_fd_test_open_w(&buf, 123), &buf);
  TEST_EQ(buf_flush(&buf), -1);
  TEST_EQ(sink.size, progress);
  TEST_MEMCMP(sink.data, data, progress);
  TEST_EQ(sink.calls, progress + 1);
  TEST_EQ(sink.sends, sink.calls);
  TEST_EQ(sink.writes, sink.file ? sink.calls : 0);
  TEST_EQ(sink.polls, mode >= 2 ? 1 : 0);
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
    buf_fd_test_close(&buf);
  else
    TEST_EQ(buf_flush(&buf), done - progress);
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
    buf_fd_test_close(&buf);
  TEST_EQ(buf.user_ptr, NULL);
  TEST_EQ(buf.flush, NULL);
  buf_clean(&buf);
  g_buf_fd_sink = NULL;
  return 0;
}

static int buf_fd_child_test (char (*test)(bool), bool arg)
{
  pid_t child;
  pid_t waited;
  int status;
  if ((child = fork()) < 0)
    return -1;
  if (! child) {
    alarm(10);
    _exit(test(arg));
  }
  do {
    waited = waitpid(child, &status, 0);
  } while (waited < 0 && errno == EINTR);
  if (waited != child)
    return -1;
  if (WIFSIGNALED(status))
    return -WTERMSIG(status);
  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

static char buf_fd_socket_error_test (bool shutdown_write)
{
  struct sigaction action = {0};
  s_buf buf;
  int fd[2];
  sigset_t mask;
  action.sa_handler = SIG_DFL;
  TEST_EQ(sigemptyset(&action.sa_mask), 0);
  TEST_EQ(sigaction(SIGPIPE, &action, NULL), 0);
  TEST_EQ(sigemptyset(&mask), 0);
  TEST_EQ(sigaddset(&mask, SIGPIPE), 0);
  TEST_EQ(sigprocmask(SIG_UNBLOCK, &mask, NULL), 0);
  TEST_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);
  if (shutdown_write)
    TEST_EQ(shutdown(fd[0], SHUT_WR), 0);
  else
    TEST_EQ(close(fd[1]), 0);
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_fd_open_w(&buf, fd[0]), &buf);
  TEST_EQ(buf_write_1(&buf, "record"), 6);
  TEST_EQ(buf_flush(&buf), -1);
  TEST_EQ(buf.wpos, 6);
  TEST_EQ(buf.rpos, 0);
  TEST_MEMCMP(buf.ptr.p_pchar, "record", 6);
  buf_fd_close(&buf);
  TEST_EQ(buf.user_ptr, NULL);
  TEST_EQ(buf.flush, NULL);
  buf_clean(&buf);
  TEST_EQ(close(fd[0]), 0);
  if (shutdown_write)
    TEST_EQ(close(fd[1]), 0);
  TEST_EQ(sigaction(SIGPIPE, NULL, &action), 0);
  TEST_ASSERT(action.sa_handler == SIG_DFL);
  TEST_EQ(sigprocmask(SIG_SETMASK, NULL, &mask), 0);
  TEST_EQ(sigismember(&mask, SIGPIPE), 0);
  return 0;
}

static char buf_fd_transfer_test (bool socket)
{
  s_buf buf;
  const char data[] = {'A', 0, 'B', '\n', (char) 0xFF};
  int fd[2];
  char received[sizeof(data)];
  if (socket)
    TEST_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);
  else
    TEST_EQ(pipe(fd), 0);
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_fd_open_w(&buf, fd[1]), &buf);
  TEST_EQ(buf_write(&buf, data, sizeof(data)), sizeof(data));
  TEST_EQ(buf_flush(&buf), sizeof(data));
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(buf.rpos, 0);
  TEST_EQ(read(fd[0], received, sizeof(received)), sizeof(received));
  TEST_MEMCMP(received, data, sizeof(data));
  TEST_EQ(buf_flush(&buf), buf.size);
  buf_fd_close(&buf);
  buf_clean(&buf);
  TEST_EQ(close(fd[1]), 0);
  TEST_EQ(read(fd[0], received, sizeof(received)), 0);
  TEST_EQ(close(fd[0]), 0);
  return 0;
}

void buf_fd_test (void);
TEST_CASE_PROTOTYPE(buf_fd_closed_peer);
TEST_CASE_PROTOTYPE(buf_fd_shutdown_write);
TEST_CASE_PROTOTYPE(buf_fd_socket_flush);
TEST_CASE_PROTOTYPE(buf_fd_pipe_flush);
TEST_CASE_PROTOTYPE(buf_fd_file_flush);
TEST_CASE_PROTOTYPE(buf_fd_read_tell);
TEST_CASE_PROTOTYPE(buf_fd_partial_retry);
TEST_CASE_PROTOTYPE(buf_fd_partial_close);
TEST_CASE_PROTOTYPE(buf_fd_partial_saves);

void buf_fd_test (void)
{
  TEST_CASE_RUN(buf_fd_closed_peer);
  TEST_CASE_RUN(buf_fd_shutdown_write);
  TEST_CASE_RUN(buf_fd_socket_flush);
  TEST_CASE_RUN(buf_fd_pipe_flush);
  TEST_CASE_RUN(buf_fd_file_flush);
  TEST_CASE_RUN(buf_fd_read_tell);
  TEST_CASE_RUN(buf_fd_partial_retry);
  TEST_CASE_RUN(buf_fd_partial_close);
  TEST_CASE_RUN(buf_fd_partial_saves);
}

TEST_CASE(buf_fd_partial_retry)
{
  uw mode;
  uw progress;
  for (mode = 0; mode < 4; mode++)
    for (progress = 0; progress < 4; progress++)
      TEST_EQ(buf_fd_partial_error_test(progress, mode, false, false), 0);
}
TEST_CASE_END(buf_fd_partial_retry)

TEST_CASE(buf_fd_partial_close)
{
  uw mode;
  uw progress;
  for (mode = 0; mode < 4; mode++)
    for (progress = 0; progress < 4; progress++)
      TEST_EQ(buf_fd_partial_error_test(progress, mode, false, true), 0);
}
TEST_CASE_END(buf_fd_partial_close)

TEST_CASE(buf_fd_partial_saves)
{
  uw mode;
  uw progress;
  for (mode = 0; mode < 4; mode++)
    for (progress = 0; progress < 4; progress++) {
      TEST_EQ(buf_fd_partial_error_test(progress, mode, true, false), 0);
      TEST_EQ(buf_fd_partial_error_test(progress, mode, true, true), 0);
    }
}
TEST_CASE_END(buf_fd_partial_saves)

TEST_CASE(buf_fd_closed_peer)
{
  TEST_EQ(buf_fd_child_test(buf_fd_socket_error_test, false), 0);
}
TEST_CASE_END(buf_fd_closed_peer)

TEST_CASE(buf_fd_shutdown_write)
{
  TEST_EQ(buf_fd_child_test(buf_fd_socket_error_test, true), 0);
}
TEST_CASE_END(buf_fd_shutdown_write)

TEST_CASE(buf_fd_socket_flush)
{
  TEST_EQ(buf_fd_child_test(buf_fd_transfer_test, true), 0);
}
TEST_CASE_END(buf_fd_socket_flush)

TEST_CASE(buf_fd_pipe_flush)
{
  TEST_EQ(buf_fd_child_test(buf_fd_transfer_test, false), 0);
}
TEST_CASE_END(buf_fd_pipe_flush)

TEST_CASE(buf_fd_file_flush)
{
  s_buf buf;
  const char data[] = {'A', 0, 'B', '\n', (char) 0xFF};
  int fd;
  FILE *fp;
  char received[sizeof(data)];
  TEST_ASSERT((fp = tmpfile()));
  TEST_ASSERT((fd = fileno(fp)) >= 0);
  TEST_ASSERT(buf_init_alloc(&buf, 16));
  TEST_EQ(buf_fd_open_w(&buf, fd), &buf);
  TEST_EQ(buf_write(&buf, data, sizeof(data)), sizeof(data));
  TEST_EQ(buf_flush(&buf), sizeof(data));
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(lseek(fd, 0, SEEK_SET), 0);
  TEST_EQ(read(fd, received, sizeof(received)), sizeof(received));
  TEST_MEMCMP(received, data, sizeof(data));
  TEST_EQ(read(fd, received, sizeof(received)), 0);
  buf_fd_close(&buf);
  buf_clean(&buf);
  TEST_EQ(fclose(fp), 0);
}
TEST_CASE_END(buf_fd_file_flush)

TEST_CASE(buf_fd_read_tell)
{
  s_buf buf;
  const char data[] = "0123456789abcdefghijklmn";
  int fd;
  FILE *fp;
  uw i;
  u64 position;
  s_buf_save save;
  uw start;
  u8 value;
  TEST_ASSERT((fp = tmpfile()));
  TEST_ASSERT((fd = fileno(fp)) >= 0);
  TEST_EQ(write(fd, data, sizeof(data) - 1), sizeof(data) - 1);
  for (start = 0; start <= 3; start += 3) {
    TEST_EQ(lseek(fd, start, SEEK_SET), start);
    TEST_ASSERT(buf_init_alloc(&buf, 8));
    TEST_EQ(buf_fd_open_r(&buf, fd), &buf);
    for (i = start; i < sizeof(data) - 1; i++) {
      TEST_EQ(buf_tell_r(&buf, &position), &position);
      TEST_EQ(position, i);
      TEST_EQ(buf_read_u8(&buf, &value), 1);
      TEST_EQ(value, data[i]);
      TEST_EQ(buf_tell_r(&buf, &position), &position);
      TEST_EQ(position, i + 1);
    }
    TEST_ASSERT(buf_read_u8(&buf, &value) <= 0);
    TEST_EQ(buf_tell_r(&buf, &position), &position);
    TEST_EQ(position, sizeof(data) - 1);
    buf_fd_close(&buf);
    buf_clean(&buf);
  }
  TEST_EQ(lseek(fd, 0, SEEK_SET), 0);
  TEST_ASSERT(buf_init_alloc(&buf, 8));
  TEST_EQ(buf_fd_open_r(&buf, fd), &buf);
  TEST_EQ(buf_ignore(&buf, 6), 6);
  TEST_EQ(buf_save_init(&buf, &save), &buf);
  TEST_EQ(buf_ignore(&buf, 4), 4);
  TEST_EQ(save.rpos, 0);
  TEST_EQ(buf_tell_r(&buf, &position), &position);
  TEST_EQ(position, 10);
  TEST_EQ(buf_save_restore_rpos(&buf, &save), &buf);
  TEST_EQ(buf_tell_r(&buf, &position), &position);
  TEST_EQ(position, 6);
  TEST_EQ(buf_read_u8(&buf, &value), 1);
  TEST_EQ(value, data[6]);
  TEST_EQ(buf_tell_r(&buf, &position), &position);
  TEST_EQ(position, 7);
  TEST_EQ(buf_save_clean(&buf, &save), &buf);
  buf_fd_close(&buf);
  buf_clean(&buf);
  TEST_EQ(fclose(fp), 0);
}
TEST_CASE_END(buf_fd_read_tell)
