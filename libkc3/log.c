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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_file.h"
#include "hash.h"
#include "log.h"
#include "marshall.h"
#include "str.h"

void log_clean (s_log *log)
{
  s_log_hook *hook;
  s_log_hook *next;
  assert(log);
  hook = log->hooks;
  while (hook) {
    next = hook->next;
    free(hook);
    hook = next;
  }
  log->hooks = NULL;
  buf_clean(&log->buf);
  str_clean(&log->path);
  if (log->marshall)
    marshall_delete(log->marshall);
  if (log->after_dump_marshall) {
    buf_clean(&log->after_dump_buf);
    str_clean(&log->after_dump_path);
    marshall_delete(log->after_dump_marshall);
  }
}

void log_close (s_log *log)
{
  buf_file_close(&log->buf);
  if (log->after_dump_marshall)
    buf_file_close(&log->after_dump_buf);
}

void log_delete (s_log *log)
{
  assert(log);
  log_clean(log);
  free(log);
}

s_log * log_init (s_log *log)
{
  s_log tmp = {0};
  if (! buf_init_alloc(&tmp.buf, BUF_SIZE))
    return NULL;
  *log = tmp;
  return log;
}

s_log * log_new (void)
{
  s_log *log;
  log = alloc(sizeof(s_log));
  if (! log)
    return NULL;
  if (! log_init(log)) {
    free(log);
    return NULL;
  }
  return log;
}

s_log * log_open (s_log *log, FILE *fp, const s_str *path)
{
  assert(log);
  assert(fp);
  assert(path);
  if (! buf_file_open_w(&log->buf, fp))
    return NULL;
  if (! str_init_copy(&log->path, path)) {
    buf_file_close(&log->buf);
    return NULL;
  }
  return log;
}

s_log * log_open_binary (s_log *log, FILE *fp, const s_str *path)
{
  assert(log);
  assert(fp);
  assert(path);
  if (! buf_file_open_w(&log->buf, fp))
    return NULL;
  if (! str_init_copy(&log->path, path)) {
    buf_file_close(&log->buf);
    return NULL;
  }
  if (! (log->marshall = marshall_new())) {
    str_clean(&log->path);
    buf_file_close(&log->buf);
    return NULL;
  }
  return log;
}

s_log * log_open_after_dump (s_log *log, FILE *fp, const s_str *path)
{
  assert(log);
  assert(fp);
  assert(path);
  if (! buf_init_alloc(&log->after_dump_buf, BUF_SIZE))
    return NULL;
  if (! buf_file_open_w(&log->after_dump_buf, fp)) {
    buf_clean(&log->after_dump_buf);
    return NULL;
  }
  if (! str_init_copy(&log->after_dump_path, path)) {
    buf_file_close(&log->after_dump_buf);
    buf_clean(&log->after_dump_buf);
    return NULL;
  }
  if (! (log->after_dump_marshall = marshall_new())) {
    str_clean(&log->after_dump_path);
    buf_file_close(&log->after_dump_buf);
    buf_clean(&log->after_dump_buf);
    return NULL;
  }
  return log;
}

s_str * log_path_to_binary_path (const s_str *path, s_str *dest)
{
  const s_str suffix = STR(".bin.facts");
  s_str tmp = {0};
  if (! str_init_concatenate(&tmp, path, &suffix))
    return NULL;
  *dest = tmp;
  return dest;
}

s_log_hook * log_hook_add (s_log *log, f_log_hook f, void *context)
{
  s_log_hook *hook;
  assert(log);
  assert(f);
  hook = alloc(sizeof(s_log_hook));
  if (! hook)
    return NULL;
  hook->f = f;
  hook->context = context;
  hook->next = log->hooks;
  log->hooks = hook;
  return hook;
}

bool log_hook_remove (s_log *log, s_log_hook *hook)
{
  s_log_hook **h;
  assert(log);
  assert(hook);
  h = &log->hooks;
  while (*h) {
    if (*h == hook) {
      *h = hook->next;
      free(hook);
      return true;
    }
    h = &(*h)->next;
  }
  return false;
}
