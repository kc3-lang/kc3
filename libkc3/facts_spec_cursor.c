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
#include "assert.h"
#include "facts_spec_cursor.h"

s_facts_spec_cursor *
facts_spec_cursor_init (s_facts_spec_cursor *cursor,
                        p_facts_spec spec)
{
  assert(cursor);
  assert(spec);
  cursor->spec = spec;
  cursor->subject = spec[0];
  cursor->pos = 1;
  return cursor;
}

bool facts_spec_cursor_next (s_facts_spec_cursor *cursor,
                             s_fact *fact)
{
  s_tag *predicate;
  s_tag *object;
  assert(cursor);
  assert(fact);
  do {
    if (!cursor->subject)
      return false;
    predicate = cursor->spec[cursor->pos];
    if (! predicate) {
      cursor->subject = cursor->spec[cursor->pos + 1];
      cursor->pos += 2;
    }
  } while (! predicate);
  object = cursor->spec[cursor->pos + 1];
  if (! object) {
    err_puts("facts_spec_cursor_next: NULL object");
    assert(! "facts_spec_cursor_next: NULL object");
    return false;
  }
  cursor->pos += 2;
  fact->subject = cursor->subject;
  fact->predicate = predicate;
  fact->object = object;
  return true;
}
