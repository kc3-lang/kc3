#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "../to_c/to_c.h"
#include "test.h"

static u8 g_to_c_parse_mode;
static uw g_to_c_write_calls;
static uw g_to_c_fail_write;
static sw g_to_c_write_error;
static FILE *g_to_c_input;
static FILE *g_to_c_files[3];
static u8 g_to_c_open_error;
static u8 g_to_c_flush_error;
static u8 g_to_c_close_error;
static bool g_to_c_read_error;
static bool g_to_c_bad_close;
static uw g_to_c_env_cleans;

static sw to_c_parse_test (s_buf *buf, s_tag *tag)
{
  switch (g_to_c_parse_mode) {
  case 1: return 0;
  case 2: return -7;
  case 3:
    return tag_init_1(tag, "defmodule F22 do end") ? 1 : -1;
  }
  return buf_parse_tag(buf, tag);
}

static sw to_c_write_1_test (s_buf *buf, const char *str)
{
  if (++g_to_c_write_calls == g_to_c_fail_write)
    return g_to_c_write_error;
  return buf_write_1(buf, str);
}

static sw to_c_write_str_test (s_buf *buf, const s_str *str)
{
  if (++g_to_c_write_calls == g_to_c_fail_write)
    return g_to_c_write_error;
  return buf_write_str(buf, str);
}

static FILE * to_c_open_test (const s_str *path, const char *mode)
{
  int fd;
  FILE *fp;
  uw slot;
  if (! strcmp(mode, "rb")) {
    slot = 0;
    if (g_to_c_read_error)
      fp = fopen("/dev/null", "wb");
    else {
      if (! g_to_c_input || (fd = dup(fileno(g_to_c_input))) < 0)
        return NULL;
      if (! (fp = fdopen(fd, "rb"))) {
        close(fd);
        return NULL;
      }
      rewind(fp);
    }
  }
  else {
    slot = path->ptr.p_pchar[path->size - 1] == 'c' ? 1 : 2;
    if (g_to_c_open_error == slot)
      return NULL;
    fp = g_to_c_flush_error == slot ? fopen("/dev/null", "rb") :
      tmpfile();
  }
  if (fp)
    g_to_c_files[slot] = fp;
  return fp;
}

static int to_c_fclose_test (FILE *fp)
{
  uw i;
  int r;
  for (i = 0; i < 3; i++)
    if (fp == g_to_c_files[i]) {
      g_to_c_files[i] = NULL;
      r = fclose(fp);
      return g_to_c_close_error == i + 1 ? EOF : r;
    }
  g_to_c_bad_close = true;
  return EOF;
}

#define buf_parse_tag to_c_parse_test
#define buf_write_1 to_c_write_1_test
#define buf_write_str to_c_write_str_test
#define file_open to_c_open_test
#define fclose to_c_fclose_test

#include "../to_c/to_c.c"

#undef buf_parse_tag
#undef buf_write_1
#undef buf_write_str
#undef file_open
#undef fclose

static s_env * to_c_kc3_init_test (s_env *env, int *argc, char ***argv)
{
  (void) env;
  (*argc)--;
  (*argv)++;
  return env_global();
}

static void to_c_kc3_clean_test (s_env *env)
{
  (void) env;
  g_to_c_env_cleans++;
}

#define kc3_init to_c_kc3_init_test
#define kc3_clean to_c_kc3_clean_test
#define main to_c_main_test
#define usage to_c_usage_test

#include "../to_c/kc3_to_c/kc3_to_c.c"

#undef kc3_init
#undef kc3_clean
#undef main
#undef usage

static char to_c_context_test (s_to_c *context, uw size)
{
  to_c_init(context);
  TEST_ASSERT(buf_init_alloc(&context->c_buf, size));
  TEST_ASSERT(buf_init_alloc(&context->h_buf, size));
  TEST_ASSERT(str_init_copy_1(&context->name, "f22"));
  return 0;
}

static char to_c_input_test (const char *text)
{
  uw size = strlen(text);
  TEST_ASSERT((g_to_c_input = tmpfile()));
  TEST_EQ(fwrite(text, 1, size, g_to_c_input), size);
  TEST_EQ(fflush(g_to_c_input), 0);
  rewind(g_to_c_input);
  return 0;
}

static char to_c_finish_test (void)
{
  uw i;
  for (i = 0; i < 3; i++)
    TEST_EQ(g_to_c_files[i], NULL);
  TEST_EQ(g_to_c_bad_close, false);
  if (g_to_c_input) {
    TEST_EQ(fclose(g_to_c_input), 0);
    g_to_c_input = NULL;
  }
  return 0;
}

static char to_c_empty_test (void)
{
  const char *input[] = {"", " \n\t\r", "#", "# trailing comment",
                         "# first\n\n  # last", " # comment\n  \t"};
  s_to_c context;
  s_buf in;
  uw i;
  for (i = 0; i < sizeof(input) / sizeof(*input); i++) {
    TEST_EQ(to_c_context_test(&context, 1024), 0);
    buf_init_1_const(&in, input[i]);
    TEST_EQ(to_c_buf(&context, &in), 0);
    TEST_EQ(in.rpos, in.wpos);
    TEST_EQ(context.c_buf.wpos, 0);
    TEST_EQ(context.h_buf.wpos, 0);
    buf_clean(&in);
    to_c_clean(&context);
  }
  return 0;
}

