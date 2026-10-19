#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/buf.h"
#include "../libkc3/buf_fd.h"
#include "test.h"

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

void buf_fd_test (void)
{
  TEST_CASE_RUN(buf_fd_closed_peer);
  TEST_CASE_RUN(buf_fd_shutdown_write);
  TEST_CASE_RUN(buf_fd_socket_flush);
  TEST_CASE_RUN(buf_fd_pipe_flush);
  TEST_CASE_RUN(buf_fd_file_flush);
}

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
