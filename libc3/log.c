/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include "buf.h"
#include "buf_file.h"
#include "hash.h"
#include "log.h"

void log_clean (s_log *log)
{
  assert(log);
  buf_clean(&log->buf);
  hash_clean(&log->hash);
}

void log_close (s_log *log)
{
  buf_file_close(&log->buf);
}

void log_delete (s_log *log)
{
  assert(log);
  log_clean(log);
  free(log);
}

void log_init (s_log *log)
{
  buf_init_alloc(&log->buf, BUF_SIZE);
  log->count = 0;
  hash_init(&log->hash);
}

s_log * log_new ()
{
  s_log *log;
  if (! (log = malloc(sizeof(s_log)))) {
    warnx("log_new: failed to allocate memory");
    return NULL;
  }
  log_init(log);
  return log;
}

sw log_open (s_log *log, FILE *fp)
{
  assert(log);
  assert(fp);
  buf_file_open_w(&log->buf, fp);
  return 0;
}