static char to_c_valid_test (void)
{
  const char expected_c[] =
    "/* This file was generated from KC3. */\n#include \"f22.h\"\n\n";
  const char expected_h[] =
    "/* This file was generated from KC3. */\n"
    "#ifndef KC3_TO_C_DEFMODULE_F22_H\n\n"
    "#endif /* KC3_TO_C_DEFMODULE_F22_H */\n";
  const char *input[] = {"defmodule F22 do end\n",
                         "# first\ndefmodule F22 do end\n# last",
                         "defmodule F22 do end # last\n"};
  s_to_c context;
  s_buf in;
  uw i;
  sw r;
  for (i = 0; i < sizeof(input) / sizeof(*input); i++) {
    TEST_EQ(to_c_context_test(&context, 1024), 0);
    buf_init_1_const(&in, input[i]);
    TEST_ASSERT((r = to_c_buf(&context, &in)) > 0);
    TEST_EQ(r, context.c_buf.wpos + context.h_buf.wpos);
    TEST_EQ(in.rpos, in.wpos);
    TEST_MEM_EQ(context.c_buf.ptr.p_pchar, context.c_buf.wpos,
                 expected_c, sizeof(expected_c) - 1);
    TEST_MEM_EQ(context.h_buf.ptr.p_pchar, context.h_buf.wpos,
                 expected_h, sizeof(expected_h) - 1);
    buf_clean(&in);
    to_c_clean(&context);
  }
  return 0;
}

static char to_c_bad_input_test (const char *input)
{
  s_to_c context;
  s_buf in;
  TEST_EQ(to_c_context_test(&context, 1024), 0);
  buf_init_1_const(&in, input);
  TEST_ASSERT(to_c_buf(&context, &in) < 0);
  buf_clean(&in);
  to_c_clean(&context);
  return 0;
}

static char to_c_no_match_test (void)
{
  TEST_EQ(to_c_bad_input_test(")\n"), 0);
  TEST_EQ(to_c_bad_input_test("]\n"), 0);
  TEST_EQ(to_c_bad_input_test("}\n"), 0);
  return 0;
}

static char to_c_truncated_test (void)
{
  TEST_EQ(to_c_bad_input_test("\"unfinished"), 0);
  TEST_EQ(to_c_bad_input_test("\xC2"), 0);
  TEST_EQ(to_c_bad_input_test("\xFF"), 0);
  return 0;
}

static char to_c_unsupported_test (void)
{
  TEST_EQ(to_c_bad_input_test("1\n"), 0);
  TEST_EQ(to_c_bad_input_test("unknown()\n"), 0);
  TEST_EQ(to_c_bad_input_test("defmodule F22 do 1 end\n"), 0);
  return 0;
}

static char to_c_trailing_test (void)
{
  TEST_EQ(to_c_bad_input_test("defmodule F22 do end\n)\n"), 0);
  TEST_EQ(to_c_bad_input_test("defmodule F22 do end\n\"unfinished"), 0);
  return 0;
}

static char to_c_parse_failure_test (void)
{
  for (g_to_c_parse_mode = 1; g_to_c_parse_mode <= 3;
       g_to_c_parse_mode++) {
    g_to_c_write_calls = 0;
    TEST_EQ(to_c_bad_input_test("input"), 0);
    TEST_EQ(g_to_c_write_calls, 0);
  }
  return 0;
}

typedef struct to_c_source {
  const char *text;
  uw offset;
  sw error;
} s_to_c_source;

static sw to_c_refill_test (s_buf *buf)
{
  s_to_c_source *source = buf->user_ptr;
  if (! source->text[source->offset])
    return source->error;
  if (buf->wpos == buf->size)
    return -1;
  buf->ptr.p_pchar[buf->wpos++] = source->text[source->offset++];
  return 1;
}

static char to_c_stream_test (void)
{
  s_to_c context;
  s_buf in;
  s_to_c_source source;
  char buffer[128];
  const char *input[] = {"", " \n", "# comment", "# comment\n",
                         "defmodule F22 do end\n"};
  uw i;
  uw j;
  sw r;
  for (i = 0; i < sizeof(input) / sizeof(*input); i++)
    for (j = 0; j < 2; j++) {
      TEST_EQ(to_c_context_test(&context, 1024), 0);
      buf_init(&in, false, sizeof(buffer), buffer);
      source = (s_to_c_source) {.text = input[i], .error = j ? -7 : 0};
      in.refill = to_c_refill_test;
      in.user_ptr = &source;
      r = to_c_buf(&context, &in);
      if (j)
        TEST_ASSERT(r < 0);
      else
        TEST_ASSERT(r >= 0);
      TEST_EQ(source.offset, strlen(input[i]));
      buf_clean(&in);
      to_c_clean(&context);
    }
  return 0;
}

