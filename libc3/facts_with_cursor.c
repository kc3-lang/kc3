/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include "facts_cursor.h"
#include "facts_spec.h"
#include "facts_with.h"
#include "facts_with_cursor.h"

void facts_with_cursor_clean (s_facts_with_cursor *cursor)
{
  assert(cursor);
  free(cursor->l);
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
    level = cursor->l + (cursor->facts_count - 1);
    level->fact = facts_cursor_next(&level->cursor);
    if (level->fact)
      return level->fact;
    free(level->spec);
    level->spec = NULL;
    cursor->level--;
    if (!cursor->level) {
      cursor->facts_count = 0;
      return NULL;
    }
    cursor->level--;
  }
  while (cursor->level < cursor->facts_count) {
    level = cursor->l + cursor->level;
    if (! level->spec) {
      parent_spec = cursor->level ?
        cursor->l[cursor->level - 1].spec + 4 :
        cursor->spec;
      level->spec = facts_spec_new_expand(parent_spec);
      facts_with_tags(cursor->facts, &level->cursor, level->spec[0],
                      level->spec[1], level->spec[2]);
    }
    level->fact = fact = facts_cursor_next(&level->cursor);
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
