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

static sw kc3txt_from_facts (s_buf *in, FILE *fp)
{
  u8 action;
  char b[BUF_SIZE];
  s_fact fact;
  uw id;
  s_buf out;
  s_marshall_read mr = {0};
  sw r;
  if (! marshall_read_init_buf(&mr, in))
    return 0;
  buf_init(&out, false, sizeof(b), b);
  buf_file_open_w(&out, stdout);
  while ((r = buf_peek_1(mr.buf, "KC3MARSH")) > 0) {
    if (! marshall_read_header(&mr) ||
        ! marshall_read_chunk(&mr))
      goto done;
    while ((r = buf_peek_1(mr.buf, "_KC3UW_")) > 0) {
      if (! marshall_read_uw(&mr, false, &id) ||
          ! marshall_read_u8(&mr, false, &action) ||
          ! marshall_read_fact(&mr, false, &fact))
        goto done;
      kc3txt_buf_inspect_log(&out, id, action, &fact);
      fact_clean_all(&fact);
    }
    if (! marshall_read_chunk_reset(&mr))
      goto done;
  }
 done:
  buf_flush(&out);
  buf_file_close(&out);
  marshall_read_clean(&mr);
  fclose(fp);
  return 0;
}

static sw kc3txt_from_text (s_buf *in, FILE *fp)
{
  e_fact_action action = -1;
  bool fact_initialized = false;
  s_fact fact = {0};
  uw id;
  s_marshall m = {0};
  bool marshall_initialized = false;
  s_buf out = {0};
  char  out_buf[BUF_SIZE];
  bool out_initialized = false;
  bool out_open = false;
  sw r;
  sw result = 1;
  if (! buf_init(&out, false, sizeof(out_buf), out_buf))
    goto ko;
  out_initialized = true;
  if (! buf_file_open_w(&out, stdout))
    goto ko;
  out_open = true;
  if (! marshall_init(&m, BUF_SIZE)) {
    err_puts("kc3txt_from_text: marshall_init");
    goto ko;
  }
  marshall_initialized = true;
  while (1) {
    if ((r = buf_ignore_spaces(in)) < 0) {
      if (feof(fp) && in->rpos == in->wpos)
        break;
      err_puts("kc3txt_from_text: failed to read input");
      goto ko;
    }
    if ((r = buf_refill(in, 1)) < 0 || ferror(fp)) {
      err_puts("kc3txt_from_text: failed to read input");
      goto ko;
    }
    if (! r)
      break;
    if ((r = buf_parse_uw(in, &id)) <= 0) {
      err_puts("kc3txt_from_text: invalid id");
      goto ko;
    }
    if ((r = buf_read_1(in, g_action_str[FACT_ACTION_ADD])) < 0)
      goto ko;
    if (r) {
      action = FACT_ACTION_ADD;
      goto action_ok;
    }
    if ((r = buf_read_1(in, g_action_str[FACT_ACTION_REMOVE])) < 0)
      goto ko;
    if (r) {
      action = FACT_ACTION_REMOVE;
      goto action_ok;
    }
    if ((r = buf_read_1(in, g_action_str[FACT_ACTION_REPLACE])) < 0)
      goto ko;
    if (r) {
      action = FACT_ACTION_REPLACE;
      goto action_ok;
    }
    err_puts("kc3txt_from_text: invalid action");
    goto ko;
  action_ok:
    if ((r = buf_parse_fact(in, &fact)) <= 0) {
      err_puts("kc3txt_from_text: invalid fact");
      goto ko;
    }
    fact_initialized = true;
    if (! marshall_uw(&m, false, id)) {
      err_puts("kc3txt_from_text: marshall_uw");
      goto ko;
    }
    if (! marshall_u8(&m, false, action)) {
      err_puts("kc3txt_from_text: marshall_u8");
      goto ko;
    }
    if (! marshall_fact(&m, false, &fact)) {
      err_puts("kc3txt_from_text: marshall_fact");
      goto ko;
    }
    if ((r = marshall_to_buf(&m, &out)) <= 0) {
      err_puts("kc3txt_from_text: marshall_to_buf");
      goto ko;
    }
    if (! marshall_reset_ht(&m)) {
      err_puts("kc3txt_from_text: marshall_reset_ht");
      goto ko;
    }
    fact_clean_all(&fact);
    fact_initialized = false;
  }
  result = 0;
  goto clean;
 ko:
  result = 1;
 clean:
  if (fact_initialized)
    fact_clean_all(&fact);
  if (marshall_initialized)
    marshall_clean(&m);
  if (out_open) {
    if (buf_flush(&out) < 0)
      result = 1;
    buf_file_close(&out);
  }
  if (out_initialized)
    buf_clean(&out);
  fclose(fp);
  return result;
}

int main (int argc, char **argv)
{
  const char *file_path;
  FILE *fp = NULL;
  bool from_text = false;
  s_buf in;
  char  in_buf[BUF_SIZE];
  s_str in_path;
  g_env_argv0_default = PROG;
  g_env_argv0_dir_default = PREFIX;
  if (argc < 2)
    return usage(PROG);
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  if (argc > 0 && ! strcmp(argv[0], "-t")) {
    from_text = true;
    argc--;
    argv++;
  }
  if (argc <= 0) {
    kc3_clean(NULL);
    return usage(PROG);
  }
  if (argc > 0) {
    file_path = argv[0];
    buf_init(&in, false, sizeof(in_buf), in_buf);
    if (file_path[0] == '-' && file_path[1] == 0)
      fp = stdin;
    else {
      str_init_1(&in_path, NULL, file_path);
      if (! (fp = file_open(&in_path, "rb")))
        return 1;
    }
    buf_file_open_r(&in, fp);
    if (from_text)
      return kc3txt_from_text(&in, fp);
    return kc3txt_from_facts(&in, fp);
  }
  kc3_clean(NULL);
  return 0;
}

static sw usage (const char *argv0)
{
  fprintf(stderr, "Usage: %s [-t] FILE\n", argv0);
  return 1;
}