static char to_c_write_failure_test (void)
{
  uw i;
  sw r;
  for (i = 1; i <= 9; i++)
    for (r = 0; r >= -7; r -= 7) {
      g_to_c_write_calls = 0;
      g_to_c_fail_write = i;
      g_to_c_write_error = r;
      TEST_EQ(to_c_bad_input_test("defmodule F22 do end\n"), 0);
      TEST_EQ(g_to_c_write_calls, i);
    }
  return 0;
}

static char to_c_read_failure_test (void)
{
  s_to_c context;
  s_str name = STR("input");
  TEST_EQ(to_c_context_test(&context, 1024), 0);
  g_to_c_read_error = true;
  TEST_ASSERT(to_c_file(&context, &name) < 0);
  to_c_clean(&context);
  TEST_EQ(to_c_finish_test(), 0);
  return 0;
}

static int to_c_cli_test (void)
{
  char *argv[] = {"kc3_to_c", "input", "output", NULL};
  g_to_c_env_cleans = 0;
  return to_c_main_test(3, argv);
}

static char to_c_cli_success_test (void)
{
  const char *input[] = {"", "defmodule F22 do end\n# last"};
  uw i;
  for (i = 0; i < sizeof(input) / sizeof(*input); i++) {
    TEST_EQ(to_c_input_test(input[i]), 0);
    TEST_EQ(to_c_cli_test(), 0);
    TEST_EQ(g_to_c_env_cleans, 1);
    TEST_EQ(to_c_finish_test(), 0);
  }
  return 0;
}

static char to_c_cli_invalid_test (void)
{
  const char *input[] = {")\n", "\"unfinished", "1\n",
                         "defmodule F22 do 1 end\n"};
  uw i;
  for (i = 0; i < sizeof(input) / sizeof(*input); i++) {
    TEST_EQ(to_c_input_test(input[i]), 0);
    TEST_ASSERT(to_c_cli_test() != 0);
    TEST_EQ(g_to_c_env_cleans, 1);
    TEST_EQ(to_c_finish_test(), 0);
  }
  g_to_c_read_error = true;
  TEST_ASSERT(to_c_cli_test() != 0);
  TEST_EQ(to_c_finish_test(), 0);
  return 0;
}

static char to_c_cli_io_failure_test (void)
{
  u8 i;
  TEST_EQ(to_c_input_test("defmodule F22 do end\n"), 0);
  for (i = 1; i <= 2; i++) {
    g_to_c_flush_error = i;
    TEST_ASSERT(to_c_cli_test() != 0);
    TEST_EQ(g_to_c_env_cleans, 1);
    TEST_EQ(g_to_c_files[0], NULL);
    TEST_EQ(g_to_c_files[1], NULL);
    TEST_EQ(g_to_c_files[2], NULL);
  }
  g_to_c_flush_error = 0;
  for (i = 1; i <= 3; i++) {
    g_to_c_close_error = i;
    TEST_ASSERT(to_c_cli_test() != 0);
    TEST_EQ(g_to_c_env_cleans, 1);
    TEST_EQ(g_to_c_files[0], NULL);
    TEST_EQ(g_to_c_files[1], NULL);
    TEST_EQ(g_to_c_files[2], NULL);
  }
  g_to_c_close_error = 0;
  for (i = 1; i <= 2; i++) {
    g_to_c_open_error = i;
    TEST_ASSERT(to_c_cli_test() != 0);
    TEST_EQ(g_to_c_env_cleans, 1);
  }
  TEST_EQ(to_c_finish_test(), 0);
  return 0;
}

static int to_c_child_test (char (*test)(void))
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

#define TO_C_CASE(name)                              \
  TEST_CASE(to_c_ ## name)                           \
  {                                                 \
    TEST_EQ(to_c_child_test(to_c_ ## name ## _test), 0); \
  }                                                 \
  TEST_CASE_END(to_c_ ## name)

TO_C_CASE(empty)
TO_C_CASE(valid)
TO_C_CASE(no_match)
TO_C_CASE(truncated)
TO_C_CASE(unsupported)
TO_C_CASE(trailing)
TO_C_CASE(parse_failure)
TO_C_CASE(stream)
TO_C_CASE(write_failure)
TO_C_CASE(read_failure)
TO_C_CASE(cli_success)
TO_C_CASE(cli_invalid)
TO_C_CASE(cli_io_failure)

void to_c_test (void)
{
  TEST_CASE_RUN(to_c_empty);
  TEST_CASE_RUN(to_c_valid);
  TEST_CASE_RUN(to_c_no_match);
  TEST_CASE_RUN(to_c_truncated);
  TEST_CASE_RUN(to_c_unsupported);
  TEST_CASE_RUN(to_c_trailing);
  TEST_CASE_RUN(to_c_parse_failure);
  TEST_CASE_RUN(to_c_stream);
  TEST_CASE_RUN(to_c_write_failure);
  TEST_CASE_RUN(to_c_read_failure);
  TEST_CASE_RUN(to_c_cli_success);
  TEST_CASE_RUN(to_c_cli_invalid);
  TEST_CASE_RUN(to_c_cli_io_failure);
}
