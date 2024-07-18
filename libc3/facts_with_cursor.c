/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "env.h"
#include "facts_cursor.h"
#include "facts_spec.h"
#include "facts_with.h"
#include "facts_with_cursor.h"

void facts_with_cursor_clean (s_facts_with_cursor *cursor)
{
  uw i = 0;
  assert(cursor);
  while (i < cursor->facts_count) {
    if (cursor->levels[i].spec) {
      free(cursor->levels[i].spec);
      facts_cursor_clean(&cursor->levels[i].cursor);
    }
    i++;
  }
  free(cursor->levels);
  free(cursor->spec);
  if (pthread_mutex_destroy(&cursor->mutex)) {
    err_puts("facts_with_cursor_clean: pthread_mutex_destroy");
    assert(! "facts_with_cursor_clean: pthread_mutex_destroy");
    exit(1);
  }
}

s_fact * facts_with_cursor_next (s_facts_with_cursor *cursor)
{
  s_fact *fact = NULL;
  s_facts_with_cursor_level *level;
  p_facts_spec parent_spec;
  assert(cursor);
  if (pthread_mutex_lock(&cursor->mutex)) {
    err_puts("facts_with_cursor_next: pthread_mutex_lock");
    assert(! "facts_with_cursor_next: pthread_mutex_lock");
    exit(1);
  }
  if (! cursor->facts_count)
    goto ko;
  if (cursor->level == cursor->facts_count) {
    level = &cursor->levels[cursor->facts_count - 1];
#ifdef DEBUG_FACTS
    err_write_1("[debug] cursor->level=");
    err_inspect_uw(&cursor->level);
    err_write_1(" level->spec=");
    err_inspect_facts_spec(level->spec);
    err_write_1(" ");
    err_inspect_fact(level->fact);
#endif
    level->fact = facts_cursor_next(&level->cursor);
#ifdef DEBUG_FACTS
    err_write_1(" -> ");
    err_inspect_fact(level->fact);
    err_write_1("\n");
    err_flush(err);
#endif
    if (level->fact) {
      if (pthread_mutex_unlock(&cursor->mutex)) {
        err_puts("facts_with_cursor_next: pthread_mutex_unlock");
        assert(! "facts_with_cursor_next: pthread_mutex_unlock");
        exit(1);
      }
      return level->fact;
    }
    free(level->spec);
    level->spec = NULL;
    cursor->level--;
    if (! cursor->level) {
      cursor->facts_count = 0;
      goto ko;
    }
    cursor->level--;
  }
  while (cursor->level < cursor->facts_count) {
    level = &cursor->levels[cursor->level];
    if (! level->spec) {
      if (cursor->level)
        parent_spec = cursor->levels[cursor->level - 1].spec + 4;
      else
        parent_spec = cursor->spec;
      level->spec = facts_spec_new_expand(parent_spec);
      facts_with_tags(cursor->facts, &level->cursor,
                      level->spec[0], level->spec[1],
                      level->spec[2]);
    }
#ifdef DEBUG_FACTS
    buf_write_1(&g_c3_env.err, "[debug] cursor->level=");
    buf_inspect_uw(&g_c3_env.err, &cursor->level);
    buf_write_1(&g_c3_env.err, " level->spec=");
    buf_inspect_facts_spec(&g_c3_env.err, level->spec);
    buf_write_1(&g_c3_env.err, " ");
    buf_inspect_fact(&g_c3_env.err, level->fact);
#endif
    fact = facts_cursor_next(&level->cursor);
    level->fact = fact;
#ifdef DEBUG_FACTS
    buf_write_1(&g_c3_env.err, " -> ");
    buf_inspect_fact(&g_c3_env.err, level->fact);
    buf_write_1(&g_c3_env.err, "\n");
    buf_flush(&g_c3_env.err);
#endif    
    if (fact) {
      cursor->level++;
      continue;
    }
    free(level->spec);
    level->spec = NULL;
    if (! cursor->level) {
      cursor->facts_count = 0;
      goto ko;
    }
    cursor->level--;
  }
  if (pthread_mutex_unlock(&cursor->mutex)) {
    err_puts("facts_with_cursor_next: pthread_mutex_unlock");
    assert(! "facts_with_cursor_next: pthread_mutex_unlock");
    exit(1);
  }
  return fact;
 ko:
  if (pthread_mutex_unlock(&cursor->mutex)) {
    err_puts("facts_with_cursor_next: pthread_mutex_unlock");
    assert(! "facts_with_cursor_next: pthread_mutex_unlock");
    exit(1);
  }
  return NULL;
}
