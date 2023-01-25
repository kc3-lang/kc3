/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <stdlib.h>
#include "debug.h"
#include "facts_cursor.h"
#include "facts_spec.h"
#include "facts_with.h"
#include "facts_with_cursor.h"

void facts_with_cursor_clean (s_facts_with_cursor *cursor)
{
  uw i = 0;
  assert(cursor);
  while (i < cursor->facts_count) {
    free(cursor->levels[i].spec);
    i++;
  }
  free(cursor->levels);
  free(cursor->spec);
}

s_fact * facts_with_cursor_next (s_facts_with_cursor *cursor)
{
  s_fact *fact = NULL;
  s_facts_with_cursor_level *level;
  p_facts_spec parent_spec;
  assert(cursor);
  if (! cursor->facts_count)
    return NULL;
  if (cursor->level == cursor->facts_count) {
    level = &cursor->levels[cursor->facts_count - 1];
#ifdef DEBUG
    buf_write_1(&g_debug_buf, "[debug] cursor->level=");
    buf_inspect_u64(&g_debug_buf, cursor->level);
    buf_write_1(&g_debug_buf, " level->spec=");
    buf_inspect_fact_spec(&g_debug_buf, level->spec);
    buf_write_1(&g_debug_buf, " ");
    buf_inspect_fact(&g_debug_buf, level->fact);
#endif
    level->fact = facts_cursor_next(&level->cursor);
#ifdef DEBUG
    buf_write_1(&g_debug_buf, " -> ");
    buf_inspect_fact(&g_debug_buf, level->fact);
    buf_write_1(&g_debug_buf, "\n");
    buf_flush(&g_debug_buf);
#endif
    if (level->fact)
      return level->fact;
    free(level->spec);
    level->spec = NULL;
    cursor->level--;
    if (! cursor->level) {
      cursor->facts_count = 0;
      return NULL;
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
#ifdef DEBUG
    buf_write_1(&g_debug_buf, "[debug] cursor->level=");
    buf_inspect_u64(&g_debug_buf, cursor->level);
    buf_write_1(&g_debug_buf, " level->spec=");
    buf_inspect_fact_spec(&g_debug_buf, level->spec);
    buf_write_1(&g_debug_buf, " ");
    buf_inspect_fact(&g_debug_buf, level->fact);
#endif
    fact = facts_cursor_next(&level->cursor);
    level->fact = fact;
#ifdef DEBUG
    buf_write_1(&g_debug_buf, " -> ");
    buf_inspect_fact(&g_debug_buf, level->fact);
    buf_write_1(&g_debug_buf, "\n");
    buf_flush(&g_debug_buf);
#endif    
    if (fact) {
      cursor->level++;
      continue;
    }
    free(level->spec);
    level->spec = NULL;
    if (! cursor->level) {
      cursor->facts_count = 0;
      return NULL;
    }
    cursor->level--;
  }
  return fact;
}
