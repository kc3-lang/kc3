/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
  if (log->binary_path.size) {
    buf_clean(&log->binary_buf);
    str_clean(&log->binary_path);
  }
}

void log_close (s_log *log)
{
  buf_file_close(&log->buf);
  if (log->binary_path.size)
    buf_file_close(&log->binary_buf);
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
  s_log tmp = {0};
  assert(log);
  assert(fp);
  assert(path);
  tmp = *log;
  if (! buf_file_open_w(&tmp.buf, fp))
    return NULL;
  if (! str_init_copy(&tmp.path, path)) {
    buf_file_close(&tmp.buf);
    return NULL;
  }
  *log = tmp;
  return log;
}

s_log * log_open_binary (s_log *log, FILE *fp, const s_str *path)
{
  s_log tmp = {0};
  assert(log);
  assert(fp);
  assert(path);
  tmp = *log;
  if (! buf_init_alloc(&tmp.binary_buf, BUF_SIZE))
    return NULL;
  if (! buf_file_open_w(&tmp.binary_buf, fp))
    return NULL;
  if (! str_init_copy(&tmp.binary_path, path)) {
    buf_file_close(&tmp.binary_buf);
    buf_clean(&tmp.binary_buf);
    return NULL;
  }
  *log = tmp;
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
