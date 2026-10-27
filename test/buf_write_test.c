#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "test.h"

typedef struct buf_write_sink {
  u8 data[128];
  uw size;
  uw chunk;
  uw calls;
  sw result;
} s_buf_write_sink;

static sw buf_write_flush_test (s_buf *buf)
{
  s_buf_write_sink *sink = buf->user_ptr;
  uw size = buf->wpos;
  sink->calls++;
  if (sink->calls > 64)
    return -99;
  if (sink->result < 0)
    return sink->result;
  if (size > sink->chunk)
    size = sink->chunk;
  if (size > sizeof(sink->data) - sink->size)
    return -1;
  memcpy(sink->data + sink->size, buf->ptr.p_pvoid, size);
  sink->size += size;
  memmove(buf->ptr.p_pvoid, buf->ptr.p_pu8 + size,
           buf->wpos - size);
  buf->wpos -= size;
  return sink->result;
}

static int buf_write_child (char (*test)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(3);
    _exit(test());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

static char buf_write_full (void)
{
  s_buf buf;
  u8 data[18];
  memset(data, 0xA5, sizeof(data));
  TEST_ASSERT(buf_init(&buf, false, 16, (char *) data + 1));
  TEST_EQ(buf_write(&buf, "0123456789abcdef", 16), 16);
  TEST_EQ(buf_write(&buf, "x", 1), -1);
  TEST_EQ(buf.wpos, 16);
  TEST_MEM_EQ(data + 1, 16, "0123456789abcdef", 16);
  TEST_EQ(data[0], 0xA5);
  TEST_EQ(data[17], 0xA5);
  buf_clean(&buf);
  return 0;
}

static char buf_write_zero (void)
{
  s_buf buf;
  TEST_ASSERT(buf_init(&buf, false, 0, NULL));
  TEST_EQ(buf_write(&buf, NULL, 0), 0);
  TEST_EQ(buf_write(&buf, "x", 1), -1);
  TEST_EQ(buf.wpos, 0);
  buf_clean(&buf);
  return 0;
}

static char buf_write_fixed (void)
{
  s_buf buf;
  u8 data[6];
  memset(data, 0xA5, sizeof(data));
  TEST_ASSERT(buf_init(&buf, false, 4, (char *) data + 1));
  TEST_EQ(buf_write(&buf, "123456789", 9), -1);
  TEST_ASSERT(buf.wpos <= buf.size);
  TEST_MEM_EQ(data + 1, buf.wpos, "1234", buf.wpos);
  TEST_EQ(data[0], 0xA5);
  TEST_EQ(data[5], 0xA5);
  buf_clean(&buf);
  return 0;
}

static char buf_write_stall (void)
{
  s_buf buf;
  char data[4];
  uw i;
  s_buf_write_sink sink;
  for (i = 0; i < 2; i++) {
    sink = (s_buf_write_sink) {.result = i ? 17 : 0};
    TEST_ASSERT(buf_init(&buf, false, sizeof(data), data));
    buf.flush = buf_write_flush_test;
    buf.user_ptr = &sink;
    TEST_EQ(buf_write(&buf, "abcd", 4), 4);
    TEST_EQ(buf_write(&buf, "e", 1), -1);
    TEST_EQ(sink.calls, 1);
    TEST_EQ(buf.wpos, 4);
    TEST_MEM_EQ(data, 4, "abcd", 4);
    buf_clean(&buf);
  }
  return 0;
}

static char buf_write_partial (void)
{
  s_buf buf;
  u8 data[10];
  const char input[] = "0123456789abcdef0123456789abcdef";
  uw i;
  s_buf_write_sink sink;
  for (i = 0; i < 2; i++) {
    sink = (s_buf_write_sink) {.chunk = 3, .result = i ? 3 : 0};
    memset(data, 0xA5, sizeof(data));
    TEST_ASSERT(buf_init(&buf, false, 8, (char *) data + 1));
    buf.flush = buf_write_flush_test;
    buf.user_ptr = &sink;
    TEST_EQ(buf_write(&buf, "pre", 3), 3);
    TEST_EQ(buf_write(&buf, input, sizeof(input) - 1),
             sizeof(input) - 1);
    TEST_ASSERT(buf.wpos <= buf.size);
    while (buf.wpos)
      TEST_ASSERT(buf_flush(&buf) >= 0);
    TEST_EQ(sink.size, 3 + sizeof(input) - 1);
    TEST_MEM_EQ(sink.data, 3, "pre", 3);
    TEST_MEM_EQ(sink.data + 3, sizeof(input) - 1,
                 input, sizeof(input) - 1);
    TEST_EQ(data[0], 0xA5);
    TEST_EQ(data[9], 0xA5);
    buf_clean(&buf);
  }
  return 0;
}

static char buf_write_error (void)
{
  s_buf buf;
  char data[4];
  s_buf_write_sink sink = {.result = -7};
  TEST_ASSERT(buf_init(&buf, false, sizeof(data), data));
  buf.flush = buf_write_flush_test;
  buf.user_ptr = &sink;
  TEST_EQ(buf_write(&buf, "abcd", 4), 4);
  TEST_EQ(sink.calls, 0);
  TEST_EQ(buf_write(&buf, "e", 1), -7);
  TEST_EQ(sink.calls, 1);
  TEST_EQ(buf.wpos, 4);
  TEST_MEM_EQ(data, 4, "abcd", 4);
  TEST_EQ(buf_write(&buf, NULL, 0), 0);
  TEST_EQ(sink.calls, 1);
  TEST_EQ(buf_write(&buf, "x", (uw) SW_MAX + 1), -1);
  TEST_EQ(sink.calls, 1);
  buf_clean(&buf);
  return 0;
}

static char buf_write_stream (void)
{
  s_buf buf;
  u8 data[18];
  u8 input[33];
  uw capacity;
  uw chunk;
  uw initial;
  uw length;
  uw i;
  s_buf_write_sink sink;
  for (i = 0; i < sizeof(input); i++)
    input[i] = i;
  for (capacity = 1; capacity <= 16; capacity *= 4)
    for (chunk = 1; chunk <= capacity; chunk++)
      for (initial = 0; initial <= capacity; initial++)
        for (length = 0; length <= sizeof(input); length++) {
          sink = (s_buf_write_sink) {.chunk = chunk};
          memset(data, 0xA5, sizeof(data));
          TEST_ASSERT(buf_init(&buf, false, capacity,
                                (char *) data + 1));
          buf.flush = buf_write_flush_test;
          buf.user_ptr = &sink;
          TEST_EQ(buf_write(&buf, input, initial), initial);
          TEST_EQ(buf_write(&buf, input, length), length);
          TEST_ASSERT(buf.wpos <= buf.size);
          while (buf.wpos)
            TEST_ASSERT(buf_flush(&buf) >= 0);
          TEST_EQ(sink.size, initial + length);
          TEST_MEM_EQ(sink.data, initial, input, initial);
          TEST_MEM_EQ(sink.data + initial, length, input, length);
          TEST_EQ(data[0], 0xA5);
          TEST_EQ(data[capacity + 1], 0xA5);
          buf_clean(&buf);
        }
  return 0;
}

static char buf_write_saved (void)
{
  s_buf buf;
  s_buf_save inner;
  s_buf_save save;
  char data[4];
  char readback[8];
  FILE *fp;
  TEST_ASSERT((fp = tmpfile()));
  TEST_ASSERT(buf_init(&buf, false, sizeof(data), data));
  TEST_ASSERT(buf_file_open_w(&buf, fp));
  TEST_ASSERT(buf_save_init(&buf, &save));
  TEST_EQ(buf_write(&buf, "abcd", 4), 4);
  TEST_EQ(buf_write(&buf, "e", 1), -1);
  TEST_EQ(buf.wpos, 4);
  TEST_EQ(save.wpos, 0);
  TEST_EQ(ftell(fp), 0);
  TEST_MEM_EQ(data, 4, "abcd", 4);
  buf_save_clean(&buf, &save);
  TEST_ASSERT(buf_flush(&buf) >= 0);
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(buf_write(&buf, "A", 1), 1);
  TEST_ASSERT(buf_save_init(&buf, &save));
  TEST_EQ(buf_write(&buf, "bc", 2), 2);
  TEST_ASSERT(buf_save_init(&buf, &inner));
  TEST_EQ(buf_write(&buf, "d", 1), 1);
  TEST_EQ(buf_flush(&buf), 1);
  TEST_EQ(buf.wpos, 3);
  TEST_EQ(save.wpos, 0);
  TEST_EQ(inner.wpos, 2);
  TEST_EQ(buf.save, &inner);
  TEST_EQ(inner.next, &save);
  TEST_EQ(ftell(fp), 5);
  TEST_MEM_EQ(data, 3, "bcd", 3);
  TEST_ASSERT(buf_flush(&buf) >= 0);
  TEST_EQ(buf.wpos, 3);
  TEST_EQ(ftell(fp), 5);
  TEST_MEM_EQ(data, 3, "bcd", 3);
  TEST_ASSERT(buf_save_restore_wpos(&buf, &inner));
  TEST_EQ(buf.wpos, 2);
  buf_save_clean(&buf, &inner);
  TEST_EQ(buf_write(&buf, "E", 1), 1);
  buf_save_clean(&buf, &save);
  TEST_EQ(buf_flush(&buf), 3);
  TEST_EQ(buf.wpos, 0);
  TEST_EQ(buf.save, NULL);
  rewind(fp);
  TEST_EQ(fread(readback, 1, sizeof(readback), fp), sizeof(readback));
  TEST_MEM_EQ(readback, 8, "abcdAbcE", 8);
  buf_file_close(&buf);
  TEST_EQ(fclose(fp), 0);
  buf_clean(&buf);
  return 0;
}

#define BUF_WRITE_CASE(name)                   \
  TEST_CASE(buf_write_ ## name)                \
  {                                           \
    TEST_EQ(buf_write_child(buf_write_ ## name), 0); \
  }                                           \
  TEST_CASE_END(buf_write_ ## name)

BUF_WRITE_CASE(full)
BUF_WRITE_CASE(zero)
BUF_WRITE_CASE(fixed)
BUF_WRITE_CASE(stall)
BUF_WRITE_CASE(partial)
BUF_WRITE_CASE(error)
BUF_WRITE_CASE(stream)
BUF_WRITE_CASE(saved)

void buf_write_test (void)
{
  TEST_CASE_RUN(buf_write_full);
  TEST_CASE_RUN(buf_write_zero);
  TEST_CASE_RUN(buf_write_fixed);
  TEST_CASE_RUN(buf_write_stall);
  TEST_CASE_RUN(buf_write_partial);
  TEST_CASE_RUN(buf_write_error);
  TEST_CASE_RUN(buf_write_stream);
  TEST_CASE_RUN(buf_write_saved);
}
