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
#include <errno.h>
#include <stdatomic.h>
#include <string.h>
#if ! (defined(WIN32) || defined(WIN64))
# include <fcntl.h>
# include <unistd.h>
#endif
#include "daemon.h"
#include "io.h"

static atomic_bool g_daemon_stopping;
static s32         g_daemon_stop_pipe[2] = {-1, -1};

void daemon_stop (void)
{
  atomic_store_explicit(&g_daemon_stopping, true,
                        memory_order_relaxed);
}

void daemon_stop_clean (void)
{
#if ! (defined(WIN32) || defined(WIN64))
  if (g_daemon_stop_pipe[0] >= 0)
    close(g_daemon_stop_pipe[0]);
  if (g_daemon_stop_pipe[1] >= 0)
    close(g_daemon_stop_pipe[1]);
#endif
  g_daemon_stop_pipe[0] = -1;
  g_daemon_stop_pipe[1] = -1;
}

s64 daemon_stop_fd (void)
{
  return g_daemon_stop_pipe[0];
}

bool daemon_stop_init (void)
{
#if ! (defined(WIN32) || defined(WIN64))
  s32 e;
  s32 flags;
#endif
  atomic_store_explicit(&g_daemon_stopping, false,
                        memory_order_relaxed);
  daemon_stop_clean();
#if ! (defined(WIN32) || defined(WIN64))
  if (pipe(g_daemon_stop_pipe)) {
    e = errno;
    err_write_1("daemon_stop_init: pipe: ");
    err_puts(strerror(e));
    return false;
  }
  if ((flags = fcntl(g_daemon_stop_pipe[0], F_GETFL)) < 0 ||
      fcntl(g_daemon_stop_pipe[0], F_SETFL,
            flags | O_NONBLOCK) < 0 ||
      (flags = fcntl(g_daemon_stop_pipe[1], F_GETFL)) < 0 ||
      fcntl(g_daemon_stop_pipe[1], F_SETFL,
            flags | O_NONBLOCK) < 0 ||
      (flags = fcntl(g_daemon_stop_pipe[0], F_GETFD)) < 0 ||
      fcntl(g_daemon_stop_pipe[0], F_SETFD,
            flags | FD_CLOEXEC) < 0 ||
      (flags = fcntl(g_daemon_stop_pipe[1], F_GETFD)) < 0 ||
      fcntl(g_daemon_stop_pipe[1], F_SETFD,
            flags | FD_CLOEXEC) < 0) {
    e = errno;
    daemon_stop_clean();
    errno = e;
    err_write_1("daemon_stop_init: fcntl: ");
    err_puts(strerror(e));
    return false;
  }
#endif
  return true;
}

void daemon_stop_notify (s32 signal)
{
#if ! (defined(WIN32) || defined(WIN64))
  s32 e;
  u8 notification;
  e = errno;
  notification = (u8) signal;
  if (g_daemon_stop_pipe[1] >= 0) {
    ssize_t written;
    written = write(g_daemon_stop_pipe[1], &notification,
                    sizeof(notification));
    (void) written;
  }
  errno = e;
#else
  (void) signal;
#endif
}

bool daemon_stopping (void)
{
  return atomic_load_explicit(&g_daemon_stopping,
                              memory_order_relaxed);
}
