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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../libkc3/kc3.h"

static const char *g_action_str[] = {" ADD ", " REMOVE ", " REPLACE "};

static sw usage (const char *argv0);

static sw kc3txt_buf_inspect_log (s_buf *out, uw id, u8 action, s_fact *fact)
{
  sw r;
  sw result = 0;
  assert(out);
  if ((r = buf_inspect_uw_decimal(out, id)) <= 0)
    return r;
  result += r;
  if (action >= 3) {
    err_puts("kc3txt_buf_inspect_log: unknown action");
    return -1;
  }
  if ((r = buf_write_1(out, g_action_str[action])) <= 0)
    return r;
  result += r;
  if ((r = buf_inspect_fact(out, fact)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(out, "\n")) <= 0)
    return r;
  result += r;
  return result;
}

static sw kc3txt_from_facts (const char *file_path)
{
  u8 action;
  char b[BUF_SIZE];
  s_fact fact;
  uw id;
  s_buf out;
  s_str path;
  str_init_1(&path, NULL, file_path);
  s_marshall_read mr = {0};
  sw r;
  if (! marshall_read_init_file(&mr, &path))
    return 0;
  buf_init(&out, false, sizeof(b), b);
  buf_file_open_w(&out, stdout);
  while (1) {
    while ((r = buf_peek_1(mr.buf, "KC3MARSH")) == 0) {
      if (buf_peek_1(mr.buf, "_KC3UW_") <= 0) goto done;
      if (! marshall_read_uw(&mr, false, &id) ||
          ! marshall_read_u8(&mr, false, &action) ||
          ! marshall_read_fact(&mr, false, &fact))
        goto done;
      kc3txt_buf_inspect_log(&out, id, action, &fact);
      fact_clean_all(&fact);
    }
    if (r > 0) {
      marshall_read_chunk_file(&mr);
      continue;
    }
    break;
  }
 done:
  buf_flush(&out);
  buf_file_close(&out);
  marshall_read_clean(&mr);
  str_clean(&path);
  return 0;
}

static sw kc3txt_from_text (const char *file_path)
{
  e_fact_action action = -1;
  bool b;
  s_facts *db = NULL;
  s_fact fact;
  FILE *fp = NULL;
  uw id;
  s_buf in;
  char  in_buf[BUF_SIZE];
  s_str in_path;
  s_buf out;
  char  out_buf[BUF_SIZE];
  s_str path;
  sw r;
  str_init_1(&path, NULL, file_path);
  buf_init(&in, false, sizeof(in_buf), in_buf);
  str_init_1(&in_path, NULL, file_path);
  if (! (fp = file_open(&in_path, "rb")))
    return 1;
  if (! pfacts_init(&db))
    goto ko;
  buf_init(&out, false, sizeof(out_buf), out_buf);
  buf_file_open_w(&out, stdout);
  while (1) {
    if ((r = buf_parse_uw(&in, &id)) <= 0)
      return r;
    if ((r = buf_read_1(&in, g_action_str[FACT_ACTION_ADD])) < 0)
      return r;
    if (r) {
      action = FACT_ACTION_ADD;
      goto ok;
    }
    if ((r = buf_read_1(&in, g_action_str[FACT_ACTION_REMOVE])) < 0)
      return r;
    if (r) {
      action = FACT_ACTION_REMOVE;
      goto ok;
    }
    if ((r = buf_read_1(&in, g_action_str[FACT_ACTION_REPLACE])) < 0)
      return r;
    if (r) {
      action = FACT_ACTION_REPLACE;
      goto ok;
    }
    err_puts("kc3txt_from_text: invalid action");
    return 1;
  }
 ok:
  if ((r = buf_parse_fact(&in, &fact)) <= 0) {
    err_puts("kc3txt_from_text: invalid fact");
    goto ko;
  }
  switch (action) {
  case FACT_ACTION_ADD:
    facts_add_fact(db, &fact);
    break;
  case FACT_ACTION_REMOVE:
    facts_remove_fact(db, &fact, &b);
    break;
  case FACT_ACTION_REPLACE:
    facts_replace_fact(db, &fact);
    break;
  }
  return 0;
 ko:
  fclose(fp);
  return 1;
}

int main (int argc, char **argv)
{
  const char *file_path;
  bool from_text = false;
  g_env_argv0_default = PROG;
  g_env_argv0_dir_default = PREFIX;
  if (argc < 2)
    return usage(PROG);
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  if (argc > 0 && argv[0][0] == '-') {
    if (argv[0][1] == 't')
      from_text = true;
    argc--;
    argv++;
  }
  if (argc > 0) {
    file_path = argv[0];
    if (from_text)
      return kc3txt_from_text(file_path);
    return kc3txt_from_facts(file_path);
  }
  kc3_clean(NULL);
  return 0;
}

static sw usage (const char *argv0)
{
  fprintf(stderr, "Usage: %s FILE\n", argv0);
  return 1;
}
